/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usb_audio_handler_none.h"
#include "usb_client.h"
#include "usb_defs.h"
#include "usb_dfu_handler.h"
#include "usb_setup.h"
#include "usbd.h"
#include "usbd_core.h"
#include "usbd_msos.h"
#include "usbd_string.h"
#include "usbd_strings_index.h"
#include <algorithm>
#include <cstddef>
#include <stdarg.h>
#include <stdint.h>
#include <type_traits>
#include <utl/bug.h>

namespace lac
{
	struct usb_endpoint
	{
		uint8_t		 id	   = 0xFF;
		usb_ep_state state = USB_EP_IDLE;

		const uint8_t* data;
		uint32_t	   size;
		uint32_t	   sent;
		const char*	   when;
		bool		   needs_zlp = false;

		constexpr void init(uint32_t id)
		{
			this->id		= (uint8_t)id;
			this->state		= USB_EP_IDLE;
			this->data		= 0;
			this->size		= 0;
			this->sent		= 0;
			this->when		= "idle";
			this->needs_zlp = false;
		}
	};

	template <typename usb_per_t, typename usb_dfu_t = usb_dfu_handler, typename usb_audio_t = usb_audio_handler_none>
	struct usb_state
	{
		constexpr static bool show_trace = false;

		using this_t = usb_state<usb_per_t, usb_dfu_t, usb_audio_t>;

		const static uint32_t SEND_EP_COUNT	  = 4;
		const static uint32_t MAX_PACKET_SIZE = usb_per_t::MAX_USB_SIZE;

		usb_per_t  per;
		usb_client client;

		const usbd_descriptors* desc = 0;

		usbd_strings_index strings;

		usb_dfu_t*	dfu	  = nullptr;
		usb_audio_t audio = {};

		static constexpr bool HAS_AUDIO = !std::is_same_v<usb_audio_t, usb_audio_handler_none>;

		const usbd_winusb_desc* winusb = nullptr;

		usb_endpoint send_ep[SEND_EP_COUNT];

		uint32_t active_config = 0;
		uint32_t ep0_in_len	   = 0;
		uint32_t data_ep_base  = 1;

		static void trace(const char* fmt, ...)
		{
			if constexpr(show_trace)
			{
				va_list ap;
				va_start(ap, fmt);
				bug::print_va(fmt, &ap);
			}
		}

		void init()
		{
			client.context	 = this;
			client.usb_setup = setup;
			client.usb_recv	 = recv;
			client.usb_sent	 = sent;
			client.usb_reset = reset;
			client.usb_sof	 = sof_thunk;

			reset();

			per.init(&client);
		}

		bool has_address() const { return per.has_address(); }

		bool is_suspended() const { return per.is_suspended(); }

		bool is_active() const { return active_config != 0 && has_address() && !is_suspended(); }

		void set_data_endpoint_count(uint32_t count) { client.interrupt_endpoint_count = count; }

		void set_data_endpoint_base(uint32_t ep)
		{
			data_ep_base = ep;
			per.set_data_endpoint_base(ep);
		}

		void set_iso_config(uint8_t out_ep, uint8_t in_ep, uint16_t max_packet)
		{ per.set_iso_config(out_ep, in_ep, max_packet); }

		void set_iso_out_streaming(bool on) { per.set_iso_out_streaming(on); }

		void stage_iso_in(const uint8_t* data, uint32_t size) { per.stage_iso_in(data, size); }

		const uint8_t* get_iso_out() const { return per.iso_out_buf; }

		uint32_t get_ep(uint32_t ep)
		{
			for(uint32_t i = 0; i < SEND_EP_COUNT; i++)
			{
				if(ep == send_ep[i].id)
				{
					return i;
				}
			}
			lac_assert(false);
			return SEND_EP_COUNT;
		}

		void set_string(usbd_stringid idx, const usbd_string* str) { strings.set(idx, str); }

		void set_winusb(const usbd_winusb_desc* w) { winusb = w; }

		void set_descriptors(const usbd_descriptors* d) { desc = d; }

		uint32_t get_data_count(uint32_t ep) const { return per.get_data_count(ep); }

		const uint8_t* get_data(uint32_t ep) { return per.get_buffer_rx(ep); }

