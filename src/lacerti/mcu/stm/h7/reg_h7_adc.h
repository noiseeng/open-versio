/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <atomic>
#include <cstdint>
#include <utl/bitarray.h>
#include <utl/bitfield.h>
#include <utl/bitflag.h>
#include <utl/bits.h>
#include <utl/bug.h>

#define ADC_STAB_DLY_US 10

namespace lac
{
	struct reg_h7_adc_common
	{
		volatile uint32_t CSR;
		volatile uint32_t RESERVED;
		volatile uint32_t CCR;
		volatile uint32_t CDR;
		volatile uint32_t CDR2;

		void set_clockdiv(uint32_t prescale) { bits::reg_clrset(CCR, 15 << 18, (prescale & 15) << 18); }
	};

	struct reg_h7_adc
	{
		volatile uint32_t ISR;
		volatile uint32_t IER;
		volatile uint32_t CR;
		volatile uint32_t CFGR;
		volatile uint32_t CFGR2;
		volatile uint32_t SMPR1;
		volatile uint32_t SMPR2;
		volatile uint32_t PCSEL;
		volatile uint32_t LTR1;
		volatile uint32_t HTR1;
		volatile uint32_t RESERVED1;
		volatile uint32_t RESERVED2;
		volatile uint32_t SQR[4];
		volatile uint32_t DR;
		volatile uint32_t RESERVED3;
		volatile uint32_t RESERVED4;
		volatile uint32_t JSQR;
		volatile uint32_t RESERVED5[4];
		volatile uint32_t OFR[4];
		volatile uint32_t RESERVED6[4];
		volatile uint32_t JDR[4];
		volatile uint32_t RESERVED7[4];
		volatile uint32_t AWD2CR;
		volatile uint32_t AWD3CR;
		volatile uint32_t RESERVED8;
		volatile uint32_t RESERVED9;
		volatile uint32_t LTR2;
		volatile uint32_t HTR2;
		volatile uint32_t LTR3;
		volatile uint32_t HTR3;
		volatile uint32_t DIFSEL;
		volatile uint32_t CALFACT;
		volatile uint32_t CALFACT2;

		using ISR_LDORDY = bitflag<12>;
		using ISR_JQOVF	 = bitflag<10>;
		using ISR_AWD	 = bitfield<7, 3>;
		using ISR_AWD1	 = bitflag<7>;
		using ISR_JEOS	 = bitflag<6>;
		using ISR_JEOC	 = bitflag<5>;
		using ISR_OVR	 = bitflag<4>;
		using ISR_EOS	 = bitflag<3>;
		using ISR_EOC	 = bitflag<2>;
		using ISR_EOSMP	 = bitflag<1>;
		using ISR_ADRDY	 = bitflag<0>;

		using IER_JQOVFIE = bitflag<10>;
		using IER_AWDIE	  = bitfield<7, 3>;
		using IER_JEOSIE  = bitflag<6>;
		using IER_JEOCIE  = bitflag<5>;
		using IER_OVRIE	  = bitflag<4>;
		using IER_EOSIE	  = bitflag<3>;
		using IER_EOCIE	  = bitflag<2>;
		using IER_EOSMPIE = bitflag<1>;
		using IER_ADRDYIE = bitflag<0>;

		using CR_ADCAL		= bitflag<31>;
		using CR_ADCALDIF	= bitflag<30>;
		using CR_DEEPPWD	= bitflag<29>;
		using CR_ADVREGEN	= bitflag<28>;
		using CR_LINCALRDYW = bitfield<22, 6>;
		using CR_ADCALLIN	= bitflag<16>;
		using CR_BOOST		= bitflag<8>;
		using CR_JADSTP		= bitflag<5>;
		using CR_ADSTP		= bitflag<4>;
		using CR_JADSTART	= bitflag<3>;
		using CR_ADSTART	= bitflag<2>;
		using CR_ADDIS		= bitflag<1>;
		using CR_ADEN		= bitflag<0>;

