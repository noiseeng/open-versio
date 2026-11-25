/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <dev/dev_asc4c16.h>
#include <mcu/stm/h7/per_h7_gpio.h>
#include <mcu/stm/h750/adr_h750.h>
#include <mcu/stm/stm32/per_gpio.h>

namespace lac
{
	using brd_es_dfm_sdram_pins = dev_asc4c16_pins<gpio_bit<adr_h750::GPIOD, 14, 12>,
												   gpio_bit<adr_h750::GPIOD, 15, 12>,
												   gpio_bit<adr_h750::GPIOD, 0, 12>,
												   gpio_bit<adr_h750::GPIOD, 1, 12>,
												   gpio_bit<adr_h750::GPIOE, 7, 12>,
												   gpio_bit<adr_h750::GPIOE, 8, 12>,
												   gpio_bit<adr_h750::GPIOE, 9, 12>,
												   gpio_bit<adr_h750::GPIOE, 10, 12>,
												   gpio_bit<adr_h750::GPIOE, 11, 12>,
												   gpio_bit<adr_h750::GPIOE, 12, 12>,
												   gpio_bit<adr_h750::GPIOE, 13, 12>,
												   gpio_bit<adr_h750::GPIOE, 14, 12>,
												   gpio_bit<adr_h750::GPIOE, 15, 12>,
												   gpio_bit<adr_h750::GPIOD, 8, 12>,
												   gpio_bit<adr_h750::GPIOD, 9, 12>,
												   gpio_bit<adr_h750::GPIOD, 10, 12>,
												   gpio_bit<adr_h750::GPIOH, 8, 12>,
												   gpio_bit<adr_h750::GPIOH, 9, 12>,
												   gpio_bit<adr_h750::GPIOH, 10, 12>,
												   gpio_bit<adr_h750::GPIOH, 11, 12>,
												   gpio_bit<adr_h750::GPIOH, 12, 12>,
												   gpio_bit<adr_h750::GPIOH, 13, 12>,
												   gpio_bit<adr_h750::GPIOH, 14, 12>,
												   gpio_bit<adr_h750::GPIOH, 15, 12>,
												   gpio_bit<adr_h750::GPIOI, 0, 12>,
												   gpio_bit<adr_h750::GPIOI, 1, 12>,
												   gpio_bit<adr_h750::GPIOI, 2, 12>,
												   gpio_bit<adr_h750::GPIOI, 3, 12>,
												   gpio_bit<adr_h750::GPIOI, 6, 12>,
												   gpio_bit<adr_h750::GPIOI, 7, 12>,
												   gpio_bit<adr_h750::GPIOI, 9, 12>,
												   gpio_bit<adr_h750::GPIOI, 10, 12>,
												   gpio_bit<adr_h750::GPIOF, 0, 12>,
												   gpio_bit<adr_h750::GPIOF, 1, 12>,
												   gpio_bit<adr_h750::GPIOF, 2, 12>,
												   gpio_bit<adr_h750::GPIOF, 3, 12>,
												   gpio_bit<adr_h750::GPIOF, 4, 12>,
												   gpio_bit<adr_h750::GPIOF, 5, 12>,
												   gpio_bit<adr_h750::GPIOF, 12, 12>,
												   gpio_bit<adr_h750::GPIOF, 13, 12>,
												   gpio_bit<adr_h750::GPIOF, 14, 12>,
												   gpio_bit<adr_h750::GPIOF, 15, 12>,
												   gpio_bit<adr_h750::GPIOG, 0, 12>,
												   gpio_bit<adr_h750::GPIOG, 1, 12>,
												   gpio_bit<adr_h750::GPIOG, 2, 12>,
												   gpio_bit<adr_h750::GPIOG, 4, 12>,
												   gpio_bit<adr_h750::GPIOG, 5, 12>,
												   gpio_bit<adr_h750::GPIOE, 0, 12>,
												   gpio_bit<adr_h750::GPIOE, 1, 12>,
												   gpio_bit<adr_h750::GPIOI, 4, 12>,
												   gpio_bit<adr_h750::GPIOI, 5, 12>,
												   gpio_bit<adr_h750::GPIOG, 15, 12>,
												   gpio_bit<adr_h750::GPIOF, 11, 12>,
												   gpio_bit<adr_h750::GPIOH, 5, 12>,
												   gpio_bit<adr_h750::GPIOH, 2, 12>,
												   gpio_bit<adr_h750::GPIOH, 3, 12>,
												   gpio_bit<adr_h750::GPIOG, 8, 12>>;

	template <typename mcu_t>
	using brd_es_dfm_sdram
		= dev_asc4c16<mcu_t, adr_h750::FMC_BANK5_6_R, 64 * 1024 * 1024, brd_es_dfm_sdram_pins, typename mcu_t::tim2_t>;
} // namespace lac
