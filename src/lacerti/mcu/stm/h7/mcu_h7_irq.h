/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

#include <bug/eabi.h>
#include <utl/bug.h>

#include <mcu/stm/h7/reg_h7_dmamux.h>

namespace lac
{
	template <typename adr_type, typename irq_type, typename isv_type>
	struct mcu_h7_irq
	{
		using isv_t = isv_type;
		using irq_t = irq_type;

		consteval static isv_t get_dma_isv(dma_stream_id stream_id)
		{
			switch(stream_id)
			{
				case DMA1_STREAM_0: return isv_t::DMA1_STREAM0;
				case DMA1_STREAM_1: return isv_t::DMA1_STREAM1;
				case DMA1_STREAM_2: return isv_t::DMA1_STREAM2;
				case DMA1_STREAM_3: return isv_t::DMA1_STREAM3;
				case DMA1_STREAM_4: return isv_t::DMA1_STREAM4;
				case DMA1_STREAM_5: return isv_t::DMA1_STREAM5;
				case DMA1_STREAM_6: return isv_t::DMA1_STREAM6;
				case DMA1_STREAM_7: return isv_t::DMA1_STREAM7;
				case DMA2_STREAM_0: return isv_t::DMA2_STREAM0;
				case DMA2_STREAM_1: return isv_t::DMA2_STREAM1;
				case DMA2_STREAM_2: return isv_t::DMA2_STREAM2;
				case DMA2_STREAM_3: return isv_t::DMA2_STREAM3;
				case DMA2_STREAM_4: return isv_t::DMA2_STREAM4;
				case DMA2_STREAM_5: return isv_t::DMA2_STREAM5;
				case DMA2_STREAM_6: return isv_t::DMA2_STREAM6;
				case DMA2_STREAM_7: return isv_t::DMA2_STREAM7;
				case DMA_STREAM_NONE: return isv_t::INVALID;
				default: break;
			}

			lac_assert(false);

			return isv_t::INVALID;
		}

		consteval static irq_t get_dma_irq(dma_stream_id stream_id)
		{
			switch(stream_id)
			{
				case DMA1_STREAM_0: return irq_t::DMA1_STREAM0;
				case DMA1_STREAM_1: return irq_t::DMA1_STREAM1;
				case DMA1_STREAM_2: return irq_t::DMA1_STREAM2;
				case DMA1_STREAM_3: return irq_t::DMA1_STREAM3;
				case DMA1_STREAM_4: return irq_t::DMA1_STREAM4;
				case DMA1_STREAM_5: return irq_t::DMA1_STREAM5;
				case DMA1_STREAM_6: return irq_t::DMA1_STREAM6;
				case DMA1_STREAM_7: return irq_t::DMA1_STREAM7;
				case DMA2_STREAM_0: return irq_t::DMA2_STREAM0;
				case DMA2_STREAM_1: return irq_t::DMA2_STREAM1;
				case DMA2_STREAM_2: return irq_t::DMA2_STREAM2;
				case DMA2_STREAM_3: return irq_t::DMA2_STREAM3;
				case DMA2_STREAM_4: return irq_t::DMA2_STREAM4;
				case DMA2_STREAM_5: return irq_t::DMA2_STREAM5;
				case DMA2_STREAM_6: return irq_t::DMA2_STREAM6;
				case DMA2_STREAM_7: return irq_t::DMA2_STREAM7;
				default: break;
			}

			lac_assert(false);

			return irq_t::INVALID;
		}

		consteval static isv_t get_adc_isrv(uint32_t port)
		{
			switch(port)
			{
				case adr_type::ADC1: return isv_t::ADC12;
				case adr_type::ADC2: return isv_t::ADC12;
				case adr_type::ADC3: return isv_t::ADC3;
				default: __builtin_unreachable();
			};
		}

		consteval static irq_t get_adc_irq(uint32_t port)
		{
			switch(port)
			{
				case adr_type::ADC1: return irq_t::ADC12;
				case adr_type::ADC2: return irq_t::ADC12;
				case adr_type::ADC3: return irq_t::ADC3;
				default: __builtin_unreachable();
			};
		}

		consteval static irq_type get_tim_irq_up(uint32_t port)
		{
			switch(port)
			{
				case adr_type::TIM1: return irq_type::TIM1_UP;
				case adr_type::TIM2: return irq_type::TIM2;
				case adr_type::TIM3: return irq_type::TIM3;
				case adr_type::TIM4: return irq_type::TIM4;
				case adr_type::TIM5: return irq_type::TIM5;
				case adr_type::TIM6: return irq_type::TIM6_DAC;
				case adr_type::TIM7: return irq_type::TIM7;
				case adr_type::TIM8: return irq_type::TIM8_UP_TIM13;
				case adr_type::TIM12: return irq_type::TIM8_BRK_TIM12;
				case adr_type::TIM13: return irq_type::TIM8_UP_TIM13;
				case adr_type::TIM14: return irq_type::TIM8_TRG_COM_TIM14;
				case adr_type::LPTIM1: return irq_type::LPTIM1;
				case adr_type::LPTIM2: return irq_type::LPTIM2;
				case adr_type::LPTIM3: return irq_type::LPTIM3;
				case adr_type::LPTIM4: return irq_type::LPTIM4;
				case adr_type::LPTIM5: return irq_type::LPTIM5;
				default: bug_assert(false); return irq_type::INVALID;
			}
		}
	};
} // namespace lac
