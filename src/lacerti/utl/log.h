/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "bug.h"
#include <array>
#include <atomic>
#include <cstddef>
#include <stdint.h>

namespace lac
{
	template <int32_t up_count, size_t up_size, int32_t dn_count, size_t dn_size>
	struct log
	{
		static constexpr size_t DN_CAPACITY = dn_size - 1;

		struct buffer
		{
			const char*		  name		  = nullptr;
			char*			  buffer	  = nullptr;
			uint32_t		  size		  = 0;
			volatile uint32_t write_index = 0;
			volatile uint32_t read_index  = 0;
			uint32_t		  flags		  = 0;
		};

		char	id[16]			  = {};
		int32_t up_buffer_count	  = up_count;
		int32_t down_buffer_count = dn_count;
		buffer	up[up_count]	  = {};
		buffer	dn[dn_count]	  = {};

		bool initialized = false;
		bool block		 = false;

		std::array<std::array<char, up_size>, up_count> up_buffers = {};
		std::array<std::array<char, dn_size>, dn_count> dn_buffers = {};

		log(const log&)			   = delete;
		log& operator=(const log&) = delete;

		constexpr log()
		{
			for(int32_t i = 0; i < up_count; i++)
			{
				up[i].name	 = "terminal";
				up[i].buffer = up_buffers[i].data();
				up[i].size	 = (uint32_t)up_buffers[i].size();
			}

			for(int32_t i = 0; i < dn_count; i++)
			{
				dn[i].name	 = "terminal";
				dn[i].buffer = dn_buffers[i].data();
				dn[i].size	 = (uint32_t)dn_buffers[i].size();
			}

			static constexpr char str[sizeof(id)] = "SEGGER RTT";

			if(__builtin_is_constant_evaluated())
			{
				for(size_t i = 0; i < sizeof(str); i++)
				{
					id[i] = str[i];
				}
			}
			else
			{
				volatile char* d = id;

				for(size_t i = 0; i < sizeof(str); i++)
				{
					d[i] = str[i];
				}
			}

			initialized = true;
		}

		constexpr bool push(int32_t index, char c)
		{
			lac_assert(initialized);
			lac_assert(index < up_buffer_count && index >= 0);

			buffer& buf = up[index];

			uint32_t next = (buf.write_index + 1) % buf.size;

			while(next == buf.read_index)
			{
				if(!block)
				{
					return false;
				}
				std::atomic_thread_fence(std::memory_order_seq_cst);
			}

			buf.buffer[buf.write_index] = c;
			buf.write_index				= next;
			std::atomic_thread_fence(std::memory_order_seq_cst);
			return true;
		}

		constexpr void send(int32_t index, const char* data)
		{
			lac_assert(initialized);
			lac_assert(index < up_buffer_count && index >= 0);

			buffer& buf = up[index];

			for(size_t i = 0; data[i] != '\0'; i++)
			{
				uint32_t next = (buf.write_index + 1) % buf.size;

				while(next == buf.read_index)
				{
					if(!block)
					{
						break;
					}
					std::atomic_thread_fence(std::memory_order_seq_cst);
				}

				buf.buffer[buf.write_index] = data[i];
				buf.write_index				= next;
				std::atomic_thread_fence(std::memory_order_seq_cst);
			}
		}

		constexpr size_t recv(uint32_t index, size_t size, char* data)
		{
			lac_assert(initialized);
			lac_assert(index < (uint32_t)down_buffer_count);

			buffer& buf = dn[index];

			for(size_t i = 0; i < size; i++)
			{
				if(buf.read_index == buf.write_index)
				{
					return i;
				}

				data[i]		   = buf.buffer[buf.read_index];
				buf.read_index = (buf.read_index + 1) % buf.size;
			}

			return size;
		}

		constexpr size_t pull(int32_t index, size_t size, char* data)
		{
			lac_assert(initialized);
			lac_assert(index < up_buffer_count && index >= 0);

			buffer&	 buf  = up[index];
			uint32_t read = buf.read_index;
			size_t	 n	  = 0;

			while(n < size && read != buf.write_index)
			{
				data[n++] = buf.buffer[read];
				read	  = (read + 1) % buf.size;
			}

			std::atomic_thread_fence(std::memory_order_seq_cst);
			buf.read_index = read;
			return n;
		}

		constexpr size_t space(uint32_t index) const
		{
			lac_assert(initialized);
			lac_assert(index < (uint32_t)down_buffer_count);

			const buffer& buf = dn[index];
			return (buf.read_index + buf.size - buf.write_index - 1) % buf.size;
		}

		constexpr bool put(uint32_t index, size_t size, const char* data)
		{
			lac_assert(initialized);
			lac_assert(index < (uint32_t)down_buffer_count);

			if(space(index) < size)
			{
				return false;
			}

			buffer&	 buf   = dn[index];
			uint32_t write = buf.write_index;

			for(size_t i = 0; i < size; i++)
			{
				buf.buffer[write] = data[i];
				write			  = (write + 1) % buf.size;
			}

			std::atomic_thread_fence(std::memory_order_seq_cst);
			buf.write_index = write;
			return true;
		}
	};

} // namespace lac
