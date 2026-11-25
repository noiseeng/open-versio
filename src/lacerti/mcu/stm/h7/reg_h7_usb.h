/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <utl/bitfield.h>
#include <utl/bitflag.h>

namespace lac
{
	struct reg_h7_otg_global
	{
		volatile uint32_t GOTGCTL;
		volatile uint32_t GOTGINT;
		volatile uint32_t GAHBCFG;
		volatile uint32_t GUSBCFG;
		volatile uint32_t GRSTCTL;
		volatile uint32_t GINTSTS;
		volatile uint32_t GINTMSK;
		volatile uint32_t GRXSTSR;
		volatile uint32_t GRXSTSP;
		volatile uint32_t GRXFSIZ;
		volatile uint32_t DIEPTXF0;
		volatile uint32_t HNPTXSTS;
		volatile uint32_t pad0[2];
		volatile uint32_t GCCFG;
		volatile uint32_t CID;

		using GOTGCTL_BVALOEN  = bitflag<6>;
		using GOTGCTL_BVALOVAL = bitflag<7>;
		using GOTGCTL_BSVLD	   = bitflag<19>;

		using GAHBCFG_GINTMSK = bitflag<0>;
		using GAHBCFG_TXFELVL = bitflag<7>;

		using GUSBCFG_TOCAL	 = bitfield<0, 3>;
		using GUSBCFG_PHYSEL = bitflag<6>;
		using GUSBCFG_TRDT	 = bitfield<10, 4>;
		using GUSBCFG_FHMOD	 = bitflag<29>;
		using GUSBCFG_FDMOD	 = bitflag<30>;

		using GRSTCTL_CSRST	  = bitflag<0>;
		using GRSTCTL_RXFFLSH = bitflag<4>;
		using GRSTCTL_TXFFLSH = bitflag<5>;
		using GRSTCTL_TXFNUM  = bitfield<6, 5>;
		using GRSTCTL_AHBIDL  = bitflag<31>;

		using GINT_MMIS		= bitflag<1>;
		using GINT_SOF		= bitflag<3>;
		using GINT_RXFLVL	= bitflag<4>;
		using GINT_GONAKEFF = bitflag<7>;
		using GINT_USBSUSP	= bitflag<11>;
		using GINT_USBRST	= bitflag<12>;
		using GINT_ENUMDNE	= bitflag<13>;
		using GINT_IEPINT	= bitflag<18>;
		using GINT_OEPINT	= bitflag<19>;
		using GINT_IISOIXFR = bitflag<20>;
		using GINT_WKUPINT	= bitflag<31>;

		using GRXSTS_EPNUM	= bitfield<0, 4>;
		using GRXSTS_BCNT	= bitfield<4, 11>;
		using GRXSTS_DPID	= bitfield<15, 2>;
		using GRXSTS_PKTSTS = bitfield<17, 4>;

		using GCCFG_PWRDWN = bitflag<16>;
		using GCCFG_VBDEN  = bitflag<21>;

		enum
		{
			PKTSTS_GLOBAL_OUT_NAK = 1,
			PKTSTS_OUT_DATA		  = 2,
			PKTSTS_OUT_COMPLETE	  = 3,
			PKTSTS_SETUP_COMPLETE = 4,
			PKTSTS_SETUP_DATA	  = 6
		};
	};

	struct reg_h7_otg_device
	{
		volatile uint32_t DCFG;
		volatile uint32_t DCTL;
		volatile uint32_t DSTS;
		volatile uint32_t pad0;
		volatile uint32_t DIEPMSK;
		volatile uint32_t DOEPMSK;
		volatile uint32_t DAINT;
		volatile uint32_t DAINTMSK;

		using DCFG_DSPD		= bitfield<0, 2>;
		using DCFG_NZLSOHSK = bitflag<2>;
		using DCFG_DAD		= bitfield<4, 7>;

		using DCTL_SDIS	  = bitflag<1>;
		using DCTL_CGINAK = bitflag<8>;
		using DCTL_SGONAK = bitflag<9>;
		using DCTL_CGONAK = bitflag<10>;

		using DSTS_SUSPSTS = bitflag<0>;
		using DSTS_ENUMSPD = bitfield<1, 2>;
		using DSTS_FNSOF   = bitfield<8, 14>;

