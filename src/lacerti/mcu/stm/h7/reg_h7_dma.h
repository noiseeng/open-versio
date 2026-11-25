/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <atomic>
#include <cstdint>
#include <utl/bitfield.h>
#include <utl/bitflag.h>
#include <utl/bug.h>

namespace lac
{
	enum dma_dir
	{
		DMA_PER2MEM = 0,
		DMA_MEM2PER = 1,
		DMA_MEM2MEM = 2
	};

	enum dma_priority
	{
		DMA_PL_LOW,
		DMA_PL_MEDIUM,
		DMA_PL_HIGH,
		DMA_PL_VERY_HIGH
	};

	enum dma_request
	{
		DMA1_REQ_MEM2MEM	= 0,
		DMA1_REQ_GENERATOR0 = 1,
		DMA1_REQ_GENERATOR1 = 2,
		DMA1_REQ_GENERATOR2 = 3,
		DMA1_REQ_GENERATOR3 = 4,
		DMA1_REQ_GENERATOR4 = 5,
		DMA1_REQ_GENERATOR5 = 6,
		DMA1_REQ_GENERATOR6 = 7,
		DMA1_REQ_GENERATOR7 = 8,
		DMA1_REQ_ADC1		= 9,
		DMA1_REQ_ADC2		= 10,

		DMA1_REQ_TIM1_CH1  = 11,
		DMA1_REQ_TIM1_CH2  = 12,
		DMA1_REQ_TIM1_CH3  = 13,
		DMA1_REQ_TIM1_CH4  = 14,
		DMA1_REQ_TIM1_UP   = 15,
		DMA1_REQ_TIM1_TRIG = 16,
		DMA1_REQ_TIM1_COM  = 17,
		DMA1_REQ_TIM2_CH1  = 18,
		DMA1_REQ_TIM2_CH2  = 19,
		DMA1_REQ_TIM2_CH3  = 20,
		DMA1_REQ_TIM2_CH4  = 21,
		DMA1_REQ_TIM2_UP   = 22,
		DMA1_REQ_TIM3_CH1  = 23,
		DMA1_REQ_TIM3_CH2  = 24,
		DMA1_REQ_TIM3_CH3  = 25,
		DMA1_REQ_TIM3_CH4  = 26,
		DMA1_REQ_TIM3_UP   = 27,
		DMA1_REQ_TIM3_TRIG = 28,
		DMA1_REQ_TIM4_CH1  = 29,
		DMA1_REQ_TIM4_CH2  = 30,
		DMA1_REQ_TIM4_CH3  = 31,
		DMA1_REQ_TIM4_UP   = 32,

		DMA1_REQ_I2C1_RX = 33,
		DMA1_REQ_I2C1_TX = 34,
		DMA1_REQ_I2C2_RX = 35,
		DMA1_REQ_I2C2_TX = 36,

		DMA1_REQ_SPI1_RX = 37,
		DMA1_REQ_SPI1_TX = 38,
		DMA1_REQ_SPI2_RX = 39,
		DMA1_REQ_SPI2_TX = 40,

		DMA1_REQ_USART1_RX = 41,
		DMA1_REQ_USART1_TX = 42,
		DMA1_REQ_USART2_RX = 43,
		DMA1_REQ_USART2_TX = 44,
		DMA1_REQ_USART3_RX = 45,
		DMA1_REQ_USART3_TX = 46,

		DMA1_REQ_TIM8_CH1  = 47,
		DMA1_REQ_TIM8_CH2  = 48,
		DMA1_REQ_TIM8_CH3  = 49,
		DMA1_REQ_TIM8_CH4  = 50,
		DMA1_REQ_TIM8_UP   = 51,
		DMA1_REQ_TIM8_TRIG = 52,
		DMA1_REQ_TIM8_COM  = 53,

		DMA1_REQ_TIM5_CH1  = 55,
		DMA1_REQ_TIM5_CH2  = 56,
		DMA1_REQ_TIM5_CH3  = 57,
		DMA1_REQ_TIM5_CH4  = 58,
		DMA1_REQ_TIM5_UP   = 59,
		DMA1_REQ_TIM5_TRIG = 60,

		DMA1_REQ_SPI3_RX = 61,
		DMA1_REQ_SPI3_TX = 62,

		DMA1_REQ_UART4_RX = 63,
		DMA1_REQ_UART4_TX = 64,
		DMA1_REQ_UART5_RX = 65,
		DMA1_REQ_UART5_TX = 66,

		DMA1_REQ_DAC1_CH1 = 67,
		DMA1_REQ_DAC1_CH2 = 68,

		DMA1_REQ_TIM6_UP = 69,
		DMA1_REQ_TIM7_UP = 70,

		DMA1_REQ_USART6_RX = 71,
		DMA1_REQ_USART6_TX = 72,

