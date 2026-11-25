/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usb_defs.h"
#include "usbd.h"
#include "usbd_core.h"
#include <bug/compiler.h>
#include <stdint.h>

#pragma pack(push, 1)

namespace lac
{

	enum usbd_audio_subclass : uint8_t
	{
		USBD_AUDIO_UNDEFINED	  = 0,
		USBD_AUDIO_AUDIOCONTROL	  = 1,
		USBD_AUDIO_AUDIOSTREAMING = 2,
		USBD_AUDIO_MIDISTREAMING  = 3,
	};

	enum usbd_audio_cs : uint8_t
	{
		USBD_AUDIO_CS_UNDEFINED		= 0x20,
		USBD_AUDIO_CS_DEVICE		= 0x21,
		USBD_AUDIO_CS_CONFIGURATION = 0x22,
		USBD_AUDIO_CS_STRING		= 0x23,
		USBD_AUDIO_CS_INTERFACE		= 0x24,
		USBD_AUDIO_CS_ENDPOINT		= 0x25,
	};

	struct usbd_audio_standard_unused
	{
		constexpr static uint32_t SIZE = usbd_interface::SIZE;

		usbd_interface iface = {
			.bInterfaceNumber	= 0,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 0,
			.bInterfaceClass	= 1,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 0,
			.iInterface			= USBD_STRID_IFACE0,
		};
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_audio_standard_unused);

	struct usbd_audio_class
	{
		const static uint32_t SIZE = 9;

		usb_header head = {SIZE, USBD_DESC_CS_INTERFACE};

		uint8_t	 bDescriptorSubtype = 1;
		uint16_t bcdADC				= 0x100;
		uint16_t wTotalLength		= 0x9;
		uint8_t	 bInCollection		= 1;
		uint8_t	 baInterfaceNR		= 1;
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_audio_class);

	// Synchronization: the playback (OUT) endpoint is marked *adaptive* and the capture
	// (IN) endpoint *asynchronous*, so no explicit feedback endpoint is required — the
	// device rate-adapts via a jitter ring with occasional sample slip (Decision 3's
	// bring-up path). An explicit feedback endpoint is the documented follow-on.

	// 48 samples × 2ch × 3 bytes — the exact per-frame size for a single fixed 48 kHz rate. Windows
	// usbaudio builds no streaming pin if the iso wMaxPacketSize doesn't match the format-derived
	// size for a single declared frequency, so this must be exactly 288, not an over-provisioned 300.
	static constexpr uint16_t USBD_AUDIO_PACKET_SIZE = 288;
	static constexpr uint32_t USBD_AUDIO_SAMPLE_RATE = 48000;

	static constexpr uint8_t USBD_AUDIO_EP_OUT = 0x01;
	static constexpr uint8_t USBD_AUDIO_EP_IN  = 0x82; // dir bit set

	// Endpoint *numbers* (no direction bit) for the peripheral driver / iso config.
	static constexpr uint8_t USBD_AUDIO_EP_OUT_NUM = 1;
	static constexpr uint8_t USBD_AUDIO_EP_IN_NUM  = 2;

	enum usbd_audio_ac_subtype : uint8_t
	{
		USBD_AC_HEADER			= 1,
		USBD_AC_INPUT_TERMINAL	= 2,
		USBD_AC_OUTPUT_TERMINAL = 3,
		USBD_AC_FEATURE_UNIT	= 6,
	};

	enum usbd_audio_as_subtype : uint8_t
	{
		USBD_AS_GENERAL		= 1,
		USBD_AS_FORMAT_TYPE = 2,
	};

	enum usbd_audio_ep_subtype : uint8_t
	{
		USBD_AS_EP_GENERAL = 1,
	};

	enum usbd_audio_terminal_type : uint16_t
	{
		USBD_TT_USB_STREAMING = 0x0101, // USB endpoint terminal
		USBD_TT_LINE		  = 0x0603, // line connector (analog in/out)
	};

	struct usbd_audio_ac_interface_header
	{
		const static uint32_t SIZE = 10;

		usb_header head = {SIZE, USBD_AUDIO_CS_INTERFACE};

		uint8_t	 bDescriptorSubtype = USBD_AC_HEADER;
		uint16_t bcdADC				= 0x0100;
		uint16_t wTotalLength;
		uint8_t	 bInCollection	= 2;
		uint8_t	 baInterfaceNr1 = 1;
		uint8_t	 baInterfaceNr2 = 2;
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_audio_ac_interface_header);

