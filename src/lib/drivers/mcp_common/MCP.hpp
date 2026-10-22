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
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/device/i2c.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/i2c_spi_buses.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/gpio_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/gpio_in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/gpio_out.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/PublicationMulti.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/perf/perf_counter.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform/gpio/mcp.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/module.h>

ਸਥਿਰ constexpr uint8_t I2_C_ਪਤਾ_MCP23009 = 0x25;
ਸਥਿਰ constexpr uint8_t I2_C_ਪਤਾ_MCP23017 = 0x27;

ਗਿਣਤੀ ਵਰਗ
PinType : uint8_t {
	Output,
	ਨਿਵੇਸ਼_2,
	ਨਿਵੇਸ਼_Pull_ਉੱਪਰ,
};

ਬਣਤਰ MCP230XX_config_t {
	uint16_t device_type;
	uint8_t i2c_addr = 0;
	uint8_t i2c_bus = 0;
	uint8_t ਪਹਿਲਾ_minor = 0;
	uint8_t ਅੰਕ_pins = 8;
	ਪੂਰਨ ਅੰਕ_banks = 1;
	uint16_t interval = 10;
	mcp_gpio_dev_s _gpio_handle[16];
	CallbackHandler *_callback_handler = ਖਾਲੀ_ਸੂਚਕ;

	uint16_t direction = 0xFFFF;
	uint16_t state = 0x0000;
	uint16_t pullup = 0x0000;
};

ਵਰਗ MCP230XX :  ਜਨਤਕ device::I2C, ਜਨਤਕ I2CSPIDriver<MCP230XX>
{
ਜਨਤਕ:
	MCP230XX(ਅਚਲ I2CSPIDriverConfig &config);
	ਆਭਾਸੀ ~MCP230XX();
	ਸਥਿਰ I2_CSPIDriver_ਅਧਾਰ *instantiate(ਅਚਲ I2CSPIDriverConfig &config, ਪੂਰਨ runtime_instance);
	ਸਥਿਰ ਖਾਲੀ print_usage();

	ਖਾਲੀ ਲੜੀਵਾਰ_Impl();
	ਖਾਲੀ print_ਸਥਿਤੀ() override;
	ਖਾਲੀ custom_ਵਿਧੀ(ਅਚਲ BusCLIArguments &cli) override;
	ਪੂਰਨ probe();
	ਆਭਾਸੀ ਪੂਰਨ init();

ਸੁਰੱਖਿਅਤ:
	MCP230XX_config_t mcp_config;

ਨਿਜੀ:
	uORB::Subscription_Callback_Work_ਵਸਤੂ gpio_ਬਾਹਰਲਾ_sub{ਇਹ, ORB_ID(gpio_ਬਾਹਰਲਾ)};
	uORB::Subscription_Callback_Work_ਵਸਤੂ _gpio_config_sub{ਇਹ, ORB_ID(gpio_config)};
	uORB::PublicationMulti<gpio_ਵਿੱਚ_s> ਮੰਜ਼ਿਲ_gpio_ਵਿੱਚ{ORB_ID(gpio_ਵਿੱਚ_2)};

	perf_counter_t ਚੱਕਰ_perf;
	perf_counter_t _comms_errors;
	perf_counter_t ਰਜਿਸਟਰ_check;

	ਆਭਾਸੀ ਪੂਰਨ configure(uint16_t ਨਕਾਬ, PinType type);
	ਆਭਾਸੀ ਪੂਰਨ ਪਠ(uint16_t *ਨਕਾਬ);
	ਪੂਰਨ read_reg(uint8_t ਪਤਾ, uint8_t &ਅੰਕੜੇ);
	ਪੂਰਨ ਲਿਖ(uint16_t ਨਕਾਬ_ਸੈੱਟ, uint16_t ਨਕਾਬ_clear);
	ਪੂਰਨ write_reg(uint8_t ਪਤਾ, uint8_t value);
	ਪੂਰਨ ਸੈੱਟ_ਉੱਪਰ_2();
	ਪੂਰਨ sanity_check();
	ਪੂਰਨ init_communication();
	ਪੂਰਨ init_uorb();
	ਖਾਲੀ cleanup_uorb();

	ਆਭਾਸੀ ਪੂਰਨ get_olat(ਪੂਰਨ bank, uint8_t *addr) = 0;
	ਆਭਾਸੀ ਪੂਰਨ get_gppu(ਪੂਰਨ bank, uint8_t *addr) = 0;
	ਆਭਾਸੀ ਪੂਰਨ get_iodir(ਪੂਰਨ bank, uint8_t *addr) = 0;
	ਆਭਾਸੀ ਪੂਰਨ get_gpio(ਪੂਰਨ bank, uint8_t *addr) = 0;
	ਆਭਾਸੀ ਪੂਰਨ get_probe_reg(uint8_t *addr) = 0;

	uint16_t _olat;
	uint16_t _iodir;
	uint16_t _gppu;

	uint16_t _checking_freq = 10;
	ਪੂਰਨ ਗਿਣਤੀ_4 = 0;

	ਗਿਣਤੀ ਵਰਗ STATE : uint8_t {
		INIT_COMMUNICATION,
		CONFIGURE,
		CHECK,
		RUNNING
	} _curr_state {STATE::INIT_COMMUNICATION};
};
