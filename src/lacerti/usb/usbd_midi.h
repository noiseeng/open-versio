/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usbd.h"
#include "usbd_audio.h"
#include "usbd_core.h"
#include <bug/compiler.h>
#include <stdint.h>

#pragma pack(push, 1)

namespace lac
{

	struct usbd_midi
	{
		enum interface_type : uint8_t
		{
			CS_UNDEFINED	= 0x20,
			CS_DEVICE		= 0x21,
			CS_CONFIG		= 0x22,
			CS_STRING		= 0x23,
			CS_INTERFACE	= 0x24,
			CS_ENDPOINT		= 0x25,
			CS_GR_TRM_BLOCK = 0x26,
		};

		enum interface_subtype : uint8_t
		{
			MS_DESCRIPTOR_UNDEFINED = 0,
			MS_HEADER				= 1,
			MS_IN_JACK				= 2,
			MS_OUT_JACK				= 3,
			MS_ELEMENT				= 4,
		};

		enum endpoint_subtype : uint8_t
		{
			DESCRIPTOR_UNDEFINED = 0,
			MS_GENERAL			 = 1,
			MS_GENERAL_2_0		 = 2,
		};

		enum terminal_block_subtype : uint8_t
		{
			GR_TRM_BLOCK_UNDEFINED = 0,
			GR_TRM_BLOCK_HEADER	   = 1,
			GR_TRM_BLOCK		   = 2,
		};

		enum ms_class : uint16_t
		{
			MS_MIDI_1_0 = 0x0100,
			MS_MIDI_2_0 = 0x0200,
		};

		enum jack_type : uint8_t
		{
			JACK_TYPE_UNDEFINED = 0,
			EMBEDDED			= 1,
			EXTERNAL			= 2,
		};

		enum terminal_type : uint8_t
		{
			BIDIRECTIONAL = 0,
			INPUT_ONLY	  = 1,
			OUTPUT_ONLY	  = 2,
		};

		enum terminal_default : uint8_t
		{
			USE_MIDI_CI						 = 0x00,
			MIDI_1_0_UP_TO_64_BITS			 = 0x01,
			MIDI_1_0_UP_TO_64_BITS_AND_JRTS	 = 0x02,
			MIDI_1_0_UP_TO_128_BITS			 = 0x03,
			MIDI_1_0_UP_TO_128_BITS_AND_JRTS = 0x04,
			MIDI_2_0						 = 0x11,
			MIDI_2_0_AND_JRTS				 = 0x12,
		};
	};

	struct usbd_midi_interface
	{
		const static uint32_t SIZE = 9;

		uint8_t bLength			  = SIZE;
		uint8_t bDescriptorType	  = USBD_DESC_INTERFACE;
		uint8_t bInterfaceNumber  = 0;
		uint8_t bAlternateSetting = 0;
		uint8_t bNumEndpoints;
		uint8_t bInterfaceClass	   = USBD_CLASS_AUDIO;
		uint8_t bInterfaceSubClass = USBD_AUDIO_MIDISTREAMING;
		uint8_t bInterfaceProtocol = 0;
		uint8_t iInterface		   = 0;
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_midi_interface)

	struct usbd_midi_class
	{
		const static uint32_t SIZE = 7;

		uint8_t	 bLength			= SIZE;
		uint8_t	 bDescriptorType	= USBD_AUDIO_CS_INTERFACE;
		uint8_t	 bDescriptorSubtype = usbd_midi::MS_HEADER;
		uint16_t bcdMSC				= 0x0100;
		uint16_t wTotalLength;
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_midi_class);

	struct usbd_midi_jack_in
	{
		const static uint32_t SIZE = 6;

		uint8_t bLength			   = SIZE;
		uint8_t bDescriptorType	   = USBD_AUDIO_CS_INTERFACE;
		uint8_t bDescriptorSubtype = usbd_midi::MS_IN_JACK;
		uint8_t bJackType		   = usbd_midi::EXTERNAL;
		uint8_t bJackId			   = 0;
		uint8_t bJack			   = 0;
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_midi_jack_in);

	struct usbd_midi_jack_out
	{
		const static uint32_t SIZE = 9;

		uint8_t bLength			   = SIZE;
		uint8_t bDescriptorType	   = USBD_AUDIO_CS_INTERFACE;
		uint8_t bDescriptorSubtype = usbd_midi::MS_OUT_JACK;
		uint8_t bJackType		   = usbd_midi::EXTERNAL;
		uint8_t bJackId			   = 0;
		uint8_t bNrInputPins	   = 1;
		uint8_t baSourceId		   = 0;
		uint8_t baSourcePin		   = 0;
		uint8_t iJack			   = 0;
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_midi_jack_out);

