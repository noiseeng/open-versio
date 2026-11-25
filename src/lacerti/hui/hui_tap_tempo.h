/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hui_tac.h"
#include <stdint.h>

namespace lac
{
	template <uint64_t time_hz>
	struct hui_tap_tempo
	{
		static constexpr uint32_t TRG_MIN_CHANGE = (uint32_t)(time_hz / 33);
		static constexpr uint32_t SNAP_CHANGE	 = (uint32_t)(time_hz / 20);

		uint32_t period_new = 0;
		uint32_t period_clk = 0;
		bool	 tap_armed	= false;
		bool	 trg_armed	= false;
		bool	 cleared	= false;

		static constexpr uint32_t udiff(uint32_t a, uint32_t b) { return a > b ? a - b : b - a; }

		static constexpr uint32_t ulpf_adapt(uint32_t dx)
		{
			constexpr uint32_t MAX_CHECK = 8;

			for(uint32_t i = 0; i < MAX_CHECK; i++)
			{
				const uint32_t s = MAX_CHECK - i;

				if(dx > (uint32_t{1} << s))
				{
					return dx >> (s - 1);
				}
			}

			return dx;
		}

		static constexpr uint32_t ulpf(uint32_t x, uint32_t y)
		{
			if(x == y)
			{
				return x;
			}

			return x > y ? y + ulpf_adapt(x - y) : y - ulpf_adapt(y - x);
		}

		constexpr bool get_cleared()
		{
			const bool c = cleared;
			cleared		 = false;
			return c;
		}

		constexpr void tick(const hui_value_tac& tap, const hui_value_tac& trg)
		{
			if(tap.held_reached(1))
			{
				cleared	   = true;
				tap_armed  = false;
				trg_armed  = false;
				period_new = 0;
				period_clk = 0;
				return;
			}

			if(tap.pressed())
			{
				if(tap_armed && tap.tap_time)
				{
					trg_armed  = false;
					period_new = (uint32_t)tap.tap_time;
				}

				tap_armed = true;
			}

			if(trg.pressed())
			{
				if(trg_armed && trg.tap_time)
				{
					tap_armed = false;

					if(udiff(period_new, (uint32_t)trg.tap_time) > TRG_MIN_CHANGE)
					{
						period_new = (uint32_t)trg.tap_time;
					}
				}

				trg_armed = true;
			}

			if(!period_new)
			{
				return;
			}

			if(!period_clk || udiff(period_clk, period_new) > SNAP_CHANGE)
			{
				period_clk = period_new;
			}
			else
			{
				period_clk = ulpf(period_new, period_clk);
			}
		}

		constexpr int32_t get_period(uint64_t unit_hz) const { return (int32_t)(unit_hz * period_clk / time_hz); }
	};
} // namespace lac
