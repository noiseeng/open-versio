/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

#include <mcu/stm/stm32/per_gpio.h>
#include <mcu/stm/stm32/reg_gpio.h>

namespace lac
{
	struct gpio_word
	{
		uint32_t mask = 0;
		uint32_t val  = 0;

		constexpr void add(uint32_t index, uint32_t width, uint32_t v)
		{
			mask |= ((1u << width) - 1) << (index * width);
			val |= v << (index * width);
		}

		void apply(volatile uint32_t& reg) const { reg = (reg & ~mask) | val; }
	};

	struct gpio_port_init
	{
		uint32_t  port = 0;
		gpio_word moder;
		gpio_word otyper;
		gpio_word ospeedr;
		gpio_word pupdr;
		gpio_word afr_0;
		gpio_word afr_1;

		template <typename pin_t>
		constexpr void add()
		{
			const auto f = gpio_fields::get(pin_t::MODE, pin_t::is_alt());

			moder.add(pin_t::BIT, 2, f.mode);
			otyper.add(pin_t::BIT, 1, f.otype);
			ospeedr.add(pin_t::BIT, 2, pin_t::POWER);
			pupdr.add(pin_t::BIT, 2, f.pupd);

			if(pin_t::is_alt())
			{
				(pin_t::BIT < 8 ? afr_0 : afr_1).add(pin_t::BIT % 8, 4, pin_t::ALT);
			}
		}

		void apply(reg_gpio* p) const
		{
			afr_0.apply(p->AFR_0);
			afr_1.apply(p->AFR_1);
			otyper.apply(p->OTYPER);
			ospeedr.apply(p->OSPEEDR);
			pupdr.apply(p->PUPDR);
			moder.apply(p->MODER);
		}

		[[gnu::noinline, gnu::cold]] static void apply_all(const gpio_port_init* rows, size_t count)
		{
			for(size_t i = 0; i < count; i++)
			{
				rows[i].apply((reg_gpio*)(uintptr_t)rows[i].port);
			}
		}
	};

	template <typename pin_t>
	constexpr bool gpio_mode_known()
	{
		if constexpr(!pin_t::is_valid())
		{
			return true;
		}
		else if constexpr(pin_t::MODE == GPIO_MODE_NONE || pin_t::MODE == GPIO_MODE_OD || pin_t::MODE == GPIO_MODE_AF_PU)
		{
			return pin_t::is_alt();
		}
		else
		{
			return true;
		}
	}

	template <typename... pins_t>
	struct gpio_list
	{
		static_assert((gpio_mode_known<pins_t>() && ...),
					  "pin has no direction: use gpi_/gpipu_/gpipd_/gpo_ or an alternate function pin");

		struct build_t
		{
			std::array<gpio_port_init, sizeof...(pins_t)> rows{};
			size_t										  count	   = 0;
			bool										  distinct = true;

			template <typename pin_t>
			constexpr void add()
			{
				if constexpr(pin_t::is_valid())
				{
					size_t i = 0;
					while(i < count && rows[i].port != pin_t::PORT)
					{
						i++;
					}
					if(i == count)
					{
						rows[count++].port = pin_t::PORT;
					}
					distinct = distinct && !(rows[i].otyper.mask & pin_t::PIN);
					rows[i].template add<pin_t>();
				}
			}
		};

		static constexpr build_t build()
		{
			build_t b;
			(b.template add<pins_t>(), ...);
			return b;
		}

		static constexpr size_t COUNT = build().count;
		static_assert(build().distinct, "pin listed twice");

		static constexpr auto ROWS = [] {
			std::array<gpio_port_init, COUNT> r{};
			for(size_t i = 0; i < COUNT; i++)
			{
				r[i] = build().rows[i];
			}
			return r;
		}();

		static void init() { gpio_port_init::apply_all(ROWS.data(), COUNT); }
	};
} // namespace lac
