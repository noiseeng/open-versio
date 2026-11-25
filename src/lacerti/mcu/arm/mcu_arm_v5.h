/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <bug/compiler.h>

#include <cstdint>

namespace lac
{
	struct mcu_arm_v5
	{
		static bug_force_inline void breakpoint() { __asm__ volatile("bkpt"); }
		static bug_force_inline void set_msp(uint32_t top) { __asm volatile("MSR msp, %0" : : "r"(top) :); }
	};
} // namespace lac