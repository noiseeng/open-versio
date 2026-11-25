/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>

#include <utl/bitfield.h>
#include <utl/bitflag.h>
#include <utl/bits.h>
#include <utl/bug.h>

namespace lac
{
	struct reg_h7_rcc
	{
		volatile uint32_t CR;
		volatile uint32_t HSICFGR;
		volatile uint32_t CRRCR;
		volatile uint32_t CSICFGR;
		volatile uint32_t CFGR;
		volatile uint32_t RESERVED1;
		volatile uint32_t D1CFGR;
		volatile uint32_t D2CFGR;
		volatile uint32_t D3CFGR;
		volatile uint32_t RESERVED2;
		volatile uint32_t PLLCKSELR;
		volatile uint32_t PLLCFGR;
		volatile uint32_t PLL1DIVR;
		volatile uint32_t PLL1FRACR;
		volatile uint32_t PLL2DIVR;
		volatile uint32_t PLL2FRACR;
		volatile uint32_t PLL3DIVR;
		volatile uint32_t PLL3FRACR;
		volatile uint32_t RESERVED3;
		volatile uint32_t D1CCIPR;
		volatile uint32_t D2CCIP1R;
		volatile uint32_t D2CCIP2R;
		volatile uint32_t D3CCIPR;
		volatile uint32_t RESERVED4;
		volatile uint32_t CIER;
		volatile uint32_t CIFR;
		volatile uint32_t CICR;
		volatile uint32_t RESERVED5;
		volatile uint32_t BDCR;
		volatile uint32_t CSR;
		volatile uint32_t RESERVED6;
		volatile uint32_t AHB3RSTR;
		volatile uint32_t AHB1RSTR;
		volatile uint32_t AHB2RSTR;
		volatile uint32_t AHB4RSTR;
		volatile uint32_t APB3RSTR;
		volatile uint32_t APB1LRSTR;
		volatile uint32_t APB1HRSTR;
		volatile uint32_t APB2RSTR;
		volatile uint32_t APB4RSTR;
		volatile uint32_t GCR;
		volatile uint32_t RESERVED8;
		volatile uint32_t D3AMR;
		volatile uint32_t RESERVED11[9];
		volatile uint32_t RSR;
		volatile uint32_t AHB3ENR;
		volatile uint32_t AHB1ENR;
		volatile uint32_t AHB2ENR;
		volatile uint32_t AHB4ENR;
		volatile uint32_t APB3ENR;
		volatile uint32_t APB1LENR;
		volatile uint32_t APB1HENR;
		volatile uint32_t APB2ENR;
		volatile uint32_t APB4ENR;
		volatile uint32_t RESERVED12;
		volatile uint32_t AHB3LPENR;
		volatile uint32_t AHB1LPENR;
		volatile uint32_t AHB2LPENR;
		volatile uint32_t AHB4LPENR;
		volatile uint32_t APB3LPENR;
		volatile uint32_t APB1LLPENR;
		volatile uint32_t APB1HLPENR;
		volatile uint32_t APB2LPENR;
		volatile uint32_t APB4LPENR;
		volatile uint32_t RESERVED13[4];

		using CR_HSION	  = bitflag<0>;
		using CR_HSIKERON = bitflag<1>;
		using CR_HSIRDY	  = bitflag<2>;
		using CR_HSIDIV	  = bitfield<3, 2>;
		using CR_HSIDIVF  = bitflag<5>;
		using CR_CSION	  = bitflag<7>;
		using CR_CSIRDY	  = bitflag<8>;
		using CR_CSIKERON = bitflag<9>;
		using CR_HSI48ON  = bitflag<12>;
		using CR_HSI48RDY = bitflag<13>;
		using CR_D1CKRDY  = bitflag<14>;
		using CR_D2CKRDY  = bitflag<15>;
		using CR_HSEON	  = bitflag<16>;
		using CR_HSERDY	  = bitflag<17>;
		using CR_HSEBYP	  = bitflag<18>;
		using CR_HSECSSON = bitflag<19>;
		using CR_PLL1ON	  = bitflag<24>;
		using CR_PLL1RDY  = bitflag<25>;
		using CR_PLL2ON	  = bitflag<26>;
		using CR_PLL2RDY  = bitflag<27>;
		using CR_PLL3ON	  = bitflag<28>;
		using CR_PLL3RDY  = bitflag<29>;

		using HSICFGR_HSICAL  = bitfield<0, 12>;
		using HSICFGR_HSITRIM = bitfield<24, 7>;

		using CRRCR_HSI48CAL = bitfield<0, 10>;

		using CSICFGR_CSICAL  = bitfield<0, 10>;
		using CSICFGR_CSITRIM = bitfield<24, 6>;

		using CFGR_SW		   = bitfield<0, 3>;
		using CFGR_SWS		   = bitfield<3, 3>;
		using CFGR_STOPWUCK	   = bitflag<6>;
		using CFGR_STOPKERWUCK = bitflag<7>;
		using CFGR_RTCPRE	   = bitfield<8, 6>;
		using CFGR_HRTIMSEL	   = bitflag<14>;
		using CFGR_TIMPRE	   = bitflag<15>;
		using CFGR_MCO1PRE	   = bitfield<18, 4>;
		using CFGR_MCO1		   = bitfield<22, 3>;
		using CFGR_MCO2PRE	   = bitfield<25, 4>;
		using CFGR_MCO2		   = bitfield<29, 3>;

		using D1CFGR_HPRE	= bitfield<0, 4>;
		using D1CFGR_D1PPRE = bitfield<4, 3>;
		using D1CFGR_D1CPRE = bitfield<8, 4>;

		using D2CFGR_D2PPRE1 = bitfield<4, 3>;
		using D2CFGR_D2PPRE2 = bitfield<8, 3>;

		using D3CFGR_D3PPRE = bitfield<4, 3>;

		using PLLCKSELR_PLLSRC = bitfield<0, 2>;
		using PLLCKSELR_DIVM1  = bitfield<4, 6>;
		using PLLCKSELR_DIVM2  = bitfield<12, 6>;
		using PLLCKSELR_DIVM3  = bitfield<20, 6>;

		using PLLCFGR_PLL1FRACEN = bitflag<0>;
		using PLLCFGR_PLL1VCOSEL = bitflag<1>;
		using PLLCFGR_PLL1RGE	 = bitfield<2, 2>;
		using PLLCFGR_PLL2FRACEN = bitflag<4>;
		using PLLCFGR_PLL2VCOSEL = bitflag<5>;
		using PLLCFGR_PLL2RGE	 = bitfield<6, 2>;
		using PLLCFGR_PLL3FRACEN = bitflag<8>;
		using PLLCFGR_PLL3VCOSEL = bitflag<9>;
		using PLLCFGR_PLL3RGE	 = bitfield<10, 2>;
		using PLLCFGR_DIVP1EN	 = bitflag<16>;
		using PLLCFGR_DIVQ1EN	 = bitflag<17>;
		using PLLCFGR_DIVR1EN	 = bitflag<18>;
		using PLLCFGR_DIVP2EN	 = bitflag<19>;
		using PLLCFGR_DIVQ2EN	 = bitflag<20>;
		using PLLCFGR_DIVR2EN	 = bitflag<21>;
		using PLLCFGR_DIVP3EN	 = bitflag<22>;
		using PLLCFGR_DIVQ3EN	 = bitflag<23>;
		using PLLCFGR_DIVR3EN	 = bitflag<24>;

		using PLL1DIVR_DIVN1 = bitfield<0, 9>;
		using PLL1DIVR_DIVP1 = bitfield<9, 7>;
		using PLL1DIVR_DIVQ1 = bitfield<16, 7>;
		using PLL1DIVR_DIVR1 = bitfield<24, 7>;

		using PLL1FRACR_FRACN1 = bitfield<3, 13>;

		using PLL2DIVR_DIVN2 = bitfield<0, 9>;
		using PLL2DIVR_DIVP2 = bitfield<9, 7>;
		using PLL2DIVR_DIVQ2 = bitfield<16, 7>;
		using PLL2DIVR_DIVR2 = bitfield<24, 7>;

		using PLL2FRACR_FRACN2 = bitfield<3, 13>;

		using PLL3DIVR_DIVN3 = bitfield<0, 9>;
		using PLL3DIVR_DIVP3 = bitfield<9, 7>;
		using PLL3DIVR_DIVQ3 = bitfield<16, 7>;
		using PLL3DIVR_DIVR3 = bitfield<24, 7>;

		using PLL3FRACR_FRACN3 = bitfield<3, 13>;

		using D1CCIPR_FMCSEL   = bitfield<0, 2>;
		using D1CCIPR_QSPISEL  = bitfield<4, 2>;
		using D1CCIPR_SDMMCSEL = bitflag<16>;
		using D1CCIPR_CKPERSEL = bitfield<28, 2>;

