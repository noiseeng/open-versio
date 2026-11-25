/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <mcu/stm/h7/reg_h7_syscfg.h>
#include <utl/bitfield.h>

namespace lac
{
	struct reg_h750_syscfg : reg_h7_syscfg
	{
		using UR2_BOOT_ADD0 = bitfield<16, 16>;
		using UR3_BOOT_ADD1 = bitfield<16, 16>;
	};

	static_assert(sizeof(reg_h750_syscfg) == sizeof(reg_h7_syscfg));
	static_assert(reg_h750_syscfg::PWRCR_ODEN::BIT == 0x1);
	static_assert(reg_h750_syscfg::CCCSR_EN::BIT == 0x1);
	static_assert(reg_h750_syscfg::UR0_RDP::MSK == 0x00FF0000);

} // namespace lac