/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <utl/bitfield.h>
#include <utl/bitflag.h>

namespace lac
{
	struct reg_v7m_coredebug
	{
		volatile uint32_t DHCSR;
		volatile uint32_t DCRSR;
		volatile uint32_t DCRDR;
		volatile uint32_t DEMCR;

		using DHCSR_C_DEBUGEN	= bitflag<0>;
		using DHCSR_C_HALT		= bitflag<1>;
		using DHCSR_C_STEP		= bitflag<2>;
		using DHCSR_C_MASKINTS	= bitflag<3>;
		using DHCSR_C_SNAPSTALL = bitflag<5>;
		using DHCSR_DBGKEY		= bitfield<16, 16>;
		using DHCSR_S_REGRDY	= bitflag<16>;
		using DHCSR_S_HALT		= bitflag<17>;
		using DHCSR_S_SLEEP		= bitflag<18>;
		using DHCSR_S_LOCKUP	= bitflag<19>;
		using DHCSR_S_RETIRE_ST = bitflag<24>;
		using DHCSR_S_RESET_ST	= bitflag<25>;

		using DEMCR_VC_CORERESET = bitflag<0>;
		using DEMCR_MON_EN		 = bitflag<16>;
		using DEMCR_TRCENA		 = bitflag<24>;

		bool debugger_attached() const { return DHCSR_C_DEBUGEN::get(DHCSR) != 0; }
	};
} // namespace lac