		using CFGR_JQDIS   = bitflag<31>;
		using CFGR_AWD1CH  = bitfield<26, 5>;
		using CFGR_JAUTO   = bitflag<25>;
		using CFGR_JAWD1EN = bitflag<24>;
		using CFGR_AWD1EN  = bitflag<23>;
		using CFGR_AWD1SGL = bitflag<22>;
		using CFGR_JQM	   = bitflag<21>;
		using CFGR_JDISCEN = bitflag<20>;
		using CFGR_DISCNUM = bitfield<17, 3>;
		using CFGR_DISCEN  = bitflag<16>;
		using CFGR_AUTDLY  = bitflag<14>;
		using CFGR_CONT	   = bitflag<13>;
		using CFGR_OVRMOD  = bitflag<12>;
		using CFGR_EXTEN   = bitfield<10, 2>;
		using CFGR_EXTSEL  = bitfield<5, 5>;
		using CFGR_RES	   = bitfield<2, 3>;
		using CFGR_DMNGT   = bitfield<0, 2>;

		using CFGR2_LSHIFT = bitfield<28, 4>;
		using CFGR2_OSVR   = bitfield<16, 10>;
		using CFGR2_RSHIFT = bitfield<11, 4>;
		using CFGR2_ROVSM  = bitflag<10>;
		using CFGR2_TROVS  = bitflag<9>;
		using CFGR2_OVSS   = bitfield<5, 4>;
		using CFGR2_JOVSE  = bitflag<1>;
		using CFGR2_ROVSE  = bitflag<0>;

		using SMPR1_SMPR = bitarray<10, 3>;

		using SMPR2_SMPR = bitarray<10, 3>;

		using PCSEL_PCSEL = bitfield<0, 20>;

		using LTR1_LTR1 = bitfield<0, 26>;

		using HTR1_HTR1 = bitfield<0, 26>;

		using SQR1_L   = bitfield<0, 4>;
		using SQR1_SQ1 = bitfield<6, 5>;
		using SQR1_SQ2 = bitfield<12, 5>;
		using SQR1_SQ3 = bitfield<18, 5>;
		using SQR1_SQ4 = bitfield<24, 5>;

		using SQR2_SQ5 = bitfield<0, 5>;
		using SQR2_SQ6 = bitfield<6, 5>;
		using SQR2_SQ7 = bitfield<12, 5>;
		using SQR2_SQ8 = bitfield<18, 5>;
		using SQR2_SQ9 = bitfield<24, 5>;

		using SQ3_SQ10 = bitfield<0, 5>;
		using SQ3_SQ11 = bitfield<6, 5>;
		using SQ3_SQ12 = bitfield<12, 5>;
		using SQ3_SQ13 = bitfield<18, 5>;
		using SQ3_SQ14 = bitfield<24, 5>;

		using SQ4_SQ15 = bitfield<0, 5>;
		using SQ4_SQ16 = bitfield<6, 5>;

		using DR_RDATA = bitfield<0, 32>;

		using JSQR_JL	   = bitfield<0, 2>;
		using JSQR_JEXTSEL = bitfield<2, 5>;
		using JSQR_JEXTEN  = bitfield<7, 2>;
		using JSQR_JSQ1	   = bitfield<9, 5>;
		using JSQR_JSQ2	   = bitfield<15, 5>;
		using JSQR_JSQ3	   = bitfield<21, 5>;
		using JSQR_JSQ4	   = bitfield<27, 5>;

		using OFRY_OFFSETY	  = bitfield<0, 26>;
		using OFRY_OFFSETY_CH = bitfield<26, 5>;
		using OFRY_SSATE	  = bitflag<31>;

		using JDRY_JDATA = bitfield<0, 32>;

		using AWD2CR_AWD2CH = bitfield<0, 20>;

		using AWD3CR_AWD3CH = bitfield<0, 20>;

		using LTR2_LTR2 = bitfield<0, 26>;

		using HTR2_HTR2 = bitfield<0, 26>;

		using LTR3_LTR3 = bitfield<0, 26>;

		using HTR3_HTR3 = bitfield<0, 26>;

		using DIFSEL_DIFSEL = bitfield<0, 20>;

