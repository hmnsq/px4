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

#شامل <stdint.h>
#شامل <drivers/device/i2c.h>
#شامل <px4_platform_common/i2c_spi_buses.h>
#شامل <uORB/topics/gpio_config.h>
#شامل <uORB/topics/gpio_in.h>
#شامل <uORB/topics/gpio_out.h>
#شامل <uORB/PublicationMulti.hpp>
#شامل <lib/perf/perf_counter.h>
#شامل <drivers/drv_hrt.h>
#شامل <px4_platform/gpio/mcp.hpp>
#شامل <px4_platform_common/module.h>

ایستا constexpr uint8_t I2C_ADDRESS_MCP23009 = 0x25;
ایستا constexpr uint8_t I2C_ADDRESS_MCP23017 = 0x27;

شمارش رده
PinType : uint8_t {
	Output,
	درون_داد_2,
	درون_داد_Pull_Up,
};

ساختار MCP230XX_config_t {
	uint16_t device_type;
	uint8_t i2c_addr = 0;
	uint8_t i2c_bus = 0;
	uint8_t first_minor = 0;
	uint8_t رقم_pins = 8;
	صحیح رقم_banks = 1;
	uint16_t interval = 10;
	mcp_gpio_dev_s _gpio_handle[16];
	CallbackHandler *_callback_handler = nullptr;

	uint16_t direction = 0xFFFF;
	uint16_t state = 0x0000;
	uint16_t pullup = 0x0000;
};

رده MCP230XX :  عمومی device::I2C, عمومی I2CSPIDriver<MCP230XX>
{
عمومی:
	MCP230XX(ثابت I2CSPIDriverConfig &config);
	مجازی ~MCP230XX();
	ایستا I2CSPIDriverBase *instantiate(ثابت I2CSPIDriverConfig &config, صحیح runtime_instance);
	ایستا تهی print_usage();

	تهی RunImpl();
	تهی print_status() override;
	تهی custom_method(ثابت BusCLIArguments &cli) override;
	صحیح probe();
	مجازی صحیح init();

محافظتشده:
	MCP230XX_config_t mcp_config;

خصوصی:
	uORB::SubscriptionCallbackWorkItem _gpio_out_sub{این, ORB_ID(gpio_out)};
	uORB::SubscriptionCallbackWorkItem _gpio_config_sub{این, ORB_ID(gpio_config)};
	uORB::PublicationMulti<gpio_in_s> _to_gpio_in{ORB_ID(gpio_in)};

	perf_counter_t _cycle_perf;
	perf_counter_t _comms_errors;
	perf_counter_t _register_check;

	مجازی صحیح configure(uint16_t mask, PinType type);
	مجازی صحیح اقرأ(uint16_t *mask);
	صحیح read_reg(uint8_t address, uint8_t &data);
	صحیح اكتب(uint16_t mask_set, uint16_t mask_clear);
	صحیح write_reg(uint8_t address, uint8_t value);
	صحیح set_up();
	صحیح sanity_check();
	صحیح init_communication();
	صحیح init_uorb();
	تهی cleanup_uorb();

	مجازی صحیح get_olat(صحیح bank, uint8_t *addr) = 0;
	مجازی صحیح get_gppu(صحیح bank, uint8_t *addr) = 0;
	مجازی صحیح get_iodir(صحیح bank, uint8_t *addr) = 0;
	مجازی صحیح get_gpio(صحیح bank, uint8_t *addr) = 0;
	مجازی صحیح get_probe_reg(uint8_t *addr) = 0;

	uint16_t _olat;
	uint16_t _iodir;
	uint16_t _gppu;

	uint16_t _checking_freq = 10;
	صحیح شمار_3 = 0;

	شمارش رده STATE : uint8_t {
		INIT_COMMUNICATION,
		CONFIGURE,
		CHECK,
		RUNNING
	} _curr_state {STATE::INIT_COMMUNICATION};
};
