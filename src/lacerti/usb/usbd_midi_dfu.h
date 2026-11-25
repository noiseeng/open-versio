/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usbd.h"
#include "usbd_audio.h"
#include "usbd_core.h"
#include "usbd_dfu.h"
#include "usbd_midi.h"
#include "usbd_string_const.h"
#include <array>
#include <bug/compiler.h>
#include <stdint.h>

#pragma pack(push, 1)

namespace lac
{
	template <uint16_t MAX_PACKET = 32, uint8_t ALTS = 2>
	struct usbd_midi_dfu_conf_t
	{
		static constexpr uint32_t CONF_SIZE = sizeof(usbd_config) + sizeof(usbd_midi_function_t<0, 1, 1, MAX_PACKET>)
											  + sizeof(usbd_dfu_alts_t<2, USBD_STRID_IFACE2, ALTS>)
											  + sizeof(usb_dfu_descriptor);

		usbd_config config = {
			.wTotalLength		 = CONF_SIZE,
			.bNumInterfaces		 = 3,
			.bConfigurationValue = 1,
			.iConfiguration		 = 0,
			.bmAttributes		 = usbd_config::get_attributes(true),
			.bMaxPower			 = 0x32,
		};

		usbd_midi_function_t<0, 1, 1, MAX_PACKET> midi = {};

		usbd_dfu_alts_t<2, USBD_STRID_IFACE2, ALTS> dfu_alts = {};

		usb_dfu_descriptor dfu_desc = {};
	} bug_packed;

	using usbd_midi_dfu_conf = usbd_midi_dfu_conf_t<>;

	template <typename layout_t,
			  uint16_t PID		  = 0x4E4D,
			  uint16_t BCD_DEVICE = 0x0002,
			  uint16_t MAX_PACKET = 32,
			  bool	   HIGH_SPEED = false>
	struct usbd_midi_dfu_t
	{
		constexpr static bool					COMPOSITE_WINUSB = true;
		constexpr static std::array<uint8_t, 1> WINUSB_IFACES	 = {2};

		constexpr static usbd_device dev = {
			.bcdUSB			= USBD_VER_2_1,
			.bMaxPacketSize = 64,
			.idVendor		= 0x0483,
			.idProduct		= PID,
			.bcdDevice		= BCD_DEVICE,
		};
		constexpr static usbd_qualifier qual = {
			.bcdUSB				= USBD_VER_2_0,
			.bMaxPacketSize		= 64,
			.bNumConfigurations = 1,
		};
		constexpr static uint8_t								ALTS	 = usbd_dfu_alt_count<layout_t>;
		constexpr static usbd_midi_dfu_conf_t<MAX_PACKET, ALTS> conf	 = {};
		constexpr static const char16_t*						cstr_if0 = layout_t::descriptor;
		constexpr static const char16_t*						cstr_if1 = usbd_dfu_options_descriptor<layout_t>();
		constexpr static usbd_string_const<cstr_if0>			str_if0	 = {};
		constexpr static usbd_string_const<cstr_if1>			str_if1	 = {};
		constexpr static usbd_descriptors desc = {&dev, {&conf.config, 0, 0, 0}, 1, HIGH_SPEED ? &qual : nullptr};

		template <class usb_t>
		void init(usb_t& usb)
		{
			usb.set_string(USBD_STRID_IFACE2, str_if0.get_string());
			if constexpr(ALTS == 2)
			{
				usb.set_string(USBD_STRID_IFACE3, str_if1.get_string());
			}
			usb.set_descriptors(&desc);
			usb.set_data_endpoint_count(1);
			usb.dfu->dfu_iface = 2;
		}
	};

} // namespace lac

#pragma pack(pop)
