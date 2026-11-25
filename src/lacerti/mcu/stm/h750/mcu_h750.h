/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

#include "adr_h750.h"
#include "irq_h750.h"
#include "isv_h750.h"
#include "mem_h750.h"
#include "reg_h750_pwr.h"
#include "reg_h750_syscfg.h"
#include <atomic>
#include <bug/eabi.h>
#include <cstddef>
#include <mcu/arm/mcu_arm_v7m.h>
#include <mcu/arm/mcu_arm_v7m_fault.h>
#include <mcu/arm/reg_cm7_scb.h>
#include <mcu/arm/reg_v7m_coredebug.h>
#include <mcu/stm/h7/mcu_h7_dma.h>
#include <mcu/stm/h7/mcu_h7_irq.h>
#include <mcu/stm/h7/per_h7_gpio.h>
#include <mcu/stm/h7/per_h7_rcc.h>
#include <mcu/stm/h7/per_h7_tim.h>
#include <mcu/stm/h7/per_h7_usb.h>
#include <mcu/stm/h7/reg_h7_flash.h>
#include <mcu/stm/h7/reg_h7_mpu.h>
#include <mcu/stm/h7/reg_h7_nvic.h>
#include <mcu/stm/h7/reg_h7_rcc.h>
#include <mcu/stm/h7/reg_h7_rtc.h>
#include <utl/bug.h>

#include <mcu/linker.h>
#include <type_traits>
#include <utl/allocator.h>

#include <mcu/ram.h>
#include <mcu/stm/h7/mcu_h7_periph.h>
#include <mcu/stm/h7/reg_h7_dmamux.h>
#include <mcu/stm/stm32/per_gpio.h>

namespace lac
{
	template <typename mcu_t>
	struct mcu_h750_no_ext_ram
	{
		constexpr static uint32_t SIZE = 0;

		static void init() {}
	};