		void set_ep_ready(uint32_t ep)
		{
			typename usb_per_t::irq_guard_t irq_guard;
			per.set_rx_valid(ep);
		}

		void recv(uint32_t ep, uint32_t count, const uint8_t* data)
		{
			trace("recv %d %3d %x %x %x %x\n",
				  ep,
				  count,
				  data ? data[0] : 0,
				  data ? data[1] : 0,
				  data ? data[2] : 0,
				  data ? data[3] : 0);
			if(dfu)
			{
				dfu->on_recv(this, ep, count, data);
			}
			audio.on_recv(this, ep, count, data);
		}

		void sof() { audio.on_sof(this); }

		void sent(uint32_t ep, uint32_t count)
		{
			uint32_t idx = get_ep(ep);

			if(idx >= SEND_EP_COUNT)
			{
				trace("sent on unslotted ep %d\n", ep);
				return;
			}

			auto& sep = send_ep[idx];

			if(sep.sent < sep.size)
			{
				trace("cont %d %d\n", ep, count);
				per.send_part(sep);
				return;
			}

			if(sep.needs_zlp)
			{
				sep.needs_zlp = false;
				trace("zlp %d\n", ep);
				per.send_part(sep);
				return;
			}

			trace("sent %d %d\n", ep, count);
			sep.state = USB_EP_IDLE;
			sep.when  = "idle";

			per.set_tx_nak(ep);

			if(ep == 0)
			{
				per.set_rx_valid(0);
			}
		}

		void reset()
		{
			uint32_t epc = 1 + client.interrupt_endpoint_count;

			trace("usb reset %d\n******************\n", epc);

			active_config = 0;
			active_alt	  = 0;
			audio.reset();

			for(uint32_t i = 0; i < SEND_EP_COUNT; i++)
			{
				uint32_t id = (i == 0) ? 0 : (i < epc ? data_ep_base + (i - 1) : 0xFF);
				send_ep[i].init(id);
			}
		}

		bool is_send_idle(uint32_t ep)
		{
			uint32_t idx = get_ep(ep);
			return idx < SEND_EP_COUNT && send_ep[idx].state == USB_EP_IDLE;
		}

		void send(uint32_t ep, uint32_t size, const uint8_t* data, const char* when)
		{
			trace("send %s (%s) %d %3d %08x %02x %02x %02x %02x\n",
				  when,
				  ep,
				  size,
				  data,
				  data ? data[0] : 0,
				  data ? data[1] : 0,
				  data ? data[2] : 0,
				  data ? data[3] : 0);

			uint32_t idx = get_ep(ep);

			lac_assert_fancy(idx < SEND_EP_COUNT);

			if(idx >= SEND_EP_COUNT)
			{
				return;
			}

			auto& sep = send_ep[idx];

			lac_assert_fancy(sep.state == USB_EP_IDLE);

			typename usb_per_t::irq_guard_t irq_guard;

			sep.state = USB_EP_BUSY;
			sep.data  = data;
			sep.size  = size;
			sep.sent  = 0;
			sep.when  = when;

			sep.needs_zlp = (ep == 0) && (size > 0) && (size % MAX_PACKET_SIZE == 0) && (size < ep0_in_len);

			per.send_part(sep);
		}

		template <class cb_t>
		void get_data(uint32_t ep, cb_t cb)
		{ lac_assert(false); }

		void setup(const usb_setup* setup)
		{
			const uint8_t* raw = (const uint8_t*)setup;
			trace("sup [%02x %02x %02x %02x %02x %02x %02x %02x] ",
				  raw[0],
				  raw[1],
				  raw[2],
				  raw[3],
				  raw[4],
				  raw[5],
				  raw[6],
				  raw[7]);

			ep0_in_len = setup->get_length();

			send_ep[get_ep(0)].init(0);

			if(dfu)
			{
				dfu->cancel_data();
			}
			audio.cancel_data();

			switch(setup->get_recipient())
			{
				case USB_RECIPIENT_DEVICE: setup_device(setup); break;
				case USB_RECIPIENT_INTERFACE: setup_interface(setup); break;
				case USB_RECIPIENT_ENDPOINT: setup_endpoint(setup); break;
				default:
					trace("stall ");
					per.stall_ep0();
					break;
			}

			trace("\n");

			per.set_rx_valid(0);
		}

