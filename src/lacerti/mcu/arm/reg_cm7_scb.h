/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "mcu_arm_v7m.h"
#include "reg_v7m_scb.h"
#include <atomic>
#include <cstdint>
#include <utl/bitfield.h>
#include <utl/bitflag.h>

namespace lac
{
	constexpr uint32_t SCB_CACHE_LINE_SIZE = 32;
	constexpr uint32_t SCB_CACHE_LINE_MASK = (~31);

	struct reg_cm7_scb : reg_v7m_scb
	{
		const volatile uint32_t RESERVED3[93U];
		volatile uint32_t		STIR;
		const volatile uint32_t RESERVED4[15U];
		const volatile uint32_t MVFR0;
		const volatile uint32_t MVFR1;
		const volatile uint32_t MVFR2;
		const volatile uint32_t RESERVED5[1U];
		volatile uint32_t		ICIALLU;
		const volatile uint32_t RESERVED6[1U];
		volatile uint32_t		ICIMVAU;
		volatile uint32_t		DCIMVAC;
		volatile uint32_t		DCISW;
		volatile uint32_t		DCCMVAU;
		volatile uint32_t		DCCMVAC;
		volatile uint32_t		DCCSW;
		volatile uint32_t		DCCIMVAC;
		volatile uint32_t		DCCISW;
		const volatile uint32_t RESERVED7[6U];
		volatile uint32_t		ITCMCR;
		volatile uint32_t		DTCMCR;
		volatile uint32_t		AHBPCR;
		volatile uint32_t		CACR;
		volatile uint32_t		AHBSCR;
		const volatile uint32_t RESERVED8[1U];
		volatile uint32_t		ABFSR;

		using CCR_DC = bitflag<16>;
		using CCR_IC = bitflag<17>;
		using CCR_BP = bitflag<18>;

		using CLIDR_LOC	 = bitfield<24, 3>;
		using CLIDR_LOUU = bitfield<27, 3>;

		using CTR_IMINLINE = bitfield<0, 4>;
		using CTR_DMINLINE = bitfield<16, 4>;
		using CTR_ERG	   = bitfield<20, 4>;
		using CTR_CWG	   = bitfield<24, 4>;
		using CTR_FORMAT   = bitfield<29, 3>;

		using CCSIDR_LINESIZE	   = bitfield<0, 3>;
		using CCSIDR_ASSOCIATIVITY = bitfield<3, 10>;
		using CCSIDR_NUMSETS	   = bitfield<13, 15>;
		using CCSIDR_WA			   = bitflag<28>;
		using CCSIDR_RA			   = bitflag<29>;
		using CCSIDR_WB			   = bitflag<30>;
		using CCSIDR_WT			   = bitflag<31>;

		using CSSELR_IND   = bitflag<0>;
		using CSSELR_LEVEL = bitfield<1, 3>;

		using STIR_INTID = bitfield<0, 9>;

		using DCISW_SET = bitfield<5, 9>;
		using DCISW_WAY = bitfield<30, 2>;

		using DCCSW_SET = bitfield<5, 17>;
		using DCCSW_WAY = bitfield<30, 2>;

		using DCCISW_SET = bitfield<5, 17>;
		using DCCISW_WAY = bitfield<30, 2>;

		using ITCMCR_EN	   = bitflag<0>;
		using ITCMCR_RMW   = bitflag<1>;
		using ITCMCR_RETEN = bitflag<2>;
		using ITCMCR_SZ	   = bitfield<3, 4>;

		using DTCMCR_EN	   = bitflag<0>;
		using DTCMCR_RMW   = bitflag<1>;
		using DTCMCR_RETEN = bitflag<2>;
		using DTCMCR_SZ	   = bitfield<3, 4>;

		using AHBPCR_EN = bitflag<0>;
		using AHBPCR_SZ = bitfield<1, 3>;

