/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <utl/bitfield.h>
#include <utl/bitflag.h>

namespace lac
{
	enum dma_stream_id : uint32_t
	{
		DMA1_STREAM_0,
		DMA1_STREAM_1,
		DMA1_STREAM_2,
		DMA1_STREAM_3,
		DMA1_STREAM_4,
		DMA1_STREAM_5,
		DMA1_STREAM_6,
		DMA1_STREAM_7,
		DMA2_STREAM_0,
		DMA2_STREAM_1,
		DMA2_STREAM_2,
		DMA2_STREAM_3,
		DMA2_STREAM_4,
		DMA2_STREAM_5,
		DMA2_STREAM_6,
		DMA2_STREAM_7,
		DMA_STREAM_COUNT,
		DMA_STREAM_NONE = 0xFFFFFFFF
	};

	struct reg_h7_dmamux_channel
	{
		volatile uint32_t CCR;

		using CCR_DMAREQ_ID = bitfield<0, 7>;
		using CCR_SOIE		= bitflag<8>;
		using CCR_EGE		= bitflag<9>;
		using CCR_SE		= bitflag<16>;
		using CCR_SPOL		= bitfield<17, 2>;
		using CCR_NBREQ		= bitfield<19, 5>;
		using CCR_SYNC_ID	= bitfield<24, 3>;

		void set_dmareq(uint32_t req) { CCR_DMAREQ_ID::set(CCR, req); }
	};

	struct reg_h7_dmamux_req_gen
	{
		volatile uint32_t RGCR;

		using RGCR_SIG_ID = bitfield<0, 3>;
		using RGCR_OIE	  = bitflag<8>;
		using RGCR_GE	  = bitflag<16>;
		using RGCR_GPOL	  = bitfield<17, 2>;
		using RGCR_GNBREQ = bitfield<19, 5>;
	};

	struct reg_h7_dmamux
	{
		const static uint32_t STREAM_COUNT = 16;
		const static uint32_t REQ_COUNT	   = 8;

		reg_h7_dmamux_channel channels[STREAM_COUNT];

		uint8_t padding0[0x80 - sizeof(channels)];

		volatile uint32_t CSR;
		volatile uint32_t CFR;

		uint8_t padding1[0x100 - 0x80 - 8];

		reg_h7_dmamux_req_gen req[REQ_COUNT];

		uint8_t padding2[0x140 - 0x100 - sizeof(req)];

		volatile uint32_t RGSR;
		volatile uint32_t RGCFR;

		void clr_csr(dma_stream_id id) { CFR = 1 << id; }
		void set_cfr(dma_stream_id id) { CFR = 1 << id; }
		void set_dmareq(dma_stream_id id, uint32_t req) { channels[id].set_dmareq(req); }
	};

