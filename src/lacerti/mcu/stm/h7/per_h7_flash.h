/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <mcu/arm/mcu_arm_v7m.h>

#include "per_h7_flash_geometry.h"
#include "reg_h7_flash.h"

namespace lac
{
	template <uint32_t reg_base, uint32_t BANKS = 2>
	struct per_h7_flash : per_h7_flash_geometry
	{
		static constexpr uint32_t WORD_SIZE = 32;

		static constexpr bool is_bank1(uint32_t addr) { return BANKS == 1 || bank_of(addr) == 1; }

		static reg_h7_flash *reg() { return (reg_h7_flash *)reg_base; }

		static void unlock()
		{
			reg()->KEYR1 = 0x45670123U;
			reg()->KEYR1 = 0xCDEF89ABU;
		}

		static void lock() { reg_h7_flash::CR1_LOCK1::set(reg()->CR1); }

		static void unlock_at(uint32_t addr)
		{
			auto *f = reg();
			if(is_bank1(addr))
			{
				f->KEYR1 = 0x45670123U;
				f->KEYR1 = 0xCDEF89ABU;
			}
			else
			{
				f->KEYR2 = 0x45670123U;
				f->KEYR2 = 0xCDEF89ABU;
			}
		}

		static void lock_at(uint32_t addr)
		{
			auto *f = reg();
			if(is_bank1(addr))
			{
				reg_h7_flash::CR1_LOCK1::set(f->CR1);
			}
			else
			{
				reg_h7_flash::CR2_LOCK2::set(f->CR2);
			}
		}

		static bool erase_sector()
		{
			auto *f = reg();

			while(reg_h7_flash::SR1_QW1::get(f->SR1) || reg_h7_flash::SR1_BSY1::get(f->SR1)) {}

			f->CCR1 = reg_h7_flash::CCR1_CLR_EOP1::BIT | reg_h7_flash::CCR1_CLR_WRPERR1::BIT
					  | reg_h7_flash::CCR1_CLK_PGSERR1::BIT | reg_h7_flash::CCR1_CLR_STRBERR1::BIT
					  | reg_h7_flash::CCR1_CLR_INCERR1::BIT;

			f->CR1 = reg_h7_flash::CR1_SER1::BIT | reg_h7_flash::CR1_SNB1::val(0) | reg_h7_flash::CR1_PSIZE1::val(2)
					 | reg_h7_flash::CR1_START1::BIT;

			while(reg_h7_flash::SR1_QW1::get(f->SR1) || reg_h7_flash::SR1_BSY1::get(f->SR1)) {}

			bool ok = !(reg_h7_flash::SR1_WRPERR1::get(f->SR1) || reg_h7_flash::SR1_PGSERR1::get(f->SR1)
						|| reg_h7_flash::SR1_STRBERR1::get(f->SR1) || reg_h7_flash::SR1_INCERR1::get(f->SR1));

			f->CCR1 = reg_h7_flash::CCR1_CLR_EOP1::BIT | reg_h7_flash::CCR1_CLR_WRPERR1::BIT
					  | reg_h7_flash::CCR1_CLK_PGSERR1::BIT | reg_h7_flash::CCR1_CLR_STRBERR1::BIT
					  | reg_h7_flash::CCR1_CLR_INCERR1::BIT;

			f->CR1 = 0;

			return ok;
		}

		static bool program_word256(volatile uint32_t *dst, const uint32_t *src, bool force_flush)
		{
			auto *f = reg();

			while(reg_h7_flash::SR1_QW1::get(f->SR1)) {}

			f->CCR1 = reg_h7_flash::CCR1_CLR_EOP1::BIT | reg_h7_flash::CCR1_CLR_WRPERR1::BIT
					  | reg_h7_flash::CCR1_CLK_PGSERR1::BIT | reg_h7_flash::CCR1_CLR_STRBERR1::BIT
					  | reg_h7_flash::CCR1_CLR_INCERR1::BIT;

			f->CR1 = reg_h7_flash::CR1_PG1::BIT | (force_flush ? reg_h7_flash::CR1_FW1::BIT : 0U);

			mcu_arm_v7m::dsb();

			for(uint32_t i = 0; i < 8; i++)
			{
				dst[i] = src[i];
			}

			mcu_arm_v7m::dsb();

			while(reg_h7_flash::SR1_WBNE1::get(f->SR1) || reg_h7_flash::SR1_QW1::get(f->SR1)) {}

			bool ok = !(reg_h7_flash::SR1_WRPERR1::get(f->SR1) || reg_h7_flash::SR1_PGSERR1::get(f->SR1)
						|| reg_h7_flash::SR1_STRBERR1::get(f->SR1) || reg_h7_flash::SR1_INCERR1::get(f->SR1));

			f->CCR1 = reg_h7_flash::CCR1_CLR_EOP1::BIT | reg_h7_flash::CCR1_CLR_WRPERR1::BIT
					  | reg_h7_flash::CCR1_CLK_PGSERR1::BIT | reg_h7_flash::CCR1_CLR_STRBERR1::BIT
					  | reg_h7_flash::CCR1_CLR_INCERR1::BIT;

			f->CR1 = 0;

			return ok;
		}