		using D2CCIP1R_SAI1SEL	 = bitfield<0, 3>;
		using D2CCIP1R_SAI23SEL	 = bitfield<6, 3>;
		using D2CCIP1R_SPI123SEL = bitfield<12, 3>;
		using D2CCIP1R_SPI45SEL	 = bitfield<16, 3>;
		using D2CCIP1R_SPDIFSEL	 = bitfield<20, 2>;
		using D2CCIP1R_DFSDM1SEL = bitflag<24>;
		using D2CCIP1R_FDCANSEL	 = bitfield<28, 2>;
		using D2CCIP1R_SWPSEL	 = bitflag<31>;

		using D2CCIP2R_USART234578SEL = bitfield<0, 3>;
		using D2CCIP2R_USART16SEL	  = bitfield<3, 3>;
		using D2CCIP2R_RNGSEL		  = bitfield<8, 2>;
		using D2CCIP2R_I2C123SEL	  = bitfield<12, 2>;
		using D2CCIP2R_USBSEL		  = bitfield<20, 2>;
		using D2CCIP2R_CECSEL		  = bitfield<22, 2>;
		using D2CCIP2R_LPTIM1SEL	  = bitfield<28, 3>;

		using D3CCIPR_LPUART1SEL  = bitfield<0, 3>;
		using D3CCIPR_I2C4SEL	  = bitfield<8, 2>;
		using D3CCIPR_LPTIM2SEL	  = bitfield<10, 3>;
		using D3CCIPR_LPTIM345SEL = bitfield<13, 3>;
		using D3CCIPR_ADCSEL	  = bitfield<16, 2>;
		using D3CCIPR_SAI4ASEL	  = bitfield<21, 3>;
		using D3CCIPR_SAI4BSEL	  = bitfield<24, 3>;
		using D3CCIPR_SPI6SEL	  = bitfield<28, 3>;

		using CIER_LSIRDYIE	  = bitflag<0>;
		using CIER_LSERDYIE	  = bitflag<1>;
		using CIER_HSIRDYIE	  = bitflag<2>;
		using CIER_HSERDYIE	  = bitflag<3>;
		using CIER_CSIRDYIE	  = bitflag<4>;
		using CIER_HSI48RDYIE = bitflag<5>;
		using CIER_PLL1RDYIE  = bitflag<6>;
		using CIER_PLL2RDYIE  = bitflag<7>;
		using CIER_PLL3RDYIE  = bitflag<8>;
		using CIER_LSECSSIE	  = bitflag<9>;

		using CIFR_LSIRDYF	 = bitflag<0>;
		using CIFR_LSERDYF	 = bitflag<1>;
		using CIFR_HSIRDYF	 = bitflag<2>;
		using CIFR_HSERDYF	 = bitflag<3>;
		using CIFR_CSIRDYF	 = bitflag<4>;
		using CIFR_HSI48RDYF = bitflag<5>;
		using CIFR_PLL1RDYF	 = bitflag<6>;
		using CIFR_PLL2RDYF	 = bitflag<7>;
		using CIFR_PLL3RDYF	 = bitflag<8>;
		using CIFR_LSECSSF	 = bitflag<9>;
		using CIFR_HSECSSF	 = bitflag<10>;

		using CICR_LSIRDYC	 = bitflag<0>;
		using CICR_LSERDYC	 = bitflag<1>;
		using CICR_HSIRDYC	 = bitflag<2>;
		using CICR_HSERDYC	 = bitflag<3>;
		using CICR_CSIRDYC	 = bitflag<4>;
		using CICR_HSI48RDYC = bitflag<5>;
		using CICR_PLL1RDYC	 = bitflag<6>;
		using CICR_PLL2RDYC	 = bitflag<7>;
		using CICR_PLL3RDYC	 = bitflag<8>;
		using CICR_LSECSSC	 = bitflag<9>;
		using CICR_HSECSSC	 = bitflag<10>;

		using BDCR_LSEON	= bitflag<0>;
		using BDCR_LSERDY	= bitflag<1>;
		using BDCR_LSEBYP	= bitflag<2>;
		using BDCR_LSEDRV	= bitfield<3, 2>;
		using BDCR_LSECSSON = bitflag<5>;
		using BDCR_LSECSSD	= bitflag<6>;
		using BDCR_RTCSEL	= bitfield<8, 2>;
		using BDCR_RTCEN	= bitflag<15>;
		using BDCR_BDRST	= bitflag<16>;

		using CSR_LSION	 = bitflag<0>;
		using CSR_LSIRDY = bitflag<1>;

		using AHB3RSTR_MDMARST	 = bitflag<0>;
		using AHB3RSTR_DMA2DRST	 = bitflag<4>;
		using AHB3RSTR_JPGDECRST = bitflag<5>;
		using AHB3RSTR_FMCRST	 = bitflag<12>;
		using AHB3RSTR_QSPIRST	 = bitflag<14>;
		using AHB3RSTR_SDMMC1RST = bitflag<16>;

		using AHB1RSTR_DMA1RST	  = bitflag<0>;
		using AHB1RSTR_DMA2RST	  = bitflag<1>;
		using AHB1RSTR_ADC12RST	  = bitflag<5>;
		using AHB1RSTR_ETH1MACRST = bitflag<15>;
		using AHB1RSTR_USB1OTGRST = bitflag<25>;
		using AHB1RSTR_USB2OTGRST = bitflag<27>;

		using AHB2RSTR_CAMITFRST = bitflag<0>;
		using AHB2RSTR_CRYPTRST	 = bitflag<4>;
		using AHB2RSTR_HASHRST	 = bitflag<5>;
		using AHB2RSTR_RNGRST	 = bitflag<6>;
		using AHB2RSTR_SDMMC2RST = bitflag<9>;

		using AHB4RSTR_GPIOARST = bitflag<0>;
		using AHB4RSTR_GPIOBRST = bitflag<1>;
		using AHB4RSTR_GPIOCRST = bitflag<2>;
		using AHB4RSTR_GPIODRST = bitflag<3>;
		using AHB4RSTR_GPIOERST = bitflag<4>;
		using AHB4RSTR_GPIOFRST = bitflag<5>;
		using AHB4RSTR_GPIOGRST = bitflag<6>;
		using AHB4RSTR_GPIOHRST = bitflag<7>;
		using AHB4RSTR_GPIOIRST = bitflag<8>;
		using AHB4RSTR_GPIOJRST = bitflag<9>;
		using AHB4RSTR_GPIOKRST = bitflag<10>;
		using AHB4RSTR_CRCRST	= bitflag<11>;
		using AHB4RSTR_BDMARST	= bitflag<12>;
		using AHB4RSTR_ADC3RST	= bitflag<13>;
		using AHB4RSTR_HSEMRST	= bitflag<14>;

		using APB3RSTR_LTDCRST = bitflag<3>;

		using APB1LRSTR_TIM2RST	   = bitflag<0>;
		using APB1LRSTR_TIM3RST	   = bitflag<1>;
		using APB1LRSTR_TIM4RST	   = bitflag<2>;
		using APB1LRSTR_TIM5RST	   = bitflag<3>;
		using APB1LRSTR_TIM6RST	   = bitflag<4>;
		using APB1LRSTR_TIM7RST	   = bitflag<5>;
		using APB1LRSTR_TIM12RST   = bitflag<6>;
		using APB1LRSTR_TIM13RST   = bitflag<7>;
		using APB1LRSTR_TIM14RST   = bitflag<8>;
		using APB1LRSTR_LPTIM1RST  = bitflag<9>;
		using APB1LRSTR_SPI2RST	   = bitflag<14>;
		using APB1LRSTR_SPI3RST	   = bitflag<15>;
		using APB1LRSTR_SPDIFRXRST = bitflag<16>;
		using APB1LRSTR_USART2RST  = bitflag<17>;
		using APB1LRSTR_USART3RST  = bitflag<18>;
		using APB1LRSTR_UART4RST   = bitflag<19>;
		using APB1LRSTR_UART5RST   = bitflag<20>;
		using APB1LRSTR_I2C1RST	   = bitflag<21>;
		using APB1LRSTR_I2C2RST	   = bitflag<22>;
		using APB1LRSTR_I2C3RST	   = bitflag<23>;
		using APB1LRSTR_CECRST	   = bitflag<27>;
		using APB1LRSTR_DAC12RST   = bitflag<29>;
		using APB1LRSTR_UART7RST   = bitflag<30>;
		using APB1LRSTR_UART8RST   = bitflag<31>;

		using APB1HRSTR_CRSRST	 = bitflag<1>;
		using APB1HRSTR_SWPRST	 = bitflag<2>;
		using APB1HRSTR_OPAMPRST = bitflag<4>;
		using APB1HRSTR_MDIOSRST = bitflag<5>;
		using APB1HRSTR_FDCANRST = bitflag<8>;

