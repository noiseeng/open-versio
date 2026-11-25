/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <mcu/stm/h7/adr_h7.h>
#include <utl/bug.h>

namespace lac
{

	struct reg_cm7_scb;
	struct reg_v7m_coredebug;
	struct reg_h750_syscfg;
	struct reg_h750_pwr;

	struct adr_h750 : adr_h7
	{
		const static uint32_t CRYP		  = D2_AHB2PERIPH + 0x1000;
		const static uint32_t HASH		  = D2_AHB2PERIPH + 0x1400;
		const static uint32_t HASH_DIGEST = D2_AHB2PERIPH + 0x1710;

		const static uint32_t FLASH_END = 0x0801FFFF;

		static auto* get_scb() { return (reg_cm7_scb*)SCB; }
		static auto* get_coredebug() { return (reg_v7m_coredebug*)CoreDebug; }
		static auto* get_syscfg() { return (reg_h750_syscfg*)SYSCFG; }
		static auto* get_pwr() { return (reg_h750_pwr*)PWR; }
	};

	static_assert(adr_h750::FLASH_END == 0x0801FFFF);
	static_assert(adr_h750::HASH == 0x48021400);
	static_assert(adr_h750::RCC == 0x58024400);
	static_assert(adr_h750::DMA1_STREAM0 == 0x40020010);

} // namespace lac
