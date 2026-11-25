/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <mcu/stm/h7/mcu_h7_boot_vectors.h>
#include <mcu/stm/h7/mcu_h7_sram_flash.h>
#include <mcu/stm/h750/mcu_h750.h>
#include <mcu/stm/h750/mem_h750.h>

using namespace lac;

__attribute__((section(".flash_boot"), noreturn)) static void flash_reset_handler()
{
	flash_copy_sram_image();
	void (*volatile entry)() = mcu_h750<1>::reset_handler;
	entry();
	__builtin_unreachable();
}

alignas(256) constexpr mcu_h7_boot_vectors isr_boot_vectors __attribute__((section(".isr_vector"), used))
= {mem_h750::STACK_TOP,
   flash_reset_handler,
   flash_fault_spin,
   flash_fault_spin,
   mcu_h7_boot_vectors::MAGIC,
   'unkn',
   0,
   123456,
   {0, 0, 0, 0}};

static_assert(isr_boot_vectors.stack == 0x20020000);
static_assert(isr_boot_vectors.reset == &flash_reset_handler);
static_assert(isr_boot_vectors.nmi == &flash_fault_spin);
static_assert(isr_boot_vectors.hard_fault == &flash_fault_spin);
static_assert(isr_boot_vectors.magic == 0x841F2345);
static_assert(isr_boot_vectors.product == (uint32_t)'unkn');
static_assert(isr_boot_vectors.version == 0);
static_assert(isr_boot_vectors.size == 123456);
static_assert(isr_boot_vectors.md5[0] == 0);
static_assert(isr_boot_vectors.md5[1] == 0);
static_assert(isr_boot_vectors.md5[2] == 0);
static_assert(isr_boot_vectors.md5[3] == 0);
