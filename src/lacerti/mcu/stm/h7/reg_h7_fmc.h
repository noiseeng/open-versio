/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstddef>
#include <cstdint>
#include <utl/bitfield.h>
#include <utl/bitflag.h>
#include <utl/bug.h>

namespace lac
{
	struct reg_h7_fmc
	{
		volatile uint32_t BCR1;
		volatile uint32_t BTR1;
		volatile uint32_t BCR2;
		volatile uint32_t BTR2;
		volatile uint32_t BCR3;
		volatile uint32_t BTR3;
		volatile uint32_t BCR4;
		volatile uint32_t BTR4;

		uint32_t PAD_B1[16];

		volatile uint32_t PCR2;
		volatile uint32_t SR2;
		volatile uint32_t PMEM2;
		volatile uint32_t PATT2;
		volatile uint32_t RESERVED0;
		volatile uint32_t ECCR2;

		uint32_t PAD_B2[2];

		volatile uint32_t PCR;
		volatile uint32_t SR;
		volatile uint32_t PMEM;
		volatile uint32_t PATT;
		volatile uint32_t ECCR;

		uint32_t PAD_B3[28];

		volatile uint32_t BWTR1;
		volatile uint32_t PAD1;
		volatile uint32_t BWTR2;
		volatile uint32_t PAD2;
		volatile uint32_t BWTR3;
		volatile uint32_t PAD3;
		volatile uint32_t BWTR4;
		volatile uint32_t PAD4;

		uint32_t PAD_B4[7];

		volatile uint32_t SDCR1;
		volatile uint32_t SDCR2;
		volatile uint32_t SDTR1;
		volatile uint32_t SDTR2;
		volatile uint32_t SDCMR;
		volatile uint32_t SDRTR;
		volatile uint32_t SDSR;

		using BCR_MBKEN		= bitflag<0>;
		using BCR_MUXEN		= bitflag<1>;
		using BCR_MTYP		= bitfield<2, 2>;
		using BCR_MWID		= bitfield<4, 2>;
		using BCR_FACCEN	= bitflag<6>;
		using BCR_BURSTEN	= bitflag<8>;
		using BCR_WAITPOL	= bitflag<9>;
		using BCR_WAITCFG	= bitflag<11>;
		using BCR_WREN		= bitflag<12>;
		using BCR_WAITEN	= bitflag<13>;
		using BCR_EXTMOD	= bitflag<14>;
		using BCR_ASYNCWAIT = bitflag<15>;
		using BCR_CPSIZE	= bitfield<16, 3>;
		using BCR_CBURSTRW	= bitflag<19>;
		using BCR_CCLKEN	= bitflag<20>;
		using BCR_WFDIS		= bitflag<21>;
		using BCR_BMAP		= bitfield<24, 2>;
		using BCR_FMCEN		= bitflag<31>;

		using BTR_ADDSET  = bitfield<0, 4>;
		using BTR_ADDHLD  = bitfield<4, 4>;
		using BTR_DATAST  = bitfield<8, 8>;
		using BTR_BUSTURN = bitfield<16, 4>;
		using BTR_CLKDIV  = bitfield<20, 4>;
		using BTR_DATLAT  = bitfield<24, 4>;
		using BTR_ACCMOD  = bitfield<28, 2>;

		using BWTR_ADDSET  = bitfield<0, 4>;
		using BWTR_ADDHLD  = bitfield<4, 4>;
		using BWTR_DATAST  = bitfield<8, 8>;
		using BWTR_BUSTURN = bitfield<16, 4>;
		using BWTR_ACCMOD  = bitfield<28, 2>;

		using PCR_PWAITEN = bitflag<1>;
		using PCR_PBKEN	  = bitflag<2>;
		using PCR_PWID	  = bitfield<4, 2>;
		using PCR_ECCEN	  = bitflag<6>;
		using PCR_TLCR	  = bitfield<9, 4>;
		using PCR_TAR	  = bitfield<13, 4>;
		using PCR_ECCPS	  = bitfield<17, 3>;

		using SR_IRS   = bitflag<0>;
		using SR_ILS   = bitflag<1>;
		using SR_IFS   = bitflag<2>;
		using SR_IREN  = bitflag<3>;
		using SR_ILEN  = bitflag<4>;
		using SR_IFEN  = bitflag<5>;
		using SR_FEMPT = bitflag<6>;

		using PMEM_MEMSET  = bitfield<0, 8>;
		using PMEM_MEMWAIT = bitfield<8, 8>;
		using PMEM_MEMHOLD = bitfield<16, 8>;
		using PMEM_MEMHIZ  = bitfield<24, 8>;

		using PATT_ATTSET  = bitfield<0, 8>;
		using PATT_ATTWAIT = bitfield<8, 8>;
		using PATT_ATTHOLD = bitfield<16, 8>;
		using PATT_ATTHIZ  = bitfield<24, 8>;

		using SDCR_NC	  = bitfield<0, 2>;
		using SDCR_NR	  = bitfield<2, 2>;
		using SDCR_MWID	  = bitfield<4, 2>;
		using SDCR_NB	  = bitflag<6>;
		using SDCR_CAS	  = bitfield<7, 2>;
		using SDCR_WP	  = bitflag<9>;
		using SDCR_SDCLK  = bitfield<10, 2>;
		using SDCR_RBURST = bitflag<12>;
		using SDCR_RPIPE  = bitfield<13, 2>;

