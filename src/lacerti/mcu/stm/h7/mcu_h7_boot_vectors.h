/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace lac
{

	struct mcu_h7_boot_vectors
	{
		constexpr static uint32_t MAGIC = 0x841F2345;
		uint32_t				  stack;
		void (*reset)();
		void (*nmi)();
		void (*hard_fault)();
		uint32_t magic;
		uint32_t product;
		uint32_t version;
		uint32_t size;
		uint32_t md5[4];
	};

	struct mcu_h7_boot_vectors16
	{
		mcu_h7_boot_vectors head;
		void (*debug_mon)();
		uint32_t reserved;
		void (*pend_sv)();
		void (*sys_tick)();
	};

	static_assert(sizeof(void (*)()) != 4 || sizeof(mcu_h7_boot_vectors) == 48);
	static_assert(offsetof(mcu_h7_boot_vectors, magic) == 16);
	static_assert(offsetof(mcu_h7_boot_vectors, md5) == 32);
	static_assert(sizeof(mcu_h7_boot_vectors16) == 64);
	static_assert(offsetof(mcu_h7_boot_vectors16, reserved) == 52);

} // namespace lac
