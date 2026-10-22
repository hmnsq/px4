/****************************************************************************
 *
 *   Copyright (C) 2025 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/
#pragma once

#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <drivers/device/i2c.h>
#অন্তর্ভাব <px4_platform_common/i2c_spi_buses.h>
#অন্তর্ভাব <uORB/topics/gpio_config.h>
#অন্তর্ভাব <uORB/topics/gpio_in.h>
#অন্তর্ভাব <uORB/topics/gpio_out.h>
#অন্তর্ভাব <uORB/PublicationMulti.hpp>
#অন্তর্ভাব <lib/perf/perf_counter.h>
#অন্তর্ভাব <drivers/drv_hrt.h>
#অন্তর্ভাব <px4_platform/gpio/mcp.hpp>
#অন্তর্ভাব <px4_platform_common/module.h>

স্থির constexpr uint8_t I2C_ADDRESS_MCP23009 = 0x25;
স্থির constexpr uint8_t I2C_ADDRESS_MCP23017 = 0x27;

গণনা শ্রেণী
PinType : uint8_t {
	Output,
	প্রবেশ_2,
	প্রবেশ_Pull_Up,
};

কাঠামো MCP230XX_config_t {
	uint16_t device_type;
	uint8_t i2c_addr = 0;
	uint8_t i2c_bus = 0;
	uint8_t first_minor = 0;
	uint8_t অঙ্ক_pins = 8;
	পূর্ণসংখ্যা অঙ্ক_banks = 1;
	uint16_t interval = 10;
	mcp_gpio_dev_s _gpio_handle[16];
	CallbackHandler *_callback_handler = nullptr;

	uint16_t direction = 0xFFFF;
	uint16_t state = 0x0000;
	uint16_t pullup = 0x0000;
};

শ্রেণী MCP230XX :  সর্বজনীন device::I2C, সর্বজনীন I2CSPIDriver<MCP230XX>
{
সর্বজনীন:
	MCP230XX(ধ্রুবক I2CSPIDriverConfig &config);
	আভাসী ~MCP230XX();
	স্থির I2CSPIDriverBase *instantiate(ধ্রুবক I2CSPIDriverConfig &config, পূর্ণসংখ্যা runtime_instance);
	স্থির শূন্য print_usage();

	শূন্য RunImpl();
	শূন্য print_status() override;
	শূন্য custom_method(ধ্রুবক BusCLIArguments &cli) override;
	পূর্ণসংখ্যা probe();
	আভাসী পূর্ণসংখ্যা init();

সুরক্ষিত:
	MCP230XX_config_t mcp_config;

ব্যক্তিগত:
	uORB::SubscriptionCallbackWorkItem _gpio_out_sub{এই, ORB_ID(gpio_out)};
	uORB::SubscriptionCallbackWorkItem _gpio_config_sub{এই, ORB_ID(gpio_config)};
	uORB::PublicationMulti<gpio_in_s> _to_gpio_in{ORB_ID(gpio_in)};

	perf_গণক_t _cycle_perf;
	perf_গণক_t _comms_errors;
	perf_গণক_t _register_check;

	আভাসী পূর্ণসংখ্যা configure(uint16_t mask, PinType type);
	আভাসী পূর্ণসংখ্যা পঠ(uint16_t *mask);
	পূর্ণসংখ্যা read_reg(uint8_t address, uint8_t &তথ্য);
	পূর্ণসংখ্যা লিখ(uint16_t mask_set, uint16_t mask_clear);
	পূর্ণসংখ্যা write_reg(uint8_t address, uint8_t value);
	পূর্ণসংখ্যা set_up();
	পূর্ণসংখ্যা sanity_check();
	পূর্ণসংখ্যা init_communication();
	পূর্ণসংখ্যা init_uorb();
	শূন্য cleanup_uorb();

	আভাসী পূর্ণসংখ্যা get_olat(পূর্ণসংখ্যা bank, uint8_t *addr) = 0;
	আভাসী পূর্ণসংখ্যা get_gppu(পূর্ণসংখ্যা bank, uint8_t *addr) = 0;
	আভাসী পূর্ণসংখ্যা get_iodir(পূর্ণসংখ্যা bank, uint8_t *addr) = 0;
	আভাসী পূর্ণসংখ্যা get_gpio(পূর্ণসংখ্যা bank, uint8_t *addr) = 0;
	আভাসী পূর্ণসংখ্যা get_probe_reg(uint8_t *addr) = 0;

	uint16_t _olat;
	uint16_t _iodir;
	uint16_t _gppu;

	uint16_t _checking_freq = 10;
	পূর্ণসংখ্যা গুনতি_3 = 0;

	গণনা শ্রেণী STATE : uint8_t {
		INIT_COMMUNICATION,
		CONFIGURE,
		CHECK,
		RUNNING
	} _curr_state {STATE::INIT_COMMUNICATION};
};