		using CALFACT_CALFACT_S = bitfield<0, 11>;
		using CALFACT_CALFACT_D = bitfield<16, 11>;

		using CALFACT2_LINCALFACT = bitfield<0, 30>;

		volatile uint32_t& get_smpr(uint32_t channel_idx) { return channel_idx < 10 ? SMPR1 : SMPR2; }

		volatile uint32_t& get_sqr(uint32_t channel_idx)
		{
			if(channel_idx < 4)
			{
				return SQR[0];
			}
			else
			{
				return SQR[(channel_idx - 4) / 5 + 1];
			}
		}

		constexpr static uint32_t get_sqr_shift(uint32_t channel_idx)
		{
			if(channel_idx <= 3)
			{
				return 6 * (channel_idx + 1);
			}
			else
			{
				return 6 * ((channel_idx - 4) % 5);
			}
		}

		void set_eosie() { IER_EOSIE::set(IER); }

		void clr_eosie() { IER_EOSIE::clr(IER); }

		void clear_eos() { ISR_EOS::set(ISR); }

		void power_up()
		{
			CR = 0;
			CR_ADVREGEN::set(CR);
		}

		void set_boost() { CR_BOOST::set(CR); }

		void enable()
		{
			while(!ISR_ADRDY::get(ISR))
			{
				CR_ADEN::set(CR);
			}
		}

		void start() { CR_ADSTART::set(CR); }

		void channel(uint32_t channel_idx, uint32_t seq_idx, uint32_t sample_time)
		{
			const uint32_t channel_flag = 1 << channel_idx;

			bits::reg_set(PCSEL, channel_flag);

			const uint32_t smpr_shift = 3 * (channel_idx > 10 ? channel_idx - 10 : channel_idx);
			const uint32_t smpr_mask  = 7 << smpr_shift;
			const uint32_t smpr_value = sample_time << smpr_shift;

			bits::reg_clrset(get_smpr(channel_idx), smpr_mask, smpr_value);

			const uint32_t sqr_shift = get_sqr_shift(seq_idx);
			const uint32_t sqr_mask	 = 63 << sqr_shift;
			const uint32_t sqr_value = channel_idx << sqr_shift;

			bits::reg_clrset(get_sqr(seq_idx), sqr_mask, sqr_value);
		}

		void set_sequence_count(uint32_t count) { bits::reg_clrset(SQR[0], 15, (count - 1) & 15); }

		void continuous_dma(uint32_t count)
		{
			CFGR_CONT::set(CFGR);
			CFGR_DMNGT::set(CFGR, 3);
			set_sequence_count(count);
		}

		void set_resolution(uint32_t res)
		{
			uint32_t bits = 0;
			switch(res)
			{
				case 16: bits = 0; break;
				case 14: bits = 1; break;
				case 12: bits = 2; break;
				case 10: bits = 3; break;
				default: lac_assert(false); break;
			}

			CFGR_RES::set(CFGR, bits);
		}

		void set_oversample(uint32_t ovr, int32_t shift)
		{
			CFGR2_OSVR::set(CFGR2, ovr);

			if(shift > 0)
			{
				CFGR2_OVSS::set(CFGR2, shift);
				CFGR2_LSHIFT::set(CFGR2, 0);
			}
			else
			{
				CFGR2_OVSS::set(CFGR2, 0);
				CFGR2_LSHIFT::set(CFGR2, -shift);
			}

			CFGR2_ROVSE::set(CFGR2);
		}

		template <typename timer_t>
		void calibrate()
		{
			lac_assert(!CR_ADEN::get(CR));

			CR_DEEPPWD::clr(CR);
			CR_ADVREGEN::set(CR);
			timer_t::delay_us(2 * ADC_STAB_DLY_US);

			CR_ADCALDIF::clr(CR);
			CR_ADCALLIN::set(CR);
			CR_ADCAL::set(CR);

			timer_t::delay_ms(100);

			while(CR_ADCAL::get(CR))
			{
				std::atomic_thread_fence(std::memory_order_seq_cst);
			}
		}
	};

} // namespace lac