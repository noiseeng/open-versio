/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <bug/eabi.h>
#include <cstdint>
#include <dev/dev_ak4554.h>
#include <dev/dev_qspi_is25lp064.h>
#include <mcu/stm/h7/per_h7_gpio.h>
#include <mcu/stm/h7/per_h7_qspi.h>
#include <mcu/stm/h7/per_h7_sai.h>
#include <mcu/stm/h7/reg_h7_dmamux.h>
#include <mcu/stm/h750/adr_h750.h>
#include <mcu/stm/h750/mcu_h750.h>
#include <mcu/stm/h750/STM32H750IBKx.pin.h>
#include <mcu/stm/stm32/per_gpio.h>
#include <type_traits>
#include <utl/bug.h>

namespace lac
{
	template <uint32_t mcu_div, template <typename> typename ext_ram = mcu_h750_no_ext_ram>
	struct brd_es_dfm
	{
		using mcu_t	 = mcu_h750<mcu_div>;
		using rcc_t	 = typename mcu_t::rcc_t;
		using tim2_t = typename mcu_t::tim2_t;

		using gpio_null = lac::gpio_null;
		using pin_t		= pin_stm32h750ibkx;

		using test_point_t = pin_t::gpio_g14;
		using led_seed_t   = pin_t::gpio_c7;
		using dfmid_t	   = pin_t::gpio_d4;
		using eddy_t	   = pin_t::gpio_d5;

		using codec_pdn = pin_t::gpio_b11;
		using sai1_mclk = pin_t::sai1_mclk_a_e2;
		using sai1_sdo	= pin_t::sai1_sd_b_e3;
		using sai1_fs	= pin_t::sai1_fs_a_e4;
		using sai1_sck	= pin_t::sai1_sck_a_e5;
		using sai1_sdi	= pin_t::sai1_sd_a_e6;

		using codec_t = dev_ak4554<codec_pdn, tim2_t>;

		using sai1_pins_t
			= per_h7_sai_pins<sai1_mclk, sai1_sck, sai1_fs, SAI_DIR_TX, sai1_sdo, SAI_DIR_RX, sai1_sdi, SAI_SLAVE_B, false>;

		using sai1_48k_t = per_h7_sai<mcu_t, adr_h750::SAI1, 4, sizeof(int32_t), sai1_pins_t>;
		using sai1_96k_t = per_h7_sai<mcu_t, adr_h750::SAI1, 2, sizeof(int32_t), sai1_pins_t>;
		using sai1_t	 = sai1_96k_t;

		template <uint32_t frame_rate>
		static constexpr auto sai1_at_rate()
		{
			static_assert(frame_rate == 48000 || frame_rate == 96000);
			if constexpr(frame_rate == 96000)
			{
				return std::type_identity<sai1_96k_t>{};
			}
			else
			{
				return std::type_identity<sai1_48k_t>{};
			}
		}

		template <uint32_t frame_rate, uint32_t sz = 128>
		using dfm_audio_t
			= per_h7_sai_dma<mcu_t, typename decltype(sai1_at_rate<frame_rate>())::type, DMA1_STREAM_0, DMA1_STREAM_1, sz>;

		using qspi_pins_t = per_h7_qspi_pins<pin_t::quadspi_bk1_io0_f8,
											 pin_t::quadspi_bk1_io1_f9,
											 pin_t::quadspi_bk1_io2_f7,
											 pin_t::quadspi_bk1_io3_f6,
											 pin_t::quadspi_clk_f10,
											 pin_t::quadspi_bk1_ncs_g6>;
		static_assert(mcu_div == 1 || mcu_div == 2, "brd_es_dfm QSPI rate is only declared for mcu_div 1 and 2");
		using qspi_t  = per_h7_qspi<mcu_t, adr_h750::QSPI, qspi_pins_t, (mcu_div == 1 ? 48 : 40) * 1000000>;
		using flash_t = dev_qspi_is25lp064<qspi_t, tim2_t>;

		constexpr static uint32_t FLASH_BOOT_SIZE = 512 * 1024;

		using ext_ram_t = ext_ram<mcu_t>;

		inline static bool init_done = false;

		mcu_t mcu = {ext_ram_t{}};

		bool is_dfm	 = false;
		bool is_eddy = false;
		bool is_seed = false;

		brd_es_dfm()
		{
			bug_print("boot\n");
			lac_assert(!init_done);
			bug_print("\n\n\n**** INIT ****\n");

			led_seed_t::init_out();
			led_seed_t::clr();
			test_point_t::init_out();
			led_seed_t::set();
			dfmid_t::init_inpu();
			codec_pdn::init_out();
			codec_pdn::clr();
			flash_t::init();
			eddy_t::init_inpu();

			tim2_t::delay_ms(20);

			is_dfm	= !dfmid_t::get();
			is_eddy = !eddy_t::get();
			is_seed = !is_dfm && !is_eddy;

			if(is_eddy)
			{
				bug_print("Eddy hardware\n");
			}
			else if(is_dfm)
			{
				bug_print("DFM hardware\n");
			}
			else
			{
				bug_print("Seed hardware\n");
				codec_t::init();
			}

			init_done = true;
		}
	};

} // namespace lac
