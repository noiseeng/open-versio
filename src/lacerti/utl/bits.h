/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <atomic>
#include <bit>
#include <concepts>
#include <stdint.h>

inline constexpr uint32_t BIT0 = 1u << 0;
inline constexpr uint32_t BIT1 = 1u << 1;
inline constexpr uint32_t BIT2 = 1u << 2;
inline constexpr uint32_t BIT3 = 1u << 3;
inline constexpr uint32_t BIT4 = 1u << 4;
inline constexpr uint32_t BIT5 = 1u << 5;
inline constexpr uint32_t BIT6 = 1u << 6;
inline constexpr uint32_t BIT7 = 1u << 7;
inline constexpr uint32_t BIT8 = 1u << 8;
inline constexpr uint32_t BIT9 = 1u << 9;

namespace lac
{

	struct bits
	{
		static void reg_set(auto &r, auto b) { r = r | b; }

		static auto reg_get(auto &r, auto b) { return r & b; }

		static void reg_clr(auto &r, auto b) { r = r & (~b); }

		static void reg_clrset(auto &r, auto c, auto s) { r = (r & (~c)) | s; }

		static void reg_sync(auto &r)
		{
			auto v = r;
			(void)v;
			std::atomic_thread_fence(std::memory_order_seq_cst);
		}

		static void reg_setdly(auto &r, auto b)
		{
			reg_set(r, b);
			reg_sync(r);
		}

		static void reg_clrdly(auto &r, auto b)
		{
			reg_clr(r, b);
			reg_sync(r);
		}

		static void reg_clrsetdly(auto &r, auto c, auto s)
		{
			reg_clrset(r, c, s);
			reg_sync(r);
		}

		template <typename integral_t>
		static constexpr integral_t flag(uint32_t pos)
		{ return (integral_t)1 << pos; }

		template <typename integral_t>
		static constexpr integral_t clear_mask(uint32_t pos)
		{ return ~flag<integral_t>(pos); }

		static constexpr bool get(std::unsigned_integral auto b, uint32_t pos)
		{
			if constexpr(sizeof(b) == 8)
			{
				return (b & flag<uint64_t>(pos)) != 0;
			}
			if constexpr(sizeof(b) == 4)
			{
				return (b & flag<uint32_t>(pos)) != 0;
			}
			if constexpr(sizeof(b) == 2)
			{
				return (b & flag<uint16_t>(pos)) != 0;
			}
			if constexpr(sizeof(b) == 1)
			{
				return (b & flag<uint8_t>(pos)) != 0;
			}
		}

		static constexpr auto set(std::unsigned_integral auto b, uint32_t pos)
		{
			if constexpr(sizeof(b) == 8)
			{
				return (uint64_t)(b | flag<uint64_t>(pos));
			}
			if constexpr(sizeof(b) == 4)
			{
				return (uint32_t)(b | flag<uint32_t>(pos));
			}
			if constexpr(sizeof(b) == 2)
			{
				return (uint16_t)(b | flag<uint16_t>(pos));
			}
			if constexpr(sizeof(b) == 1)
			{
				return (uint8_t)(b | flag<uint8_t>(pos));
			}
		}

		static constexpr auto clr(std::unsigned_integral auto b, uint32_t pos)
		{
			if constexpr(sizeof(b) == 8)
			{
				return (uint64_t)(b & clear_mask<uint64_t>(pos));
			}
			if constexpr(sizeof(b) == 4)
			{
				return (uint32_t)(b & clear_mask<uint32_t>(pos));
			}
			if constexpr(sizeof(b) == 2)
			{
				return (uint16_t)(b & clear_mask<uint16_t>(pos));
			}
			if constexpr(sizeof(b) == 1)
			{
				return (uint8_t)(b & clear_mask<uint8_t>(pos));
			}
		}

		static constexpr auto set(std::unsigned_integral auto b, uint32_t pos, bool v) { return v ? set(b, pos) : clr(b, pos); }

		static constexpr auto tog(std::unsigned_integral auto b, uint32_t pos) { return set(b, pos, !get(b, pos)); }

		template <uint32_t MASK>
		static consteval uint32_t count()
		{ return std::popcount(MASK); }

		static constexpr uint32_t count(std::unsigned_integral auto mask) { return (uint32_t)std::popcount(mask); }

		template <uint32_t MASK>
		static consteval std::array<uint8_t, std::popcount(MASK)> positions()
		{
			std::array<uint8_t, std::popcount(MASK)> pos{};
			uint32_t								 i = 0;
			for(uint32_t mask = MASK; mask; mask &= mask - 1)
			{
				uint32_t bit = mask & (0u - mask);
				uint8_t	 n	 = 0;
				while(!(bit & 1))
				{
					bit >>= 1;
					++n;
				}
				pos[i++] = n;
			}
			return pos;
		}

		template <uint32_t MASK>
		static constexpr uint32_t gather(uint32_t values)
		{
			constexpr auto pos	  = positions<MASK>();
			uint32_t	   result = 0;
			for(uint32_t i = 0; i < pos.size(); ++i)
			{
				if(values & (1u << pos[i]))
				{
					result |= 1u << i;
				}
			}
			return result;
		}

		template <uint32_t MASK>
		static constexpr uint32_t scatter(uint32_t values)
		{
			constexpr auto pos	  = positions<MASK>();
			uint32_t	   result = 0;

			for(uint32_t i = 0; i < pos.size(); ++i)
			{
				if(values & (1u << i))
				{
					result |= 1u << pos[i];
				}
			}

			return result;
		}

		template <typename integer_t, class cb_t>
		static constexpr void for_each_set(integer_t i, cb_t &&cb)
		{
			constexpr uint32_t count = 8 * sizeof(integer_t);

			integer_t f = 1;

			for(uint32_t b = 0; b < count; b++)
			{
				if(f & i)
				{
					cb(b);
				}

				f = f << 1;
			}
		}
	};
}; // namespace lac