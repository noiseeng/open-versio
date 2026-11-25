/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <atomic>
#include <cstdint>

#include <mcu/stm/stm32/reg_tim.h>

namespace lac
{
	template <typename mcu_t, uint32_t port, uint32_t timer_hz>
	struct per_h7_tim
	{
		constexpr static uint32_t TIMER_HERTZ = timer_hz;
		constexpr static uint32_t MS_SCALE	  = TIMER_HERTZ / 1000;
		constexpr static uint32_t US_SCALE	  = TIMER_HERTZ / 1000000;
		constexpr static uint32_t PORT		  = port;

		constexpr static uint32_t get_hz() { return TIMER_HERTZ; }

		constexpr static uint32_t clk_to_us(uint32_t clk) { return clk / US_SCALE; }

		constexpr static uint32_t ms_to_clk(uint32_t ms) { return ms * MS_SCALE; }

		constexpr static uint32_t us_to_clk(uint32_t us) { return us * US_SCALE; }

		static reg_tim* reg() { return (reg_tim*)port; }

		static uint32_t read() { return reg()->read(); }

		static void delay_ck(uint32_t tics)
		{
			uint32_t start = read();
			while(read() - start < tics) {}
			std::atomic_signal_fence(std::memory_order_seq_cst);
		}

		static void delay_ms(uint32_t ms)
		{
			if(ms)
			{
				delay_ck(ms_to_clk(ms));
			}
		}

		static void delay_us(uint32_t us) { delay_ck(us_to_clk(us)); }

		static void freerun()
		{
			mcu_t::template enable_clock<port>();
			reg()->freerun();
		}

		static uint32_t get_clk() { return read(); }

		static void enable_irq(void (*cb)(), uint32_t priority = 0)
		{
			constexpr auto irq	= mcu_t::get_tim_irq_up(port);
			constexpr auto isrv = irq2isv(irq);

			reg()->set_urs();

			mcu_t::set_isv(isrv, cb);
			mcu_t::get_nvic()->clear_irq(irq);
			mcu_t::get_nvic()->enable_irq(irq, priority);

			reg()->enable_irq();
		}

		static void disable_irq()
		{
			mcu_t::get_nvic()->disable_irq(mcu_t::get_tim_irq_up(port));
			reg()->disable_irq();
		}

		static void clear_irq()
		{
			mcu_t::get_nvic()->clear_irq(mcu_t::get_tim_irq_up(port));
			reg()->clear_uif();
		}

		static void enable_delay_update() { reg()->enable_delay_update(); }

		static void set_fastest() { reg()->set_fastest(); }

		static void set_down() { reg()->set_down(); }

		static void set_next_delay(uint32_t delay) { reg()->set_next_delay(delay); }

		static void set_delay_hz(uint32_t hz) { set_next_delay(TIMER_HERTZ / hz); }

		static bool check_time()
		{
			uint32_t real_hz = mcu_t::template get_tim_hz<port>();

			bool match = TIMER_HERTZ == real_hz;

			if(!match)
			{
				mcu_t::template get_tim_hz<port>();
			}

			return match;
		}

		static void init()
		{
			mcu_t::template enable_clock<port>();

			reg()->CR1	 = 0;
			reg()->CCER	 = 0;
			reg()->CCMR1 = 0;
			reg()->CCMR2 = 0;
			reg()->ARR	 = 0xFFFFFFFF;
			reg()->PSC	 = 0;
			reg()->SMCR	 = 0;
			reg()->CR2	 = 0;
			reg()->CNT	 = 0;
		}

		static void enable() { reg()->enable(); }

		static void disable() { reg()->disable(); }

		constexpr static bool is_valid() { return true; }

		static void set_trgo_update() { reg()->set_trgo_update(); }

		static uint32_t get_period_hz() { return TIMER_HERTZ / reg()->get_delay(); }

		static void set_period_hz(uint32_t hz)
		{
			constexpr uint64_t max_arr = (((uint64_t)1) << mcu_t::template get_tim_width<port>()) - 1;

			uint64_t total = (uint64_t)TIMER_HERTZ / hz;
			uint32_t psc   = 0;

			while(total / (psc + 1) > max_arr + 1)
			{
				psc++;
			}

			reg()->PSC = psc;
			reg()->ARR = (uint32_t)(total / (psc + 1)) - 1;
		}

