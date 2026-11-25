/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "std.h"
#include <bug/eabi.h>
#include <new> // IWYU pragma: keep
#include <stdint.h>

namespace lac
{
	template <class allocator_t>
	struct allocator
	{
		allocator_t source;

		const static uint32_t SHRED_ALLOC = 0xFEEDBABE;
		const static uint32_t SHRED_FREE  = 0xDEADBEEF;

		static constexpr bool shred = !bug::is_build_ship();

		constexpr allocator() {}

		void* alloc_internal(uint32_t size, void** block, const char* function)
		{
			size = (uint32_t)align(size, 8);

			*block = source.alloc(size, function);

			if(shred)
			{
				shredit(*block, size);
			}

			return *block;
		}

		void* alloc(uint32_t size, void** block, const char* function) { return alloc_internal(size, block, function); }

		void free(void* block, const char* function) { source.free(block, function); }

		template <typename obj_type>
		obj_type* alloc(obj_type** obj, const char* function)
		{ return (obj_type*)alloc_internal(sizeof(obj_type), (void**)obj, function); }

		template <typename obj_t>
		obj_t* alloc_array_init(uint32_t count, obj_t** objs, const char* fn)
		{
			alloc(sizeof(obj_t) * count, (void**)objs, fn);

			for(uint32_t i = 0; i < count; i++)
			{
				new(*objs + i) obj_t();
			}

			return *objs;
		}

		static void shredit(void* data, uint32_t size)
		{
			uint32_t* d = (uint32_t*)data;

			for(uint32_t i = 0; i < size / 4; i++)
			{
				d[i] = SHRED_ALLOC;
			}
		}

		void lock() { source.lock(); }

		void unlock() { source.unlock(); }
	};

} // namespace lac
