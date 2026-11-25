/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <utl/bitfield.h>
#include <utl/bitflag.h>

namespace lac
{
	struct reg_h7_flash
	{
		volatile uint32_t ACR;
		volatile uint32_t KEYR1;
		volatile uint32_t OPTKEYR;
		volatile uint32_t CR1;
		volatile uint32_t SR1;
		volatile uint32_t CCR1;
		volatile uint32_t OPTCR;
		volatile uint32_t OPTSR_CUR;
		volatile uint32_t OPTSR_PRG;
		volatile uint32_t OPTCCR;
		volatile uint32_t PRAR_CUR1;
		volatile uint32_t PRAR_PRG1;
		volatile uint32_t SCAR_CUR1;
		volatile uint32_t SCAR_PRG1;
		volatile uint32_t WPSN_CUR1;
		volatile uint32_t WPSN_PRG1;
		volatile uint32_t BOOT_CUR;
		volatile uint32_t BOOT_PRG;
		volatile uint32_t RESERVED0[2];
		volatile uint32_t CRCCR1;
		volatile uint32_t CRCSADD1;
		volatile uint32_t CRCEADD1;
		volatile uint32_t CRCDATA;
		volatile uint32_t ECC_FA1;
		volatile uint32_t RESERVED1[40];
		volatile uint32_t KEYR2;
		volatile uint32_t RESERVED2;
		volatile uint32_t CR2;
		volatile uint32_t SR2;
		volatile uint32_t CCR2;
		volatile uint32_t RESERVED3[4];
		volatile uint32_t PRAR_CUR2;
		volatile uint32_t PRAR_PRG2;
		volatile uint32_t SCAR_CUR2;
		volatile uint32_t SCAR_PRG2;
		volatile uint32_t WPSN_CUR2;
		volatile uint32_t WPSN_PRG2;
		volatile uint32_t RESERVED4[4];
		volatile uint32_t CRCCR2;
		volatile uint32_t CRCSADD2;
		volatile uint32_t CRCEADD2;
		volatile uint32_t CRCDATA2;
		volatile uint32_t ECC_FA2;

		using ACR_LATENCY	 = bitfield<0, 4>;
		using ACR_WRHIGHFREQ = bitfield<4, 2>;

		using KEYR1_KEY1R = bitfield<0, 32>;

		using OPTKEYR_OPTKEYR = bitfield<0, 32>;

		using CR1_LOCK1		  = bitflag<0>;
		using CR1_PG1		  = bitflag<1>;
		using CR1_SER1		  = bitflag<2>;
		using CR1_BER1		  = bitflag<3>;
		using CR1_PSIZE1	  = bitfield<4, 2>;
		using CR1_FW1		  = bitflag<6>;
		using CR1_START1	  = bitflag<7>;
		using CR1_SNB1		  = bitfield<8, 3>;
		using CR1_CRC_EN	  = bitflag<15>;
		using CR1_EOPIE1	  = bitflag<16>;
		using CR1_WRPERRIE1	  = bitflag<17>;
		using CR1_PGSERRIE1	  = bitflag<18>;
		using CR1_STRBERRIE1  = bitflag<19>;
		using CR1_INCERRIE1	  = bitflag<21>;
		using CR1_OPERRIE1	  = bitflag<22>;
		using CR1_RDPERRIE1	  = bitflag<23>;
		using CR1_RDSERRIE1	  = bitflag<24>;
		using CR1_SNECCERRIE1 = bitflag<25>;
		using CR1_DBECCERRIE1 = bitflag<26>;
		using CR1_CRCENDIE1	  = bitflag<27>;
		using CR1_CRCRDERRIE1 = bitflag<28>;

		using SR1_BSY1		= bitflag<0>;
		using SR1_WBNE1		= bitflag<1>;
		using SR1_QW1		= bitflag<2>;
		using SR1_CRC_BUSY1 = bitflag<3>;
		using SR1_EOP1		= bitflag<16>;
		using SR1_WRPERR1	= bitflag<17>;
		using SR1_PGSERR1	= bitflag<18>;
		using SR1_STRBERR1	= bitflag<19>;
		using SR1_INCERR1	= bitflag<21>;
		using SR1_OPERR1	= bitflag<22>;
		using SR1_RDPERR1	= bitflag<23>;
		using SR1_RDSERR1	= bitflag<24>;
		using SR1_SNECCERR1 = bitflag<25>;
		using SR1_DBECCERR1 = bitflag<26>;
		using SR1_CRCEND1	= bitflag<27>;

