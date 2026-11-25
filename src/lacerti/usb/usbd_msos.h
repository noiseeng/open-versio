/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>
#include <stdint.h>

namespace lac
{
	static constexpr uint8_t MSOS_VENDOR_CODE	= 0x22;
	static constexpr uint8_t WEBUSB_VENDOR_CODE = 0x21;
	static constexpr uint8_t MSOS_DESC_INDEX	= 0x07;
	static constexpr uint8_t WEBUSB_GET_URL		= 0x02;

#pragma pack(push, 1)

	struct usbd_msos_set
	{
		uint16_t hdr_wLength		  = 10;
		uint16_t hdr_wDescriptorType  = 0x0000;
		uint32_t hdr_dwWindowsVersion = 0x06030000;
		uint16_t hdr_wTotalLength	  = 162;

		uint16_t cid_wLength			= 20;
		uint16_t cid_wDescriptorType	= 0x0003;
		uint8_t	 cid_compatibleID[8]	= {'W', 'I', 'N', 'U', 'S', 'B', 0, 0};
		uint8_t	 cid_subCompatibleID[8] = {0, 0, 0, 0, 0, 0, 0, 0};

		uint16_t rp_wLength				= 132;
		uint16_t rp_wDescriptorType		= 0x0004;
		uint16_t rp_wPropertyDataType	= 0x0007;
		uint16_t rp_wPropertyNameLength = 42;
		char16_t rp_propertyName[21]	= u"DeviceInterfaceGUIDs";
		uint16_t rp_wPropertyDataLength = 80;
		char16_t rp_propertyData[40]	= u"{9D8F4E45-7C3B-4A2E-9F1D-2E6B5C8A7D40}";
	};
	static_assert(sizeof(usbd_msos_set) == 162, "MS OS 2.0 set size/layout");

	struct usbd_msos_function
	{
		uint16_t fn_wLength			= 8;
		uint16_t fn_wDescriptorType = 0x0002;
		uint8_t	 fn_bFirstInterface = 0;
		uint8_t	 fn_bReserved		= 0;
		uint16_t fn_wSubsetLength	= 160;

		uint16_t cid_wLength			= 20;
		uint16_t cid_wDescriptorType	= 0x0003;
		uint8_t	 cid_compatibleID[8]	= {'W', 'I', 'N', 'U', 'S', 'B', 0, 0};
		uint8_t	 cid_subCompatibleID[8] = {0, 0, 0, 0, 0, 0, 0, 0};

		uint16_t rp_wLength				= 132;
		uint16_t rp_wDescriptorType		= 0x0004;
		uint16_t rp_wPropertyDataType	= 0x0007;
		uint16_t rp_wPropertyNameLength = 42;
		char16_t rp_propertyName[21]	= u"DeviceInterfaceGUIDs";
		uint16_t rp_wPropertyDataLength = 80;
		char16_t rp_propertyData[40]	= u"{9D8F4E45-7C3B-4A2E-9F1D-2E6B5C8A7D40}";
	};
	static_assert(sizeof(usbd_msos_function) == 160, "MS OS 2.0 function subset size/layout");

	template <size_t N>
	struct usbd_msos_set_composite
	{
		uint16_t hdr_wLength		  = 10;
		uint16_t hdr_wDescriptorType  = 0x0000;
		uint32_t hdr_dwWindowsVersion = 0x06030000;
		uint16_t hdr_wTotalLength	  = (uint16_t)(18 + N * sizeof(usbd_msos_function));

		uint16_t cfg_wLength			 = 8;
		uint16_t cfg_wDescriptorType	 = 0x0001;
		uint8_t	 cfg_bConfigurationValue = 0;
		uint8_t	 cfg_bReserved			 = 0;
		uint16_t cfg_wTotalLength		 = (uint16_t)(8 + N * sizeof(usbd_msos_function));

		usbd_msos_function fn[N] = {};
	};
	static_assert(sizeof(usbd_msos_set_composite<1>) == 178, "composite MS OS 2.0 set size/layout");
	static_assert(sizeof(usbd_msos_set_composite<2>) == 338, "two-function MS OS 2.0 set size/layout");

	struct usbd_winusb_bos
	{
		uint8_t	 bLength		 = 5;
		uint8_t	 bDescriptorType = 0x0F;
		uint16_t wTotalLength	 = 57;
		uint8_t	 bNumDeviceCaps	 = 2;

