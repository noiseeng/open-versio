/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>
#include <utl/timer_delay.h>

namespace lac
{
	struct hui_debounce
	{
		timer_delay timer;

		int32_t last_value = false;
		int32_t value	   = false;

		constexpr int32_t operator()(int32_t now, int32_t period, int32_t next_value)
		{
			if(value != next_value)
			{
				timer = period;
				value = next_value;
			}

			if(!timer(now) && value != last_value)
			{
				last_value = value;
			}

			return last_value;
		}
	};
} // namespace lac