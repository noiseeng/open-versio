/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <utl/bug.h>

namespace lac
{
	struct reg_h7_flash;
	struct reg_h7_fmc;
	struct reg_h7_mpu;
	struct reg_h7_nvic;
	struct reg_h7_rcc;
	struct reg_h7_rtc;
	struct reg_h7_usb;
	struct reg_uuid;
	struct reg_v7m_dwt;

	struct adr_h7
	{
		const static uint32_t SCS		= 0xE000E000;
		const static uint32_t ITM		= 0xE0000000;
		const static uint32_t DWT		= 0xE0001000;
		const static uint32_t TPI		= 0xE0040000;
		const static uint32_t CoreDebug = 0xE000EDF0;
		const static uint32_t SYSTICK	= SCS + 0x0010;
		const static uint32_t NVIC		= SCS + 0x0100;
		const static uint32_t SCB		= SCS + 0x0D00;
		const static uint32_t MPU		= SCS + 0x0D90;
		const static uint32_t FPU		= SCS + 0x0F30;

		constexpr static uint32_t D1_ITCMRAM  = 0x00000000UL;
		constexpr static uint32_t D1_ITCMICP  = 0x00100000UL;
		constexpr static uint32_t D1_DTCMRAM  = 0x20000000UL;
		constexpr static uint32_t D1_AXIFLASH = 0x08000000UL;
		constexpr static uint32_t D1_AXIICP	  = 0x1FF00000UL;
		constexpr static uint32_t D1_AXISRAM  = 0x24000000UL;

		constexpr static uint32_t D2_AXISRAM = 0x10000000UL;
		constexpr static uint32_t D2_AHBSRAM = 0x30000000UL;

		constexpr static uint32_t D3_BKPSRAM = 0x38800000UL;
		constexpr static uint32_t D3_SRAM	 = 0x38000000UL;

		constexpr static uint32_t PERIPH   = 0x40000000UL;
		constexpr static uint32_t QSPI_MAP = 0x90000000UL;

		constexpr static uint32_t FLASH_BANK1 = 0x08000000UL;
		constexpr static uint32_t FLASH_BANK2 = 0x08100000UL;

		constexpr static uint32_t FLASH = FLASH_BANK1;

		constexpr static uint32_t UID		= 0x1FF1E800UL;
		constexpr static uint32_t FLASHSIZE = 0x1FF1E880UL;

		constexpr static uint32_t D2_APB1PERIPH = PERIPH;
		constexpr static uint32_t D2_APB2PERIPH = PERIPH + 0x00010000UL;
		constexpr static uint32_t D2_AHB1PERIPH = PERIPH + 0x00020000UL;
		constexpr static uint32_t D2_AHB2PERIPH = PERIPH + 0x08020000UL;

		constexpr static uint32_t D1_APB1PERIPH = PERIPH + 0x10000000UL;
		constexpr static uint32_t D1_AHB1PERIPH = PERIPH + 0x12000000UL;

		constexpr static uint32_t D3_APB1PERIPH = PERIPH + 0x18000000UL;
		constexpr static uint32_t D3_AHB1PERIPH = PERIPH + 0x18020000UL;

		constexpr static uint32_t APB1PERIPH = PERIPH;
		constexpr static uint32_t APB2PERIPH = PERIPH + 0x00010000UL;
		constexpr static uint32_t AHB1PERIPH = PERIPH + 0x00020000UL;
		constexpr static uint32_t AHB2PERIPH = PERIPH + 0x08000000UL;

		constexpr static uint32_t MDMA		  = D1_AHB1PERIPH + 0x0000UL;
		constexpr static uint32_t DMA2D		  = D1_AHB1PERIPH + 0x1000UL;
		constexpr static uint32_t JPGDEC	  = D1_AHB1PERIPH + 0x3000UL;
		constexpr static uint32_t FLASH_R	  = D1_AHB1PERIPH + 0x2000UL;
		constexpr static uint32_t FMC_R		  = D1_AHB1PERIPH + 0x4000UL;
		constexpr static uint32_t QSPI		  = D1_AHB1PERIPH + 0x5000UL;
		constexpr static uint32_t DLYB_QSPI	  = D1_AHB1PERIPH + 0x6000UL;
		constexpr static uint32_t SDMMC1	  = D1_AHB1PERIPH + 0x7000UL;
		constexpr static uint32_t DLYB_SDMMC1 = D1_AHB1PERIPH + 0x8000UL;
		constexpr static uint32_t RAMECC1	  = D1_AHB1PERIPH + 0x9000UL;

