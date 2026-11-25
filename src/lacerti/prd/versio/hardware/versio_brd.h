/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <bug/eabi.h>
#include <mcu/stm/h7/per_h7_adc.h>
#include <mcu/stm/h7/per_h7_usb_dfu_flash.h>
#include <mcu/stm/h7/reg_h7_dmamux.h>
#include <mcu/stm/h750/adr_h750.h>
#include <mcu/stm/stm32/per_adc.h>
#include <utl/bug.h>
#include <utl/slug.h>
#include <utl/timer_poll.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "versio_hui.h"
#include <dev/dev_led_rgb.h>
#include <dev/dev_led_rgb_pwm.h>
#include <hui/hui_tap_tempo.h>
#include <mcu/es/brd_es_dfm.h>
#include <mcu/es/brd_es_dfm_sdram.h>
#include <mcu/stm/h750/STM32H750IBKx.pin.h>
#include <prd/versio/versio_inp.h>
#include <prd/versio/versio_out.h>

#include <numerus/color.h>
#include <prd/versio/versio_rate.h>
#include <utl/audio_io.h>

#include "versio_usb.h"
#include <cstring>
#include <mcu/stm/h7/per_h7_usb_dfu.h>
#include <mcu/stm/h7/per_h7_usb_dfu_log.h>
#include <utl/timer_interval.h>

namespace versio
{

	template <typename dsp_type, uint32_t HARDWARE_FRAME_RATE>
	struct versio_brd
	{
		using this_t = versio_brd;
		using dfm_t	 = lac::brd_es_dfm<1, lac::brd_es_dfm_sdram>;
		using mcu_t	 = dfm_t::mcu_t;
		using tim_t	 = mcu_t::tim2_t;
		using pin_t	 = lac::pin_stm32h750ibkx;
		using dsp_t	 = dsp_type;

		constexpr static size_t FRAME_COUNT = dsp_t::FRAME_COUNT;

		using res_t	  = lac::audio_io;
		using frame_t = res_t::frame_t;
		using audio_t = dfm_t::template dfm_audio_t<HARDWARE_FRAME_RATE, 2 * 2 * FRAME_COUNT>;

		using jack_trig = pin_t::gpipu_a1;
		using tac_hit	= pin_t::gpipu_b15;
		using tog_sw1a	= pin_t::gpipu_c12;
		using tog_sw1b	= pin_t::gpipu_d2;
		using tog_sw2a	= pin_t::gpipu_c11;
		using tog_sw2b	= pin_t::gpipu_b12;

		using led1_t = lac::dev_led_rgb<pin_t::gpo_b5, pin_t::gpo_c9, pin_t::gpo_c8, true>;
		using led2_t = lac::dev_led_rgb<pin_t::gpo_b9, pin_t::gpo_b6, pin_t::gpo_b8, true>;
		using led3_t = lac::dev_led_rgb<pin_t::gpo_a0, pin_t::gpo_d11, pin_t::gpo_b7, true>;
		using led4_t = lac::dev_led_rgb<pin_t::gpo_b14, pin_t::gpo_g9, pin_t::gpo_c0, true>;

		using rgb1_t = lac::dev_led_rgb_pwm<led1_t, 6>;
		using rgb2_t = lac::dev_led_rgb_pwm<led2_t, 6>;
		using rgb3_t = lac::dev_led_rgb_pwm<led3_t, 6>;
		using rgb4_t = lac::dev_led_rgb_pwm<led4_t, 6>;

		enum
		{
			ADC_KNOB0,
			ADC_KNOB1,
			ADC_KNOB2,
			ADC_KNOB3,
			ADC_KNOB4,
			ADC_KNOB5,
			ADC_KNOB6
		};

		struct adc_pins
		{
			static constexpr std::array<lac::adc_pin_config, 7> pins = {{
				ADC_PIN(pin_t::adc1p_c4),
				ADC_PIN(pin_t::adc1p_a5),
				ADC_PIN(pin_t::adc1p_a2),
				ADC_PIN(pin_t::adc1p_a4),
				ADC_PIN(pin_t::adc1p_a3),
				ADC_PIN(pin_t::adc1p_b1),
				ADC_PIN(pin_t::adc1p_a6),
			}};
		};

		using adc_t	  = lac::per_h7_adc<mcu_t,
										lac::adr_h750::ADC1,
										adc_pins,
										lac::DMA1_STREAM_2,
										lac::ADC_TIME_16_5,
										3,
										16,
										4,
										0,
										mcu_t::tim2_t>;
		using timer_t = lac::timer_interval<mcu_t::tim2_t>;
		using hui_t	  = versio::versio_hui<mcu_t::tim2_t>;
		using dbg_t	  = lac::timer_poll<mcu_t::tim2_t, false>;
		using slug_t  = lac::slug;