		using CCR1_CLR_EOP1		 = bitflag<16>;
		using CCR1_CLR_WRPERR1	 = bitflag<17>;
		using CCR1_CLK_PGSERR1	 = bitflag<18>;
		using CCR1_CLR_STRBERR1	 = bitflag<19>;
		using CCR1_CLR_INCERR1	 = bitflag<21>;
		using CCR1_CLR_OPERR1	 = bitflag<22>;
		using CCR1_CLR_RDPERR1	 = bitflag<23>;
		using CCR1_CLR_RDSERR1	 = bitflag<24>;
		using CCR1_CLR_SNECCERR1 = bitflag<25>;
		using CCR1_CLR_DBECCERR1 = bitflag<26>;
		using CCR1_CLR_CRCEND1	 = bitflag<27>;

		using OPTCR1_OPTLOCK		= bitflag<0>;
		using OPTCR1_OPTSTART		= bitflag<1>;
		using OPTCR1_MER			= bitflag<4>;
		using OPTCR1_OPTCHANGEERRIE = bitflag<30>;
		using OPTCR1_SWAP_BANK		= bitflag<31>;

		using OPTSR_CUR_OPT_BUSY	  = bitflag<0>;
		using OPTSR_CUR_BOR_LEV		  = bitfield<2, 2>;
		using OPTSR_CUR_IWDG1_SW	  = bitflag<4>;
		using OPTSR_CUR_NRST_STOP_D1  = bitflag<6>;
		using OPTSR_CUR_NRST_STDY_D1  = bitflag<7>;
		using OPTSR_CUR_RDP			  = bitfield<8, 8>;
		using OPTSR_CUR_IWDG_FZ_STOP  = bitflag<17>;
		using OPTSR_CUR_IWDG_FZ_SDBY  = bitflag<18>;
		using OPTSR_CUR_ST_RAM_SIZE	  = bitfield<19, 2>;
		using OPTSR_CUR_SECURITY	  = bitflag<21>;
		using OPTSR_CUR_BOOT_CM4	  = bitflag<22>;
		using OPTSR_CUR_BOOT_CM7	  = bitflag<23>;
		using OPTSR_CUR_IO_HSLV		  = bitflag<29>;
		using OPTSR_CUR_OPTCHANGEERR  = bitflag<30>;
		using OPTSR_CUR_SWAP_BANK_OPT = bitflag<31>;

		using OPTSR_PRG_BOR_EV		  = bitfield<2, 2>;
		using OPTSR_PRG_IWDG1_SW	  = bitflag<4>;
		using OPTSR_PRG_NRST_STOP_D1  = bitflag<6>;
		using OPTSR_PRG_NRST_STDY_D1  = bitflag<7>;
		using OPTSR_PRG_RDP			  = bitfield<8, 8>;
		using OPTSR_PRG_IWDG_FZ_STOP  = bitflag<17>;
		using OPTSR_PRG_IWDG_FZ_SDBY  = bitflag<18>;
		using OPTSR_PRG_ST_RAM_SIZE	  = bitfield<19, 2>;
		using OPTSR_PRG_SECURITY	  = bitflag<21>;
		using OPTSR_PRG_BOOT_CM4	  = bitflag<22>;
		using OPTSR_PRG_BOOT_CM7	  = bitflag<23>;
		using OPTSR_PRG_IO_HSLV		  = bitflag<29>;
		using OPTSR_PRG_SWAP_BANK_OPT = bitflag<31>;

		using OPTCCR_CLK_OPTCHANGEERR = bitflag<30>;

		using PRAR_CUR1_PROT_AREA_START1 = bitfield<0, 12>;
		using PRAR_CUR1_PROT_AREA_END1	 = bitfield<16, 12>;
		using PRAR_CUR1_DMEP1			 = bitflag<31>;

		using PRAR_PRG1_PROT_AREA_START1 = bitfield<0, 12>;
		using PRAR_PRG1_PROT_AREA_END1	 = bitfield<16, 12>;
		using PRAR_PRG1_DMEP1			 = bitflag<31>;

		using SCAR_CUR1_SEC_AREA_START1 = bitfield<0, 12>;
		using SCAR_CUR1_SEC_AREA_END1	= bitfield<16, 12>;
		using SCAR_CUR1_DMES1			= bitflag<31>;

