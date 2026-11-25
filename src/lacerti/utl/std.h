/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "bug.h"
#include <concepts>
#include <stdint.h>
#include <string.h>

namespace lac
{
	constexpr inline auto align(std::integral auto size, std::integral auto align)
	{
		lac_assert(((align - 1) & align) == 0);

		decltype(size) mask = align - 1;
		decltype(size) ofs	= size & mask;

		return ofs ? size + align - ofs : size;
	}

	template <typename T>
	inline bool same(const T &a, const T &b)
	{
		if constexpr(alignof(T) >= 4 && (sizeof(T) % 4) == 0)
		{
			const uint32_t *wa = (const uint32_t *)&a;
			const uint32_t *wb = (const uint32_t *)&b;

#pragma GCC unroll 1
			for(size_t i = 0; i < sizeof(T) / 4; i++)
			{
				if(wa[i] != wb[i])
				{
					return false;
				}
			}

			return true;
		}
		else
		{
			const uint8_t *ba = (const uint8_t *)&a;
			const uint8_t *bb = (const uint8_t *)&b;

			for(size_t i = 0; i < sizeof(T); i++)
			{
				if(ba[i] != bb[i])
				{
					return false;
				}
			}

			return true;
		}
	}

	inline void memzero(void *d, size_t n)
	{
		lac_assert(!(((intptr_t)d) & 3));
		lac_assert(!(n & 3));

		size_t	  c = n >> 2;
		uint32_t *h = (uint32_t *)d;

		const uint32_t *e = h + c;

		while(h != e)
		{
			*h++ = 0;
		}
	}
} // namespace lac