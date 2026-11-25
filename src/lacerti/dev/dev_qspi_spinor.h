/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utl/bitflag.h>
#include <utl/bug.h>
#include <utl/std.h>

namespace lac
{
	template <typename part_t, typename qspi_t, typename timer_t>
	struct dev_qspi_spinor
	{
		enum command
		{
			CMD_PP	   = 0x02,
			CMD_NORD   = 0x03,
			CMD_FRD	   = 0x0B,
			CMD_FRQO   = 0x6B,
			CMD_WREN   = 0x06,
			CMD_WRDI   = 0x04,
			CMD_RDSR   = 0x05,
			CMD_CER	   = 0xC7,
			CMD_SER	   = 0x20,
			CMD_BER32K = 0x52,
			CMD_BER64K = 0xD8,
			CMD_RDID   = 0x9F,
		};

		const static uint32_t ADDRESS_BITS		= part_t::ADDRESS_BITS;
		const static uint32_t MMAP_ADDRESS_BITS = part_t::MMAP_ADDRESS_BITS;
		const static uint32_t DUMMY_CYCLES		= part_t::DUMMY_CYCLES;
		const static uint32_t PAGE_BYTES		= 256;
		const static uint32_t SECTOR_BYTES		= 4096;
		const static uint32_t BLOCK32_BYTES		= 32768;
		const static uint32_t BLOCK64_BYTES		= 65536;

		const static qspi_t::reg_t::op_mode MODE_READ  = qspi_t::reg_t::MODE_READ;
		const static qspi_t::reg_t::op_mode MODE_WRITE = qspi_t::reg_t::MODE_WRITE;

		using this_t   = dev_qspi_spinor<part_t, qspi_t, timer_t>;
		using cmd_qspi = typename qspi_t::cmd_qspi;

		using WIP = bitflag<0>;
		using WEL = bitflag<1>;
		using QE  = typename part_t::QE;

		constexpr static bool jedec_id_ok(uint8_t mfg, uint8_t memtype, uint8_t capacity)
		{ return mfg == part_t::JEDEC_MFG && memtype == part_t::JEDEC_MEMTYPE && capacity == part_t::JEDEC_CAPACITY; }

		constexpr static uint8_t encode_qe(uint8_t sr, bool enable)
		{ return enable ? (uint8_t)(sr | QE::BIT) : (uint8_t)(sr & ~QE::BIT); }

		constexpr static bool decode_qe(uint8_t sr) { return (sr & QE::BIT) != 0; }

		constexpr static bool decode_wip(uint8_t sr1) { return (sr1 & WIP::BIT) != 0; }

		constexpr static bool decode_wel(uint8_t sr1) { return (sr1 & WEL::BIT) != 0; }

		constexpr static cmd_qspi cmd_wait_write(uint8_t* sr) { return {MODE_READ, CMD_RDSR, 0, 0, 1, sr, WIP::BIT, 0}; }

		constexpr static cmd_qspi cmd_write_enable() { return {MODE_WRITE, CMD_WREN}; }

		constexpr static cmd_qspi cmd_erase(uint8_t cmd, uint32_t addr) { return {MODE_WRITE, cmd, 3, addr}; }

		constexpr static cmd_qspi cmd_write(uint32_t address, uint32_t count, const uint8_t* data)
		{
			lac_assert(count != 0);
			return {MODE_WRITE, CMD_PP, 3, address, count, (uint8_t*)data};
		}

		constexpr static cmd_qspi cmd_read(uint32_t address, uint32_t count, uint8_t* data)
		{
			lac_assert(count != 0);
			return {MODE_READ, CMD_NORD, 3, address, count, data};
		}

		dev_qspi_spinor() { init(); }

		static void init()
		{
			qspi_t::init(MMAP_ADDRESS_BITS);

			if constexpr(part_t::HAS_JEDEC_ID)
			{
				uint8_t id[3] = {};
				qspi_t::cmdr(qspi_t::reg_t::MODE_SPI, CMD_RDID, 3);
				qspi_t::recv(3, id);
				qspi_t::wait_busy();

				lac_assert(jedec_id_ok(id[0], id[1], id[2]));
			}
		}

		static uint8_t read_status(uint8_t cmd)
		{
			lac_assert(!qspi_t::is_busy());
			qspi_t::wait_busy();
			uint8_t sr;
			qspi_t::cmdr(qspi_t::reg_t::MODE_SPI, cmd, 1);
			qspi_t::recv(1, &sr);
			qspi_t::wait_busy();
			return sr;
		}

