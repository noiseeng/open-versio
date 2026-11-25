/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

#include "per_h7_rcc.h"
#include <utl/bug.h>

namespace lac
{
	template <typename adr_type, typename rcc_type>
	struct mcu_h7_periph
	{
		consteval static uint32_t get_trigger_bit(uint32_t port)
		{
			switch(port)
			{
				case adr_type::TIM1: return 1;
				case adr_type::TIM2: return 2;
				case adr_type::TIM4: return 3;
				case adr_type::TIM5: return 4;
				case adr_type::TIM6: return 5;
				case adr_type::TIM7: return 6;
				case adr_type::TIM8: return 7;
				case adr_type::TIM15: return 8;
				case adr_type::HRTIM1: return 9;
				case adr_type::LPTIM1: return 11;
				case adr_type::LPTIM2: return 12;
				default: lac_assert(false); return 0;
			}
		}

		template <uint32_t base>
		static void enable_clock()
		{
			auto *rcc = adr_type::get_rcc();

			switch(base)
			{
				default: lac_assert(false); [[fallthrough]];
				case adr_type::ADC1: rcc->enable_adc12(); break;
				case adr_type::ADC2: rcc->enable_adc12(); break;
				case adr_type::ADC3: rcc->enable_adc3(); break;
				case adr_type::DMA1: rcc->enable_dma1(); break;
				case adr_type::DMA2: rcc->enable_dma2(); break;
				case adr_type::GPIOA: rcc->enable_gpioa(); break;
				case adr_type::GPIOB: rcc->enable_gpiob(); break;
				case adr_type::GPIOC: rcc->enable_gpioc(); break;
				case adr_type::GPIOD: rcc->enable_gpiod(); break;
				case adr_type::GPIOE: rcc->enable_gpioe(); break;
				case adr_type::GPIOF: rcc->enable_gpiof(); break;
				case adr_type::GPIOG: rcc->enable_gpiog(); break;
				case adr_type::GPIOH: rcc->enable_gpioh(); break;
				case adr_type::GPIOI: rcc->enable_gpioi(); break;
				case adr_type::GPIOJ: rcc->enable_gpioj(); break;
				case adr_type::GPIOK: rcc->enable_gpiok(); break;
				case adr_type::I2C1: rcc->enable_i2c1(); break;
				case adr_type::I2C2: rcc->enable_i2c2(); break;
				case adr_type::I2C3: rcc->enable_i2c3(); break;
				case adr_type::I2C4: rcc->enable_i2c4(); break;
				case adr_type::LPTIM1: rcc->enable_lptim1(); break;
				case adr_type::QSPI: rcc->enable_qspi(); break;
				case adr_type::RNG: rcc->enable_rng(); break;
				case adr_type::SAI1: rcc->enable_sai1(); break;
				case adr_type::SAI2: rcc->enable_sai2(); break;
				case adr_type::SAI3: rcc->enable_sai3(); break;
				case adr_type::SPI1: rcc->enable_spi1(); break;
				case adr_type::SPI2: rcc->enable_spi2(); break;
				case adr_type::SPI3: rcc->enable_spi3(); break;
				case adr_type::SPI4: rcc->enable_spi4(); break;
				case adr_type::SPI5: rcc->enable_spi5(); break;
				case adr_type::SPI6: rcc->enable_spi6(); break;
				case adr_type::TIM1: rcc->enable_tim1(); break;
				case adr_type::TIM12: rcc->enable_tim12(); break;
				case adr_type::TIM13: rcc->enable_tim13(); break;
				case adr_type::TIM14: rcc->enable_tim14(); break;
				case adr_type::TIM15: rcc->enable_tim15(); break;
				case adr_type::TIM16: rcc->enable_tim16(); break;
				case adr_type::TIM17: rcc->enable_tim17(); break;
				case adr_type::TIM2: rcc->enable_tim2(); break;
				case adr_type::TIM3: rcc->enable_tim3(); break;
				case adr_type::TIM4: rcc->enable_tim4(); break;
				case adr_type::TIM5: rcc->enable_tim5(); break;
				case adr_type::TIM6: rcc->enable_tim6(); break;
				case adr_type::TIM7: rcc->enable_tim7(); break;
				case adr_type::TIM8: rcc->enable_tim8(); break;
				case adr_type::UART4: rcc->enable_uart4(); break;
				case adr_type::UART5: rcc->enable_uart5(); break;
				case adr_type::UART7: rcc->enable_uart7(); break;
				case adr_type::UART8: rcc->enable_uart8(); break;
				case adr_type::USART1: rcc->enable_usart1(); break;
				case adr_type::USART2: rcc->enable_usart2(); break;
				case adr_type::USART3: rcc->enable_usart3(); break;
				case adr_type::USART6: rcc->enable_usart6(); break;
			}
		}

