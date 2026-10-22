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

#অন্তর্ভাৱ <stdint.h>
#অন্তর্ভাৱ <drivers/device/i2c.h>
#অন্তর্ভাৱ <px4_platform_common/i2c_spi_buses.h>
#অন্তর্ভাৱ <uORB/topics/gpio_config.h>
#অন্তর্ভাৱ <uORB/topics/gpio_in.h>
#অন্তর্ভাৱ <uORB/topics/gpio_out.h>
#অন্তর্ভাৱ <uORB/PublicationMulti.hpp>
#অন্তর্ভাৱ <lib/perf/perf_counter.h>
#অন্তর্ভাৱ <drivers/drv_hrt.h>
#অন্তর্ভাৱ <px4_platform/gpio/mcp.hpp>
#অন্তর্ভাৱ <px4_platform_common/module.h>

স্থির constexpr uint8_t I2C_ADDRESS_MCP23009 = 0x25;
স্থির constexpr uint8_t I2C_ADDRESS_MCP23017 = 0x27;

গণনা শ্ৰেণী
PinType : uint8_t {
	Output,
	প্ৰবেশ_2,
	প্ৰবেশ_Pull_Up,
};

সংরচনা MCP230XX_config_t {
	uint16_t device_type;
	uint8_t i2c_addr = 0;
	uint8_t i2c_bus = 0;
	uint8_t first_minor = 0;
	uint8_t অঙ্ক_pins = 8;
	পূর্ণাঙ্ক অঙ্ক_banks = 1;
	uint16_t interval = 10;
	mcp_gpio_dev_s _gpio_handle[16];
	CallbackHandler *_callback_handler = nullptr;

	uint16_t direction = 0xFFFF;
	uint16_t state = 0x0000;
	uint16_t pullup = 0x0000;
};

শ্ৰেণী MCP230XX :  সার্ৱজনিক device::I2C, সার্ৱজনিক I2CSPIDriver<MCP230XX>
{
সার্ৱজনিক:
	MCP230XX(ধ্রুৱ I2CSPIDriverConfig &config);
	আভাস ~MCP230XX();
	স্থির I2CSPIDriverBase *instantiate(ধ্রুৱ I2CSPIDriverConfig &config, পূর্ণাঙ্ক runtime_instance);
	স্থির শূন্য print_usage();

	শূন্য RunImpl();
	শূন্য print_status() override;
	শূন্য custom_method(ধ্রুৱ BusCLIArguments &cli) override;
	পূর্ণাঙ্ক probe();
	আভাস পূর্ণাঙ্ক init();

রক্ষিত:
	MCP230XX_config_t mcp_config;

গুপ্ত:
	uORB::SubscriptionCallbackWorkItem _gpio_out_sub{এতত্, ORB_ID(gpio_out)};
	uORB::SubscriptionCallbackWorkItem _gpio_config_sub{এতত্, ORB_ID(gpio_config)};
	uORB::PublicationMulti<gpio_in_s> _to_gpio_in{ORB_ID(gpio_in)};

	perf_counter_t _cycle_perf;
	perf_counter_t _comms_errors;
	perf_counter_t _register_check;

	আভাস পূর্ণাঙ্ক configure(uint16_t mask, PinType type);
	আভাস পূর্ণাঙ্ক পঠ(uint16_t *mask);
	পূর্ণাঙ্ক read_reg(uint8_t address, uint8_t &data);
	পূর্ণাঙ্ক লিখ(uint16_t mask_set, uint16_t mask_clear);
	পূর্ণাঙ্ক write_reg(uint8_t address, uint8_t value);
	পূর্ণাঙ্ক set_up();
	পূর্ণাঙ্ক sanity_check();
	পূর্ণাঙ্ক init_communication();
	পূর্ণাঙ্ক init_uorb();
	শূন্য cleanup_uorb();

	আভাস পূর্ণাঙ্ক get_olat(পূর্ণাঙ্ক bank, uint8_t *addr) = 0;
	আভাস পূর্ণাঙ্ক get_gppu(পূর্ণাঙ্ক bank, uint8_t *addr) = 0;
	আভাস পূর্ণাঙ্ক get_iodir(পূর্ণাঙ্ক bank, uint8_t *addr) = 0;
	আভাস পূর্ণাঙ্ক get_gpio(পূর্ণাঙ্ক bank, uint8_t *addr) = 0;
	আভাস পূর্ণাঙ্ক get_probe_reg(uint8_t *addr) = 0;

	uint16_t _olat;
	uint16_t _iodir;
	uint16_t _gppu;

	uint16_t _checking_freq = 10;
	পূর্ণাঙ্ক গুনতি_3 = 0;

	গণনা শ্ৰেণী STATE : uint8_t {
		INIT_COMMUNICATION,
		CONFIGURE,
		CHECK,
		RUNNING
	} _curr_state {STATE::INIT_COMMUNICATION};
};
