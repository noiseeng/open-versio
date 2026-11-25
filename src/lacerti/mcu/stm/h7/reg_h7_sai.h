/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <bug/compiler.h>
#include <cstdint>

#include <utl/bitfield.h>
#include <utl/bitflag.h>
#include <utl/bits.h>
#include <utl/bug.h>

enum
{
	SYNCEXT_DISABLE,
	SYNCEXT_OUTBLOCKA_ENABLE,
	SYNCEXT_OUTBLOCKB_ENABLE
};

enum
{
	SAI_ASYNC,
	SAI_SYNC,
	SAI_SYNC_EXT_SAI1,
	SAI_SYNC_EXT_SAI2,
	SAI_SYNC_EXT_SAI3,
	SAI_SYNC_EXT_SAI4
};

enum
{
	SAI_CLKSTROBING_FALLINGEDGE,
	SAI_CLKSTROBING_RISINGEDGE
};

enum
{
	SAI_SLOTSZ_DATASIZE,
	SAI_SLOTSZ_16B,
	SAI_SLOTSZ_32B
};

enum
{
	SAI_DATASZ_RES1,
	SAI_DATASZ_RES2,
	SAI_DATASZ_8,
	SAI_DATASZ_10,
	SAI_DATASZ_16,
	SAI_DATASZ_20,
	SAI_DATASZ_24,
	SAI_DATASZ_32,
};

namespace lac
{
	struct reg_h7_sai_block
	{
		volatile uint32_t CR1;
		volatile uint32_t CR2;
		volatile uint32_t FRCR;
		volatile uint32_t SLOTR;
		volatile uint32_t IMR;
		volatile uint32_t SR;
		volatile uint32_t CLRFR;
		volatile uint32_t DR;

		using CR1_MODE	   = bitfield<0, 2>;
		using CR1_PRTCFG   = bitfield<2, 4>;
		using CR1_DS	   = bitfield<5, 3>;
		using CR1_LSBFIRST = bitflag<8>;
		using CR1_CKSTR	   = bitflag<9>;
		using CR1_SYNCEN   = bitfield<10, 2>;
		using CR1_MONO	   = bitflag<12>;
		using CR1_OUTDRIV  = bitflag<13>;
		using CR1_SAIEN	   = bitflag<16>;
		using CR1_DMAEN	   = bitflag<17>;
		using CR1_NOMCK	   = bitflag<19>;
		using CR1_MCKDIV   = bitfield<20, 6>;
		using CR1_OSR	   = bitflag<26>;

		using CR2_FTH	  = bitfield<0, 3>;
		using CR2_FFLUSH  = bitflag<3>;
		using CR2_TRIS	  = bitflag<4>;
		using CR2_MUTE	  = bitflag<5>;
		using CR2_MUTEVAL = bitflag<6>;
		using CR2_MUTECNT = bitfield<7, 6>;
		using CR2_CPL	  = bitflag<13>;
		using CR2_COMP	  = bitfield<14, 2>;

		using FRCR_FRL	 = bitfield<0, 8>;
		using FRCR_FSALL = bitfield<8, 7>;
		using FRCR_FSDEF = bitflag<16>;
		using FRCR_FSPOL = bitflag<17>;
		using FRCR_FSOFF = bitflag<18>;

		using SLOTR_FBOFF  = bitfield<0, 5>;
		using SLOTR_SLOTSZ = bitfield<6, 2>;
		using SLOTR_NBSLOT = bitfield<8, 4>;
		using SLOTR_SLOTEN = bitfield<16, 16>;

		using IMR_OVRUDRIE	= bitflag<0>;
		using IMR_MUTEDETIE = bitflag<1>;
		using IMR_WCKCFGIE	= bitflag<2>;
		using IMR_FREQIE	= bitflag<3>;
		using IMR_CNRDYIE	= bitflag<4>;
		using IMR_AFSDETIE	= bitflag<5>;
		using IMR_LFSDETIE	= bitflag<6>;

		using SR_OVRUDR	 = bitflag<0>;
		using SR_MUTEDET = bitflag<1>;
		using SR_WCKCFG	 = bitflag<2>;
		using SR_FREQ	 = bitflag<3>;
		using SR_CNRDY	 = bitflag<4>;
		using SR_AFSDET	 = bitflag<5>;
		using SR_LFSDET	 = bitflag<6>;
		using SR_FLVL	 = bitfield<16, 3>;

		using CLRFR_COVRUDR	 = bitflag<0>;
		using CLRFR_CMUTEDET = bitflag<1>;
		using CLRFR_CWCKCFG	 = bitflag<2>;
		using CLRFR_CCNRDY	 = bitflag<4>;
		using CLRFR_CAFSDET	 = bitflag<5>;
		using CLRFR_CLFSDET	 = bitflag<6>;

		constexpr static uint32_t mck_enable_bits() { return 0; }

		constexpr static uint32_t get_mclk_div(uint32_t sai_clock, uint32_t desired_rate, uint32_t word_size)
		{
			const uint32_t osample = 256;
			const uint32_t base	   = sai_clock / desired_rate;
			const uint32_t integer = base / osample;
			const uint32_t round   = (base / (osample / 2)) & 1;
			return integer + round;
		}

		constexpr static uint32_t actual_rate(uint32_t sai_clock, uint32_t sample_rate)
		{ return sai_clock / get_mclk_div(sai_clock, sample_rate, 32) / 256; }

		void enable_dma() { CR1_DMAEN::set(CR1); }

		void flush_fifo() { CR2_FFLUSH::set(CR2); }

		void enable() { CR1_SAIEN::setdly(CR1); }

		void disable()
		{
			while(CR1_SAIEN::get(CR1))
			{
				bits::reg_clrdly(CR1, CR1_SAIEN::BIT);
			}
		}

		void sanity()
		{
			const uint32_t fboff  = SLOTR_FBOFF::get(SLOTR);
			const uint32_t slotsz = SLOTR_SLOTSZ::get(SLOTR);
			const uint32_t ds	  = CR1_DS::get(CR1);
			const uint32_t nbslot = SLOTR_NBSLOT::get(SLOTR);
			const uint32_t frl	  = FRCR_FRL::get(FRCR);

			lac_assert(fboff <= (slotsz - ds));
			lac_assert(ds <= slotsz);
			lac_assert(nbslot * slotsz <= frl);
			lac_assert((frl & (frl + 1)) == 0);
			lac_assert(frl + 1 >= 8);
			lac_assert(frl + 1 <= 256);

			bug::ignore(fboff);
			bug::ignore(slotsz);
			bug::ignore(ds);
			bug::ignore(nbslot);
			bug::ignore(frl);
		}

		uint32_t get_fifo_level() { return SR_FLVL::get(SR); }

		void set_i2s()
		{
			FRCR_FSOFF::set(FRCR);
			FRCR_FSPOL::clr(FRCR);
			FRCR_FSDEF::set(FRCR);
		}
	};

	struct reg_h7_sai
	{
		using block_t = reg_h7_sai_block;

		volatile uint32_t GCR;
		reg_h7_sai_block  blocks[2];
		volatile uint32_t PDMCR;
		volatile uint32_t PDMDLY;

		using GCR_SYNCIN  = bitfield<0, 2>;
		using GCR_SYNCOUT = bitfield<4, 2>;
	};

} // namespace lac