/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <utl/bug.h>

#include "reg_gpio.h"

namespace lac
{
	struct gpio_fields
	{
		uint32_t mode;
		uint32_t otype;
		uint32_t pupd;

		static constexpr gpio_fields get(gpio_mode mode, bool alt)
		{
			const uint32_t in  = alt ? reg_gpio::MODE_AF : reg_gpio::MODE_INPUT;
			const uint32_t out = alt ? reg_gpio::MODE_AF : reg_gpio::MODE_OUTPUT;

			switch(mode)
			{
				case GPIO_MODE_ANALOG: return {reg_gpio::MODE_ANALOG, reg_gpio::OTYPE_PP, reg_gpio::PUPDR_NOPULL};
				case GPIO_MODE_IN: return {in, reg_gpio::OTYPE_PP, reg_gpio::PUPDR_NOPULL};
				case GPIO_MODE_INPU: return {reg_gpio::MODE_INPUT, reg_gpio::OTYPE_PP, reg_gpio::PUPDR_PULLUP};
				case GPIO_MODE_INPD: return {reg_gpio::MODE_INPUT, reg_gpio::OTYPE_PP, reg_gpio::PUPDR_PULLDOWN};
				case GPIO_MODE_OD: return {reg_gpio::MODE_AF, reg_gpio::OTYPE_OD, reg_gpio::PUPDR_NOPULL};
				case GPIO_MODE_AF_PU: return {reg_gpio::MODE_AF, reg_gpio::OTYPE_PP, reg_gpio::PUPDR_PULLUP};
				case GPIO_MODE_OUT:
				case GPIO_MODE_NONE: break;
			}
			return {out, reg_gpio::OTYPE_PP, reg_gpio::PUPDR_NOPULL};
		}
	};

	template <gpio_mode mode>
	inline void gpio_init(reg_gpio *p, uint32_t bit, gpio_alt alt, gpio_speed power)
	{
		static constexpr uint32_t PIN_MODE = gpio_fields::get(mode, false).mode;
		static constexpr uint32_t ALT_MODE = gpio_fields::get(mode, true).mode;
		static constexpr uint32_t OTYPE	   = gpio_fields::get(mode, false).otype;
		static constexpr uint32_t PUPD	   = gpio_fields::get(mode, false).pupd;

		static_assert(OTYPE == gpio_fields::get(mode, true).otype && PUPD == gpio_fields::get(mode, true).pupd,
					  "alternate function only changes the mode field");

		if constexpr(mode == GPIO_MODE_OD || mode == GPIO_MODE_AF_PU)
		{
			lac_assert(alt < 32);
		}

		if constexpr(PIN_MODE == ALT_MODE)
		{
			p->init(bit, PIN_MODE, OTYPE, PUPD, alt, power);
		}
		else
		{
			p->init(bit, alt == GPIO_NOT_ALT ? PIN_MODE : ALT_MODE, OTYPE, PUPD, alt, power);
		}
	}

	template <uint32_t	 port,
			  uint32_t	 bit,
			  uint32_t	 alt   = GPIO_NOT_ALT,
			  gpio_mode	 mode  = GPIO_MODE_NONE,
			  gpio_speed power = GPIO_PWR_HIGH>
	struct gpio_bit
	{
		static_assert(port != 0, "port not defined on this hardware");
		static_assert(bit <= 15, "bit cannot be > 16");

		constexpr static uint32_t	PORT  = port;
		constexpr static uint32_t	BIT	  = bit;
		constexpr static uint32_t	PIN	  = 1 << bit;
		constexpr static gpio_alt	ALT	  = (gpio_alt)alt;
		constexpr static gpio_speed POWER = power;
		constexpr static gpio_mode	MODE  = mode;
		constexpr static uint32_t	RPIN  = 1 << (bit + 16);

		static_assert(alt == GPIO_NOT_ALT || alt < 32);

		constexpr static bool is_null() { return false; }
		constexpr static bool is_valid() { return true; }
		constexpr static bool is_alt() { return alt != GPIO_NOT_ALT; }

		static reg_gpio *p() { return (reg_gpio *)port; }

		static int get() { return p()->IDR & PIN; }

		static void set() { p()->BSRR = PIN; }

		static void clr() { p()->BSRR = RPIN; }
		static void set(int data) { data ? set() : clr(); }
		static void tog() { set(!get()); }

		static void init_in() { gpio_init<GPIO_MODE_IN>(p(), BIT, ALT, POWER); }
		static void init_analog() { gpio_init<GPIO_MODE_ANALOG>(p(), BIT, ALT, POWER); }
		static void init_inpu() { gpio_init<GPIO_MODE_INPU>(p(), BIT, ALT, POWER); }
		static void init_inpd() { gpio_init<GPIO_MODE_INPD>(p(), BIT, ALT, POWER); }
		static void init_out() { gpio_init<GPIO_MODE_OUT>(p(), BIT, ALT, POWER); }
		static void init_od() { gpio_init<GPIO_MODE_OD>(p(), BIT, ALT, POWER); }

		explicit operator bool() const { return get() != 0; }
		explicit operator uint64_t() const { return get() ? 1 : 0; }

		static void init_af_pu() { gpio_init<GPIO_MODE_AF_PU>(p(), BIT, ALT, POWER); }

		template <gpio_mode type, gpio_speed pwr>
		static void init()
		{
			if constexpr(type != GPIO_MODE_NONE)
			{
				gpio_init<type>(p(), bit, ALT, POWER);
			}

			p()->set_speed(bit, pwr);
		}

		gpio_bit() { init<MODE, POWER>(); }

		static void set_low_speed() { p()->set_speed(bit, GPIO_PWR_LOW); }
		static void set_med_speed() { p()->set_speed(bit, GPIO_PWR_MED); }
		static void set_high_speed() { p()->set_speed(bit, GPIO_PWR_HIGH); }
		static void set_highest_speed() { p()->set_speed(bit, GPIO_PWR_HIGHEST); }
	};

	struct gpio_null
	{
		constexpr static bool is_null() { return true; }
		constexpr static bool is_valid() { return false; }
		constexpr static bool is_alt() { return false; }

		constexpr static int  get() { return 0; }
		constexpr static void set() {}
		constexpr static void clr() {}
		constexpr static void set(int data) {}
		constexpr static void init_in() {}
		constexpr static void init_analog() {}
		constexpr static void init_inpu() {}
		constexpr static void init_inpd() {}
		constexpr static void init_out() {}
		constexpr static void init_od() {}
		constexpr static void init_af_pu() {}
		constexpr static void set_low_speed() {}
	};

	template <typename gpio_t>
	consteval bool gpio_is_out()
	{
		if constexpr(gpio_t::is_null())
		{
			return true;
		}
		else
		{
			return gpio_t::MODE == GPIO_MODE_OUT;
		}
	}

	template <typename a_t, typename b_t>
	constexpr bool gpio_same_pin = a_t::PORT == b_t::PORT && a_t::BIT == b_t::BIT;

	template <typename... pins_t>
	consteval bool gpio_distinct_pins()
	{
		constexpr std::array<uint64_t, sizeof...(pins_t)> keys = {{((uint64_t)pins_t::PORT << 4 | pins_t::BIT)...}};

		for(size_t i = 0; i < keys.size(); i++)
		{
			for(size_t j = i + 1; j < keys.size(); j++)
			{
				if(keys[i] == keys[j])
				{
					return false;
				}
			}
		}

		return true;
	}

} // namespace lac