		constexpr static uint32_t DMA1		   = D2_AHB1PERIPH + 0x0000UL;
		constexpr static uint32_t DMA2		   = D2_AHB1PERIPH + 0x0400UL;
		constexpr static uint32_t DMAMUX1	   = D2_AHB1PERIPH + 0x0800UL;
		constexpr static uint32_t ADC1		   = D2_AHB1PERIPH + 0x2000UL;
		constexpr static uint32_t ADC2		   = D2_AHB1PERIPH + 0x2100UL;
		constexpr static uint32_t ADC12_COMMON = D2_AHB1PERIPH + 0x2300UL;
		constexpr static uint32_t ETH		   = D2_AHB1PERIPH + 0x8000UL;
		constexpr static uint32_t ETH_MAC	   = ETH;

		constexpr static uint32_t USB1_OTG_HS_PERIPH		= 0x40040000UL;
		constexpr static uint32_t USB2_OTG_FS_PERIPH		= 0x40080000UL;
		constexpr static uint32_t USB_OTG_GLOBAL			= 0x000UL;
		constexpr static uint32_t USB_OTG_DEVICE			= 0x800UL;
		constexpr static uint32_t USB_OTG_IN_ENDPOINT		= 0x900UL;
		constexpr static uint32_t USB_OTG_OUT_ENDPOINT		= 0xB00UL;
		constexpr static uint32_t USB_OTG_EP_REG_SIZE		= 0x20UL;
		constexpr static uint32_t USB_OTG_HOST				= 0x400UL;
		constexpr static uint32_t USB_OTG_HOST_PORT			= 0x440UL;
		constexpr static uint32_t USB_OTG_HOST_CHANNEL		= 0x500UL;
		constexpr static uint32_t USB_OTG_HOST_CHANNEL_SIZE = 0x20UL;
		constexpr static uint32_t USB_OTG_PCGCCTL			= 0xE00UL;
		constexpr static uint32_t USB_OTG_FIFO				= 0x1000UL;
		constexpr static uint32_t USB_OTG_FIFO_SIZE			= 0x1000UL;

		constexpr static uint32_t DCMI		  = D2_AHB2PERIPH + 0x0000UL;
		constexpr static uint32_t RNG		  = D2_AHB2PERIPH + 0x1800UL;
		constexpr static uint32_t SDMMC2	  = D2_AHB2PERIPH + 0x2400UL;
		constexpr static uint32_t DLYB_SDMMC2 = D2_AHB2PERIPH + 0x2800UL;
		constexpr static uint32_t RAMECC2	  = D2_AHB2PERIPH + 0x3000UL;

		constexpr static uint32_t GPIOA = D3_AHB1PERIPH + 0x0000UL;
		constexpr static uint32_t GPIOB = D3_AHB1PERIPH + 0x0400UL;
		constexpr static uint32_t GPIOC = D3_AHB1PERIPH + 0x0800UL;
		constexpr static uint32_t GPIOD = D3_AHB1PERIPH + 0x0C00UL;
		constexpr static uint32_t GPIOE = D3_AHB1PERIPH + 0x1000UL;
		constexpr static uint32_t GPIOF = D3_AHB1PERIPH + 0x1400UL;
		constexpr static uint32_t GPIOG = D3_AHB1PERIPH + 0x1800UL;
		constexpr static uint32_t GPIOH = D3_AHB1PERIPH + 0x1C00UL;
		constexpr static uint32_t GPIOI = D3_AHB1PERIPH + 0x2000UL;
		constexpr static uint32_t GPIOJ = D3_AHB1PERIPH + 0x2400UL;
		constexpr static uint32_t GPIOK = D3_AHB1PERIPH + 0x2800UL;
		constexpr static uint32_t get_gpio(uint32_t idx)
		{
			switch(idx)
			{
				case 0: return GPIOA;
				case 1: return GPIOB;
				case 2: return GPIOC;
				case 3: return GPIOD;
				case 4: return GPIOE;
				case 5: return GPIOF;
				case 6: return GPIOG;
				case 7: return GPIOH;
				case 8: return GPIOI;
				case 9: return GPIOJ;
				case 10: return GPIOK;
				default: break;
			}
			lac_assert(false);
			return GPIOA;
		}

