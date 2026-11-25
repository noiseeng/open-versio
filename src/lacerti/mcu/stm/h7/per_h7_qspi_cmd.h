/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <utl/bug.h>

#include <cstdint>

namespace lac
{
	template <typename qspi_t>
	struct per_h7_qspi_cmd
	{
		using reg_t	  = typename qspi_t::reg_t;
		using op_mode = typename reg_t::op_mode;

		enum cmd_status
		{
			CMD_START,
			CMD_SEND,
			CMD_RECV,
			CMD_WAIT,
			CMD_CHECK,
			CMD_DONE,
			CMD_END
		};

		op_mode	 mode		 = reg_t::MODE_READ;
		uint8_t	 instruction = 0;
		uint32_t addr_bytes	 = 0;
		uint32_t address	 = 0;
		uint32_t data_bytes	 = 0;
		uint8_t* data		 = 0;

		uint8_t flag  = 0;
		uint8_t check = 0;

		cmd_status status = CMD_START;

		uint32_t data_index = 0;

		static per_h7_qspi_cmd cmd_end() { return {reg_t::MODE_WRITE, 0, 0, 0, 0, 0, 0, 0, CMD_END}; }

		bool is_data() const { return data_bytes != 0 && data; }

		bool is_recv() const { return mode == reg_t::MODE_READ; }

		bool is_send() const { return mode == reg_t::MODE_WRITE; }

		cmd_status get_data_status() const
		{
			if(is_data())
			{
				if(is_recv())
				{
					return CMD_RECV;
				}
				else if(is_send())
				{
					return CMD_SEND;
				}
			}

			return CMD_WAIT;
		}

		bool step()
		{
			switch(status)
			{
				case CMD_START: {
					qspi_t::r()->cmd(mode,
									 reg_t::MODE_SPI,
									 instruction,
									 addr_bytes ? reg_t::MODE_SPI : reg_t::MODE_NONE,
									 addr_bytes,
									 address,
									 reg_t::MODE_NONE,
									 0,
									 0,
									 0,
									 data_bytes ? reg_t::MODE_SPI : reg_t::MODE_NONE,
									 data_bytes);
					status = get_data_status();
					break;
				}
				case CMD_RECV: {
					lac_assert(is_recv() && is_data());
					if(!qspi_t::r()->pol_recv(data_index, data_bytes, data))
					{
						status = CMD_WAIT;
					}
					break;
				}
				case CMD_SEND: {
					lac_assert(is_send() && is_data());
					if(!qspi_t::r()->pol_send(data_index, data_bytes, data))
					{
						status = CMD_WAIT;
					}
					break;
				}
				case CMD_WAIT: {
					if(!qspi_t::is_busy())
					{
						status = CMD_CHECK;
					}
					break;
				}
				case CMD_CHECK: {
					if(flag)
					{
						lac_assert(data);
						lac_assert(data_bytes == 1);

						if((flag & *data) == check)
						{
							status = CMD_DONE;
						}
						else
						{
							status	   = CMD_START;
							data_index = 0;
						}
					}
					else
					{
						status = CMD_DONE;
					}
					break;
				}
				case CMD_DONE: return true;
				case CMD_END: return false;
			}

			return false;
		}

		static bool step(uint32_t& ip, per_h7_qspi_cmd* cmds)
		{
			while(cmds[ip].step())
			{
				ip += 1;
			}

			lac_assert(ip < 100);

			return cmds[ip].status == CMD_END;
		}

		static void blk_exec(per_h7_qspi_cmd* cmds)
		{
			uint32_t ip = 0;

			while(!step(ip, cmds))
				;
		}
	};

} // namespace lac
