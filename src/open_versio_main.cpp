/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#include "hardware/versio_brd.h"
#include <mcu/stm/h750/mcu_h750_sram_flash.h>
#include "av_example.h"

int main()
{ return versio::versio_main<versio::av_example<32>, 96000>(); }
