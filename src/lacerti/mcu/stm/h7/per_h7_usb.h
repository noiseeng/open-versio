/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <mcu/arm/mcu_arm_v7m.h>
#include <mcu/arm/scope_disable_irq.h>
#include <usb/usb_client.h>
#include <usb/usb_state.h>
#include <utl/bug.h>

#include <cstdint>

#include "reg_h7_usb.h"
#include <usb/usb_defs.h>

namespace lac
{
	template <typename gpio_dm, typename gpio_dp>
	struct per_h7_usb_pins
	{
		using pin_dm_t = gpio_dm;
		using pin_dp_t = gpio_dp;

		static void init()
		{
			pin_dm_t::init_af_pu();
			pin_dp_t::init_af_pu();
			pin_dm_t::set_highest_speed();
			pin_dp_t::set_highest_speed();
		}
	};

	template <typename rcc_t, typename timer_t, uint32_t base_addr, typename pins_t>
	struct per_h7_usb
	{
		using this_t  = per_h7_usb<rcc_t, timer_t, base_addr, pins_t>;
		using glob_t  = reg_h7_otg_global;
		using dev_t	  = reg_h7_otg_device;
		using inep_t  = reg_h7_otg_inep;
		using outep_t = reg_h7_otg_outep;

		static constexpr uint32_t MAX_USB_SIZE = 64;

		using irq_guard_t = scope_disable_irq<mcu_arm_v7m>;

		static constexpr uint32_t CTRL_OUT_MAX	 = 1024;
		static constexpr uint32_t RX_FIFO_WORDS	 = 128;
		static constexpr uint32_t TX0_FIFO_WORDS = 64;
		static constexpr uint32_t TX1_FIFO_WORDS = 16;
		static constexpr uint32_t TX2_FIFO_WORDS = 80;
		static constexpr uint32_t TX3_FIFO_WORDS = 32;
		static constexpr uint32_t TXF_FLUSH_ALL	 = 0x10;
		static constexpr uint32_t DATA_EP_COUNT	 = 3;
		static constexpr uint32_t DATA_OUT_MAX	 = MAX_USB_SIZE;

		static constexpr uint32_t EPTYP_ISO			 = 1;
		static constexpr uint32_t EPTYP_BULK		 = 2;
		static constexpr uint32_t ISO_MAX_PACKET	 = 300;
		static constexpr uint32_t ISO_OUT_MISS_LIMIT = 3;

		static reg_h7_usb* usb() { return (reg_h7_usb*)base_addr; }
		static glob_t*	   g() { return usb()->global(); }
		static dev_t*	   d() { return usb()->device(); }

		usb_client* client			= nullptr;
		uint32_t	last_in_size[4] = {};

		alignas(4) uint8_t setup_buf[8]			 = {};
		alignas(4) uint8_t out_buf[CTRL_OUT_MAX] = {};
		uint32_t out_count						 = 0;
		uint32_t ctrl_out_expected				 = 0;
		bool	 ctrl_out_short					 = false;

		uint8_t	 data_buf[DATA_EP_COUNT][DATA_OUT_MAX] = {};
		uint32_t data_count[DATA_EP_COUNT]			   = {};

		struct iso_config_t
		{
			uint8_t	 out_ep		   = 0;
			uint8_t	 in_ep		   = 0;
			uint16_t max_packet	   = 0;
			bool	 active		   = false;
			bool	 out_streaming = false;
		} iso;

		uint8_t	 iso_out_buf[ISO_MAX_PACKET] = {};
		uint32_t iso_out_count				 = 0;
		uint32_t iso_out_miss				 = 0;
		uint8_t	 iso_out_recov				 = 0;
		uint8_t	 iso_in_buf[ISO_MAX_PACKET]	 = {};
		uint32_t iso_in_size				 = 0;

		volatile uint32_t dbg_sof			 = 0;
		volatile uint32_t dbg_iso_arm		 = 0;
		volatile uint32_t dbg_iso_skip_busy	 = 0;
		volatile uint32_t dbg_iso_xfrc		 = 0;
		volatile uint32_t dbg_iso_incomplete = 0;

		inline static this_t* instance = nullptr;

		static void irq_thunk()
		{
			if(instance)
			{
				instance->irq();
			}
		}

