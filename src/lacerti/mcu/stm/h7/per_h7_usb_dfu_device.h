/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <mcu/stm/h7/per_h7_usb_dfu_flash.h>
#include <usb/usb_audio_handler_none.h>
#include <usb/usb_device.h>
#include <usb/usbd_str16.h>

namespace lac
{
	template <typename mcu_t,
			  typename descr_type,
			  usbd_str16 PROD,
			  bool		 HAS_WINUSB,
			  typename usb_audio_t = usb_audio_handler_none>
	struct per_h7_dfu_device
	{
		using descr_t  = descr_type;
		using device_t = usb_device<mcu_t, descr_t, PROD, HAS_WINUSB, usb_audio_t>;

		static_assert(mcu_t::usb_per_t::CTRL_OUT_MAX >= 1024, "EP0 OUT buffer < DFU block");

		device_t	   device = {};
		usb_dfu<mcu_t> dfu	  = {};

		bool active() const { return dfu.active(); }
		bool failed() const { return dfu.failed; }

		template <typename led_cb_t>
		void poll(led_cb_t leds)
		{ dfu.poll(leds); }
	};
} // namespace lac
