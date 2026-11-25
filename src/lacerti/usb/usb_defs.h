/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <bug/compiler.h>
#include <stdint.h>

#pragma pack(push, 1)

namespace lac
{
	enum usb_request_device : uint8_t
	{
		USB_DEVICE_GET_STATUS		 = 0x00,
		USB_DEVICE_CLEAR_FEATURE	 = 0x01,
		USB_DEVICE_SET_FEATURE		 = 0x03,
		USB_DEVICE_SET_ADDRESS		 = 0x05,
		USB_DEVICE_GET_DESCRIPTOR	 = 0x06,
		USB_DEVICE_SET_DESCRIPTOR	 = 0x07,
		USB_DEVICE_GET_CONFIGURATION = 0x08,
		USB_DEVICE_SET_CONFIGURATION = 0x09
	};

	enum usb_request_interface : uint8_t
	{
		USB_INTERFACE_GET_STATUS	= 0x00,
		USB_INTERFACE_CLEAR_FEATURE = 0x01,
		USB_INTERFACE_SET_FEATURE	= 0x03,
		USB_INTERFACE_GET_INTERFACE = 0x0A,
		USB_INTERFACE_SET_INTERFACE = 0x0B
	};

	enum usb_request_endpoint : uint8_t
	{
		USB_ENDPOINT_GET_STATUS	   = 0x00,
		USB_ENDPOINT_CLEAR_FEATURE = 0x01,
		USB_ENDPOINT_SET_FEATURE   = 0x03,
		USB_ENDPOINT_SYNC_FRAME	   = 0x0C
	};

	enum usb_request_type_type
	{
		USB_REQUEST_TYPE_STANDARD = 0,
		USB_REQUEST_TYPE_CLASS,
		USB_REQUEST_TYPE_VENDOR,
		USB_REQUEST_TYPE_RESERVED
	};

	enum usb_request_recipient
	{
		USB_RECIPIENT_DEVICE = 0,
		USB_RECIPIENT_INTERFACE,
		USB_RECIPIENT_ENDPOINT,
		USB_RECIPIENT_OTHER,
		USB_RECIPIENT_COUNT
	};

	enum usb_interface_class
	{
		USB_IF_AUDIO = 1,
	};

	constexpr uint16_t utoh(uint16_t b)
	{ return b; }

	constexpr uint16_t htou(uint16_t b)
	{ return b; }

	enum usb_ep_state : uint8_t
	{
		USB_EP_IDLE,
		USB_EP_BUSY
	};

	struct usb_header
	{
		uint8_t bLength;
		uint8_t bDescriptorType;
	} bug_packed;

} // namespace lac

#pragma pack(pop)
