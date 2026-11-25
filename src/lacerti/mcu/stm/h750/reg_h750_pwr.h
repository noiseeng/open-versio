/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <mcu/stm/h7/reg_h7_pwr.h>
#include <utl/bitflag.h>

namespace lac
{
	struct reg_h750_pwr : reg_h7_pwr
	{
		using CR3_SCUEN = bitflag<2>;

		void enable_ldo()
		{
			CR3_LDOEN::set(CR3);
			while(!CSR1_ACTVOSRDY::get(CSR1))
				;
		}
	};

	static_assert(sizeof(reg_h750_pwr) == sizeof(reg_h7_pwr));
	static_assert(reg_h750_pwr::CR1_DBP::BIT == 0x100);
	static_assert(reg_h750_pwr::CR3_LDOEN::BIT == 0x2);
	static_assert(reg_h750_pwr::CR3_SCUEN::BIT == 0x4);
	static_assert(reg_h750_pwr::CR3_USB33DEN::BIT == 0x1000000);
	static_assert(reg_h750_pwr::CSR1_ACTVOSRDY::BIT == 0x2000);
	static_assert(reg_h750_pwr::D3CR_VOSRDY::BIT == 0x2000);
	static_assert(reg_h750_pwr::D3CR_VOS::MSK == 0xC000);
} // namespace lac