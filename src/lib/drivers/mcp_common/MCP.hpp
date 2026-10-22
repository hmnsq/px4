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

#अन्तर्भाव <stdint.h>
#अन्तर्भाव <drivers/device/i2c.h>
#अन्तर्भाव <px4_platform_common/i2c_spi_buses.h>
#अन्तर्भाव <uORB/topics/gpio_config.h>
#अन्तर्भाव <uORB/topics/gpio_in.h>
#अन्तर्भाव <uORB/topics/gpio_out.h>
#अन्तर्भाव <uORB/PublicationMulti.hpp>
#अन्तर्भाव <lib/perf/perf_counter.h>
#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <px4_platform/gpio/mcp.hpp>
#अन्तर्भाव <px4_platform_common/module.h>

स्थिर constexpr uint8_t I2C_ADDRESS_MCP23009 = 0x25;
स्थिर constexpr uint8_t I2C_ADDRESS_MCP23017 = 0x27;

गणना वग्ग
PinType : uint8_t {
	Output,
	निवेशः_2,
	निवेशः_Pull_Up,
};

संरचना MCP230XX_config_t {
	uint16_t device_type;
	uint8_t i2c_addr = 0;
	uint8_t i2c_bus = 0;
	uint8_t first_minor = 0;
	uint8_t अङ्कः_pins = 8;
	पूर्णाङ्क अङ्कः_banks = 1;
	uint16_t interval = 10;
	mcp_gpio_dev_s _gpio_handle[16];
	CallbackHandler *_callback_handler = nullptr;

	uint16_t direction = 0xFFFF;
	uint16_t state = 0x0000;
	uint16_t pullup = 0x0000;
};

वग्ग MCP230XX :  सार्वजनिक device::I2C, सार्वजनिक I2CSPIDriver<MCP230XX>
{
सार्वजनिक:
	MCP230XX(ध्रुव I2CSPIDriverConfig &config);
	आभास ~MCP230XX();
	स्थिर I2CSPIDriverBase *instantiate(ध्रुव I2CSPIDriverConfig &config, पूर्णाङ्क runtime_instance);
	स्थिर शून्य print_usage();

	शून्य RunImpl();
	शून्य print_status() override;
	शून्य custom_method(ध्रुव BusCLIArguments &cli) override;
	पूर्णाङ्क probe();
	आभास पूर्णाङ्क init();

रक्षित:
	MCP230XX_config_t mcp_config;

गुप्त:
	uORB::SubscriptionCallbackWorkItem _gpio_out_sub{एतत्, ORB_ID(gpio_out)};
	uORB::SubscriptionCallbackWorkItem _gpio_config_sub{एतत्, ORB_ID(gpio_config)};
	uORB::PublicationMulti<gpio_in_s> _to_gpio_in{ORB_ID(gpio_in)};

	perf_counter_t _cycle_perf;
	perf_counter_t _comms_errors;
	perf_counter_t _register_check;

	आभास पूर्णाङ्क configure(uint16_t mask, PinType type);
	आभास पूर्णाङ्क पठ(uint16_t *mask);
	पूर्णाङ्क read_reg(uint8_t address, uint8_t &data);
	पूर्णाङ्क लिख(uint16_t mask_set, uint16_t mask_clear);
	पूर्णाङ्क write_reg(uint8_t address, uint8_t value);
	पूर्णाङ्क set_up();
	पूर्णाङ्क sanity_check();
	पूर्णाङ्क init_communication();
	पूर्णाङ्क init_uorb();
	शून्य cleanup_uorb();

	आभास पूर्णाङ्क get_olat(पूर्णाङ्क bank, uint8_t *addr) = 0;
	आभास पूर्णाङ्क get_gppu(पूर्णाङ्क bank, uint8_t *addr) = 0;
	आभास पूर्णाङ्क get_iodir(पूर्णाङ्क bank, uint8_t *addr) = 0;
	आभास पूर्णाङ्क get_gpio(पूर्णाङ्क bank, uint8_t *addr) = 0;
	आभास पूर्णाङ्क get_probe_reg(uint8_t *addr) = 0;

	uint16_t _olat;
	uint16_t _iodir;
	uint16_t _gppu;

	uint16_t _checking_freq = 10;
	पूर्णाङ्क गणनम्_3 = 0;

	गणना वग्ग STATE : uint8_t {
		INIT_COMMUNICATION,
		CONFIGURE,
		CHECK,
		RUNNING
	} _curr_state {STATE::INIT_COMMUNICATION};
};
