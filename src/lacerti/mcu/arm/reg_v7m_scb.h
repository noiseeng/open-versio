/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "mcu_arm_v7m.h"
#include <cstddef>
#include <cstdint>
#include <utl/bitarray.h>
#include <utl/bitfield.h>
#include <utl/bitflag.h>

namespace lac
{
	struct reg_v7m_scb
	{
		const volatile uint32_t CPUID;
		volatile uint32_t		ICSR;
		volatile uint32_t		VTOR;
		volatile uint32_t		AIRCR;
		volatile uint32_t		SCR;
		volatile uint32_t		CCR;
		volatile uint8_t		SHPR[12U];
		volatile uint32_t		SHCSR;
		volatile uint32_t		CFSR;
		volatile uint32_t		HFSR;
		volatile uint32_t		DFSR;
		volatile uint32_t		MMFAR;
		volatile uint32_t		BFAR;
		volatile uint32_t		AFSR;
		const volatile uint32_t ID_PFR[2U];
		const volatile uint32_t ID_DFR;
		const volatile uint32_t ID_AFR;
		const volatile uint32_t ID_MFR[4U];
		const volatile uint32_t ID_ISAR[5U];
		const volatile uint32_t RESERVED0[1U];
		const volatile uint32_t CLIDR;
		const volatile uint32_t CTR;
		const volatile uint32_t CCSIDR;
		volatile uint32_t		CSSELR;
		volatile uint32_t		CPACR;

		using CPUID_REVISION	 = bitfield<0, 4>;
		using CPUID_PARTNO		 = bitfield<4, 12>;
		using CPUID_ARCHITECTURE = bitfield<16, 4>;
		using CPUID_VARIANT		 = bitfield<20, 4>;
		using CPUID_IMPLEMENTER	 = bitfield<24, 8>;

		using ICSR_VECTACTIVE  = bitfield<0, 9>;
		using ICSR_RETTOBASE   = bitflag<11>;
		using ICSR_VECTPENDING = bitfield<12, 9>;
		using ICSR_ISRPENDING  = bitflag<22>;
		using ICSR_ISRPREEMPT  = bitflag<23>;
		using ICSR_PENDSTCLR   = bitflag<25>;
		using ICSR_PENDSTSET   = bitflag<26>;
		using ICSR_PENDSVCLR   = bitflag<27>;
		using ICSR_PENDSVSET   = bitflag<28>;
		using ICSR_NMIPENDSET  = bitflag<31>;

		using VTOR_TBLOFF = bitfield<7, 25>;

		using AIRCR_VECTRESET	  = bitflag<0>;
		using AIRCR_VECTCLRACTIVE = bitflag<1>;
		using AIRCR_SYSRESETREQ	  = bitflag<2>;
		using AIRCR_PRIGROUP	  = bitfield<8, 3>;
		using AIRCR_ENDIANNESS	  = bitflag<15>;
		using AIRCR_VECTKEY		  = bitfield<16, 16>;
		using AIRCR_VECTKEYSTAT	  = bitfield<16, 16>;

		using SCR_SLEEPONEXIT = bitflag<1>;
		using SCR_SLEEPDEEP	  = bitflag<2>;
		using SCR_SEVONPEND	  = bitflag<4>;

		using CCR_NONBASETHRDENA = bitflag<0>;
		using CCR_USERSETMPEND	 = bitflag<1>;
		using CCR_UNALIGN_TRP	 = bitflag<3>;
		using CCR_DIV_0_TRP		 = bitflag<4>;
		using CCR_BFHFNMIGN		 = bitflag<8>;
		using CCR_STKALIGN		 = bitflag<9>;

		using SHCSR_MEMFAULTACT	   = bitflag<0>;
		using SHCSR_BUSFAULTACT	   = bitflag<1>;
		using SHCSR_USGFAULTACT	   = bitflag<3>;
		using SHCSR_SVCALLACT	   = bitflag<7>;
		using SHCSR_MONITORACT	   = bitflag<8>;
		using SHCSR_PENDSVACT	   = bitflag<10>;
		using SHCSR_SYSTICKACT	   = bitflag<11>;
		using SHCSR_USGFAULTPENDED = bitflag<12>;
		using SHCSR_MEMFAULTPENDED = bitflag<13>;
		using SHCSR_BUSFAULTPENDED = bitflag<14>;
		using SHCSR_SVCALLPENDED   = bitflag<15>;
		using SHCSR_MEMFAULTENA	   = bitflag<16>;
		using SHCSR_BUSFAULTENA	   = bitflag<17>;
		using SHCSR_USGFAULTENA	   = bitflag<18>;