		template <uint32_t port>
		static uint32_t get_tim_hz()
		{
			switch(port)
			{
				default: lac_assert(false); [[fallthrough]];
				case adr_type::TIM1: return rcc_type::get_hz_apb2_tim();
				case adr_type::TIM2: return rcc_type::get_hz_apb1_tim();
				case adr_type::TIM3: return rcc_type::get_hz_apb1_tim();
				case adr_type::TIM4: return rcc_type::get_hz_apb1_tim();
				case adr_type::TIM5: return rcc_type::get_hz_apb1_tim();
				case adr_type::TIM6: return rcc_type::get_hz_apb1_tim();
				case adr_type::TIM7: return rcc_type::get_hz_apb1_tim();
				case adr_type::TIM8: return rcc_type::get_hz_apb2_tim();
				case adr_type::TIM12: return rcc_type::get_hz_apb1_tim();
				case adr_type::TIM13: return rcc_type::get_hz_apb1_tim();
				case adr_type::TIM14: return rcc_type::get_hz_apb1_tim();
				case adr_type::TIM15: return rcc_type::get_hz_apb2_tim();
				case adr_type::TIM16: return rcc_type::get_hz_apb2_tim();
				case adr_type::TIM17: return rcc_type::get_hz_apb2_tim();
				case adr_type::LPTIM1: return rcc_type::get_hz_apb1_tim();
			}
		}

		template <uint32_t port>
		constexpr static uint32_t get_uart_hz()
		{
			switch(port)
			{
				default: lac_assert(false);
				//falls through
				case adr_type::USART1: return rcc_type::get_hz_usart1();
				case adr_type::USART2: return rcc_type::get_hz_usart2();
				case adr_type::USART3: return rcc_type::get_hz_usart3();
				case adr_type::UART4: return rcc_type::get_hz_uart4();
				case adr_type::UART5: return rcc_type::get_hz_uart5();
				case adr_type::USART6: return rcc_type::get_hz_usart6();
				case adr_type::UART7: return rcc_type::get_hz_uart7();
				case adr_type::UART8: return rcc_type::get_hz_uart8();
			}
		}

		template <uint32_t port>
		constexpr static uint32_t get_spi_hz()
		{
			switch(port)
			{
				default: lac_assert(false); [[fallthrough]];
				case adr_type::SPI1: return rcc_type::get_hz_spi1();
				case adr_type::SPI2: return rcc_type::get_hz_spi2();
				case adr_type::SPI3: return rcc_type::get_hz_spi3();
				case adr_type::SPI4: return rcc_type::get_hz_spi4();
				case adr_type::SPI5: return rcc_type::get_hz_spi5();
				case adr_type::SPI6: return rcc_type::get_hz_spi6();
			}
		}

		consteval static uint32_t get_gpio_index(uint32_t addr)
		{
			switch(addr)
			{
				default: lac_assert(false);
				//falls through
				case adr_type::GPIOA: return 0;
				case adr_type::GPIOB: return 1;
				case adr_type::GPIOC: return 2;
				case adr_type::GPIOD: return 3;
				case adr_type::GPIOE: return 4;
				case adr_type::GPIOF: return 5;
				case adr_type::GPIOG: return 6;
				case adr_type::GPIOH: return 7;
				case adr_type::GPIOI: return 8;
				case adr_type::GPIOJ: return 9;
				case adr_type::GPIOK: return 10;
			}
		}
	};
} // namespace lac
