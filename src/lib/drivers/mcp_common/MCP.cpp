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
#ਅਨ੍ਤਰ੍ਭਾਵ "MCP.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/gpio/mcp23009/MCP23009.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(ਅਚਲ I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	ਚੱਕਰ_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_ਗਿਣਤੀ, MODULE_NAME": comms errors")),
	ਰਜਿਸਟਰ_check(perf_alloc(PC_ਗਿਣਤੀ, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	ਸਮਾਂ_ਸਾਰਣੀ_Clear();
	perf_free(ਚੱਕਰ_perf);
	perf_free(_comms_errors);
	perf_free(ਰਜਿਸਟਰ_check);
}

ਪੂਰਨ MCP230XX::init_uorb()
{
	ਜੇ (!_gpio_config_sub.ਰਜਿਸਟਰ_Callback() ||
	    !gpio_ਬਾਹਰਲਾ_sub.ਰਜਿਸਟਰ_Callback()) {
		PX4_ERR("callback registration failed");
		ਵਾਪਸ PX4_ERROR;
	}

	ਵਾਪਸ PX4_OK;
}

ਖਾਲੀ MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	gpio_ਬਾਹਰਲਾ_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.ਪਹਿਲਾ_minor, mcp_config.ਅੰਕ_pins, mcp_config._gpio_handle);
}

ਪੂਰਨ MCP230XX::ਪਠ(uint16_t *ਨਕਾਬ)
{
	*ਨਕਾਬ = 0;
	ਪੂਰਨ ret = PX4_OK;

	ਲਈ (ਪੂਰਨ i = 0; i < mcp_config.ਅੰਕ_banks; i++) {
		uint8_t gpio_addr;
		uint8_t ਅੰਕੜੇ;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, ਅੰਕੜੇ);

		*ਨਕਾਬ |= (((uint16_t) ਅੰਕੜੇ) << (8 * i));
	}

	ਵਾਪਸ ret;
}

ਪੂਰਨ MCP230XX::ਲਿਖ(ਅਚਲ uint16_t ਨਕਾਬ_ਸੈੱਟ, ਅਚਲ uint16_t ਨਕਾਬ_clear)
{
	ਪੂਰਨ ret = PX4_OK;
	_olat = (_olat & ~ਨਕਾਬ_clear) | ਨਕਾਬ_ਸੈੱਟ;

	ਲਈ (ਪੂਰਨ i = 0; i < mcp_config.ਅੰਕ_banks; i++) {
		uint8_t reg_addr;
		uint8_t ਅੰਕੜੇ;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		ਜੇ (ret != PX4_OK) {
			ਵਾਪਸ ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, ਅੰਕੜੇ);

		ਜੇ (ret != PX4_OK || ਅੰਕੜੇ != curr_olat) {
			ਵਾਪਸ PX4_ERROR;
		}
	}

	ਵਾਪਸ ret;
}

ਪੂਰਨ MCP230XX::read_reg(ਅਚਲ uint8_t ਪਤਾ, uint8_t &ਅੰਕੜੇ)
{
	ਪੂਰਨ ret = transfer(&ਪਤਾ, 1, &ਅੰਕੜੇ, 1);

	ਜੇ (ret != PX4_OK) {
		perf_ਗਿਣਤੀ(_comms_errors);
	}

	ਵਾਪਸ ret;
}

ਪੂਰਨ MCP230XX::write_reg(ਅਚਲ uint8_t ਪਤਾ, ਅਚਲ uint8_t value)
{
	uint8_t ਅੰਕੜੇ[2] = {ਪਤਾ, value};
	ਪੂਰਨ ret = transfer(ਅੰਕੜੇ, 2, ਖਾਲੀ_ਸੂਚਕ, 0);

	ਜੇ (ret != PX4_OK) {
		perf_ਗਿਣਤੀ(_comms_errors);
	}

	ਵਾਪਸ ret;
}

