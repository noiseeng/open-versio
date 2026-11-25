/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <bug/compiler.h>
#include <cstdint>

namespace lac
{
	struct mcu_arm_vfp
	{
		static bug_force_inline void enable_ftz()
		{
			uint32_t result;
			__asm volatile("VMRS %0, fpscr" : "=r"(result));
			result |= (1 << 24);
#if defined(__clang__)
			__asm volatile("VMSR fpscr, %0" : : "r"(result) : "memory");
#else
			__asm volatile("VMSR fpscr, %0" : : "r"(result) : "vfpcc", "memory");
#endif
		}
	};

} // namespace lac