		void init(usb_client* cl)
		{
			pins_t::init();
			client							   = cl;
			client->isochronous_endpoint_count = client_iso_count;
			instance						   = this;

			auto* clk = rcc_t::reg();
			clk->enable_usb2_otg_fs();
			clk->reset_usb2_otg_fs();

			auto* gg = g();

			glob_t::GUSBCFG_PHYSEL::set(gg->GUSBCFG);
			glob_t::GCCFG_PWRDWN::set(gg->GCCFG);
			core_reset();

			glob_t::GCCFG_PWRDWN::set(gg->GCCFG);
			glob_t::GCCFG_VBDEN::clr(gg->GCCFG);
			glob_t::GOTGCTL_BVALOEN::set(gg->GOTGCTL);
			glob_t::GOTGCTL_BVALOVAL::set(gg->GOTGCTL);

			glob_t::GUSBCFG_FDMOD::set(gg->GUSBCFG);
			glob_t::GUSBCFG_FHMOD::clr(gg->GUSBCFG);
			glob_t::GUSBCFG_TRDT::set(gg->GUSBCFG, 6);
			timer_t::delay_ms(50);

			device_init();
		}

		void core_reset()
		{
			auto* gg = g();
			while(!glob_t::GRSTCTL_AHBIDL::get(gg->GRSTCTL)) {}
			glob_t::GRSTCTL_CSRST::set(gg->GRSTCTL);
			while(glob_t::GRSTCTL_CSRST::get(gg->GRSTCTL)) {}
			while(!glob_t::GRSTCTL_AHBIDL::get(gg->GRSTCTL)) {}
		}

		void flush_tx(uint32_t num)
		{
			auto* gg = g();
			while(!glob_t::GRSTCTL_AHBIDL::get(gg->GRSTCTL)) {}
			gg->GRSTCTL = glob_t::GRSTCTL_TXFFLSH::BIT | glob_t::GRSTCTL_TXFNUM::val(num);
			while(glob_t::GRSTCTL_TXFFLSH::get(gg->GRSTCTL)) {}
		}

		void flush_rx()
		{
			auto* gg = g();
			while(!glob_t::GRSTCTL_AHBIDL::get(gg->GRSTCTL)) {}
			gg->GRSTCTL = glob_t::GRSTCTL_RXFFLSH::BIT;
			while(glob_t::GRSTCTL_RXFFLSH::get(gg->GRSTCTL)) {}
		}

		void device_init()
		{
			auto* gg = g();
			auto* dd = d();

			*usb()->pcgcctl() = 0;

			dev_t::DCFG_DSPD::set(dd->DCFG, dev_t::DSPD_FULL_SPEED);
			dev_t::DCFG_NZLSOHSK::set(dd->DCFG);

			dev_t::DCTL_SDIS::set(dd->DCTL);
			timer_t::delay_ms(50);

			gg->GRXFSIZ		   = RX_FIFO_WORDS;
			gg->DIEPTXF0	   = (TX0_FIFO_WORDS << 16) | RX_FIFO_WORDS;
			*usb()->dieptxf(1) = (TX1_FIFO_WORDS << 16) | (RX_FIFO_WORDS + TX0_FIFO_WORDS);
			*usb()->dieptxf(2) = (TX2_FIFO_WORDS << 16) | (RX_FIFO_WORDS + TX0_FIFO_WORDS + TX1_FIFO_WORDS);
			*usb()->dieptxf(3) = (TX3_FIFO_WORDS << 16) | (RX_FIFO_WORDS + TX0_FIFO_WORDS + TX1_FIFO_WORDS + TX2_FIFO_WORDS);

			flush_tx(TXF_FLUSH_ALL);
			flush_rx();

			for(uint32_t ep = 0; ep < 4; ep++)
			{
				usb()->inep(ep)->CTL  = 0;
				usb()->inep(ep)->INT  = 0xFF;
				usb()->outep(ep)->CTL = 0;
				usb()->outep(ep)->INT = 0xFF;
			}

			dd->DAINTMSK = (1u << 0) | (1u << 16);
			dd->DIEPMSK	 = dev_t::DIEPMSK_XFRCM::BIT | dev_t::DIEPMSK_TOM::BIT;
			dd->DOEPMSK	 = dev_t::DOEPMSK_XFRCM::BIT | dev_t::DOEPMSK_STUPM::BIT;

			gg->GINTSTS = 0xFFFFFFFF;
			gg->GINTMSK = glob_t::GINT_USBRST::BIT | glob_t::GINT_ENUMDNE::BIT | glob_t::GINT_RXFLVL::BIT
						  | glob_t::GINT_IEPINT::BIT | glob_t::GINT_OEPINT::BIT | glob_t::GINT_USBSUSP::BIT;

			glob_t::GAHBCFG_GINTMSK::set(gg->GAHBCFG);
			dev_t::DCTL_SDIS::clr(dd->DCTL);
		}