		constexpr static uint32_t get_adc_common_addr(uint32_t port)
		{
			switch(port)
			{
				case ADC1: return ADC12_COMMON;
				case ADC2: return ADC12_COMMON;
				case ADC3: return ADC3_COMMON;
				default: lac_assert(false); return 0;
			};
		}

		constexpr static uint32_t RCC		  = D3_AHB1PERIPH + 0x4400UL;
		constexpr static uint32_t PWR		  = D3_AHB1PERIPH + 0x4800UL;
		constexpr static uint32_t CRC		  = D3_AHB1PERIPH + 0x4C00UL;
		constexpr static uint32_t BDMA		  = D3_AHB1PERIPH + 0x5400UL;
		constexpr static uint32_t DMAMUX2	  = D3_AHB1PERIPH + 0x5800UL;
		constexpr static uint32_t ADC3		  = D3_AHB1PERIPH + 0x6000UL;
		constexpr static uint32_t ADC3_COMMON = D3_AHB1PERIPH + 0x6300UL;
		constexpr static uint32_t HSEM		  = D3_AHB1PERIPH + 0x6400UL;
		constexpr static uint32_t RAMECC3	  = D3_AHB1PERIPH + 0x7000UL;

		constexpr static uint32_t LTDC		  = D1_APB1PERIPH + 0x1000UL;
		constexpr static uint32_t LTDC_Layer1 = LTDC + 0x84UL;
		constexpr static uint32_t LTDC_Layer2 = LTDC + 0x104UL;
		constexpr static uint32_t WWDG1		  = D1_APB1PERIPH + 0x3000UL;

		constexpr static uint32_t TIM2	 = D2_APB1PERIPH + 0x0000UL;
		constexpr static uint32_t TIM3	 = D2_APB1PERIPH + 0x0400UL;
		constexpr static uint32_t TIM4	 = D2_APB1PERIPH + 0x0800UL;
		constexpr static uint32_t TIM5	 = D2_APB1PERIPH + 0x0C00UL;
		constexpr static uint32_t TIM6	 = D2_APB1PERIPH + 0x1000UL;
		constexpr static uint32_t TIM7	 = D2_APB1PERIPH + 0x1400UL;
		constexpr static uint32_t TIM12	 = D2_APB1PERIPH + 0x1800UL;
		constexpr static uint32_t TIM13	 = D2_APB1PERIPH + 0x1C00UL;
		constexpr static uint32_t TIM14	 = D2_APB1PERIPH + 0x2000UL;
		constexpr static uint32_t LPTIM1 = D2_APB1PERIPH + 0x2400UL;

