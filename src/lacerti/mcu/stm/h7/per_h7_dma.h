/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <utl/bits.h>
#include <utl/bug.h>

#include "reg_h7_dma.h"
#include "reg_h7_dmamux.h"

namespace lac
{
	enum dma_wait_result
	{
		DMA_WAIT_OK,
		DMA_WAIT_ERR_TRANSFER,
		DMA_WAIT_ERR_DIRECT_MODE,
		DMA_WAIT_ERR_FIFO,
		DMA_WAIT_ERR_TIMEOUT
	};

	template <uint32_t base>
	struct per_h7_dma
	{
		static reg_h7_dma *reg() { return (reg_h7_dma *)base; }

		static void init(uint32_t stream_id, uint32_t memory, uint32_t peripheral, uint32_t size)
		{
			auto *dma = reg();

			lac_assert(dma);

			auto *stream = dma->get_stream(stream_id);

			lac_assert(stream);

			dma->LIFCR = 0x3FUL << (stream_id & 0x1FU);
			stream->clear_dbm();
			stream->NDTR = size;
			stream->PAR	 = peripheral;
			stream->M0AR = memory;
		}

		static reg_h7_dma_stream *get_stream(uint32_t idx) { return reg()->get_stream(idx); }
	};

	template <typename mcu_t,
			  dma_stream_id	 stream_id,
			  uint32_t		 request,
			  uint32_t		 align_size,
			  dma_dir		 direction,
			  bool			 per_flow  = false,
			  dma_burst_size per_burst = DMA_BURST_1,
			  dma_burst_size mem_burst = DMA_BURST_1>
	struct per_h7_dma_stream
	{
		using adr_t = mcu_t::adr_t;
		using isv_t = mcu_t::isv_t;
		using irq_t = mcu_t::irq_t;

		constexpr static bool	  valid_stream	= mcu_t::is_valid_dma_stream(stream_id);
		constexpr static bool	  active_stream = stream_id != DMA_STREAM_NONE;
		constexpr static uint32_t base			= mcu_t::get_dma_from_stream(stream_id);
		constexpr static uint32_t mux_base		= adr_t::DMAMUX1;
		constexpr static dma_dir  DIRECTION		= direction;
		constexpr static bool	  valid_base	= base == adr_t::DMA1 || base == adr_t::DMA2;
		constexpr static uint32_t stream_index	= stream_id - mcu_t::get_dma_first_stream(base);

		static_assert(valid_stream, "invalid dma stream");

		using dma_t = per_h7_dma<base>;

		constexpr static bool is_valid() { return valid_stream; }

		constexpr static reg_h7_dma_stream *stream() { return dma_t::get_stream(stream_index); }

		constexpr static reg_h7_dmamux *mux() { return (reg_h7_dmamux *)mux_base; }

		static bool is_irq_error() { return dma_t::reg()->is_irq_error(stream_index); }
		static bool is_irq_half() { return dma_t::reg()->is_irq_half(stream_index); }
		static bool is_irq_full() { return dma_t::reg()->is_irq_full(stream_index); }
		static bool is_irq_fifo_error() { return dma_t::reg()->is_irq_fifo_error(stream_index); }

		static uint32_t get_stream_shift()
		{
			static const uint8_t idx_to_shift[] = {0, 6, 16, 22};

			return idx_to_shift[stream_id & 0x3];
		}

		static volatile uint32_t &get_isr_ref()
		{
			if(stream_index > 3)
			{
				return dma_t::reg()->HISR;
			}
			else
			{
				return dma_t::reg()->LISR;
			}
		}

		static volatile uint32_t &get_ifcr_ref()
		{
			if(stream_index > 3)
			{
				return dma_t::reg()->HIFCR;
			}
			else
			{
				return dma_t::reg()->LIFCR;
			}
		}

		static uint32_t get_isr() { return get_isr_ref(); }
		static void		set_ifcr(uint32_t x) { get_ifcr_ref() = x; }
		static void		clr_ifcr() { set_ifcr(0x3F << get_stream_shift()); }

