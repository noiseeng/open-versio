/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace lac
{
	template <uint32_t EP, uint32_t MAX_PACKET>
	struct usb_log
	{
		alignas(4) uint8_t tx[MAX_PACKET] = {};

		template <typename usb_t, typename log_t>
		void poll(usb_t& usb, log_t& log)
		{
			static_assert(log_t::DN_CAPACITY >= MAX_PACKET,
						  "a full OUT packet can never fit the log down ring and input wedges");

			if(!usb.is_active())
			{
				return;
			}

			uint32_t rx = usb.get_data_count(EP);
			if(rx > 0 && log.put(0, rx, (const char*)usb.get_data(EP)))
			{
				usb.set_ep_ready(EP);
			}

			if(usb.is_send_idle(EP))
			{
				size_t n = log.pull(0, MAX_PACKET, (char*)tx);
				if(n > 0)
				{
					usb.send(EP, (uint32_t)n, tx, "log");
				}
			}
		}
	};
} // namespace lac
