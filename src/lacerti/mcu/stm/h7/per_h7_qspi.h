/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <bug/eabi.h>
#include <utl/bug.h>

#include <cstdint>

#include "per_h7_qspi_cmd.h"
#include "reg_h7_qspi.h"

namespace lac
{
	template <typename gpio_io0, typename gpio_io1, typename gpio_io2, typename gpio_io3, typename gpio_clk, typename gpio_ncs>
	struct per_h7_qspi_pins
	{
		using io0_t = gpio_io0;
		using io1_t = gpio_io1;
		using io2_t = gpio_io2;
		using io3_t = gpio_io3;
		using clk_t = gpio_clk;
		using ncs_t = gpio_ncs;

		io0_t io0;
		io1_t io1;
		io2_t io2;
		io3_t io3;
		clk_t clk;
		ncs_t ncs;

		static void init_pins()
		{
			gpio_io0::init_out();
			gpio_io1::init_out();
			gpio_io2::init_out();
			gpio_io3::init_out();
			gpio_clk::init_out();
			gpio_ncs::init_out();
		}
	};

	template <typename mcu_t, uint32_t base, typename pins_t, uint32_t clk_hz>
	struct per_h7_qspi
	{
		using this_t = per_h7_qspi<mcu_t, base, pins_t, clk_hz>;
		using reg_t	 = reg_h7_qspi;
		using rcc_t	 = mcu_t::rcc_t;

		const static uint32_t BASE = base;

		static reg_h7_qspi* r() { return (reg_h7_qspi*)BASE; }

		static void init(uint32_t addr_bits)
		{
			pins_t::init_pins();
			mcu_t::get_rcc()->enable_qspi();
			mcu_t::get_rcc()->reset_qspi();

			uint32_t clkdiv = rcc_t::get_hz_quadspi() / clk_hz;

			lac_assert(clkdiv);
			lac_assert(clkdiv * clk_hz == rcc_t::get_hz_quadspi());
			lac_assert(clkdiv < 256);

			r()->init(clkdiv, addr_bits);
		}

		static void wait_busy() { r()->wait_busy(); }

		static void cmdr(reg_h7_qspi::spi_mode inst_mode,
						 uint8_t			   inst,
						 reg_h7_qspi::spi_mode addr_mode,
						 uint32_t			   addr_bytes,
						 uint32_t			   addr,
						 uint32_t			   data_count)
		{
			r()->cmd(reg_h7_qspi::MODE_READ,
					 inst_mode,
					 inst,
					 addr_mode,
					 addr_bytes,
					 addr,
					 reg_h7_qspi::MODE_NONE,
					 0,
					 0,
					 0,
					 reg_h7_qspi::MODE_SPI,
					 data_count);
		}

		static void cmdr(reg_h7_qspi::spi_mode inst_mode,
						 uint8_t			   inst,
						 reg_h7_qspi::spi_mode addr_mode,
						 uint32_t			   addr_bytes,
						 uint32_t			   addr,
						 reg_h7_qspi::spi_mode data_mode,
						 uint32_t			   dummy_cycles,
						 uint32_t			   data_count)
		{
			r()->cmd(reg_h7_qspi::MODE_READ,
					 inst_mode,
					 inst,
					 addr_mode,
					 addr_bytes,
					 addr,
					 reg_h7_qspi::MODE_NONE,
					 0,
					 0,
					 dummy_cycles,
					 data_mode,
					 data_count);
		}

		static void cmdw(reg_h7_qspi::spi_mode inst_mode,
						 uint8_t			   inst,
						 reg_h7_qspi::spi_mode addr_mode,
						 uint32_t			   addr_bytes,
						 uint32_t			   addr,
						 uint32_t			   count = 0)
		{
			r()->cmd(reg_h7_qspi::MODE_WRITE,
					 inst_mode,
					 inst,
					 addr_mode,
					 addr_bytes,
					 addr,
					 reg_h7_qspi::MODE_NONE,
					 0,
					 0,
					 0,
					 count ? reg_h7_qspi::MODE_SPI : reg_h7_qspi::MODE_NONE,
					 count);
		}

		static void cmdw(reg_h7_qspi::spi_mode inst_mode, uint8_t inst)
		{
			r()->cmd(reg_h7_qspi::MODE_WRITE,
					 inst_mode,
					 inst,
					 reg_h7_qspi::MODE_NONE,
					 0,
					 0,
					 reg_h7_qspi::MODE_NONE,
					 0,
					 0,
					 0,
					 reg_h7_qspi::MODE_NONE,
					 0);
		}

		static void cmdr(reg_h7_qspi::spi_mode inst_mode, uint8_t inst, uint32_t data_bytes)
		{
			r()->cmd(reg_h7_qspi::MODE_READ,
					 inst_mode,
					 inst,
					 reg_h7_qspi::MODE_NONE,
					 0,
					 0,
					 reg_h7_qspi::MODE_NONE,
					 0,
					 0,
					 0,
					 data_bytes ? reg_h7_qspi::MODE_SPI : reg_h7_qspi::MODE_NONE,
					 data_bytes);
		}

		static void enter_mmap(uint8_t inst, uint32_t addr_bytes, uint32_t dummy_cycles)
		{
			r()->enter_mmap(
				reg_h7_qspi::MODE_SPI, inst, reg_h7_qspi::MODE_SPI, addr_bytes, reg_h7_qspi::MODE_QPI, dummy_cycles);
		}

		static void exit_mmap() { r()->abort(); }

		static void send(uint32_t count, const uint8_t* data) { r()->send(count, data); }

		static void recv(uint32_t count, uint8_t* data) { r()->recv(count, data); }

		static void recv32(uint32_t count, uint8_t* data) { r()->recv32(count, data); }

		static bool is_busy() { return r()->is_busy(); }

		using cmd_qspi = per_h7_qspi_cmd<this_t>;
	};

} // namespace lac
