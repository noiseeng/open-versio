/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <mcu/stm/h7/reg_h7_dma.h>
#include <mcu/stm/h7/reg_h7_dmamux.h>
#include <utl/bug.h>

#include <array>
#include <cstdint>
#include <cstring>

#include "per_h7_dma.h"
#include "reg_h7_sai.h"

namespace lac
{

	enum sai_dir
	{
		SAI_DIR_TX,
		SAI_DIR_RX,
	};

	enum sai_slave
	{
		SAI_SLAVE_A,
		SAI_SLAVE_B,
	};

	template <typename gpio_mclk,
			  typename gpio_sck,
			  typename gpio_fs,
			  sai_dir dir_a,
			  typename gpio_a,
			  sai_dir dir_b,
			  typename gpio_b,
			  sai_slave slave,
			  bool		i2s>
	struct per_h7_sai_pins
	{
		using pin_mclk_t = gpio_mclk;
		using pin_sck_t	 = gpio_sck;
		using pin_fs_t	 = gpio_fs;
		using pin_a_t	 = gpio_a;
		using pin_b_t	 = gpio_b;

		constexpr static sai_dir   DIR_A = dir_a;
		constexpr static sai_dir   DIR_B = dir_b;
		constexpr static sai_slave SLAVE = slave;
		constexpr static bool	   I2S	 = i2s;

		static void init()
		{
			pin_mclk_t::init_out();
			pin_sck_t::init_out();
			pin_fs_t::init_out();
			pin_a_t::init_out();
			pin_b_t::init_out();
		}
	};

	template <typename mcu_t,
			  uint32_t port,
			  uint32_t mck_div,
			  uint32_t data_size_bytes,
			  typename pin_t,
			  typename sai_reg_t = reg_h7_sai>
	struct per_h7_sai
	{
		using rcc_t = mcu_t::rcc_t;
		using blk_t = sai_reg_t::block_t;

		const static uint32_t BASE = port;

		constexpr static sai_reg_t* reg() { return (sai_reg_t*)port; }
		constexpr static blk_t*		reg_a() { return reg()->blocks + 0; }
		constexpr static blk_t*		reg_b() { return reg()->blocks + 1; }

		constexpr static bool is_a_rx() { return pin_t::DIR_A == SAI_DIR_RX; }
		constexpr static bool is_a_slave() { return pin_t::SLAVE == SAI_SLAVE_A; }

		static uint32_t get_gcr(uint32_t sync_ext, uint32_t sync)
		{
			uint32_t gcr = 0;

			switch(sync_ext)
			{
				case SYNCEXT_OUTBLOCKA_ENABLE: gcr = sai_reg_t::GCR_SYNCOUT::val(1); break;
				case SYNCEXT_OUTBLOCKB_ENABLE: gcr = sai_reg_t::GCR_SYNCOUT::val(2); break;
			}

			switch(sync)
			{
				case SAI_SYNC_EXT_SAI2: gcr |= sai_reg_t::GCR_SYNCIN::val(1); break;
				case SAI_SYNC_EXT_SAI3: gcr |= sai_reg_t::GCR_SYNCIN::val(2); break;
				case SAI_SYNC_EXT_SAI4: gcr |= sai_reg_t::GCR_SYNCIN::val(3); break;
			}

			return gcr;
		}

		static uint32_t get_syncen(uint32_t sync)
		{
			uint32_t syncen = 0;

			switch(sync)
			{
				case SAI_SYNC: syncen = blk_t::CR1_SYNCEN::val(1); break;
				case SAI_SYNC_EXT_SAI1:
				case SAI_SYNC_EXT_SAI2:
				case SAI_SYNC_EXT_SAI3:
				case SAI_SYNC_EXT_SAI4: syncen = blk_t::CR1_SYNCEN::val(2); break;
			}

			return syncen;
		}

		static uint32_t get_ckstr(uint32_t is_slave, uint32_t strobe)
		{
			if(!is_slave)
			{
				return (strobe == SAI_CLKSTROBING_RISINGEDGE) ? 0U : blk_t::CR1_CKSTR::BIT;
			}
			else
			{
				return (strobe == SAI_CLKSTROBING_RISINGEDGE) ? blk_t::CR1_CKSTR::BIT : 0U;
			}
		}

		static uint32_t get_slotsize()
		{
			switch(data_size_bytes)
			{
				case 1: return blk_t::SLOTR_SLOTSZ::val(SAI_SLOTSZ_16B);
				case 2: return blk_t::SLOTR_SLOTSZ::val(SAI_SLOTSZ_16B);
				case 3: return blk_t::SLOTR_SLOTSZ::val(SAI_SLOTSZ_32B);
				case 4: return blk_t::SLOTR_SLOTSZ::val(SAI_SLOTSZ_32B);
				default: bug::abort(); blk_t::SLOTR_SLOTSZ::val(SAI_SLOTSZ_16B);
			}
		}