		using SCAR_PRG1_SEC_AREA_START1 = bitfield<0, 12>;
		using SCAR_PRG1_SEC_AREA_END1	= bitfield<16, 12>;
		using SCAR_PRG1_DMES1			= bitflag<31>;

		using WPSN_CUR1R_WRPSN1 = bitfield<0, 8>;

		using WPSN_PRG1R_WRPSN1 = bitfield<0, 8>;

		using BOOT_CUR_BOOT_ADD = bitfield<0, 32>;

		using BOOT_PRG_BOOT_ADD = bitfield<0, 32>;

		using CRCCR1_CRC_SECT	 = bitfield<0, 3>;
		using CRCCR1_ALL_BANK	 = bitflag<7>;
		using CRCCR1_CRC_BY_SECT = bitflag<8>;
		using CRCCR1_ADD_SECT	 = bitflag<9>;
		using CRCCR1_CLEAN_SECT	 = bitflag<10>;
		using CRCCR1_SRART_CRC	 = bitflag<16>;
		using CRCCR1_CLEAN_CRC	 = bitflag<17>;
		using CRCCR1_CRC_BURST	 = bitfield<20, 2>;

		using CRCSADD1R_CRC_START_ADDR = bitfield<2, 18>;

		using CRCEADD1R_CRC_END_ADDR = bitfield<2, 18>;

		using CRCDATAR_CRC_DATA = bitfield<0, 32>;

		using ECC_FA1R_FAIL_ECC_ADDR1 = bitfield<0, 15>;

		using KEYR2_KEY2R = bitfield<0, 32>;

		using CR2_LOCK2		  = bitflag<0>;
		using CR2_PG2		  = bitflag<1>;
		using CR2_SER2		  = bitflag<2>;
		using CR2_BER2		  = bitflag<3>;
		using CR2_PSIZE2	  = bitfield<4, 2>;
		using CR2_FW2		  = bitflag<6>;
		using CR2_START2	  = bitflag<7>;
		using CR2_SNB2		  = bitfield<8, 3>;
		using CR2_SPSS2		  = bitflag<14>;
		using CR2_CRC_EN	  = bitflag<15>;
		using CR2_EOPIE2	  = bitflag<16>;
		using CR2_WRPERRIE2	  = bitflag<17>;
		using CR2_PGSERRIE2	  = bitflag<18>;
		using CR2_STRBERRIE2  = bitflag<19>;
		using CR2_INCERRIE2	  = bitflag<21>;
		using CR2_OPERRIE2	  = bitflag<22>;
		using CR2_RDPERRIE2	  = bitflag<23>;
		using CR2_RDSERRIE2	  = bitflag<24>;
		using CR2_SNECCERRIE2 = bitflag<25>;
		using CR2_DBECCERRIE2 = bitflag<26>;
		using CR2_CRCENDIE2	  = bitflag<27>;
		using CR2_CRCRDERRIE2 = bitflag<28>;

		using SR2_BSY2		= bitflag<0>;
		using SR2_WBNE2		= bitflag<1>;
		using SR2_QW2		= bitflag<2>;
		using SR2_CRC_BUSY2 = bitflag<3>;
		using SR2_EOP2		= bitflag<16>;
		using SR2_WRPERR2	= bitflag<17>;
		using SR2_PGSERR2	= bitflag<18>;
		using SR2_STRBERR2	= bitflag<19>;
		using SR2_INCERR2	= bitflag<21>;
		using SR2_OPERR2	= bitflag<22>;
		using SR2_RDPERR2	= bitflag<23>;
		using SR2_RDSERR2	= bitflag<24>;
		using SR2_SNECCERR2 = bitflag<25>;
		using SR2_DBECCERR2 = bitflag<26>;
		using SR2_CRCEND2	= bitflag<27>;

		using CCR2_CLR_EOP2		 = bitflag<16>;
		using CCR2_CLR_WRPERR2	 = bitflag<17>;
		using CCR2_CLR_PGSERR2	 = bitflag<18>;
		using CCR2_CLR_STRBERR2	 = bitflag<19>;
		using CCR2_CLR_INCERR2	 = bitflag<21>;
		using CCR2_CLR_OPERR2	 = bitflag<22>;
		using CCR2_CLR_RDPERR2	 = bitflag<23>;
		using CCR2_CLR_RDSERR2	 = bitflag<24>;
		using CCR2_CLR_SNECCERR2 = bitflag<25>;
		using CCR2_CLR_DBECCERR2 = bitflag<26>;
		using CCR2_CLR_CRCEND2	 = bitflag<27>;