		void on_usb_reset()
		{
			auto* dd = d();
			dev_t::DCFG_DAD::set(dd->DCFG, 0);
			out_count		  = 0;
			ctrl_out_expected = 0;
			ctrl_out_short	  = false;
			for(uint32_t i = 0; i < DATA_EP_COUNT; i++)
			{
				data_count[i] = 0;
			}
			iso_out_count = 0;
			iso_in_size	  = 0;
			flush_tx(TXF_FLUSH_ALL);
			client->reset();
		}

		void ep0_configure()
		{
			auto* in = usb()->inep(0);
			inep_t::CTL_MPSIZ::set(in->CTL, inep_t::MPSIZ0_64);
			arm_ep0_out();

			for(uint32_t i = 0; i < client->interrupt_endpoint_count; i++)
			{
				setup_interrupt_endpoint(data_ep_base + i);
			}

			if(iso.active)
			{
				if(iso.out_ep)
				{
					setup_isochronous_endpoint(iso.out_ep, false, iso.max_packet);
				}
				if(iso.in_ep)
				{
					setup_isochronous_endpoint(iso.in_ep, true, iso.max_packet);
				}
				g()->GINTMSK |= glob_t::GINT_SOF::BIT | glob_t::GINT_IISOIXFR::BIT;
			}
		}

		void set_iso_config(uint8_t out_ep, uint8_t in_ep, uint16_t max_packet)
		{
			iso.out_ep		 = out_ep;
			iso.in_ep		 = in_ep;
			iso.max_packet	 = max_packet;
			iso.active		 = true;
			client_iso_count = (out_ep ? 1u : 0u) + (in_ep ? 1u : 0u);
		}

		void set_iso_out_streaming(bool on) { iso.out_streaming = on; }

		uint32_t client_iso_count = 0;

		uint32_t data_ep_base = 1;
		void	 set_data_endpoint_base(uint32_t ep) { data_ep_base = ep; }

		void setup_isochronous_endpoint(uint32_t ep, bool is_in, uint32_t max_packet)
		{
			auto* dd = d();

			if(is_in)
			{
				auto* in = usb()->inep(ep);
				in->CTL	 = inep_t::CTL_USBAEP::BIT | inep_t::CTL_SNAK::BIT | inep_t::CTL_EPTYP::val(EPTYP_ISO)
						   | inep_t::CTL_TXFNUM::val(ep) | max_packet;
				dd->DAINTMSK |= (1u << ep);
			}
			else
			{
				auto* o = usb()->outep(ep);
				o->CTL	= outep_t::CTL_USBAEP::BIT | outep_t::CTL_EPTYP::val(EPTYP_ISO) | max_packet;
				arm_iso_out();
				dd->DAINTMSK |= (1u << (ep + 16));
			}
		}

		bool frame_even() const { return (dev_t::DSTS_FNSOF::get(d()->DSTS) & 1u) == 0; }

		void arm_iso_in()
		{
			if(!iso.active || !iso.in_ep)
			{
				return;
			}
			auto* in = usb()->inep(iso.in_ep);
			if(inep_t::CTL_EPENA::get(in->CTL))
			{
				dbg_iso_skip_busy = dbg_iso_skip_busy + 1;
				return;
			}
			dbg_iso_arm = dbg_iso_arm + 1;
			if(frame_even())
			{
				inep_t::CTL_SODDFRM::set(in->CTL);
			}
			else
			{
				inep_t::CTL_SD0PID::set(in->CTL);
			}
			in->TSIZ = inep_t::TSIZ_PKTCNT::val(1) | inep_t::TSIZ_XFRSIZ::val(iso_in_size);
			in->CTL |= inep_t::CTL_EPENA::BIT | inep_t::CTL_CNAK::BIT;
			write_fifo(iso.in_ep, iso_in_buf, iso_in_size);
		}

