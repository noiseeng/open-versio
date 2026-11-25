/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

#include "per_h7_dma.h"
#include "reg_h7_adc.h"
#include "reg_h7_dma.h"
#include "reg_h7_dmamux.h"
#include <utl/bug.h>

namespace lac
{
	enum
	{
		ADC_TIME_1_5 = 0,
		ADC_TIME_2_5,
		ADC_TIME_8_5,
		ADC_TIME_16_5,
		ADC_TIME_32_5,
		ADC_TIME_64_5,
		ADC_TIME_387_5,
		ADC_TIME_810_5,
	};

	template <typename mcu_t,
			  uint32_t port,
			  typename pin_config_type,
			  dma_stream_id dma_stream,
			  uint32_t		sample_time,
			  uint32_t		oversample,
			  uint32_t		bit_resolution,
			  uint32_t		ave_count,
			  int32_t		ave_shift,
			  class timer_t>
	struct per_h7_adc
	{
		using isv_t = mcu_t::isv_t;
		using irq_t = mcu_t::irq_t;

		constexpr static reg_h7_adc_common *get_common() { return (reg_h7_adc_common *)mcu_t::get_adc_common_addr(port); }
		constexpr static reg_h7_adc		   *get() { return (reg_h7_adc *)port; }

		using this_t = per_h7_adc<mcu_t,
								  port,
								  pin_config_type,
								  dma_stream,
								  sample_time,
								  oversample,
								  bit_resolution,
								  ave_count,
								  ave_shift,
								  timer_t>;

		using dma_t = per_h7_dma_stream<mcu_t, dma_stream, mcu_t::get_adc_dmamux_req(port), sizeof(uint32_t), DMA_PER2MEM>;

		static const uint32_t CHANNEL_COUNT	   = pin_config_type::pins.size();
		static const uint32_t OVERSAMPLE_COUNT = oversample;
		static const uint32_t SAMPLE_COUNT	   = CHANNEL_COUNT * OVERSAMPLE_COUNT;
		static const uint32_t SAMPLE_BYTES	   = SAMPLE_COUNT * sizeof(uint32_t);
		static const uint32_t MAX_VALUE		   = 0x10000 * OVERSAMPLE_COUNT;
		static const uint32_t PRECISION		   = ave_count + bit_resolution;

		struct values
		{
			constexpr static size_t COUNT = pin_config_type::pins.size() * oversample;

			std::array<volatile uint32_t, COUNT> samples = {};

			uint32_t operator[](size_t idx) const { return samples[idx]; }

			volatile uint32_t &operator[](size_t idx) { return samples[idx]; }

			constexpr values()
			{
				for(auto &v : samples)
				{
					v = 0;
				}
			}

			auto data() { return samples.data(); }

			constexpr uint32_t get(uint32_t idx) const
			{
				lac_assert(OVERSAMPLE_COUNT == 1);

				return samples[idx];
			}

			constexpr uint32_t get_median(uint32_t idx) const
			{
				lac_assert(OVERSAMPLE_COUNT == 3);

				const uint32_t a = samples[idx + 0 * CHANNEL_COUNT];
				const uint32_t b = samples[idx + 1 * CHANNEL_COUNT];
				const uint32_t c = samples[idx + 2 * CHANNEL_COUNT];
				return (b < a) ? (b < c) ? (c < a) ? c : a : b : (a < c) ? (c < b) ? c : b : a;
			}

			constexpr int32_t get_median_fix24(uint32_t idx) const { return get_median(idx) << (24 - get_precision()); }

			constexpr float get_median_float(uint32_t idx) { return (float)get_median_fix24(idx) * (1.0f / (float)(1 << 24)); }

			constexpr float get_float(uint32_t idx) const
			{
				const static float scale = 1.0f / (MAX_VALUE - OVERSAMPLE_COUNT);
				const uint32_t	   x	 = get_median(idx);
				return scale * (float)x;
			}
		};

		using adc_callback = void (*)(uint32_t count, values &values);

		values		&raw_values;
		adc_callback callback = 0;

		constexpr static uint32_t get_precision() { return PRECISION - ave_shift; }

		constexpr uint32_t get_median(uint32_t idx) const { return raw_values.get_median(idx); }

		per_h7_adc(values &v) : raw_values(v) { init(); }

		void init()
		{
			if(singleton(this))
			{
				return;
			}

			auto *adc = get();
			auto *com = get_common();

			mcu_t::template enable_clock<port>();

			adc->power_up();
			timer_t::delay_us(100);
			adc->set_boost();
			com->set_clockdiv(4);
			adc->continuous_dma(CHANNEL_COUNT);
			adc->set_resolution(bit_resolution);
			adc->set_oversample((1 << ave_count) - 1, ave_shift);
			adc->template calibrate<timer_t>();

			for(uint32_t i = 0; i < CHANNEL_COUNT; i++)
			{
				pin_config_type::pins[i].init();
				adc->channel(pin_config_type::pins[i].adc, i, sample_time);
			}

			dma_t::init(true);
		}

		void start()
		{
			auto *adc = get();
			adc->enable();
			dma_t::start(&adc->DR, (void *)raw_values.data(), SAMPLE_COUNT, false);
			adc->start();
		}

		void eosie(adc_callback cb)
		{
			if(cb)
			{
				callback = cb;
				std::atomic_thread_fence(std::memory_order_seq_cst);
				mcu_t::set_isv(mcu_t::get_adc_isrv(port), adc_isr);
				mcu_t::get_nvic()->enable_irq(mcu_t::get_adc_irq(port), 0);
				get()->set_eosie();
			}
			else
			{
				get()->clr_eosie();
				mcu_t::get_nvic()->disable_irq(mcu_t::get_adc_irq(port));
				mcu_t::get_nvic()->clear_irq(mcu_t::get_adc_irq(port));
				std::atomic_thread_fence(std::memory_order_seq_cst);
				callback = 0;
			}
		}

		static void adc_isr()
		{
			this_t *self = singleton();
			get()->clear_eos();
			self->callback(SAMPLE_COUNT, self->raw_values);
		}

		static this_t *singleton(this_t *s = 0)
		{
			static this_t *instance = 0;

			if(instance)
			{
				return instance;
			}
			else if(s)
			{
				instance = s;
				return 0;
			}
			else
			{
				lac_assert(instance);
				return instance;
			}
		}

		void set_oversample(uint32_t ovr, uint32_t shift) { get()->set_oversample(ovr, shift); }

		void set_resolution(uint32_t res) { get()->set_resolution(res); }
	};
}; // namespace lac