		static void init(bool circular)
		{
			if(stream_id == DMA_STREAM_NONE)
			{
				return;
			}

			mcu_t::template enable_clock<base>();

			reg_h7_dma_stream *dmas = stream();

			dmas->disable();
			dmas->stall();

			const uint32_t circ_flag = circular ? reg_h7_dma_stream::CR_CIRC::BIT : 0;

			const uint32_t cr_clear
				= reg_h7_dma_stream::CR_MBURST::MSK | reg_h7_dma_stream::CR_PBURST::MSK | reg_h7_dma_stream::CR_PL::MSK
				  | reg_h7_dma_stream::CR_MSIZE::MSK | reg_h7_dma_stream::CR_PSIZE::MSK | reg_h7_dma_stream::CR_MINC::BIT
				  | reg_h7_dma_stream::CR_PINC::BIT | reg_h7_dma_stream::CR_CIRC::BIT | reg_h7_dma_stream::CR_DIR::MSK
				  | reg_h7_dma_stream::CR_CT::BIT | reg_h7_dma_stream::CR_DBM::BIT | reg_h7_dma_stream::CR_PFCTRL::BIT;
			const uint32_t cr_set = reg_h7_dma_stream::CR_DIR::val(DIRECTION) | reg_h7_dma_stream::CR_MINC::BIT
									| reg_h7_dma_stream::CR_TRBUFF::BIT | reg_h7_dma_stream::CR_MSIZE::val(align_size / 2)
									| reg_h7_dma_stream::CR_PSIZE::val(align_size / 2) | circ_flag
									| reg_h7_dma_stream::CR_PL::val(DMA_PL_HIGH) | reg_h7_dma_stream::CR_MBURST::val(mem_burst)
									| reg_h7_dma_stream::CR_PBURST::val(per_burst)
									| reg_h7_dma_stream::CR_PFCTRL::val(per_flow);

			bits::reg_clrset(dmas->CR, cr_clear, cr_set);

			reg_h7_dma_stream::FCR_DMDIS::clr(dmas->FCR);
			reg_h7_dma_stream::FCR_FTH::set(dmas->FCR, 0);

			mux()->set_dmareq(stream_id, request);
			mux()->clr_csr(stream_id);
		}

		static void init_mem2mem()
		{
			if(stream_id == DMA_STREAM_NONE)
			{
				return;
			}

			mcu_t::template enable_clock<base>();

			reg_h7_dma_stream *dmas = stream();

			dmas->disable();
			dmas->stall();

			const uint32_t cr_clear
				= reg_h7_dma_stream::CR_MBURST::MSK | reg_h7_dma_stream::CR_PBURST::MSK | reg_h7_dma_stream::CR_PL::MSK
				  | reg_h7_dma_stream::CR_MSIZE::MSK | reg_h7_dma_stream::CR_PSIZE::MSK | reg_h7_dma_stream::CR_MINC::BIT
				  | reg_h7_dma_stream::CR_PINC::BIT | reg_h7_dma_stream::CR_CIRC::BIT | reg_h7_dma_stream::CR_DIR::MSK
				  | reg_h7_dma_stream::CR_CT::BIT | reg_h7_dma_stream::CR_DBM::BIT | reg_h7_dma_stream::CR_PFCTRL::BIT;

			bits::reg_clrset(dmas->CR, cr_clear, reg_h7_dma_stream::mem2mem_cr(align_size));

			dmas->FCR = reg_h7_dma_stream::mem2mem_fcr();

			mux()->set_dmareq(stream_id, h7_dmamux1_request::MUX1_MEM2MEM);
			mux()->clr_csr(stream_id);
		}

