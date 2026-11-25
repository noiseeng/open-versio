/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

namespace lac
{
	template <typename pdn, typename timer_t>
	struct dev_ak4554
	{
		constexpr static uint32_t get_frame_rate() { return 96000; }

		static void init()
		{
			pdn::init_out();
			pdn::clr();
			timer_t::delay_ms(10);
			pdn::set();
		}
	};
} // namespace lac