		constexpr static uint32_t SPI2		= D2_APB1PERIPH + 0x3800UL;
		constexpr static uint32_t SPI3		= D2_APB1PERIPH + 0x3C00UL;
		constexpr static uint32_t SPDIFRX	= D2_APB1PERIPH + 0x4000UL;
		constexpr static uint32_t USART2	= D2_APB1PERIPH + 0x4400UL;
		constexpr static uint32_t USART3	= D2_APB1PERIPH + 0x4800UL;
		constexpr static uint32_t UART4		= D2_APB1PERIPH + 0x4C00UL;
		constexpr static uint32_t UART5		= D2_APB1PERIPH + 0x5000UL;
		constexpr static uint32_t I2C1		= D2_APB1PERIPH + 0x5400UL;
		constexpr static uint32_t I2C2		= D2_APB1PERIPH + 0x5800UL;
		constexpr static uint32_t I2C3		= D2_APB1PERIPH + 0x5C00UL;
		constexpr static uint32_t CEC		= D2_APB1PERIPH + 0x6C00UL;
		constexpr static uint32_t DAC1		= D2_APB1PERIPH + 0x7400UL;
		constexpr static uint32_t UART7		= D2_APB1PERIPH + 0x7800UL;
		constexpr static uint32_t UART8		= D2_APB1PERIPH + 0x7C00UL;
		constexpr static uint32_t CRS		= D2_APB1PERIPH + 0x8400UL;
		constexpr static uint32_t SWPMI1	= D2_APB1PERIPH + 0x8800UL;
		constexpr static uint32_t OPAMP		= D2_APB1PERIPH + 0x9000UL;
		constexpr static uint32_t OPAMP1	= D2_APB1PERIPH + 0x9000UL;
		constexpr static uint32_t OPAMP2	= D2_APB1PERIPH + 0x9010UL;
		constexpr static uint32_t MDIOS		= D2_APB1PERIPH + 0x9400UL;
		constexpr static uint32_t FDCAN1	= D2_APB1PERIPH + 0xA000UL;
		constexpr static uint32_t FDCAN2	= D2_APB1PERIPH + 0xA400UL;
		constexpr static uint32_t FDCAN_CCU = D2_APB1PERIPH + 0xA800UL;
		constexpr static uint32_t SRAMCAN	= D2_APB1PERIPH + 0xAC00UL;

		constexpr static uint32_t TIM1			  = D2_APB2PERIPH + 0x0000UL;
		constexpr static uint32_t TIM8			  = D2_APB2PERIPH + 0x0400UL;
		constexpr static uint32_t USART1		  = D2_APB2PERIPH + 0x1000UL;
		constexpr static uint32_t USART6		  = D2_APB2PERIPH + 0x1400UL;
		constexpr static uint32_t SPI1			  = D2_APB2PERIPH + 0x3000UL;
		constexpr static uint32_t SPI4			  = D2_APB2PERIPH + 0x3400UL;
		constexpr static uint32_t TIM15			  = D2_APB2PERIPH + 0x4000UL;
		constexpr static uint32_t TIM16			  = D2_APB2PERIPH + 0x4400UL;
		constexpr static uint32_t TIM17			  = D2_APB2PERIPH + 0x4800UL;
		constexpr static uint32_t SPI5			  = D2_APB2PERIPH + 0x5000UL;
		constexpr static uint32_t SAI1			  = D2_APB2PERIPH + 0x5800UL;
		constexpr static uint32_t SAI1_BLOCK_A	  = SAI1 + 0x004UL;
		constexpr static uint32_t SAI1_BLOCK_B	  = SAI1 + 0x024UL;
		constexpr static uint32_t SAI2			  = D2_APB2PERIPH + 0x5C00UL;
		constexpr static uint32_t SAI2_BLOCK_A	  = SAI2 + 0x004UL;
		constexpr static uint32_t SAI2_BLOCK_B	  = SAI2 + 0x024UL;
		constexpr static uint32_t SAI3			  = D2_APB2PERIPH + 0x6000UL;
		constexpr static uint32_t SAI3_BLOCK_A	  = SAI3 + 0x004UL;
		constexpr static uint32_t SAI3_BLOCK_B	  = SAI3 + 0x024UL;
		constexpr static uint32_t DFSDM1		  = D2_APB2PERIPH + 0x7000UL;
		constexpr static uint32_t DFSDM1_CHANNEL0 = DFSDM1 + 0x00UL;
		constexpr static uint32_t DFSDM1_CHANNEL1 = DFSDM1 + 0x20UL;
		constexpr static uint32_t DFSDM1_CHANNEL2 = DFSDM1 + 0x40UL;
		constexpr static uint32_t DFSDM1_CHANNEL3 = DFSDM1 + 0x60UL;
		constexpr static uint32_t DFSDM1_CHANNEL4 = DFSDM1 + 0x80UL;
		constexpr static uint32_t DFSDM1_CHANNEL5 = DFSDM1 + 0xA0UL;
		constexpr static uint32_t DFSDM1_CHANNEL6 = DFSDM1 + 0xC0UL;
		constexpr static uint32_t DFSDM1_CHANNEL7 = DFSDM1 + 0xE0UL;
		constexpr static uint32_t DFSDM1_FILTER0  = DFSDM1 + 0x100UL;
		constexpr static uint32_t DFSDM1_FILTER1  = DFSDM1 + 0x180UL;
		constexpr static uint32_t DFSDM1_Filter2  = DFSDM1 + 0x200UL;
		constexpr static uint32_t DFSDM1_FILTER3  = DFSDM1 + 0x280UL;
		constexpr static uint32_t HRTIM1		  = D2_APB2PERIPH + 0x7400UL;
		constexpr static uint32_t HRTIM1_TIMA	  = HRTIM1 + 0x00000080UL;
		constexpr static uint32_t HRTIM1_TIMB	  = HRTIM1 + 0x00000100UL;
		constexpr static uint32_t HRTIM1_TIMC	  = HRTIM1 + 0x00000180UL;
		constexpr static uint32_t HRTIM1_TIMD	  = HRTIM1 + 0x00000200UL;
		constexpr static uint32_t HRTIM1_TIME	  = HRTIM1 + 0x00000280UL;
		constexpr static uint32_t HRTIM1_COMMON	  = HRTIM1 + 0x00000380UL;

