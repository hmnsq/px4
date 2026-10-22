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

#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <drivers/device/i2c.h>
#அந்தர்பாவ <px4_platform_common/i2c_spi_buses.h>
#அந்தர்பாவ <uORB/topics/gpio_config.h>
#அந்தர்பாவ <uORB/topics/gpio_in.h>
#அந்தர்பாவ <uORB/topics/gpio_out.h>
#அந்தர்பாவ <uORB/PublicationMulti.hpp>
#அந்தர்பாவ <lib/perf/perf_counter.h>
#அந்தர்பாவ <drivers/drv_hrt.h>
#அந்தர்பாவ <px4_platform/gpio/mcp.hpp>
#அந்தர்பாவ <px4_platform_common/module.h>

நிலையான constexpr uint8_t I2C_ADDRESS_MCP23009 = 0x25;
நிலையான constexpr uint8_t I2C_ADDRESS_MCP23017 = 0x27;

எண்ணிக்கை வகுப்பு
PinType : uint8_t {
	Output,
	உள்ளீடு_2,
	உள்ளீடு_Pull_Up,
};

கட்டமைப்பு MCP230XX_config_t {
	uint16_t device_type;
	uint8_t i2c_addr = 0;
	uint8_t i2c_bus = 0;
	uint8_t first_minor = 0;
	uint8_t இலக்கம்_pins = 8;
	முழு இலக்கம்_banks = 1;
	uint16_t interval = 10;
	mcp_gpio_dev_s _gpio_handle[16];
	CallbackHandler *_callback_handler = nullptr;

	uint16_t direction = 0xFFFF;
	uint16_t state = 0x0000;
	uint16_t pullup = 0x0000;
};

வகுப்பு MCP230XX :  பொது device::I2C, பொது I2CSPIDriver<MCP230XX>
{
பொது:
	MCP230XX(மாறிலி I2CSPIDriverConfig &config);
	மெய்நிகர் ~MCP230XX();
	நிலையான I2CSPIDriverBase *instantiate(மாறிலி I2CSPIDriverConfig &config, முழு runtime_instance);
	நிலையான வெறுமை print_usage();

	வெறுமை RunImpl();
	வெறுமை print_status() override;
	வெறுமை custom_method(மாறிலி BusCLIArguments &cli) override;
	முழு probe();
	மெய்நிகர் முழு init();

பாதுகாக்கப்பட்ட:
	MCP230XX_config_t mcp_config;

தனிப்பட்ட:
	uORB::SubscriptionCallbackWorkItem _gpio_out_sub{இது, ORB_ID(gpio_out)};
	uORB::SubscriptionCallbackWorkItem _gpio_config_sub{இது, ORB_ID(gpio_config)};
	uORB::PublicationMulti<gpio_in_s> _to_gpio_in{ORB_ID(gpio_in)};

	perf_counter_t _cycle_perf;
	perf_counter_t _comms_errors;
	perf_counter_t _register_check;

	மெய்நிகர் முழு configure(uint16_t mask, PinType type);
	மெய்நிகர் முழு பட(uint16_t *mask);
	முழு read_reg(uint8_t address, uint8_t &data);
	முழு லிக(uint16_t mask_set, uint16_t mask_clear);
	முழு write_reg(uint8_t address, uint8_t value);
	முழு set_up();
	முழு sanity_check();
	முழு init_communication();
	முழு init_uorb();
	வெறுமை cleanup_uorb();

	மெய்நிகர் முழு get_olat(முழு bank, uint8_t *addr) = 0;
	மெய்நிகர் முழு get_gppu(முழு bank, uint8_t *addr) = 0;
	மெய்நிகர் முழு get_iodir(முழு bank, uint8_t *addr) = 0;
	மெய்நிகர் முழு get_gpio(முழு bank, uint8_t *addr) = 0;
	மெய்நிகர் முழு get_probe_reg(uint8_t *addr) = 0;

	uint16_t _olat;
	uint16_t _iodir;
	uint16_t _gppu;

	uint16_t _checking_freq = 10;
	முழு மொத்தம்_3 = 0;

	எண்ணிக்கை வகுப்பு STATE : uint8_t {
		INIT_COMMUNICATION,
		CONFIGURE,
		CHECK,
		RUNNING
	} _curr_state {STATE::INIT_COMMUNICATION};
};
