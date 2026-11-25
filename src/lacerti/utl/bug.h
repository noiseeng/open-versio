/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <bug/eabi.h>
#include <stdarg.h>

namespace lac
{
	constexpr inline bool use_log()
	{
#if LAC_USE_LOG
		return true;
#else
		return false;
#endif
	}
} // namespace lac

#if LAC_MCU
#include <bug/format.h>
namespace lac
{
	void write_char(char c, void *ctx);
} // namespace lac

namespace bug
{
	inline void abort(const char *file, int line, const char *expr)
	{
		if constexpr(!bug::is_build_ship())
		{
			bug_print("bug_assert: %s:%d - %s\n", file, line, expr);
			bug_trap();
		}
	}

	inline void abort()
	{
		if constexpr(!bug::is_build_ship())
		{
			bug_trap();
		}
	}

	inline void print_va(const char *fmt, va_list *ap)
	{
		if constexpr(lac::use_log())
		{
			bug::format::va(lac::write_char, (void *)0, fmt, *ap);
		}
	}

	inline void print(const char *fmt, ...)
	{
		if constexpr(!bug::is_build_ship())
		{
			va_list ap;
			va_start(ap, fmt);
			bug::print_va(fmt, &ap);
		}
	}

} // namespace bug
#endif

#define LAC_STRINGIFY(x) #x
#define LAC_AT __FILE__ ":" LAC_STRINGIFY(__LINE__)

#if defined(LAC_SHIP)
#define lac_assert(x)
#define lac_assert_fancy(x)
#define lac_only_assert(x)
#else
#define lac_assert_fancy(x) bug_assert(x)

#if LAC_ALWAYS_FANCY && !defined(LAC_DEBUG)
#define lac_assert(x) bug_assert(x)
#else
#define lac_assert(x) bug_assert_slim(x)
#endif
#define lac_only_assert(x) lac_assert(x)
#endif

#if defined(LAC_DEBUG)
#define lac_debug_assert(x) bug_assert_debug(x)
#else
#define lac_debug_assert(x)
#endif

#define lac_assert_align(ptr, align) lac_assert(0 == (((intptr_t)(ptr)) & ((align) - 1)))
#define lac_assert_ispow2(x) lac_assert(((x) & ((x) - 1)) == 0)
