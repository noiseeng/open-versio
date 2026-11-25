/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <utl/bitfield.h>
#include <utl/bitflag.h>
#include <utl/bug.h>

#include <cstdint>

namespace lac
{
	struct reg_h7_mpu
	{
		volatile uint32_t TYPE;
		volatile uint32_t CTRL;
		volatile uint32_t RNR;
		volatile uint32_t RBAR;
		volatile uint32_t RASR;
		volatile uint32_t RBAR_A1;
		volatile uint32_t RASR_A1;
		volatile uint32_t RBAR_A2;
		volatile uint32_t RASR_A2;
		volatile uint32_t RBAR_A3;
		volatile uint32_t RASR_A3;

		using TYPE_DREGION = bitfield<8, 8>;
		using TYPE_IREGION = bitfield<16, 8>;

		using CTRL_ENABLE	  = bitflag<0>;
		using CTRL_HFNMIENA	  = bitflag<1>;
		using CTRL_PRIVDEFENA = bitflag<2>;

		using RNR_REGION = bitfield<0, 8>;

		using RBAR_REGION = bitfield<0, 4>;
		using RBAR_ADDR	  = bitfield<5, 27>;

		using RASR_ENABLE = bitflag<0>;
		using RASR_SIZE	  = bitfield<1, 5>;
		using RASR_SRD	  = bitfield<8, 8>;
		using RASR_B	  = bitflag<16>;
		using RASR_C	  = bitflag<17>;
		using RASR_S	  = bitflag<18>;
		using RASR_TEX	  = bitfield<19, 3>;
		using RASR_AP	  = bitfield<24, 3>;
		using RASR_XN	  = bitflag<28>;

		constexpr static uint32_t AP_RW_RW		 = RASR_AP::val(3);
		constexpr static uint32_t TEXSCB_NOCACHE = RASR_TEX::val(4) | RASR_S::val();
		constexpr static uint32_t TEXSCB_CACHE	 = RASR_B::val() | RASR_C::val();

		uint32_t get_data_region_count() { return (TYPE >> 8) & 0xFF; }

		uint32_t get_next_region()
		{
			static uint32_t region = 0;
			lac_assert(region < get_data_region_count());
			return region++;
		}

		void set_nocache(uint32_t start, uint32_t size) { set_region(start, size, TEXSCB_NOCACHE); }

		void set_cache(uint32_t start, uint32_t size) { set_region(start, size, TEXSCB_CACHE); }

		void set_region(uint32_t start, uint32_t size, uint32_t texscb)
		{
			uint32_t rasr_size = 32 - __builtin_clz(size) - 2;

			lac_assert_ispow2(size);
			lac_assert(((size - 1) & start) == 0);
			lac_assert(size == (uint32_t)(1 << (rasr_size + 1)));

			CTRL = CTRL_ENABLE::BIT | CTRL_PRIVDEFENA::BIT;

			RNR	 = get_next_region();
			RBAR = start;
			RASR = RASR_ENABLE::BIT | AP_RW_RW | texscb | RASR_SIZE::val(rasr_size);
		}
	};

} // namespace lac