		static bool erase_at(uint32_t addr)
		{
			auto	*f	 = reg();
			uint32_t snb = sector_of(addr);

			if(is_bank1(addr))
			{
				while(reg_h7_flash::SR1_QW1::get(f->SR1) || reg_h7_flash::SR1_BSY1::get(f->SR1)) {}

				f->CR1 = reg_h7_flash::CR1_SER1::BIT | reg_h7_flash::CR1_SNB1::val(snb) | reg_h7_flash::CR1_PSIZE1::val(2)
						 | reg_h7_flash::CR1_START1::BIT;

				while(reg_h7_flash::SR1_QW1::get(f->SR1) || reg_h7_flash::SR1_BSY1::get(f->SR1)) {}

				bool ok = !(reg_h7_flash::SR1_WRPERR1::get(f->SR1) || reg_h7_flash::SR1_PGSERR1::get(f->SR1)
							|| reg_h7_flash::SR1_STRBERR1::get(f->SR1) || reg_h7_flash::SR1_INCERR1::get(f->SR1));

				f->CCR1 = reg_h7_flash::CCR1_CLR_EOP1::BIT | reg_h7_flash::CCR1_CLR_WRPERR1::BIT
						  | reg_h7_flash::CCR1_CLK_PGSERR1::BIT | reg_h7_flash::CCR1_CLR_STRBERR1::BIT
						  | reg_h7_flash::CCR1_CLR_INCERR1::BIT;

				f->CR1 = 0;
				return ok;
			}
			else
			{
				while(reg_h7_flash::SR2_QW2::get(f->SR2) || reg_h7_flash::SR2_BSY2::get(f->SR2)) {}

				f->CR2 = reg_h7_flash::CR2_SER2::BIT | reg_h7_flash::CR2_SNB2::val(snb) | reg_h7_flash::CR2_PSIZE2::val(2)
						 | reg_h7_flash::CR2_START2::BIT;

				while(reg_h7_flash::SR2_QW2::get(f->SR2) || reg_h7_flash::SR2_BSY2::get(f->SR2)) {}

				bool ok = !(reg_h7_flash::SR2_WRPERR2::get(f->SR2) || reg_h7_flash::SR2_PGSERR2::get(f->SR2)
							|| reg_h7_flash::SR2_STRBERR2::get(f->SR2) || reg_h7_flash::SR2_INCERR2::get(f->SR2));

				f->CCR2 = reg_h7_flash::CCR2_CLR_EOP2::BIT | reg_h7_flash::CCR2_CLR_WRPERR2::BIT
						  | reg_h7_flash::CCR2_CLR_PGSERR2::BIT | reg_h7_flash::CCR2_CLR_STRBERR2::BIT
						  | reg_h7_flash::CCR2_CLR_INCERR2::BIT;

				f->CR2 = 0;
				return ok;
			}
		}