		dfm_t dfm;

		audio_t audio = {*mcu_t::alloc_dma<typename audio_t::dma_buffer>()};
		adc_t	adc	  = {*mcu_t::alloc_dma<typename adc_t::values>()};

		mcu_t::mcu_alloc_t alloc;

		jack_trig trig;
		tac_hit	  hit;
		tog_sw1a  sw1a;
		tog_sw1b  sw1b;
		tog_sw2a  sw2a;
		tog_sw2b  sw2b;
		timer_t	  cpu{timer_t::budget_of(FRAME_COUNT, HARDWARE_FRAME_RATE)};
		dbg_t	  wait_debug = {mcu_t::tim2_t::ms_to_clk(1000)};
		slug_t	  slug;
		hui_t	  hui;
		dsp_t	  dsp;
		rgb1_t	  rgb1;
		rgb2_t	  rgb2;
		rgb3_t	  rgb3;
		rgb4_t	  rgb4;

		constexpr static uint32_t frame_rate  = HARDWARE_FRAME_RATE;
		constexpr static float	  METER_DECAY = versio::versio_meter_decay(frame_rate);

		static_assert(hui_t::HOLD_TIMEOUT == (int32_t)tim_t::ms_to_clk(2000), "tap tempo clears on the first 2 s hold count");
		constexpr static int32_t TAP_CLEAR_HOLDS = 2;

		lac::hui_tap_tempo<tim_t::get_hz()> tap_tempo;

		bool		  last_hit_state	= false;
		volatile bool tap_clear_pending = false;
		bool		  toggle_state		= false;

		versio::versio_inp ctl;
		versio::versio_out out;

		bool test_mode = false;

		inline static this_t* that = 0;

		std::conditional_t<lac::use_log(),
						   lac::per_h7_dfu_log<mcu_t, u"Versio Hot Swap", lac::usbd_dfu_log_h750_versio>,
						   lac::per_h7_dfu<mcu_t, u"Versio Hot Swap", true, lac::usbd_dfu_h750_versio>>
				dfu		   = {};
		int32_t tempo_held = 0;
		bool	jack_prev  = false;
		bool	btn_prev   = false;

		versio_brd()
		{
			lac_assert(!that);
			that = this;

			init_usb();

			dsp.set_frame_rate((float)frame_rate, &alloc);

			adc.init();
			adc.start();

			mcu_t::tim2_t::delay_ms(100);

			test_mode = !hit.get();

			audio.init(test_mode ? testcb : audiocb);
		}

		void init_usb()
		{
			dfu.init();

			if constexpr(lac::use_log())
			{
				mcu_t::post_mortem = post_mortem_thunk;
			}
		}

		[[noreturn]] void post_mortem()
		{
			audio.mute();
			set_all_leds(numerus::color::magenta());

			for(;;)
			{
				poll_dfu();
				dfu.poll_log();
			}
		}

		static void post_mortem_thunk() { that->post_mortem(); }

		static void audiocb(uint32_t count, const int32_t* in, int32_t* out)
		{
			if(that->dfu.active())
			{
				std::memset(out, 0, sizeof(int32_t) * count);
				return;
			}

			that->frame(count / 2, in, out);
		}

		static void testcb(uint32_t count, const int32_t* in, int32_t* out) { that->test(count / 2, in, out); }

		void step_led()
		{
			rgb1();
			rgb2();
			rgb3();
			rgb4();
		}