		static uint32_t get_achieved_hz()
		{
			uint64_t psc = reg()->PSC;
			uint64_t arr = reg()->ARR;

			return (uint32_t)((uint64_t)TIMER_HERTZ / ((psc + 1) * (arr + 1)));
		}

		constexpr static uint32_t PWM_MODE1 = 0b0110;

		template <uint32_t channel>
		static void setup_pwm()
		{
			static_assert(channel >= 1 && channel <= mcu_t::template get_tim_channel_count<port>(),
						  "channel exceeds this timer's channel count");

			auto* r = reg();

			if constexpr(channel == 1)
			{
				reg_tim::CCMR1_CC1S::set(r->CCMR1, 0);
				reg_tim::CCMR1_OC1M::set(r->CCMR1, PWM_MODE1);
				reg_tim::CCMR1_OC1M3::clr(r->CCMR1);
				reg_tim::CCMR1_OC1PE::set(r->CCMR1);
				reg_tim::CCER_CC1E::set(r->CCER);
			}
			else if constexpr(channel == 2)
			{
				reg_tim::CCMR1_CC2S::set(r->CCMR1, 0);
				reg_tim::CCMR1_OC2M::set(r->CCMR1, PWM_MODE1);
				reg_tim::CCMR1_OC2M3::clr(r->CCMR1);
				reg_tim::CCMR1_OC2PE::set(r->CCMR1);
				reg_tim::CCER_CC2E::set(r->CCER);
			}
			else if constexpr(channel == 3)
			{
				reg_tim::CCMR2_CC3S::set(r->CCMR2, 0);
				reg_tim::CCMR2_OC3M::set(r->CCMR2, PWM_MODE1);
				reg_tim::CCMR2_OC3M3::clr(r->CCMR2);
				reg_tim::CCMR2_OC3PE::set(r->CCMR2);
				reg_tim::CCER_CC3E::set(r->CCER);
			}
			else if constexpr(channel == 4)
			{
				reg_tim::CCMR2_CC4S::set(r->CCMR2, 0);
				reg_tim::CCMR2_OC4M::set(r->CCMR2, PWM_MODE1);
				reg_tim::CCMR2_OC4M3::clr(r->CCMR2);
				reg_tim::CCMR2_OC4PE::set(r->CCMR2);
				reg_tim::CCER_CC4E::set(r->CCER);
			}

			reg_tim::CR1_ARPE::set(r->CR1);

			if constexpr(mcu_t::template get_tim_has_moe<port>())
			{
				reg_tim::BDTR_MOE::set(r->BDTR);
			}

			reg_tim::EGR_UG::set(r->EGR);
		}

		template <uint32_t channel>
		static void set_duty(uint32_t ccr)
		{
			static_assert(channel >= 1 && channel <= mcu_t::template get_tim_channel_count<port>(),
						  "channel exceeds this timer's channel count");

			if constexpr(channel == 1)
			{
				reg()->CCR1 = ccr;
			}
			else if constexpr(channel == 2)
			{
				reg()->CCR2 = ccr;
			}
			else if constexpr(channel == 3)
			{
				reg()->CCR3 = ccr;
			}
			else if constexpr(channel == 4)
			{
				reg()->CCR4 = ccr;
			}
		}

		template <class cb_t>
		static void delay_ms_poll(uint32_t ms, cb_t cb)
		{
			int32_t clk	 = ms_to_clk(ms);
			int32_t time = get_clk();

			while(((int32_t)get_clk() - time) < clk)
			{
				if(!cb())
				{
					return;
				}
			}
		}

		template <class cb_t>
		static void delay_us_poll(uint32_t us, cb_t cb)
		{
			int32_t clk	 = us_to_clk(us);
			int32_t time = get_clk();

			while(((int32_t)get_clk() - time) < clk)
			{
				if(!cb())
				{
					return;
				}
			}
		}
	};

	struct per_h7_tim_null
	{
		constexpr static bool is_valid() { return false; }
		constexpr static void set_delay_hz(uint32_t x) {}
		constexpr static void set_down() {}
		constexpr static void set_fastest() {}
		constexpr static void enable() {}
		constexpr static void init() {}
		constexpr static void set_trgo_update() {}

		const static uint32_t PORT = 0;
	};

} // namespace lac