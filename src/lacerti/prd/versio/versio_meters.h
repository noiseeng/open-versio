/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "versio_inp.h"
#include "versio_out.h"

namespace versio
{

	struct versio_meters
	{
		float meter_inpl = 0.0f;
		float meter_inpr = 0.0f;
		float meter_outl = 0.0f;
		float meter_outr = 0.0f;

		constexpr versio_meters() = default;

		constexpr versio_meters(const versio_inp& av)
		: meter_inpl(av.meter_inpl), meter_inpr(av.meter_inpr), meter_outl(av.meter_outl), meter_outr(av.meter_outr)
		{}
	};

	constexpr versio_out vu_out(const versio_meters& m)
	{ return {get_vu(m.meter_inpl), get_vu(m.meter_inpr), get_vu(m.meter_outl), get_vu(m.meter_outr)}; }

} // namespace versio