		using CFSR_MEMFAULTSR = bitfield<0, 8>;
		using CFSR_BUSFAULTSR = bitfield<8, 8>;
		using CFSR_USGFAULTSR = bitfield<16, 16>;

		using CFSR_IACCVIOL	 = bitflag<0>;
		using CFSR_DACCVIOL	 = bitflag<1>;
		using CFSR_MUNSTKERR = bitflag<3>;
		using CFSR_MSTKERR	 = bitflag<4>;
		using CFSR_MLSPERR	 = bitflag<5>;
		using CFSR_MMARVALID = bitflag<7>;

		using CFSR_IBUSERR	   = bitflag<8>;
		using CFSR_PRECISERR   = bitflag<9>;
		using CFSR_IMPRECISERR = bitflag<10>;
		using CFSR_UNSTKERR	   = bitflag<11>;
		using CFSR_STKERR	   = bitflag<12>;
		using CFSR_LSPERR	   = bitflag<13>;
		using CFSR_BFARVALID   = bitflag<15>;

		using CFSR_UNDEFINSTR = bitflag<16>;
		using CFSR_INVSTATE	  = bitflag<17>;
		using CFSR_INVPC	  = bitflag<18>;
		using CFSR_NOCP		  = bitflag<19>;
		using CFSR_UNALIGNED  = bitflag<24>;
		using CFSR_DIVBYZERO  = bitflag<25>;

		using HFSR_VECTTBL	= bitflag<1>;
		using HFSR_FORCED	= bitflag<30>;
		using HFSR_DEBUGEVT = bitflag<31>;

		using DFSR_HALTED	= bitflag<0>;
		using DFSR_BKPT		= bitflag<1>;
		using DFSR_DWTTRAP	= bitflag<2>;
		using DFSR_VCATCH	= bitflag<3>;
		using DFSR_EXTERNAL = bitflag<4>;

		using CPACR_CP = bitarray<12, 2>;

		uint32_t get_priority_grouping() const { return AIRCR_PRIGROUP::get(AIRCR); }

		void set_priority_grouping(uint32_t g)
		{
			uint32_t key = AIRCR_VECTKEY::val(0x5FA);
			uint32_t grp = AIRCR_PRIGROUP::val(g & 7);
			AIRCR		 = key | grp;
		}

		void reset()
		{
			mcu_arm_v7m::dsb();

			uint32_t key = AIRCR_VECTKEY::val(0x5FA);
			uint32_t val = AIRCR_SYSRESETREQ::BIT;
			uint32_t set = key | val;

			AIRCR = set;

			mcu_arm_v7m::dsb();

			for(;;) {}
		}

		void enable_fpu()
		{
			CPACR = 0xf00000;
			mcu_arm_v7m::dsb();
			mcu_arm_v7m::isb();
		}
	};

	static_assert(offsetof(reg_v7m_scb, CPUID) == 0x00, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, ICSR) == 0x04, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, VTOR) == 0x08, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, AIRCR) == 0x0C, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, SCR) == 0x10, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, CCR) == 0x14, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, SHPR) == 0x18, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, SHCSR) == 0x24, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, CFSR) == 0x28, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, HFSR) == 0x2C, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, DFSR) == 0x30, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, MMFAR) == 0x34, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, BFAR) == 0x38, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, AFSR) == 0x3C, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, ID_PFR) == 0x40, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, ID_DFR) == 0x48, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, ID_AFR) == 0x4C, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, ID_MFR) == 0x50, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, ID_ISAR) == 0x60, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, CLIDR) == 0x78, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, CTR) == 0x7C, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, CCSIDR) == 0x80, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, CSSELR) == 0x84, "invalid offset");
	static_assert(offsetof(reg_v7m_scb, CPACR) == 0x88, "invalid offset");
	static_assert(sizeof(reg_v7m_scb) == 0x8C, "invalid size");

} // namespace lac
