/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <stdint.h>

namespace lac
{
	struct per_h7_flash_geometry
	{
		static constexpr uint32_t SECTOR_SIZE = 0x20000;
		static constexpr uint32_t BANK1_BASE  = 0x08000000;
		static constexpr uint32_t BANK2_BASE  = 0x08100000;

		static constexpr uint32_t bank_of(uint32_t addr) { return (addr >= BANK2_BASE) ? 2 : 1; }

		static constexpr uint32_t sector_of(uint32_t addr)
		{ return (addr - ((addr >= BANK2_BASE) ? BANK2_BASE : BANK1_BASE)) / SECTOR_SIZE; }
	};
} // namespace lac