		using APB2RSTR_TIM1RST	 = bitflag<0>;
		using APB2RSTR_TIM8RST	 = bitflag<1>;
		using APB2RSTR_USART1RST = bitflag<4>;
		using APB2RSTR_USART6RST = bitflag<5>;
		using APB2RSTR_SPI1RST	 = bitflag<12>;
		using APB2RSTR_SPI4RST	 = bitflag<13>;
		using APB2RSTR_TIM15RST	 = bitflag<16>;
		using APB2RSTR_TIM16RST	 = bitflag<17>;
		using APB2RSTR_TIM17RST	 = bitflag<18>;
		using APB2RSTR_SPI5RST	 = bitflag<20>;
		using APB2RSTR_SAI1RST	 = bitflag<22>;
		using APB2RSTR_SAI2RST	 = bitflag<23>;
		using APB2RSTR_SAI3RST	 = bitflag<24>;
		using APB2RSTR_DFSDM1RST = bitflag<28>;
		using APB2RSTR_HRTIMRST	 = bitflag<29>;

		using APB4RSTR_SYSCFGRST  = bitflag<1>;
		using APB4RSTR_LPUART1RST = bitflag<3>;
		using APB4RSTR_SPI6RST	  = bitflag<5>;
		using APB4RSTR_I2C4RST	  = bitflag<7>;
		using APB4RSTR_LPTIM2RST  = bitflag<9>;
		using APB4RSTR_LPTIM3RST  = bitflag<10>;
		using APB4RSTR_LPTIM4RST  = bitflag<11>;
		using APB4RSTR_LPTIM5RST  = bitflag<12>;
		using APB4RSTR_COMP12RST  = bitflag<14>;
		using APB4RSTR_VREFRST	  = bitflag<15>;
		using APB4RSTR_SAI4RST	  = bitflag<21>;

		using GCR_WW1RSC  = bitflag<0>;
		using GCR_WW2RSC  = bitflag<1>;
		using GCR_BOOT_C1 = bitflag<2>;
		using GCR_BOOT_C2 = bitflag<3>;

		using D3AMR_BDMAAMEN	= bitflag<0>;
		using D3AMR_LPUART1AMEN = bitflag<3>;
		using D3AMR_SPI6AMEN	= bitflag<5>;
		using D3AMR_I2C4AMEN	= bitflag<7>;
		using D3AMR_LPTIM2AMEN	= bitflag<9>;
		using D3AMR_LPTIM3AMEN	= bitflag<10>;
		using D3AMR_LPTIM4AMEN	= bitflag<11>;
		using D3AMR_LPTIM5AMEN	= bitflag<12>;
		using D3AMR_COMP12AMEN	= bitflag<14>;
		using D3AMR_VREFAMEN	= bitflag<15>;
		using D3AMR_RTCAMEN		= bitflag<16>;
		using D3AMR_CRCAMEN		= bitflag<19>;
		using D3AMR_SAI4AMEN	= bitflag<21>;
		using D3AMR_ADC3AMEN	= bitflag<24>;
		using D3AMR_BKPRAMAMEN	= bitflag<28>;
		using D3AMR_SRAM4AMEN	= bitflag<29>;

		using RSR_RMVF		= bitflag<16>;
		using RSR_CPURSTF	= bitflag<17>;
		using RSR_CPU2RSTF	= bitflag<18>;
		using RSR_D1RSTF	= bitflag<19>;
		using RSR_D2RSTF	= bitflag<20>;
		using RSR_BORRSTF	= bitflag<21>;
		using RSR_PINRSTF	= bitflag<22>;
		using RSR_PORRSTF	= bitflag<23>;
		using RSR_SFTRSTF	= bitflag<24>;
		using RSR_IWDG1RSTF = bitflag<26>;
		using RSR_IWDG2RSTF = bitflag<27>;
		using RSR_WWDG1RSTF = bitflag<28>;
		using RSR_WWDG2RSTF = bitflag<29>;
		using RSR_LPWRRSTF	= bitflag<30>;

		using AHB3ENR_MDMAEN   = bitflag<0>;
		using AHB3ENR_DMA2DEN  = bitflag<4>;
		using AHB3ENR_JPGDECEN = bitflag<5>;
		using AHB3ENR_FMCEN	   = bitflag<12>;
		using AHB3ENR_QSPIEN   = bitflag<14>;
		using AHB3ENR_SDMMC1EN = bitflag<16>;

		using AHB1ENR_DMA1EN		  = bitflag<0>;
		using AHB1ENR_DMA2EN		  = bitflag<1>;
		using AHB1ENR_ADC12EN		  = bitflag<5>;
		using AHB1ENR_ETH1MACEN		  = bitflag<15>;
		using AHB1ENR_ETH1TXEN		  = bitflag<16>;
		using AHB1ENR_ETH1RXEN		  = bitflag<17>;
		using AHB1ENR_USB2OTGHSULPIEN = bitflag<18>;
		using AHB1ENR_USB1OTGHSEN	  = bitflag<25>;
		using AHB1ENR_USB1OTGHSULPIEN = bitflag<26>;
		using AHB1ENR_USB2OTGHSEN	  = bitflag<27>;

		using AHB2ENR_DCMIEN   = bitflag<0>;
		using AHB2ENR_CRYPTEN  = bitflag<4>;
		using AHB2ENR_HASHEN   = bitflag<5>;
		using AHB2ENR_RNGEN	   = bitflag<6>;
		using AHB2ENR_SDMMC1EN = bitflag<9>;
		using AHB2ENR_SRAM1EN  = bitflag<29>;
		using AHB2ENR_SRAM2EN  = bitflag<30>;
		using AHB2ENR_SRAM3EN  = bitflag<31>;

		using AHB4ENR_GPIOAEN  = bitflag<0>;
		using AHB4ENR_GPIOBEN  = bitflag<1>;
		using AHB4ENR_GPIOCEN  = bitflag<2>;
		using AHB4ENR_GPIODEN  = bitflag<3>;
		using AHB4ENR_GPIOEEN  = bitflag<4>;
		using AHB4ENR_GPIOFEN  = bitflag<5>;
		using AHB4ENR_GPIOGEN  = bitflag<6>;
		using AHB4ENR_GPIOHEN  = bitflag<7>;
		using AHB4ENR_GPIOIEN  = bitflag<8>;
		using AHB4ENR_GPIOJEN  = bitflag<9>;
		using AHB4ENR_GPIOKEN  = bitflag<10>;
		using AHB4ENR_CRCEN	   = bitflag<19>;
		using AHB4ENR_BDMAEN   = bitflag<21>;
		using AHB4ENR_ADC3EN   = bitflag<24>;
		using AHB4ENR_HSEMEN   = bitflag<25>;
		using AHB4ENR_BKPRAMEN = bitflag<28>;

		using APB3ENR_LTDCEN  = bitflag<3>;
		using APB3ENR_WWDG1EN = bitflag<6>;

		using APB1LENR_TIM2EN	 = bitflag<0>;
		using APB1LENR_TIM3EN	 = bitflag<1>;
		using APB1LENR_TIM4EN	 = bitflag<2>;
		using APB1LENR_TIM5EN	 = bitflag<3>;
		using APB1LENR_TIM6EN	 = bitflag<4>;
		using APB1LENR_TIM7EN	 = bitflag<5>;
		using APB1LENR_TIM12EN	 = bitflag<6>;
		using APB1LENR_TIM13EN	 = bitflag<7>;
		using APB1LENR_TIM14EN	 = bitflag<8>;
		using APB1LENR_LPTIM1EN	 = bitflag<9>;
		using APB1LENR_SPI2EN	 = bitflag<14>;
		using APB1LENR_SPI3EN	 = bitflag<15>;
		using APB1LENR_SPDIFRXEN = bitflag<16>;
		using APB1LENR_USART2EN	 = bitflag<17>;
		using APB1LENR_USART3EN	 = bitflag<18>;
		using APB1LENR_UART4EN	 = bitflag<19>;
		using APB1LENR_UART5EN	 = bitflag<20>;
		using APB1LENR_I2C1EN	 = bitflag<21>;
		using APB1LENR_I2C2EN	 = bitflag<22>;
		using APB1LENR_I2C3EN	 = bitflag<23>;
		using APB1LENR_CECEN	 = bitflag<27>;
		using APB1LENR_DAC12EN	 = bitflag<29>;
		using APB1LENR_UART17EN	 = bitflag<30>;
		using APB1LENR_UART18EN	 = bitflag<31>;