		template <typename hui_t>
		void step_ctl(hui_t& hui)
		{
			uint32_t bits = 0;
			bits |= (trig.get() ? (1 << versio::BIT_JACK) : 0);
			bits |= (hit.get() ? (1 << versio::BIT_TAC) : 0);
			bits |= (sw1a.get() ? (1 << versio::BIT_SW1A) : 0);
			bits |= (sw1b.get() ? (1 << versio::BIT_SW1B) : 0);
			bits |= (sw2a.get() ? (1 << versio::BIT_SW2A) : 0);
			bits |= (sw2b.get() ? (1 << versio::BIT_SW2B) : 0);
			hui(bits,
				{
					adc.get_median(ADC_KNOB0),
					adc.get_median(ADC_KNOB1),
					adc.get_median(ADC_KNOB2),
					adc.get_median(ADC_KNOB3),
					adc.get_median(ADC_KNOB4),
					adc.get_median(ADC_KNOB5),
					adc.get_median(ADC_KNOB6),
				});

			const bool tac_trig = hui.tac[0].pressed();
			const bool jac_trig = hui.tac[1].pressed();

			tap_tempo.tick(hui.tac[0], hui.tac[1]);

			if(hui.tac[0].held_reached(TAP_CLEAR_HOLDS))
			{
				tap_clear_pending = true;
			}

			const bool g_jack = (bool)hui.tac[1];
			const bool g_btn  = (bool)hui.tac[0];
			const bool e_jack = g_jack && !jack_prev;
			const bool e_btn  = g_btn && !btn_prev;
			jack_prev		  = g_jack;
			btn_prev		  = g_btn;

			const bool tempo_cleared = tap_tempo.get_cleared();

			const int32_t tempo_db = 3 * (int32_t)frame_rate / 1000;
			const int32_t tap	   = tap_tempo.get_period(frame_rate);
			const int32_t change   = tap > tempo_held ? tap - tempo_held : tempo_held - tap;
			if(tempo_held == 0 || tap == 0 || change > tempo_db)
			{
				tempo_held = tap;
			}

			ctl = versio::versio_inp{
				hui.adcf[ADC_KNOB0].value,
				hui.adcf[ADC_KNOB1].value,
				hui.adcf[ADC_KNOB2].value,
				hui.adcf[ADC_KNOB3].value,
				hui.adcf[ADC_KNOB4].value,
				hui.adcf[ADC_KNOB5].value,
				hui.adcf[ADC_KNOB6].value,
				hui.tog3[0].value,
				hui.tog3[1].value,
				e_btn,
				g_btn,
				e_jack,
				g_jack,
				tempo_held,
				tempo_cleared,
				last_hit_state,
				tap_clear_pending,
				meter_inp[0],
				meter_inp[1],
				meter_out[0],
				meter_out[1],
			};

			last_hit_state = tac_trig || jac_trig;
		}

		frame_t				   meter_inp = {};
		frame_t				   meter_out = {};
		float				   fade		 = 0.0f;
		float				   fade_dt	 = 0.25f / (float)frame_rate;
		static constexpr float boot_of(float f) { return f * f < 0.5f ? 1.0f : 2.0f - 2.0f * f * f; }
		static constexpr float gain_of(float f) { return f < 0.5f ? 0.0f : 2.0f * f - 1.0f; }

		std::array<frame_t, FRAME_COUNT> buffer_s;
		float							 buffer_lr[FRAME_COUNT * 2];

		float* get_l() { return buffer_lr; }
		float* get_r() { return buffer_lr + FRAME_COUNT; }

		void deinterleave()
		{
			float* l = get_l();
			float* r = get_r();
			for(uint32_t i = 0; i < FRAME_COUNT; i++)
			{
				l[i] = buffer_s[i][0];
				r[i] = buffer_s[i][1];
			}
		}

		void interleave()
		{
			float* l = get_l();
			float* r = get_r();
			for(uint32_t i = 0; i < FRAME_COUNT; i++)
			{
				buffer_s[i] = {l[i], r[i]};
			}
		}

		void test(uint32_t frame_count, const int32_t* audio_in, int32_t* audio_out)
		{
			lac_assert(frame_count == FRAME_COUNT);

			step_led();

			cpu.start();

			step_ctl(hui);

			step_led();

			numerus::color c1
				= numerus::color::red() * ctl.k1 + numerus::color::green() * ctl.k3 + numerus::color::blue() * ctl.k5;

			numerus::color c2
				= numerus::color::green() * ctl.k2 + numerus::color::blue() * ctl.k4 + numerus::color::red() * ctl.k6;

			numerus::color c3 = numerus::color::red() * ctl.k7;
			if(ctl.jack_gate)
			{
				c3 = c3 + numerus::color::blue();
			}
			if(ctl.btn_gate)
			{
				c3 = c3 + numerus::color::green();
			}

			numerus::color c4 = numerus::color::black();
			if(ctl.sw1 == 1)
			{
				c4 = c4 + numerus::color::white();
			}
			else if(ctl.sw1 == 2)
			{
				c4 = c4 + numerus::color::red();
			}
			if(ctl.sw2 == 1)
			{
				c4 = c4 + numerus::color::green();
			}
			else if(ctl.sw2 == 2)
			{
				c4 = c4 + numerus::color::cyan();
			}

			step_led();

			for(uint32_t i = 0; i < FRAME_COUNT; i++)
			{
				fade = fade + fade_dt < 1.0f ? fade + fade_dt : 1.0f;
			}

			const float boot = boot_of(fade);

			rgb1 = numerus::color::lerp(boot, c1, dsp_t::color0);
			rgb2 = numerus::color::lerp(boot, c2, dsp_t::color1);
			rgb3 = numerus::color::lerp(boot, c3, dsp_t::color2);
			rgb4 = numerus::color::lerp(boot, c4, dsp_t::color3);

			step_led();

			res_t::convert(FRAME_COUNT, audio_in, audio_out, [](frame_t inp) { return inp; });

			if(dfm.is_dfm)
			{
				invert(audio_out);
			}

			step_led();

			cpu.end();
		}

