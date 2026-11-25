/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <mcu/stm/h750/usb_h750.h>
#include <usb/usbd.h>
#include <usb/usbd_core.h>
#include <usb/usbd_dfu.h>
#include <usb/usbd_dfu_log.h>

namespace lac
{
	using usbd_dfu_h750_versio = usbd_dfu<dfu_layout_h750,
										  usbd_device{
											  .bcdUSB		  = USBD_VER_2_1,
											  .bMaxPacketSize = 64,
											  .idVendor		  = 0x0483,
											  .idProduct	  = 0x4E56,
											  .bcdDevice	  = 0x0003,
										  },
										  2>;

	using usbd_dfu_log_h750_versio = usbd_dfu_log<dfu_layout_h750,
												  usbd_device{
													  .bcdUSB		  = USBD_VER_2_1,
													  .bMaxPacketSize = 64,
													  .idVendor		  = 0x0483,
													  .idProduct	  = 0x4E56,
													  .bcdDevice	  = 0x0103,
												  }>;
} // namespace lac
