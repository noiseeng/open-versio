/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usb_setup.h"
#include <stdint.h>
#include <utl/bug.h>

namespace lac
{
	struct usb_dfu_sink
	{
		void* ctx = nullptr;

		bool (*erase)(void* ctx, uint32_t addr)									   = nullptr;
		bool (*write)(void* ctx, uint32_t addr, const uint8_t* data, uint32_t len) = nullptr;
		const uint8_t* (*read)(void* ctx, uint32_t addr, uint32_t len)			   = nullptr;
		bool (*manifest)(void* ctx)												   = nullptr;

		uint32_t erase_poll_ms	  = 0;
		uint32_t write_poll_ms	  = 0;
		uint32_t manifest_poll_ms = 500;
	};

	struct usb_dfu_handler
	{
		static constexpr uint8_t ST_IDLE		  = 2;
		static constexpr uint8_t ST_DNLOAD_SYNC	  = 3;
		static constexpr uint8_t ST_DNBUSY		  = 4;
		static constexpr uint8_t ST_DNLOAD_IDLE	  = 5;
		static constexpr uint8_t ST_MANIFEST_SYNC = 6;
		static constexpr uint8_t ST_MANIFEST	  = 7;
		static constexpr uint8_t ST_MANIFEST_WAIT = 8;
		static constexpr uint8_t ST_ERROR		  = 10;

		static constexpr uint8_t OK			 = 0x00;
		static constexpr uint8_t ERR_TARGET	 = 0x01;
		static constexpr uint8_t ERR_ADDRESS = 0x08;
		static constexpr uint8_t ERR_STALLED = 0x0F;

		static constexpr uint8_t REQ_DETACH	   = 0x00;
		static constexpr uint8_t REQ_DNLOAD	   = 0x01;
		static constexpr uint8_t REQ_UPLOAD	   = 0x02;
		static constexpr uint8_t REQ_GETSTATUS = 0x03;
		static constexpr uint8_t REQ_CLRSTATUS = 0x04;
		static constexpr uint8_t REQ_GETSTATE  = 0x05;
		static constexpr uint8_t REQ_ABORT	   = 0x06;

		static constexpr uint8_t CMD_SET_ADDR  = 0x21;
		static constexpr uint8_t CMD_ERASE_SEC = 0x41;

		enum class op
		{
			none,
			erase,
			write,
			manifest
		};

		uint8_t dfu_iface = 0;

		struct status_pkt
		{
			uint8_t bStatus;
			uint8_t bwPollTimeout[3];
			uint8_t bState;
			uint8_t iString;
		};

		usb_dfu_sink sink = {};

		uint32_t flash_base		   = 0;
		uint32_t flash_size		   = 0;
		uint32_t erase_granularity = 0;
		uint32_t read_base		   = 0;
		uint32_t read_size		   = 0;
		uint32_t transfer_size	   = 1024;

		void* callback_ctx			 = nullptr;
		void (*on_detach)(void* ctx) = nullptr;

		uint8_t	 state	  = ST_IDLE;
		uint8_t	 bstatus  = OK;
		uint32_t addr_ptr = 0;

		bool detach_seen = false;

		uint16_t pend_block = 0;
		bool	 pend_valid = false;

		volatile op pending = op::none;

		volatile bool op_failed = false;

		status_pkt status_response = {};

		void init(const usb_dfu_sink& s, uint32_t base, uint32_t fsize, uint32_t gran, uint32_t rbase = 0, uint32_t rsize = 0)
		{
			sink			  = s;
			flash_base		  = base;
			flash_size		  = fsize;
			erase_granularity = gran;
			read_base		  = rsize ? rbase : base;
			read_size		  = rsize ? rsize : fsize;
			state			  = ST_IDLE;
			bstatus			  = OK;
			addr_ptr		  = base;
			detach_seen		  = false;
			pend_valid		  = false;
			pending			  = op::none;
			op_failed		  = false;
		}

		bool busy() const { return pending != op::none; }

		void set_op_done() { pending = op::none; }

		void set_op_failed()
		{
			op_failed = true;
			pending	  = op::none;
		}

		constexpr void cancel_data() { pend_valid = false; }

