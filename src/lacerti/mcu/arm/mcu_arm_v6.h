/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <bug/compiler.h>

#include "mcu_arm_v5.h"

namespace lac
{
	struct mcu_arm_v6 : mcu_arm_v5
	{
		static bug_force_inline void enable_irq() { asm volatile("cpsie i"); }
		static bug_force_inline void disable_irq() { asm volatile("cpsid i"); }
	};
} // namespace lac