/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usb_char16.h"
#include "usb_defs.h"
#include "usbd.h"
#include "usbd_str16.h"
#include "usbd_string.h"
#include <array>
#include <bug/compiler.h>
#include <cstddef>

#pragma pack(push, 1)

namespace lac
{
	template <usbd_str16 LIT>
	struct usbd_string_literal
	{
		constexpr static size_t str_len	 = sizeof(LIT.value) / sizeof(char16_t) - 1;
		constexpr static size_t str_size = sizeof(char16_t) * str_len;

		const usb_header head = {str_size + 2, USBD_DESC_STRING};

		std::array<char16_t, str_len> array = char16_to_array<str_len>(LIT.value);

		const usbd_string* get_string() const { return (const usbd_string*)this; }
	} bug_packed;
} // namespace lac

#pragma pack(pop)
