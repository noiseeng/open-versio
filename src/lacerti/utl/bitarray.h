/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "bug.h"
#include <stdint.h>

namespace lac
{
	template <uint32_t array_count, uint32_t bit_count, uint32_t start = 0>
	struct bitarray
	{
		const static uint32_t BMASK = (1 << bit_count) - 1;

		static uint32_t shift(uint32_t i)
		{
			lac_assert(i < array_count);
			uint32_t shift = i * bit_count + start;
			return shift;
		}

		static uint32_t index(uint32_t i)
		{
			const static uint32_t array_per_reg = 32 / bit_count;
			return i / array_per_reg;
		}

		static uint32_t mask(uint32_t i) { return BMASK << shift(i); }

		static uint32_t get(const volatile uint32_t &reg, uint32_t i) { return (reg & mask(i)) >> shift(i); }

		static void set(volatile uint32_t &reg, uint32_t i, uint32_t v)
		{
			lac_assert((v & (~BMASK)) == 0);

			reg = (reg & ~mask(i)) | (v << shift(i));
		}

		static void set(volatile uint32_t &reg, uint32_t i) { set(reg, i, BMASK); }

		static void clr(volatile uint32_t &reg, uint32_t i) { set(reg, i, 0); }

		static uint32_t get(const volatile uint32_t *ar, uint32_t i)
		{
			const volatile uint32_t &reg = ar[index(i)];
			return (reg & mask(i)) >> shift(i);
		}

		static void set(volatile uint32_t *ar, uint32_t i, uint32_t v)
		{
			volatile uint32_t &reg = ar[index(i)];

			lac_assert((v & (~BMASK)) == 0);

			reg = (reg & ~mask(i)) | (v << shift(i));
		}

		static void set(volatile uint32_t *ar, uint32_t i) { set(ar, i, BMASK); }

		static void clr(volatile uint32_t *ar, uint32_t i) { set(ar, i, 0); }
	};
} // namespace lac