		static bool program_word256_at(uint32_t addr, const uint32_t *src, bool force_flush)
		{
			auto			  *f   = reg();
			volatile uint32_t *dst = (volatile uint32_t *)addr;

			if(is_bank1(addr))
			{
				while(reg_h7_flash::SR1_QW1::get(f->SR1)) {}

				f->CCR1 = reg_h7_flash::CCR1_CLR_EOP1::BIT | reg_h7_flash::CCR1_CLR_WRPERR1::BIT
						  | reg_h7_flash::CCR1_CLK_PGSERR1::BIT | reg_h7_flash::CCR1_CLR_STRBERR1::BIT
						  | reg_h7_flash::CCR1_CLR_INCERR1::BIT;

				f->CR1 = reg_h7_flash::CR1_PG1::BIT | (force_flush ? reg_h7_flash::CR1_FW1::BIT : 0U);

				mcu_arm_v7m::dsb();

				for(uint32_t i = 0; i < 8; i++)
				{
					dst[i] = src[i];
				}

				mcu_arm_v7m::dsb();

				while(reg_h7_flash::SR1_WBNE1::get(f->SR1) || reg_h7_flash::SR1_QW1::get(f->SR1)) {}

				bool ok = !(reg_h7_flash::SR1_WRPERR1::get(f->SR1) || reg_h7_flash::SR1_PGSERR1::get(f->SR1)
							|| reg_h7_flash::SR1_STRBERR1::get(f->SR1) || reg_h7_flash::SR1_INCERR1::get(f->SR1));

				f->CCR1 = reg_h7_flash::CCR1_CLR_EOP1::BIT | reg_h7_flash::CCR1_CLR_WRPERR1::BIT
						  | reg_h7_flash::CCR1_CLK_PGSERR1::BIT | reg_h7_flash::CCR1_CLR_STRBERR1::BIT
						  | reg_h7_flash::CCR1_CLR_INCERR1::BIT;

				f->CR1 = 0;
				return ok;
			}
			else
			{
				while(reg_h7_flash::SR2_QW2::get(f->SR2)) {}

				f->CCR2 = reg_h7_flash::CCR2_CLR_EOP2::BIT | reg_h7_flash::CCR2_CLR_WRPERR2::BIT
						  | reg_h7_flash::CCR2_CLR_PGSERR2::BIT | reg_h7_flash::CCR2_CLR_STRBERR2::BIT
						  | reg_h7_flash::CCR2_CLR_INCERR2::BIT;

				f->CR2 = reg_h7_flash::CR2_PG2::BIT | (force_flush ? reg_h7_flash::CR2_FW2::BIT : 0U);

				mcu_arm_v7m::dsb();

				for(uint32_t i = 0; i < 8; i++)
				{
					dst[i] = src[i];
				}

				mcu_arm_v7m::dsb();

				while(reg_h7_flash::SR2_WBNE2::get(f->SR2) || reg_h7_flash::SR2_QW2::get(f->SR2)) {}

				bool ok = !(reg_h7_flash::SR2_WRPERR2::get(f->SR2) || reg_h7_flash::SR2_PGSERR2::get(f->SR2)
							|| reg_h7_flash::SR2_STRBERR2::get(f->SR2) || reg_h7_flash::SR2_INCERR2::get(f->SR2));

				f->CCR2 = reg_h7_flash::CCR2_CLR_EOP2::BIT | reg_h7_flash::CCR2_CLR_WRPERR2::BIT
						  | reg_h7_flash::CCR2_CLR_PGSERR2::BIT | reg_h7_flash::CCR2_CLR_STRBERR2::BIT
						  | reg_h7_flash::CCR2_CLR_INCERR2::BIT;

				f->CR2 = 0;
				return ok;
			}
		}

		static void cache_invalidate(const void *addr, uint32_t size)
		{
			volatile uint32_t *dccimvac = (volatile uint32_t *)0xE000EF70U;

			uint32_t a	 = (uint32_t)addr & ~(WORD_SIZE - 1U);
			uint32_t end = ((uint32_t)addr + size + WORD_SIZE - 1U) & ~(WORD_SIZE - 1U);

			mcu_arm_v7m::dsb();
			for(; a < end; a += WORD_SIZE)
			{
				*dccimvac = a;
			}
			mcu_arm_v7m::dsb();
			mcu_arm_v7m::isb();
		}

		static bool verify(uint32_t addr, const uint8_t *src, uint32_t len)
		{
			cache_invalidate((const void *)addr, len);
			const volatile uint8_t *fl = (const volatile uint8_t *)addr;
			for(uint32_t i = 0; i < len; i++)
			{
				if(fl[i] != src[i])
				{
					return false;
				}
			}
			return true;
		}

		static void read_at(uint32_t addr, uint8_t *dst, uint32_t len)
		{
			cache_invalidate((const void *)addr, len);
			const volatile uint8_t *src = (const volatile uint8_t *)addr;
			for(uint32_t i = 0; i < len; i++)
			{
				dst[i] = src[i];
			}
		}
	};

} // namespace lac