		DMA1_REQ_I2C3_RX = 73,
		DMA1_REQ_I2C3_TX = 74,

		DMA1_REQ_DCMI = 75,

		DMA1_REQ_CRYP_IN  = 76,
		DMA1_REQ_CRYP_OUT = 77,

		DMA1_REQ_HASH_IN = 78,

		DMA1_REQ_UART7_RX = 79,
		DMA1_REQ_UART7_TX = 80,
		DMA1_REQ_UART8_RX = 81,
		DMA1_REQ_UART8_TX = 82,

		DMA1_REQ_SPI4_RX = 83,
		DMA1_REQ_SPI4_TX = 84,
		DMA1_REQ_SPI5_RX = 85,
		DMA1_REQ_SPI5_TX = 86,

		DMA1_REQ_SAI1_A = 87,
		DMA1_REQ_SAI1_B = 88,
		DMA1_REQ_SAI2_A = 89,
		DMA1_REQ_SAI2_B = 90,

		DMA1_REQ_SWPMI_RX = 91,
		DMA1_REQ_SWPMI_TX = 92,

		DMA1_REQ_SPDIF_RX_DT = 93,
		DMA1_REQ_SPDIF_RX_CS = 94,

		DMA1_REQ_HRTIM_MASTER  = 95,
		DMA1_REQ_HRTIM_TIMER_A = 96,
		DMA1_REQ_HRTIM_TIMER_B = 97,
		DMA1_REQ_HRTIM_TIMER_C = 98,
		DMA1_REQ_HRTIM_TIMER_D = 99,
		DMA1_REQ_HRTIM_TIMER_E = 100,

		DMA1_REQ_DFSDM1_FLT0 = 101,
		DMA1_REQ_DFSDM1_FLT1 = 102,
		DMA1_REQ_DFSDM1_FLT2 = 103,
		DMA1_REQ_DFSDM1_FLT3 = 104,

		DMA1_REQ_TIM15_CH1	= 105,
		DMA1_REQ_TIM15_UP	= 106,
		DMA1_REQ_TIM15_TRIG = 107,
		DMA1_REQ_TIM15_COM	= 108,

		DMA1_REQ_TIM16_CH1 = 109,
		DMA1_REQ_TIM16_UP  = 110,

		DMA1_REQ_TIM17_CH1 = 111,
		DMA1_REQ_TIM17_UP  = 112,

		DMA1_REQ_SAI3_A = 113,
		DMA1_REQ_SAI3_B = 114,

		DMA1_REQ_ADC3 = 115,

		DMA_REQ_INVALID = 0xFFFFFFFF
	};

	enum bdma_request
	{
		DMA2_REQ_MEM2MEM	= 0,
		DMA2_REQ_GENERATOR0 = 1,
		DMA2_REQ_GENERATOR1 = 2,
		DMA2_REQ_GENERATOR2 = 3,
		DMA2_REQ_GENERATOR3 = 4,
		DMA2_REQ_GENERATOR4 = 5,
		DMA2_REQ_GENERATOR5 = 6,
		DMA2_REQ_GENERATOR6 = 7,
		DMA2_REQ_GENERATOR7 = 8,
		DMA2_REQ_LPUART1_RX = 9,
		DMA2_REQ_LPUART1_TX = 10,
		DMA2_REQ_SPI6_RX	= 11,
		DMA2_REQ_SPI6_TX	= 12,
		DMA2_REQ_I2C4_RX	= 13,
		DMA2_REQ_I2C4_TX	= 14,
		DMA2_REQ_SAI4_A		= 15,
		DMA2_REQ_SAI4_B		= 16,
		DMA2_REQ_ADC3		= 17
	};

	enum dma_burst_size
	{
		DMA_BURST_1	 = 0,
		DMA_BURST_4	 = 1,
		DMA_BURST_8	 = 2,
		DMA_BURST_16 = 3
	};

	struct reg_h7_dma_stream
	{
		volatile uint32_t CR;
		volatile uint32_t NDTR;
		volatile uint32_t PAR;
		volatile uint32_t M0AR;
		volatile uint32_t M1AR;
		volatile uint32_t FCR;

		using CR_MBURST = bitfield<23, 2>;
		using CR_PBURST = bitfield<21, 2>;
		using CR_TRBUFF = bitflag<20>;
		using CR_CT		= bitflag<19>;
		using CR_DBM	= bitflag<18>;
		using CR_PL		= bitfield<16, 2>;
		using CR_PINCOS = bitflag<15>;
		using CR_MSIZE	= bitfield<13, 2>;
		using CR_PSIZE	= bitfield<11, 2>;
		using CR_MINC	= bitflag<10>;
		using CR_PINC	= bitflag<9>;
		using CR_CIRC	= bitflag<8>;
		using CR_DIR	= bitfield<6, 2>;
		using CR_PFCTRL = bitflag<5>;
		using CR_TCIE	= bitflag<4>;
		using CR_HTIE	= bitflag<3>;
		using CR_TEIE	= bitflag<2>;
		using CR_DMEIE	= bitflag<1>;
		using CR_EN		= bitflag<0>;

