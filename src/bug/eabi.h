/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "compiler.h"

#include <stdarg.h>

namespace bug
{
	inline constexpr bool is_build_debug()
	{
#ifdef DEBUG
		return true;
#else
		return false;
#endif
	}

	inline constexpr bool is_build_ship()
	{
#ifdef NDEBUG
		return true;
#else
		return false;
#endif
	}

	inline constexpr bool is_build_asan()
	{
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
		return true;
#else
		return false;
#endif
#elif defined(__SANITIZE_ADDRESS__)
		return true;
#else
		return false;
#endif
	}

	void abort(const char* file, int line, const char* expr);

	void abort();

	void print(const char* fmt, ...);

	void print_va(const char* fmt, va_list* ap);

	using print_sink_t = void (*)(const char* text, void* ctx);

	void set_print_sink(print_sink_t fn, void* ctx);

	bug_constexpr_runtime void constexpr_abort()
	{
		if(bug_is_constant_evaluated())
		{
			bug_trap();
		}
		else
		{
			bug::abort();
		}
	}

	bug_constexpr_runtime void constexpr_abort(const char* file, int line, const char* expr)
	{
		if(bug_is_constant_evaluated())
		{
			bug_trap();
		}
		else
		{
			bug::abort(file, line, expr);
		}
	}

} // namespace bug

#ifndef bug_assert_slim
#define bug_assert_slim(x)              \
	if constexpr(!bug::is_build_ship()) \
	{                                   \
		if(!(x))                        \
		{                               \
			bug::constexpr_abort();     \
		}                               \
	}
#endif

#ifndef bug_assert
#if BUG_ASSERT_SLIM
#define bug_assert(x) bug_assert_slim(x)
#else
#define bug_assert(x)                                     \
	if constexpr(!bug::is_build_ship())                   \
	{                                                     \
		if(!(x))                                          \
		{                                                 \
			bug::constexpr_abort(__FILE__, __LINE__, #x); \
		}                                                 \
	}
#endif
#endif

#ifndef bug_assert_fancy
#define bug_assert_fancy(x)                               \
	if constexpr(!bug::is_build_ship())                   \
	{                                                     \
		if(!(x))                                          \
		{                                                 \
			bug::constexpr_abort(__FILE__, __LINE__, #x); \
		}                                                 \
	}
#endif

#ifndef bug_assert_debug
#define bug_assert_debug(x)                               \
	if constexpr(bug::is_build_debug())                   \
	{                                                     \
		if(!(x))                                          \
		{                                                 \
			bug::constexpr_abort(__FILE__, __LINE__, #x); \
		}                                                 \
	}
#endif

#ifndef bug_print
#ifdef NDEBUG
#define bug_print(...)                 \
	do                                 \
	{                                  \
		if(false)                      \
			::bug::print(__VA_ARGS__); \
	} while(0)
#else
#define bug_print(...) ::bug::print(__VA_ARGS__)
#endif
#endif