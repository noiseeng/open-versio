/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "mcu_arm_v6m.h"
#include "mcu_arm_vfp.h"

namespace lac
{
	struct mcu_arm_v7m : mcu_arm_v6m, mcu_arm_vfp
	{
		template <typename slot_t>
		static void
		fill_isrv(slot_t *vectors, size_t count, uint32_t stack, void (*reset)(), void (*unused)(), void (*hard_fault)())
		{
			vectors[0] = (slot_t)stack;
			vectors[1] = (slot_t)reset;
			vectors[2] = (slot_t)unused;
			vectors[3] = (slot_t)hard_fault;

			for(size_t i = 4; i < count; i++)
			{
				vectors[i] = (slot_t)unused;
			}
		}
	};
} // namespace lac