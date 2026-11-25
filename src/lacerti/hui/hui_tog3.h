/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <stdint.h>
#include <utl/bug.h>

namespace lac
{
	enum hui_tog3_position : int8_t
	{
		HUI_TOG3_L = 0,
		HUI_TOG3_M,
		HUI_TOG3_R,
		HUI_TOG3_B = 0,
		HUI_TOG3_C,
		HUI_TOG3_T,
	};

	struct hui_conf_tog3
	{
		int8_t bita = 0;
		int8_t bitb = 0;
	};

	template <uint32_t count>
	using hui_array_tog3 = std::array<hui_conf_tog3, count>;

	struct hui_value_tog3
	{
		int8_t value	  = 0;
		int8_t last_value = 0;

		constexpr bool changed() const { return value != last_value; }

		constexpr int32_t get_motion() const { return value - last_value; }

		constexpr explicit operator uint32_t() const { return value; }
		constexpr explicit operator int32_t() const { return value; }

		template <class fix_t>
		fix_t to_fixed() const
		{
			lac_assert(value >= 0 && value <= 2);
			return fix_t::NTH(2) * value;
		}
	};

	struct hui_filter_tog3
	{
		constexpr static int8_t tog3_encode(bool bita, bool bitb)
		{
			if(!bita)
			{
				return 0;
			}

			if(!bitb)
			{
				return 2;
			}

			return 1;
		}

		constexpr hui_value_tog3 operator()(bool a1, bool b1, bool a0, bool b0) const
		{ return hui_value_tog3{tog3_encode(a1, b1), tog3_encode(a0, b0)}; }
	};

} // namespace lac