		using AHB1ENR_CSREN	  = bitflag<1>;
		using AHB1ENR_SWPEN	  = bitflag<2>;
		using AHB1ENR_OPAMPEN = bitflag<4>;
		using AHB1ENR_MDIOSEN = bitflag<5>;
		using AHB1ENR_FDCANEN = bitflag<8>;

		using APB2ENR_TIM1EN   = bitflag<0>;
		using APB2ENR_TIM8EN   = bitflag<1>;
		using APB2ENR_USART1EN = bitflag<4>;
		using APB2ENR_USART6EN = bitflag<5>;
		using APB2ENR_SPI1EN   = bitflag<12>;
		using APB2ENR_SPI4EN   = bitflag<13>;
		using APB2ENR_TIM15EN  = bitflag<16>;
		using APB2ENR_TIM16EN  = bitflag<17>;
		using APB2ENR_TIM17EN  = bitflag<18>;
		using APB2ENR_SPI5EN   = bitflag<20>;
		using APB2ENR_SAI1EN   = bitflag<22>;
		using APB2ENR_SAI2EN   = bitflag<23>;
		using APB2ENR_SAI3EN   = bitflag<24>;
		using APB2ENR_DFSDM1EN = bitflag<28>;
		using APB2ENR_HRTIMEN  = bitflag<29>;

		using APB4ENR_SYSCFGEN	= bitflag<1>;
		using APB4ENR_LPUART1EN = bitflag<3>;
		using APB4ENR_SPI6EN	= bitflag<5>;
		using APB4ENR_I2C4EN	= bitflag<7>;
		using APB4ENR_LPTIM2EN	= bitflag<9>;
		using APB4ENR_LPTIM3EN	= bitflag<10>;
		using APB4ENR_LPTIM4EN	= bitflag<11>;
		using APB4ENR_LPTIM5EN	= bitflag<12>;
		using APB4ENR_COMP12EN	= bitflag<14>;
		using APB4ENR_VREFEN	= bitflag<15>;
		using APB4ENR_RTCAPBEN	= bitflag<16>;
		using APB4ENR_SAI4EN	= bitflag<21>;

		using AHB3LPENR_MDMALPEN	= bitflag<0>;
		using AHB3LPENR_DMA2DLPEN	= bitflag<4>;
		using AHB3LPENR_JPGDECLPEN	= bitflag<5>;
		using AHB3LPENR_FLASHLPEN	= bitflag<8>;
		using AHB3LPENR_FMCLPEN		= bitflag<12>;
		using AHB3LPENR_QSPILPEN	= bitflag<14>;
		using AHB3LPENR_SDMMC1LPEN	= bitflag<16>;
		using AHB3LPENR_DTCM1LPEN	= bitflag<28>;
		using AHB3LPENR_DTCM2LPEN	= bitflag<29>;
		using AHB3LPENR_ITCMLPEN	= bitflag<30>;
		using AHB3LPENR_AXISRAMLPEN = bitflag<31>;

		using AHB1LPENR_DMA1LPEN		  = bitflag<0>;
		using AHB1LPENR_DMA2LPEN		  = bitflag<1>;
		using AHB1LPENR_ADC12LPEN		  = bitflag<5>;
		using AHB1LPENR_ETH1MACLPEN		  = bitflag<15>;
		using AHB1LPENR_ETH1TXLPEN		  = bitflag<16>;
		using AHB1LPENR_ETH1RXLPEN		  = bitflag<17>;
		using AHB1LPENR_USB1OTGHSLPEN	  = bitflag<25>;
		using AHB1LPENR_USB1OTGHSULPILPEN = bitflag<26>;
		using AHB1LPENR_USB2OTGHSLPEN	  = bitflag<27>;
		using AHB1LPENR_USB2OTGHSULPILPEN = bitflag<28>;

		using AHB2LPENR_CAMITFLPEN = bitflag<0>;
		using AHB2LPENR_CRYPTLPEN  = bitflag<4>;
		using AHB2LPENR_HASHLPEN   = bitflag<5>;
		using AHB2LPENR_RNGLPEN	   = bitflag<6>;
		using AHB2LPENR_SDMMC2LPEN = bitflag<9>;
		using AHB2LPENR_SRAM1LPEN  = bitflag<29>;
		using AHB2LPENR_SRAM2LPEN  = bitflag<30>;
		using AHB2LPENR_SRAM3LPEN  = bitflag<31>;

		using AHB4LPENR_GPIOALPEN = bitflag<0>;
		using AHB4LPENR_GPIOBLPEN = bitflag<1>;
		using AHB4LPENR_GPIOCLPEN = bitflag<2>;
		using AHB4LPENR_GPIODLPEN = bitflag<3>;
		using AHB4LPENR_GPIOELPEN = bitflag<4>;
		using AHB4LPENR_GPIOFLPEN = bitflag<5>;
		using AHB4LPENR_GPIOGLPEN = bitflag<6>;
		using AHB4LPENR_GPIOHLPEN = bitflag<7>;
		using AHB4LPENR_GPIOILPEN = bitflag<8>;
		using AHB4LPENR_GPIOJLPEN = bitflag<9>;
		using AHB4LPENR_GPIOKLPEN = bitflag<10>;

		using APB3LPENR_LTDCLPEN  = bitflag<3>;
		using APB3LPENR_WWDG1LPEN = bitflag<6>;

		using APB1LLPENR_TIM2LPEN	 = bitflag<0>;
		using APB1LLPENR_TIM3LPEN	 = bitflag<1>;
		using APB1LLPENR_TIM4LPEN	 = bitflag<2>;
		using APB1LLPENR_TIM5LPEN	 = bitflag<3>;
		using APB1LLPENR_TIM6LPEN	 = bitflag<4>;
		using APB1LLPENR_TIM7LPEN	 = bitflag<5>;
		using APB1LLPENR_TIM12LPEN	 = bitflag<6>;
		using APB1LLPENR_TIM13LPEN	 = bitflag<7>;
		using APB1LLPENR_TIM14LPEN	 = bitflag<8>;
		using APB1LLPENR_LPTIM1LPEN	 = bitflag<9>;
		using APB1LLPENR_SPI2LPEN	 = bitflag<14>;
		using APB1LLPENR_SPI3LPEN	 = bitflag<15>;
		using APB1LLPENR_SPDIFRXLPEN = bitflag<16>;
		using APB1LLPENR_USART2LPEN	 = bitflag<17>;
		using APB1LLPENR_USART3LPEN	 = bitflag<18>;
		using APB1LLPENR_UART4LPEN	 = bitflag<19>;
		using APB1LLPENR_UART5LPEN	 = bitflag<20>;
		using APB1LLPENR_I2C1LPEN	 = bitflag<21>;
		using APB1LLPENR_I2C2LPEN	 = bitflag<22>;
		using APB1LLPENR_I2C3LPEN	 = bitflag<22>;
		using APB1LLPENR_CECLPEN	 = bitflag<27>;
		using APB1LLPENR_DAC12LPEN	 = bitflag<29>;
		using APB1LLPENR_UART7LPEN	 = bitflag<30>;
		using APB1LLPENR_UART8LPEN	 = bitflag<31>;

		using APB1HLPENR_CRSLPEN   = bitflag<1>;
		using APB1HLPENR_SWPLPEN   = bitflag<2>;
		using APB1HLPENR_OPAMPLPEN = bitflag<4>;
		using APB1HLPENR_MDIOSLPEN = bitflag<5>;
		using APB1HLPENR_FDCANLPEN = bitflag<8>;

		using APB2LPENR_TIM1LPEN   = bitflag<0>;
		using APB2LPENR_TIM8LPEN   = bitflag<1>;
		using APB2LPENR_USART1LPEN = bitflag<4>;
		using APB2LPENR_USART6LPEN = bitflag<5>;
		using APB2LPENR_SPI1LPEN   = bitflag<12>;
		using APB2LPENR_SPI4LPEN   = bitflag<13>;
		using APB2LPENR_TIM15LPEN  = bitflag<16>;
		using APB2LPENR_TIM16LPEN  = bitflag<17>;
		using APB2LPENR_TIM17LPEN  = bitflag<18>;
		using APB2LPENR_SPI5LPEN   = bitflag<20>;
		using APB2LPENR_SAI1LPEN   = bitflag<22>;
		using APB2LPENR_SAI2LPEN   = bitflag<23>;
		using APB2LPENR_SAI3LPEN   = bitflag<24>;
		using APB2LPENR_DFSDM1LPEN = bitflag<28>;
		using APB2LPENR_HRTIMLPEN  = bitflag<29>;