		void do_arm_iso_out(outep_t* o)
		{
			if(frame_even())
			{
				outep_t::CTL_SD0PID::set(o->CTL);
			}
			else
			{
				outep_t::CTL_SODDFRM::set(o->CTL);
			}
			o->TSIZ = outep_t::TSIZ_PKTCNT::val(1) | outep_t::TSIZ_XFRSIZ::val(iso.max_packet);
			o->CTL |= outep_t::CTL_EPENA::BIT | outep_t::CTL_CNAK::BIT;
		}

		void step_iso_out_recovery()
		{
			if(iso_out_recov == 0)
			{
				return;
			}
			auto* o = usb()->outep(iso.out_ep);
			if(iso_out_recov == 1)
			{
				if(glob_t::GINT_GONAKEFF::get(g()->GINTSTS))
				{
					o->CTL |= outep_t::CTL_EPDIS::BIT | outep_t::CTL_SNAK::BIT;
					iso_out_recov = 2;
				}
			}
			else if(outep_t::INT_EPDISD::get(o->INT))
			{
				o->INT = outep_t::INT_EPDISD::BIT;
				d()->DCTL |= dev_t::DCTL_CGONAK::BIT;
				iso_out_recov = 0;
				do_arm_iso_out(o);
			}
		}

		void arm_iso_out()
		{
			if(!iso.active || !iso.out_ep || !iso.out_streaming)
			{
				if(iso_out_recov != 0)
				{
					d()->DCTL |= dev_t::DCTL_CGONAK::BIT;
					iso_out_recov = 0;
				}
				return;
			}
			if(iso_out_recov != 0)
			{
				return;
			}
			auto* o = usb()->outep(iso.out_ep);
			if(outep_t::CTL_EPENA::get(o->CTL))
			{
				if(++iso_out_miss < ISO_OUT_MISS_LIMIT)
				{
					return;
				}
				iso_out_miss = 0;
				d()->DCTL |= dev_t::DCTL_SGONAK::BIT;
				iso_out_recov = 1;
				return;
			}
			iso_out_miss = 0;
			do_arm_iso_out(o);
		}

		void handle_iso_in_incomplete()
		{
			if(!iso.active || !iso.in_ep)
			{
				return;
			}
			auto* in = usb()->inep(iso.in_ep);
			if(!inep_t::CTL_EPENA::get(in->CTL))
			{
				return;
			}
			in->CTL |= inep_t::CTL_EPDIS::BIT | inep_t::CTL_SNAK::BIT;
			for(uint32_t w = 0; w < 100000 && !inep_t::INT_EPDISD::get(in->INT); ++w) {}
			in->INT = inep_t::INT_EPDISD::BIT;
			flush_tx(iso.in_ep);
		}

		void stage_iso_in(const uint8_t* data, uint32_t size)
		{
			if(size > ISO_MAX_PACKET)
			{
				size = ISO_MAX_PACKET;
			}
			for(uint32_t i = 0; i < size; i++)
			{
				iso_in_buf[i] = data[i];
			}
			iso_in_size = size;
		}

		void setup_interrupt_endpoint(uint32_t ep)
		{
			lac_assert(ep > 0 && ep <= DATA_EP_COUNT);

			auto* dd = d();

			auto* in = usb()->inep(ep);
			in->CTL	 = inep_t::CTL_USBAEP::BIT | inep_t::CTL_SNAK::BIT | inep_t::CTL_EPTYP::val(EPTYP_BULK)
					   | inep_t::CTL_TXFNUM::val(ep) | MAX_USB_SIZE;

			auto* o = usb()->outep(ep);
			o->CTL	= outep_t::CTL_USBAEP::BIT | outep_t::CTL_SNAK::BIT | outep_t::CTL_EPTYP::val(EPTYP_BULK) | MAX_USB_SIZE;
			o->TSIZ = outep_t::TSIZ_PKTCNT::val(1) | outep_t::TSIZ_XFRSIZ::val(MAX_USB_SIZE);
			o->CTL |= outep_t::CTL_EPENA::BIT | outep_t::CTL_CNAK::BIT;

			dd->DAINTMSK |= (1u << ep) | (1u << (ep + 16));
		}

		void arm_ep0_out()
		{
			auto* o = usb()->outep(0);
			o->TSIZ = outep_t::TSIZ_STUPCNT::val(3) | outep_t::TSIZ_PKTCNT::val(1) | outep_t::TSIZ_XFRSIZ::val(MAX_USB_SIZE);
			o->CTL |= outep_t::CTL_EPENA::BIT | outep_t::CTL_CNAK::BIT;
		}