		using NDTR_NDT = bitfield<0, 16>;

		using PAR_PAR = bitfield<0, 32>;

		using M0AR_M0A = bitfield<0, 32>;

		using M1AR_M1A = bitfield<0, 32>;

		using FCR_FTH	= bitfield<0, 2>;
		using FCR_DMDIS = bitflag<2>;
		using FCR_FS	= bitfield<3, 3>;
		using FCR_FEIE	= bitflag<7>;

		void stall()
		{
			while(CR_EN::get(CR))
				;
		}

		void disable() { CR_EN::clr(CR); }

		void wait_en()
		{
			while(CR_EN::get(CR))
				;
		}

		void enable() { CR_EN::set(CR); }

		void clear_dbm() { CR_DBM::clr(CR); }

		void set_pfctrl(bool value) { CR_PFCTRL::set(CR, value); }

		void set_mburst(dma_burst_size bs) { CR_MBURST::set(CR, bs); }

		void set_pburst(dma_burst_size bs) { CR_PBURST::set(CR, bs); }

		uint32_t get_remain() const { return NDTR_NDT::get(NDTR); }

		static constexpr uint32_t mem2mem_cr(uint32_t align_size, dma_priority pl = DMA_PL_HIGH)
		{
			return CR_DIR::val(DMA_MEM2MEM) | CR_MINC::BIT | CR_PINC::BIT | CR_MSIZE::val(align_size / 2)
				   | CR_PSIZE::val(align_size / 2) | CR_PL::val(pl);
		}

		static constexpr uint32_t mem2mem_fcr() { return FCR_DMDIS::BIT | FCR_FTH::val(3); }
	};

	struct reg_h7_dma
	{
		const static uint32_t STREAM_COUNT = 8;

		using LISR_FEIF0				= bitflag<0>;
		using LISR_NOPE0				= bitflag<1>;
		using LISR_DMEIF0				= bitflag<2>;
		using LISR_TEIF0				= bitflag<3>;
		using LISR_HTIF0				= bitflag<4>;
		using LISR_TCIF0				= bitflag<5>;
		const static uint32_t LISR_ERR0 = LISR_DMEIF0::BIT | LISR_TEIF0::BIT;

		using LISR_FEIF1				= bitflag<6>;
		using LISR_NOPE1				= bitflag<7>;
		using LISR_DMEIF1				= bitflag<8>;
		using LISR_TEIF1				= bitflag<9>;
		using LISR_HTIF1				= bitflag<10>;
		using LISR_TCIF1				= bitflag<11>;
		const static uint32_t LISR_ERR1 = LISR_DMEIF1::BIT | LISR_TEIF1::BIT;

		using LISR_FEIF2				= bitflag<16>;
		using LISR_NOPE2				= bitflag<17>;
		using LISR_DMEIF2				= bitflag<18>;
		using LISR_TEIF2				= bitflag<19>;
		using LISR_HTIF2				= bitflag<20>;
		using LISR_TCIF2				= bitflag<21>;
		const static uint32_t LISR_ERR2 = LISR_DMEIF2::BIT | LISR_TEIF2::BIT;

		using LISR_FEIF3				= bitflag<22>;
		using LISR_NOPE3				= bitflag<23>;
		using LISR_DMEIF3				= bitflag<24>;
		using LISR_TEIF3				= bitflag<25>;
		using LISR_HTIF3				= bitflag<26>;
		using LISR_TCIF3				= bitflag<27>;
		const static uint32_t LISR_ERR3 = LISR_DMEIF3::BIT | LISR_TEIF3::BIT;

		using HISR_FEIF4				= bitflag<0>;
		using HISR_NOPE4				= bitflag<1>;
		using HISR_DMEIF4				= bitflag<2>;
		using HISR_TEIF4				= bitflag<3>;
		using HISR_HTIF4				= bitflag<4>;
		using HISR_TCIF4				= bitflag<5>;
		const static uint32_t HISR_ERR4 = HISR_DMEIF4::BIT | HISR_TEIF4::BIT;

		using HISR_FEIF5				= bitflag<6>;
		using HISR_NOPE5				= bitflag<7>;
		using HISR_DMEIF5				= bitflag<8>;
		using HISR_TEIF5				= bitflag<9>;
		using HISR_HTIF5				= bitflag<10>;
		using HISR_TCIF5				= bitflag<11>;
		const static uint32_t HISR_ERR5 = HISR_DMEIF5::BIT | HISR_TEIF5::BIT;

