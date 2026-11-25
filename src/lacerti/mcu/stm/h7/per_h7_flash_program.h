/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

namespace lac
{
	template <typename flash_t, typename step_t>
	static inline bool flash_program_range(uint32_t addr, const uint8_t *src, uint32_t len, step_t step)
	{
		const uint32_t total_words = (len + 31u) / 32u;

		for(uint32_t w = 0; w < total_words; w++)
		{
			uint32_t word[8];
			for(uint32_t i = 0; i < 8; i++)
			{
				const uint32_t b  = w * 32u + i * 4u;
				const uint8_t  b0 = (b + 0u < len) ? src[b + 0u] : 0xFFu;
				const uint8_t  b1 = (b + 1u < len) ? src[b + 1u] : 0xFFu;
				const uint8_t  b2 = (b + 2u < len) ? src[b + 2u] : 0xFFu;
				const uint8_t  b3 = (b + 3u < len) ? src[b + 3u] : 0xFFu;
				word[i]			  = (uint32_t)b0 | ((uint32_t)b1 << 8) | ((uint32_t)b2 << 16) | ((uint32_t)b3 << 24);
			}

			const bool last	 = (w == total_words - 1u);
			const bool flush = last && (len % 32u != 0u);

			if(!flash_t::program_word256_at(addr + w * 32u, word, flush))
			{
				return false;
			}
			step(w);
		}

		return flash_t::verify(addr, src, len);
	}
} // namespace lac