		constexpr static uint32_t EXTI	  = D3_APB1PERIPH + 0x0000UL;
		constexpr static uint32_t EXTI_D1 = EXTI + 0x0080UL;
		constexpr static uint32_t EXTI_D2 = EXTI + 0x00C0UL;
		constexpr static uint32_t SYSCFG  = D3_APB1PERIPH + 0x0400UL;
		constexpr static uint32_t LPUART1 = D3_APB1PERIPH + 0x0C00UL;
		constexpr static uint32_t SPI6	  = D3_APB1PERIPH + 0x1400UL;
		constexpr static uint32_t I2C4	  = D3_APB1PERIPH + 0x1C00UL;
		constexpr static uint32_t LPTIM2  = D3_APB1PERIPH + 0x2400UL;
		constexpr static uint32_t LPTIM3  = D3_APB1PERIPH + 0x2800UL;
		constexpr static uint32_t LPTIM4  = D3_APB1PERIPH + 0x2C00UL;
		constexpr static uint32_t LPTIM5  = D3_APB1PERIPH + 0x3000UL;
		constexpr static uint32_t COMP12  = D3_APB1PERIPH + 0x3800UL;
		constexpr static uint32_t COMP1	  = COMP12 + 0x0CUL;
		constexpr static uint32_t COMP2	  = COMP12 + 0x10UL;
		constexpr static uint32_t VREFBUF = D3_APB1PERIPH + 0x3C00UL;
		constexpr static uint32_t RTC	  = D3_APB1PERIPH + 0x4000UL;
		constexpr static uint32_t IWDG1	  = D3_APB1PERIPH + 0x4800UL;

		constexpr static uint32_t SAI4		   = D3_APB1PERIPH + 0x5400UL;
		constexpr static uint32_t SAI4_BLOCK_A = SAI4 + 0x004UL;
		constexpr static uint32_t SAI4_BLOCK_B = SAI4 + 0x024UL;

		constexpr static uint32_t BDMA_CHANNEL0 = BDMA + 0x0008UL;
		constexpr static uint32_t BDMA_CHANNEL1 = BDMA + 0x001CUL;
		constexpr static uint32_t BDMA_CHANNEL2 = BDMA + 0x0030UL;
		constexpr static uint32_t BDMA_CHANNEL3 = BDMA + 0x0044UL;
		constexpr static uint32_t BDMA_CHANNEL4 = BDMA + 0x0058UL;
		constexpr static uint32_t BDMA_CHANNEL5 = BDMA + 0x006CUL;
		constexpr static uint32_t BDMA_CHANNEL6 = BDMA + 0x0080UL;
		constexpr static uint32_t BDMA_CHANNEL7 = BDMA + 0x0094UL;