	struct usbd_midi_endpoint
	{
		const static uint32_t SIZE = 5;

		uint8_t bLength			   = SIZE;
		uint8_t bDescriptorType	   = usbd_midi::CS_ENDPOINT;
		uint8_t bDescriptorSubtype = usbd_midi::MS_GENERAL;
		uint8_t bNumEmbMIDIJack	   = 1;
		uint8_t baAssocJackId	   = 1;
	} bug_packed;

	LAC_USBD_SIZE_CHECK(usbd_midi_endpoint);

	template <uint16_t MAX_PACKET = 32, uint8_t EP = 1>
	struct usbd_midi_basic_class_t
	{
		usbd_midi_class midi_class = {
			.wTotalLength = sizeof(usbd_midi_basic_class_t),
		};

		usbd_midi_jack_in jack_in_emb = {
			.bJackType = usbd_midi::EMBEDDED,
			.bJackId   = 0x01,
		};

		usbd_midi_jack_in jack_in_ext = {
			.bJackType = usbd_midi::EXTERNAL,
			.bJackId   = 0x02,
		};

		usbd_midi_jack_out jack_out_emb = {
			.bJackType	  = usbd_midi::EMBEDDED,
			.bJackId	  = 3,
			.bNrInputPins = 1,
			.baSourceId	  = 2,
			.baSourcePin  = 1,
		};

		usbd_midi_jack_out jack_out_ext = {
			.bJackType	  = usbd_midi::EXTERNAL,
			.bJackId	  = 4,
			.bNrInputPins = 1,
			.baSourceId	  = 1,
			.baSourcePin  = 1,
		};

		usbd_endpoint ep_out = {
			.bEndpointAddress = EP,
			.bmAttributes	  = 2,
			.wMaxPacketSize	  = MAX_PACKET,
		};

		usbd_midi_endpoint midi_out = {
			.bNumEmbMIDIJack = 1,
			.baAssocJackId	 = 1,
		};

		usbd_endpoint ep_in = {
			.bEndpointAddress = (uint8_t)(EP | USBD_ENDPOINT_IN),
			.bmAttributes	  = 2,
			.wMaxPacketSize	  = MAX_PACKET,
		};

		usbd_midi_endpoint midi_in = {
			.bNumEmbMIDIJack = 1,
			.baAssocJackId	 = 3,
		};
	} bug_packed;

	using usbd_midi_basic_class = usbd_midi_basic_class_t<>;

	template <uint8_t AC_IFACE, uint8_t MS_IFACE, uint8_t EP, uint16_t MAX_PACKET = 32>
	struct usbd_midi_function_t
	{
		usbd_interface ac_iface = {
			.bInterfaceNumber	= AC_IFACE,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 0,
			.bInterfaceClass	= USBD_CLASS_AUDIO,
			.bInterfaceSubClass = USBD_AUDIO_AUDIOCONTROL,
			.bInterfaceProtocol = 0,
			.iInterface			= USBD_STRID_IFACE0,
		};
		usbd_audio_class ac_class = {
			.baInterfaceNR = MS_IFACE,
		};
		usbd_midi_interface ms_iface = {
			.bInterfaceNumber  = MS_IFACE,
			.bAlternateSetting = 0,
			.bNumEndpoints	   = 2,
			.iInterface		   = USBD_STRID_IFACE0,
		};
		usbd_midi_basic_class_t<MAX_PACKET, EP> ms = {};
	} bug_packed;

	struct usbd_midi_basic
	{
		usbd_config config = {
			.wTotalLength		 = sizeof(usbd_midi_basic),
			.bNumInterfaces		 = 2,
			.bConfigurationValue = 1,
			.iConfiguration		 = 0,
			.bmAttributes		 = usbd_config::get_attributes(true),
			.bMaxPower			 = 0x32,
		};

		usbd_audio_standard_unused ac_standard;

		usbd_audio_class ac_class = {
			.bDescriptorSubtype = 1,
			.bcdADC				= 0x100,
			.wTotalLength		= 0x9,
			.bInCollection		= 1,
			.baInterfaceNR		= 1,
		};

		usbd_midi_interface midi_interface = {
			.bInterfaceNumber  = 1,
			.bAlternateSetting = 0,
			.bNumEndpoints	   = 2,
		};

		usbd_midi_basic_class midi;

	} bug_packed;

} // namespace lac

#pragma pack(pop)