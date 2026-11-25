/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usbd.h"
#include "usbd_core.h"
#include "usbd_dfu.h"
#include "usbd_string_const.h"
#include <array>
#include <bug/compiler.h>
#include <stdint.h>

#pragma pack(push, 1)

namespace lac
{
	static constexpr uint8_t  USBD_LOG_IFACE		 = 1;
	static constexpr uint8_t  USBD_LOG_EP			 = 1;
	static constexpr uint16_t USBD_LOG_MAX_PACKET	 = 64;
	static constexpr uint16_t USBD_LOG_MAX_PACKET_HS = 512;

	template <uint16_t MAX_PACKET, uint8_t IFACE = USBD_LOG_IFACE, uint8_t EP = USBD_LOG_EP>
	struct usbd_log_function
	{
		usbd_interface iface = {
			.bInterfaceNumber	= IFACE,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 2,
			.bInterfaceClass	= USBD_CLASS_VEND_SPEC,
			.bInterfaceSubClass = 0,
			.bInterfaceProtocol = 0,
			.iInterface			= USBD_STRID_IFACE2,
		};

		usbd_bulk_endpoint ep_out = {
			.bEndpointAddress = EP,
			.wMaxPacketSize	  = MAX_PACKET,
		};

		usbd_bulk_endpoint ep_in = {
			.bEndpointAddress = 0x80 | EP,
			.wMaxPacketSize	  = MAX_PACKET,
		};
	} bug_packed;

	template <uint8_t ALTS = 2>
	struct usbd_dfu_log_config
	{
		usbd_config config = {
			.wTotalLength	= (uint16_t)(sizeof(usbd_config) + sizeof(usbd_dfu_alts_t<0, USBD_STRID_IFACE0, ALTS>)
										 + sizeof(usb_dfu_descriptor) + sizeof(usbd_log_function<USBD_LOG_MAX_PACKET>)),
			.bNumInterfaces = 2,
		};

		usbd_dfu_alts_t<0, USBD_STRID_IFACE0, ALTS> dfu_alts = {};

		usb_dfu_descriptor dfu = {};

		usbd_log_function<USBD_LOG_MAX_PACKET> log = {};
	} bug_packed;

	template <uint8_t IFACE>
	constexpr usbd_interface usbd_dfu_runtime_iface = {
		.bInterfaceNumber	= IFACE,
		.bAlternateSetting	= 0,
		.bNumEndpoints		= 0,
		.bInterfaceClass	= USBD_CLASS_APP_SPEC,
		.bInterfaceSubClass = 1,
		.bInterfaceProtocol = 1,
		.iInterface			= 0,
	};

	template <uint16_t MAX_PACKET>
	struct usbd_dfu_runtime_log_config
	{
		usbd_config config = {
			.wTotalLength	= (uint16_t)(sizeof(usbd_config) + sizeof(usbd_interface) + sizeof(usb_dfu_descriptor)
										 + sizeof(usbd_log_function<MAX_PACKET>)),
			.bNumInterfaces = 2,
		};

		usbd_interface dfu_iface = usbd_dfu_runtime_iface<0>;

		usb_dfu_descriptor dfu = {};

		usbd_log_function<MAX_PACKET> log = {};
	} bug_packed;

	template <typename layout_t, usbd_device DEV>
	struct usbd_dfu_log
	{
		constexpr static bool					COMPOSITE_WINUSB = true;
		constexpr static std::array<uint8_t, 2> WINUSB_IFACES	 = {0, USBD_LOG_IFACE};

		constexpr static usbd_device				 dev	  = DEV;
		constexpr static uint8_t					 ALTS	  = usbd_dfu_alt_count<layout_t>;
		constexpr static usbd_dfu_log_config<ALTS>	 conf	  = {};
		constexpr static const char16_t*			 cstr_if0 = layout_t::descriptor;
		constexpr static const char16_t*			 cstr_if1 = usbd_dfu_options_descriptor<layout_t>();
		constexpr static const char16_t*			 cstr_log = u"Log";
		constexpr static usbd_string_const<cstr_if0> str_if0  = {};
		constexpr static usbd_string_const<cstr_if1> str_if1  = {};
		constexpr static usbd_string_const<cstr_log> str_log  = {};
		constexpr static usbd_descriptors			 desc	  = {&dev, {&conf.config, 0, 0, 0}, 1};

		template <class usb_t>
		void init(usb_t& usb)
		{
			usb.set_string(USBD_STRID_IFACE0, str_if0.get_string());
			if constexpr(ALTS == 2)
			{
				usb.set_string(USBD_STRID_IFACE1, str_if1.get_string());
			}
			usb.set_string(USBD_STRID_IFACE2, str_log.get_string());
			usb.set_descriptors(&desc);
			usb.set_data_endpoint_count(1);
		}
	};

	template <usbd_device DEV, bool HIGH_SPEED, uint16_t FS_MAX_PACKET = USBD_LOG_MAX_PACKET>
	struct usbd_dfu_runtime_log
	{
		constexpr static bool					COMPOSITE_WINUSB = true;
		constexpr static std::array<uint8_t, 2> WINUSB_IFACES	 = {0, USBD_LOG_IFACE};
		constexpr static uint16_t				MAX_PACKET		 = HIGH_SPEED ? USBD_LOG_MAX_PACKET_HS : FS_MAX_PACKET;
		constexpr static uint8_t				LOG_EP			 = USBD_LOG_EP;

		constexpr static usbd_device	dev	 = DEV;
		constexpr static usbd_qualifier qual = {
			.bcdUSB				= USBD_VER_2_0,
			.bMaxPacketSize		= 64,
			.bNumConfigurations = 1,
		};
		constexpr static usbd_dfu_runtime_log_config<MAX_PACKET> conf	  = {};
		constexpr static const char16_t*						 cstr_log = u"Log";
		constexpr static usbd_string_const<cstr_log>			 str_log  = {};
		constexpr static usbd_descriptors desc = {&dev, {&conf.config, 0, 0, 0}, 1, HIGH_SPEED ? &qual : nullptr};

		template <class usb_t>
		void init(usb_t& usb)
		{
			usb.set_string(USBD_STRID_IFACE2, str_log.get_string());
			usb.set_descriptors(&desc);
			usb.set_data_endpoint_count(1);
			usb.dfu->dfu_iface = conf.dfu_iface.bInterfaceNumber;
		}
	};

} // namespace lac

#pragma pack(pop)