		constexpr static uint32_t DMAMUX2_CHANNEL0 = DMAMUX2;
		constexpr static uint32_t DMAMUX2_CHANNEL1 = DMAMUX2 + 0x0004UL;
		constexpr static uint32_t DMAMUX2_CHANNEL2 = DMAMUX2 + 0x0008UL;
		constexpr static uint32_t DMAMUX2_CHANNEL3 = DMAMUX2 + 0x000CUL;
		constexpr static uint32_t DMAMUX2_CHANNEL4 = DMAMUX2 + 0x0010UL;
		constexpr static uint32_t DMAMUX2_CHANNEL5 = DMAMUX2 + 0x0014UL;
		constexpr static uint32_t DMAMUX2_CHANNEL6 = DMAMUX2 + 0x0018UL;
		constexpr static uint32_t DMAMUX2_CHANNEL7 = DMAMUX2 + 0x001CUL;

		constexpr static uint32_t DMAMUX2_REQUESTGENERATOR0 = DMAMUX2 + 0x0100UL;
		constexpr static uint32_t DMAMUX2_REQUESTGENERATOR1 = DMAMUX2 + 0x0104UL;
		constexpr static uint32_t DMAMUX2_REQUESTGENERATOR2 = DMAMUX2 + 0x0108UL;
		constexpr static uint32_t DMAMUX2_REQUESTGENERATOR3 = DMAMUX2 + 0x010CUL;
		constexpr static uint32_t DMAMUX2_REQUESTGENERATOR4 = DMAMUX2 + 0x0110UL;
		constexpr static uint32_t DMAMUX2_REQUESTGENERATOR5 = DMAMUX2 + 0x0114UL;
		constexpr static uint32_t DMAMUX2_REQUESTGENERATOR6 = DMAMUX2 + 0x0118UL;
		constexpr static uint32_t DMAMUX2_REQUESTGENERATOR7 = DMAMUX2 + 0x011CUL;

		constexpr static uint32_t DMAMUX2_CHANNELSTATUS	   = DMAMUX2 + 0x0080UL;
		constexpr static uint32_t DMAMUX2_REQUESTGENSTATUS = DMAMUX2 + 0x0140UL;

		constexpr static uint32_t DMA1_STREAM0 = DMA1 + 0x010UL;
		constexpr static uint32_t DMA1_STREAM1 = DMA1 + 0x028UL;
		constexpr static uint32_t DMA1_STREAM2 = DMA1 + 0x040UL;
		constexpr static uint32_t DMA1_STREAM3 = DMA1 + 0x058UL;
		constexpr static uint32_t DMA1_STREAM4 = DMA1 + 0x070UL;
		constexpr static uint32_t DMA1_STREAM5 = DMA1 + 0x088UL;
		constexpr static uint32_t DMA1_STREAM6 = DMA1 + 0x0A0UL;
		constexpr static uint32_t DMA1_STREAM7 = DMA1 + 0x0B8UL;

		constexpr static uint32_t DMA2_STREAM0 = DMA2 + 0x010UL;
		constexpr static uint32_t DMA2_STREAM1 = DMA2 + 0x028UL;
		constexpr static uint32_t DMA2_STREAM2 = DMA2 + 0x040UL;
		constexpr static uint32_t DMA2_STREAM3 = DMA2 + 0x058UL;
		constexpr static uint32_t DMA2_STREAM4 = DMA2 + 0x070UL;
		constexpr static uint32_t DMA2_STREAM5 = DMA2 + 0x088UL;
		constexpr static uint32_t DMA2_STREAM6 = DMA2 + 0x0A0UL;
		constexpr static uint32_t DMA2_STREAM7 = DMA2 + 0x0B8UL;

