/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usbd.h"
#include "usbd_string.h"
#include <stdint.h>
#include <utl/bug.h>

namespace lac
{
	struct usbd_strings_index
	{
		const static uint32_t MAX_STRING_ID = 16;

		const usbd_string* strings[MAX_STRING_ID] = {};

		constexpr void set(usbd_stringid idx, const usbd_string* value)
		{
			lac_assert(idx < MAX_STRING_ID);

			if(idx < MAX_STRING_ID)
			{
				strings[idx] = value;
			}
		}

		const usbd_string* get(usbd_stringid idx) const
		{
			if(idx < MAX_STRING_ID)
			{
				return strings[idx];
			}
			else
			{
				return nullptr;
			}
		}
	};
} // namespace lac
