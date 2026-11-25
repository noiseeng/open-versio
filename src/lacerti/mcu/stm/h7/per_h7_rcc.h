/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <utl/bug.h>

#include <cstdint>

#include "reg_h7_rcc.h"

namespace lac
{
	template <uint32_t port, uint32_t hsehz, uint32_t sys_clk_hz, uint32_t i2sin, uint32_t lsehz>
	struct per_h7_rcc
	{
		static reg_h7_rcc* reg() { return (reg_h7_rcc*)port; }

		const static uint32_t LSIRCHZ	= 32000;
		const static uint32_t HSIRC		= 64000000;
		const static uint32_t CSIRC		= 4000000;
		const static uint32_t HSEHZ		= hsehz;
		const static uint32_t RC48HZ	= 48000000;
		const static uint32_t I2SCKINHZ = i2sin;

		static uint32_t get_hz_lsi() { return LSIRCHZ; }

		static uint32_t get_hz_hsi() { return reg()->is_hsi() ? HSIRC >> reg()->get_hsi_shift() : 0; }

		static uint32_t get_hz_csi() { return CSIRC; }

		static uint32_t get_hz_hse() { return HSEHZ; }

		static uint32_t get_hz_rc48() { return RC48HZ; }

		static uint32_t get_hz_i2sin() { return I2SCKINHZ; }

		static uint32_t get_hz_lse() { return lsehz; }

		static uint32_t get_hz_pll_src()
		{
			switch(reg()->get_mux_pll_src())
			{
				default: lac_assert(false); return 0;
				case reg_h7_rcc::PLL_SRC_HSI: return get_hz_hsi();
				case reg_h7_rcc::PLL_SRC_CSI: return get_hz_csi();
				case reg_h7_rcc::PLL_SRC_HSE: return get_hz_hse();
				case reg_h7_rcc::PLL_SRC_NONE: return 0;
			}
		}

		static uint32_t get_hz_pll1()
		{
			if(reg()->ready_pll1())
			{
				uint32_t n = get_hz_pll_src();
				lac_assert(n);
				uint32_t d = reg()->get_div_pll1m();
				lac_assert(d);
				uint32_t s = reg()->get_mul_pll1n();
				return n / d * s;
			}
			else
			{
				return 0;
			}
		}

		static uint32_t get_hz_pll1p()
		{
			if(reg()->is_enable_pll1p())
			{
				uint32_t n = get_hz_pll1();
				lac_assert(n);
				uint32_t d = reg()->get_div_pll1p();
				lac_assert(d);
				return n / d;
			}
			else
			{
				return 0;
			}
		}

		static uint32_t get_hz_pll1q()
		{
			if(reg()->is_enable_pll1q())
			{
				uint32_t n = get_hz_pll1();
				lac_assert(n);
				uint32_t d = reg()->get_div_pll1q();
				lac_assert(d);
				return n / d;
			}
			else
			{
				return 0;
			}
		}

		static uint32_t get_hz_pll1r()
		{
			if(reg()->is_enable_pll1r())
			{
				uint32_t n = get_hz_pll1();
				lac_assert(n);
				uint32_t d = reg()->get_div_pll1r();
				lac_assert(d);

				return n / d;
			}
			else
			{}
		}

		static uint32_t get_hz_pll2()
		{
			if(reg()->ready_pll2())
			{
				uint32_t src = get_hz_pll_src();
				lac_assert(src);
				uint32_t mul = reg()->get_mul_pll2n();
				lac_assert(mul);
				uint32_t div = reg()->get_div_pll2m();
				lac_assert(div);

				return src * mul / div;
			}
			else
			{
				return 0;
			}
		}

		static uint32_t get_hz_pll2p()
		{
			if(reg()->is_enable_pll2p())
			{
				uint32_t n = get_hz_pll2();
				lac_assert(n);
				uint32_t d = reg()->get_div_pll2p();
				lac_assert(d);
				return n / d;
			}
			else
			{
				return 0;
			}
		}