		static uint8_t get_status() { return read_status(CMD_RDSR); }

		static uint8_t get_status2() { return read_status(part_t::CMD_RDSR2); }

		static void wait_write(uint32_t dlyms = 0)
		{
			while(qspi_t::is_busy())
			{
				timer_t::delay_ms(dlyms);
			}

			lac_assert(!qspi_t::is_busy());

			while(decode_wip(get_status()))
			{
				timer_t::delay_ms(dlyms);
			}

			lac_assert(!qspi_t::is_busy());
		}

		static void cmdw(command instruction) { qspi_t::cmdw(qspi_t::reg_t::MODE_SPI, instruction); }

		static void cmdw(command instruction, uint32_t address, uint32_t count = 0)
		{ qspi_t::cmdw(qspi_t::reg_t::MODE_SPI, instruction, qspi_t::reg_t::MODE_SPI, 3, address, count); }

		static void cmdr(command instruction, uint32_t address, uint32_t count)
		{ qspi_t::cmdr(qspi_t::reg_t::MODE_SPI, instruction, qspi_t::reg_t::MODE_SPI, 3, address, count); }

		static void enable_write()
		{
			cmdw(CMD_WREN);
			qspi_t::wait_busy();
		}

		static void disable_write()
		{
			cmdw(CMD_WRDI);
			qspi_t::wait_busy();
		}

		static void write_status(uint8_t cmd, uint8_t value)
		{
			enable_write();
			qspi_t::wait_busy();
			qspi_t::cmdw(qspi_t::reg_t::MODE_SPI, cmd, qspi_t::reg_t::MODE_NONE, 0, 0, 1);
			qspi_t::send(1, &value);
			wait_write();
		}

		static void enable_quad()
		{
			uint8_t sr = read_status(part_t::CMD_QE_READ);
			if(decode_qe(sr))
			{
				return;
			}
			write_status(part_t::CMD_QE_WRITE, encode_qe(sr, true));
		}

		static void erase_4k(uint32_t address)
		{
			lac_assert(address % 4096 == 0);
			enable_write();
			qspi_t::wait_busy();
			cmdw(CMD_SER, address);
			wait_write();
		}

		static void erase_32k(uint32_t address)
		{
			lac_assert(address % 32768 == 0);
			enable_write();
			qspi_t::wait_busy();
			cmdw(CMD_BER32K, address);
			wait_write();
		}

		static void erase_64k(uint32_t address)
		{
			lac_assert(address % 65536 == 0);
			enable_write();
			qspi_t::wait_busy();
			cmdw(CMD_BER64K, address);
			wait_write();
		}

		static void erase_all()
		{
			enable_write();
			qspi_t::wait_busy();
			cmdw(CMD_CER);
			wait_write();
		}

		static void write(uint32_t address, uint32_t count, const uint8_t* data)
		{
			while(count)
			{
				enable_write();
				qspi_t::wait_busy();
				uint32_t amt = std::min((uint32_t)256, count);
				cmdw(CMD_PP, address, amt);
				qspi_t::send(amt, data);
				wait_write();
				count -= amt;
				address += amt;
				data += amt;
			}
		}

		static void write_block(uint32_t ofs, uint32_t size, const uint8_t* data)
		{
			erase_4k(ofs);
			wait_write();
			write(ofs, size, data);
			wait_write();
		}

		static void write_blocks(uint32_t addr, uint32_t size, const uint8_t* data)
		{
			lac_assert(addr % SECTOR_BYTES == 0);

			auto ofs = addr;
			auto rem = size;
			auto fww = data;

			while(rem > 0)
			{
				size_t s = rem > SECTOR_BYTES ? SECTOR_BYTES : rem;

				write_block(ofs, s, fww);

				rem -= s;
				fww += s;
				ofs += s;
			}
		}

		static void read(uint32_t address, uint32_t count, uint8_t* data)
		{
			cmdr(CMD_NORD, address, count);
			qspi_t::recv(count, data);
			qspi_t::wait_busy();
		}