		void read_fifo(uint8_t* dst, uint32_t bytes)
		{
			volatile uint32_t* f	 = usb()->fifo(0);
			uint32_t		   words = (bytes + 3) / 4;
			for(uint32_t i = 0; i < words; i++)
			{
				uint32_t w = *f;
				for(uint32_t b = 0; b < 4; b++)
				{
					uint32_t idx = i * 4 + b;
					if(idx < bytes)
					{
						dst[idx] = (uint8_t)(w >> (8 * b));
					}
				}
			}
		}

		void read_fifo_append(uint32_t bytes)
		{
			volatile uint32_t* f	 = usb()->fifo(0);
			uint32_t		   words = (bytes + 3) / 4;
			for(uint32_t i = 0; i < words; i++)
			{
				uint32_t w = *f;
				for(uint32_t b = 0; b < 4; b++)
				{
					uint32_t idx = i * 4 + b;
					if(idx < bytes && out_count < CTRL_OUT_MAX)
					{
						out_buf[out_count++] = (uint8_t)(w >> (8 * b));
					}
				}
			}
			ctrl_out_short = (bytes < MAX_USB_SIZE);
		}

		void read_fifo_ep(uint32_t ep, uint32_t bytes)
		{
			lac_assert(ep > 0 && ep <= DATA_EP_COUNT);
			volatile uint32_t* f	 = usb()->fifo(0);
			uint32_t		   words = (bytes + 3) / 4;
			uint32_t		   idx	 = ep - 1;
			uint32_t		   cnt	 = 0;
			for(uint32_t i = 0; i < words; i++)
			{
				uint32_t w = *f;
				for(uint32_t b = 0; b < 4; b++)
				{
					uint32_t off = i * 4 + b;
					if(off < bytes && cnt < DATA_OUT_MAX)
					{
						data_buf[idx][cnt++] = (uint8_t)(w >> (8 * b));
					}
				}
			}
			data_count[idx] = cnt;
		}

		void drain_fifo(uint32_t bytes)
		{
			volatile uint32_t* f	 = usb()->fifo(0);
			uint32_t		   words = (bytes + 3) / 4;
			for(uint32_t i = 0; i < words; i++)
			{
				(void)*f;
			}
		}

		void write_fifo(uint32_t ep, const uint8_t* src, uint32_t bytes)
		{
			volatile uint32_t* f	 = usb()->fifo(ep);
			uint32_t		   words = (bytes + 3) / 4;
			for(uint32_t i = 0; i < words; i++)
			{
				uint32_t w = 0;
				for(uint32_t b = 0; b < 4; b++)
				{
					uint32_t idx = i * 4 + b;
					if(idx < bytes)
					{
						w |= (uint32_t)src[idx] << (8 * b);
					}
				}
				*f = w;
			}
		}

		bool has_address() const { return dev_t::DCFG_DAD::get(d()->DCFG) != 0; }

		bool is_suspended() const { return dev_t::DSTS_SUSPSTS::get(d()->DSTS) != 0; }

		const uint8_t* get_buffer_rx(uint32_t ep) const
		{
			if(ep == 0)
			{
				return out_buf;
			}
			lac_assert(ep <= DATA_EP_COUNT);
			return data_buf[ep - 1];
		}

		uint32_t get_data_count(uint32_t ep) const
		{
			if(ep == 0)
			{
				return out_count;
			}
			lac_assert(ep <= DATA_EP_COUNT);
			return data_count[ep - 1];
		}

		void set_rx_valid(uint32_t ep)
		{
			if(ep == 0)
			{
				arm_ep0_out();
				return;
			}
			lac_assert(ep <= DATA_EP_COUNT);
			data_count[ep - 1] = 0;
			auto* o			   = usb()->outep(ep);
			o->TSIZ			   = outep_t::TSIZ_PKTCNT::val(1) | outep_t::TSIZ_XFRSIZ::val(MAX_USB_SIZE);
			o->CTL |= outep_t::CTL_EPENA::BIT | outep_t::CTL_CNAK::BIT;
		}

		void set_tx_nak(uint32_t ep) { usb()->inep(ep)->CTL |= inep_t::CTL_SNAK::BIT; }

