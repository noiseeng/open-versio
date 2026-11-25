/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>
#include <usb/usbd.h>
#include <usb/usbd_audio_dfu.h>
#include <usb/usbd_audio_midi_dfu.h>
#include <usb/usbd_core.h>
#include <usb/usbd_dfu.h>
#include <usb/usbd_dfu_log.h>
#include <usb/usbd_midi_dfu.h>

namespace lac
{
	struct dfu_layout_h750
	{
		static constexpr bool			 HAS_SD			   = false;
		static constexpr bool			 HAS_OPTIONS	   = false;
		static constexpr uint32_t		 flash_base		   = 0x08000000u;
		static constexpr uint32_t		 flash_size		   = 128u * 1024u;
		static constexpr uint32_t		 erase_granularity = 128u * 1024u;
		static constexpr const char16_t* descriptor		   = u"@Internal Flash   /0x08000000/01*128Kg";
	};

	using usbd_dfu_h750 = usbd_dfu<dfu_layout_h750,
								   usbd_device{
									   .bcdUSB		   = USBD_VER_2_1,
									   .bMaxPacketSize = 64,
									   .idVendor	   = 0x0483,
									   .idProduct	   = 0x4E45,
								   },
								   2>;

	using usbd_dfu_log_h750 = usbd_dfu_log<dfu_layout_h750,
										   usbd_device{
											   .bcdUSB		   = USBD_VER_2_1,
											   .bMaxPacketSize = 64,
											   .idVendor	   = 0x0483,
											   .idProduct	   = 0x4E45,
											   .bcdDevice	   = 0x0100,
										   }>;

	using usbd_midi_dfu_h750 = usbd_midi_dfu_t<dfu_layout_h750>;

	using usbd_audio_dfu_h750 = usbd_audio_dfu<usbd_device{
		.bcdUSB			= USBD_VER_2_1,
		.bMaxPacketSize = 64,
		.idVendor		= 0x0483,
		.idProduct		= 0x4E45,
	}>;

	using usbd_audio_midi_dfu_h750 = usbd_audio_midi_dfu<dfu_layout_h750,
														 usbd_device{
															 .bcdUSB		 = USBD_VER_2_1,
															 .bMaxPacketSize = 64,
															 .idVendor		 = 0x0483,
															 .idProduct		 = 0x4E4D,
															 .bcdDevice		 = 0x0004,
														 }>;
} // namespace lac