		template <typename usb_t>
		void on_setup(usb_t* usb, const usb_setup* s)
		{
			settle();
			switch(s->request)
			{
				case REQ_DETACH: handle_detach(usb, s); break;
				case REQ_DNLOAD: handle_dnload_setup(usb, s); break;
				case REQ_UPLOAD: handle_upload(usb, s); break;
				case REQ_GETSTATUS: handle_getstatus(usb, s); break;
				case REQ_CLRSTATUS: handle_clrstatus(usb, s); break;
				case REQ_GETSTATE: handle_getstate(usb, s); break;
				case REQ_ABORT: handle_abort(usb, s); break;
				default: set_error(usb, ERR_STALLED); break;
			}
		}

		template <typename usb_t>
		void on_recv(usb_t* usb, uint32_t ep, uint32_t count, const uint8_t* data)
		{
			if(ep != 0 || !pend_valid)
			{
				return;
			}

			pend_valid = false;
			process_dnload_data(usb, pend_block, count, data);
		}

	  private:
		constexpr void notify_detach()
		{
			if(!detach_seen)
			{
				detach_seen = true;
				if(on_detach)
				{
					on_detach(callback_ctx);
				}
			}
		}

		void settle()
		{
			if(pending != op::none)
			{
				return;
			}
			if(op_failed)
			{
				op_failed = false;
				state	  = ST_ERROR;
				bstatus	  = ERR_TARGET;
				return;
			}
			if(state == ST_DNBUSY)
			{
				state	= ST_DNLOAD_IDLE;
				bstatus = OK;
			}
			else if(state == ST_MANIFEST)
			{
				state	= ST_MANIFEST_WAIT;
				bstatus = OK;
			}
		}

		void start(op o, bool deferred)
		{
			if(deferred)
			{
				pending = o;
				state	= (o == op::manifest) ? ST_MANIFEST : ST_DNBUSY;
			}
			else
			{
				state = (o == op::manifest) ? ST_MANIFEST_WAIT : ST_DNLOAD_SYNC;
			}
		}

		uint32_t poll_ms() const
		{
			switch(pending)
			{
				case op::erase: return sink.erase_poll_ms;
				case op::write: return sink.write_poll_ms;
				case op::manifest: return sink.manifest_poll_ms;
				default: return 0;
			}
		}

		template <typename usb_t>
		void handle_detach(usb_t* usb, const usb_setup*)
		{
			notify_detach();
			usb->setup_ack();
		}

		template <typename usb_t>
		void handle_dnload_setup(usb_t* usb, const usb_setup* s)
		{
			uint16_t block	= s->get_value();
			uint16_t length = s->get_length();

			if(state == ST_ERROR || busy())
			{
				pend_valid = false;
				if(state != ST_ERROR)
				{
					state	= ST_ERROR;
					bstatus = ERR_STALLED;
				}
				usb->per.stall_ep0();
				return;
			}

			if(length == 0)
			{
				if(block == 2 || block == 0)
				{
					state	   = ST_MANIFEST_SYNC;
					pend_valid = false;
				}
				usb->setup_ack();
				return;
			}

			state	   = ST_DNLOAD_SYNC;
			pend_block = block;
			pend_valid = true;
		}

		template <typename usb_t>
		void handle_upload(usb_t* usb, const usb_setup* s)
		{
			uint16_t block	= s->get_value();
			uint16_t length = s->get_length();

			if(block == 0)
			{
				static const uint8_t cmds[] = {0x00, CMD_SET_ADDR, CMD_ERASE_SEC};
				uint32_t			 n		= length < sizeof(cmds) ? length : (uint32_t)sizeof(cmds);
				usb->send(0, n, cmds, "dfu getcmds");
				return;
			}

			if(block < 2 || !sink.read || busy() || addr_ptr < read_base)
			{
				usb->send(0, 0, nullptr, "dfu upload none");
				return;
			}

			uint32_t addr = addr_ptr + (uint32_t)(block - 2) * transfer_size;
			if(addr >= read_base + read_size)
			{
				usb->send(0, 0, nullptr, "dfu upload end");
				return;
			}

			uint32_t avail = read_base + read_size - addr;
			uint32_t n	   = length;
			if(n > transfer_size)
			{
				n = transfer_size;
			}
			if(n > avail)
			{
				n = avail;
			}

			const uint8_t* p = sink.read(sink.ctx, addr, n);
			if(!p)
			{
				usb->send(0, 0, nullptr, "dfu upload none");
				return;
			}
			usb->send(0, n, p, "dfu upload");
		}

