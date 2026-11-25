/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <utl/bitfield.h>
#include <utl/bitflag.h>
#include <utl/bug.h>

#include <atomic>
#include <cstdint>

namespace lac
{
	struct reg_h7_qspi
	{
		enum spi_mode
		{
			MODE_NONE = 0,
			MODE_SPI,
			MODE_DPI,
			MODE_QPI
		};

		enum op_mode
		{
			MODE_WRITE = 0,
			MODE_READ,
			MODE_POLL,
			MODE_MAP
		};

		volatile uint32_t CR;
		volatile uint32_t DCR;
		volatile uint32_t SR;
		volatile uint32_t FCR;
		volatile uint32_t DLR;
		volatile uint32_t CCR;
		volatile uint32_t AR;
		volatile uint32_t ABR;
		volatile uint32_t DR;
		volatile uint32_t PSMKR;
		volatile uint32_t PSMAR;
		volatile uint32_t PIR;
		volatile uint32_t LPTR;

		using CR_EN		   = bitflag<0>;
		using CR_ABORT	   = bitflag<1>;
		using CR_TCEN	   = bitflag<3>;
		using CR_SSHIFT	   = bitflag<4>;
		using CR_DFM	   = bitflag<6>;
		using CR_FSEL	   = bitflag<7>;
		using CR_FTHRES	   = bitfield<8, 5>;
		using CR_TEIE	   = bitflag<16>;
		using CR_TCIE	   = bitflag<17>;
		using CR_FTIE	   = bitflag<18>;
		using CR_SMIE	   = bitflag<19>;
		using CR_TOIE	   = bitflag<20>;
		using CR_APMS	   = bitflag<22>;
		using CR_PMM	   = bitflag<23>;
		using CR_PRESCALER = bitfield<24, 8>;

		using DCR_FSIZE	 = bitfield<16, 5>;
		using DCR_CSHT	 = bitfield<8, 2>;
		using DCR_CKMODE = bitflag<0>;

		using SR_FLEVEL = bitfield<8, 6>;
		using SR_BUSY	= bitflag<5>;
		using SR_TOF	= bitflag<4>;
		using SR_SMF	= bitflag<3>;
		using SR_FTF	= bitflag<2>;
		using SR_TCF	= bitflag<1>;
		using SR_TEF	= bitflag<0>;

		using CCR_DDRM		  = bitflag<31>;
		using CCR_DHHC		  = bitflag<30>;
		using CCR_FRCM		  = bitflag<29>;
		using CCR_SIOO		  = bitflag<28>;
		using CCR_FMODE		  = bitfield<26, 2>;
		using CCR_DMODE		  = bitfield<24, 2>;
		using CCR_DCYC		  = bitfield<18, 5>;
		using CCR_ABSIZE	  = bitfield<16, 2>;
		using CCR_ABMODE	  = bitfield<14, 2>;
		using CCR_ADSIZE	  = bitfield<12, 2>;
		using CCR_ADMODE	  = bitfield<10, 2>;
		using CCR_IMODE		  = bitfield<8, 2>;
		using CCR_INSTRUCTION = bitfield<0, 8>;

		bool is_busy() const { return SR_BUSY::get(SR) != 0; }

		bool is_fifo_ready() const { return SR_FTF::get(SR) != 0; }

		void wait_busy() const
		{
			while(is_busy()) {}
		}

		void wait_fifo_ready() const
		{
			while(!is_fifo_ready()) {}
		}

		void init(uint32_t clkdiv, uint32_t flash_address_bits)
		{
			lac_assert(flash_address_bits <= 32);
			CR_EN::clr(CR);
			wait_busy();
			DCR_FSIZE::set(DCR, flash_address_bits - 1);
			CR_PRESCALER::set(CR, clkdiv - 1);
			CR_FTHRES::set(CR, 0);
			CR_EN::set(CR);
		}

		template <typename per_rcc_t>
		void get_clk_hz()
		{
			uint32_t clkdiv = CR_PRESCALER::get(CR) + 1;
			return per_rcc_t::get_hz_quadspi() / (clkdiv);
		}

