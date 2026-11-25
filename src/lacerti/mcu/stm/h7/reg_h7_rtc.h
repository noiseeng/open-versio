/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>

namespace lac
{
	struct reg_h7_rtc
	{
		volatile uint32_t		TR;
		volatile uint32_t		DR;
		volatile uint32_t		CR;
		volatile uint32_t		ISR;
		volatile uint32_t		PRER;
		volatile uint32_t		WUTR;
		const volatile uint32_t RESERVED0;
		volatile uint32_t		ALRMAR;
		volatile uint32_t		ALRMBR;
		volatile uint32_t		WPR;
		volatile uint32_t		SSR;
		volatile uint32_t		SHIFTR;
		volatile uint32_t		TSTR;
		volatile uint32_t		TSDR;
		volatile uint32_t		TSSSR;
		volatile uint32_t		CALR;
		volatile uint32_t		TAMPCR;
		volatile uint32_t		ALRMASSR;
		volatile uint32_t		ALRMBSSR;
		volatile uint32_t		OR;
		volatile uint32_t		BKPR[32];

		volatile uint32_t *get_bkp(uint32_t n) { return &BKPR[n]; }
	};
} // namespace lac
