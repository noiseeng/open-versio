/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usb_defs.h"
#include "usbd.h"
#include <bug/compiler.h>
#include <stdint.h>

#pragma pack(push, 1)

namespace lac
{
	struct usbd_string
	{
		usb_header head = {2, USBD_DESC_STRING};
		char16_t   str[1];

		constexpr uint32_t get_size() const { return head.bLength; }

		const uint8_t* get_data() const { return (const uint8_t*)this; }
	} bug_packed;
} // namespace lac

#pragma pack(pop)
