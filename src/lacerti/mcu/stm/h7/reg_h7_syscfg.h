/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstddef>
#include <cstdint>
#include <utl/bitarray.h>
#include <utl/bitfield.h>
#include <utl/bitflag.h>

namespace lac
{
	struct reg_h7_syscfg
	{
		uint32_t		  RESERVED1;
		volatile uint32_t PMCR;
		volatile uint32_t EXTICR[4];
		volatile uint32_t CFGR;
		uint32_t		  RESERVED2;
		volatile uint32_t CCCSR;
		volatile uint32_t CCVR;
		volatile uint32_t CCCR;
		volatile uint32_t PWRCR;
		uint32_t		  RESERVED3[61];
		volatile uint32_t PKGR;
		uint32_t		  RESERVED4[118];
		volatile uint32_t UR0;
		volatile uint32_t UR1;
		volatile uint32_t UR2;
		volatile uint32_t UR3;
		volatile uint32_t UR4;
		volatile uint32_t UR5;
		volatile uint32_t UR6;
		volatile uint32_t UR7;
		volatile uint32_t UR8;
		volatile uint32_t UR9;
		volatile uint32_t UR10;
		volatile uint32_t UR11;
		volatile uint32_t UR12;
		volatile uint32_t UR13;
		volatile uint32_t UR14;
		volatile uint32_t UR15;
		volatile uint32_t UR16;
		volatile uint32_t UR17;

		using PMCR_I2C1FMP	   = bitflag<0>;
		using PMCR_I2C2FMP	   = bitflag<1>;
		using PMCR_I2C3FMP	   = bitflag<2>;
		using PMCR_I2C4FMP	   = bitflag<3>;
		using PMCR_PB6FMP	   = bitflag<4>;
		using PMCR_PB7FMP	   = bitflag<5>;
		using PMCR_PB8FMP	   = bitflag<6>;
		using PMCR_PB9FMP	   = bitflag<7>;
		using PMCR_BOOSTE	   = bitflag<8>;
		using PMCR_BOOSTVDDSEL = bitflag<9>;
		using PMCR_EPIS		   = bitfield<21, 3>;
		using PMCR_PA0SO	   = bitflag<24>;
		using PMCR_PA1SO	   = bitflag<25>;
		using PMCR_PC2SO	   = bitflag<26>;
		using PMCR_PC3SO	   = bitflag<27>;

		using EXTICR_EXTI = bitarray<4, 4, 0>;

		using CFGR_PVDL		= bitflag<2>;
		using CFGR_FLASHL	= bitflag<3>;
		using CFGR_CM7L		= bitflag<6>;
		using CFGR_BKRAML	= bitflag<7>;
		using CFGR_SRAM4L	= bitflag<9>;
		using CFGR_SRAM3L	= bitflag<10>;
		using CFGR_SRAM2L	= bitflag<11>;
		using CFGR_SRAM1L	= bitflag<12>;
		using CFGR_DTCML	= bitflag<13>;
		using CFGR_ITCML	= bitflag<14>;
		using CFGR_AXISRAML = bitflag<15>;

		using CCCSR_EN	  = bitflag<0>;
		using CCCSR_CS	  = bitflag<1>;
		using CCCSR_READY = bitflag<8>;
		using CCCSR_HSLV  = bitflag<16>;

		using CCVR_NCV = bitfield<0, 4>;
		using CCVR_PCV = bitfield<4, 4>;

		using CCCR_NCC = bitfield<0, 4>;
		using CCCR_PCC = bitfield<4, 4>;

		using PWRCR_ODEN = bitflag<0>;

		using PKGR_PKG = bitfield<0, 4>;

		using UR0_BKS = bitflag<0>;
		using UR0_RDP = bitfield<16, 8>;

		using UR2_BORH = bitfield<0, 2>;

		using UR4_MEPAD_BANK1 = bitflag<16>;

		using UR5_MESAD_BANK1 = bitflag<0>;
		using UR5_WRPN_BANK1  = bitfield<16, 8>;

		using UR6_PABEG_BANK1 = bitfield<0, 12>;
		using UR6_PAEND_BANK1 = bitfield<16, 12>;

		using UR7_SABEG_BANK1 = bitfield<0, 12>;
		using UR7_SAEND_BANK1 = bitfield<16, 12>;

		using UR8_MEPAD_BANK2 = bitflag<0>;
		using UR8_MESAD_BANK2 = bitflag<16>;

		using UR9_WRPN_BANK2  = bitfield<0, 8>;
		using UR9_PABEG_BANK2 = bitfield<16, 12>;

		using UR10_PAEND_BANK2 = bitfield<0, 12>;
		using UR10_SABEG_BANK2 = bitfield<16, 12>;

		using UR11_SAEND_BANK2 = bitfield<0, 12>;
		using UR11_IWDG1M	   = bitflag<16>;

		using UR12_SECURE = bitflag<16>;

		using UR13_SDRS	   = bitfield<0, 2>;
		using UR13_D1SBRST = bitflag<16>;

		using UR14_D1STPRST = bitflag<0>;

		using UR15_FZIWDGSTB = bitflag<16>;

		using UR16_FZIWDGSTP = bitflag<0>;
		using UR16_PKP		 = bitflag<16>;

		using UR17_IOHSLV = bitflag<0>;

		void enable_cccsr() { CCCSR_EN::set(CCCSR); }
		void enable_od() { PWRCR_ODEN::set(PWRCR); }
	};

	static_assert(offsetof(reg_h7_syscfg, PMCR) == 0x04);
	static_assert(offsetof(reg_h7_syscfg, CFGR) == 0x18);
	static_assert(offsetof(reg_h7_syscfg, CCCSR) == 0x20);
	static_assert(offsetof(reg_h7_syscfg, PWRCR) == 0x2C);
	static_assert(offsetof(reg_h7_syscfg, PKGR) == 0x124);
	static_assert(offsetof(reg_h7_syscfg, UR0) == 0x300);
	static_assert(offsetof(reg_h7_syscfg, UR17) == 0x344);
	static_assert(sizeof(reg_h7_syscfg) == 0x348);

} // namespace lac
