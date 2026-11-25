/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usb_setup.h"
#include <stdint.h>
#include <type_traits>

namespace lac
{
	struct usb_audio_handler_none
	{
		constexpr bool owns_iface(uint8_t) const { return false; }

		constexpr void cancel_data() {}

		constexpr void reset() {}

		template <typename usb_t>
		void on_setup(usb_t *, const usb_setup *)
		{}
		template <typename usb_t>
		void on_ep_setup(usb_t *, const usb_setup *)
		{}
		template <typename usb_t>
		void on_recv(usb_t *, uint32_t, uint32_t, const uint8_t *)
		{}
		template <typename usb_t>
		void on_sof(usb_t *)
		{}
	};
	static_assert(std::is_empty_v<usb_audio_handler_none>);
} // namespace lac