		template <typename usb_t>
		void handle_getstatus(usb_t* usb, const usb_setup*)
		{
			if(state == ST_MANIFEST_SYNC)
			{
				bool deferred = sink.manifest ? sink.manifest(sink.ctx) : false;
				start(op::manifest, deferred);
			}
			else if(state == ST_DNLOAD_SYNC)
			{
				state = ST_DNLOAD_IDLE;
			}

			build_status(bstatus, state, busy() ? poll_ms() : 0);
			usb->send(0, sizeof(status_pkt), (const uint8_t*)&status_response, "dfu getstatus");
		}

		template <typename usb_t>
		void handle_clrstatus(usb_t* usb, const usb_setup*)
		{
			if(state == ST_ERROR)
			{
				state	= ST_IDLE;
				bstatus = OK;
			}
			usb->setup_ack();
		}

		template <typename usb_t>
		void handle_getstate(usb_t* usb, const usb_setup*)
		{ usb->send(0, 1, &state, "dfu getstate"); }

		template <typename usb_t>
		void handle_abort(usb_t* usb, const usb_setup*)
		{
			state	   = ST_IDLE;
			bstatus	   = OK;
			pend_valid = false;
			usb->setup_ack();
		}

		template <typename usb_t>
		void process_dnload_data(usb_t* usb, uint16_t block, uint32_t count, const uint8_t* data)
		{
			if(busy())
			{
				set_error(usb, ERR_TARGET);
			}
			else if(block == 0)
			{
				process_special_cmd(usb, count, data);
			}
			else if(block >= 2)
			{
				process_data_write(usb, block, count, data);
			}
			else
			{
				set_error(usb, ERR_TARGET);
			}
		}

		static uint32_t cmd_addr(const uint8_t* data)
		{
			lac_assert_align(data, 4);
			const uint32_t* w = (const uint32_t*)data;
			return (w[0] >> 8) | (w[1] << 24);
		}

		template <typename usb_t>
		void process_special_cmd(usb_t* usb, uint32_t count, const uint8_t* data)
		{
			if(count < 1)
			{
				set_error(usb, ERR_TARGET);
				return;
			}

			uint8_t cmd = data[0];

			if(cmd == CMD_SET_ADDR && count == 5)
			{
				uint32_t addr = cmd_addr(data);
				if(addr < read_base || addr >= read_base + read_size)
				{
					set_error(usb, ERR_ADDRESS);
					return;
				}
				addr_ptr = addr;
				state	 = ST_DNLOAD_SYNC;
				usb->setup_ack();
				return;
			}

			if(cmd == CMD_ERASE_SEC && count == 5)
			{
				uint32_t addr = cmd_addr(data);
				if(addr < flash_base || addr >= flash_base + flash_size
				   || (erase_granularity > 0 && (addr - flash_base) % erase_granularity != 0))
				{
					set_error(usb, ERR_ADDRESS);
					return;
				}
				notify_detach();
				start(op::erase, sink.erase ? sink.erase(sink.ctx, addr) : false);
				usb->setup_ack();
				return;
			}

			set_error(usb, ERR_TARGET);
		}

		template <typename usb_t>
		void process_data_write(usb_t* usb, uint16_t block, uint32_t count, const uint8_t* data)
		{
			if(addr_ptr < flash_base)
			{
				set_error(usb, ERR_ADDRESS);
				return;
			}
			uint32_t offset = (addr_ptr - flash_base) + (uint32_t)(block - 2) * transfer_size;
			if(count > transfer_size || offset + count > flash_size)
			{
				set_error(usb, ERR_ADDRESS);
				return;
			}
			bool deferred = (sink.write && count > 0) ? sink.write(sink.ctx, flash_base + offset, data, count) : false;
			start(op::write, deferred);
			usb->setup_ack();
		}

		template <typename usb_t>
		void set_error(usb_t* usb, uint8_t err)
		{
			state	= ST_ERROR;
			bstatus = err;
			build_status(err, ST_ERROR, 0);
			usb->setup_ack();
		}

		constexpr void build_status(uint8_t bstat, uint8_t bstate, uint32_t ms)
		{
			status_response.bStatus			 = bstat;
			status_response.bwPollTimeout[0] = (uint8_t)(ms & 0xFF);
			status_response.bwPollTimeout[1] = (uint8_t)((ms >> 8) & 0xFF);
			status_response.bwPollTimeout[2] = (uint8_t)((ms >> 16) & 0xFF);
			status_response.bState			 = bstate;
			status_response.iString			 = 0;
		}
	};
} // namespace lac