		uint8_t	 wu_bLength			   = 24;
		uint8_t	 wu_bDescriptorType	   = 0x10;
		uint8_t	 wu_bDevCapabilityType = 0x05;
		uint8_t	 wu_bReserved		   = 0;
		uint8_t	 wu_uuid[16] = {0x38, 0xB6, 0x08, 0x34, 0xA9, 0x09, 0xA0, 0x47, 0x8B, 0xFD, 0xA0, 0x76, 0x88, 0x15, 0xB6, 0x65};
		uint16_t wu_bcdVersion	 = 0x0100;
		uint8_t	 wu_bVendorCode	 = WEBUSB_VENDOR_CODE;
		uint8_t	 wu_iLandingPage = 1;

		uint8_t	 ms_bLength			   = 28;
		uint8_t	 ms_bDescriptorType	   = 0x10;
		uint8_t	 ms_bDevCapabilityType = 0x05;
		uint8_t	 ms_bReserved		   = 0;
		uint8_t	 ms_uuid[16] = {0xDF, 0x60, 0xDD, 0xD8, 0x89, 0x45, 0xC7, 0x4C, 0x9C, 0xD2, 0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F};
		uint32_t ms_dwWindowsVersion			  = 0x06030000;
		uint16_t ms_wMSOSDescriptorSetTotalLength = 162;
		uint8_t	 ms_bMS_VendorCode				  = MSOS_VENDOR_CODE;
		uint8_t	 ms_bAltEnumCode				  = 0;
	};
	static_assert(sizeof(usbd_winusb_bos) == 57, "BOS size/layout");

	struct usbd_webusb_url
	{
		uint8_t bLength			= 22;
		uint8_t bDescriptorType = 0x03;
		uint8_t bScheme			= 0x01;
		char	url[19] = {'n', 'o', 'i', 's', 'e', 'e', 'n', 'g', 'i', 'n', 'e', 'e', 'r', 'i', 'n', 'g', '.', 'u', 's'};
	};
	static_assert(sizeof(usbd_webusb_url) == 22, "WebUSB URL size/layout");

#pragma pack(pop)

	struct usbd_winusb_desc
	{
		const uint8_t* bos;
		uint16_t	   bos_len;
		const uint8_t* msos;
		uint16_t	   msos_len;
		const uint8_t* url;
		uint16_t	   url_len;
		uint8_t		   msos_vendor_code;
		uint8_t		   webusb_vendor_code;
	};

	inline const usbd_winusb_desc* get_winusb_desc()
	{
		static const usbd_winusb_bos  bos = {};
		static const usbd_msos_set	  set = {};
		static const usbd_webusb_url  url = {};
		static const usbd_winusb_desc d	  = {(const uint8_t*)&bos,
											 (uint16_t)sizeof(bos),
											 (const uint8_t*)&set,
											 (uint16_t)sizeof(set),
											 (const uint8_t*)&url,
											 (uint16_t)sizeof(url),
											 MSOS_VENDOR_CODE,
											 WEBUSB_VENDOR_CODE};
		return &d;
	}

	template <auto IFACES>
	inline const usbd_winusb_desc* get_winusb_desc_composite()
	{
		using set_t						 = usbd_msos_set_composite<IFACES.size()>;
		static const usbd_winusb_bos bos = [] {
			usbd_winusb_bos b				   = {};
			b.ms_wMSOSDescriptorSetTotalLength = (uint16_t)sizeof(set_t);
			return b;
		}();
		static const set_t set = [] {
			set_t s = {};
			for(size_t i = 0; i < IFACES.size(); i++)
			{
				s.fn[i].fn_bFirstInterface = IFACES[i];
			}
			return s;
		}();
		static const usbd_webusb_url  url = {};
		static const usbd_winusb_desc d	  = {(const uint8_t*)&bos,
											 (uint16_t)sizeof(bos),
											 (const uint8_t*)&set,
											 (uint16_t)sizeof(set),
											 (const uint8_t*)&url,
											 (uint16_t)sizeof(url),
											 MSOS_VENDOR_CODE,
											 WEBUSB_VENDOR_CODE};
		return &d;
	}

} // namespace lac
