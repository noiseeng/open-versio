/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include <hui/hui_adcf.h>
#include <hui/hui_const.h>
#include <hui/hui_tac.h>
#include <hui/hui_tog3.h>

namespace versio
{

	enum
	{
		BIT_JACK,
		BIT_TAC,
		BIT_SW1A,
		BIT_SW1B,
		BIT_SW2A,
		BIT_SW2B
	};

	template <typename tim2_t>
	struct versio_hui
	{
		using timer_t = tim2_t;

		constexpr static size_t ADC_BITS = 20;
		constexpr static bool	ADC_FLIP = true;
		constexpr static float	ADC_MIN	 = 0.01f;
		constexpr static float	ADC_MAX	 = 0.99f;
		constexpr static float	ADC_HIST = 1.0f / 512.0f;
		constexpr static float	ADC_FILT = 0.5f;

		constexpr static uint32_t						 ADCF_COUNT = 7;
		constexpr static lac::hui_array_adcf<ADCF_COUNT> ADCF		= {{
			{ADC_BITS, ADC_FLIP, ADC_MIN, ADC_MAX, ADC_HIST, ADC_FILT},
			{ADC_BITS, ADC_FLIP, ADC_MIN, ADC_MAX, ADC_HIST, ADC_FILT},
			{ADC_BITS, ADC_FLIP, ADC_MIN, ADC_MAX, ADC_HIST, ADC_FILT},
			{ADC_BITS, ADC_FLIP, ADC_MIN, ADC_MAX, ADC_HIST, ADC_FILT},
			{ADC_BITS, ADC_FLIP, ADC_MIN, ADC_MAX, ADC_HIST, ADC_FILT},
			{ADC_BITS, ADC_FLIP, ADC_MIN, ADC_MAX, ADC_HIST, ADC_FILT},
			{ADC_BITS, ADC_FLIP, ADC_MIN, ADC_MAX, ADC_HIST, ADC_FILT},
		}};

		constexpr static uint32_t					   TAC_COUNT = 2;
		constexpr static lac::hui_array_tac<TAC_COUNT> TAC		 = {{
			{BIT_TAC, lac::HUI_FLIP},
			{BIT_JACK},
		}};

		constexpr static uint32_t						 TOG3_COUNT = 2;
		constexpr static lac::hui_array_tog3<TOG3_COUNT> TOG3		= {{
			{BIT_SW1A, BIT_SW1B},
			{BIT_SW2A, BIT_SW2B},
		}};

		constexpr static int32_t DEBOUNCE_TIME = timer_t::ms_to_clk(2);
		constexpr static int32_t TAP_TIMEOUT   = timer_t::ms_to_clk(4000);
		constexpr static int32_t HOLD_TIMEOUT  = timer_t::ms_to_clk(2000);

		uint32_t bits_this = 0;
		uint32_t bits_that = 0;

		std::array<lac::hui_filter_tog3, TOG3_COUNT> tog3_filter = {};
		std::array<lac::hui_filter_tac, TAC_COUNT>	 tac_filter	 = {};
		std::array<lac::hui_filter_adcf, ADCF_COUNT> adcf_filter = {};

		std::array<lac::hui_value_tog3, TOG3_COUNT> tog3 = {};
		std::array<lac::hui_value_tac, TAC_COUNT>	tac	 = {};
		std::array<lac::hui_value_adcf, ADCF_COUNT> adcf = {};

		constexpr bool get(int8_t bit) const { return (bits_this >> bit) & 1; }
		constexpr bool get_last(int8_t bit) const { return (bits_that >> bit) & 1; }

		void operator()(uint32_t bits, const std::array<uint32_t, ADCF_COUNT>& adc)
		{
			bits_that = bits_this;
			bits_this = bits;

			for(uint32_t i = 0; i < TOG3_COUNT; i++)
			{
				tog3[i] = tog3_filter[i](get(TOG3[i].bita), get(TOG3[i].bitb), get_last(TOG3[i].bita), get_last(TOG3[i].bitb));
			}

			for(uint32_t i = 0; i < TAC_COUNT; i++)
			{
				tac[i] = tac_filter[i](
					timer_t::get_clk(), DEBOUNCE_TIME, TAP_TIMEOUT, HOLD_TIMEOUT, lac::flip_bool(TAC[i].flip, get(TAC[i].bit)));
			}

			for(uint32_t i = 0; i < ADCF_COUNT; i++)
			{
				adcf[i] = adcf_filter[i](ADCF[i], adc[i]);
			}
		}
	};

} // namespace versio