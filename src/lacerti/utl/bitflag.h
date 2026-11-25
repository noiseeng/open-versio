/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

#include "bits.h"

namespace lac
{
	template <uint32_t pos, typename field_t = uint32_t>
	struct bitflag
	{
		static_assert(pos < 8 * sizeof(field_t), "pos too big");

		const static field_t POS  = pos;
		const static field_t BIT  = (field_t)((field_t)1 << pos);
		const static field_t MASK = (field_t)~BIT;

		constexpr static field_t val() { return BIT; }

		constexpr static field_t val(bool s) { return s ? BIT : (field_t)0; }

		constexpr static void set(volatile field_t &reg, bool value)
		{
			if(value)
			{
				set(reg);
			}
			else
			{
				clr(reg);
			}
		}

		constexpr static void set(volatile field_t &reg) { reg = (field_t)(reg | BIT); }

		static void setdly(volatile field_t &reg)
		{
			reg = (field_t)(reg | BIT);
			bits::reg_sync(reg);
		}

		constexpr static void clr(volatile field_t &reg) { reg = (field_t)(reg & ~BIT); }

		static void clrdly(volatile field_t &reg)
		{
			reg = (field_t)(reg & ~BIT);
			bits::reg_sync(reg);
		}

		constexpr static field_t get(const volatile field_t &reg) { return (field_t)(reg & BIT); }

		template <bool value = true>
		static void wait(const volatile field_t &reg)
		{
			while((0 != get(reg)) != value)
				;
		}
	};
} // namespace lac
