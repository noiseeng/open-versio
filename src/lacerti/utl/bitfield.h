/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

#include "bits.h"
#include "bug.h"

namespace lac
{
	template <uint32_t pos, uint32_t cnt, typename field_t = uint32_t>
	struct bitfield
	{
		static_assert(cnt != 0, "count cant be 0");
		static_assert(pos < 8 * sizeof(field_t), "pos too big");
		static_assert(pos + cnt <= 8 * sizeof(field_t), "field exceeds width");

		const static field_t POS  = pos;
		const static field_t CNT  = cnt;
		const static field_t MAX  = cnt >= 8 * sizeof(field_t) ? (field_t) ~(field_t)0 : (field_t)(((field_t)1 << cnt) - 1);
		const static field_t MSK  = MAX << pos;
		const static field_t MASK = MSK;

		constexpr static field_t val(field_t v) { return (v << POS) & MSK; }

		constexpr static void set(volatile field_t &reg, field_t v)
		{
			field_t rv = val(v);

			lac_assert(rv == (rv & MSK));

			bits::reg_clrset(reg, MSK, rv);
		}

		constexpr static void set(field_t &reg, field_t v)
		{
			field_t rv = val(v);

			lac_assert(rv == (rv & MSK));

			bits::reg_clrset(reg, MSK, rv);
		}

		static void setdly(volatile field_t &reg, field_t v)
		{
			field_t rv = val(v);

			lac_assert(rv == (rv & MSK));

			bits::reg_clrsetdly(reg, MSK, rv);
		}

		constexpr static field_t get(const volatile field_t &reg) { return (reg & MSK) >> POS; }

		constexpr static field_t get(const field_t &reg) { return (reg & MSK) >> POS; }
	};
} // namespace lac
