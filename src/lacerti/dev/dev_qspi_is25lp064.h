/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

#include "dev_qspi_spinor.h"
#include <utl/bitflag.h>

namespace lac
{
	struct spinor_is25lp064
	{
		constexpr static uint32_t ADDRESS_BITS		= 23;
		constexpr static uint32_t MMAP_ADDRESS_BITS = 23;
		constexpr static uint32_t DUMMY_CYCLES		= 8;
		constexpr static bool	  HAS_JEDEC_ID		= false;
		constexpr static uint8_t  CMD_QE_READ		= 0x05;
		constexpr static uint8_t  CMD_QE_WRITE		= 0x01;

		using QE = bitflag<6>;
	};

	template <typename qspi_t, typename timer_t>
	using dev_qspi_is25lp064 = dev_qspi_spinor<spinor_is25lp064, qspi_t, timer_t>;
} // namespace lac