		static uint32_t get_datasize()
		{
			switch(data_size_bytes)
			{
				case 1: return blk_t::CR1_DS::val(SAI_DATASZ_8);
				case 2: return blk_t::CR1_DS::val(SAI_DATASZ_16);
				case 3: return blk_t::CR1_DS::val(SAI_DATASZ_24);
				case 4: return blk_t::CR1_DS::val(SAI_DATASZ_32);
				default:
					bug::abort();
					return blk_t::CR1_DS::val(SAI_DATASZ_32);
					;
			}
		}

		static uint32_t get_frl() { return 2 * data_size_bytes * 8 - 1; }

		static uint32_t get_fsall() { return data_size_bytes * 8 - 1; }

		static void init_i2s_master(sai_reg_t* sai, blk_t* saiblk, bool is_slave, bool is_rx)
		{
			using blk = blk_t;

			saiblk->disable();

			const uint32_t sync = is_slave ? SAI_SYNC : SAI_ASYNC;

			const uint32_t syncen_bits = get_syncen(sync);
			const uint32_t mode_bits   = (is_slave ? blk::CR1_MODE::val(2) : 0) | (is_rx ? blk::CR1_MODE::val(1) : 0);

			sai->GCR = 0;

			const uint32_t mck_bits = is_slave ? 0u : blk::mck_enable_bits();

			const uint32_t cr1
				= mode_bits | get_datasize() | blk::CR1_CKSTR::BIT | syncen_bits | blk::CR1_MCKDIV::val(mck_div) | mck_bits;
			const uint32_t cr2 = 0;

			const uint32_t frcr_i2s = blk::FRCR_FSOFF::BIT | blk::FRCR_FSDEF::BIT;
			const uint32_t frcr_fb	= blk::FRCR_FSPOL::BIT | blk::FRCR_FSDEF::BIT;
			const uint32_t frcr
				= blk::FRCR_FRL::val(get_frl()) | blk::FRCR_FSALL::val(get_fsall()) | (pin_t::I2S ? frcr_i2s : frcr_fb);

			const uint32_t slotr = get_slotsize() | blk::SLOTR_SLOTEN::MSK | blk::SLOTR_NBSLOT::val(1);

			saiblk->CR1	  = cr1;
			saiblk->CR2	  = cr2;
			saiblk->FRCR  = frcr;
			saiblk->SLOTR = slotr;

			lac_assert(!blk::CR1_SAIEN::get(saiblk->CR1));
		}

		static uint32_t get_rx_fifo_level()
		{
			if constexpr(is_a_rx())
			{
				return reg_a()->get_fifo_level();
			}
			else
			{
				return reg_b()->get_fifo_level();
			}
		}

		static void flush_fifo()
		{
			reg_a()->flush_fifo();
			reg_b()->flush_fifo();
		}

		static void init()
		{
			mcu_t::template enable_clock<port>();

			pin_t::init();

			disable();

			init_i2s_master(reg(), reg_a(), is_a_slave(), is_a_rx());
			init_i2s_master(reg(), reg_b(), !is_a_slave(), !is_a_rx());

			lac_assert(!blk_t::CR1_SAIEN::get(reg_a()->CR1));
			lac_assert(!blk_t::CR1_SAIEN::get(reg_b()->CR1));
		}

		static void enable()
		{
			if(is_a_slave())
			{
				reg_a()->enable();
				reg_b()->enable();
			}
			else
			{
				reg_b()->enable();
				reg_a()->enable();
			}
		}

		static void disable()
		{
			if(is_a_slave())
			{
				reg_b()->disable();
				reg_a()->disable();
			}
			else
			{
				reg_a()->disable();
				reg_b()->disable();
			}
		}

		static uint32_t get_isr_flag_a() { return 0; }

		static uint32_t get_isr_flag_b() { return blk_t::IMR_OVRUDRIE::BIT | blk_t::IMR_WCKCFGIE::BIT; }
	};

	using audio_stream_callback = void (*)(uint32_t word_count, const int32_t* in_buffer, int32_t* out_buffer);

	template <typename mcu_t,
			  typename sai_type,
			  dma_stream_id dma_rx_stream,
			  dma_stream_id dma_tx_stream,
			  uint32_t		dma_word_count>
	struct per_h7_sai_dma
	{
		using this_type = per_h7_sai_dma<mcu_t, sai_type, dma_rx_stream, dma_tx_stream, dma_word_count>;

		using dma_rx = per_h7_dma_stream<mcu_t,
										 dma_rx_stream,
										 mcu_t::get_sai_dma_req(sai_type::BASE, sai_type::is_a_rx()),
										 sizeof(int32_t),
										 DMA_PER2MEM>;

		using dma_tx = per_h7_dma_stream<mcu_t,
										 dma_tx_stream,
										 mcu_t::get_sai_dma_req(sai_type::BASE, !sai_type::is_a_rx()),
										 sizeof(int32_t),
										 DMA_MEM2PER>;

		const static uint32_t DMA_WORD_COUNT = dma_word_count;
		const static uint32_t CB_WORD_COUNT	 = dma_word_count / 2;

