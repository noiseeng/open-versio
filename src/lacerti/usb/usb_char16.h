/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <cstddef>
#include <utility>

namespace lac
{
	constexpr inline size_t strlen16(const char16_t* const& strarg)
	{
		if(!strarg)
		{
			return (size_t)-1;
		}

		const char16_t* str = strarg;

		for(; *str; ++str)
			;

		return str - strarg;
	}

	template <std::size_t N, std::size_t... Is>
	constexpr inline std::array<char16_t, N> char16_to_array(const char16_t* const& a, std::index_sequence<Is...>)
	{ return {{a[Is]...}}; }

	template <std::size_t N>
	constexpr inline std::array<char16_t, N> char16_to_array(const char16_t* const& a)
	{ return char16_to_array<N>(a, std::make_index_sequence<N>()); }
} // namespace lac
