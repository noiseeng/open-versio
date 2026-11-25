/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <bug/eabi.h>
#include <utl/bug.h>
#include <utl/std.h>

#include <atomic>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>

namespace lac
{
	enum ram_zone
	{
		RAM_ZONE_INVALID = -1,
		RAM_ZONE_0		 = 0,
		RAM_ZONE_1,
		RAM_ZONE_2,
		RAM_ZONE_3,
		RAM_ZONE_4,
		RAM_ZONE_5,
		RAM_ZONE_6,
	};

	enum ram_cache
	{
		RAM_NOCACHE,
		RAM_CACHE,
		RAM_DEFAULT,
	};

	struct ram_config
	{
		const char* name;
		ram_cache	cache;
		uintptr_t	addr;
		uintptr_t	size;
	};

	struct ram_usage
	{
		uintptr_t size = 0;
		uintptr_t used = 0;
	};

	struct ram
	{
		constexpr static size_t MAX_ZONES = 8;
		constexpr static size_t ALIGN	  = 8;

		const size_t	  count	 = 0;
		const ram_config* config = 0;

		ram_usage usage[MAX_ZONES] = {};

		constexpr ram() {}

		constexpr ram(size_t count, const ram_config* config) : count(count), config(config)
		{
			for(size_t i = 0; i < count; i++)
			{
				usage[i].used = 0;
				usage[i].size = config[i].size;
			}
		}

		constexpr void enable(ram_zone type, size_t size)
		{
			lac_assert((size_t)type < count);
			lac_assert(!size || !usage[type].used); //only allow disabling or enabling
			usage[type].size = size;
		}

		constexpr ram_zone get_zone(const void* vp) const
		{
			uintptr_t addr = std::bit_cast<uintptr_t>(vp);

			for(uint32_t i = 0; i < count; i++)
			{
				const auto& d = config[i];

				if(addr >= d.addr && addr < d.addr + usage[i].size)
				{
					return (ram_zone)i;
				}
			}

			lac_assert(false);

			return RAM_ZONE_INVALID;
		}

		void* alloc(ram_zone type, uintptr_t size, const char* extra)
		{
			lac_assert((size_t)type < count);

			const auto& d = config[type];
			auto&		u = usage[type];

			u.used = align(u.used, ALIGN);

			size = align(size, ALIGN);

			if(u.used + size > u.size)
			{
				bug::abort();
			}

			void* block = (void*)(d.addr + u.used);

			u.used += size;

			bug_print("ram::alloc %s %p 0x%05x %s\n", d.name, block, size, extra);

			return block;
		}

		void* alloc(size_t size, ram_cache cache, const char* extra)
		{
			size = align(size, ALIGN);

			for(size_t type = 0; type < count; type++)
			{
				const auto& d = config[type];
				const auto& u = usage[type];

				if(align(u.used, ALIGN) + size <= u.size && d.cache == cache)
				{
					return alloc((ram_zone)type, size, extra);
				}
			}

			bug_print("alloc failure %x %s\n", size, extra);

			bug::abort();

			return 0;
		}

		void* alloc_first(size_t size, const char* extra)
		{
			size = align(size, ALIGN);

			for(size_t zone = 0; zone < count; zone++)
			{
				if(align(usage[zone].used, ALIGN) + size <= usage[zone].size)
				{
					return alloc((ram_zone)zone, size, extra);
				}
			}

			bug_print("alloc failure %x %s\n", size, extra);

			bug::abort();

			return 0;
		}

		void* alloc_order(std::span<const ram_zone> zones, size_t size, const char* extra)
		{
			size = align(size, ALIGN);

			for(const auto& zone : zones)
			{
				lac_assert((size_t)zone < count);
				if(align(usage[zone].used, ALIGN) + size <= usage[zone].size)
				{
					return alloc((ram_zone)zone, size, extra);
				}
			}

			bug_print("alloc failure %x %s\n", size, extra);

			bug::abort();

			return 0;
		}

		template <typename class_t>
		class_t* alloc(size_t count, ram_cache cached, const char* extra)
		{
			class_t* c = (class_t*)alloc(sizeof(class_t) * count, cached, extra);

			for(size_t i = 0; i < count; i++)
			{
				new(&c[i]) class_t();
			}

			return c;
		}

		template <typename class_t>
		class_t* alloc(ram_cache cached, const char* extra)
		{ return alloc<class_t>(1, cached, extra); }

		constexpr void linker(const void* ptr, size_t size)
		{
			if(size)
			{
				const ram_zone type = get_zone(ptr);

				auto&			u	= usage[type];
				const uintptr_t off = std::bit_cast<uintptr_t>(ptr) - config[type].addr;

				lac_assert(off >= u.used);
				lac_assert(off + size <= u.size);

				u.used = off + size;
			}
		}

		void test(ram_zone type, bool fast = false)
		{
			const auto& d = config[type];
			const auto& a = usage[type];

			uint32_t* block = (uint32_t*)(d.addr + a.used);
			uint32_t  count = (d.size - a.used) / sizeof(uint32_t);

			if(fast)
			{
				count = count < 32 ? count : 32;
			}

			for(uint32_t i = 0; i < count; i++)
			{
				block[i] = 0x00000000;

				std::atomic_thread_fence(std::memory_order_seq_cst);

				if(block[i] != 0x00000000)
				{
					bug::abort();
				}

				for(int i = 0; i < 32; i++)
				{
					uint32_t test = 1 << i;

					block[i] = test;

					std::atomic_thread_fence(std::memory_order_seq_cst);

					if(block[i] != test)
					{
						bug::abort();
					}
				}

				block[i] = 0;
			}
		}

		constexpr void test()
		{
			for(size_t i = 0; i < count; i += 1)
			{
				test((ram_zone)i);
			}
		}

		void trace() const
		{
			if(!bug::is_build_ship())
			{
				for(size_t i = 0; i < count; i++)
				{
					bug_print("%s 0x%x 0x%07x 0x%07x\n",
							  config[i].name,
							  (unsigned)config[i].addr,
							  (unsigned)usage[i].used,
							  (unsigned)(usage[i].size - usage[i].used));
				}
			}
		}
	};

} // namespace lac