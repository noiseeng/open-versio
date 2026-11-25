/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#include "bug.h"
#include "log_default.h"

constinit lac::log_default_t _SEGGER_RTT;

namespace lac
{
	void write_char(char c, void* ctx)
	{ ::_SEGGER_RTT.push((int)(intptr_t)ctx, c); }
} // namespace lac