		void cmd(op_mode  mode,
				 spi_mode inst_mode,
				 uint8_t  inst,
				 spi_mode addr_mode,
				 uint32_t addr_bytes,
				 uint32_t addr,
				 spi_mode alt_mode,
				 uint32_t alt_bytes,
				 uint32_t alt,
				 uint32_t dummy_bits,
				 spi_mode data_mode,
				 uint32_t data_bytes)
		{
			lac_assert(!is_busy());

			if(data_mode)
			{
				DLR = data_bytes - 1;
			}

			lac_assert(data_mode ? data_bytes : data_bytes == 0);

			std::atomic_thread_fence(std::memory_order_seq_cst);

			uint32_t ccr = CCR_INSTRUCTION::val(inst) | CCR_IMODE::val(inst_mode) | CCR_ADMODE::val(addr_mode)
						   | CCR_ADSIZE::val(addr_mode ? addr_bytes - 1 : 0) | CCR_ABMODE::val(alt_mode)
						   | CCR_ABSIZE::val(alt_mode ? alt_bytes - 1 : 0) | CCR_DCYC::val(dummy_bits)
						   | CCR_DMODE::val(data_mode) | CCR_FMODE::val(mode);

			lac_assert(ccr);

			CCR = ccr;

			std::atomic_thread_fence(std::memory_order_seq_cst);

			if(addr_mode)
			{
				AR = addr;
			}
		}

		void enter_mmap(spi_mode inst_mode,
						uint8_t	 inst,
						spi_mode addr_mode,
						uint32_t addr_bytes,
						spi_mode data_mode,
						uint32_t dummy_bits)
		{
			lac_assert(!is_busy());

			std::atomic_thread_fence(std::memory_order_seq_cst);

			uint32_t ccr = CCR_INSTRUCTION::val(inst) | CCR_IMODE::val(inst_mode) | CCR_ADMODE::val(addr_mode)
						   | CCR_ADSIZE::val(addr_mode ? addr_bytes - 1 : 0) | CCR_DCYC::val(dummy_bits)
						   | CCR_DMODE::val(data_mode) | CCR_FMODE::val(op_mode::MODE_MAP);

			lac_assert(ccr);

			CCR = ccr;

			std::atomic_thread_fence(std::memory_order_seq_cst);
		}

		void abort()
		{
			CR_ABORT::set(CR);
			while(CR_ABORT::get(CR)) {}
			wait_busy();
		}

		void send(uint32_t count, const uint8_t* data)
		{
			for(uint32_t i = 0; i < count; i++)
			{
				wait_fifo_ready();
				std::atomic_thread_fence(std::memory_order_seq_cst);
				*((volatile uint8_t*)&DR) = data[i];
			}

			wait_busy();
		}

		void recv(uint32_t count, uint8_t* data)
		{
			for(uint32_t i = 0; i < count; i++)
			{
				wait_fifo_ready();
				std::atomic_thread_fence(std::memory_order_seq_cst);
				data[i] = *((volatile uint8_t*)&DR);
			}

			wait_busy();
		}

		void recv32(uint32_t count, uint8_t* data)
		{
			uint32_t words = count >> 2;

			for(uint32_t i = 0; i < words; i++)
			{
				while(SR_FLEVEL::get(SR) < 4) {}
				std::atomic_thread_fence(std::memory_order_seq_cst);
				uint32_t w = *((volatile uint32_t*)&DR);
				__builtin_memcpy(data, &w, 4);
				data += 4;
			}

			for(uint32_t i = 0; i < (count & 3); i++)
			{
				wait_fifo_ready();
				std::atomic_thread_fence(std::memory_order_seq_cst);
				*data++ = *((volatile uint8_t*)&DR);
			}

			wait_busy();
		}

		bool pol_recv(uint32_t& i, uint32_t count, uint8_t* data)
		{
			lac_assert(i < count);

			while(is_fifo_ready() && i < count)
			{
				std::atomic_thread_fence(std::memory_order_seq_cst);
				data[i++] = *((volatile uint8_t*)&DR);
			}

			return i < count;
		}

		bool pol_send(uint32_t& i, uint32_t count, const uint8_t* data)
		{
			lac_assert(i < count);

			while(is_fifo_ready() && i < count)
			{
				std::atomic_thread_fence(std::memory_order_seq_cst);
				*((volatile uint8_t*)&DR) = data[i++];
			}

			return i < count;
		}
	};

} // namespace lac
