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
#ଅନ୍ତର୍ଭାଵ "MCP.hpp"
#ଅନ୍ତର୍ଭାଵ <drivers/gpio/mcp23009/MCP23009.hpp>
#ଅନ୍ତର୍ଭାଵ <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(ଅଚଳ I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	_cycle_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_ଗଣନା, MODULE_NAME": comms errors")),
	_register_check(perf_alloc(PC_ଗଣନା, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	ScheduleClear();
	perf_free(_cycle_perf);
	perf_free(_comms_errors);
	perf_free(_register_check);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା MCP230XX::init_uorb()
{
	ଯଦି (!_gpio_config_sub.registerCallback() ||
	    !_gpio_out_sub.registerCallback()) {
		PX4_ERR("callback registration failed");
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ଫେରନ୍ତୁ PX4_OK;
}

ଶୂନ୍ୟ MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	_gpio_out_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.first_minor, mcp_config.ଅଙ୍କ_pins, mcp_config._gpio_handle);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା MCP230XX::ପଠ(uint16_t *mask)
{
	*mask = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = PX4_OK;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < mcp_config.ଅଙ୍କ_banks; i++) {
		uint8_t gpio_addr;
		uint8_t data;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, data);

		*mask |= (((uint16_t) data) << (8 * i));
	}

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା MCP230XX::ଲିଖ(ଅଚଳ uint16_t mask_set, ଅଚଳ uint16_t mask_clear)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = PX4_OK;
	_olat = (_olat & ~mask_clear) | mask_set;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < mcp_config.ଅଙ୍କ_banks; i++) {
		uint8_t reg_addr;
		uint8_t data;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		ଯଦି (ret != PX4_OK) {
			ଫେରନ୍ତୁ ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, data);

		ଯଦି (ret != PX4_OK || data != curr_olat) {
			ଫେରନ୍ତୁ PX4_ERROR;
		}
	}

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା MCP230XX::read_reg(ଅଚଳ uint8_t address, uint8_t &data)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = transfer(&address, 1, &data, 1);

	ଯଦି (ret != PX4_OK) {
		perf_ଗଣନା(_comms_errors);
	}

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା MCP230XX::write_reg(ଅଚଳ uint8_t address, ଅଚଳ uint8_t value)
{
	uint8_t data[2] = {address, value};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = transfer(data, 2, nullptr, 0);

	ଯଦି (ret != PX4_OK) {
		perf_ଗଣନା(_comms_errors);
	}

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା MCP230XX::configure(ଅଚଳ uint16_t mask, PinType type)
{
	ବାଛ (type) {
	ସ୍ଥିତି PinType::ନିବେଶ_2:
		_iodir |= mask;
		_gppu &= ~mask;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PinType::ନିବେଶ_Pull_Up:
		_iodir |= mask;
		_gppu |= mask;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PinType::Output:
		_iodir &= ~mask;
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ -EINVAL;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = PX4_OK;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < mcp_config.ଅଙ୍କ_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		ଯଦି (ret != PX4_OK) {
			ଫେରନ୍ତୁ ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		ଯଦି (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			ଫେରନ୍ତୁ ret;
		}
	}

	ଫେରନ୍ତୁ PX4_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା MCP230XX::init()
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = I2C::init();

	ଯଦି (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		ଫେରନ୍ତୁ ret;
	}

	ScheduleNow();
	ଫେରନ୍ତୁ PX4_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା MCP230XX::init_communication()
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = mcp230XX_register_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.first_minor, _iodir, mcp_config.ଅଙ୍କ_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା MCP230XX::set_up()
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < mcp_config.ଅଙ୍କ_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		ଯଦି (ret != PX4_OK) {
			ଫେରନ୍ତୁ ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		ଯଦି (ret != PX4_OK) {
			ଫେରନ୍ତୁ ret;
		}
	}

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା MCP230XX::sanity_check()
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = PX4_OK;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < mcp_config.ଅଙ୍କ_banks; i++) {
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

		ଯଦି (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_ଗଣନା(_register_check);
			ଫେରନ୍ତୁ PX4_ERROR;
		}
	}

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t data;
	uint8_t addr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = PX4_OK;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		ଯଦି (ret == PX4_OK) {
			ret = read_reg(addr, data);

			ଯଦି (ret == PX4_OK) {
				ଫେରନ୍ତୁ PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	ଫେରନ୍ତୁ PX4_ERROR;
}

ଶୂନ୍ୟ MCP230XX::RunImpl()
{
	ଯଦି (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		ଫେରନ୍ତୁ;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = PX4_OK;

	ବାଛ (_curr_state) {
	ସ୍ଥିତି STATE::INIT_COMMUNICATION:
		ret = init_communication();

		ଯଦି (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			ScheduleNow();

		} ଅନ୍ୟଥା {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		ଭାଙ୍ଗ;

	ସ୍ଥିତି STATE::CONFIGURE:
		ret = set_up();

		ଯଦି (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			ScheduleNow();

		} ଅନ୍ୟଥା {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		ଭାଙ୍ଗ;

	ସ୍ଥିତି STATE::CHECK:
		ret = sanity_check();

		ଯଦି (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			ScheduleOnInterval(mcp_config.interval * 1000);

		} ଅନ୍ୟଥା {
			_curr_state = STATE::CONFIGURE;
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		ଭାଙ୍ଗ;

	ସ୍ଥିତି STATE::RUNNING:
		perf_begin(_cycle_perf);
		gpio_config_s config;

		ଯଦି (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::ନିବେଶ_2;

			ବାଛ (config.config) {
			ସ୍ଥିତି config.ନିବେଶ_PULLUP:	type = PinType::ନିବେଶ_Pull_Up; ଭାଙ୍ଗ;

			ସ୍ଥିତି config.OUTPUT:		type = PinType::Output; ଭାଙ୍ଗ;
			}

			ret |= ଲିଖ(config.state, config.mask);
			ret |= configure(config.mask, type);
		}

		gpio_out_s output;

		ଯଦି (_gpio_out_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= ଲିଖ(output.state, output.mask);
		}

		{
			gpio_in_s _gpio_in;
			_gpio_in.timestamp = hrt_absolute_time();
			_gpio_in.device_id = get_device_id();
			uint16_t ନିବେଶ = 0;

			ଯଦି (ପଠ(&ନିବେଶ) == PX4_OK) {
				_gpio_in.state = ନିବେଶ;
				_to_gpio_in.publish(_gpio_in);
			}
		}

		ଗଣନା_4++;

		ଯଦି (ଗଣନା_4 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			ଗଣନା_4 = 0;
			ScheduleClear();
			ScheduleNow();
		}

		perf_end(_cycle_perf);
	}
}

ଶୂନ୍ୟ MCP230XX::print_status()
{
	I2CSPIDriverBase::print_status();
	perf_print_counter(_cycle_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(_register_check);
}

ଶୂନ୍ୟ MCP230XX::custom_method(ଅଚଳ BusCLIArguments &cli)
{
	ବାଛ (cli.custom1) {
	ସ୍ଥିତି 0: {
			// write: only pins configured as outputs are affected
			ଅଚଳ uint16_t outputs = (uint16_t)~_iodir;
			ଅଚଳ uint16_t state = (uint16_t)cli.custom2;

			ଯଦି (ଲିଖ(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			ଭାଙ୍ଗ;
		}
	}
}

ଶୂନ୍ୟ MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(ସତ୍ୟ, ମିଥ୍ୟା);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", ସତ୍ୟ);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", ସତ୍ୟ);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", ସତ୍ୟ);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", ସତ୍ୟ);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", ସତ୍ୟ);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2CSPIDriverBase *MCP230XX::instantiate(ଅଚଳ I2CSPIDriverConfig &config, ପୂର୍ଣ୍ଣସଂଖ୍ୟା runtime_instance)
{
	MCP230XX *instance = nullptr;

	ବାଛ (config.devid_driver_index) {
	ସ୍ଥିତି DRV_GPIO_DEVTYPE_MCP23009:
		instance = ନୂତନ MCP23009(config);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି DRV_GPIO_DEVTYPE_MCP23017:
		instance = ନୂତନ MCP23017(config);
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		instance = nullptr;
		ଭାଙ୍ଗ;
	}

	ଯଦି (!instance) {
		PX4_ERR("alloc failed");
		ଫେରନ୍ତୁ nullptr;
	}

	MCP230XX_config_t tmp_config = *static_cast<ଅଚଳ MCP230XX_config_t *>(config.custom_data);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_address;

	ଯଦି (OK != instance->init()) {
		delete instance;
		ଫେରନ୍ତୁ nullptr;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	ଫେରନ୍ତୁ instance;
}
