/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <bug/compiler.h>
#include <cstdint>
#include <mcu/linker.h>
#include <mcu/stm/h7/mcu_h7_boot_vectors.h>

namespace lac
{

	__attribute__((section(".flash_boot"), used)) static void flash_fault_spin()
	{
		for(;;)
		{
			bug_trap();
		}
	}

	__attribute__((always_inline)) static inline void flash_copy_sram_image()
	{
		volatile uint32_t		*dst = &_ssram;
		const volatile uint32_t *src = &_sram_image_lma;
		while(dst < &_esram)
		{
			*dst++ = *src++;
		}
	}

} // namespace lac
