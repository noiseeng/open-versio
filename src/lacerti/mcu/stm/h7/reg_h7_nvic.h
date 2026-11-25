/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <utl/bitarray.h>
#include <utl/bug.h>

#include <atomic>
#include <cstdint>

namespace lac
{
	struct reg_h7_nvic
	{
		constexpr static uint32_t __NVIC_PRIO_BITS = 4;

		volatile uint32_t		ISER[8];
		const volatile uint32_t RESERVED0[24];
		volatile uint32_t		ICER[8];
		const volatile uint32_t RSERVED1[24];
		volatile uint32_t		ISPR[8];
		const volatile uint32_t RESERVED2[24];
		volatile uint32_t		ICPR[8];
		const volatile uint32_t RESERVED3[24];
		volatile uint32_t		IABR[8];
		const volatile uint32_t RESERVED4[56];
		volatile uint8_t		IP[240];
		const volatile uint32_t RESERVED5[644];
		volatile uint32_t		STIR;

		using IP_PRI_N = bitarray<4, 8>;

		template <typename irq_t>
		void enable_irq(irq_t irqt)
		{
			uint32_t irq   = (uint32_t)irqt;
			ISER[irq >> 5] = (1 << (irq & 0x1F));
			std::atomic_thread_fence(std::memory_order_seq_cst);
		}

		template <typename irq_t>
		void disable_irq(irq_t irqt)
		{
			uint32_t irq   = (uint32_t)irqt;
			ICER[irq >> 5] = (1 << (irq & 0x1F));
			std::atomic_thread_fence(std::memory_order_seq_cst);
		}

		template <typename irq_t>
		void clear_irq(irq_t irqt)
		{
			uint32_t irq   = (uint32_t)irqt;
			ICPR[irq >> 5] = (1 << (irq & 0x1F));
			std::atomic_thread_fence(std::memory_order_seq_cst);
		}

		template <typename irq_t>
		void enable_irq(irq_t irqt, uint32_t priority)
		{
			uint32_t irq = (uint32_t)irqt;
			lac_assert(irq < 240);
			const uint32_t p = (priority << (8U - __NVIC_PRIO_BITS)) & 0xFF;
			IP[irq]			 = (uint8_t)p;
			enable_irq(irq);
		}
	};
} // namespace lac