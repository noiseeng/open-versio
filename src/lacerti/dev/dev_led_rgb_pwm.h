/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "dev_pwm.h"
#include <numerus/color.h>
#include <stdint.h>
#include <utl/bits.h>

namespace lac
{
	template <typename rgb_led, uint32_t PWM_BITS = 5>
	struct dev_led_rgb_pwm
	{
		rgb_led led;

		dev_pwm<PWM_BITS> r;
		dev_pwm<PWM_BITS> g;
		dev_pwm<PWM_BITS> b;

		dev_led_rgb_pwm() {}

		void operator()()
		{
			bool r_on = r();
			led		  = flags(r_on, g(r.count), b(r.count));
		}

		void operator()(uint32_t clock) { led = flags(r(clock), g(clock), b(clock)); }

		void update()
		{
			r.update();
			g.update();
			b.update();
		}

		template <typename color_t>
		void operator=(const color_t& c)
		{
			r = (uint32_t)c.a * c.r;
			g = (uint32_t)c.a * c.g;
			b = (uint32_t)c.a * c.b;
		}

		void set_solid(const numerus::color& c) { led.set_bits(flags(c.r >= 0x80, c.g >= 0x80, c.b >= 0x80)); }

		void clear() { *this = numerus::color::black(); }

		static constexpr uint32_t flags(bool r_on, bool g_on, bool b_on)
		{ return (r_on ? BIT0 : 0) | (g_on ? BIT1 : 0) | (b_on ? BIT2 : 0); }
	};

} // namespace lac
