/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <mcu/stm/h7/per_h7_usb_dfu.h>
#include <usb/usb_log.h>
#include <usb/usbd_dfu_log.h>
#include <usb/usbd_str16.h>
#include <utl/log_default.h>

namespace lac
{
	template <typename mcu_t, usbd_str16 PROD, typename descr_t>
	struct per_h7_dfu_log : per_h7_dfu<mcu_t, PROD, true, descr_t>
	{
		usb_log<USBD_LOG_EP, USBD_LOG_MAX_PACKET> log = {};

		void poll_log() { log.poll(this->device.usb, ::_SEGGER_RTT); }
	};
} // namespace lac