	struct h7_dmamux1_request
	{
		enum
		{
			MUX1_MEM2MEM	   = 0,
			MUX1_GENERATOR0	   = 1,
			MUX1_GENERATOR1	   = 2,
			MUX1_GENERATOR2	   = 3,
			MUX1_GENERATOR3	   = 4,
			MUX1_GENERATOR4	   = 5,
			MUX1_GENERATOR5	   = 6,
			MUX1_GENERATOR6	   = 7,
			MUX1_GENERATOR7	   = 8,
			MUX1_ADC1		   = 9,
			MUX1_ADC2		   = 10,
			MUX1_TIM1_CH1	   = 11,
			MUX1_TIM1_CH2	   = 12,
			MUX1_TIM1_CH3	   = 13,
			MUX1_TIM1_CH4	   = 14,
			MUX1_TIM1_UP	   = 15,
			MUX1_TIM1_TRIG	   = 16,
			MUX1_TIM1_COM	   = 17,
			MUX1_TIM2_CH1	   = 18,
			MUX1_TIM2_CH2	   = 19,
			MUX1_TIM2_CH3	   = 20,
			MUX1_TIM2_CH4	   = 21,
			MUX1_TIM2_UP	   = 22,
			MUX1_TIM3_CH1	   = 23,
			MUX1_TIM3_CH2	   = 24,
			MUX1_TIM3_CH3	   = 25,
			MUX1_TIM3_CH4	   = 26,
			MUX1_TIM3_UP	   = 27,
			MUX1_TIM3_TRIG	   = 28,
			MUX1_TIM4_CH1	   = 29,
			MUX1_TIM4_CH2	   = 30,
			MUX1_TIM4_CH3	   = 31,
			MUX1_TIM4_UP	   = 32,
			MUX1_I2C1_RX	   = 33,
			MUX1_I2C1_TX	   = 34,
			MUX1_I2C2_RX	   = 35,
			MUX1_I2C2_TX	   = 36,
			MUX1_SPI1_RX	   = 37,
			MUX1_SPI1_TX	   = 38,
			MUX1_SPI2_RX	   = 39,
			MUX1_SPI2_TX	   = 40,
			MUX1_USART1_RX	   = 41,
			MUX1_USART1_TX	   = 42,
			MUX1_USART2_RX	   = 43,
			MUX1_USART2_TX	   = 44,
			MUX1_USART3_RX	   = 45,
			MUX1_USART3_TX	   = 46,
			MUX1_TIM8_CH1	   = 47,
			MUX1_TIM8_CH2	   = 48,
			MUX1_TIM8_CH3	   = 49,
			MUX1_TIM8_CH4	   = 50,
			MUX1_TIM8_UP	   = 51,
			MUX1_TIM8_TRIG	   = 52,
			MUX1_TIM8_COM	   = 53,
			MUX1_TIM5_CH1	   = 55,
			MUX1_TIM5_CH2	   = 56,
			MUX1_TIM5_CH3	   = 57,
			MUX1_TIM5_CH4	   = 58,
			MUX1_TIM5_UP	   = 59,
			MUX1_TIM5_TRIG	   = 60,
			MUX1_SPI3_RX	   = 61,
			MUX1_SPI3_TX	   = 62,
			MUX1_UART4_RX	   = 63,
			MUX1_UART4_TX	   = 64,
			MUX1_UART5_RX	   = 65,
			MUX1_UART5_TX	   = 66,
			MUX1_DAC1_CH1	   = 67,
			MUX1_DAC1_CH2	   = 68,
			MUX1_TIM6_UP	   = 69,
			MUX1_TIM7_UP	   = 70,
			MUX1_USART6_RX	   = 71,
			MUX1_USART6_TX	   = 72,
			MUX1_I2C3_RX	   = 73,
			MUX1_I2C3_TX	   = 74,
			MUX1_DCMI		   = 75,
			MUX1_CRYP_IN	   = 76,
			MUX1_CRYP_OUT	   = 77,
			MUX1_HASH_IN	   = 78,
			MUX1_UART7_RX	   = 79,
			MUX1_UART7_TX	   = 80,
			MUX1_UART8_RX	   = 81,
			MUX1_UART8_TX	   = 82,
			MUX1_SPI4_RX	   = 83,
			MUX1_SPI4_TX	   = 84,
			MUX1_SPI5_RX	   = 85,
			MUX1_SPI5_TX	   = 86,
			MUX1_SAI1_A		   = 87,
			MUX1_SAI1_B		   = 88,
			MUX1_SAI2_A		   = 89,
			MUX1_SAI2_B		   = 90,
			MUX1_SWPMI_RX	   = 91,
			MUX1_SWPMI_TX	   = 92,
			MUX1_SPDIF_RX_DT   = 93,
			MUX1_SPDIF_RX_CS   = 94,
			MUX1_HRTIM_MASTER  = 95,
			MUX1_HRTIM_TIMER_A = 96,
			MUX1_HRTIM_TIMER_B = 97,
			MUX1_HRTIM_TIMER_C = 98,
			MUX1_HRTIM_TIMER_D = 99,
			MUX1_HRTIM_TIMER_E = 100,
			MUX1_DFSDM1_FLT0   = 101,
			MUX1_DFSDM1_FLT1   = 102,
			MUX1_DFSDM1_FLT2   = 103,
			MUX1_DFSDM1_FLT3   = 104,
			MUX1_TIM15_CH1	   = 105,
			MUX1_TIM15_UP	   = 106,
			MUX1_TIM15_TRIG	   = 107,
			MUX1_TIM15_COM	   = 108,
			MUX1_TIM16_CH1	   = 109,
			MUX1_TIM16_UP	   = 110,
			MUX1_TIM17_CH1	   = 111,
			MUX1_TIM17_UP	   = 112,
			MUX1_SAI3_A		   = 113,
			MUX1_SAI3_B		   = 114,
			MUX1_ADC3		   = 115
		};
	};

} // namespace lac