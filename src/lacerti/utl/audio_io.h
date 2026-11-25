/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace lac
{
	struct audio_io
	{
		using frame_t = std::array<float, 2>;

		struct peaks_t
		{
			frame_t inp = {};
			frame_t out = {};
		};

		constexpr static float OUT_MAX	 = 0.999985f;
		constexpr static float OUT_MIN	 = -OUT_MAX;
		constexpr static float OUT_SCALE = 2147483648.0f;
		constexpr static float INP_SCALE = 4.656612873077393e-10f;

		static constexpr float inp(int32_t x) { return (float)x * INP_SCALE; }

		static constexpr int32_t out(float x)
		{
			x = x > OUT_MIN ? x : OUT_MIN;
			x = x < OUT_MAX ? x : OUT_MAX;
			return (int32_t)(x * OUT_SCALE);
		}

		static constexpr frame_t maxabs(frame_t m, const frame_t& a)
		{
			for(size_t c = 0; c < m.size(); c++)
			{
				const float v = a[c] > 0 ? a[c] : -a[c];
				m[c]		  = m[c] > v ? m[c] : v;
			}
			return m;
		}

		static constexpr frame_t convert(uint32_t frames, const int32_t* in, frame_t* out)
		{
			frame_t peak = {};
			for(uint32_t i = 0; i < frames; i++)
			{
				const frame_t x = {inp(in[2 * i]), inp(in[2 * i + 1])};
				peak			= maxabs(peak, x);
				out[i]			= x;
			}
			return peak;
		}

		static constexpr frame_t convert(uint32_t frames, const frame_t* in, int32_t* out)
		{
			frame_t peak = {};
			for(uint32_t i = 0; i < frames; i++)
			{
				peak		   = maxabs(peak, in[i]);
				out[2 * i]	   = audio_io::out(in[i][0]);
				out[2 * i + 1] = audio_io::out(in[i][1]);
			}
			return peak;
		}

		template <class cb_t>
		static constexpr peaks_t convert(uint32_t frames, const int32_t* in, int32_t* out, cb_t&& cb)
		{
			peaks_t peak = {};
			for(uint32_t i = 0; i < frames; i++)
			{
				const frame_t x = {inp(in[2 * i]), inp(in[2 * i + 1])};
				const frame_t y = cb(x);
				peak.inp		= maxabs(peak.inp, x);
				peak.out		= maxabs(peak.out, y);
				out[2 * i]		= audio_io::out(y[0]);
				out[2 * i + 1]	= audio_io::out(y[1]);
			}
			return peak;
		}
	};
} // namespace lac