	struct usbd_audio_input_terminal
	{
		const static uint32_t SIZE = 12;

		usb_header head = {SIZE, USBD_AUDIO_CS_INTERFACE};

		uint8_t	 bDescriptorSubtype = USBD_AC_INPUT_TERMINAL;
		uint8_t	 bTerminalID		= 0;
		uint16_t wTerminalType		= USBD_TT_USB_STREAMING;
		uint8_t	 bAssocTerminal		= 0;
		uint8_t	 bNrChannels		= 2;
		uint16_t wChannelConfig		= 0x0003; // left front + right front
		uint8_t	 iChannelNames		= 0;
		uint8_t	 iTerminal			= 0;
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_audio_input_terminal);

	struct usbd_audio_output_terminal
	{
		const static uint32_t SIZE = 9;

		usb_header head = {SIZE, USBD_AUDIO_CS_INTERFACE};

		uint8_t	 bDescriptorSubtype = USBD_AC_OUTPUT_TERMINAL;
		uint8_t	 bTerminalID		= 0;
		uint16_t wTerminalType		= USBD_TT_LINE;
		uint8_t	 bAssocTerminal		= 0;
		uint8_t	 bSourceID			= 0;
		uint8_t	 iTerminal			= 0;
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_audio_output_terminal);

	// No controls are declared (all bmaControls = 0): the handler exposes no host-adjustable mixer
	// controls in this bring-up, so declaring e.g. master mute would make usbaudio.sys query a
	// control the handler doesn't service at start and fail the audio function (code 10).
	struct usbd_audio_feature_unit
	{
		const static uint32_t SIZE = 10;

		usb_header head = {SIZE, USBD_AUDIO_CS_INTERFACE};

		uint8_t bDescriptorSubtype = USBD_AC_FEATURE_UNIT;
		uint8_t bUnitID			   = 0;
		uint8_t bSourceID		   = 0;
		uint8_t bControlSize	   = 1;
		uint8_t bmaControls0	   = 0x00;
		uint8_t bmaControls1	   = 0x00;
		uint8_t bmaControls2	   = 0x00;
		uint8_t iFeature		   = 0;
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_audio_feature_unit);

	struct usbd_audio_as_general
	{
		const static uint32_t SIZE = 7;

		usb_header head = {SIZE, USBD_AUDIO_CS_INTERFACE};

		uint8_t	 bDescriptorSubtype = USBD_AS_GENERAL;
		uint8_t	 bTerminalLink		= 0;
		uint8_t	 bDelay				= 1;
		uint16_t wFormatTag			= 0x0001; // PCM
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_audio_as_general);

	struct usbd_audio_format_type_i
	{
		const static uint32_t SIZE = 11;

		usb_header head = {SIZE, USBD_AUDIO_CS_INTERFACE};

		uint8_t bDescriptorSubtype = USBD_AS_FORMAT_TYPE;
		uint8_t bFormatType		   = 1;
		uint8_t bNrChannels		   = 2;
		uint8_t bSubframeSize	   = 3;
		uint8_t bBitResolution	   = 24;
		uint8_t bSamFreqType	   = 1; // one discrete frequency
		uint8_t tSamFreq0		   = (uint8_t)(USBD_AUDIO_SAMPLE_RATE & 0xFF);
		uint8_t tSamFreq1		   = (uint8_t)((USBD_AUDIO_SAMPLE_RATE >> 8) & 0xFF);
		uint8_t tSamFreq2		   = (uint8_t)((USBD_AUDIO_SAMPLE_RATE >> 16) & 0xFF);
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_audio_format_type_i);

	struct usbd_audio_iso_endpoint
	{
		const static uint32_t SIZE = 7;

		usb_header head = {SIZE, USBD_AUDIO_CS_ENDPOINT};

		uint8_t bDescriptorSubtype = USBD_AS_EP_GENERAL;
		// No endpoint controls. With a single fixed 48 kHz rate the sampling-frequency control is
		// optional (UAC1 §4.10.1.2); declaring it (0x01) makes usbaudio issue a SET/GET sampling-freq
		// control transfer during pin creation that we don't cleanly service, so it builds no
		// streaming pin (no render/capture endpoint). A known-good fixed-rate UAC1 device declares 0x00.
		uint8_t	 bmAttributes	 = 0x00;
		uint8_t	 bLockDelayUnits = 0;
		uint16_t wLockDelay		 = 0;
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_audio_iso_endpoint);

