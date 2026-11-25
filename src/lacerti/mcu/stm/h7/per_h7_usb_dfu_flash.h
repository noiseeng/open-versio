/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <mcu/stm/h7/per_h7_flash.h>
#include <mcu/stm/h7/per_h7_flash_program.h>
#include <mcu/stm/h750/adr_h750.h>
#include <mcu/stm/h750/usb_h750.h>
#include <usb/usb_dfu_handler.h>
#include <usb/usb_dfu_stage.h>

namespace lac
{
	enum class dfu_led
	{
		pulse,
		done,
		error
	};

	template <typename mcu_t>
	struct usb_dfu
	{
		using flash_t = lac::per_h7_flash<lac::adr_h750::FLASH_R, 1>;

		lac::usb_dfu_handler dfu   = {};
		lac::usb_dfu_stage	 stage = {};

		volatile bool detach_seen	 = false;
		volatile bool commit_pending = false;
		bool		  committed		 = false;
		bool		  failed		 = false;

		volatile uint32_t commit_base = 0;
		uint8_t* volatile commit_buf  = nullptr;
		volatile uint32_t commit_len  = 0;

		inline static usb_dfu* that = nullptr;

		static void on_detach_cb(void*)
		{
			if(that)
			{
				that->detach_seen = true;
			}
		}

		static bool on_commit_cb(void*, uint32_t base, uint8_t* buf, uint32_t len)
		{
			if(!that)
			{
				return false;
			}
			that->commit_base	 = base;
			that->commit_buf	 = buf;
			that->commit_len	 = len;
			that->commit_pending = true;
			return true;
		}

		template <typename usb_t>
		void attach(usb_t& usb)
		{
			that = this;

			uint8_t* buf = (uint8_t*)mcu_t::mem_t::alloc_cheap(lac::dfu_layout_h750::flash_size, "dfu stage");

			stage.init(buf,
					   lac::dfu_layout_h750::flash_size,
					   lac::dfu_layout_h750::flash_base,
					   lac::dfu_layout_h750::erase_granularity);
			stage.on_commit = on_commit_cb;

			dfu.init(stage.sink(),
					 lac::dfu_layout_h750::flash_base,
					 lac::dfu_layout_h750::flash_size,
					 lac::dfu_layout_h750::erase_granularity);
			dfu.callback_ctx = this;
			dfu.on_detach	 = on_detach_cb;

			usb.dfu = &dfu;
		}

		template <typename led_step_t>
		bool commit(led_step_t led_step)
		{
			const uint32_t base = commit_base;
			uint8_t* const buf	= commit_buf;
			const uint32_t len	= commit_len;

			if(len == 0 || len > stage.size)
			{
				return false;
			}

			flash_t::unlock();

			bool ok = flash_t::erase_sector();
			led_step();

			if(ok)
			{
				ok = flash_program_range<flash_t>(base, buf, len, [&](uint32_t w) {
					if((w & 0x3Fu) == 0u)
					{
						led_step();
					}
				});
			}

			flash_t::lock();

			return ok;
		}

		bool active() const { return detach_seen; }

		template <typename led_cb_t>
		void poll(led_cb_t leds)
		{
			if(commit_pending && !committed)
			{
				committed = true;

				if(commit([&]() { leds(dfu_led::pulse); }))
				{
					leds(dfu_led::done);
					dfu.set_op_done();
					mcu_t::tim2_t::delay_ms(50);
					mcu_t::reset();
				}
				else
				{
					dfu.set_op_failed();
					failed = true;
				}
			}

			if(failed)
			{
				leds(dfu_led::error);
			}
		}
	};
} // namespace lac
