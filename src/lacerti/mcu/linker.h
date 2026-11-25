/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <utl/bug.h>

#include <atomic>
#include <cstdint>

extern "C" __attribute__((naked, noreturn)) void reset_handler();

extern "C" int	main();
extern "C" void main_cm7();
extern "C" void main_cm4();
extern "C" void (*__preinit_array_start[])();
extern "C" void (*__preinit_array_end[])();
extern "C" void (*__init_array_start[])();
extern "C" void (*__init_array_end[])();
extern "C" void	   *_estack;
extern "C" uint32_t _sidata;
extern "C" uint32_t _sdata;
extern "C" uint32_t _edata;
extern "C" uint32_t _scode;
extern "C" uint32_t _ecode;
extern "C" uint32_t _sicode;
extern "C" uint32_t _sbss;
extern "C" uint32_t _ebss;
extern "C" uint32_t _ssram;
extern "C" uint32_t _esram;
extern "C" uint32_t _stext;
extern "C" uint32_t _etext;
extern "C" uint32_t _sram_image_lma;

namespace lac
{

	struct linker
	{
		static void code()
		{
			for(int i = 0; i < (__preinit_array_end - __preinit_array_start); i++)
			{
				__preinit_array_start[i]();
			}

			for(int i = 0; i < (__init_array_end - __init_array_start); i++)
			{
				__init_array_start[i]();
			}
		}

		const static uint32_t COPY_RETRIES = 16;

		constexpr static void copy(const uint32_t *src, uint32_t *dst, const uint32_t *end)
		{
			lac_assert(dst <= end);
			lac_assert(src == dst || src + (end - dst) <= dst || src >= end);

			while(dst != end)
			{
				for(uint32_t i = COPY_RETRIES; *dst != *src && i; i--)
				{
					*dst = *src;
					std::atomic_thread_fence(std::memory_order_seq_cst);
				}

				lac_assert(*dst == *src);

				dst += 1;
				src += 1;
			}
		}

		constexpr static void verify(const uint32_t *src, uint32_t *dst, const uint32_t *end)
		{
			while(dst != end)
			{
				lac_assert(*dst == *src);

				dst += 1;
				src += 1;
			}
		}

		constexpr static void zero(uint32_t *dst, const uint32_t *end)
		{
			lac_assert(dst <= end);

			while(dst < end)
			{
				*dst++ = 0;
			}
		}

		constexpr static void shred(uint32_t *dst, const uint32_t *end)
		{
			while(dst != end)
			{
				*dst++ = 0xDEADBEEF;
				std::atomic_thread_fence(std::memory_order_seq_cst);
			}
		}

		static void init()
		{
			zero(&_sbss, &_ebss);
			copy(&_sidata, &_sdata, &_edata);
			verify(&_sidata, &_sdata, &_edata);
			code();
		}

		template <typename mem_t>
		static void usage(mem_t &mem)
		{
			const uint8_t *str_data = (const uint8_t *)&_sdata;
			const uint8_t *end_data = (const uint8_t *)&_edata;
			mem.linker(str_data, end_data - str_data);

			const uint8_t *str_bss = (const uint8_t *)&_sbss;
			const uint8_t *end_bss = (const uint8_t *)&_ebss;
			mem.linker(str_bss, end_bss - str_bss);

			const uint8_t *str_sram = (const uint8_t *)&_ssram;
			const uint8_t *end_sram = (const uint8_t *)&_esram;
			mem.linker(str_sram, end_sram - str_sram);
		}
	};

} // namespace lac