		void reset_data_toggle(uint32_t ep_addr)
		{
			uint32_t ep = ep_addr & 0x0F;
			if(ep == 0 || ep > DATA_EP_COUNT)
			{
				return;
			}
			if(ep_addr & 0x80)
			{
				if(iso.active && ep == iso.in_ep)
				{
					return;
				}
				inep_t::CTL_SD0PID::set(usb()->inep(ep)->CTL);
			}
			else
			{
				if(iso.active && ep == iso.out_ep)
				{
					return;
				}
				outep_t::CTL_SD0PID::set(usb()->outep(ep)->CTL);
			}
		}

		void stall_ep0()
		{
			arm_ep0_out();
			usb()->inep(0)->CTL |= inep_t::CTL_STALL::BIT;
			usb()->outep(0)->CTL |= outep_t::CTL_STALL::BIT;
		}

		void set_address(uint32_t addr) { dev_t::DCFG_DAD::set(d()->DCFG, addr); }

		void send_part(usb_endpoint& ep)
		{
			lac_assert(ep.state == USB_EP_BUSY);

			uint32_t remain = ep.sent < ep.size ? ep.size - ep.sent : 0;
			uint32_t size	= remain > MAX_USB_SIZE ? MAX_USB_SIZE : remain;

			auto* in = usb()->inep(ep.id);
			in->TSIZ = inep_t::TSIZ_PKTCNT::val(1) | inep_t::TSIZ_XFRSIZ::val(size);
			in->CTL |= inep_t::CTL_EPENA::BIT | inep_t::CTL_CNAK::BIT;

			write_fifo(ep.id, ep.data + ep.sent, size);

			last_in_size[ep.id] = size;
			ep.sent += size;
		}

		void irq()
		{
			auto*	 gg	 = g();
			uint32_t sts = gg->GINTSTS & gg->GINTMSK;

			if(iso.active && iso.out_ep)
			{
				step_iso_out_recovery();
			}

			if(glob_t::GINT_USBRST::get(sts))
			{
				on_usb_reset();
				gg->GINTSTS = glob_t::GINT_USBRST::BIT;
			}

			if(glob_t::GINT_ENUMDNE::get(sts))
			{
				ep0_configure();
				gg->GINTSTS = glob_t::GINT_ENUMDNE::BIT;
			}

			while(glob_t::GINT_RXFLVL::get(gg->GINTSTS))
			{
				handle_rxflvl();
			}

			if(glob_t::GINT_OEPINT::get(sts))
			{
				handle_oepint();
			}

			if(glob_t::GINT_IEPINT::get(sts))
			{
				handle_iepint();
			}

			if(glob_t::GINT_USBSUSP::get(sts))
			{
				gg->GINTSTS = glob_t::GINT_USBSUSP::BIT;
			}

			if(glob_t::GINT_IISOIXFR::get(sts))
			{
				dbg_iso_incomplete = dbg_iso_incomplete + 1;
				handle_iso_in_incomplete();
				gg->GINTSTS = glob_t::GINT_IISOIXFR::BIT;
			}

			if(glob_t::GINT_SOF::get(sts))
			{
				gg->GINTSTS = glob_t::GINT_SOF::BIT;
				dbg_sof		= dbg_sof + 1;
				client->sof();
				arm_iso_in();
				arm_iso_out();
			}
		}

		void handle_rxflvl()
		{
			auto*	 gg		= g();
			uint32_t stsp	= gg->GRXSTSP;
			uint32_t epnum	= glob_t::GRXSTS_EPNUM::get(stsp);
			uint32_t pktsts = glob_t::GRXSTS_PKTSTS::get(stsp);
			uint32_t bcnt	= glob_t::GRXSTS_BCNT::get(stsp);

			if(epnum == 0)
			{
				switch(pktsts)
				{
					case glob_t::PKTSTS_SETUP_DATA: read_fifo(setup_buf, bcnt); break;
					case glob_t::PKTSTS_OUT_DATA: read_fifo_append(bcnt); break;
					default: drain_fifo(bcnt); break;
				}
			}
			else if(iso.active && epnum == iso.out_ep)
			{
				switch(pktsts)
				{
					case glob_t::PKTSTS_OUT_DATA: read_fifo_iso(bcnt); break;
					default: drain_fifo(bcnt); break;
				}
			}
			else
			{
				switch(pktsts)
				{
					case glob_t::PKTSTS_OUT_DATA: read_fifo_ep(epnum, bcnt); break;
					default: drain_fifo(bcnt); break;
				}
			}
		}

