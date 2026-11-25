/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace lac
{
	template <typename arm_t>
	struct scope_disable_irq
	{
		const uint32_t saved = arm_t::irq_save();

		scope_disable_irq() = default;
		~scope_disable_irq() { arm_t::irq_restore(saved); }

		scope_disable_irq(const scope_disable_irq&)			   = delete;
		scope_disable_irq& operator=(const scope_disable_irq&) = delete;
	};
} // namespace lac