		using SDTR_TMRD = bitfield<0, 4>;
		using SDTR_TXSR = bitfield<4, 4>;
		using SDTR_TRAS = bitfield<8, 4>;
		using SDTR_TRC	= bitfield<12, 4>;
		using SDTR_TWR	= bitfield<16, 4>;
		using SDTR_TRP	= bitfield<20, 4>;
		using SDTR_TRCD = bitfield<24, 4>;

		using SDCMR_MODE = bitfield<0, 3>;
		using SDCMR_CTB2 = bitflag<3>;
		using SDCMR_CTB1 = bitflag<4>;
		using SDCMR_NRFS = bitfield<5, 3>;
		using SDCMR_MRD	 = bitfield<9, 14>;

		using SDRTR_CRE	  = bitflag<0>;
		using SDRTR_COUNT = bitfield<1, 13>;
		using SDRTR_REIE  = bitflag<14>;

		using SDSR_RE	  = bitflag<0>;
		using SDSR_MODES1 = bitfield<1, 2>;
		using SDSR_MODES2 = bitfield<3, 2>;

		void set_bmap(uint32_t val) { BCR_BMAP::set(BCR1, val); }

		void enable() { BCR_FMCEN::set(BCR1); }

		void set_refresh_time(uint32_t count) { SDRTR_COUNT::set(SDRTR, count); }

		enum bank_flag
		{
			BANK_1 = 1,
			BANK_2 = 2,
			BANK_COUNT
		};

		enum mode
		{
			CMD_NORMAL,
			CMD_CLOCK_ENABLE,
			CMD_PALL,
			CMD_AUTO_REFRESH,
			CMD_LOAD_MODE,
			CMD_SELF_REFRESH,
			CMD_POWER_DOWN
		};

		void send(uint32_t mode, bank_flag target, uint32_t refresh, uint32_t mode_register)
		{
			uint32_t cmd = 0;

			SDCMR_MODE::set(cmd, mode);
			SDCMR_MRD::set(cmd, mode_register);
			SDCMR_NRFS::set(cmd, refresh - 1);

			if(target & BANK_1)
			{
				SDCMR_CTB1::set(cmd);
			}

			if(target & BANK_2)
			{
				SDCMR_CTB2::set(cmd);
			}

			SDCMR = SDCMR | cmd;
		}

		constexpr static uint32_t get_nc(uint32_t cols)
		{
			switch(cols)
			{
				default: lac_assert(false);
				//falls through
				case 8: return 0;
				case 9: return 1;
				case 10: return 2;
				case 11: return 3;
			}
		}

		constexpr static uint32_t get_nr(uint32_t rows)
		{
			switch(rows)
			{
				default: lac_assert(false);
				//falls through
				case 11: return 0;
				case 12: return 1;
				case 13: return 2;
			}
		}

		constexpr static uint32_t get_mwid(uint32_t bits)
		{
			switch(bits)
			{
				default: lac_assert(false);
				//falls through
				case 8: return 0;
				case 16: return 1;
				case 32: return 2;
			}
		}

		constexpr static uint32_t sdram_control(uint32_t col_bits,
												uint32_t row_bits,
												uint32_t bus_width,
												uint32_t bank_count,
												uint32_t cas_count,
												uint32_t rpipe_count,
												uint32_t clock_count,
												bool	 write_protect,
												bool	 burst)
		{
			lac_assert(rpipe_count < 3);
			lac_assert(cas_count > 0 && cas_count <= 3);
			lac_assert(bank_count == 2 || bank_count == 4);
			lac_assert(clock_count == 2 || clock_count == 3);

			return SDCR_RPIPE::val(rpipe_count) | SDCR_RBURST::val(burst) | SDCR_SDCLK::val(clock_count)
				   | SDCR_WP::val(write_protect) | SDCR_CAS::val(cas_count) | SDCR_NB::val(bank_count == 4)
				   | SDCR_MWID::val(get_mwid(bus_width)) | SDCR_NC::val(get_nc(col_bits)) | SDCR_NR::val(get_nr(row_bits));
		}

		constexpr static uint32_t
		sdram_timing(uint32_t tmrd, uint32_t txsr, uint32_t tras, uint32_t trc, uint32_t twr, uint32_t trp, uint32_t trcd)
		{
			return SDTR_TMRD::val(tmrd - 1) | SDTR_TXSR::val(txsr - 1) | SDTR_TRAS::val(tras - 1) | SDTR_TRC::val(trc - 1)
				   | SDTR_TWR::val(twr - 1) | SDTR_TRP::val(trp - 1) | SDTR_TRCD::val(trcd - 1);
		}
	};

	static_assert(offsetof(reg_h7_fmc, BCR1) == 0, "invalid offset");
	static_assert(offsetof(reg_h7_fmc, PAD_B1) == 32, "invalid offset");
	static_assert(offsetof(reg_h7_fmc, PCR2) == 0x60, "invalid offset");
	static_assert(offsetof(reg_h7_fmc, PCR) == 0x80, "invalid offset");
	static_assert(offsetof(reg_h7_fmc, BWTR1) == 0x104, "invalid offset");
	static_assert(offsetof(reg_h7_fmc, SDCR1) == 0x140, "invalid offset");
} // namespace lac
