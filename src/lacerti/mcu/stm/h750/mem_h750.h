/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include <bug/compiler.h>
#include <mcu/ram.h>
#include <utl/allocator.h>
#include <utl/bug.h>

namespace lac
{
	struct mem_h750
	{
		constexpr static uint32_t STACK_TOP	 = 0x20020000;
		constexpr static uint32_t STACK_SIZE = 4 * 1024;
		constexpr static uint32_t STACK_BOT	 = STACK_TOP - STACK_SIZE;

		constexpr static ram_zone ZONE_DTCM	 = RAM_ZONE_0;
		constexpr static ram_zone ZONE_ITCM	 = RAM_ZONE_1;
		constexpr static ram_zone ZONE_SRAM1 = RAM_ZONE_2;
		constexpr static ram_zone ZONE_SRAM2 = RAM_ZONE_3;
		constexpr static ram_zone ZONE_SRAM4 = RAM_ZONE_4;
		constexpr static ram_zone ZONE_SRAMB = RAM_ZONE_5;
		constexpr static ram_zone ZONE_FMC	 = RAM_ZONE_6;

		constexpr static std::array<ram_config, 7> ZONES = {
			{
				{"DTCM", RAM_DEFAULT, 0x20000000, 128 * 1024 - STACK_SIZE},
				{"ITCM", RAM_DEFAULT, 0x00000000, 64 * 1024},
				{"SRAM1", RAM_DEFAULT, 0x24000000, 512 * 1024},
				{"SRAM2", RAM_DEFAULT, 0x30000000, 288 * 1024},
				{"SRAM4", RAM_NOCACHE, 0x38000000, 64 * 1024},
				{"SRAMB", RAM_DEFAULT, 0x38800000, 4 * 1024},
				{"FMC", RAM_CACHE, 0x60000000, 64 * 1024 * 1024},
			},
		};

		constexpr static std::array<ram_zone, 3> CHEAP_ZONES = {ZONE_FMC, ZONE_SRAM2, ZONE_SRAM1};

		inline static ram* instance = 0;

		static ram* get()
		{
			lac_assert(instance);
			return instance;
		}

		static void* alloc_fast(size_t size, const char* extra) { return get()->alloc_first(size, extra); }

		static void* alloc_cheap(size_t size, const char* extra) { return get()->alloc_order(CHEAP_ZONES, size, extra); }

		static void* alloc_dma(size_t size, const char* extra) { return get()->alloc(ZONE_SRAM4, size, extra); }

		static void trace() { get()->trace(); }

		template <typename T>
		static T* alloc_dma(size_t count, const char* extra)
		{
			T* c = (T*)alloc_dma(sizeof(T) * count, extra);

			for(size_t i = 0; i < count; i++)
			{
				new(&c[i]) T();
			}

			return c;
		}

		template <typename T>
		static T* alloc_fast(size_t count, const char* extra)
		{
			T* c = (T*)alloc_fast(sizeof(T) * count, extra);

			for(size_t i = 0; i < count; i++)
			{
				new(&c[i]) T();
			}

			return c;
		}

		template <typename T>
		static T* alloc_dma()
		{
			void* obj = alloc_dma(sizeof(T), "");
			return new(obj) T{};
		}
	};

	struct mem_h750_alloc
	{
		constexpr static void* alloc(uint32_t count, const char* fn = "") { return mem_h750::alloc_fast(count, fn); }

		constexpr static void free(void* block, const char* fn = 0)
		{
			bug::ignore(block);
			bug::ignore(fn);
			lac_assert(false);
		}

		constexpr static void lock() {}

		constexpr static void unlock() {}
	};

	using mem_h750_alloc_t = allocator<mem_h750_alloc>;
} // namespace lac