		static uint32_t get_hz_pll2q()
		{
			if(reg()->is_enable_pll2q())
			{
				uint32_t n = get_hz_pll2();
				lac_assert(n);
				uint32_t d = reg()->get_div_pll2q();
				lac_assert(d);
				return n / d;
			}
			else
			{
				return 0;
			}
		}

		static uint32_t get_hz_pll2r()
		{
			if(reg()->is_enable_pll2r())
			{
				uint32_t n = get_hz_pll2();
				lac_assert(n);
				uint32_t d = reg()->get_div_pll2r();
				lac_assert(d);
				return n / d;
			}
			else
			{
				return 0;
			}
		}

		static uint32_t get_hz_pll3()
		{
			if(reg()->ready_pll3())
			{
				uint32_t n = get_hz_pll_src();
				lac_assert(n);
				uint32_t d = reg()->get_div_pll3m();
				lac_assert(d);
				uint32_t s	  = reg()->get_mul_pll3n();
				uint32_t frac = reg()->get_frac_pll3();
				return n / d * s + (uint32_t)((uint64_t)(n / d) * frac / 8192);
			}
			else
			{
				return 0;
			}
		}

		static uint32_t get_hz_pll3p()
		{
			if(reg()->is_enable_pll3p())
			{
				uint32_t n = get_hz_pll3();
				lac_assert(n);
				uint32_t d = reg()->get_div_pll3p();
				lac_assert(d);
				return n / d;
			}
			else
			{
				return 0;
			}
		}

		static uint32_t get_hz_pll3q()
		{
			if(reg()->is_enable_pll3q())
			{
				uint32_t n = get_hz_pll3();
				lac_assert(n);
				uint32_t d = reg()->get_div_pll3q();
				lac_assert(d);
				return n / d;
			}
			else
			{
				return 0;
			}
		}

		static uint32_t get_hz_pll3r()
		{
			if(reg()->is_enable_pll3r())
			{
				uint32_t n = get_hz_pll3();
				uint32_t d = reg()->get_div_pll3r();
				lac_assert(d);
				return n / d;
			}
			else
			{
				return 0;
			}
		}

		static uint32_t get_hz_sysclk()
		{
			switch(reg()->get_mux_sysclk())
			{
				default: lac_assert(false); return 0;
				case reg_h7_rcc::SYS_CLK_HSI: return get_hz_hsi();
				case reg_h7_rcc::SYS_CLK_CSI: return get_hz_csi();
				case reg_h7_rcc::SYS_CLK_HSE: return get_hz_hse();
				case reg_h7_rcc::SYS_CLK_PLL: return get_hz_pll1p();
			};
		};

		static uint32_t get_hz_cpu()
		{
			uint32_t n = get_hz_sysclk();
			lac_assert(n);
			uint32_t d = reg()->get_div_d1cpre();
			lac_assert(d);
			return n / d;
		}

		static uint32_t get_hz_systick()
		{
			uint32_t n = get_hz_cpu();
			lac_assert(n);
			uint32_t d = reg()->get_div_systick();
			lac_assert(d);
			return n / d;
		}

		static uint32_t get_hz_sys_hpre()
		{
			uint32_t n = get_hz_cpu();
			lac_assert(n);
			uint32_t d = reg()->get_div_hpre();
			lac_assert(d);
			return n / d;
		}

		static uint32_t get_hz_axi() { return get_hz_sys_hpre(); }

		static uint32_t get_hz_hclk3() { return get_hz_sys_hpre(); }

		static uint32_t get_hz_ahb1() { return get_hz_sys_hpre(); }

		static uint32_t get_hz_ahb2() { return get_hz_sys_hpre(); }

		static uint32_t get_hz_ahb4() { return get_hz_sys_hpre(); }

		static uint32_t get_hz_apb1()
		{
			uint32_t n = get_hz_sys_hpre();
			lac_assert(n);
			uint32_t d = reg()->get_div_d2ppre1();
			lac_assert(d);
			return n / d;
		}

		static uint32_t get_hz_apb1_tim() { return 2 * get_hz_apb1(); }

