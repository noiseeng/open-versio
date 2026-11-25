/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usb_defs.h"
#include "usbd.h"
#include "usbd_string.h"
#include <bug/compiler.h>
#include <stdint.h>

#pragma pack(push, 1)

namespace lac
{
	struct usbd_string_langid
	{
		usb_header head	   = {4, USBD_DESC_STRING};
		uint16_t   wLANGID = 0x0409;

		const usbd_string* get_string() const { return (const usbd_string*)this; }
	} bug_packed;
} // namespace lac

#pragma pack(pop)
