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
	// Composite UAC1 audio + USB-MIDI + DFU-runtime configuration for the H750. One configuration
	// that carries the full stereo-in/stereo-out audio interfaces (AudioControl 0, AudioStreaming
	// OUT 1 / IN 2, iso EP1 OUT + iso EP2 IN) AND a USB-MIDI function (AudioControl 3 + MIDIStreaming
	// 4, bulk EP3) AND a DFU runtime interface (5). A normally-running Versio presents a
	// class-compliant audio + MIDI device, while a host DFU_DETACH drops into the existing
	// self-hosted hot-flash path. All three functions share one usb_state.
	//
	// Endpoint allocation deconflicts audio iso EP1/EP2 from MIDI bulk: the MIDI bulk endpoint is
	// moved to EP3 (0x03 / 0x83). The audio block is byte-identical to usbd_audio_dfu_config's audio
	// interfaces; the MIDI block mirrors usbd_midi_basic_class with the EP addresses moved to EP3.

	static constexpr uint8_t USBD_AUDIO_MIDI_AC_IFACE  = 3;
	static constexpr uint8_t USBD_AUDIO_MIDI_MS_IFACE  = 4;
	static constexpr uint8_t USBD_AUDIO_MIDI_DFU_IFACE = 5;

	static constexpr uint8_t USBD_AUDIO_MIDI_EP_OUT = 0x03;
	static constexpr uint8_t USBD_AUDIO_MIDI_EP_IN	= 0x83;

	template <uint8_t ALTS = 2>
	struct usbd_audio_midi_dfu_config
	{
		usbd_config config = {
			.wTotalLength		 = sizeof(usbd_audio_midi_dfu_config),
			.bNumInterfaces		 = 6,
			.bConfigurationValue = 1,
			.iConfiguration		 = 0,
			.bmAttributes		 = usbd_config::get_attributes(true),
			.bMaxPower			 = 0x32,
		};

		usbd_audio_uac1_stereo audio = {};

		usbd_midi_function_t<USBD_AUDIO_MIDI_AC_IFACE, USBD_AUDIO_MIDI_MS_IFACE, USBD_AUDIO_MIDI_EP_OUT> midi = {};

		usbd_dfu_alts_t<USBD_AUDIO_MIDI_DFU_IFACE, USBD_STRID_IFACE2, ALTS> dfu_alts = {};
		usb_dfu_descriptor dfu_desc = {}; // bmAttributes 0x0B carries WILL_DETACH
	} bug_packed;

	template <typename layout_t, usbd_device DEV>
	struct usbd_audio_midi_dfu
	{
		constexpr static bool					COMPOSITE_WINUSB = true;
		constexpr static std::array<uint8_t, 1> WINUSB_IFACES	 = {USBD_AUDIO_MIDI_DFU_IFACE};

		constexpr static usbd_device					  dev	   = DEV;
		constexpr static uint8_t						  ALTS	   = usbd_dfu_alt_count<layout_t>;
		constexpr static usbd_audio_midi_dfu_config<ALTS> conf	   = {};
		constexpr static const char16_t*				  cstr_if0 = layout_t::descriptor;
		constexpr static const char16_t*				  cstr_if1 = usbd_dfu_options_descriptor<layout_t>();
		constexpr static usbd_string_const<cstr_if0>	  str_if0  = {};
		constexpr static usbd_string_const<cstr_if1>	  str_if1  = {};
		constexpr static usbd_descriptors				  desc	   = {&dev, {&conf.config, 0, 0, 0}, 1};

		template <class usb_t>
		void init(usb_t& usb)
		{
			usb.set_string(USBD_STRID_IFACE2, str_if0.get_string());
			if constexpr(ALTS == 2)
			{
				usb.set_string(USBD_STRID_IFACE3, str_if1.get_string());
			}
			usb.set_descriptors(&desc);
			usb.set_data_endpoint_count(1);						// one MIDI bulk endpoint pair
			usb.set_data_endpoint_base(USBD_AUDIO_MIDI_EP_OUT); // bulk lives on EP3 (iso owns EP1/EP2)
			usb.dfu->dfu_iface = USBD_AUDIO_MIDI_DFU_IFACE;
		}
	};

} // namespace lac

#pragma pack(pop)
