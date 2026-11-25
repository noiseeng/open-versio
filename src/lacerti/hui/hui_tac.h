/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hui_const.h"
#include "hui_debounce.h"
#include <array>
#include <stdint.h>
#include <utl/bug.h>

namespace lac
{
	struct hui_conf_tac
	{
		int8_t	 bit  = 0;
		hui_flip flip = HUI_NOFLIP;
	};

	template <uint32_t count>
	using hui_array_tac = std::array<hui_conf_tac, count>;

	struct hui_value_tac
	{
		bool	value	   = false;
		bool	last_value = false;
		int32_t tap_time   = 0;
		int32_t held	   = 0;
		int32_t last_held  = 0;

		constexpr bool	  held_reached(int32_t count) const { return held >= count && last_held < count; }
		constexpr bool	  changed() const { return value != last_value; }
		constexpr bool	  pressed() const { return changed() && value; }
		constexpr bool	  released() const { return changed() && !value; }
		constexpr hui_dir get_motion() const { return flags_to_dir(value, last_value); }
		constexpr bool	  is_double_tap(int32_t time) const { return pressed() && tap_time && tap_time < time; }

		constexpr explicit operator hui_dir() const { return get_motion(); }
		constexpr explicit operator bool() const { return value; }

		template <class fix_t>
		constexpr fix_t to_fixed() const
		{ return value ? fix_t::ONE() : fix_t::ZERO(); }
	};

	struct hui_filter_tac
	{
		hui_debounce debounce;
		bool		 last_value	   = false;
		int32_t		 last_tap_time = 0;
		int32_t		 tap_time	   = 0;
		int32_t		 held_start	   = 0;
		int32_t		 last_held	   = 0;

		constexpr void clear_tap_time()
		{
			tap_time	  = 0;
			last_tap_time = 0;
		}

		constexpr hui_value_tac
		operator()(int32_t now, int32_t debounce_time, int32_t tap_timeout, int32_t hold_time, int32_t next_value)
		{
			lac_assert(hold_time);

			bool value = debounce(now, debounce_time, next_value) != 0;

			if(value && !last_value)
			{
				tap_time	  = last_tap_time ? hui_clk_sub(now, last_tap_time) : 0;
				last_tap_time = now;

				held_start = now;
			}
			else if(last_tap_time && hui_clk_sub(now, last_tap_time) > tap_timeout)
			{
				last_tap_time = 0;
			}

			int32_t held = 0;

			if(value)
			{
				if(hold_time && held_start && hui_clk_sub(now, held_start) >= hold_time)
				{
					held = hui_clk_sub(now, held_start) / hold_time;

					if(held >= 4)
					{
						held_start = hui_clk_sub(now, 4 * hold_time);
					}
				}
			}
			else
			{
				held	   = 0;
				held_start = 0;
			}

			hui_value_tac r = {value, last_value, tap_time, held, last_held};

			last_held = held;

			last_value = value;

			return r;
		}
	};
} // namespace lac