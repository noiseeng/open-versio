/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <bug/eabi.h>
#include <stdint.h>

namespace lac
{
	template <typename timer_t, bool in_ship = true>
	struct timer_poll
	{
		const uint32_t period = 0;

		uint32_t time = timer_t::get_clk();

		bool operator()()
		{
			bool ready = false;

			if constexpr(in_ship || !bug::is_build_ship())
			{
				if(timer_t::get_clk() - time > period)
				{
					ready = true;
					time  = timer_t::get_clk();
				}
			}

			return ready;
		}

		template <class cb_t>
		void operator()(cb_t cb)
		{
			if(operator()())
			{
				cb();
			}
		}
	};

} // namespace lac