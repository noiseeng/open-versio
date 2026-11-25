/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

namespace lac
{
	struct usb_client
	{
		uint32_t interrupt_endpoint_count	= 0;
		uint32_t isochronous_endpoint_count = 0;

		void* context = 0;

		void (*usb_setup)(void* ctx, uint32_t count, const uint8_t* data);
		void (*usb_recv)(void* ctx, uint32_t ep, uint32_t count, const uint8_t* data);
		void (*usb_sent)(void* ctx, uint32_t ep, uint32_t count);
		void (*usb_reset)(void* ctx);
		void (*usb_sof)(void* ctx) = nullptr;

		constexpr void recv(uint32_t ep, uint32_t count, const uint8_t* data) { usb_recv(context, ep, count, data); }

		constexpr void setup(uint32_t count, const uint8_t* data) { usb_setup(context, count, data); }

		constexpr void sent(uint32_t ep, uint32_t count) { usb_sent(context, ep, count); }

		constexpr void reset() { usb_reset(context); }

		constexpr void sof()
		{
			if(usb_sof)
			{
				usb_sof(context);
			}
		}
	};
} // namespace lac