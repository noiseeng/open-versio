/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "usbd.h"
#include "usbd_core.h"
#include "usbd_string_const.h"
#include <bug/compiler.h>
#include <stdint.h>
#include <type_traits>
#include <utl/bitflag.h>

#pragma pack(push, 1)

namespace lac
{
	struct usb_dfu_descriptor
	{
		uint8_t	 bLength		 = 9;
		uint8_t	 bDescriptorType = 0x21;
		uint8_t	 bmAttributes	 = 0x0B;
		uint16_t wDetachTimeout	 = 0x00FF;
		uint16_t wTransferSize	 = 0x400;
		uint16_t bcdDFUVersion	 = 0x011A;

		using ATTR_CAN_DOWNLOAD		 = bitflag<0>;
		using ATTR_CAN_UPLOAD		 = bitflag<1>;
		using ATTR_MANIFEST_TOLERANT = bitflag<2>;
		using ATTR_WILL_DETACH		 = bitflag<3>;
	} bug_packed;

	template <uint8_t IFACE, uint8_t STR, uint8_t ALT>
	constexpr usbd_interface usbd_dfu_alt = {
		.bInterfaceNumber	= IFACE,
		.bAlternateSetting	= ALT,
		.bNumEndpoints		= 0,
		.bInterfaceClass	= USBD_CLASS_APP_SPEC,
		.bInterfaceSubClass = 1,
		.bInterfaceProtocol = 2,
		.iInterface			= STR,
	};

	template <uint8_t IFACE, uint8_t STR_ALT0, uint8_t ALTS = 2>
	struct usbd_dfu_alts_t
	{
		usbd_interface alt0 = usbd_dfu_alt<IFACE, STR_ALT0, 0>;
		usbd_interface alt1 = usbd_dfu_alt<IFACE, (uint8_t)(STR_ALT0 + 1), 1>;
	} bug_packed;

	template <uint8_t IFACE, uint8_t STR_ALT0>
	struct usbd_dfu_alts_t<IFACE, STR_ALT0, 1>
	{
		usbd_interface alt0 = usbd_dfu_alt<IFACE, STR_ALT0, 0>;
	} bug_packed;

	template <typename layout_t>
	constexpr uint8_t usbd_dfu_alt_count = layout_t::HAS_OPTIONS ? 2 : 1;

	template <typename layout_t>
	static constexpr const char16_t* usbd_dfu_options_descriptor()
	{
		if constexpr(layout_t::HAS_OPTIONS)
		{
			return layout_t::descriptor_options;
		}
		else
		{
			return u"";
		}
	}

	template <uint8_t ALTS = 2>
	struct usbd_dfu_config
	{
		usbd_config config = {
			.wTotalLength = (uint16_t)(sizeof(usbd_config) + sizeof(usbd_dfu_alts_t<0, USBD_STRID_IFACE0, ALTS>)
									   + sizeof(usb_dfu_descriptor)),
		};

		usbd_dfu_alts_t<0, USBD_STRID_IFACE0, ALTS> dfu_alts = {};

		usb_dfu_descriptor dfu = {};
	} bug_packed;

	struct usbd_dfu_config3
	{
		usbd_config config = {
			.wTotalLength = (uint16_t)(sizeof(usbd_config) + 3 * sizeof(usbd_interface) + sizeof(usb_dfu_descriptor)),
		};

		usbd_dfu_alts_t<0, USBD_STRID_IFACE0> dfu_alts = {};

		usbd_interface iface2 = {
			.bAlternateSetting	= 2,
			.bInterfaceClass	= 0xFE,
			.bInterfaceSubClass = 1,
			.bInterfaceProtocol = 2,
			.iInterface			= USBD_STRID_IFACE2,
		};

		usb_dfu_descriptor dfu = {};
	} bug_packed;

	template <typename layout_t, usbd_device DEV, uint8_t DATA_EP_COUNT, bool HIGH_SPEED = false>
	struct usbd_dfu
	{
		constexpr static bool	 COMPOSITE_WINUSB = false;
		constexpr static bool	 HAS_SD			  = layout_t::HAS_SD;
		constexpr static uint8_t ALTS			  = usbd_dfu_alt_count<layout_t>;

		static_assert(!HAS_SD || ALTS == 2, "the SD alt is alt 2, so a layout with SD also needs its options alt");

		static constexpr const char16_t* get_sd_descriptor()
		{
			if constexpr(HAS_SD)
			{
				return layout_t::descriptor_sd;
			}
			else
			{
				return u"";
			}
		}

		constexpr static usbd_device	dev	 = DEV;
		constexpr static usbd_qualifier qual = {
			.bcdUSB				= USBD_VER_2_0,
			.bDeviceClass		= DEV.bDeviceClass,
			.bDeviceSubClass	= DEV.bDeviceSubClass,
			.bDeviceProtocol	= DEV.bDeviceProtocol,
			.bMaxPacketSize		= 64,
			.bNumConfigurations = 1,
		};

		using config_t = std::conditional_t<HAS_SD, usbd_dfu_config3, usbd_dfu_config<ALTS>>;

		constexpr static config_t					 conf	  = {};
		constexpr static const char16_t*			 cstr_if0 = layout_t::descriptor;
		constexpr static const char16_t*			 cstr_if1 = usbd_dfu_options_descriptor<layout_t>();
		constexpr static const char16_t*			 cstr_if2 = get_sd_descriptor();
		constexpr static usbd_string_const<cstr_if0> str_if0  = {};
		constexpr static usbd_string_const<cstr_if1> str_if1  = {};
		constexpr static usbd_string_const<cstr_if2> str_if2  = {};
		constexpr static usbd_descriptors			 desc = {&dev, {&conf.config, 0, 0, 0}, 1, HIGH_SPEED ? &qual : nullptr};

		template <class usb_t>
		void init(usb_t& usb)
		{
			usb.set_string(USBD_STRID_IFACE0, str_if0.get_string());
			if constexpr(ALTS == 2)
			{
				usb.set_string(USBD_STRID_IFACE1, str_if1.get_string());
			}
			if constexpr(HAS_SD)
			{
				usb.set_string(USBD_STRID_IFACE2, str_if2.get_string());
			}
			usb.set_descriptors(&desc);
			usb.set_data_endpoint_count(DATA_EP_COUNT);
		}
	};

} // namespace lac

#pragma pack(pop)
