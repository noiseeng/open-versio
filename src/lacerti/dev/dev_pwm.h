/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>
#include <utl/bug.h>

namespace lac
{
	template <uint32_t BITS>
	struct dev_pwm
	{
		static_assert(BITS <= 16, "pwm > 16 bits is crazy, yo!");
		const static uint32_t MAX_COUNT = 1 << BITS;
		const static uint32_t MASK		= MAX_COUNT - 1;

		uint32_t next  = 0;
		uint32_t level = 0;
		uint32_t count = 0;

		void reset() { count = MASK; }

		bool operator()()
		{
			count = (count + 1) & MASK;

			if(!count)
			{
				level = next;
			}

			return level > count;
		}

		bool operator()(uint32_t count)
		{
			if(!count)
			{
				level = next;
			}

			return level > count;
		}

		void update() { level = next; }

		void operator=(uint32_t level_u16)
		{
			lac_assert(level_u16 <= 0xFFFF);
			next = (level_u16 >> (16 - BITS));
		}
	};
} // namespace lac