		constexpr static uint32_t DMAMUX1_CHANNEL0	= DMAMUX1;
		constexpr static uint32_t DMAMUX1_CHANNEL1	= DMAMUX1 + 0x0004UL;
		constexpr static uint32_t DMAMUX1_CHANNEL2	= DMAMUX1 + 0x0008UL;
		constexpr static uint32_t DMAMUX1_CHANNEL3	= DMAMUX1 + 0x000CUL;
		constexpr static uint32_t DMAMUX1_CHANNEL4	= DMAMUX1 + 0x0010UL;
		constexpr static uint32_t DMAMUX1_CHANNEL5	= DMAMUX1 + 0x0014UL;
		constexpr static uint32_t DMAMUX1_CHANNEL6	= DMAMUX1 + 0x0018UL;
		constexpr static uint32_t DMAMUX1_CHANNEL7	= DMAMUX1 + 0x001CUL;
		constexpr static uint32_t DMAMUX1_CHANNEL8	= DMAMUX1 + 0x0020UL;
		constexpr static uint32_t DMAMUX1_CHANNEL9	= DMAMUX1 + 0x0024UL;
		constexpr static uint32_t DMAMUX1_CHANNEL10 = DMAMUX1 + 0x0028UL;
		constexpr static uint32_t DMAMUX1_CHANNEL11 = DMAMUX1 + 0x002CUL;
		constexpr static uint32_t DMAMUX1_CHANNEL12 = DMAMUX1 + 0x0030UL;
		constexpr static uint32_t DMAMUX1_CHANNEL13 = DMAMUX1 + 0x0034UL;
		constexpr static uint32_t DMAMUX1_CHANNEL14 = DMAMUX1 + 0x0038UL;
		constexpr static uint32_t DMAMUX1_CHANNEL15 = DMAMUX1 + 0x003CUL;

		constexpr static uint32_t DMAMUX1_REQUESTGENERATOR0 = DMAMUX1 + 0x0100UL;
		constexpr static uint32_t DMAMUX1_REQUESTGENERATOR1 = DMAMUX1 + 0x0104UL;
		constexpr static uint32_t DMAMUX1_REQUESTGENERATOR2 = DMAMUX1 + 0x0108UL;
		constexpr static uint32_t DMAMUX1_REQUESTGENERATOR3 = DMAMUX1 + 0x010CUL;
		constexpr static uint32_t DMAMUX1_REQUESTGENERATOR4 = DMAMUX1 + 0x0110UL;
		constexpr static uint32_t DMAMUX1_REQUESTGENERATOR5 = DMAMUX1 + 0x0114UL;
		constexpr static uint32_t DMAMUX1_REQUESTGENERATOR6 = DMAMUX1 + 0x0118UL;
		constexpr static uint32_t DMAMUX1_REQUESTGENERATOR7 = DMAMUX1 + 0x011CUL;

		constexpr static uint32_t DMAMUX1_CHANNELSTATUS	   = DMAMUX1 + 0x0080UL;
		constexpr static uint32_t DMAMUX1_REQUESTGENSTATUS = DMAMUX1 + 0x0140UL;

		constexpr static uint32_t FMC_BANK1_R	= FMC_R + 0x0000UL;
		constexpr static uint32_t FMC_BANK1E_R	= FMC_R + 0x0104UL;
		constexpr static uint32_t FMC_BANK2_R	= FMC_R + 0x0060UL;
		constexpr static uint32_t FMC_BANK3_R	= FMC_R + 0x0080UL;
		constexpr static uint32_t FMC_BANK5_6_R = FMC_R + 0x0140UL;

		constexpr static uint32_t DBGMCU = 0x5C001000UL;

		constexpr static uint32_t MDMA_CHANNEL0	 = MDMA + 0x00000040UL;
		constexpr static uint32_t MDMA_CHANNEL1	 = MDMA + 0x00000080UL;
		constexpr static uint32_t MDMA_CHANNEL2	 = MDMA + 0x000000C0UL;
		constexpr static uint32_t MDMA_CHANNEL3	 = MDMA + 0x00000100UL;
		constexpr static uint32_t MDMA_CHANNEL4	 = MDMA + 0x00000140UL;
		constexpr static uint32_t MDMA_CHANNEL5	 = MDMA + 0x00000180UL;
		constexpr static uint32_t MDMA_CHANNEL6	 = MDMA + 0x000001C0UL;
		constexpr static uint32_t MDMA_CHANNEL7	 = MDMA + 0x00000200UL;
		constexpr static uint32_t MDMA_CHANNEL8	 = MDMA + 0x00000240UL;
		constexpr static uint32_t MDMA_CHANNEL9	 = MDMA + 0x00000280UL;
		constexpr static uint32_t MDMA_CHANNEL10 = MDMA + 0x000002C0UL;
		constexpr static uint32_t MDMA_CHANNEL11 = MDMA + 0x00000300UL;
		constexpr static uint32_t MDMA_CHANNEL12 = MDMA + 0x00000340UL;
		constexpr static uint32_t MDMA_CHANNEL13 = MDMA + 0x00000380UL;
		constexpr static uint32_t MDMA_CHANNEL14 = MDMA + 0x000003C0UL;
		constexpr static uint32_t MDMA_CHANNEL15 = MDMA + 0x00000400UL;

