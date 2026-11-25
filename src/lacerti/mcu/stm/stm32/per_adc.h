/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "reg_gpio.h"
#include <cstdint>
#include <utl/bug.h>

#define ADC_PIN(X) {X::PORT, X::BIT, X::ADCIDX}

namespace lac
{
	template <typename gpio_type, uint32_t adcidx>
	struct adc_pin
	{
		const static uint32_t PORT	 = gpio_type::PORT;
		const static uint32_t BIT	 = gpio_type::BIT;
		const static uint32_t ADCIDX = adcidx;
	};

	struct adc_pin_config
	{
		uint32_t port;
		uint32_t bit;
		uint32_t adc;

		uint32_t init() const
		{
			lac_assert(port);
			reg_gpio* gpio = (reg_gpio*)port;
			gpio->init(bit, reg_gpio::MODE_ANALOG, reg_gpio::OTYPE_PP, reg_gpio::PUPDR_NOPULL);
			return 1 << adc;
		}
	};

} // namespace lac