		using HISR_FEIF6				= bitflag<16>;
		using HISR_NOPE6				= bitflag<17>;
		using HISR_DMEIF6				= bitflag<18>;
		using HISR_TEIF6				= bitflag<19>;
		using HISR_HTIF6				= bitflag<20>;
		using HISR_TCIF6				= bitflag<21>;
		const static uint32_t HISR_ERR6 = HISR_DMEIF6::BIT | HISR_TEIF6::BIT;

		using HISR_FEIF7				= bitflag<22>;
		using HISR_NOPE7				= bitflag<23>;
		using HISR_DMEIF7				= bitflag<24>;
		using HISR_TEIF7				= bitflag<25>;
		using HISR_HTIF7				= bitflag<26>;
		using HISR_TCIF7				= bitflag<27>;
		const static uint32_t HISR_ERR7 = HISR_DMEIF7::BIT | HISR_TEIF7::BIT;

		volatile uint32_t LISR;
		volatile uint32_t HISR;
		volatile uint32_t LIFCR;
		volatile uint32_t HIFCR;

		reg_h7_dma_stream streams[STREAM_COUNT];

		static reg_h7_dma *reg(uint32_t base)
		{
			lac_assert(base);
			return (reg_h7_dma *)base;
		}

		constexpr reg_h7_dma_stream *get_stream(uint32_t idx)
		{
			lac_assert(idx < STREAM_COUNT);
			return streams + idx;
		}

		static bool has_and_clear(volatile uint32_t &isr, uint32_t mask)
		{
			if((isr & mask) != 0)
			{
				isr = mask;
				std::atomic_thread_fence(std::memory_order_seq_cst);
				return true;
			}
			else
			{
				return false;
			}
		}

		bool is_irq_error(uint32_t stream_idx)
		{
			switch(stream_idx)
			{
				case 0: return has_and_clear(LISR, LISR_ERR0);
				case 1: return has_and_clear(LISR, LISR_ERR1);
				case 2: return has_and_clear(LISR, LISR_ERR2);
				case 3: return has_and_clear(LISR, LISR_ERR3);
				case 4: return has_and_clear(HISR, HISR_ERR4);
				case 5: return has_and_clear(HISR, HISR_ERR5);
				case 6: return has_and_clear(HISR, HISR_ERR6);
				case 7: return has_and_clear(HISR, HISR_ERR7);
				default: lac_assert(false); return 0;
			}
		}

		bool is_irq_half(uint32_t stream_idx)
		{
			switch(stream_idx)
			{
				case 0: return has_and_clear(LISR, LISR_HTIF0::BIT);
				case 1: return has_and_clear(LISR, LISR_HTIF1::BIT);
				case 2: return has_and_clear(LISR, LISR_HTIF2::BIT);
				case 3: return has_and_clear(LISR, LISR_HTIF3::BIT);
				case 4: return has_and_clear(HISR, HISR_HTIF4::BIT);
				case 5: return has_and_clear(HISR, HISR_HTIF5::BIT);
				case 6: return has_and_clear(HISR, HISR_HTIF6::BIT);
				case 7: return has_and_clear(HISR, HISR_HTIF7::BIT);
				default: lac_assert(false); return 0;
			}
		}

		bool is_irq_full(uint32_t stream_idx)
		{
			switch(stream_idx)
			{
				case 0: return has_and_clear(LISR, LISR_TCIF0::BIT);
				case 1: return has_and_clear(LISR, LISR_TCIF1::BIT);
				case 2: return has_and_clear(LISR, LISR_TCIF2::BIT);
				case 3: return has_and_clear(LISR, LISR_TCIF3::BIT);
				case 4: return has_and_clear(HISR, HISR_TCIF4::BIT);
				case 5: return has_and_clear(HISR, HISR_TCIF5::BIT);
				case 6: return has_and_clear(HISR, HISR_TCIF6::BIT);
				case 7: return has_and_clear(HISR, HISR_TCIF7::BIT);
				default: lac_assert(false); return 0;
			}
		}

		bool is_irq_fifo_error(uint32_t stream_idx)
		{
			switch(stream_idx)
			{
				case 0: return has_and_clear(LISR, LISR_FEIF0::BIT);
				case 1: return has_and_clear(LISR, LISR_FEIF1::BIT);
				case 2: return has_and_clear(LISR, LISR_FEIF2::BIT);
				case 3: return has_and_clear(LISR, LISR_FEIF3::BIT);
				case 4: return has_and_clear(HISR, HISR_FEIF4::BIT);
				case 5: return has_and_clear(HISR, HISR_FEIF5::BIT);
				case 6: return has_and_clear(HISR, HISR_FEIF6::BIT);
				case 7: return has_and_clear(HISR, HISR_FEIF7::BIT);
				default: lac_assert(false); return 0;
			}
		}
	};

} // namespace lac