		using CACR_SIWT	   = bitflag<0>;
		using CACR_ECCDIS  = bitflag<1>;
		using CACR_FORCEWT = bitflag<2>;

		using ABFSR_ITCM	 = bitflag<0>;
		using ABFSR_DTCM	 = bitflag<1>;
		using ABFSR_AHBP	 = bitflag<2>;
		using ABFSR_AXIM	 = bitflag<3>;
		using ABFSR_EPPB	 = bitflag<4>;
		using ABFSR_AXIMTYPE = bitfield<8, 2>;

		using AHBSCR_CTL	   = bitfield<0, 2>;
		using AHBSCR_TPRI	   = bitfield<2, 9>;
		using AHBSCR_INITCOUNT = bitfield<11, 5>;

		void enable_unalign_trap() { CCR = CCR | CCR_UNALIGN_TRP::BIT; }

		void enable_icache()
		{
			mcu_arm_v7m::dsb();
			mcu_arm_v7m::isb();
			ICIALLU = 0;
			mcu_arm_v7m::dsb();
			mcu_arm_v7m::isb();
			CCR = CCR | CCR_IC::BIT;
			mcu_arm_v7m::dsb();
			mcu_arm_v7m::isb();
		}

		uint32_t get_sets() const { return CCSIDR_NUMSETS::get(CCSIDR); }

		uint32_t get_ways() const { return CCSIDR_ASSOCIATIVITY::get(CCSIDR); }

		void enable_dcache()
		{
			CSSELR = 0;
			mcu_arm_v7m::dsb();

			uint32_t sets = get_sets();
			do
			{
				uint32_t ways = get_ways();
				do
				{
					DCISW = DCISW_WAY::val(ways) | DCISW_SET::val(sets);
				} while(ways-- != 0);
			} while(sets-- != 0);
			mcu_arm_v7m::dsb();

			CCR = CCR | CCR_DC::BIT;

			mcu_arm_v7m::dsb();
			mcu_arm_v7m::isb();
		}

		void invalidate_dcache(void* addr, int32_t dsize)
		{
			uint32_t start = ((uint32_t)addr) & SCB_CACHE_LINE_MASK;
			uint32_t end   = (dsize + (uint32_t)addr) & SCB_CACHE_LINE_MASK;

			mcu_arm_v7m::dsb();

			while(start != end)
			{
				DCIMVAC = start;
				start += SCB_CACHE_LINE_SIZE;
			}

			mcu_arm_v7m::dsb();
			mcu_arm_v7m::isb();
		}

		void clean_dcache()
		{
			CSSELR = 0;

			mcu_arm_v7m::dsb();

			uint32_t sets = 0x7f;
			do
			{
				DCCSW = DCCSW_WAY::val(3) | DCCSW_SET::val(sets);
				DCCSW = DCCSW_WAY::val(2) | DCCSW_SET::val(sets);
				DCCSW = DCCSW_WAY::val(1) | DCCSW_SET::val(sets);
				DCCSW = DCCSW_WAY::val(0) | DCCSW_SET::val(sets);

			} while(sets-- != 0);

			mcu_arm_v7m::dsb();
			mcu_arm_v7m::isb();
		}

		void reset_dcache()
		{
			CSSELR = 0U;

			mcu_arm_v7m::dsb();

			uint32_t sets = get_sets();

			do
			{
				uint32_t ways = get_ways();

				do
				{
					DCCISW = DCCISW_WAY::val(ways) | DCCISW_SET::val(sets);
					std::atomic_thread_fence(std::memory_order_seq_cst);
				} while(ways-- != 0U);
			} while(sets-- != 0U);

			mcu_arm_v7m::dsb();
			mcu_arm_v7m::isb();
		}

		void disable_dcache()
		{
			mcu_arm_v7m::dsb();
			CCR = CCR & ~CCR_DC::BIT;
			reset_dcache();
		}
	};

	static_assert(sizeof(reg_cm7_scb) == 0x2AC, "invalid size");

} // namespace lac