		const static uint32_t DMA_FLG_FEIF0_4  = (uint32_t)0x00000001U;
		const static uint32_t DMA_FLG_DMEIF0_4 = (uint32_t)0x00000004U;
		const static uint32_t DMA_FLG_TEIF0_4  = (uint32_t)0x00000008U;
		const static uint32_t DMA_FLG_HTIF0_4  = (uint32_t)0x00000010U;
		const static uint32_t DMA_FLG_TCIF0_4  = (uint32_t)0x00000020U;

		struct dma_buffer
		{
			const static uint32_t FENCE_MAGIC = 0x12345678;

			uint32_t fence0 = FENCE_MAGIC;

			std::array<int32_t, DMA_WORD_COUNT> a = {0};

			uint32_t fence1 = FENCE_MAGIC;

			std::array<int32_t, DMA_WORD_COUNT> b = {0};

			uint32_t fence2 = FENCE_MAGIC;

			void sanity()
			{
				lac_assert(fence0 == FENCE_MAGIC);
				lac_assert(fence1 == FENCE_MAGIC);
				lac_assert(fence2 == FENCE_MAGIC);
			}
		};

		dma_buffer& buffer;

		audio_stream_callback callback = 0;

		static this_type* singleton(this_type* s = 0)
		{
			static this_type* instance = 0;

			if(s)
			{
				lac_assert(!instance);
				instance = s;
			}
			else
			{
				lac_assert(instance);
			}
			return instance;
		}

		void init(audio_stream_callback cb)
		{
			lac_assert(cb);
			singleton(this);
			lac_assert(singleton(0));

			dma_rx::init(true);
			dma_tx::init(true);

			sai_type::disable();

			sai_type::init();

			callback = cb;

			buffer.sanity();

			sai_type::flush_fifo();

			setup_dma((uint8_t*)buffer.a.data(), DMA_WORD_COUNT, (uint8_t*)buffer.b.data(), DMA_WORD_COUNT);

			sai_type::enable();

			sai_type::reg_a()->enable_dma();
			sai_type::reg_b()->enable_dma();
		}

		static bool get_it_source(uint32_t flag)
		{
			if(flag == reg_h7_dma_stream::FCR_FEIE::BIT)
			{
				return dma_rx::stream()->FCR & flag;
			}
			else
			{
				return dma_rx::stream()->CR & flag;
			}
		}

		void setup_dma(uint8_t* rdata, uint32_t rsize, uint8_t* tdata, uint32_t tsize)
		{
			mcu_t::set_isv(mcu_t::get_dma_isv(dma_rx_stream), tx_isr);

			auto* ba = sai_type::reg_a();
			auto* bb = sai_type::reg_b();

			auto* rdr = sai_type::is_a_rx() ? &ba->DR : &bb->DR;
			auto* tdr = sai_type::is_a_rx() ? &bb->DR : &ba->DR;

			dma_rx::start(rdr, rdata, rsize, true);
			dma_tx::start(tdr, tdata, rsize, false);

			ba->IMR = 0;
			bb->IMR = 0;
		}

		void irq(bool half)
		{
			buffer.sanity();
			uint32_t ofs = half ? 0 : CB_WORD_COUNT;
			callback(CB_WORD_COUNT, (const int32_t*)buffer.a.data() + ofs, (int32_t*)buffer.b.data() + ofs);
			buffer.sanity();
		}

		static void loopback(uint32_t count, const int32_t* in, int32_t* out) { memcpy(out, in, sizeof(int32_t) * count); }

		static void silence(uint32_t count, const int32_t* in, int32_t* out) { std::memset(out, 0, sizeof(int32_t) * count); }

		void mute()
		{
			callback = silence;
			buffer.b.fill(0);
		}

		void saw(uint32_t count, const int32_t* in, int32_t* out)
		{
			static uint32_t t  = 0;
			uint32_t		dt = 1 << 16;

			for(uint32_t i = 0; i < count / 2; i++)
			{
				t = (t + dt);

				out[2 * i + 0] = t;
				out[2 * i + 1] = t;
			}
		}

		static void tx_isr()
		{
			const uint32_t tmpisr_dma = dma_rx::get_isr();
			const uint32_t isrshift	  = dma_rx::get_stream_shift();
			const uint32_t errors	  = DMA_FLG_TEIF0_4 | DMA_FLG_FEIF0_4 | DMA_FLG_DMEIF0_4;

			if(tmpisr_dma & (errors << isrshift))
			{
				if(get_it_source(reg_h7_dma_stream::CR_TEIE::BIT))
				{
					bug::abort();
				}
			}

			if(tmpisr_dma & (DMA_FLG_HTIF0_4 << isrshift))
			{
				if(get_it_source(reg_h7_dma_stream::CR_HTIE::BIT))
				{
					dma_rx::clr_ifcr();
					singleton()->irq(true);
				}
			}

			if(tmpisr_dma & (DMA_FLG_TCIF0_4 << isrshift))
			{
				if(get_it_source(reg_h7_dma_stream::CR_TCIE::BIT) != 0U)
				{
					dma_rx::clr_ifcr();
					singleton()->irq(false);
				}
			}

			dma_rx::clr_ifcr();
		}
	};

} // namespace lac