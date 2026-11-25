/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>
#include <utl/bitflag.h>
#include <utl/bits.h>

#define LAC_USBD_SIZE_CHECK(type) static_assert(sizeof(type) == type::SIZE, #type "::SIZE wrong");

namespace lac
{
	using usbd_const_string = const char16_t *;

	enum usbd_type : uint8_t
	{
		USBD_DESC_DEVICE	   = 1,
		USBD_DESC_CONFIG	   = 2,
		USBD_DESC_STRING	   = 3,
		USBD_DESC_INTERFACE	   = 4,
		USBD_DESC_ENDPOINT	   = 5,
		USBD_DESC_QUALIFIER	   = 6,
		USBD_DESC_OTHER_SPEED  = 7,
		USBD_DESC_POWER		   = 8,
		USBD_DESC_IAD		   = 0x0B,
		USBD_DESC_BOS		   = 0x0F,
		USBD_DESC_HUB		   = 0x29,
		USBD_DESC_CS_INTERFACE = 0x24
	};

	enum usbd_bcd_version : uint16_t
	{
		USBD_VER_1_0 = 0x100,
		USBD_VER_1_1 = 0x110,
		USBD_VER_2_0 = 0x200,
		USBD_VER_2_1 = 0x210,
	};

	enum usbd_endpoint_dir : uint8_t
	{
		USBD_ENDPOINT_IN  = BIT7,
		USBD_ENDPOINT_OUT = 0
	};

	enum usbd_endpoint_type : uint8_t
	{
		USBD_ENDPOINT_CONTROL = 0,
		USBD_ENDPOINT_ISO	  = 1,
		USBD_ENDPOINT_BULK	  = 2,
		USBD_ENDPOINT_INT	  = 3,
	};

	enum usbd_class : uint8_t
	{
		USBD_CLASS_DEVICE	 = 0x00,
		USBD_CLASS_AUDIO	 = 0x01,
		USBD_CLASS_CDC		 = 0x02,
		USBD_CLASS_HID		 = 0x03,
		USBD_CLASS_PHYS		 = 0x05,
		USBD_CLASS_IMAGE	 = 0x06,
		USBD_CLASS_PRINT	 = 0x07,
		USBD_CLASS_STORAGE	 = 0x08,
		USBD_CLASS_HUB		 = 0x09,
		USBD_CLASS_CDC_DATA	 = 0x0A,
		USBD_CLASS_SDCARD	 = 0x0B,
		USBD_CLASS_SECURITY	 = 0x0D,
		USBD_CLASS_VIDEO	 = 0x0E,
		USBD_CLASS_HEALTH	 = 0x0F,
		USBD_CLASS_DISPLAY	 = 0x10,
		USBD_CLASS_BILLBOARD = 0x11,
		USBD_CLASS_USBC		 = 0x12,
		USBD_CLASS_I3C		 = 0x3C,
		USBD_CLASS_DIAG		 = 0xDC,
		USBD_CLASS_WIRELESS	 = 0xE0,
		USBD_CLASS_MISC		 = 0xEF,
		USBD_CLASS_APP_SPEC	 = 0xFE,
		USBD_CLASS_VEND_SPEC = 0xFF,
	};

	enum usbd_stringid : uint8_t
	{
		USBD_STRID_LANGID = 0,
		USBD_STRID_MANU,
		USBD_STRID_PROD,
		USBD_STRID_SERIAL,
		USBD_STRID_CONFIG,
		USBD_STRID_IFACE0,
		USBD_STRID_IFACE1,
		USBD_STRID_IFACE2,
		USBD_STRID_IFACE3
	};

	using USBD_ATTRIB_BUS_POWERED	= bitflag<7>;
	using USBD_ATTRIB_SELF_POWERED	= bitflag<6>;
	using USBD_ATTRIB_REMOTE_WAKEUP = bitflag<5>;

} // namespace lac