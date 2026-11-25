/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <utl/bitfield.h>
#include <utl/bitflag.h>

namespace lac
{
	struct reg_tim
	{
		volatile uint32_t CR1;
		volatile uint32_t CR2;
		volatile uint32_t SMCR;
		volatile uint32_t DIER;
		volatile uint32_t SR;
		volatile uint32_t EGR;
		volatile uint32_t CCMR1;
		volatile uint32_t CCMR2;
		volatile uint32_t CCER;
		volatile uint32_t CNT;
		volatile uint32_t PSC;
		volatile uint32_t ARR;
		volatile uint32_t RCR;
		volatile uint32_t CCR1;
		volatile uint32_t CCR2;
		volatile uint32_t CCR3;
		volatile uint32_t CCR4;
		volatile uint32_t BDTR;
		volatile uint32_t DCR;
		volatile uint32_t DMAR;
		volatile uint32_t RESERVED1;
		volatile uint32_t CCMR3;
		volatile uint32_t CCR5;
		volatile uint32_t CCR6;
		volatile uint32_t AF1;
		volatile uint32_t AF2;
		volatile uint32_t TISEL;

		using CR1_CEN  = bitflag<0>;
		using CR1_UDIS = bitflag<1>;
		using CR1_URS  = bitflag<2>;
		using CR1_OPM  = bitflag<3>;
		using CR1_DIR  = bitflag<4>;
		using CR1_CMS  = bitfield<5, 2>;
		using CR1_ARPE = bitflag<7>;
		using CR1_CKD  = bitfield<8, 2>;

		using CR2_CCDS = bitflag<3>;
		using CR2_MMS  = bitfield<4, 3>;
		using CR2_TI1S = bitflag<7>;

		using SMCR_SMS	= bitfield<0, 3>;
		using SMCR_OCCS = bitflag<3>;
		using SMCR_TS	= bitfield<4, 3>;
		using SMCR_MSM	= bitflag<7>;
		using SMCR_ETF	= bitfield<8, 3>;
		using SMCR_ETPS = bitfield<12, 2>;
		using SMCR_ECE	= bitflag<14>;
		using SMCR_ETP	= bitflag<15>;

		using DIER_UIE	 = bitflag<0>;
		using DIER_CC1IE = bitflag<1>;
		using DIER_CC2IE = bitflag<2>;
		using DIER_CC3IE = bitflag<3>;
		using DIER_CC4IE = bitflag<4>;
		using DIER_TIE	 = bitflag<6>;
		using DIER_UDE	 = bitflag<8>;
		using DIER_CC1DE = bitflag<9>;
		using DIER_CC2DE = bitflag<10>;
		using DIER_CC3DE = bitflag<11>;
		using DIER_CC4DE = bitflag<12>;
		using DIER_TDE	 = bitflag<14>;

		using SR_UIF   = bitflag<0>;
		using SR_CC1IF = bitflag<1>;
		using SR_CC2IF = bitflag<2>;
		using SR_CC3IF = bitflag<3>;
		using SR_CC4IF = bitflag<4>;
		using SR_TIF   = bitflag<6>;
		using SR_CC1OF = bitflag<9>;
		using SR_CC2OF = bitflag<10>;
		using SR_CC3OF = bitflag<11>;
		using SR_CC4OF = bitflag<12>;

		using EGR_UG   = bitflag<0>;
		using EGR_CC1G = bitflag<1>;
		using EGR_CC2G = bitflag<2>;
		using EGR_CC3G = bitflag<3>;
		using EGR_CC4G = bitflag<4>;
		using EGR_TG   = bitflag<6>;

		using CCER_CC1E	 = bitflag<0>;
		using CCER_CC1P	 = bitflag<1>;
		using CCER_CC1NP = bitflag<3>;
		using CCER_CC2E	 = bitflag<4>;
		using CCER_CC2P	 = bitflag<5>;
		using CCER_CC2NP = bitflag<7>;
		using CCER_CC3E	 = bitflag<8>;
		using CCER_CC3P	 = bitflag<9>;
		using CCER_CC3NP = bitflag<11>;
		using CCER_CC4E	 = bitflag<12>;
		using CCER_CC4P	 = bitflag<13>;
		using CCER_CC4NP = bitflag<15>;

		using CCMR1_CC1S  = bitfield<0, 2>;
		using CCMR1_OC1FE = bitflag<2>;
		using CCMR1_OC1PE = bitflag<3>;
		using CCMR1_OC1M  = bitfield<4, 3>;
		using CCMR1_OC1M3 = bitflag<16>;
		using CCMR1_CC2S  = bitfield<8, 2>;
		using CCMR1_OC2FE = bitflag<10>;
		using CCMR1_OC2PE = bitflag<11>;
		using CCMR1_OC2M  = bitfield<12, 3>;
		using CCMR1_OC2M3 = bitflag<24>;

		using CCMR2_CC3S  = bitfield<0, 2>;
		using CCMR2_OC3FE = bitflag<2>;
		using CCMR2_OC3PE = bitflag<3>;
		using CCMR2_OC3M  = bitfield<4, 3>;
		using CCMR2_OC3M3 = bitflag<16>;
		using CCMR2_CC4S  = bitfield<8, 2>;
		using CCMR2_OC4FE = bitflag<10>;
		using CCMR2_OC4PE = bitflag<11>;
		using CCMR2_OC4M  = bitfield<12, 3>;
		using CCMR2_OC4M3 = bitflag<24>;

		using BDTR_MOE = bitflag<15>;

		using DCR_DBA = bitfield<0, 5>;
		using DCR_DBL = bitfield<8, 5>;

		void freerun()
		{
			CR1	 = 0;
			ARR	 = 0xFFFFFFFF;
			PSC	 = 0;
			SMCR = 0;
			CR2	 = 0;
			CR1_CEN::set(CR1);
			CNT = 0;

			while(!CNT)
				;
		}

		uint32_t read() const { return CNT; }
		uint32_t get_delay() const { return ARR; }
		void	 set_next_delay(uint32_t delay) { ARR = delay; }
		void	 set_fastest() { PSC = 0; }
		void	 set_trgo_update() { CR2_MMS::set(CR2, 2); }
		void	 enable_delay_update() { CR1_ARPE::set(CR1); }
		void	 clear_uif() { SR_UIF::clrdly(SR); }
		void	 set_down() { CR1_DIR::set(CR1); }
		void	 set_urs() { CR1_URS::set(CR1); }
		void	 enable_irq() { DIER_UIE::set(DIER); }
		void	 disable_irq() { DIER_UIE::clr(DIER); }
		void	 enable() { CR1_CEN::set(CR1); }
		void	 disable() { CR1_CEN::clr(CR1); }
	};
} // namespace lac