/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "log.h"

namespace lac
{
	using log_default_t = log<1, 1024, 1, 128>;
}

extern lac::log_default_t _SEGGER_RTT;