ਪੂਰਨ MCP230XX::configure(ਅਚਲ uint16_t ਨਕਾਬ, PinType type)
{
	ਚੋਣ (type) {
	ਹਾਲਤ PinType::ਨਿਵੇਸ਼_2:
		_iodir |= ਨਕਾਬ;
		_gppu &= ~ਨਕਾਬ;
		ਰੋਕੋ;

	ਹਾਲਤ PinType::ਨਿਵੇਸ਼_Pull_ਉੱਪਰ:
		_iodir |= ਨਕਾਬ;
		_gppu |= ਨਕਾਬ;
		ਰੋਕੋ;

	ਹਾਲਤ PinType::Output:
		_iodir &= ~ਨਕਾਬ;
		ਰੋਕੋ;

	ਮੂਲ:
		ਵਾਪਸ -EINVAL;
	}

	ਪੂਰਨ ret = PX4_OK;

	ਲਈ (ਪੂਰਨ i = 0; i < mcp_config.ਅੰਕ_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		ਜੇ (ret != PX4_OK) {
			ਵਾਪਸ ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		ਜੇ (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			ਵਾਪਸ ret;
		}
	}

	ਵਾਪਸ PX4_OK;
}

ਪੂਰਨ MCP230XX::init()
{
	ਪੂਰਨ ret = I2C::init();

	ਜੇ (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		ਵਾਪਸ ret;
	}

	ਸਮਾਂ_ਸਾਰਣੀ_Now();
	ਵਾਪਸ PX4_OK;
}

ਪੂਰਨ MCP230XX::init_communication()
{
	ਪੂਰਨ ret = mcp230_XX_ਰਜਿਸਟਰ_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.ਪਹਿਲਾ_minor, _iodir, mcp_config.ਅੰਕ_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	ਵਾਪਸ ret;
}

ਪੂਰਨ MCP230XX::ਸੈੱਟ_ਉੱਪਰ_2()
{
	ਪੂਰਨ ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	ਲਈ (ਪੂਰਨ i = 0; i < mcp_config.ਅੰਕ_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		ਜੇ (ret != PX4_OK) {
			ਵਾਪਸ ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		ਜੇ (ret != PX4_OK) {
			ਵਾਪਸ ret;
		}
	}

	ਵਾਪਸ ret;
}

ਪੂਰਨ MCP230XX::sanity_check()
{
	ਪੂਰਨ ret = PX4_OK;

	ਲਈ (ਪੂਰਨ i = 0; i < mcp_config.ਅੰਕ_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		uint8_t iodir_addr;
		uint8_t olat_addr;
		uint8_t gppu_addr;

		uint8_t read_iodir;
		uint8_t read_olat;
		uint8_t read_gppu;

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		ret |= read_reg(iodir_addr, read_iodir);
		ret |= read_reg(olat_addr, read_olat);
		ret |= read_reg(gppu_addr, read_gppu);

		ਜੇ (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_ਗਿਣਤੀ(ਰਜਿਸਟਰ_check);
			ਵਾਪਸ PX4_ERROR;
		}
	}

	ਵਾਪਸ ret;
}

ਪੂਰਨ MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t ਅੰਕੜੇ;
	uint8_t addr;
	ਪੂਰਨ ret = PX4_OK;

	ਲਈ (ਪੂਰਨ i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		ਜੇ (ret == PX4_OK) {
			ret = read_reg(addr, ਅੰਕੜੇ);

			ਜੇ (ret == PX4_OK) {
				ਵਾਪਸ PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	ਵਾਪਸ PX4_ERROR;
}

ਖਾਲੀ MCP230XX::ਲੜੀਵਾਰ_Impl()
{
	ਜੇ (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		ਵਾਪਸ;
	}

	ਪੂਰਨ ret = PX4_OK;

	ਚੋਣ (_curr_state) {
	ਹਾਲਤ STATE::INIT_COMMUNICATION:
		ret = init_communication();

		ਜੇ (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			ਸਮਾਂ_ਸਾਰਣੀ_Now();

		} ਨਹੀਂਤਾਂ {
			ਸਮਾਂ_ਸਾਰਣੀ_Delayed(mcp_config.interval * 1000);
		}

		ਰੋਕੋ;

	ਹਾਲਤ STATE::CONFIGURE:
		ret = ਸੈੱਟ_ਉੱਪਰ_2();

		ਜੇ (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			ਸਮਾਂ_ਸਾਰਣੀ_Now();

		} ਨਹੀਂਤਾਂ {
			ਸਮਾਂ_ਸਾਰਣੀ_Delayed(mcp_config.interval * 1000);
		}

		ਰੋਕੋ;

	ਹਾਲਤ STATE::CHECK:
		ret = sanity_check();

		ਜੇ (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			ਸਮਾਂ_ਸਾਰਣੀ_On_Interval(mcp_config.interval * 1000);

		} ਨਹੀਂਤਾਂ {
			_curr_state = STATE::CONFIGURE;
			ਸਮਾਂ_ਸਾਰਣੀ_Delayed(mcp_config.interval * 1000);
		}

		ਰੋਕੋ;

	ਹਾਲਤ STATE::RUNNING:
		perf_begin(ਚੱਕਰ_perf);
		gpio_config_s config;

		ਜੇ (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::ਨਿਵੇਸ਼_2;

			ਚੋਣ (config.config) {
			ਹਾਲਤ config.ਨਿਵੇਸ਼_PULLUP:	type = PinType::ਨਿਵੇਸ਼_Pull_ਉੱਪਰ; ਰੋਕੋ;

			ਹਾਲਤ config.OUTPUT:		type = PinType::Output; ਰੋਕੋ;
			}

			ret |= ਲਿਖ(config.state, config.ਨਕਾਬ);
			ret |= configure(config.ਨਕਾਬ, type);
		}

		gpio_ਬਾਹਰਲਾ_s output;

		ਜੇ (gpio_ਬਾਹਰਲਾ_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= ਲਿਖ(output.state, output.ਨਕਾਬ);
		}

		{
			gpio_ਵਿੱਚ_s gpio_ਵਿੱਚ;
			gpio_ਵਿੱਚ.timestamp = hrt_absolute_ਸਮਾਂ();
			gpio_ਵਿੱਚ.device_id = get_device_id();
			uint16_t ਨਿਵੇਸ਼ = 0;

			ਜੇ (ਪਠ(&ਨਿਵੇਸ਼) == PX4_OK) {
				gpio_ਵਿੱਚ.state = ਨਿਵੇਸ਼;
				ਮੰਜ਼ਿਲ_gpio_ਵਿੱਚ.publish(gpio_ਵਿੱਚ);
			}
		}

		ਗਿਣਤੀ_4++;

		ਜੇ (ਗਿਣਤੀ_4 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			ਗਿਣਤੀ_4 = 0;
			ਸਮਾਂ_ਸਾਰਣੀ_Clear();
			ਸਮਾਂ_ਸਾਰਣੀ_Now();
		}

		perf_ਸਿਰਾ(ਚੱਕਰ_perf);
	}
}

ਖਾਲੀ MCP230XX::print_ਸਥਿਤੀ()
{
	I2_CSPIDriver_ਅਧਾਰ::print_ਸਥਿਤੀ();
	perf_print_counter(ਚੱਕਰ_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(ਰਜਿਸਟਰ_check);
}

ਖਾਲੀ MCP230XX::custom_ਵਿਧੀ(ਅਚਲ BusCLIArguments &cli)
{
	ਚੋਣ (cli.custom1) {
	ਹਾਲਤ 0: {
			// write: only pins configured as outputs are affected
			ਅਚਲ uint16_t outputs = (uint16_t)~_iodir;
			ਅਚਲ uint16_t state = (uint16_t)cli.custom2;

			ਜੇ (ਲਿਖ(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			ਰੋਕੋ;
		}
	}
}

ਖਾਲੀ MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(ਸੱਚ, ਝੂਠ);
	PRINT_MODULE_USAGE_PARAMS_I2_C_ਪਤਾ(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", ਸੱਚ);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", ਸੱਚ);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", ਸੱਚ);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", ਸੱਚ);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", ਸੱਚ);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2_CSPIDriver_ਅਧਾਰ *MCP230XX::instantiate(ਅਚਲ I2CSPIDriverConfig &config, ਪੂਰਨ runtime_instance)
{
	MCP230XX *instance = ਖਾਲੀ_ਸੂਚਕ;

	ਚੋਣ (config.devid_driver_ਸੂਚਕਾਂਕ) {
	ਹਾਲਤ DRV_GPIO_DEVTYPE_MCP23009:
		instance = ਨਵਾਂ MCP23009(config);
		ਰੋਕੋ;

	ਹਾਲਤ DRV_GPIO_DEVTYPE_MCP23017:
		instance = ਨਵਾਂ MCP23017(config);
		ਰੋਕੋ;

	ਮੂਲ:
		instance = ਖਾਲੀ_ਸੂਚਕ;
		ਰੋਕੋ;
	}

	ਜੇ (!instance) {
		PX4_ERR("alloc failed");
		ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
	}

	MCP230XX_config_t tmp_config = *static_cast<ਅਚਲ MCP230XX_config_t *>(config.custom_ਅੰਕੜੇ);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_ਪਤਾ;

	ਜੇ (OK != instance->init()) {
		ਮਿਟਾਓ instance;
		ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	ਵਾਪਸ instance;
}
