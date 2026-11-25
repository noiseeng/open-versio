/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

#include "per_h7_usb_dfu_flash.h"
#include <mcu/stm/h7/per_h7_usb_dfu_device.h>
#include <mcu/stm/h750/usb_h750.h>
#include <usb/usbd_str16.h>

namespace lac
{
	template <typename mcu_t, usbd_str16 PROD, bool HAS_WINUSB = true, typename descr_t = lac::usbd_dfu_h750>
	struct per_h7_dfu : per_h7_dfu_device<mcu_t, descr_t, PROD, HAS_WINUSB>
	{
		uint32_t led_phase = 0;

		template <typename set_leds_t>
		void poll_leds(set_leds_t set_leds)
		{
			this->poll([&](dfu_led s) {
				if(s == dfu_led::pulse)
				{
					led_phase++;
				}
				set_leds(s, ((led_phase >> 4) & 1u) != 0);
			});
		}

		void init()
		{
			this->dfu.attach(this->device.usb);
			this->device.init();
		}

		void poll_log() {}
	};
} // namespace lac