		using DIEPMSK_XFRCM = bitflag<0>;
		using DIEPMSK_TOM	= bitflag<3>;
		using DOEPMSK_XFRCM = bitflag<0>;
		using DOEPMSK_STUPM = bitflag<3>;

		enum
		{
			DSPD_FULL_SPEED = 3
		};
	};

	struct reg_h7_otg_inep
	{
		volatile uint32_t CTL;
		volatile uint32_t pad0;
		volatile uint32_t INT;
		volatile uint32_t pad1;
		volatile uint32_t TSIZ;
		volatile uint32_t pad2;
		volatile uint32_t DTXFSTS;
		volatile uint32_t pad3;

		using CTL_MPSIZ	  = bitfield<0, 11>;
		using CTL_USBAEP  = bitflag<15>;
		using CTL_EPTYP	  = bitfield<18, 2>;
		using CTL_STALL	  = bitflag<21>;
		using CTL_TXFNUM  = bitfield<22, 4>;
		using CTL_CNAK	  = bitflag<26>;
		using CTL_SNAK	  = bitflag<27>;
		using CTL_SD0PID  = bitflag<28>;
		using CTL_SODDFRM = bitflag<29>;
		using CTL_EPDIS	  = bitflag<30>;
		using CTL_EPENA	  = bitflag<31>;

		using INT_XFRC	 = bitflag<0>;
		using INT_EPDISD = bitflag<1>;
		using INT_TOC	 = bitflag<3>;

		using TSIZ_XFRSIZ = bitfield<0, 19>;
		using TSIZ_PKTCNT = bitfield<19, 10>;

		enum
		{
			MPSIZ0_64 = 0,
			MPSIZ0_32 = 1,
			MPSIZ0_16 = 2,
			MPSIZ0_8  = 3
		};
	};

	struct reg_h7_otg_outep
	{
		volatile uint32_t CTL;
		volatile uint32_t pad0;
		volatile uint32_t INT;
		volatile uint32_t pad1;
		volatile uint32_t TSIZ;
		volatile uint32_t pad2;
		volatile uint32_t pad3;
		volatile uint32_t pad4;

		using CTL_MPSIZ	  = bitfield<0, 11>;
		using CTL_USBAEP  = bitflag<15>;
		using CTL_EPTYP	  = bitfield<18, 2>;
		using CTL_STALL	  = bitflag<21>;
		using CTL_CNAK	  = bitflag<26>;
		using CTL_SNAK	  = bitflag<27>;
		using CTL_SD0PID  = bitflag<28>;
		using CTL_SODDFRM = bitflag<29>;
		using CTL_EPDIS	  = bitflag<30>;
		using CTL_EPENA	  = bitflag<31>;

		using INT_XFRC	 = bitflag<0>;
		using INT_EPDISD = bitflag<1>;
		using INT_STUP	 = bitflag<3>;

		using TSIZ_XFRSIZ  = bitfield<0, 19>;
		using TSIZ_PKTCNT  = bitfield<19, 10>;
		using TSIZ_STUPCNT = bitfield<29, 2>;
	};

	struct reg_h7_usb
	{
		reg_h7_otg_global* global() { return (reg_h7_otg_global*)((uint8_t*)this + 0x000); }
		reg_h7_otg_device* device() { return (reg_h7_otg_device*)((uint8_t*)this + 0x800); }

		reg_h7_otg_inep*  inep(uint32_t ep) { return (reg_h7_otg_inep*)((uint8_t*)this + 0x900 + ep * 0x20); }
		reg_h7_otg_outep* outep(uint32_t ep) { return (reg_h7_otg_outep*)((uint8_t*)this + 0xB00 + ep * 0x20); }

		volatile uint32_t* dieptxf(uint32_t n) { return (volatile uint32_t*)((uint8_t*)this + 0x100 + n * 4); }

		volatile uint32_t* fifo(uint32_t ep) { return (volatile uint32_t*)((uint8_t*)this + 0x1000 + ep * 0x1000); }
		volatile uint32_t* pcgcctl() { return (volatile uint32_t*)((uint8_t*)this + 0xE00); }
	};
} // namespace lac