		using PRAR_CUR2_PROT_AREA_START2 = bitfield<0, 12>;
		using PRAR_CUR2_PROT_AREA_END2	 = bitfield<16, 12>;
		using PRAR_CUR2_DMEP2			 = bitflag<31>;

		using PRAR_PRG2_PROT_AREA_START2 = bitfield<0, 12>;
		using PRAR_PRG2_PROT_AREA_END2	 = bitfield<16, 12>;
		using PRAR_PRG2_DMEP2			 = bitflag<31>;

		using SCAR_CUR2_SEC_AREA_START2 = bitfield<0, 12>;
		using SCAR_CUR2_SEC_AREA_END2	= bitfield<16, 12>;
		using SCAR_CUR2_DMES2			= bitflag<31>;

		using SCAR_PRG2_SEC_AREA_START2 = bitfield<0, 12>;
		using SCAR_PRG2_SEC_AREA_END2	= bitfield<16, 12>;
		using SCAR_PRG2_DMES2			= bitflag<31>;

		using WPSN_CUR2_WRPSN2 = bitfield<0, 8>;

		using WPSN_PRG2_WRPSN2 = bitfield<0, 8>;

		using CRCCR2_CRC_SECT	 = bitfield<0, 3>;
		using CRCCR2_ALL_BANK	 = bitflag<7>;
		using CRCCR2_CRC_BY_SECT = bitflag<8>;
		using CRCCR2_ADD_SECT	 = bitflag<9>;
		using CRCCR2_CLEAN_SECT	 = bitflag<10>;
		using CRCCR2_START_CRC	 = bitflag<16>;
		using CRCCR2_CLEAN_CRC	 = bitflag<17>;
		using CRCCR2_CRC_BURST	 = bitfield<20, 2>;

		using CRCSADD2R_CRC_START_ADDR = bitfield<2, 18>;

		using CRCEADD2R_CRC_END_ADDR = bitfield<2, 18>;

		using EDD_FA2R_FAIL_ECC_ADDR2 = bitfield<0, 15>;

		void set_latency(uint32_t latency, uint32_t wrhighfreq)
		{
			ACR = ACR_LATENCY::val(latency) | ACR_WRHIGHFREQ::val(wrhighfreq);
			while(ACR_LATENCY::get(ACR) != latency) {}
		}

		bool boot_cm4() const { return OPTSR_CUR_BOOT_CM4::get(OPTSR_CUR) != 0; }
		bool boot_cm7() const { return OPTSR_CUR_BOOT_CM7::get(OPTSR_CUR) != 0; }

		void clear_boot_cm4()
		{
			while(OPTSR_CUR_OPT_BUSY::get(OPTSR_CUR)) {}
			OPTKEYR = 0x08192a3b;
			OPTKEYR = 0x4c5d6e7f;
			OPTSR_PRG_BOOT_CM4::clr(OPTSR_PRG);
			OPTCR1_OPTSTART::set(OPTCR);
			while(OPTSR_CUR_OPT_BUSY::get(OPTSR_CUR)) {}
		}

		void set_boot_cm4()
		{
			while(OPTSR_CUR_OPT_BUSY::get(OPTSR_CUR)) {}
			OPTKEYR = 0x08192a3b;
			OPTKEYR = 0x4c5d6e7f;
			OPTSR_PRG_BOOT_CM4::set(OPTSR_PRG);
			OPTCR1_OPTSTART::set(OPTCR);
			while(OPTSR_CUR_OPT_BUSY::get(OPTSR_CUR)) {}
		}

		void set_boot_cm7()
		{
			while(OPTSR_CUR_OPT_BUSY::get(OPTSR_CUR)) {}
			OPTKEYR = 0x08192a3b;
			OPTKEYR = 0x4c5d6e7f;
			OPTSR_PRG_BOOT_CM7::set(OPTSR_PRG);
			OPTCR1_OPTSTART::set(OPTCR);
			while(OPTSR_CUR_OPT_BUSY::get(OPTSR_CUR)) {}
		}
	};
} // namespace lac