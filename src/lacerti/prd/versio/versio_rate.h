/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace versio
{
	constexpr float versio_meter_decay(uint32_t frame_rate)
	{ return 1.0f - (3000.0f / (float)frame_rate) / 256.0f; }

	constexpr std::array<float, 2> versio_meter_max(const std::array<float, 2>& m, const std::array<float, 2>& x, float decay)
	{
		std::array<float, 2> r = {};
		for(size_t c = 0; c < r.size(); ++c)
		{
			const float ax = x[c] > 0 ? x[c] : -x[c];
			r[c]		   = (m[c] > ax ? m[c] : ax) * decay;
		}
		return r;
	}
} // namespace versio