		static uint32_t get_hz_apb2()
		{
			uint32_t n = get_hz_sys_hpre();
			lac_assert(n);
			uint32_t d = reg()->get_div_d2ppre2();
			lac_assert(d);
			return n / d;
		}

		static uint32_t get_hz_apb2_tim() { return 2 * get_hz_apb2(); }

		static uint32_t get_hz_apb3()
		{
			uint32_t n = get_hz_sys_hpre();
			lac_assert(n);
			uint32_t d = reg()->get_div_d1ppre();
			lac_assert(d);
			return n / d;
		}

		static uint32_t get_hz_apb4()
		{
			uint32_t n = get_hz_sys_hpre();
			lac_assert(n);
			uint32_t d = reg()->get_div_d3ppre();
			lac_assert(d);
			return n / d;
		}

		static uint32_t get_hz_dac() { return get_hz_apb1(); }

		static uint32_t get_hz_usart16()
		{
			switch(reg()->get_mux_usart16())
			{
				case reg_h7_rcc::USART1_PCLK2: return get_hz_apb2();
				case reg_h7_rcc::USART1_PLL2Q: return get_hz_pll2q();
				case reg_h7_rcc::USART1_PLL3Q: return get_hz_pll3q();
				case reg_h7_rcc::USART1_HSI: return get_hz_hsi();
				case reg_h7_rcc::USART1_CSI: return get_hz_csi();
				case reg_h7_rcc::USART1_LSE: return get_hz_lse();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_usart234578()
		{
			switch(reg()->get_mux_usart234578())
			{
				case reg_h7_rcc::USART2_PCLK1: return get_hz_apb2();
				case reg_h7_rcc::USART2_PLL2Q: return get_hz_pll2q();
				case reg_h7_rcc::USART2_PLL3Q: return get_hz_pll3q();
				case reg_h7_rcc::USART2_HSI: return get_hz_hsi();
				case reg_h7_rcc::USART2_CSI: return get_hz_csi();
				case reg_h7_rcc::USART2_LSE: return get_hz_lse();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_usart1() { return get_hz_usart16(); }
		static uint32_t get_hz_usart2() { return get_hz_usart234578(); }
		static uint32_t get_hz_usart3() { return get_hz_usart234578(); }
		static uint32_t get_hz_uart4() { return get_hz_usart234578(); }
		static uint32_t get_hz_uart5() { return get_hz_usart234578(); }
		static uint32_t get_hz_usart6() { return get_hz_usart16(); }
		static uint32_t get_hz_uart7() { return get_hz_usart234578(); }
		static uint32_t get_hz_uart8() { return get_hz_usart234578(); }

		static uint32_t get_hz_lpuart1()
		{
			switch(reg()->get_mux_lpuart1())
			{
				case reg_h7_rcc::LPUART1_PCLK3: return get_hz_apb3();
				case reg_h7_rcc::LPUART1_PLL2Q: return get_hz_pll2q();
				case reg_h7_rcc::LPUART1_PLL3Q: return get_hz_pll3q();
				case reg_h7_rcc::LPUART1_HSI: return get_hz_hsi();
				case reg_h7_rcc::LPUART1_CSI: return get_hz_csi();
				case reg_h7_rcc::LPUART1_LSE: return get_hz_lse();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_lptim1()
		{
			switch(reg()->get_mux_lptim1())
			{
				case reg_h7_rcc::LPTIM1_PCLK1: return get_hz_apb1();
				case reg_h7_rcc::LPTIM1_PLL2P: return get_hz_pll2p();
				case reg_h7_rcc::LPTIM1_PLL3R: return get_hz_pll3r();
				case reg_h7_rcc::LPTIM1_LSE: return get_hz_lse();
				case reg_h7_rcc::LPTIM1_LSI: return get_hz_lsi();
				case reg_h7_rcc::LPTIM1_PER_CK: return get_hz_per_ck();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_lptim2()
		{
			switch(reg()->get_mux_lptim2())
			{
				case reg_h7_rcc::LPTIM2_PCLK4: return get_hz_apb4();
				case reg_h7_rcc::LPTIM2_PLL2P: return get_hz_pll2p();
				case reg_h7_rcc::LPTIM2_PLL3R: return get_hz_pll3r();
				case reg_h7_rcc::LPTIM2_LSE: return get_hz_lse();
				case reg_h7_rcc::LPTIM2_LSI: return get_hz_lsi();
				case reg_h7_rcc::LPTIM2_PER_CK: return get_hz_per_ck();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_lptim3()
		{
			switch(reg()->get_mux_lptim3())
			{
				case reg_h7_rcc::LPTIM3_PCLK4: return get_hz_apb4();
				case reg_h7_rcc::LPTIM3_PLL2P: return get_hz_pll2p();
				case reg_h7_rcc::LPTIM3_PLL3R: return get_hz_pll3r();
				case reg_h7_rcc::LPTIM3_LSE: return get_hz_lse();
				case reg_h7_rcc::LPTIM3_LSI: return get_hz_lsi();
				case reg_h7_rcc::LPTIM3_PER_CK: return get_hz_per_ck();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_lptim4() { return get_hz_lptim3(); }

		static uint32_t get_hz_lptim5() { return get_hz_lptim3(); }

		static uint32_t get_hz_adc()
		{
			switch(reg()->get_mux_adc())
			{
				case reg_h7_rcc::ADC_PLL2P: return get_hz_pll2p();
				case reg_h7_rcc::ADC_PLL3R: return get_hz_pll3r();
				case reg_h7_rcc::ADC_PER_CK: return get_hz_per_ck();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_usb()
		{
			switch(reg()->get_mux_usb())
			{
				case reg_h7_rcc::USB_DISABLE: return 0;
				case reg_h7_rcc::USB_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::USB_PLL3Q: return get_hz_pll3q();
				case reg_h7_rcc::USB_RC48: return get_hz_rc48();
				default: lac_assert(false); return 0;
			}
		}
		static uint32_t get_hz_cec()
		{
			switch(reg()->get_mux_cec())
			{
				case reg_h7_rcc::CEC_LSE: return get_hz_lse();
				case reg_h7_rcc::CEC_LSI: return get_hz_lsi();
				case reg_h7_rcc::CEC_CSI: return get_hz_csi();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_fdcan()
		{
			switch(reg()->get_mux_fdcan())
			{
				case reg_h7_rcc::FDCAN_HSE: return get_hz_hse();
				case reg_h7_rcc::FDCAN_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::FDCAN_PLL2Q: return get_hz_pll2q();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_hrtim()
		{
			switch(reg()->get_mux_hrtim())
			{
				case reg_h7_rcc::HRTIM_TIM2: return get_hz_apb2_tim();
				case reg_h7_rcc::HRTIM_CPU: return get_hz_cpu();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_sdmmc1()
		{
			switch(reg()->get_mux_sdmmc1())
			{
				case reg_h7_rcc::SDMMC1_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::SDMMC1_PLL2R: return get_hz_pll2r();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_sdmmc2() { return get_hz_sdmmc1(); }

		static uint32_t get_hz_dfsdm()
		{
			switch(reg()->get_mux_dfsdm())
			{
				case reg_h7_rcc::DFSDM_PCLK2: return get_hz_apb2();
				case reg_h7_rcc::DFSDM_SYSCLK: return get_hz_sysclk();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_swp()
		{
			switch(reg()->get_mux_swp())
			{
				case reg_h7_rcc::SWP_PCLK1: return get_hz_apb1();
				case reg_h7_rcc::SWP_HSI: return get_hz_hsi();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_ltdc() { return get_hz_pll3r(); }

		static uint32_t get_hz_quadspi()
		{
			switch(reg()->get_mux_quadspi())
			{
				case reg_h7_rcc::QUADSPI_HCLK3: return get_hz_hclk3();
				case reg_h7_rcc::QUADSPI_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::QUADSPI_PLL2R: return get_hz_pll2r();
				case reg_h7_rcc::QUADSPI_PER_CK: return get_hz_per_ck();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_fmc()
		{
			switch(reg()->get_mux_fmc())
			{
				case reg_h7_rcc::FMC_HCLK3: return get_hz_hclk3();
				case reg_h7_rcc::FMC_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::FMC_PLL2R: return get_hz_pll2r();
				case reg_h7_rcc::FMC_PER_CK: return get_hz_per_ck();
			}
		}

		static uint32_t get_hz_spdifrx1()
		{
			switch(reg()->get_mux_spdifrx1())
			{
				case reg_h7_rcc::SPDIFRX1_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::SPDIFRX1_PLL2R: return get_hz_pll2r();
				case reg_h7_rcc::SPDIFRX1_PLL3R: return get_hz_pll3r();
				case reg_h7_rcc::SPDIFRX1_HSI: return get_hz_hsi();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_i2c1()
		{
			switch(reg()->get_mux_i2c1())
			{
				case reg_h7_rcc::I2C1_PCLK1: return get_hz_apb1();
				case reg_h7_rcc::I2C1_PLL3R: return get_hz_pll3r();
				case reg_h7_rcc::I2C1_HSI: return get_hz_hsi();
				case reg_h7_rcc::I2C1_CSI: return get_hz_csi();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_i2c2() { return get_hz_i2c1(); }

		static uint32_t get_hz_i2c3() { return get_hz_i2c1(); }

		static uint32_t get_hz_i2c4()
		{
			switch(reg()->get_mux_i2c4())
			{
				case reg_h7_rcc::I2C4_PCLK4: return get_hz_apb1();
				case reg_h7_rcc::I2C4_PLL3R: return get_hz_pll3r();
				case reg_h7_rcc::I2C4_HSI: return get_hz_hsi();
				case reg_h7_rcc::I2C4_CSI: return get_hz_csi();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_rng()
		{
			switch(reg()->get_mux_rng())
			{
				case reg_h7_rcc::RNG_RC48: return get_hz_apb1();
				case reg_h7_rcc::RNG_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::RNG_LSE: return get_hz_lse();
				case reg_h7_rcc::RNG_LSI: return get_hz_lsi();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_sai1()
		{
			switch(reg()->get_mux_sai1())
			{
				case reg_h7_rcc::SAI1_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::SAI1_PLL2P: return get_hz_pll2p();
				case reg_h7_rcc::SAI1_PLL3P: return get_hz_pll3p();
				case reg_h7_rcc::SAI1_I2S_CKIN: return get_hz_i2sin();
				case reg_h7_rcc::SAI1_PER_CK: return get_hz_per_ck();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_sai2()
		{
			switch(reg()->get_mux_sai2())
			{
				case reg_h7_rcc::SAI2_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::SAI2_PLL2P: return get_hz_pll2p();
				case reg_h7_rcc::SAI2_PLL3P: return get_hz_pll3p();
				case reg_h7_rcc::SAI2_I2S_CKIN: return get_hz_i2sin();
				case reg_h7_rcc::SAI2_PER_CK: return get_hz_per_ck();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_sai3() { return get_hz_sai2(); }

		static uint32_t get_hz_sai4a()
		{
			switch(reg()->get_mux_sai4a())
			{
				case reg_h7_rcc::SAI4A_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::SAI4A_PLL2P: return get_hz_pll2p();
				case reg_h7_rcc::SAI4A_PLL3P: return get_hz_pll3p();
				case reg_h7_rcc::SAI4A_I2S_CKIN: return get_hz_i2sin();
				case reg_h7_rcc::SAI4A_PER_CK: return get_hz_per_ck();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_sai4b()
		{
			switch(reg()->get_mux_sai4b())
			{
				case reg_h7_rcc::SAI4B_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::SAI4B_PLL2P: return get_hz_pll2p();
				case reg_h7_rcc::SAI4B_PLL3P: return get_hz_pll3p();
				case reg_h7_rcc::SAI4B_I2S_CKIN: return get_hz_i2sin();
				case reg_h7_rcc::SAI4B_PER_CK: return get_hz_per_ck();
				default: lac_assert(false); return 0;
			}
		}

		static uint32_t get_hz_dfsdm_aclk() { return get_hz_sai1(); }

		static uint32_t get_hz_spi1()
		{
			switch(reg()->get_mux_spi1())
			{
				default: lac_assert(false); return 0;
				case reg_h7_rcc::SPI1_PLL1Q: return get_hz_pll1q();
				case reg_h7_rcc::SPI1_PLL2P: return get_hz_pll2p();
				case reg_h7_rcc::SPI1_PLL3P: return get_hz_pll3p();
				case reg_h7_rcc::SPI1_I2S_CKIN: return get_hz_i2sin();
				case reg_h7_rcc::SPI1_PER_CK: return get_hz_per_ck();
			}
		}

		static uint32_t get_hz_spi2() { return get_hz_spi1(); }
		static uint32_t get_hz_spi3() { return get_hz_spi1(); }

		static uint32_t get_hz_spi4()
		{
			switch(reg()->get_mux_spi4())
			{
				default: lac_assert(false); return 0;
				case reg_h7_rcc::SPI4_PCLK2: return get_hz_apb2();
				case reg_h7_rcc::SPI4_PLL2Q: return get_hz_pll2q();
				case reg_h7_rcc::SPI4_PLL3Q: return get_hz_pll3q();
				case reg_h7_rcc::SPI4_HSI: return get_hz_hsi();
				case reg_h7_rcc::SPI4_CSI: return get_hz_csi();
				case reg_h7_rcc::SPI4_HSE: return get_hz_hse();
			}
		}
		static uint32_t get_hz_spi5() { return get_hz_spi4(); }

		static uint32_t get_hz_spi6()
		{
			switch(reg()->get_mux_spi6())
			{
				default: lac_assert(false); return 0;
				case reg_h7_rcc::SPI6_PCLK4: return get_hz_apb4();
				case reg_h7_rcc::SPI6_PLL2Q: return get_hz_pll2q();
				case reg_h7_rcc::SPI6_PLL3Q: return get_hz_pll3q();
				case reg_h7_rcc::SPI6_HSI: return get_hz_hsi();
				case reg_h7_rcc::SPI6_CSI: return get_hz_csi();
				case reg_h7_rcc::SPI6_HSE: return get_hz_hse();
			}
		}

		static uint32_t get_hz_mco1_base()
		{
			switch(reg()->get_mux_mco1())
			{
				case reg_h7_rcc::MCO1_LSE: return get_hz_lse();
				case reg_h7_rcc::MCO1_HSE: return get_hz_hse();
				case reg_h7_rcc::MCO1_HSI: return get_hz_hsi();
				case reg_h7_rcc::MCO1_RC48: return get_hz_rc48();
				case reg_h7_rcc::MCO1_PLL1Q: return get_hz_pll1q();
			}
		}

		static uint32_t get_hz_mco1()
		{
			uint32_t n = get_hz_mco1_base();
			lac_assert(n);
			uint32_t d = reg()->get_div_mco1();
			lac_assert(d);
			return n / d;
		}

		static uint32_t get_hz_mco2_base()
		{
			switch(reg()->get_mux_mco2())
			{
				case reg_h7_rcc::MCO2_SYSCLK: return get_hz_sysclk();
				case reg_h7_rcc::MCO2_PLL2P: return get_hz_pll2p();
				case reg_h7_rcc::MCO2_HSE: return get_hz_hse();
				case reg_h7_rcc::MCO2_PLL1P: return get_hz_pll1p();
				case reg_h7_rcc::MCO2_CSI: return get_hz_csi();
				case reg_h7_rcc::MCO2_CSI: return get_hz_lsi();
			}
		}

		static uint32_t get_hz_mco2()
		{
			uint32_t n = get_hz_mco2_base();
			lac_assert(n);
			uint32_t d = reg()->get_div_mco2();
			lac_assert(d);
			return n / d;
		}

		static uint32_t get_hz_per_ck()
		{
			switch(reg()->get_mux_per())
			{
				default: lac_assert(false); return 0;
				case reg_h7_rcc::PER_HSI: return get_hz_hsi();
				case reg_h7_rcc::PER_CSI: return get_hz_csi();
				case reg_h7_rcc::PER_HSE: return get_hz_hse();
			}
		}
	};
} // namespace lac