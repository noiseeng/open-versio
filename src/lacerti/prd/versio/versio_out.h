/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <numerus/color.h>

namespace versio
{

	struct versio_out
	{
		numerus::color rgb1;
		numerus::color rgb2;
		numerus::color rgb3;
		numerus::color rgb4;
	};

	constexpr numerus::color get_vu(float x)
	{
		x = 10.0f * (x >= 0.0f ? x : -x); //volts

		if(x > 8.5f)
		{
			return numerus::color::red();
		}
		else if(x > 5.5f)
		{
			return numerus::color::yellow();
		}
		else if(x > 1.0f)
		{
			return numerus::color::green();
		}
		else if(x < 0.01f)
		{
			return numerus::color::black();
		}

		const uint8_t t = (uint8_t)(x * 0xFF);

		return numerus::color::blend(t, numerus::color::black(), numerus::color::green());
	}
} // namespace versio