		void frame(uint32_t frame_count, const int32_t* audio_in, int32_t* audio_out)
		{
			lac_assert(frame_count == audio_t::CB_WORD_COUNT / 2);

			step_led();

			cpu.start();

			step_ctl(hui);

			step_led();

			out = dsp.control(ctl);

			step_led();

			if constexpr(dsp_t::BLOCK)
			{
				res_t::convert(FRAME_COUNT, audio_in, buffer_s.data());
				deinterleave();
				step_led();

				float* l = get_l();
				float* r = get_r();
				float  f = fade;
				for(uint32_t i = 0; i < FRAME_COUNT; i++)
				{
					meter_inp = versio::versio_meter_max(meter_inp, buffer_s[i], METER_DECAY);
					l[i] *= f;
					r[i] *= f;
					f = (f + fade_dt < 1.0f) ? f + fade_dt : 1.0f;
				}
				fade = f;

				dsp.block(ctl, l, r);

				step_led();
				interleave();

				for(uint32_t i = 0; i < FRAME_COUNT; i++)
				{
					meter_out = versio::versio_meter_max(meter_out, buffer_s[i], METER_DECAY);
				}

				res_t::convert(FRAME_COUNT, buffer_s.data(), audio_out);
			}
			else
			{
				res_t::convert(FRAME_COUNT, audio_in, audio_out, [&](frame_t inp) -> frame_t {
					meter_inp		= versio::versio_meter_max(meter_inp, inp, METER_DECAY);
					const frame_t y = dsp(inp);
					meter_out		= versio::versio_meter_max(meter_out, y, METER_DECAY);
					fade			= fade + fade_dt < 1.0f ? fade + fade_dt : 1.0f;
					const float g	= gain_of(fade);
					return {y[0] * g, y[1] * g};
				});
			}

			step_led();

			const float boot = boot_of(fade);

			rgb1 = numerus::color::lerp(boot, out.rgb1, dsp_t::color0);
			rgb2 = numerus::color::lerp(boot, out.rgb2, dsp_t::color1);
			rgb3 = numerus::color::lerp(boot, out.rgb3, dsp_t::color2);
			rgb4 = numerus::color::lerp(boot, out.rgb4, dsp_t::color3);

			dsp.finish();

			if(dfm.is_dfm)
			{
				invert(audio_out);
			}

			step_led();

			cpu.end();
		}

		static constexpr void invert(int32_t* v)
		{
			for(uint32_t i = 0; i < 2 * FRAME_COUNT; i++)
			{
				v[i] = -v[i];
			}
		}

		void set_all_leds(numerus::color c)
		{
			rgb1 = c;
			rgb2 = c;
			rgb3 = c;
			rgb4 = c;
			step_led();
		}

		struct no_callback
		{
			constexpr void operator()() const {}
		};

		void poll_dfu()
		{
			dfu.poll_leds([this](lac::dfu_led s, bool bright) {
				switch(s)
				{
					case lac::dfu_led::pulse: set_all_leds(numerus::color::blue() * (bright ? 1.0f : 0.2f)); break;
					case lac::dfu_led::done: set_all_leds(numerus::color::green()); break;
					case lac::dfu_led::error: set_all_leds(numerus::color::red()); break;
				}
			});
		}

		template <typename cb_t = no_callback>
		void main(cb_t cb = {})
		{
			while(true)
			{
				wait_debug([&]() {
					dsp.print();
					bug_print("cpu %d%% max %d%% overruns %d\n", cpu.percent, cpu.get_max(), cpu.overruns);
				});

				slug([&]() {
					if(tap_clear_pending)
					{
						dsp.clear_memory();
						dsp.clear_memory();
						tap_clear_pending = false;
					}
					cb();
				});

				poll_dfu();
				dfu.poll_log();
			}
		}
	}; // namespace lac

	template <typename dsp_t, uint32_t HARDWARE_FRAME_RATE>
	versio_brd<dsp_t, HARDWARE_FRAME_RATE> versio_board;

	struct versio_hook_none
	{
		template <typename brd_t>
		static void init(brd_t&)
		{}
		template <typename brd_t>
		static void idle(brd_t&)
		{}
	};

	template <typename dsp_t, uint32_t HARDWARE_FRAME_RATE, typename hook_t = versio_hook_none>
	int versio_main()
	{
		auto& av = versio_board<dsp_t, HARDWARE_FRAME_RATE>;

		hook_t::init(av);

		av.main([&]() { hook_t::idle(av); });

		return 0;
	}

} // namespace versio