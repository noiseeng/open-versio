/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

namespace lac
{
	template <typename timer_t>
	struct timer_interval
	{
		int32_t budget		= 0;
		int32_t last_start	= 0;
		int32_t time_start	= 0;
		int32_t time_end	= 0;
		int32_t percent		= 0;
		int32_t max_percent = 0;
		int32_t last_period = 0;
		int32_t last_total	= 0;
		int32_t overruns	= 0;

		static constexpr int32_t budget_of(uint32_t frames, uint32_t frame_rate)
		{ return (int32_t)((uint64_t)timer_t::TIMER_HERTZ * frames / frame_rate); }

		constexpr timer_interval() = default;

		constexpr explicit timer_interval(int32_t budget_ticks) : budget(budget_ticks) {}

		void start()
		{
			last_start = time_start;
			time_start = timer_t::get_clk();
		}

		void end()
		{
			time_end	= timer_t::get_clk();
			last_period = time_end - time_start;
			last_total	= time_start - last_start;
			percent		= budget == 0 ? 0 : (int32_t)((int64_t)100 * last_period / budget);
			if(budget != 0 && last_total > budget + budget / 2)
			{
				overruns = overruns + 1;
			}
			if(percent > max_percent)
			{
				max_percent = percent;
			}
		}

		int32_t get_max()
		{
			int32_t m	= max_percent;
			max_percent = percent;
			return m;
		}
	};

} // namespace lac
