/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

namespace lac
{
	struct timer_delay
	{
		uint32_t time_last	 = 0;
		uint32_t time_remain = 0;
		bool	 active		 = false;

		constexpr operator bool() const { return active; }

		constexpr bool operator()(uint32_t time_now)
		{
			uint32_t delta = time_now - time_last;
			time_last	   = time_now;

			if(active && time_remain > delta)
			{
				time_remain -= delta;
			}
			else
			{
				active = false;
			}

			return active;
		}

		constexpr void operator=(uint32_t delay)
		{
			if(delay != 0)
			{
				time_remain = delay;
				active		= true;
			}
			else
			{
				time_remain = 0;
				active		= false;
			}
		}
	};

} // namespace lac