		uint16_t status		  = 1;
		uint16_t iface_status = 0;
		uint8_t	 active_alt	  = 0;

		void setup_ack() { send(0, 0, 0, "desc ack"); }

		void setup_device(const usb_setup* setup)
		{
			trace("d ");

			if(setup->get_type() == USB_REQUEST_TYPE_VENDOR)
			{
				setup_vendor(setup);
				return;
			}

			if(setup->get_type() != USB_REQUEST_TYPE_STANDARD)
			{
				trace("stall ");
				per.stall_ep0();
				return;
			}

			switch(setup->request)
			{
				case USB_DEVICE_GET_STATUS:
					trace("stat");
					send(0, 2, (const uint8_t*)&status, "get status");
					break;
				case USB_DEVICE_SET_ADDRESS:
					trace("seta ");
					per.set_address(setup->get_value());
					setup_ack();
					break;
				case USB_DEVICE_GET_DESCRIPTOR: setup_get_descriptor(setup); break;
				case USB_DEVICE_GET_CONFIGURATION:
					trace("getc ");
					send(0, 1, (const uint8_t*)&active_config, "get config");
					break;
				case USB_DEVICE_SET_CONFIGURATION:
					trace("setc %d ", setup->get_value());
					active_config = setup->get_value();
					for(uint32_t i = 0; i < client.interrupt_endpoint_count; i++)
					{
						per.reset_data_toggle(data_ep_base + i);
						per.reset_data_toggle(0x80 | (data_ep_base + i));
					}
					active_alt = 0;
					audio.reset();
					setup_ack();
					break;
				default:
					trace("stall ");
					per.stall_ep0();
					break;
			}
		}

		void setup_vendor(const usb_setup* setup)
		{
			if(winusb && setup->is_to_host())
			{
				if(setup->request == winusb->msos_vendor_code && setup->get_index() == MSOS_DESC_INDEX)
				{
					send(0, std::min((uint32_t)winusb->msos_len, (uint32_t)setup->get_length()), winusb->msos, "msos");
					return;
				}
				if(setup->request == winusb->webusb_vendor_code && setup->get_index() == WEBUSB_GET_URL)
				{
					send(0, std::min((uint32_t)winusb->url_len, (uint32_t)setup->get_length()), winusb->url, "weburl");
					return;
				}
			}
			trace("stall ");
			per.stall_ep0();
		}

		void setup_get_descriptor(const usb_setup* setup)
		{
			if(!setup->is_to_host())
			{
				trace("stall ");
				per.stall_ep0();
				return;
			}

			trace("getd ");

			switch(setup->get_desc_type())
			{
				case USBD_DESC_DEVICE: {
					trace("dev    ");
					send(0,
						 std::min((uint32_t)desc->device->head.bLength, (uint32_t)setup->get_length()),
						 (const uint8_t*)desc->device,
						 "desc dev");
					break;
				}
				case USBD_DESC_CONFIG: {
					trace("conf   ");
					uint32_t idx = setup->get_desc_index();
					if(idx >= desc->config_count)
					{
						trace("stall ");
						per.stall_ep0();
						break;
					}

					auto size = std::min(desc->config[idx]->wTotalLength, setup->get_length());

					send(0, size, (const uint8_t*)desc->config[idx], "desc conf");
					break;
				}
				case USBD_DESC_STRING: {
					usbd_stringid id = (usbd_stringid)setup->get_desc_index();
					trace("str  %d ", id);
					auto* str = strings.get(id);
					if(str == nullptr)
					{
						trace("stall ");
						per.stall_ep0();
						break;
					}
					auto size = std::min((uint32_t)setup->get_length(), str->get_size());
					send(0, size, str->get_data(), "desc string");
					break;
				}
				case USBD_DESC_QUALIFIER: {
					if(desc->qualifier)
					{
						send(0,
							 std::min((uint32_t)desc->qualifier->head.bLength, (uint32_t)setup->get_length()),
							 (const uint8_t*)desc->qualifier,
							 "desc qual");
					}
					else
					{
						trace("stall ");
						per.stall_ep0();
					}
					break;
				}
				case USBD_DESC_BOS: {
					if(winusb)
					{
						send(0, std::min((uint32_t)winusb->bos_len, (uint32_t)setup->get_length()), winusb->bos, "desc bos");
					}
					else
					{
						trace("stall ");
						per.stall_ep0();
					}
					break;
				}
				case USBD_DESC_OTHER_SPEED:
				case USBD_DESC_POWER:
				default: {
					trace("stall %d ", setup->get_desc_type());
					per.stall_ep0();
					break;
				}
			}
		}

