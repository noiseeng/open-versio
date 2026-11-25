/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <bug/compiler.h>
#include <cstdint>

#include "mcu_arm_v6.h"

namespace lac
{
	struct mcu_arm_v6m : mcu_arm_v6
	{
		static bug_force_inline void dsb() { asm volatile("dsb 0xF" ::: "memory"); }
		static bug_force_inline void dmb() { asm volatile("dmb 0xF" ::: "memory"); }
		static bug_force_inline void isb() { asm volatile("isb 0xF" ::: "memory"); }

		static bug_force_inline uint32_t irq_save()
		{
			uint32_t v;
			asm volatile("mrs %0, primask\n\tcpsid i" : "=r"(v) : : "memory");
			return v;
		}

		static bug_force_inline void irq_restore(uint32_t v) { asm volatile("msr primask, %0" : : "r"(v) : "memory"); }
	};
} // namespace lac