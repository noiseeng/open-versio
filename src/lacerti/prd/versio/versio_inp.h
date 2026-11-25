/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace versio
{
	struct versio_inp
	{
		float	k1		  = 0.0f; // knobs, each plus its CV, 0 to 1
		float	k2		  = 0.0f;
		float	k3		  = 0.0f;
		float	k4		  = 0.0f;
		float	k5		  = 0.0f;
		float	k6		  = 0.0f;
		float	k7		  = 0.0f;
		int32_t sw1		  = 0;	   // ABC switch, 0 (bottom) to 2 (top)
		int32_t sw2		  = 0;	   // XYZ switch, 0 (bottom) to 2 (top)
		bool	btn_trig  = false; // FSU button was pressed this block
		bool	btn_gate  = false; // FSU button is held
		bool	jack_trig = false; // FSU gate jack went high this block
		bool	jack_gate = false; // FSU gate jack is high

		int32_t tempo_fps		  = 0;	   // period in frames tapped on the button or jack, up to 4 s; 0 is none
		bool	tap_cleared		  = false; // button reached a 2 s hold, which clears tempo_fps
		bool	last_hit		  = false; // button or jack was hit in the previous block
		bool	tap_clear_pending = false; // button reached a 4 s hold; the board is about to call clear_memory()

		float meter_inpl = 0.0f; // input and output peak levels with decay, 1 is full scale
		float meter_inpr = 0.0f;
		float meter_outl = 0.0f;
		float meter_outr = 0.0f;
	};
} // namespace versio