		static void read_fast(uint32_t address, uint32_t count, uint8_t* data)
		{
			qspi_t::cmdr(qspi_t::reg_t::MODE_SPI,
						 CMD_FRD,
						 qspi_t::reg_t::MODE_SPI,
						 3,
						 address,
						 qspi_t::reg_t::MODE_SPI,
						 DUMMY_CYCLES,
						 count);
			qspi_t::recv32(count, data);
			qspi_t::wait_busy();
		}

		static void read_quad(uint32_t address, uint32_t count, uint8_t* data)
		{
			qspi_t::cmdr(qspi_t::reg_t::MODE_SPI,
						 CMD_FRQO,
						 qspi_t::reg_t::MODE_SPI,
						 3,
						 address,
						 qspi_t::reg_t::MODE_QPI,
						 DUMMY_CYCLES,
						 count);
			qspi_t::recv32(count, data);
			qspi_t::wait_busy();
		}

		static void enter_mmap() { qspi_t::enter_mmap(CMD_FRQO, 3, DUMMY_CYCLES); }

		static void exit_mmap() { qspi_t::exit_mmap(); }

		struct pol_write_one
		{
			uint8_t	 sr = 0;
			uint32_t ip = 0;
			cmd_qspi cmds[8];

			pol_write_one() {}

			pol_write_one(uint32_t offset, uint32_t count, uint8_t* data, uint8_t* check)
			: cmds{
				  cmd_write_enable(),
				  cmd_erase(CMD_SER, offset),
				  cmd_wait_write(&sr),
				  cmd_write_enable(),
				  cmd_write(offset, count, data),
				  cmd_wait_write(&sr),
				  cmd_read(offset, count, check),
				  cmd_qspi::cmd_end(),
			  }
			{
				if(!check)
				{
					cmds[6] = cmd_qspi::cmd_end();
				}
			}

			bool step() { return cmd_qspi::step(ip, cmds); };
		};

		struct pol_erase
		{
			uint8_t	 sr = 0;
			uint32_t ip = 0;
			cmd_qspi cmds[4];

			pol_erase() {}

			pol_erase(uint8_t cmd, uint32_t address)
			: cmds{
				  cmd_write_enable(),
				  cmd_erase(cmd, address),
				  cmd_wait_write(&sr),
				  cmd_qspi::cmd_end(),
			  }
			{}

			bool step() { return cmd_qspi::step(ip, cmds); };
		};

		template <uint32_t max_write_size>
		struct pol_program
		{
			constexpr static uint32_t MAX_WRITE_SIZE  = max_write_size;
			constexpr static uint32_t WRITE_SIZE	  = 0x100;
			constexpr static uint32_t MAX_WRITE_ALIGN = align(max_write_size, WRITE_SIZE);
			constexpr static uint32_t MAX_WRITE_CMD	  = MAX_WRITE_ALIGN / WRITE_SIZE;
			constexpr static uint32_t CMD_COUNT		  = 5 + 3 * MAX_WRITE_CMD;

			static_assert(MAX_WRITE_CMD * WRITE_SIZE >= max_write_size, "bad write size");

			uint8_t	 sr = 0;
			uint32_t ip = 0;
			cmd_qspi cmds[CMD_COUNT];

			pol_program() {}

			pol_program(uint32_t address, uint32_t count, const uint8_t* write, uint8_t* read, bool erase = true)
			{
				lac_assert(max_write_size >= count);
				lac_assert(count <= SECTOR_BYTES);

				uint32_t idx = 0;

				uint32_t check_address = address;
				uint32_t check_count   = count;

				if(write)
				{
					if(erase)
					{
						cmds[idx++] = cmd_write_enable();
						cmds[idx++] = cmd_erase(CMD_SER, address);
						cmds[idx++] = cmd_wait_write(&sr);
					}

					while(count)
					{
						uint32_t amt = std::min(WRITE_SIZE, count);

						cmds[idx++] = cmd_write_enable();
						cmds[idx++] = cmd_write(address, amt, write);
						cmds[idx++] = cmd_wait_write(&sr);

						count -= amt;
						address += amt;
						write += amt;

						lac_assert(idx < CMD_COUNT);
					}
				}

				if(read)
				{
					cmds[idx++] = cmd_read(check_address, check_count, read);
				}

				cmds[idx++] = cmd_qspi::cmd_end();

				lac_assert(idx <= CMD_COUNT);
			}

			bool step() { return cmd_qspi::step(ip, cmds); };
		};
	};
} // namespace lac