		using APB4LPENR_SYSCFGLPEN	= bitflag<1>;
		using APB4LPENR_LPUART1LPEN = bitflag<3>;
		using APB4LPENR_SPI6LPEN	= bitflag<5>;
		using APB4LPENR_I2C4LPEN	= bitflag<7>;
		using APB4LPENR_LPTIM2LPEN	= bitflag<9>;
		using APB4LPENR_LPTIM3LPEN	= bitflag<10>;
		using APB4LPENR_LPTIM4LPEN	= bitflag<11>;
		using APB4LPENR_LPTIM5LPEN	= bitflag<12>;
		using APB4LPENR_COMP12LPEN	= bitflag<14>;
		using APB4LPENR_VREFLPEN	= bitflag<15>;
		using APB4LPENR_RTCAPBLPEN	= bitflag<16>;
		using APB4LPENR_SAI4LPEN	= bitflag<21>;

		void enable_csi() { CR_CSION::set(CR); }

		void enable_adc12() { AHB1ENR_ADC12EN::setdly(AHB1ENR); }
		void enable_adc3() { AHB4ENR_ADC3EN::setdly(AHB4ENR); }
		void enable_dac12() { APB1LENR_DAC12EN::setdly(APB1LENR); }

		void enable_tim1() { APB2ENR_TIM1EN::setdly(APB2ENR); }
		void enable_tim2() { APB1LENR_TIM2EN::setdly(APB1LENR); }
		void enable_tim3() { APB1LENR_TIM3EN::setdly(APB1LENR); }
		void enable_tim4() { APB1LENR_TIM4EN::setdly(APB1LENR); }
		void enable_tim5() { APB1LENR_TIM5EN::setdly(APB1LENR); }
		void enable_tim6() { APB1LENR_TIM6EN::setdly(APB1LENR); }
		void enable_tim7() { APB1LENR_TIM7EN::setdly(APB1LENR); }
		void enable_tim8() { APB2ENR_TIM8EN::setdly(APB2ENR); }
		void enable_tim12() { APB1LENR_TIM12EN::setdly(APB1LENR); }
		void enable_tim13() { APB1LENR_TIM13EN::setdly(APB1LENR); }
		void enable_tim14() { APB1LENR_TIM14EN::setdly(APB1LENR); }
		void enable_tim15() { APB2ENR_TIM15EN::setdly(APB2ENR); }
		void enable_tim16() { APB2ENR_TIM16EN::setdly(APB2ENR); }
		void enable_tim17() { APB2ENR_TIM17EN::setdly(APB2ENR); }
		void enable_lptim1() { APB1LENR_LPTIM1EN::setdly(APB1LENR); }

		void enable_dma1() { AHB1ENR_DMA1EN::setdly(AHB1ENR); }
		void enable_dma2() { AHB1ENR_DMA2EN::setdly(AHB1ENR); }
		void enable_sai1() { APB2ENR_SAI1EN::setdly(APB2ENR); }
		void enable_sai2() { APB2ENR_SAI2EN::setdly(APB2ENR); }
		void enable_sai3() { APB2ENR_SAI3EN::setdly(APB2ENR); }
		void enable_i2c1() { APB1LENR_I2C1EN::setdly(APB1LENR); }
		void enable_i2c2() { APB1LENR_I2C2EN::setdly(APB1LENR); }
		void enable_i2c3() { APB1LENR_I2C3EN::setdly(APB1LENR); }
		void enable_i2c4() { APB4ENR_I2C4EN::setdly(APB4ENR); }
		void enable_usart1() { APB2ENR_USART1EN::setdly(APB2ENR); }
		void enable_usart2() { APB1LENR_USART2EN::setdly(APB1LENR); }
		void enable_usart3() { APB1LENR_USART3EN::setdly(APB1LENR); }
		void enable_uart4() { APB1LENR_UART4EN::setdly(APB1LENR); }
		void enable_uart5() { APB1LENR_UART5EN::setdly(APB1LENR); }
		void enable_usart6() { APB2ENR_USART6EN::setdly(APB2ENR); }
		void enable_uart7() { APB1LLPENR_UART7LPEN::setdly(APB1LLPENR); }
		void enable_uart8() { APB1LLPENR_UART8LPEN::setdly(APB1LLPENR); }
		void enable_bkpsram() { AHB4ENR_BKPRAMEN::setdly(AHB4ENR); }
		void enable_syscfg() { APB4ENR_SYSCFGEN::setdly(APB4ENR); }
		void enable_rtc_apb() { APB4ENR_RTCAPBEN::setdly(APB4ENR); }
		void enable_qspi() { AHB3ENR_QSPIEN::setdly(AHB3ENR); }
		void reset_qspi()
		{
			AHB3RSTR_QSPIRST::setdly(AHB3RSTR);
			AHB3RSTR_QSPIRST::clrdly(AHB3RSTR);
		}

		void enable_spi1() { APB2ENR_SPI1EN::setdly(APB2ENR); }
		void enable_spi2() { APB1LENR_SPI2EN::setdly(APB1LENR); }
		void enable_spi3() { APB1LENR_SPI3EN::setdly(APB1LENR); }
		void enable_spi4() { APB2ENR_SPI4EN::setdly(APB2ENR); }
		void enable_spi5() { APB2ENR_SPI5EN::setdly(APB2ENR); }
		void enable_spi6() { APB4ENR_SPI6EN::setdly(APB4ENR); }

		void enable_fmc() { AHB3ENR_FMCEN::setdly(AHB3ENR); }

		void enable_usb2_otg_fs() { AHB1ENR_USB2OTGHSEN::setdly(AHB1ENR); }
		void reset_usb2_otg_fs()
		{
			AHB1RSTR_USB2OTGRST::setdly(AHB1RSTR);
			AHB1RSTR_USB2OTGRST::clrdly(AHB1RSTR);
		}

		void enable_gpioa() { AHB4ENR_GPIOAEN::setdly(AHB4ENR); }
		void enable_gpiob() { AHB4ENR_GPIOBEN::setdly(AHB4ENR); }
		void enable_gpioc() { AHB4ENR_GPIOCEN::setdly(AHB4ENR); }
		void enable_gpiod() { AHB4ENR_GPIODEN::setdly(AHB4ENR); }
		void enable_gpioe() { AHB4ENR_GPIOEEN::setdly(AHB4ENR); }
		void enable_gpiof() { AHB4ENR_GPIOFEN::setdly(AHB4ENR); }
		void enable_gpiog() { AHB4ENR_GPIOGEN::setdly(AHB4ENR); }
		void enable_gpioh() { AHB4ENR_GPIOHEN::setdly(AHB4ENR); }
		void enable_gpioi() { AHB4ENR_GPIOIEN::setdly(AHB4ENR); }
		void enable_gpioj() { AHB4ENR_GPIOJEN::setdly(AHB4ENR); }
		void enable_gpiok() { AHB4ENR_GPIOKEN::setdly(AHB4ENR); }

		void enable_gpio_all()
		{
			bits::reg_setdly(AHB4ENR,
							 AHB4ENR_GPIOAEN::BIT | AHB4ENR_GPIOBEN::BIT | AHB4ENR_GPIOCEN::BIT | AHB4ENR_GPIODEN::BIT
								 | AHB4ENR_GPIOEEN::BIT | AHB4ENR_GPIOFEN::BIT | AHB4ENR_GPIOGEN::BIT | AHB4ENR_GPIOHEN::BIT
								 | AHB4ENR_GPIOIEN::BIT | AHB4ENR_GPIOJEN::BIT | AHB4ENR_GPIOKEN::BIT);
		}

		bool is_hsi() const { return CR_HSION::get(CR) != 0; }

		uint32_t get_hsi_shift() const { return CR_HSIDIV::get(CR); }

		bool ready_hsi() { return CR_HSIDIVF::get(CR); }

		void enable_hsi()
		{
			CR_HSION::set(CR);
			while(!ready_hsi()) {}
		}

		void set_hsi_shift(uint32_t s)
		{
			CR_HSIDIV::set(CR, s);
			while(!ready_hsi()) {}
		}

		bool ready_hsi48() { return CR_HSI48RDY::get(CR); }

		void enable_hsi48()
		{
			CR_HSI48ON::set(CR);
			while(!ready_hsi48()) {}
		}

		bool ready_hse() { return CR_HSERDY::get(CR); }

		void enable_hse()
		{
			CR_HSEON::set(CR);
			while(!ready_hse()) {}
		}

		enum pll_src_mux
		{
			PLL_SRC_HSI,
			PLL_SRC_CSI,
			PLL_SRC_HSE,
			PLL_SRC_NONE,
		};

		pll_src_mux get_mux_pll_src() const { return (pll_src_mux)PLLCKSELR_PLLSRC::get(PLLCKSELR); }

		void set_mux_pll_src(pll_src_mux s) { PLLCKSELR_PLLSRC::set(PLLCKSELR, s); }

		enum sys_clk_mux
		{
			SYS_CLK_HSI,
			SYS_CLK_CSI,
			SYS_CLK_HSE,
			SYS_CLK_PLL
		};

