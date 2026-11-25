/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace lac
{
	struct mcu_arm_v7m_fault
	{
		static constexpr uint32_t FRAME_LR	 = 5;
		static constexpr uint32_t FRAME_PC	 = 6;
		static constexpr uint32_t FRAME_XPSR = 7;

		static constexpr uint32_t FRAME_WORDS	 = 8;
		static constexpr uint32_t FRAME_FP_WORDS = 26;

		static constexpr uint32_t EXC_RETURN_SPSEL = 1u << 2;
		static constexpr uint32_t EXC_RETURN_FTYPE = 1u << 4;

		static constexpr uint32_t XPSR_IPSR	 = 0x1FFu;
		static constexpr uint32_t XPSR_ALIGN = 1u << 9;
		static constexpr uint32_t XPSR_T	 = 1u << 24;

		static constexpr uint32_t TRACE_MAX = 16;

		static constexpr bool is_exc_return(uint32_t w)
		{
			switch(w)
			{
				case 0xFFFFFFE1:
				case 0xFFFFFFE9:
				case 0xFFFFFFED:
				case 0xFFFFFFF1:
				case 0xFFFFFFF9:
				case 0xFFFFFFFD: return true;
				default: return false;
			}
		}

		static constexpr uint32_t frame_words(uint32_t exc_return, uint32_t xpsr)
		{ return ((exc_return & EXC_RETURN_FTYPE) ? FRAME_WORDS : FRAME_FP_WORDS) + ((xpsr & XPSR_ALIGN) ? 1u : 0u); }

		static constexpr uint32_t ipsr(uint32_t xpsr) { return xpsr & XPSR_IPSR; }

		static void redirect(uint32_t* frame, void (*to)())
		{
			frame[FRAME_PC]	  = (uint32_t)to & ~1u;
			frame[FRAME_XPSR] = XPSR_T | (frame[FRAME_XPSR] & (XPSR_IPSR | XPSR_ALIGN));
		}

		static bool follows_call(uint32_t ret, uint32_t text_lo, uint32_t text_hi)
		{
			const uint32_t at = ret & ~1u;

			if((ret & 1u) == 0 || at < text_lo + 4 || at > text_hi)
			{
				return false;
			}

			const auto* half = (const volatile uint16_t*)(at - 4);

			const bool bl  = (half[0] & 0xF800u) == 0xF000u && (half[1] & 0xD000u) == 0xD000u;
			const bool blx = (half[1] & 0xFF87u) == 0x4780u;

			return bl || blx;
		}

		template <typename cb_t>
		static void trace(const uint32_t* frame,
						  uint32_t		  exc_return,
						  uint32_t		  stack_lo,
						  uint32_t		  stack_hi,
						  uint32_t		  text_lo,
						  uint32_t		  text_hi,
						  cb_t&&		  cb)
		{
			uint32_t at = (uint32_t)frame;

			if((exc_return & EXC_RETURN_SPSEL) != 0 || at < stack_lo || at + 4 * FRAME_WORDS > stack_hi)
			{
				return;
			}

			at += 4 * frame_words(exc_return, frame[FRAME_XPSR]);

			uint32_t count = 0;

			while(at + 4 <= stack_hi && count < TRACE_MAX)
			{
				const uint32_t w = *(const volatile uint32_t*)at;
				at += 4;

				if(is_exc_return(w))
				{
					if(at + 4 * FRAME_WORDS > stack_hi)
					{
						return;
					}

					const auto* interrupted = (const volatile uint32_t*)at;
					cb(interrupted[FRAME_PC], true);
					count++;
					at += 4 * frame_words(w, interrupted[FRAME_XPSR]);
				}
				else if(follows_call(w, text_lo, text_hi))
				{
					cb(w, false);
					count++;
				}
			}
		}
	};
} // namespace lac
