/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstdint>

#include "versio_inp.h"
#include "versio_meters.h"
#include "versio_out.h"
#include <numerus/color.h>

namespace versio
{
	template <uint32_t frame_count>
	struct av_example
	{
		constexpr void finish() {}
		constexpr void print() {}

		constexpr static numerus::color color0 = numerus::color::white();
		constexpr static numerus::color color1 = numerus::color::white();
		constexpr static numerus::color color2 = numerus::color::white();
		constexpr static numerus::color color3 = numerus::color::white();

		constexpr static uint32_t FRAME_COUNT = frame_count;
		constexpr static bool	  BLOCK		  = false;

		constexpr static float MAX_SECONDS	= 2.0f;
		constexpr static float MIN_SECONDS	= 0.01f;
		constexpr static float SLEW_SECONDS = 0.05f;
		constexpr static float MAX_FEEDBACK = 0.95f;

		using stereo = std::array<float, 2>;

		struct controls : versio_meters
		{
			float time	   = 0.0f;
			float feedback = 0.0f;
			float mix	   = 0.0f;

			constexpr controls() = default;

			constexpr controls(const versio_inp& av) : versio_meters(av), time(av.k1), feedback(av.k2), mix(av.k3) {}
		};

		using ctl_t = controls;

		stereo*	 buffer		= nullptr;
		uint32_t size		= 0;
		uint32_t write		= 0;
		float	 frame_rate = 96000.0f;
		float	 slew		= 1.0f;
		float	 target		= 1.0f;
		float	 delay		= 1.0f;
		float	 feedback	= 0.0f;
		float	 mix		= 0.0f;

		template <typename alloc_t>
		void set_frame_rate(float fps, alloc_t* alloc)
		{
			frame_rate = fps;
			slew	   = 1.0f / (SLEW_SECONDS * fps);
			size	   = (uint32_t)(MAX_SECONDS * fps) + 2;
			alloc->alloc_array_init(size, &buffer, "delay");
		}

		void clear_memory() { std::fill(buffer, buffer + size, stereo{}); }

		constexpr versio_out control(const controls& ctl)
		{
			target	 = (MIN_SECONDS + (MAX_SECONDS - MIN_SECONDS) * ctl.time) * frame_rate;
			feedback = ctl.feedback * MAX_FEEDBACK;
			mix		 = ctl.mix;
			return vu_out(ctl);
		}

		constexpr uint32_t frames_ago(uint32_t frames) const
		{ return write >= frames ? write - frames : write + size - frames; }

		constexpr stereo operator()(stereo x)
		{
			delay += (target - delay) * slew;

			const uint32_t whole = (uint32_t)delay;
			const float	   frac	 = delay - (float)whole;
			const stereo   near	 = buffer[frames_ago(whole)];
			const stereo   far	 = buffer[frames_ago(whole + 1)];

			stereo y = {};
			for(uint32_t c = 0; c < y.size(); c++)
			{
				const float wet	 = near[c] + (far[c] - near[c]) * frac;
				buffer[write][c] = std::clamp(x[c] + wet * feedback, -1.0f, 1.0f);
				y[c]			 = x[c] + (wet - x[c]) * mix;
			}

			write = write + 1 < size ? write + 1 : 0;

			return y;
		}
	};

} // namespace versio