	struct usbd_audio_uac1_stereo
	{
		usbd_interface ac_iface = {
			.bInterfaceNumber	= 0,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 0,
			.bInterfaceClass	= USBD_CLASS_AUDIO,
			.bInterfaceSubClass = USBD_AUDIO_AUDIOCONTROL,
			.bInterfaceProtocol = 0,
			.iInterface			= 0,
		};

		usbd_audio_ac_interface_header ac_header = {
			.wTotalLength = sizeof(usbd_audio_ac_interface_header) + sizeof(usbd_audio_input_terminal)
							+ sizeof(usbd_audio_output_terminal) + sizeof(usbd_audio_input_terminal)
							+ sizeof(usbd_audio_output_terminal),
		};

		// playback path: USB-in terminal → line-out terminal (no feature unit; matches a known-good
		// minimal fixed-rate UAC1 device — a controls-less feature unit kept usbaudio from activating
		// the streaming pin so no render/capture endpoint surfaced).
		usbd_audio_input_terminal it_out = {
			.bTerminalID   = 1,
			.wTerminalType = USBD_TT_USB_STREAMING,
		};
		usbd_audio_output_terminal ot_out = {
			.bTerminalID   = 3,
			.wTerminalType = USBD_TT_LINE,
			.bSourceID	   = 1,
		};

		usbd_audio_input_terminal it_in = {
			.bTerminalID   = 4,
			.wTerminalType = USBD_TT_LINE,
		};
		usbd_audio_output_terminal ot_in = {
			.bTerminalID   = 6,
			.wTerminalType = USBD_TT_USB_STREAMING,
			.bSourceID	   = 4,
		};

		usbd_interface as_out_alt0 = {
			.bInterfaceNumber	= 1,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 0,
			.bInterfaceClass	= USBD_CLASS_AUDIO,
			.bInterfaceSubClass = USBD_AUDIO_AUDIOSTREAMING,
		};
		usbd_interface as_out_alt1 = {
			.bInterfaceNumber	= 1,
			.bAlternateSetting	= 1,
			.bNumEndpoints		= 1,
			.bInterfaceClass	= USBD_CLASS_AUDIO,
			.bInterfaceSubClass = USBD_AUDIO_AUDIOSTREAMING,
		};
		usbd_audio_as_general as_out_general = {
			.bTerminalLink = 1,
		};
		usbd_audio_format_type_i as_out_format = {};
		usbd_endpoint			 as_out_ep	   = {
			.bEndpointAddress = USBD_AUDIO_EP_OUT,
			.bmAttributes	  = 0x09, // isochronous, adaptive
			.wMaxPacketSize	  = USBD_AUDIO_PACKET_SIZE,
			.bInterval		  = 1,
		};
		usbd_audio_iso_endpoint as_out_cs_ep = {};

		usbd_interface as_in_alt0 = {
			.bInterfaceNumber	= 2,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 0,
			.bInterfaceClass	= USBD_CLASS_AUDIO,
			.bInterfaceSubClass = USBD_AUDIO_AUDIOSTREAMING,
		};
		usbd_interface as_in_alt1 = {
			.bInterfaceNumber	= 2,
			.bAlternateSetting	= 1,
			.bNumEndpoints		= 1,
			.bInterfaceClass	= USBD_CLASS_AUDIO,
			.bInterfaceSubClass = USBD_AUDIO_AUDIOSTREAMING,
		};
		usbd_audio_as_general as_in_general = {
			.bTerminalLink = 6,
		};
		usbd_audio_format_type_i as_in_format = {};
		usbd_endpoint			 as_in_ep	  = {
			.bEndpointAddress = USBD_AUDIO_EP_IN,
			.bmAttributes	  = 0x05, // isochronous, asynchronous
			.wMaxPacketSize	  = USBD_AUDIO_PACKET_SIZE,
			.bInterval		  = 1,
		};
		usbd_audio_iso_endpoint as_in_cs_ep = {};
	} bug_packed;

	struct usbd_audio_basic
	{
		usbd_config config = {
			.wTotalLength		 = sizeof(usbd_audio_basic),
			.bNumInterfaces		 = 3,
			.bConfigurationValue = 1,
			.iConfiguration		 = 0,
			.bmAttributes		 = usbd_config::get_attributes(true),
			.bMaxPower			 = 0x32,
		};

		usbd_audio_uac1_stereo audio = {};
	} bug_packed;

} // namespace lac

#pragma pack(pop)