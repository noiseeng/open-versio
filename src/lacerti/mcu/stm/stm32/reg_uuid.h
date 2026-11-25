/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <array>
#include <cstdint>
#include <utl/bitfield.h>

namespace lac
{
	struct reg_uuid
	{
		uint32_t W0;
		uint32_t W1;
		uint32_t W2;

		using W0_BCDY	   = bitfield<0, 16>;
		using W0_BCDX	   = bitfield<16, 16>;
		using W1_LOT_ASCII = bitfield<8, 24>;
		using W1_WAFER	   = bitfield<0, 8>;
		using W2_LOT_ASCII = bitfield<0, 32>;

		const char*		  get_char() const { return (const char*)this; }
		const char*		  get_lot() const { return get_char() + 6; }
		const char*		  get_loc() const { return get_char(); }
		constexpr uint8_t get_wafer() const { return (uint8_t)(W1 & 0xFF); }

		const std::array<uint8_t, 12>& get_bytes() const { return *(const std::array<uint8_t, 12>*)this; }
	};
} // namespace lac
