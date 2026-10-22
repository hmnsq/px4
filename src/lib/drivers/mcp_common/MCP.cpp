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
#അന്തര്ഭാവ "MCP.hpp"
#അന്തര്ഭാവ <drivers/gpio/mcp23009/MCP23009.hpp>
#അന്തര്ഭാവ <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(സ്ഥിരം I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	_cycle_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_എണ്ണം, MODULE_NAME": comms errors")),
	_register_check(perf_alloc(PC_എണ്ണം, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	ScheduleClear();
	perf_free(_cycle_perf);
	perf_free(_comms_errors);
	perf_free(_register_check);
}

പൂർണ്ണസംഖ്യ MCP230XX::init_uorb()
{
	എങ്കിൽ (!_gpio_config_sub.registerCallback() ||
	    !_gpio_out_sub.registerCallback()) {
		PX4_ERR("callback registration failed");
		തിരികെ PX4_ERROR;
	}

	തിരികെ PX4_OK;
}

ശൂന്യം MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	_gpio_out_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.first_minor, mcp_config.അക്കം_pins, mcp_config._gpio_handle);
}

പൂർണ്ണസംഖ്യ MCP230XX::പഠ(uint16_t *mask)
{
	*mask = 0;
	പൂർണ്ണസംഖ്യ ret = PX4_OK;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < mcp_config.അക്കം_banks; i++) {
		uint8_t gpio_addr;
		uint8_t data;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, data);

		*mask |= (((uint16_t) data) << (8 * i));
	}

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ MCP230XX::ലിഖ(സ്ഥിരം uint16_t mask_set, സ്ഥിരം uint16_t mask_clear)
{
	പൂർണ്ണസംഖ്യ ret = PX4_OK;
	_olat = (_olat & ~mask_clear) | mask_set;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < mcp_config.അക്കം_banks; i++) {
		uint8_t reg_addr;
		uint8_t data;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		എങ്കിൽ (ret != PX4_OK) {
			തിരികെ ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, data);

		എങ്കിൽ (ret != PX4_OK || data != curr_olat) {
			തിരികെ PX4_ERROR;
		}
	}

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ MCP230XX::read_reg(സ്ഥിരം uint8_t address, uint8_t &data)
{
	പൂർണ്ണസംഖ്യ ret = transfer(&address, 1, &data, 1);

	എങ്കിൽ (ret != PX4_OK) {
		perf_എണ്ണം(_comms_errors);
	}

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ MCP230XX::write_reg(സ്ഥിരം uint8_t address, സ്ഥിരം uint8_t value)
{
	uint8_t data[2] = {address, value};
	പൂർണ്ണസംഖ്യ ret = transfer(data, 2, nullptr, 0);

	എങ്കിൽ (ret != PX4_OK) {
		perf_എണ്ണം(_comms_errors);
	}

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ MCP230XX::configure(സ്ഥിരം uint16_t mask, PinType type)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ PinType::നിവേശം_2:
		_iodir |= mask;
		_gppu &= ~mask;
		നിർത്തുക;

	അവസ്ഥ PinType::നിവേശം_Pull_Up:
		_iodir |= mask;
		_gppu |= mask;
		നിർത്തുക;

	അവസ്ഥ PinType::Output:
		_iodir &= ~mask;
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		തിരികെ -EINVAL;
	}

	പൂർണ്ണസംഖ്യ ret = PX4_OK;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < mcp_config.അക്കം_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		എങ്കിൽ (ret != PX4_OK) {
			തിരികെ ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		എങ്കിൽ (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			തിരികെ ret;
		}
	}

	തിരികെ PX4_OK;
}

പൂർണ്ണസംഖ്യ MCP230XX::init()
{
	പൂർണ്ണസംഖ്യ ret = I2C::init();

	എങ്കിൽ (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		തിരികെ ret;
	}

	ScheduleNow();
	തിരികെ PX4_OK;
}

