/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <bug/eabi.h>

namespace lac
{
	template <typename isv_t, typename irq_t>
	constexpr isv_t arm_irq2isv(irq_t irq)
	{
		bug_assert((int)irq + 16 >= 0 && (int)irq + 16 < (int)isv_t::COUNT);
		return (isv_t)((int)irq + 16);
	}
} // namespace lac
