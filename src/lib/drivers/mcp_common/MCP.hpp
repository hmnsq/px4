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

#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <drivers/device/i2c.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/i2c_spi_buses.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/gpio_config.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/gpio_in.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/gpio_out.h>
#ଅନ୍ତର୍ଭାଵ <uORB/PublicationMulti.hpp>
#ଅନ୍ତର୍ଭାଵ <lib/perf/perf_counter.h>
#ଅନ୍ତର୍ଭାଵ <drivers/drv_hrt.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform/gpio/mcp.hpp>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/module.h>

ସ୍ଥିର constexpr uint8_t I2C_ADDRESS_MCP23009 = 0x25;
ସ୍ଥିର constexpr uint8_t I2C_ADDRESS_MCP23017 = 0x27;

ଗଣନା ଶ୍ରେଣୀ
PinType : uint8_t {
	Output,
	ନିବେଶ_2,
	ନିବେଶ_Pull_Up,
};

ସଂରଚନା MCP230XX_config_t {
	uint16_t device_type;
	uint8_t i2c_addr = 0;
	uint8_t i2c_bus = 0;
	uint8_t first_minor = 0;
	uint8_t ଅଙ୍କ_pins = 8;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_banks = 1;
	uint16_t interval = 10;
	mcp_gpio_dev_s _gpio_handle[16];
	CallbackHandler *_callback_handler = nullptr;

	uint16_t direction = 0xFFFF;
	uint16_t state = 0x0000;
	uint16_t pullup = 0x0000;
};

ଶ୍ରେଣୀ MCP230XX :  ସର୍ବସାଧାରଣ device::I2C, ସର୍ବସାଧାରଣ I2CSPIDriver<MCP230XX>
{
ସର୍ବସାଧାରଣ:
	MCP230XX(ଅଚଳ I2CSPIDriverConfig &config);
	ଆଭାସୀ ~MCP230XX();
	ସ୍ଥିର I2CSPIDriverBase *instantiate(ଅଚଳ I2CSPIDriverConfig &config, ପୂର୍ଣ୍ଣସଂଖ୍ୟା runtime_instance);
	ସ୍ଥିର ଶୂନ୍ୟ print_usage();

	ଶୂନ୍ୟ RunImpl();
	ଶୂନ୍ୟ print_status() override;
	ଶୂନ୍ୟ custom_method(ଅଚଳ BusCLIArguments &cli) override;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା probe();
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା init();

ସୁରକ୍ଷିତ:
	MCP230XX_config_t mcp_config;

ବ୍ୟକ୍ତିଗତ:
	uORB::SubscriptionCallbackWorkItem _gpio_out_sub{ଏହି, ORB_ID(gpio_out)};
	uORB::SubscriptionCallbackWorkItem _gpio_config_sub{ଏହି, ORB_ID(gpio_config)};
	uORB::PublicationMulti<gpio_in_s> _to_gpio_in{ORB_ID(gpio_in)};

	perf_counter_t _cycle_perf;
	perf_counter_t _comms_errors;
	perf_counter_t _register_check;

	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା configure(uint16_t mask, PinType type);
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ପଠ(uint16_t *mask);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_reg(uint8_t address, uint8_t &data);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଲିଖ(uint16_t mask_set, uint16_t mask_clear);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_reg(uint8_t address, uint8_t value);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_up();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା sanity_check();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_communication();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_uorb();
	ଶୂନ୍ୟ cleanup_uorb();

	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_olat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bank, uint8_t *addr) = 0;
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_gppu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bank, uint8_t *addr) = 0;
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_iodir(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bank, uint8_t *addr) = 0;
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_gpio(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bank, uint8_t *addr) = 0;
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_probe_reg(uint8_t *addr) = 0;

	uint16_t _olat;
	uint16_t _iodir;
	uint16_t _gppu;

	uint16_t _checking_freq = 10;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_4 = 0;

	ଗଣନା ଶ୍ରେଣୀ STATE : uint8_t {
		INIT_COMMUNICATION,
		CONFIGURE,
		CHECK,
		RUNNING
	} _curr_state {STATE::INIT_COMMUNICATION};
};