		sys_clk_mux get_mux_sysclk() const { return (sys_clk_mux)CFGR_SW::get(CFGR); }

		void set_mux_sysclk(sys_clk_mux d) { CFGR_SW::set(CFGR, d); }

		bool ready_pll1() { return CR_PLL1RDY::get(CR) != 0; }

		void enable_pll1()
		{
			CR_PLL1ON::set(CR);
			while(!ready_pll1()) {}
		}

		void disable_pll1()
		{
			CR_PLL1ON::clr(CR);
			while(ready_pll1()) {}
		}

		void set_range_pll1(uint32_t r) { PLLCFGR_PLL1VCOSEL::set(PLLCFGR, r); }

		void setup_pll1(uint32_t m, uint32_t n, uint32_t p, uint32_t q, uint32_t r, uint32_t range)
		{
			PLLCKSELR_DIVM1::set(PLLCKSELR, m);
			PLLCFGR_PLL1RGE::set(PLLCFGR, range);
			enable_pll1p();
			enable_pll1q();
			enable_pll1r();
			PLL1DIVR_DIVN1::set(PLL1DIVR, n - 1);
			PLL1DIVR_DIVP1::set(PLL1DIVR, p - 1);
			PLL1DIVR_DIVQ1::set(PLL1DIVR, q - 1);
			PLL1DIVR_DIVR1::set(PLL1DIVR, r - 1);
		}

		uint32_t get_div_pll1m() const { return PLLCKSELR_DIVM1::get(PLLCKSELR); }

		uint32_t get_mul_pll1n() const { return PLL1DIVR_DIVN1::get(PLL1DIVR) + 1; }

		void enable_pll1p() { PLLCFGR_DIVP1EN::set(PLLCFGR); }

		bool is_enable_pll1p() const { return PLLCFGR_DIVP1EN::get(PLLCFGR) != 0; }

		uint32_t get_div_pll1p() const { return PLL1DIVR_DIVP1::get(PLL1DIVR) + 1; }

		void enable_pll1q() { PLLCFGR_DIVQ1EN::set(PLLCFGR); }

		bool is_enable_pll1q() const { return PLLCFGR_DIVQ1EN::get(PLLCFGR) != 0; }

		uint32_t get_div_pll1q() const { return PLL1DIVR_DIVQ1::get(PLL1DIVR) + 1; }

		void enable_pll1r() { PLLCFGR_DIVR1EN::set(PLLCFGR); }

		bool	 is_enable_pll1r() const { return PLLCFGR_DIVR1EN::get(PLLCFGR) != 0; }
		uint32_t get_div_pll1r() const { return PLL1DIVR_DIVR1::get(PLL1DIVR) + 1; }
		bool	 ready_pll2() { return CR_PLL2RDY::get(CR) != 0; }

		void enable_pll2()
		{
			CR_PLL2ON::set(CR);
			while(!ready_pll2()) {}
		}

		void disable_pll2()
		{
			CR_PLL2ON::clr(CR);
			while(ready_pll2()) {}
		}

		void set_range_pll2(uint32_t r) { PLLCFGR_PLL2VCOSEL::set(PLLCFGR, r); }

		void setup_pll2(uint32_t m, uint32_t n, uint32_t p, uint32_t q, uint32_t r, uint32_t range)
		{
			PLLCKSELR_DIVM2::set(PLLCKSELR, m);
			PLLCFGR_PLL2RGE::set(PLLCFGR, range);
			enable_pll2p();
			enable_pll2q();
			enable_pll2r();
			PLL2DIVR_DIVN2::set(PLL2DIVR, n - 1);
			PLL2DIVR_DIVP2::set(PLL2DIVR, p - 1);
			PLL2DIVR_DIVQ2::set(PLL2DIVR, q - 1);
			PLL2DIVR_DIVR2::set(PLL2DIVR, r - 1);
		}

		uint32_t get_div_pll2m() const { return PLLCKSELR_DIVM2::get(PLLCKSELR); }

		void set_div_pll2m(uint32_t d) { PLLCKSELR_DIVM2::set(PLLCKSELR, d); }

		uint32_t get_mul_pll2n() const { return PLL2DIVR_DIVN2::get(PLL2DIVR) + 1; }

		void set_mul_pll2n(uint32_t d) { PLL2DIVR_DIVN2::set(PLL2DIVR, d - 1); }

		void enable_pll2p() { PLLCFGR_DIVP2EN::set(PLLCFGR); }

		bool is_enable_pll2p() const { return PLLCFGR_DIVP2EN::get(PLLCFGR) != 0; }

		uint32_t get_div_pll2p() const { return PLL2DIVR_DIVP2::get(PLL2DIVR) + 1; }

		void set_div_pll2p(uint32_t d) { PLL2DIVR_DIVP2::set(PLL2DIVR, d - 1); }

		void enable_pll2q() { PLLCFGR_DIVQ2EN::set(PLLCFGR); }

		bool is_enable_pll2q() const { return PLLCFGR_DIVQ2EN::get(PLLCFGR) != 0; }

		uint32_t get_div_pll2q() const { return PLL2DIVR_DIVQ2::get(PLL2DIVR) + 1; }

		void set_div_pll2q(uint32_t d) { PLL2DIVR_DIVQ2::set(PLL2DIVR, d - 1); }

		void enable_pll2r() { PLLCFGR_DIVR2EN::set(PLLCFGR); }

		bool is_enable_pll2r() const { return PLLCFGR_DIVR2EN::get(PLLCFGR) != 0; }

		uint32_t get_div_pll2r() const { return PLL2DIVR_DIVR2::get(PLL2DIVR) + 1; }

		void set_div_pll2r(uint32_t d) { PLL2DIVR_DIVR2::set(PLL2DIVR, d - 1); }

		bool ready_pll3() { return CR_PLL3RDY::get(CR) != 0; }

		void enable_pll3()
		{
			CR_PLL3ON::set(CR);
			while(!ready_pll3()) {}
		}

		void disable_pll3()
		{
			CR_PLL3ON::clr(CR);
			while(ready_pll3()) {}
		}

		void set_range_pll3(uint32_t r) { PLLCFGR_PLL3VCOSEL::set(PLLCFGR, r); }

		void setup_pll3(uint32_t m, uint32_t n, uint32_t p, uint32_t q, uint32_t r, uint32_t range)
		{
			PLLCKSELR_DIVM3::set(PLLCKSELR, m);
			PLLCFGR_PLL3RGE::set(PLLCFGR, range);
			enable_pll3p();
			enable_pll3q();
			enable_pll3r();
			PLL3DIVR_DIVN3::set(PLL3DIVR, n - 1);
			PLL3DIVR_DIVP3::set(PLL3DIVR, p - 1);
			PLL3DIVR_DIVQ3::set(PLL3DIVR, q - 1);
			PLL3DIVR_DIVR3::set(PLL3DIVR, r - 1);
		}

		void set_frac_pll3(uint32_t fracn)
		{
			PLLCFGR_PLL3FRACEN::clr(PLLCFGR);
			PLL3FRACR_FRACN3::set(PLL3FRACR, fracn);
			PLLCFGR_PLL3FRACEN::set(PLLCFGR);
		}

		uint32_t get_frac_pll3() const { return PLL3FRACR_FRACN3::get(PLL3FRACR); }

		uint32_t get_div_pll3m() const { return PLLCKSELR_DIVM3::get(PLLCKSELR); }

		void set_div_pll3m(uint32_t d) { PLLCKSELR_DIVM3::set(PLLCKSELR, d); }

		uint32_t get_mul_pll3n() const { return PLL3DIVR_DIVN3::get(PLL3DIVR) + 1; }

		void set_mul_pll3n(uint32_t d) { PLL3DIVR_DIVN3::set(PLL3DIVR, d - 1); }

		void enable_pll3p() { PLLCFGR_DIVP3EN::set(PLLCFGR); }

		bool is_enable_pll3p() const { return PLLCFGR_DIVP3EN::get(PLLCFGR) != 0; }

		uint32_t get_div_pll3p() const { return PLL3DIVR_DIVP3::get(PLL3DIVR) + 1; }

		void set_div_pll3p(uint32_t d) { PLL3DIVR_DIVP3::set(PLL3DIVR, d - 1); }

		void enable_pll3q() { PLLCFGR_DIVQ3EN::set(PLLCFGR); }

		bool is_enable_pll3q() const { return PLLCFGR_DIVQ3EN::get(PLLCFGR) != 0; }

		uint32_t get_div_pll3q() const { return PLL3DIVR_DIVQ3::get(PLL3DIVR) + 1; }

		void set_div_pll3q(uint32_t d) { PLL3DIVR_DIVQ3::set(PLL3DIVR, d - 1); }

		void enable_pll3r() { PLLCFGR_DIVR3EN::set(PLLCFGR); }

