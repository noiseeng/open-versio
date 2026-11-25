/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <mcu/stm/stm32/per_gpio.h>
#include <stdint.h>
#include <utl/bits.h>

namespace lac
{
	template <typename gpio_led_r, typename gpio_led_g, typename gpio_led_b, bool off_state = false>
	struct dev_led_rgb
	{
		static_assert(gpio_is_out<gpio_led_r>(), "red pin is not declared as an output");
		static_assert(gpio_is_out<gpio_led_g>(), "green pin is not declared as an output");
		static_assert(gpio_is_out<gpio_led_b>(), "blue pin is not declared as an output");

		[[no_unique_address]] gpio_led_r red;
		[[no_unique_address]] gpio_led_g green;
		[[no_unique_address]] gpio_led_b blue;

		uint32_t state = 0;

		dev_led_rgb()
		{
			red.set_low_speed();
			green.set_low_speed();
			blue.set_low_speed();
			clear();
		}

		void set_red(bool s = true)
		{
			red.set(s != off_state);
			update(BIT0, s);
		}

		void set_green(bool s = true)
		{
			green.set(s != off_state);
			update(BIT1, s);
		}

		void set_blue(bool s = true)
		{
			blue.set(s != off_state);
			update(BIT2, s);
		}

		void clr_red() { set_red(false); }

		void clr_green() { set_green(false); }

		void clr_blue() { set_blue(false); }

		void set_bits(uint32_t bits)
		{
			set_red((bits & BIT0) != 0);
			set_green((bits & BIT1) != 0);
			set_blue((bits & BIT2) != 0);
		}

		void set(bool b) { set_bits(b ? BIT0 | BIT1 | BIT2 : 0); }

		void set_white() { set(true); }

		void clear() { set(false); }

		void operator=(uint32_t bits)
		{
			const uint32_t changed = bits ^ state;

			if(changed & BIT0)
			{
				set_red((bits & BIT0) != 0);
			}

			if(changed & BIT1)
			{
				set_green((bits & BIT1) != 0);
			}

			if(changed & BIT2)
			{
				set_blue((bits & BIT2) != 0);
			}
		}

		void index(uint32_t i)
		{
			set_red(i != 0);
			set_green(i != 1);
			set_blue(i != 2);
		}

		void update(uint32_t bit, bool s) { state = s ? (state | bit) : (state & ~bit); }
	};

} // namespace lac