		constexpr static uint32_t RAMECC1_MONITOR1 = RAMECC1 + 0x20UL;
		constexpr static uint32_t RAMECC1_MONITOR2 = RAMECC1 + 0x40UL;
		constexpr static uint32_t RAMECC1_MONITOR3 = RAMECC1 + 0x60UL;
		constexpr static uint32_t RAMECC1_MONITOR4 = RAMECC1 + 0x80UL;
		constexpr static uint32_t RAMECC1_MONITOR5 = RAMECC1 + 0xA0UL;

		constexpr static uint32_t RAMECC2_MONITOR1 = RAMECC2 + 0x20UL;
		constexpr static uint32_t RAMECC2_MONITOR2 = RAMECC2 + 0x40UL;
		constexpr static uint32_t RAMECC2_MONITOR3 = RAMECC2 + 0x60UL;
		constexpr static uint32_t RAMECC2_MONITOR4 = RAMECC2 + 0x80UL;
		constexpr static uint32_t RAMECC2_MONITOR5 = RAMECC2 + 0xA0UL;

		constexpr static uint32_t RAMECC3_MONITOR1 = RAMECC3 + 0x20UL;
		constexpr static uint32_t RAMECC3_MONITOR2 = RAMECC3 + 0x40UL;

		constexpr static uint32_t GPV = PERIPH + 0x11000000UL;

		static auto* get_flash() { return (reg_h7_flash*)FLASH_R; }
		static auto* get_fmc() { return (reg_h7_fmc*)(FMC_R); }
		static auto* get_mpu() { return (reg_h7_mpu*)MPU; }
		static auto* get_nvic() { return (reg_h7_nvic*)NVIC; }
		static auto* get_rcc() { return (reg_h7_rcc*)RCC; }
		static auto* get_rtc() { return (reg_h7_rtc*)RTC; }
		static auto* get_usb() { return (reg_h7_usb*)USB2_OTG_FS_PERIPH; }
		static auto* get_uuid() { return (reg_uuid*)UID; }
		static auto* get_dwt() { return (reg_v7m_dwt*)DWT; }
	};

	static_assert(adr_h7::SCS == 0xE000E000);
	static_assert(adr_h7::D1_AXISRAM == 0x24000000);
	static_assert(adr_h7::QSPI_MAP == 0x90000000);
	static_assert(adr_h7::RCC == 0x58024400);
	static_assert(adr_h7::GPIOA == 0x58020000);
	static_assert(adr_h7::GPIOK == 0x58022800);
	static_assert(adr_h7::DMA1_STREAM0 == 0x40020010);
	static_assert(adr_h7::DMAMUX1_CHANNEL0 == 0x40020800);
	static_assert(adr_h7::TIM2 == 0x40000000);
	static_assert(adr_h7::USB2_OTG_FS_PERIPH == 0x40080000);
	static_assert(adr_h7::FLASH_R == 0x52002000);
	static_assert(adr_h7::FMC_R == 0x52004000);
	static_assert(adr_h7::EXTI == 0x58000000);
	static_assert(adr_h7::SYSCFG == 0x58000400);
	static_assert(adr_h7::PWR == 0x58024800);
	static_assert(adr_h7::UID == 0x1FF1E800);
	static_assert(adr_h7::RAMECC3_MONITOR2 == 0x58027040);

	static_assert(adr_h7::get_adc_common_addr(adr_h7::ADC3) == adr_h7::ADC3_COMMON);

} // namespace lac
