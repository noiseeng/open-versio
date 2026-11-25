/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "usb_audio_handler_none.h"
#include "usb_dfu_handler.h"
#include <mcu/stm/stm32/reg_uuid.h> // IWYU pragma: keep
#include <usb/usb_state.h>
#include <usb/usbd.h>
#include <usb/usbd_msos.h>
#include <usb/usbd_str16.h>
#include <usb/usbd_string.h>
#include <usb/usbd_string_const.h>
#include <usb/usbd_string_langid.h>
#include <usb/usbd_string_literal.h>

#include <cstdint>

namespace lac
{
	template <typename mcu_t,
			  typename descr_t,
			  usbd_str16 PROD,
			  bool		 HAS_WINUSB = true,
			  typename usb_audio_t	= lac::usb_audio_handler_none,
			  typename usb_dfu_t	= lac::usb_dfu_handler>
	struct usb_device
	{
		using usb_per_t = typename mcu_t::usb_per_t;
		using usb_t		= lac::usb_state<usb_per_t, usb_dfu_t, usb_audio_t>;

		usb_t	usb	  = {};
		descr_t descr = {};

		void init()
		{
			alignas(4) constexpr static const char16_t*	   cstr_manu = u"Noise Engineering";
			constexpr static lac::usbd_string_langid	   str_lang	 = {};
			const static lac::usbd_string_const<cstr_manu> str_manu	 = {};
			const static lac::usbd_string_literal<PROD>	   str_prod	 = {};

			static uint8_t ser_buf[2 + 24 * 2];
			ser_buf[0]			  = sizeof(ser_buf);
			ser_buf[1]			  = lac::USBD_DESC_STRING;
			const auto*	   uid	  = mcu_t::get_uuid();
			const uint32_t w[3]	  = {uid->W0, uid->W1, uid->W2};
			const char	   hexd[] = "0123456789ABCDEF";
			for(uint32_t i = 0; i < 3; i++)
			{
				for(uint32_t n = 0; n < 8; n++)
				{
					ser_buf[2 + (i * 8 + n) * 2]	 = (uint8_t)hexd[(w[i] >> ((7 - n) * 4)) & 0xF];
					ser_buf[2 + (i * 8 + n) * 2 + 1] = 0;
				}
			}

			usb.set_string(lac::USBD_STRID_LANGID, str_lang.get_string());
			usb.set_string(lac::USBD_STRID_MANU, str_manu.get_string());
			usb.set_string(lac::USBD_STRID_PROD, str_prod.get_string());
			usb.set_string(lac::USBD_STRID_SERIAL, (const lac::usbd_string*)ser_buf);
			usb.set_string(lac::USBD_STRID_CONFIG, str_prod.get_string());

			if constexpr(HAS_WINUSB)
			{
				if constexpr(descr_t::COMPOSITE_WINUSB)
				{
					usb.set_winusb(lac::get_winusb_desc_composite<descr_t::WINUSB_IFACES>());
				}
				else
				{
					usb.set_winusb(lac::get_winusb_desc());
				}
			}

			descr.init(usb);

			usb.init();
			mcu_t::wire_usb_irq(usb_per_t::irq_thunk);
		}
	};
} // namespace lac