		void read_fifo_iso(uint32_t bytes)
		{
			volatile uint32_t* f	 = usb()->fifo(0);
			uint32_t		   words = (bytes + 3) / 4;
			uint32_t		   cnt	 = 0;
			for(uint32_t i = 0; i < words; i++)
			{
				uint32_t w = *f;
				for(uint32_t b = 0; b < 4; b++)
				{
					uint32_t off = i * 4 + b;
					if(off < bytes && cnt < ISO_MAX_PACKET)
					{
						iso_out_buf[cnt++] = (uint8_t)(w >> (8 * b));
					}
				}
			}
			iso_out_count = cnt;
		}

		void handle_oepint()
		{
			uint32_t daint = d()->DAINT;

			if(daint & (1u << 16))
			{
				auto*	 o = usb()->outep(0);
				uint32_t i = o->INT;

				if(outep_t::INT_STUP::get(i))
				{
					o->INT				= outep_t::INT_STUP::BIT;
					const uint8_t  bm	= setup_buf[0];
					const uint16_t wlen = (uint16_t)(setup_buf[6] | (setup_buf[7] << 8));
					out_count			= 0;
					ctrl_out_short		= false;
					if((bm & 0x80u) == 0 && wlen > CTRL_OUT_MAX)
					{
						ctrl_out_expected = 0;
						stall_ep0();
					}
					else
					{
						ctrl_out_expected = ((bm & 0x80u) == 0) ? wlen : 0;
						client->setup(8, setup_buf);
					}
				}

				if(outep_t::INT_XFRC::get(i))
				{
					o->INT = outep_t::INT_XFRC::BIT;
					if(out_count < ctrl_out_expected && !ctrl_out_short)
					{
						arm_ep0_out();
					}
					else
					{
						if(out_count)
						{
							client->recv(0, out_count, out_buf);
							out_count = 0;
						}
						ctrl_out_expected = 0;
						ctrl_out_short	  = false;
					}
				}
			}

			if(iso.active && iso.out_ep && (daint & (1u << (iso.out_ep + 16))))
			{
				auto*	 o = usb()->outep(iso.out_ep);
				uint32_t i = o->INT;
				if(outep_t::INT_XFRC::get(i))
				{
					o->INT		 = outep_t::INT_XFRC::BIT;
					iso_out_miss = 0;
					if(iso_out_count)
					{
						client->recv(iso.out_ep, iso_out_count, iso_out_buf);
						iso_out_count = 0;
					}
				}
			}

			for(uint32_t ep = 1; ep <= DATA_EP_COUNT; ep++)
			{
				if(iso.active && ep == iso.out_ep)
				{
					continue;
				}
				if(!(daint & (1u << (ep + 16))))
				{
					continue;
				}

				auto*	 o = usb()->outep(ep);
				uint32_t i = o->INT;

				if(outep_t::INT_XFRC::get(i))
				{
					o->INT = outep_t::INT_XFRC::BIT;
				}
			}
		}

		void handle_iepint()
		{
			uint32_t daint = d()->DAINT;

			if(iso.active && iso.in_ep && (daint & (1u << iso.in_ep)))
			{
				auto* in = usb()->inep(iso.in_ep);
				if(inep_t::INT_XFRC::get(in->INT))
				{
					dbg_iso_xfrc = dbg_iso_xfrc + 1;
					in->INT		 = inep_t::INT_XFRC::BIT | inep_t::INT_TOC::BIT;
					arm_iso_in();
				}
				else
				{
					in->INT = inep_t::INT_XFRC::BIT | inep_t::INT_TOC::BIT;
				}
			}

			for(uint32_t ep = 0; ep <= DATA_EP_COUNT; ep++)
			{
				if(iso.active && ep == iso.in_ep)
				{
					continue;
				}
				if(!(daint & (1u << ep)))
				{
					continue;
				}

				auto*	 in = usb()->inep(ep);
				uint32_t i	= in->INT;

				if(inep_t::INT_XFRC::get(i))
				{
					in->INT = inep_t::INT_XFRC::BIT;
					client->sent(ep, last_in_size[ep]);
				}

				if(inep_t::INT_TOC::get(i))
				{
					in->INT = inep_t::INT_TOC::BIT;
				}
			}
		}
	};
} // namespace lac