		void reset_interface_toggles(uint8_t iface)
		{
			if(!desc || !desc->config[0])
			{
				return;
			}

			const uint8_t* config	 = (const uint8_t*)desc->config[0];
			uint32_t	   total_len = config[2] | (config[3] << 8);

			bool   match_iface = false;
			size_t off		   = 0;

			while(off + 2 <= total_len)
			{
				const size_t len = config[off];
				if(len == 0 || off + len > total_len)
				{
					break;
				}

				if(config[off + 1] == USBD_DESC_INTERFACE)
				{
					match_iface = (config[off + 2] == iface);
				}
				else if(config[off + 1] == USBD_DESC_ENDPOINT && len >= 7)
				{
					if(match_iface)
					{
						per.reset_data_toggle(config[off + 2]);
					}
				}

				off += len;
			}
		}

		void setup_interface(const usb_setup* setup)
		{
			trace("iface ");
			if(setup->get_type() == USB_REQUEST_TYPE_CLASS)
			{
				if(dfu && setup->get_index() == dfu->dfu_iface)
				{
					dfu->on_setup(this, setup);
				}
				else if(audio.owns_iface((uint8_t)setup->get_index()))
				{
					audio.on_setup(this, setup);
				}
				else
				{
					trace("stall ");
					per.stall_ep0();
				}
				return;
			}
			if(setup->get_type() != USB_REQUEST_TYPE_STANDARD)
			{
				trace("stall ");
				per.stall_ep0();
				return;
			}
			if(audio.owns_iface((uint8_t)setup->get_index()))
			{
				audio.on_setup(this, setup);
				return;
			}
			switch(setup->request)
			{
				case USB_INTERFACE_GET_STATUS:
					trace("stat ");
					send(0, 2, (const uint8_t*)&iface_status, "iface status");
					break;
				case USB_INTERFACE_GET_INTERFACE:
					trace("geti ");
					send(0, 1, &active_alt, "iface get");
					break;
				case USB_INTERFACE_SET_INTERFACE:
					trace("seti %d ", setup->get_value());
					active_alt = (uint8_t)setup->get_value();
					reset_interface_toggles((uint8_t)setup->get_index());
					setup_ack();
					break;
				default:
					trace("stall ");
					per.stall_ep0();
					break;
			}
		}

		void setup_endpoint(const usb_setup* setup)
		{
			trace("ep ");
			if constexpr(HAS_AUDIO)
			{
				if(setup->get_type() == USB_REQUEST_TYPE_CLASS)
				{
					audio.on_ep_setup(this, setup);
					return;
				}
			}
			if(setup->get_type() != USB_REQUEST_TYPE_STANDARD)
			{
				trace("stall ");
				per.stall_ep0();
				return;
			}
			switch(setup->request)
			{
				case USB_ENDPOINT_GET_STATUS:
					trace("stat ");
					send(0, 2, (const uint8_t*)&iface_status, "ep status");
					break;
				case USB_ENDPOINT_CLEAR_FEATURE:
					trace("clrf ");
					if(setup->get_value() == 0)
					{
						per.reset_data_toggle(setup->get_index() & 0xFF);
					}
					setup_ack();
					break;
				default:
					trace("stall ");
					per.stall_ep0();
					break;
			}
		}

		void irq() { per.irq(); }

		static void setup(void* ctx, uint32_t count, const uint8_t* data) { ((this_t*)ctx)->setup((const usb_setup*)data); }

		static void recv(void* ctx, uint32_t ep, uint32_t count, const uint8_t* data) { ((this_t*)ctx)->recv(ep, count, data); }

		static void sent(void* ctx, uint32_t ep, uint32_t count) { ((this_t*)ctx)->sent(ep, count); }

		static void reset(void* ctx) { ((this_t*)ctx)->reset(); }

		static void sof_thunk(void* ctx) { ((this_t*)ctx)->sof(); }
	};
} // namespace lac