		bool is_enable_pll3r() const { return PLLCFGR_DIVR3EN::get(PLLCFGR) != 0; }

		uint32_t get_div_pll3r() const { return PLL3DIVR_DIVR3::get(PLL3DIVR) + 1; }

		void set_div_pll3r(uint32_t d) { PLL3DIVR_DIVR3::set(PLL3DIVR, d - 1); }

		constexpr uint32_t get_div_systick() const { return 8; }

		constexpr static uint32_t get_flag3_from_div(uint32_t d)
		{
			switch(d)
			{
				case 1: return 0;
				case 2: return 4;
				case 4: return 5;
				case 8: return 6;
				case 16: return 7;
				default: lac_assert(false); return 0;
			}
		}

		constexpr static uint32_t get_div_from_flag3(uint32_t d)
		{
			switch(d)
			{
				case 0: return 1;
				case 4: return 2;
				case 5: return 4;
				case 6: return 8;
				case 7: return 16;
				default: lac_assert(false); return 0;
			}
		}

		constexpr static uint32_t get_flag4_from_div(uint32_t d)
		{
			switch(d)
			{
				case 1: return 0;
				case 2: return 8;
				case 4: return 9;
				case 8: return 10;
				case 16: return 11;
				case 64: return 12;
				case 128: return 13;
				case 256: return 14;
				case 512: return 15;
				default: lac_assert(false); return 0;
			}
		}

		constexpr static uint32_t get_div_from_flag4(uint32_t d)
		{
			switch(d)
			{
				case 0: return 1;
				case 8: return 2;
				case 9: return 4;
				case 10: return 8;
				case 11: return 16;
				case 12: return 64;
				case 13: return 128;
				case 14: return 256;
				case 15: return 512;
				default: lac_assert(false); return 0;
			}
		}

		uint32_t get_div_d1cpre() const { return get_div_from_flag4(D1CFGR_D1CPRE::get(D1CFGR)); }

		void set_div_d1cpre(uint32_t m) { D1CFGR_D1CPRE::set(D1CFGR, get_flag4_from_div(m)); }

		void set_div_sysclk(uint32_t m) { set_div_d1cpre(m); }

		uint32_t get_div_hpre() const { return get_div_from_flag4(D1CFGR_HPRE::get(D1CFGR)); }

		void set_div_hpre(uint32_t m) { D1CFGR_HPRE::set(D1CFGR, get_flag4_from_div(m)); }

		void set_div_ahb(uint32_t m) { set_div_hpre(m); }

		uint32_t get_div_d1ppre() const { return get_div_from_flag3(D1CFGR_D1PPRE::get(D1CFGR)); }

		void set_div_d1ppre(uint32_t m) { D1CFGR_D1PPRE::set(D1CFGR, get_flag3_from_div(m)); }

		void set_div_apb3(uint32_t m) { set_div_d1ppre(m); }

		uint32_t get_div_d2ppre1() const { return get_div_from_flag3(D2CFGR_D2PPRE1::get(D2CFGR)); }

		void set_div_d2ppre1(uint32_t m) { D2CFGR_D2PPRE1::set(D2CFGR, get_flag3_from_div(m)); }

		void set_div_apb1(uint32_t m) { set_div_d2ppre1(m); }

		uint32_t get_div_d2ppre2() const { return get_div_from_flag3(D2CFGR_D2PPRE2::get(D2CFGR)); }

		void set_div_d2ppre2(uint32_t m) { D2CFGR_D2PPRE2::set(D2CFGR, get_flag3_from_div(m)); }

		void set_div_apb2(uint32_t m) { set_div_d2ppre2(m); }

		uint32_t get_div_d3ppre() const { return get_div_from_flag3(D3CFGR_D3PPRE::get(D3CFGR)); }

		void set_div_d3ppre(uint32_t m) { D3CFGR_D3PPRE::set(D3CFGR, get_flag3_from_div(m)); }

		void set_div_apb4(uint32_t m) { set_div_d3ppre(m); }

		enum per_mux
		{
			PER_HSI,
			PER_CSI,
			PER_HSE
		};

		uint32_t get_mux_per() const { return D1CCIPR_CKPERSEL::get(D1CCIPR); }

		void set_mux_per(uint32_t m) { D1CCIPR_CKPERSEL::set(D1CCIPR, m); }

		enum mco1_mux
		{
			MCO1_LSE,
			MCO1_HSE,
			MCO1_HSI,
			MCO1_RC48,
			MCO1_PLL1Q
		};

		uint32_t get_mux_mco1() const { return CFGR_MCO1::get(CFGR); }

		void set_mux_mco1(uint32_t m) { CFGR_MCO1::set(CFGR, m); }

		uint32_t get_div_mco1() const
		{
			uint32_t d = CFGR_MCO1PRE::get(CFGR);
			return d == 0 ? 2 : d;
		}

		void set_div_mco1(uint32_t d) { CFGR_MCO1PRE::set(CFGR, d); }

		enum mco2_mux
		{
			MCO2_SYSCLK,
			MCO2_PLL2P,
			MCO2_HSE,
			MCO2_PLL1P,
			MCO2_CSI,
			MCO2_LSI
		};

		uint32_t get_mux_mco2() const { return CFGR_MCO2::get(CFGR); }

		void set_mux_mco2(uint32_t m) { CFGR_MCO2::set(CFGR, m); }

		uint32_t get_div_mco2() const
		{
			uint32_t d = CFGR_MCO2PRE::get(CFGR);
			return d == 0 ? 1 : d;
		}

		void set_div_mco2(uint32_t d) { CFGR_MCO2PRE::set(CFGR, d); }

		enum usart1_mux
		{
			USART1_PCLK2,
			USART1_PLL2Q,
			USART1_PLL3Q,
			USART1_HSI,
			USART1_CSI,
			USART1_LSE
		};

		uint32_t get_mux_usart16() const { return D2CCIP2R_USART16SEL::get(D2CCIP2R); }

		void set_mux_usart16(uint32_t m) { D2CCIP2R_USART16SEL::set(D2CCIP2R, m); }

		enum usart2_mux
		{
			USART2_PCLK1,
			USART2_PLL2Q,
			USART2_PLL3Q,
			USART2_HSI,
			USART2_CSI,
			USART2_LSE
		};

		uint32_t get_mux_usart234578() const { return D2CCIP2R_USART234578SEL::get(D2CCIP2R); }

		void set_mux_usart234578(uint32_t m) { D2CCIP2R_USART234578SEL::set(D2CCIP2R, m); }

		enum lpuart1_mux
		{
			LPUART1_PCLK3,
			LPUART1_PLL2Q,
			LPUART1_PLL3Q,
			LPUART1_HSI,
			LPUART1_CSI,
			LPUART1_LSE
		};

		uint32_t get_mux_lpuart1() const { return D3CCIPR_LPUART1SEL::get(D3CCIPR); }

		void set_mux_lpuart1(uint32_t m) { D3CCIPR_LPUART1SEL::set(D3CCIPR, m); }

		enum lptim1_mux
		{
			LPTIM1_PCLK1,
			LPTIM1_PLL2P,
			LPTIM1_PLL3R,
			LPTIM1_LSE,
			LPTIM1_LSI,
			LPTIM1_PER_CK,
		};

		uint32_t get_mux_lptim1() const { return D2CCIP2R_LPTIM1SEL::get(D2CCIP2R); }

		void get_mux_lptim1(uint32_t m) { D2CCIP2R_LPTIM1SEL::set(D2CCIP2R, m); }

		enum lptim2_mux
		{
			LPTIM2_PCLK4,
			LPTIM2_PLL2P,
			LPTIM2_PLL3R,
			LPTIM2_LSE,
			LPTIM2_LSI,
			LPTIM2_PER_CK,
		};

		uint32_t get_mux_lptim2() const { return D3CCIPR_LPTIM2SEL::get(D3CCIPR); }

		void set_mux_lptim2(uint32_t m) { D3CCIPR_LPTIM2SEL::set(D3CCIPR, m); }

		enum lptim3_mux
		{
			LPTIM3_PCLK4,
			LPTIM3_PLL2P,
			LPTIM3_PLL3R,
			LPTIM3_LSE,
			LPTIM3_LSI,
			LPTIM3_PER_CK,
		};

		uint32_t get_mux_lptim3() const { return D3CCIPR_LPTIM345SEL::get(D3CCIPR); }

		void set_mux_lptim3(uint32_t m) { return D3CCIPR_LPTIM345SEL::set(D3CCIPR, m); }

		enum adc_mux
		{
			ADC_PLL2P,
			ADC_PLL3R,
			ADC_PER_CK
		};

		uint32_t get_mux_adc() const { return D3CCIPR_ADCSEL::get(D3CCIPR); }

