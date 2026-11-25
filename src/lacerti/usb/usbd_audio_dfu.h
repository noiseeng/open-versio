/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usbd.h"
#include "usbd_audio.h"
#include "usbd_core.h"
#include "usbd_dfu.h"
#include "usbd_string_const.h"
#include <array>
#include <bug/compiler.h>
#include <stdint.h>

#pragma pack(push, 1)

namespace lac
{
	// Composite UAC1 + DFU-runtime configuration for the H750. One configuration that
	// carries the full stereo-in/stereo-out audio interfaces of usbd_audio_basic plus a
	// DFU runtime interface, so a normally-running module presents a class-compliant USB
	// audio device while a host DFU_DETACH (bitWillDetach) drops it into the existing
	// self-hosted hot-flash DFU path. The audio interfaces are byte-identical to
	// usbd_audio_basic; the trailing DFU runtime interface (number 3, protocol 1) and DFU
	// functional descriptor are appended. This audio-only variant is a building block; the
	// shipping Versio composite that adds a MIDIStreaming function is usbd_audio_midi_dfu.

	static constexpr uint8_t USBD_AUDIO_DFU_IFACE = 3;

	struct usbd_audio_dfu_config
	{
		usbd_config config = {
			.wTotalLength		 = sizeof(usbd_audio_dfu_config),
			.bNumInterfaces		 = 4,
			.bConfigurationValue = 1,
			.iConfiguration		 = 0,
			.bmAttributes		 = usbd_config::get_attributes(true),
			.bMaxPower			 = 0x32,
		};

		usbd_audio_uac1_stereo audio = {};

		usbd_interface dfu_iface = {
			.bInterfaceNumber	= USBD_AUDIO_DFU_IFACE,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 0,
			.bInterfaceClass	= USBD_CLASS_APP_SPEC,
			.bInterfaceSubClass = 1, // DFU
			.bInterfaceProtocol = 1, // runtime (App) mode
			.iInterface			= USBD_STRID_IFACE3,
		};
		usb_dfu_descriptor dfu_func = {}; // bmAttributes 0x0B carries WILL_DETACH
	} bug_packed;

	template <usbd_device DEV>
	struct usbd_audio_dfu
	{
		constexpr static usbd_device				 dev			  = DEV;
		constexpr static bool						 COMPOSITE_WINUSB = true;
		constexpr static std::array<uint8_t, 1>		 WINUSB_IFACES	  = {USBD_AUDIO_DFU_IFACE};
		constexpr static usbd_audio_dfu_config		 conf			  = {};
		constexpr static const char16_t*			 cstr_dfu		  = u"Versio DFU";
		constexpr static usbd_string_const<cstr_dfu> str_dfu		  = {};
		constexpr static usbd_descriptors			 desc			  = {&dev, {&conf.config, 0, 0, 0}, 1};

		template <class usb_t>
		void init(usb_t& usb)
		{
			usb.set_string(USBD_STRID_IFACE3, str_dfu.get_string());
			usb.set_descriptors(&desc);
			usb.dfu->dfu_iface = USBD_AUDIO_DFU_IFACE;
		}
	};

} // namespace lac

#pragma pack(pop)
