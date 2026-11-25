/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

namespace lac
{
	enum hui_flip : int8_t
	{
		HUI_NOFLIP,
		HUI_FLIP
	};

	enum hui_dir : int8_t
	{
		HUI_UP		= 1,
		HUI_RELEASE = 1,
		HUI_NONE	= 0,
		HUI_DN		= -1,
		HUI_PRESS	= -1
	};

	constexpr inline hui_dir flags_to_dir(bool now, bool prev)
	{
		if(now == prev)
		{
			return HUI_NONE;
		}

		return now ? HUI_UP : HUI_DN;
	}

	constexpr inline bool flip_bool(hui_flip f, bool v)
	{ return f ? !v : v; }

	constexpr inline int32_t hui_clk_sub(int32_t a, int32_t b)
	{ return (int32_t)((uint32_t)a - (uint32_t)b); }

	constexpr inline int32_t hui_clk_add(int32_t a, int32_t b)
	{ return (int32_t)((uint32_t)a + (uint32_t)b); }

}; // namespace lac