		void set_mux_adc(uint32_t m) { return D3CCIPR_ADCSEL::set(D3CCIPR, m); }

		enum usb_mux
		{
			USB_DISABLE,
			USB_PLL1Q,
			USB_PLL3Q,
			USB_RC48
		};

		uint32_t get_mux_usb() const { return D2CCIP2R_USBSEL::get(D2CCIP2R); }

		void set_mux_usb(uint32_t m) { D2CCIP2R_USBSEL::set(D2CCIP2R, m); }

		enum cec_mux
		{
			CEC_LSE,
			CEC_LSI,
			CEC_CSI,
			CEC_DISABLE
		};

		uint32_t get_mux_cec() const { return D2CCIP2R_CECSEL::get(D2CCIP2R); }

		void set_mux_cec(uint32_t m) { D2CCIP2R_CECSEL::set(D2CCIP2R, m); }

		enum fdcan_mux
		{
			FDCAN_HSE,
			FDCAN_PLL1Q,
			FDCAN_PLL2Q
		};

		uint32_t get_mux_fdcan() const { return D2CCIP1R_FDCANSEL::get(D2CCIP1R); }

		void set_mux_fdcan(uint32_t m) { return D2CCIP1R_FDCANSEL::set(D2CCIP1R, m); }

		enum hrtim_mux
		{
			HRTIM_TIM2,
			HRTIM_CPU
		};

		uint32_t get_mux_hrtim() const { return CFGR_HRTIMSEL::get(CFGR); }

		void set_mux_hrtim(uint32_t m) { CFGR_HRTIMSEL::set(CFGR, m); }

		enum trace_mux
		{
			TRACE_HSI,
			TRACE_CSI,
			TRACE_HSE,
			TRACE_PLL1R
		};

		enum sdmmc1_mux
		{
			SDMMC1_PLL1Q,
			SDMMC1_PLL2R
		};

		uint32_t get_mux_sdmmc1() const { return D1CCIPR_SDMMCSEL::get(D1CCIPR); }

		void set_mux_sdmmc1(uint32_t m) { D1CCIPR_SDMMCSEL::set(D1CCIPR, m); }

		enum dfsm_mux
		{
			DFSDM_PCLK2,
			DFSDM_SYSCLK,
		};

		uint32_t get_mux_dfsdm() const { return D2CCIP1R_DFSDM1SEL::get(D2CCIP1R); }

		void set_mux_dfsdm(uint32_t m) { D2CCIP1R_DFSDM1SEL::set(D2CCIP1R, m); }

		enum swp_mux
		{
			SWP_PCLK1,
			SWP_HSI,
		};

		uint32_t get_mux_swp() const { return D2CCIP1R_SWPSEL::get(D2CCIP1R); }

		void set_mux_swp(uint32_t m) { D2CCIP1R_SWPSEL::set(D2CCIP1R, m); }

		enum quadspi_mux
		{
			QUADSPI_HCLK3,
			QUADSPI_PLL1Q,
			QUADSPI_PLL2R,
			QUADSPI_PER_CK
		};

		uint32_t get_mux_quadspi() const { return D1CCIPR_QSPISEL::get(D1CCIPR); }

		void set_mux_quadspi(uint32_t m) { D1CCIPR_QSPISEL::set(D1CCIPR, m); }

		enum fmc_mux
		{
			FMC_HCLK3,
			FMC_PLL1Q,
			FMC_PLL2R,
			FMC_PER_CK
		};

		uint32_t get_mux_fmc() const { return D1CCIPR_FMCSEL::get(D1CCIPR); }

		void set_mux_fmc(uint32_t m) { D1CCIPR_FMCSEL::set(D1CCIPR, m); }

		enum spdifrx1_mux
		{
			SPDIFRX1_PLL1Q,
			SPDIFRX1_PLL2R,
			SPDIFRX1_PLL3R,
			SPDIFRX1_HSI
		};

		uint32_t get_mux_spdifrx1() const { return D2CCIP1R_SPDIFSEL::get(D2CCIP1R); }

		void set_mux_spdifrx1(uint32_t m) { return D2CCIP1R_SPDIFSEL::set(D2CCIP1R, m); }

		enum i2c1_mux
		{
			I2C1_PCLK1,
			I2C1_PLL3R,
			I2C1_HSI,
			I2C1_CSI
		};

		uint32_t get_mux_i2c1() const { return D2CCIP2R_I2C123SEL::get(D2CCIP2R); }

		void set_mux_i2c1(uint32_t m) { D2CCIP2R_I2C123SEL::set(D2CCIP2R, m); }

		enum i2c4_mux
		{
			I2C4_PCLK4,
			I2C4_PLL3R,
			I2C4_HSI,
			I2C4_CSI
		};

		uint32_t get_mux_i2c4() const { return D3CCIPR_I2C4SEL::get(D3CCIPR); }

		void set_mux_i2c4(uint32_t m) { D3CCIPR_I2C4SEL::set(D3CCIPR, m); }

		enum sai1_mux
		{
			SAI1_PLL1Q,
			SAI1_PLL2P,
			SAI1_PLL3P,
			SAI1_I2S_CKIN,
			SAI1_PER_CK,
		};

		uint32_t get_mux_sai1() const { return D2CCIP1R_SAI1SEL::get(D2CCIP1R); }

		void set_mux_sai1(uint32_t m) { D2CCIP1R_SAI1SEL::set(D2CCIP1R, m); }

		enum sai2_mux
		{
			SAI2_PLL1Q,
			SAI2_PLL2P,
			SAI2_PLL3P,
			SAI2_I2S_CKIN,
			SAI2_PER_CK
		};

		uint32_t get_mux_sai2() const { return D2CCIP1R_SAI23SEL::get(D2CCIP1R); }

		void set_mux_sai2(uint32_t m) { D2CCIP1R_SAI23SEL::set(D2CCIP1R, m); }

		enum sai4a_mux
		{
			SAI4A_PLL1Q,
			SAI4A_PLL2P,
			SAI4A_PLL3P,
			SAI4A_I2S_CKIN,
			SAI4A_PER_CK
		};

		uint32_t get_mux_sai4a() { return D3CCIPR_SAI4ASEL::get(D3CCIPR); }

		void set_mux_sai4a(uint32_t m) { D3CCIPR_SAI4ASEL::set(D3CCIPR, m); }

		uint32_t get_mux_sai4b() { return D3CCIPR_SAI4BSEL::get(D3CCIPR); }

		void set_mux_sai4b(uint32_t m) { D3CCIPR_SAI4BSEL::set(D3CCIPR, m); }

		enum sai4b_mux
		{
			SAI4B_PLL1Q,
			SAI4B_PLL2P,
			SAI4B_PLL3P,
			SAI4B_I2S_CKIN,
			SAI4B_PER_CK
		};

		enum spi1_mux
		{
			SPI1_PLL1Q,
			SPI1_PLL2P,
			SPI1_PLL3P,
			SPI1_I2S_CKIN,
			SPI1_PER_CK
		};

		uint32_t get_mux_spi1() const { return D2CCIP1R_SPI123SEL::get(D2CCIP1R); }

		void set_mux_spi1(uint32_t m) { D2CCIP1R_SPI123SEL::set(D2CCIP1R, m); }

		enum spi4_mux
		{
			SPI4_PCLK2,
			SPI4_PLL2Q,
			SPI4_PLL3Q,
			SPI4_HSI,
			SPI4_CSI,
			SPI4_HSE
		};

		uint32_t get_mux_spi4() const { return D2CCIP1R_SPI45SEL::get(D2CCIP1R); }

		void set_mux_spi4(uint32_t m) { D2CCIP1R_SPI45SEL::set(D2CCIP1R, m); }

		enum spi6_mux
		{
			SPI6_PCLK4,
			SPI6_PLL2Q,
			SPI6_PLL3Q,
			SPI6_HSI,
			SPI6_CSI,
			SPI6_HSE
		};

		uint32_t get_mux_spi6() const { return D3CCIPR_SPI6SEL::get(D3CCIPR); }

		void get_mux_spi6(uint32_t m) { D3CCIPR_SPI6SEL::set(D3CCIPR, m); }

		enum rng_mux
		{
			RNG_RC48,
			RNG_PLL1Q,
			RNG_LSE,
			RNG_LSI
		};

		uint32_t get_mux_rng() const { return D2CCIP2R_RNGSEL::get(D2CCIP2R); }

		void set_mux_rng(uint32_t m) { D2CCIP2R_RNGSEL::set(D2CCIP2R, m); }

		void enable_rng() { AHB2ENR_RNGEN::set(AHB2ENR); }

		void boot_cm7() { GCR_BOOT_C1::set(GCR); }
		void boot_cm4() { GCR_BOOT_C2::set(GCR); }
	};

} // namespace lac