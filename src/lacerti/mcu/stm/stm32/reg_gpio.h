/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <utl/bitarray.h>
#include <utl/bitfield.h>
#include <utl/bitflag.h>

namespace lac
{
	enum gpio_alt : uint32_t
	{
		GPIO_ALT_NONE,
		GPIO_ALT_1,
		GPIO_ALT_2,
		GPIO_ALT_3,
		GPIO_ALT_4,
		GPIO_ALT_5,
		GPIO_ALT_6,
		GPIO_ALT_7,
		GPIO_ALT_8,
		GPIO_ALT_9,
		GPIO_ALT_10,
		GPIO_ALT_11,
		GPIO_ALT_12,
		GPIO_ALT_13,
		GPIO_ALT_14,
		GPIO_ALT_15,
		GPIO_NOT_ALT = 0xFFFFFFFF
	};

	enum gpio_mode
	{
		GPIO_MODE_ANALOG,
		GPIO_MODE_IN,
		GPIO_MODE_INPU,
		GPIO_MODE_INPD,
		GPIO_MODE_OUT,
		GPIO_MODE_OD,
		GPIO_MODE_AF_PU,
		GPIO_MODE_NONE
	};

	enum gpio_speed
	{
		GPIO_PWR_LOW,
		GPIO_PWR_MED,
		GPIO_PWR_HIGH,
		GPIO_PWR_HIGHEST,
	};

	struct reg_gpio
	{
		enum
		{
			PUPDR_NOPULL   = 0,
			PUPDR_PULLUP   = 1,
			PUPDR_PULLDOWN = 2
		};

		enum
		{
			OTYPE_PP = 0,
			OTYPE_OD = 1
		};
		enum
		{
			MODE_INPUT	= 0,
			MODE_OUTPUT = 1,
			MODE_AF		= 2,
			MODE_ANALOG = 3
		};

		volatile uint32_t MODER;
		volatile uint32_t OTYPER;
		volatile uint32_t OSPEEDR;
		volatile uint32_t PUPDR;
		volatile uint32_t IDR;
		volatile uint32_t ODR;
		volatile uint32_t BSRR;
		volatile uint32_t LCKR;
		volatile uint32_t AFR_0;
		volatile uint32_t AFR_1;

		using MODER_MODER = bitarray<16, 2, 0>;

		using OTYPER_OT = bitarray<16, 1, 0>;

		using OSPEEDR_OSPEEDR = bitarray<16, 2, 0>;

		using PUPDR_PUPDR = bitarray<16, 2, 0>;

		using IDR_IDR = bitfield<0, 16>;

		using ODR_ODR = bitfield<0, 16>;

		using BSRR_BS = bitfield<0, 16>;
		using BSRR_BR = bitfield<16, 16>;

		using LCKR_LCK	= bitfield<0, 16>;
		using LCKR_LCKK = bitflag<16>;

		using AFR_AFSEL_0 = bitarray<8, 4>;
		using AFR_AFSEL_1 = bitarray<8, 4>;

		using BRR_BR = bitfield<0, 16>;

		using ACR_PRFTBS  = bitflag<5>;
		using ACR_PRFTBE  = bitflag<4>;
		using ACR_LATENCY = bitfield<0, 3>;

		void init(uint32_t	 bit,
				  uint16_t	 mode,
				  uint16_t	 otype,
				  uint16_t	 pullup,
				  uint32_t	 alternate = GPIO_NOT_ALT,
				  gpio_speed speed	   = GPIO_PWR_HIGHEST)
		{
			OTYPER_OT::set(OTYPER, bit, otype);
			MODER_MODER::set(MODER, bit, mode);
			set_speed(bit, speed);
			PUPDR_PUPDR::set(PUPDR, bit, pullup);

			if(alternate != GPIO_NOT_ALT)
			{
				if(bit < 8)
				{
					AFR_AFSEL_0::set(AFR_0, bit, alternate);
				}
				else
				{
					AFR_AFSEL_1::set(AFR_1, bit - 8, alternate);
				}
			}
		}

		void set_speed(uint32_t bit, gpio_speed s) { OSPEEDR_OSPEEDR::set(OSPEEDR, bit, s); }
	};
} // namespace lac