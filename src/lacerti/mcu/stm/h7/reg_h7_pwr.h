/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <utl/bitarray.h>
#include <utl/bitfield.h>
#include <utl/bitflag.h>

namespace lac
{
	struct reg_h7_pwr
	{
		volatile uint32_t CR1;
		volatile uint32_t CSR1;
		volatile uint32_t CR2;
		volatile uint32_t CR3;
		volatile uint32_t CPUCR;
		volatile uint32_t CPU2CR;
		volatile uint32_t D3CR;
		uint32_t		  RESERVED0;
		volatile uint32_t WKUPCR;
		volatile uint32_t WKUPFR;
		volatile uint32_t WKUPEPR;

		using CR1_LPDS	= bitflag<0>;
		using CR1_PVDE	= bitflag<4>;
		using CR1_PLS	= bitfield<5, 3>;
		using CR1_DBP	= bitflag<8>;
		using CR1_FLPS	= bitflag<9>;
		using CR1_SVOS	= bitfield<14, 2>;
		using CR1_AVDEN = bitflag<16>;
		using CR1_ALS	= bitfield<17, 2>;

		using CSR1_PVDO		 = bitflag<4>;
		using CSR1_ACTVOSRDY = bitflag<13>;
		using CSR1_ACTVOS	 = bitfield<14, 2>;
		using CSR1_AVDO		 = bitflag<16>;

		using CR2_BREN	= bitflag<0>;
		using CR2_MONEN = bitflag<4>;
		using CR2_BRRDY = bitflag<16>;
		using CR2_VBATL = bitflag<20>;
		using CR2_VBATH = bitflag<21>;
		using CR2_TEMPL = bitflag<22>;
		using CR2_TEMPH = bitflag<23>;

		using CR3_BYPASS   = bitflag<0>;
		using CR3_LDOEN	   = bitflag<1>;
		using CR3_VBE	   = bitflag<8>;
		using CR3_VBRS	   = bitflag<9>;
		using CR3_USB33DEN = bitflag<24>;
		using CR3_USBREGEN = bitflag<25>;
		using CR3_USB33RDY = bitflag<26>;

		using CPUCR_PDDS_D1 = bitflag<0>;
		using CPUCR_PDDS_D2 = bitflag<1>;
		using CPUCR_PDDS_D3 = bitflag<2>;
		using CPUCR_STOPF	= bitflag<5>;
		using CPUCR_SBF		= bitflag<6>;
		using CPUCR_SBF_D1	= bitflag<7>;
		using CPUCR_SBF_D2	= bitflag<8>;
		using CPUCR_CSSF	= bitflag<9>;
		using CPUCR_RUN_D3	= bitflag<11>;

		using D3CR_VOSRDY = bitflag<13>;
		using D3CR_VOS	  = bitfield<14, 2>;

		using WKUPCR_WKUPC = bitfield<0, 6>;

		using WKUPFR_WKUPF = bitfield<0, 6>;

		using WKUPEPR_WKUPEN   = bitfield<0, 6>;
		using WKUPEPR_WKUPP	   = bitfield<8, 6>;
		using WKUPEPR_WKUPPUPD = bitarray<6, 2, 16>;

		template <typename syscfg_t>
		void max_power(syscfg_t* syscfg)
		{
			D3CR_VOS::set(D3CR, 3);
			std::atomic_thread_fence(std::memory_order_seq_cst);

			while(!D3CR_VOSRDY::get(D3CR))
				;
			while(!CSR1_ACTVOSRDY::get(CSR1))
				;

			syscfg->enable_od();

			std::atomic_thread_fence(std::memory_order_seq_cst);

			while(!D3CR_VOSRDY::get(D3CR))
				;
			while(!CSR1_ACTVOSRDY::get(CSR1))
				;
		}

		void enable_usb33() { CR3_USB33DEN::set(CR3); }

		void enable_bd() { CR1_DBP::set(CR1); }
	};

	static_assert(offsetof(reg_h7_pwr, CPUCR) == 0x10);
	static_assert(offsetof(reg_h7_pwr, D3CR) == 0x18);
	static_assert(offsetof(reg_h7_pwr, WKUPCR) == 0x20);
	static_assert(offsetof(reg_h7_pwr, WKUPEPR) == 0x28);
	static_assert(sizeof(reg_h7_pwr) == 0x2C);
} // namespace lac
