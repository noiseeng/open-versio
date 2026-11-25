/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstring>
#include <version>

#if __cplusplus >= 202002L
#include <bit>
#include <type_traits>
#define bug_is_constant_evaluated() std::is_constant_evaluated()
#else
#if defined(__has_builtin)
#if __has_builtin(__builtin_bit_cast)
#define bug_has_builtin_bit_cast 1
#endif
#if __has_builtin(__builtin_is_constant_evaluated)
#define bug_is_constant_evaluated() __builtin_is_constant_evaluated()
#endif
#endif
#ifndef bug_is_constant_evaluated
#define bug_is_constant_evaluated() false
#endif
#endif

#if defined(_MSC_VER)
#define DBG_FUNCNAME __FUNCSIG__
#else
#define DBG_FUNCNAME __PRETTY_FUNCTION__
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define bug_trap() __debugbreak()
#else
#define bug_trap() __builtin_trap()
#endif

#if __cplusplus >= 202002L
#define bug_constexpr_runtime constexpr inline
#else
#define bug_constexpr_runtime inline
#endif

#if defined(DEBUG)
#define bug_inline
#elif defined(_MSC_VER)
#define bug_inline __forceinline
#else
#define bug_inline __attribute__((always_inline)) inline
#endif

#if defined(_MSC_VER)
#define bug_force_inline __forceinline
#define bug_no_inline __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define bug_force_inline __attribute__((always_inline)) inline
#define bug_no_inline __attribute__((noinline))
#else
#define bug_force_inline inline
#define bug_no_inline
#endif

#if defined(_MSC_VER)
#define bug_is_constexpr_expr(x) false
#elif defined(__GNUC__) || defined(__clang__)
#define bug_is_constexpr_expr(x) (__builtin_constant_p(x))
#else
#define bug_is_constexpr_expr(x) false
#endif

#if defined(_MSC_VER)
#define bug_naked __declspec(naked)
#else
#define bug_naked __attribute__((naked))
#endif

#if defined(_MSC_VER)
#define bug_packed_begin __pragma(pack(push, 1))
#define bug_packed_end __pragma(pack(pop))
#define bug_packed
#else
#define bug_packed_begin
#define bug_packed_end
#define bug_packed __attribute__((packed))
#endif

namespace bug
{
	void debug_break();

	constexpr bool is_sanitize_address()
	{
#if defined(__SANITIZE_ADDRESS__)
		return true;
#elif defined(__has_feature)
		return __has_feature(address_sanitizer);
#else
		return false;
#endif
	}

	template <typename To, typename From>
	constexpr To bit_cast(From from) noexcept
	{
#if __cplusplus >= 202002L
		return std::bit_cast<To>(from);
#elif defined(bug_has_builtin_bit_cast)
		return __builtin_bit_cast(To, from);
#else
		To result;
		std::memcpy(&result, &from, sizeof(To));
		return result;
#endif
	}

	template <typename T>
	constexpr T rotl(T x, int n) noexcept
	{
		constexpr int W = (int)(sizeof(T) * 8);
		return (T)((x << (n & (W - 1))) | (x >> ((W - n) & (W - 1))));
	}

	template <typename T>
	constexpr T rotr(T x, int n) noexcept
	{
		constexpr int W = (int)(sizeof(T) * 8);
		return (T)((x >> (n & (W - 1))) | (x << ((W - n) & (W - 1))));
	}

	template <class F, int = (F{}(), 0)>
	constexpr inline bool is_constexpr(F)
	{ return true; }

	constexpr inline bool is_constexpr(...)
	{ return false; }

	template <class... Ts>
	constexpr void ignore(const Ts &...)
	{}
} // namespace bug
