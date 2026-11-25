/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <atomic>
#include <bug/eabi.h>
#include <cstdint>
#include <mcu/stm/h7/reg_h7_fmc.h>
#include <mcu/stm/stm32/gpio_list.h>
#include <utl/bitfield.h>
#include <utl/bitflag.h>
#include <utl/bug.h>

namespace lac
{
	template <typename... gpio_t>
	struct dev_asc4c16_pins
	{
		static_assert(sizeof...(gpio_t) == 57, "32 data, 13 address, 2 bank, 4 byte lane and 6 control pins");
		static_assert((gpio_t::is_alt() && ...), "sdram pins need an alternate function");

		static void init_pins() { gpio_list<gpio_t...>::init(); }
	};

	template <typename mcu_t, uint32_t port, uint32_t size, typename pins_t, typename timer_t>
	struct dev_asc4c16
	{
		using adr_t = mcu_t::adr_t;
		using mem_t = mcu_t::mem_t;

		using MODREG_BURST	   = bitfield<0, 2>;
		using MODREG_SEQENTIAL = bitflag<3>;
		using MODREG_CAS	   = bitfield<4, 2>;
		using MODREG_SINGLE	   = bitflag<9>;

		static const uint32_t PORT	  = port;
		static const uint32_t ADDRESS = 0x60000000;
		static const uint32_t SIZE	  = size;

		static uint32_t get_size() { return SIZE; }

		static void* get_address() { return (void*)ADDRESS; }

		static void test()
		{
			if(!bug::is_build_ship())
			{
				uint32_t  count = get_size() / sizeof(uint32_t);
				uint32_t* data	= (uint32_t*)get_address();

				for(int i = 0; i < count; i++)
				{
					data[i] = i;
				}

				for(int i = 0; i < count; i++)
				{
					lac_assert(data[i] == i);
				}
			}
		}

		static void init()
		{
			mcu_t::get_rcc()->enable_fmc();

			pins_t::init_pins();

			adr_t::get_fmc()->set_bmap(1);

			auto* fmc = adr_t::get_fmc();

			fmc->SDCR1 = fmc->sdram_control(9, 13, 32, 4, 3, 0, 2, false, true);
			lac_assert(fmc->SDCR1 == 0x19e9);
			fmc->SDTR1 = fmc->sdram_timing(3, 15, 15, 15, 4, 4, 4);
			lac_assert(fmc->SDTR1 == 0x333eee2);

			fmc->enable();

			const auto bank = reg_h7_fmc::BANK_1;

			fmc->send(reg_h7_fmc::CMD_CLOCK_ENABLE, bank, 1, 0);
			timer_t::delay_ms(100);
			fmc->send(reg_h7_fmc::CMD_PALL, bank, 1, 0);
			fmc->send(reg_h7_fmc::CMD_AUTO_REFRESH, bank, 1, 0);
			fmc->send(reg_h7_fmc::CMD_AUTO_REFRESH, bank, 1, 0);
			fmc->send(reg_h7_fmc::CMD_LOAD_MODE, bank, 1, MODREG_BURST::val(2) | MODREG_CAS::val(3) | MODREG_SINGLE::val());

			fmc->set_refresh_time(790);

			timer_t::delay_ms(10);

			volatile uint32_t* sdramr = (volatile uint32_t*)(ADDRESS);
			volatile uint32_t* sdramw = (volatile uint32_t*)(ADDRESS);

			for(uint32_t i = 0; i < 1024 * 64; i++)
			{
				sdramw[i] = i;
				lac_assert(sdramw[i] == i);
			}

			std::atomic_thread_fence(std::memory_order_seq_cst);

			for(uint32_t i = 0; i < 1024 * 64; i++)
			{
				if(sdramr[i] != i)
				{
					bug_print("no sdram\n");
					for(;;) {}
				}
			}
		}
	};
} // namespace lac
