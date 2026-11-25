/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <type_traits>

namespace lac
{
	struct slug
	{
		float data[1024];

		constexpr slug()
		{
			for(uint32_t i = 0; i < 1024; i++)
			{
				data[i] = (float)i;
			}
		}

		template <class cb_t>
		void operator()(cb_t&& cb)
		{
			std::atomic_thread_fence(std::memory_order_seq_cst);
			for(uint32_t i = 0; i < 1022; i++)
			{
				if constexpr(std::is_same_v<decltype(cb()), bool>)
				{
					if(!cb())
					{
						break;
					}
				}
				else
				{
					cb();
				}

				data[i] = 0.5f * data[i] + 0.25f * data[i + 1] - 0.125f * data[i + 2];
			}
		}

		constexpr void operator()()
		{
			operator()([]() {});
		}
	};

} // namespace lac