	template <uint32_t cpu_div>
	struct mcu_h750 : adr_h750,
					  mcu_h7_irq<adr_h750, irq_h750, isv_h750>,
					  mcu_h7_dma<adr_h750>,
					  mcu_h7_periph<adr_h750, per_h7_rcc<adr_h750::RCC, 16000000, 480000000 / cpu_div, 0, 0>>
	{
		static_assert(60 % cpu_div == 0, "cpu_div must divide 60");

		const static uint32_t REF_CLK_HZ = 480000000;
		const static uint32_t SYS_DIV	 = cpu_div;
		const static uint32_t SYS_CLK_HZ = REF_CLK_HZ / SYS_DIV;
		const static uint32_t EXT_CLK_HZ = 16000000;
		const static uint32_t TIM_CLK_HZ = SYS_CLK_HZ / 2;

		const static uint32_t STACK_TOP = mem_h750::STACK_TOP;

		using this_t = mcu_h750<cpu_div>;
		using adr_t	 = adr_h750;
		using tim2_t = per_h7_tim<this_t, adr_t::TIM2, TIM_CLK_HZ>;
		using rcc_t	 = per_h7_rcc<adr_t::RCC, EXT_CLK_HZ, SYS_CLK_HZ, 0, 0>;

		static_assert(std::is_same_v<rcc_t, per_h7_rcc<adr_h750::RCC, 16000000, 480000000 / cpu_div, 0, 0>>);

		using usb_pins_t = per_h7_usb_pins<gpio_bit<adr_t::GPIOA, 11, 10>, gpio_bit<adr_t::GPIOA, 12, 10>>;
		using usb_per_t	 = per_h7_usb<rcc_t, tim2_t, adr_t::USB2_OTG_FS_PERIPH, usb_pins_t>;
		using mem_t		 = mem_h750;

		ram	  allocator = {mem_h750::ZONES.size(), mem_h750::ZONES.data()};
		void *null_zone = 0;

		static void wire_usb_irq(void (*thunk)())
		{
			set_isv(isv_h750::OTG_FS, thunk);
			get_nvic()->enable_irq(irq_h750::OTG_FS);
		}

		template <typename obj_type>
		static obj_type *alloc_dma()
		{ return mem_t::template alloc_dma<obj_type>(); }

		static uint8_t *alloc_dma(size_t size, const char *extra) { return mem_t::template alloc_dma<uint8_t>(size, extra); }

		using mcu_alloc_t = mem_h750_alloc_t;

		mcu_alloc_t alloc;

		inline static volatile void **isr_vectors	   = 0;
		inline static uint32_t		  DEBUG_ACTIVE_ISR = 0;

		inline static void (*post_mortem)()		   = nullptr;
		inline static volatile bool in_post_mortem = false;

		static constexpr uint32_t CFSR_STACKING = (1u << 3) | (1u << 4) | (1u << 5) | (1u << 11) | (1u << 12) | (1u << 13);

		static bool can_post_mortem(const uint32_t *frame, uint32_t exc_return)
		{
			using fault_t = mcu_arm_v7m_fault;

			auto *scb = get_scb();

			return post_mortem && !in_post_mortem && !get_coredebug()->debugger_attached()
				   && (exc_return & fault_t::EXC_RETURN_SPSEL) == 0 && (scb->CFSR & CFSR_STACKING) == 0
				   && !reg_cm7_scb::HFSR_VECTTBL::get(scb->HFSR)
				   && fault_t::ipsr(frame[fault_t::FRAME_XPSR]) != (uint32_t)irq2isv(irq_h750::OTG_FS);
		}

		static void hardfault_report(uint32_t *frame, uint32_t exc_return)
		{
			using fault_t = mcu_arm_v7m_fault;

			auto *scb = get_scb();

			bug_print("hardfault pc %x lr %x psr %x sp %x exc %x hfsr %x cfsr %x mmfar %x bfar %x\n",
					  (unsigned)frame[fault_t::FRAME_PC],
					  (unsigned)frame[fault_t::FRAME_LR],
					  (unsigned)frame[fault_t::FRAME_XPSR],
					  (unsigned)(uint32_t)frame,
					  (unsigned)exc_return,
					  (unsigned)scb->HFSR,
					  (unsigned)scb->CFSR,
					  (unsigned)scb->MMFAR,
					  (unsigned)scb->BFAR);

			if constexpr(!bug::is_build_ship())
			{
				bug_print("bt");
				fault_t::trace(frame,
							   exc_return,
							   mem_h750::STACK_BOT,
							   mem_h750::STACK_TOP,
							   (uint32_t)&_stext,
							   (uint32_t)&_etext,
							   [](uint32_t addr, bool interrupted) {
								   if(interrupted)
								   {
									   bug_print(" |");
								   }
								   bug_print(" %x", (unsigned)addr);
							   });
				bug_print("\n");
			}

			if(can_post_mortem(frame, exc_return))
			{
				in_post_mortem = true;
				scb->CFSR	   = scb->CFSR;
				scb->HFSR	   = scb->HFSR;
				fault_t::redirect(frame, post_mortem);
				return;
			}

			bug::abort();
			reset_handler();
		}

		__attribute__((naked)) static void irq_hardfault()
		{
			__asm__ volatile(
				"tst lr, #4\n"
				"ite eq\n"
				"mrseq r0, msp\n"
				"mrsne r0, psp\n"
				"mov r1, lr\n"
				"b %0\n"
				:
				: "i"(&hardfault_report));
		}

		static void irq_unused()
		{
			auto *scb = get_scb();

			DEBUG_ACTIVE_ISR = scb->ICSR & 511;

			bug_print("Unused IRQ %d\n", DEBUG_ACTIVE_ISR);

			bug::abort();

			reset_handler();
		}

		static void init_isrv()
		{
			auto *scb = get_scb();

			isr_vectors = (volatile void **)mem_h750::alloc_dma<void *>((size_t)isv_h750::COUNT, "isrv");

			lac_assert_align(isr_vectors, 256);

			mcu_arm_v7m::fill_isrv(
				isr_vectors, (size_t)isv_h750::COUNT, mem_h750::STACK_TOP, reset_handler, irq_unused, irq_hardfault);

			scb->VTOR = (uint32_t)isr_vectors;

			mcu_arm_v7m::enable_irq();
		}

		static void set_isv(isv_h750 i, void (*v)())
		{
			lac_assert(isr_vectors);

			bug_print("%d %p\n", i, v);

			mcu_arm_v7m::disable_irq();
			isr_vectors[(size_t)i] = (volatile void *)v;
			std::atomic_thread_fence(std::memory_order_seq_cst);
			mcu_arm_v7m::dsb();
			mcu_arm_v7m::enable_irq();
		}

		static void set_system_priority(isv_h750 i, uint32_t priority)
		{
			lac_assert(i >= isv_h750::MEM_MANAGE && i <= isv_h750::SYS_TICK);
			get_scb()->SHPR[(size_t)i - (size_t)isv_h750::MEM_MANAGE]
				= (uint8_t)((priority << (8U - reg_h7_nvic::__NVIC_PRIO_BITS)) & 0xFF);
		}

		static bool check_isv(isv_h750 i)
		{
			lac_assert(isr_vectors);
			return isr_vectors[(size_t)i] != (volatile void *)irq_unused && isr_vectors[(size_t)i] != nullptr;
		}

		static void reset()
		{
			std::atomic_thread_fence(std::memory_order_seq_cst);

			auto *scb = get_scb();

			scb->AIRCR = reg_cm7_scb::AIRCR_VECTKEY::val(0x5FAUL) | reg_cm7_scb::AIRCR_SYSRESETREQ::BIT;

			std::atomic_thread_fence(std::memory_order_seq_cst);

			for(;;) {}
		}

		static void dfu_reboot()
		{
			get_pwr()->enable_bd();
			*get_rtc()->get_bkp(0) = DFU_MAGIC;
			std::atomic_thread_fence(std::memory_order_seq_cst);
			reset();
		}

		static void reset_handler()
		{
			mcu_arm_v7m::disable_irq();

			asm("ldr sp, =_estack");

			auto *rcc  = get_rcc();
			auto *bkp0 = get_rtc()->get_bkp(0);
			if(*bkp0 == DFU_MAGIC)
			{
				get_pwr()->enable_bd();
				*bkp0 = 0;
				reg_h7_rcc::RSR_RMVF::set(rcc->RSR);
				dfu_bootloader_vtor::get()->boot();
			}

			reg_h7_rcc::RSR_RMVF::set(rcc->RSR);

			auto *scb = get_scb();

			scb->CPACR = scb->CPACR | ((3 << (10 * 2)) | (3 << (11 * 2)));

			linker::init();

			main();

			bug::abort();

			for(;;)
				;
		}

		static void checkhz(uint32_t clk, uint32_t mhz) { lac_assert(clk == mhz * 1000000); }

		static void init_power()
		{
			auto *pwr = get_pwr();
			pwr->enable_ldo();
			pwr->max_power(get_syscfg());
			pwr->enable_usb33();
		}

		static void init_clock(bool hsi = false)
		{
			get_scb()->set_priority_grouping(3);
			get_flash()->set_latency(4, 0b10);

			auto clk = get_rcc();

			clk->set_mux_sysclk(reg_h7_rcc::SYS_CLK_HSI);

			clk->enable_hse();
			clk->enable_hsi48();
			clk->set_hsi_shift(2);
			clk->set_div_ahb(2);
			clk->set_div_apb3(2);
			clk->set_div_apb1(2);
			clk->set_div_apb2(2);
			clk->set_div_apb4(2);
			clk->set_div_sysclk(1);

			clk->set_mux_fmc(reg_h7_rcc::FMC_PLL2R);
			clk->set_mux_quadspi(reg_h7_rcc::QUADSPI_HCLK3);
			clk->set_mux_sdmmc1(reg_h7_rcc::SDMMC1_PLL1Q);
			clk->set_mux_sai1(reg_h7_rcc::SAI1_PLL3P);
			clk->set_mux_sai2(reg_h7_rcc::SAI2_PLL3P);
			clk->set_mux_spi1(reg_h7_rcc::SPI1_PLL1Q);
			clk->set_mux_usart16(reg_h7_rcc::USART1_PCLK2);
			clk->set_mux_usart234578(reg_h7_rcc::USART2_PCLK1);
			clk->set_mux_i2c1(reg_h7_rcc::I2C1_PCLK1);
			clk->set_mux_usb(reg_h7_rcc::USB_RC48);
			clk->set_mux_adc(reg_h7_rcc::ADC_PLL2P);

			clk->disable_pll1();
			clk->disable_pll2();
			clk->disable_pll3();

			clk->set_mux_pll_src(hsi ? reg_h7_rcc::PLL_SRC_HSI : reg_h7_rcc::PLL_SRC_HSE);

			clk->setup_pll1(1, 60 / SYS_DIV, 2, 3, 2, 0);
			clk->setup_pll2(4, 200, 20, 4, 4, 0);
			clk->setup_pll3(6, 294, 16, 4, 4, 0);
			clk->set_frac_pll3(7471);

			clk->enable_pll1();
			clk->enable_pll2();
			clk->enable_pll3();

			clk->set_mux_sysclk(reg_h7_rcc::SYS_CLK_PLL);

			auto *cfg = get_syscfg();
			clk->enable_syscfg();
			clk->enable_csi();
			cfg->enable_cccsr();

			tim2_t::freerun();

			mcu_h750<1>::get_rcc()->enable_gpio_all();

			if constexpr(bug::is_build_debug())
			{
				check_clock(rcc_t::get_hz_cpu(), SYS_CLK_HZ);
				check_clock(rcc_t::get_hz_apb2_tim(), TIM_CLK_HZ);
				check_clock(rcc_t::get_hz_sai1(), 49151985);
				check_clock(rcc_t::get_hz_apb1(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_apb2(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_apb3(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_apb4(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_ahb1(), 240000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_ahb4(), 240000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_axi(), 240000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_sai1(), 49151985);
				check_clock(rcc_t::get_hz_sai2(), 49151985);
				check_clock(rcc_t::get_hz_sai4a(), 320000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_sai4b(), 320000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_rng(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_i2c1(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_i2c4(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_spdifrx1(), 320000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_quadspi(), 240000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_swp(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_dfsdm(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_sdmmc1(), 320000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_usart1(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_usart2(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_lpuart1(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_lptim1(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_lptim2(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_lptim3(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_spi6(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_spi4(), 120000000 / SYS_DIV);
				check_clock(rcc_t::get_hz_adc(), 40000000);
				check_clock(rcc_t::get_hz_usb(), 48000000);
				check_clock(rcc_t::get_hz_cec(), 0);
				check_clock(rcc_t::get_hz_fdcan(), 16000000);
				check_clock(rcc_t::get_hz_hrtim(), 240000000 / SYS_DIV);
			}
		}

		static void check_clock(uint32_t clock, uint32_t expected)
		{
			if(clock != expected)
			{
				bug_print("Clock check failed: %u != %u\n", clock, expected);
				lac_assert(false);
			}
		}

		template <typename ext_ram_t>
		void init_mem()
		{
			ext_ram_t::init();

			get_pwr()->enable_bd();
			get_rcc()->enable_bkpsram();
			mem_h750::instance = &allocator;
			linker::usage(allocator);
			null_zone = allocator.alloc(mem_h750::ZONE_ITCM, 16, "null ptr");
			for(const auto &d : mem_h750::ZONES)
			{
				switch(d.cache)
				{
					default:
					case RAM_DEFAULT: break;
					case RAM_CACHE: get_mpu()->set_cache(d.addr, d.size); break;
					case RAM_NOCACHE: get_mpu()->set_nocache(d.addr, d.size); break;
				}
			}
			if(ext_ram_t::SIZE == 0)
			{
				allocator.enable(mem_h750::ZONE_FMC, 0);
			}

			auto *scb = get_scb();

			scb->enable_dcache();
			scb->enable_icache();
		}

		constexpr static uint32_t BOOTLOADER_KEY = 0x7A3F2B1C;
		constexpr static uint32_t DFU_MAGIC		 = 0x38509349;

		struct dfu_bootloader_vtor
		{
			volatile uint32_t stack;
			void (*reset_handler)();

			static dfu_bootloader_vtor *get() { return (dfu_bootloader_vtor *)0x1FF09800; }

			void boot()
			{
				mcu_arm_v7m::set_msp(stack);
				reset_handler();
			}
		};

		inline static this_t *instance = 0;

		mcu_h750() : mcu_h750(mcu_h750_no_ext_ram<this_t>{}) {}

		template <typename ext_ram_t>
		mcu_h750(ext_ram_t)
		{
			lac_assert(!instance);

			init_power();
			init_clock(false);
			init_mem<ext_ram_t>();
			init_isrv();
			mcu_arm_v7m::enable_ftz();
			mcu_arm_v7m::enable_irq();
			instance = this;
		}
	};

	static_assert(mcu_h750<1>::get_trigger_bit(adr_h750::TIM2) == 2);
	static_assert(mcu_h750<1>::get_trigger_bit(adr_h750::LPTIM2) == 12);
	static_assert(mcu_h750<1>::get_gpio_index(adr_h750::GPIOK) == 10);
	static_assert(mcu_h750<1>::get_tim_irq_up(adr_h750::TIM2) == irq_h750::TIM2);
	static_assert(mcu_h750<1>::get_dma_isv(DMA1_STREAM_0) == isv_h750::DMA1_STREAM0);
	static_assert(mcu_h750<1>::get_adc_irq(adr_h750::ADC3) == irq_h750::ADC3);

	using mcu = mcu_h750<1>;

} // namespace lac