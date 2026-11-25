/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <cstddef>
#include <math.h>
#include <stdint.h>

namespace lac
{
	struct hui_conf_adcf
	{
		size_t bits		= 20;
		bool   flip		= true;
		float  minimum	= 0.02f;
		float  maximum	= 0.98f;
		float  histzone = 0.0015f;
		float  filter	= 1.0f;

		constexpr float get_bit_scale() const { return 1.0f / (float)((1 << bits) - 1); }
		constexpr float get_range_scale() const { return 1.0f / (maximum - minimum); }
	};

	template <uint32_t count>
	using hui_array_adcf = std::array<hui_conf_adcf, count>;

	struct hui_value_adcf
	{
		float value	 = 0.0f;
		float change = 0.0f;

		constexpr operator float() const { return value; }
		constexpr operator bool() const { return change != 0.0f; }
	};

	struct hui_filter_adcf
	{
		float lpf  = 0.5f;
		float hist = 0.0f;
		float last = 0.0f;

		static constexpr float sqr(float x) { return x * x; }
		static constexpr float rqs(float x) { return 1.0f - sqr(1.0f - x); }
		static constexpr float lerp(float t, float a, float b) { return (b - a) * t + a; }

		constexpr float filter(const hui_conf_adcf& conf, float f)
		{
			const float t = lerp(conf.filter, 1.0f, rqs(fabs(lpf - f)));

			lpf = lerp(t, lpf, f);

			if(lpf <= 0.0f)
			{
				return 0.0f;
			}
			else if(lpf >= 1.0f)
			{
				return 1.0f;
			}
			else
			{
				return fabsf(lpf - hist) > conf.histzone ? lpf : hist;
			}
		}

		constexpr hui_value_adcf operator()(const hui_conf_adcf& conf, uint32_t raw_value)
		{
			const float r = (float)raw_value * conf.get_bit_scale();
			const float f = conf.flip ? 1.0f - r : r;
			const float v = (f - conf.minimum) * conf.get_range_scale();

			hist = filter(conf, v);

			const float d = hist;
			const float c = d - last;

			last = d;

			return {d, c};
		}
	};
} // namespace lac