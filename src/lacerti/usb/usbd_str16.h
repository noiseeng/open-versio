/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>

namespace lac
{
	template <std::size_t N>
	struct usbd_str16
	{
		char16_t value[N] = {};

		constexpr usbd_str16(const char16_t (&s)[N])
		{
			for(std::size_t i = 0; i < N; i++)
			{
				value[i] = s[i];
			}
		}
	};
} // namespace lac