പൂർണ്ണസംഖ്യ MCP230XX::init_communication()
{
	പൂർണ്ണസംഖ്യ ret = mcp230XX_register_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.first_minor, _iodir, mcp_config.അക്കം_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ MCP230XX::set_up()
{
	പൂർണ്ണസംഖ്യ ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < mcp_config.അക്കം_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		എങ്കിൽ (ret != PX4_OK) {
			തിരികെ ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		എങ്കിൽ (ret != PX4_OK) {
			തിരികെ ret;
		}
	}

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ MCP230XX::sanity_check()
{
	പൂർണ്ണസംഖ്യ ret = PX4_OK;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < mcp_config.അക്കം_banks; i++) {
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

		എങ്കിൽ (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_എണ്ണം(_register_check);
			തിരികെ PX4_ERROR;
		}
	}

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t data;
	uint8_t addr;
	പൂർണ്ണസംഖ്യ ret = PX4_OK;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		എങ്കിൽ (ret == PX4_OK) {
			ret = read_reg(addr, data);

			എങ്കിൽ (ret == PX4_OK) {
				തിരികെ PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	തിരികെ PX4_ERROR;
}

ശൂന്യം MCP230XX::RunImpl()
{
	എങ്കിൽ (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		തിരികെ;
	}

	പൂർണ്ണസംഖ്യ ret = PX4_OK;

	തിരഞ്ഞെടുപ്പ് (_curr_state) {
	അവസ്ഥ STATE::INIT_COMMUNICATION:
		ret = init_communication();

		എങ്കിൽ (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			ScheduleNow();

		} അല്ലെങ്കിൽ {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		നിർത്തുക;

	അവസ്ഥ STATE::CONFIGURE:
		ret = set_up();

		എങ്കിൽ (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			ScheduleNow();

		} അല്ലെങ്കിൽ {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		നിർത്തുക;

	അവസ്ഥ STATE::CHECK:
		ret = sanity_check();

		എങ്കിൽ (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			ScheduleOnInterval(mcp_config.interval * 1000);

		} അല്ലെങ്കിൽ {
			_curr_state = STATE::CONFIGURE;
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		നിർത്തുക;

	അവസ്ഥ STATE::RUNNING:
		perf_begin(_cycle_perf);
		gpio_config_s config;

		എങ്കിൽ (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::നിവേശം_2;

			തിരഞ്ഞെടുപ്പ് (config.config) {
			അവസ്ഥ config.നിവേശം_PULLUP:	type = PinType::നിവേശം_Pull_Up; നിർത്തുക;

			അവസ്ഥ config.OUTPUT:		type = PinType::Output; നിർത്തുക;
			}

			ret |= ലിഖ(config.state, config.mask);
			ret |= configure(config.mask, type);
		}

		gpio_out_s output;

		എങ്കിൽ (_gpio_out_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= ലിഖ(output.state, output.mask);
		}

		{
			gpio_in_s _gpio_in;
			_gpio_in.timestamp = hrt_absolute_time();
			_gpio_in.device_id = get_device_id();
			uint16_t നിവേശം = 0;

			എങ്കിൽ (പഠ(&നിവേശം) == PX4_OK) {
				_gpio_in.state = നിവേശം;
				_to_gpio_in.publish(_gpio_in);
			}
		}

		എണ്ണം_3++;

		എങ്കിൽ (എണ്ണം_3 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			എണ്ണം_3 = 0;
			ScheduleClear();
			ScheduleNow();
		}

		perf_end(_cycle_perf);
	}
}

ശൂന്യം MCP230XX::print_status()
{
	I2CSPIDriverBase::print_status();
	perf_print_counter(_cycle_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(_register_check);
}

ശൂന്യം MCP230XX::custom_method(സ്ഥിരം BusCLIArguments &cli)
{
	തിരഞ്ഞെടുപ്പ് (cli.custom1) {
	അവസ്ഥ 0: {
			// write: only pins configured as outputs are affected
			സ്ഥിരം uint16_t outputs = (uint16_t)~_iodir;
			സ്ഥിരം uint16_t state = (uint16_t)cli.custom2;

			എങ്കിൽ (ലിഖ(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			നിർത്തുക;
		}
	}
}

ശൂന്യം MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(സത്യം, അസത്യം);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", സത്യം);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", സത്യം);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", സത്യം);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", സത്യം);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", സത്യം);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2CSPIDriverBase *MCP230XX::instantiate(സ്ഥിരം I2CSPIDriverConfig &config, പൂർണ്ണസംഖ്യ runtime_instance)
{
	MCP230XX *instance = nullptr;

	തിരഞ്ഞെടുപ്പ് (config.devid_driver_index) {
	അവസ്ഥ DRV_GPIO_DEVTYPE_MCP23009:
		instance = പുതിയ MCP23009(config);
		നിർത്തുക;

	അവസ്ഥ DRV_GPIO_DEVTYPE_MCP23017:
		instance = പുതിയ MCP23017(config);
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		instance = nullptr;
		നിർത്തുക;
	}

	എങ്കിൽ (!instance) {
		PX4_ERR("alloc failed");
		തിരികെ nullptr;
	}

	MCP230XX_config_t tmp_config = *static_cast<സ്ഥിരം MCP230XX_config_t *>(config.custom_data);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_address;

	എങ്കിൽ (OK != instance->init()) {
		delete instance;
		തിരികെ nullptr;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	തിരികെ instance;
}
