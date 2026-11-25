/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usb_dfu_handler.h"
#include <stdint.h>

namespace lac
{
	struct usb_dfu_stage
	{
		uint8_t* buf		 = nullptr;
		uint32_t size		 = 0;
		uint32_t base		 = 0;
		uint32_t granularity = 0;
		uint32_t written_len = 0;

		void* commit_ctx														= nullptr;
		bool (*on_commit)(void* ctx, uint32_t base, uint8_t* buf, uint32_t len) = nullptr;

		constexpr void init(uint8_t* b, uint32_t s, uint32_t flash_base, uint32_t gran)
		{
			buf			= b;
			size		= s;
			base		= flash_base;
			granularity = gran;
			written_len = 0;
		}

		usb_dfu_sink sink()
		{
			usb_dfu_sink s = {};
			s.ctx		   = this;
			s.erase		   = erase_cb;
			s.write		   = write_cb;
			s.read		   = read_cb;
			s.manifest	   = manifest_cb;
			return s;
		}

		static bool erase_cb(void* ctx, uint32_t addr)
		{
			auto*	 t	  = (usb_dfu_stage*)ctx;
			uint32_t soff = addr - t->base;
			uint32_t slen = t->granularity ? t->granularity : t->size;
			if(soff < t->size)
			{
				if(soff + slen > t->size)
				{
					slen = t->size - soff;
				}
				for(uint32_t i = 0; i < slen; i++)
				{
					t->buf[soff + i] = 0xFF;
				}
			}
			return false;
		}

		static bool write_cb(void* ctx, uint32_t addr, const uint8_t* data, uint32_t len)
		{
			auto*	 t	 = (usb_dfu_stage*)ctx;
			uint32_t off = addr - t->base;
			if(off + len > t->size)
			{
				return false;
			}
			for(uint32_t i = 0; i < len; i++)
			{
				t->buf[off + i] = data[i];
			}
			if(off + len > t->written_len)
			{
				t->written_len = off + len;
			}
			return false;
		}

		static const uint8_t* read_cb(void* ctx, uint32_t addr, uint32_t len)
		{
			auto*	 t	 = (usb_dfu_stage*)ctx;
			uint32_t off = addr - t->base;
			if(addr < t->base || off + len > t->size)
			{
				return nullptr;
			}
			return &t->buf[off];
		}

		static bool manifest_cb(void* ctx)
		{
			auto* t = (usb_dfu_stage*)ctx;
			if(!t->on_commit)
			{
				return false;
			}
			return t->on_commit(t->commit_ctx, t->base, t->buf, t->written_len);
		}
	};
} // namespace lac