		template <class mem_word_t>
		static void start_mem2mem(mem_word_t *src, mem_word_t *dst, uint32_t len)
		{
			lac_assert(stream_id != DMA_STREAM_NONE);
			lac_assert(src);
			lac_assert(dst);

			auto *s = stream();

			s->disable();
			s->wait_en();

			is_irq_error();

			lac_assert(!is_irq_error());

			s->PAR = (uint32_t)src;
			lac_assert(s->PAR == (uint32_t)src);
			s->M0AR = (uint32_t)dst;
			lac_assert(s->M0AR == (uint32_t)dst);
			s->NDTR = len;
			lac_assert(s->NDTR == len);

			clr_ifcr();

			const uint32_t cr_clear = reg_h7_dma_stream::CR_TCIE::BIT | reg_h7_dma_stream::CR_TEIE::BIT
									  | reg_h7_dma_stream::CR_DMEIE::BIT | reg_h7_dma_stream::CR_HTIE::BIT;

			bits::reg_clrset(stream()->CR, cr_clear, 0u);

			reg_h7_dma_stream::CR_DBM::clr(stream()->CR);

			mux()->set_cfr(stream_id);

			s->enable();

			lac_assert(!is_irq_error());
		}

		static dma_wait_result wait_complete()
		{
			lac_assert(stream_id != DMA_STREAM_NONE);

			if(stream_id == DMA_STREAM_NONE)
			{
				return DMA_WAIT_ERR_TIMEOUT;
			}

			constexpr uint32_t FEIF_BIT	 = 1u << 0;
			constexpr uint32_t DMEIF_BIT = 1u << 2;
			constexpr uint32_t TEIF_BIT	 = 1u << 3;
			constexpr uint32_t TCIF_BIT	 = 1u << 5;
			constexpr uint32_t MAX_POLLS = 10'000'000;

			const uint32_t shift = get_stream_shift();

			for(uint32_t poll = 0; poll < MAX_POLLS; poll++)
			{
				const uint32_t bits = (get_isr() >> shift) & 0x3F;

				if(bits & TEIF_BIT)
				{
					clr_ifcr();
					return DMA_WAIT_ERR_TRANSFER;
				}

				if(bits & DMEIF_BIT)
				{
					clr_ifcr();
					return DMA_WAIT_ERR_DIRECT_MODE;
				}

				if(bits & FEIF_BIT)
				{
					clr_ifcr();
					return DMA_WAIT_ERR_FIFO;
				}

				if(bits & TCIF_BIT)
				{
					clr_ifcr();
					return DMA_WAIT_OK;
				}
			}

			return DMA_WAIT_ERR_TIMEOUT;
		}

		template <class per_word_t, class mem_word_t>
		static void start(per_word_t *par, mem_word_t *mem, uint32_t len, bool irq_enable = false)
		{
			lac_assert(stream_id != DMA_STREAM_NONE);
			lac_assert(par);

			auto *s = stream();

			s->disable();
			s->wait_en();

			is_irq_error();

			lac_assert(!is_irq_error());

			s->PAR = (uint32_t)par;
			lac_assert(s->PAR == (uint32_t)par);
			s->M0AR = (uint32_t)mem;
			lac_assert(s->M0AR == (uint32_t)mem);
			s->NDTR = len;
			lac_assert(s->NDTR == len);

			if(irq_enable)
			{
				mcu_t::get_nvic()->enable_irq(mcu_t::get_dma_irq(stream_id), 1);
			}

			clr_ifcr();

			reg_h7_dma_stream::CR_DBM::clr(stream()->CR);

			mux()->set_cfr(stream_id);

			const uint32_t cr_clear = reg_h7_dma_stream::CR_TCIE::BIT | reg_h7_dma_stream::CR_TEIE::BIT
									  | reg_h7_dma_stream::CR_DMEIE::BIT | reg_h7_dma_stream::CR_HTIE::BIT;
			const uint32_t cr_set	= irq_enable ? (reg_h7_dma_stream::CR_TCIE::BIT | reg_h7_dma_stream::CR_TEIE::BIT
													| reg_h7_dma_stream::CR_DMEIE::BIT | reg_h7_dma_stream::CR_HTIE::BIT)
												 : 0;

			bits::reg_clrset(stream()->CR, cr_clear, cr_set);

			s->enable();

			lac_assert(!is_irq_error());
		}

		static uint32_t get_remain() { return stream()->get_remain(); }
	};
}; // namespace lac
