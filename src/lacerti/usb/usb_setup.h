/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usb_defs.h"
#include "usbd.h"
#include <stdint.h>

namespace lac
{
	struct usb_setup
	{
		volatile uint8_t  request_type;
		volatile uint8_t  request;
		volatile uint16_t wValue;
		volatile uint16_t wIndex;
		volatile uint16_t wLength;

		bool is_to_host() const { return (request_type & 128) != 0; }

		bool is_to_device() const { return !is_to_host(); }

		usb_request_recipient get_recipient() const { return (usb_request_recipient)(request_type & 31); }

		uint32_t get_type() const { return (request_type >> 5) & 3; }

		uint32_t get_request() const { return (request_type << 8) | request; }

		uint16_t get_value() const { return wValue; }

		uint16_t get_index() const { return wIndex; }

		uint16_t get_length() const { return wLength; }

		usbd_type get_desc_type() const { return (usbd_type)(wValue >> 8); }

		uint8_t get_desc_index() const { return (uint8_t)(wValue & 0xFF); }
	};

	static_assert(sizeof(usb_setup) == 8, "invalid usb setup size");

} // namespace lac