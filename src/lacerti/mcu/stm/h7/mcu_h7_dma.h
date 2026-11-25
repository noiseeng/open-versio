/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "reg_h7_dma.h"
#include "reg_h7_dmamux.h"
#include <cstdint>
#include <utl/bug.h>
namespace lac
{
	template <typename adr_t>
	struct mcu_h7_dma
	{
		consteval static dma_request get_i2c_dma_req_rx(uint32_t port, dma_stream_id id)
		{
			if(id == DMA_STREAM_NONE)
			{
				return DMA_REQ_INVALID;
			}

			switch(port)
			{
				case adr_t::I2C1: return DMA1_REQ_I2C1_RX;
				case adr_t::I2C2: return DMA1_REQ_I2C2_RX;
				case adr_t::I2C3: return DMA1_REQ_I2C3_RX;
				default: lac_assert(false); return DMA_REQ_INVALID;
			}
		}

		consteval static dma_request get_i2c_dma_req_tx(uint32_t port, dma_stream_id id)
		{
			if(id == DMA_STREAM_NONE)
			{
				return DMA_REQ_INVALID;
			}

			switch(port)
			{
				case adr_t::I2C1: return DMA1_REQ_I2C1_TX;
				case adr_t::I2C2: return DMA1_REQ_I2C2_TX;
				case adr_t::I2C3: return DMA1_REQ_I2C3_TX;
				default: lac_assert(false); return DMA_REQ_INVALID;
			}
		}

		consteval static dma_request get_spi_dma_req_rx(uint32_t port)
		{
			switch(port)
			{
				case adr_t::SPI1: return DMA1_REQ_SPI1_RX;
				case adr_t::SPI2: return DMA1_REQ_SPI2_RX;
				case adr_t::SPI3: return DMA1_REQ_SPI3_RX;
				case adr_t::SPI4: return DMA1_REQ_SPI4_RX;
				case adr_t::SPI5: return DMA1_REQ_SPI5_RX;
				default: lac_assert(false); return DMA_REQ_INVALID;
			}
		}

		consteval static dma_request get_spi_dma_req_tx(uint32_t port)
		{
			switch(port)
			{
				case adr_t::SPI1: return DMA1_REQ_SPI1_TX;
				case adr_t::SPI2: return DMA1_REQ_SPI2_TX;
				case adr_t::SPI3: return DMA1_REQ_SPI3_TX;
				case adr_t::SPI4: return DMA1_REQ_SPI4_TX;
				case adr_t::SPI5: return DMA1_REQ_SPI5_TX;
				default: lac_assert(false); return DMA_REQ_INVALID;
			}
		}

		consteval static dma_request get_sai_dma_req(uint32_t sai_base, bool is_block_a)
		{
			switch(sai_base)
			{
				case adr_t::SAI1: return is_block_a ? DMA1_REQ_SAI1_A : DMA1_REQ_SAI1_B;
				case adr_t::SAI2: return is_block_a ? DMA1_REQ_SAI2_A : DMA1_REQ_SAI2_B;
				case adr_t::SAI3: return is_block_a ? DMA1_REQ_SAI3_A : DMA1_REQ_SAI3_B;
				default: lac_assert(false); return DMA_REQ_INVALID;
			}
		}

		constexpr static uint32_t get_adc_dmamux_req(uint32_t port)
		{
			switch(port)
			{
				case adr_t::ADC1: return h7_dmamux1_request::MUX1_ADC1;
				case adr_t::ADC2: return h7_dmamux1_request::MUX1_ADC2;
				case adr_t::ADC3: return h7_dmamux1_request::MUX1_ADC3;
				default: lac_assert(false); return 0;
			};
		}

		consteval static bool is_valid_dma_stream(dma_stream_id stream)
		{ return stream < DMA_STREAM_COUNT || stream == DMA_STREAM_NONE; }

		consteval static uint32_t get_dma_first_stream(uint32_t port)
		{
			switch(port)
			{
				default: lac_assert(false);
				case DMA_STREAM_NONE: return 0;
				case adr_t::DMA1: return DMA1_STREAM_0;
				case adr_t::DMA2: return DMA2_STREAM_0;
			}
		}

		consteval static uint32_t get_dma_from_stream(dma_stream_id stream_id)
		{
			switch(stream_id)
			{
				case DMA1_STREAM_0:
				case DMA1_STREAM_1:
				case DMA1_STREAM_2:
				case DMA1_STREAM_3:
				case DMA1_STREAM_4:
				case DMA1_STREAM_5:
				case DMA1_STREAM_6:
				case DMA1_STREAM_7: return adr_t::DMA1;
				case DMA2_STREAM_0:
				case DMA2_STREAM_1:
				case DMA2_STREAM_2:
				case DMA2_STREAM_3:
				case DMA2_STREAM_4:
				case DMA2_STREAM_5:
				case DMA2_STREAM_6:
				case DMA2_STREAM_7: return adr_t::DMA2;
				default: lac_assert(false); [[fallthrough]];
				case DMA_STREAM_NONE: return DMA_STREAM_NONE;
			}
		}

		consteval static dma_request get_dma_req_rx(uint32_t port)
		{
			switch(port)
			{
				default: lac_assert(false); [[fallthrough]];
				case adr_t::USART1: return DMA1_REQ_USART1_RX;
				case adr_t::USART2: return DMA1_REQ_USART2_RX;
				case adr_t::USART3: return DMA1_REQ_USART3_RX;
				case adr_t::UART4: return DMA1_REQ_UART4_RX;
				case adr_t::UART5: return DMA1_REQ_UART5_RX;
				case adr_t::USART6: return DMA1_REQ_USART6_RX;
				case adr_t::UART7: return DMA1_REQ_UART7_RX;
				case adr_t::UART8: return DMA1_REQ_UART8_RX;
			}
		}

		consteval static dma_request get_dma_req_tx(uint32_t port)
		{
			switch(port)
			{
				default: lac_assert(false); [[fallthrough]];
				case adr_t::USART1: return DMA1_REQ_USART1_TX;
				case adr_t::USART2: return DMA1_REQ_USART2_TX;
				case adr_t::USART3: return DMA1_REQ_USART3_TX;
				case adr_t::UART4: return DMA1_REQ_UART4_TX;
				case adr_t::UART5: return DMA1_REQ_UART5_TX;
				case adr_t::USART6: return DMA1_REQ_USART6_TX;
				case adr_t::UART7: return DMA1_REQ_UART7_TX;
				case adr_t::UART8: return DMA1_REQ_UART8_TX;
			}
		}
	};
} // namespace lac
