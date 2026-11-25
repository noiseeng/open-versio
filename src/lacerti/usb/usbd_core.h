/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usb_defs.h"
#include "usbd.h"
#include <bug/compiler.h>
#include <stdint.h>

#pragma pack(push, 1)

namespace lac
{

	struct usbd_device
	{
		const static uint32_t SIZE = 18;

		usb_header head = {SIZE, USBD_DESC_DEVICE};

		uint16_t bcdUSB				= USBD_VER_1_1;
		uint8_t	 bDeviceClass		= 0;
		uint8_t	 bDeviceSubClass	= 0;
		uint8_t	 bDeviceProtocol	= 0;
		uint8_t	 bMaxPacketSize		= 0;
		uint16_t idVendor			= 0;
		uint16_t idProduct			= 0;
		uint16_t bcdDevice			= 0;
		uint8_t	 iManufacturer		= USBD_STRID_MANU;
		uint8_t	 iProduct			= USBD_STRID_PROD;
		uint8_t	 iSerialNumber		= USBD_STRID_SERIAL;
		uint8_t	 bNumConfigurations = 1;
	} bug_packed;

	struct usbd_qualifier
	{
		const static uint32_t SIZE = 10;

		usb_header head = {SIZE, USBD_DESC_QUALIFIER};

		uint16_t bcdUSB				= USBD_VER_2_0;
		uint8_t	 bDeviceClass		= 0;
		uint8_t	 bDeviceSubClass	= 0;
		uint8_t	 bDeviceProtocol	= 0;
		uint8_t	 bMaxPacketSize		= 64;
		uint8_t	 bNumConfigurations = 1;
		uint8_t	 bReserved			= 0;
	} bug_packed;

	static_assert(sizeof(usbd_qualifier) == usbd_qualifier::SIZE);

	struct usbd_config
	{
		const static uint32_t SIZE = 9;

		usb_header head = {SIZE, USBD_DESC_CONFIG};

		uint16_t wTotalLength		 = sizeof(usbd_config);
		uint8_t	 bNumInterfaces		 = 1;
		uint8_t	 bConfigurationValue = 1;
		uint8_t	 iConfiguration		 = 0;
		uint8_t	 bmAttributes		 = usbd_config::get_attributes(true);
		uint8_t	 bMaxPower			 = 0x32;

		constexpr static uint8_t get_attributes(bool self_powered)
		{ return self_powered ? (USBD_ATTRIB_BUS_POWERED::BIT | USBD_ATTRIB_SELF_POWERED::BIT) : USBD_ATTRIB_BUS_POWERED::BIT; }
	} bug_packed;

	struct usbd_interface
	{
		const static uint32_t SIZE = 9;

		usb_header head = {SIZE, USBD_DESC_INTERFACE};

		uint8_t bInterfaceNumber   = 0;
		uint8_t bAlternateSetting  = 0;
		uint8_t bNumEndpoints	   = 0;
		uint8_t bInterfaceClass	   = 0;
		uint8_t bInterfaceSubClass = 0;
		uint8_t bInterfaceProtocol = 0;
		uint8_t iInterface		   = 0;
	} bug_packed;

	static_assert(sizeof(usbd_interface) == usbd_interface::SIZE);

	struct usbd_class_interface
	{
		const static uint32_t SIZE = 9;

		usb_header head = {SIZE, USBD_DESC_CS_INTERFACE};

		uint8_t bInterfaceNumber;
		uint8_t bAlternateSetting;
		uint8_t bNumEndpoints;
		uint8_t bInterfaceClass;
		uint8_t bInterfaceSubClass;
		uint8_t bInterfaceProtocol;
		uint8_t iInterface;
	} bug_packed;

	static_assert(sizeof(usbd_class_interface) == usbd_class_interface::SIZE);

	struct usbd_endpoint
	{
		const static uint32_t SIZE = 9;

		usb_header head = {SIZE, USBD_DESC_ENDPOINT};

		uint8_t	 bEndpointAddress = 0;
		uint8_t	 bmAttributes	  = 2;
		uint16_t wMaxPacketSize	  = 0;
		uint8_t	 bInterval		  = 0;
		uint8_t	 bRefresh		  = 0;
		uint8_t	 bSyncAddress	  = 0;
	} bug_packed;

	struct usbd_bulk_endpoint
	{
		const static uint32_t SIZE = 7;

		usb_header head = {SIZE, USBD_DESC_ENDPOINT};

		uint8_t	 bEndpointAddress = 0;
		uint8_t	 bmAttributes	  = 2;
		uint16_t wMaxPacketSize	  = 0;
		uint8_t	 bInterval		  = 0;
	} bug_packed;

	static_assert(sizeof(usbd_bulk_endpoint) == usbd_bulk_endpoint::SIZE);

} // namespace lac

#pragma pack(pop)

namespace lac
{
	struct usbd_descriptors
	{
		const static uint32_t MAX_CONFIG = 4;
		const usbd_device*	  device;
		const usbd_config*	  config[MAX_CONFIG];
		const uint32_t		  config_count;
		const usbd_qualifier* qualifier = nullptr;
	};

} // namespace lac