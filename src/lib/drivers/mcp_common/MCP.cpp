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
#அந்தர்பாவ "MCP.hpp"
#அந்தர்பாவ <drivers/gpio/mcp23009/MCP23009.hpp>
#அந்தர்பாவ <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(மாறிலி I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	_cycle_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_மொத்தம், MODULE_NAME": comms errors")),
	_register_check(perf_alloc(PC_மொத்தம், MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	ScheduleClear();
	perf_free(_cycle_perf);
	perf_free(_comms_errors);
	perf_free(_register_check);
}

முழு MCP230XX::init_uorb()
{
	எனில் (!_gpio_config_sub.registerCallback() ||
	    !_gpio_out_sub.registerCallback()) {
		PX4_ERR("callback registration failed");
		திரும்பு PX4_ERROR;
	}

	திரும்பு PX4_OK;
}

வெறுமை MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	_gpio_out_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.first_minor, mcp_config.இலக்கம்_pins, mcp_config._gpio_handle);
}

முழு MCP230XX::பட(uint16_t *mask)
{
	*mask = 0;
	முழு ret = PX4_OK;

	க்கு (முழு i = 0; i < mcp_config.இலக்கம்_banks; i++) {
		uint8_t gpio_addr;
		uint8_t data;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, data);

		*mask |= (((uint16_t) data) << (8 * i));
	}

	திரும்பு ret;
}

முழு MCP230XX::லிக(மாறிலி uint16_t mask_set, மாறிலி uint16_t mask_clear)
{
	முழு ret = PX4_OK;
	_olat = (_olat & ~mask_clear) | mask_set;

	க்கு (முழு i = 0; i < mcp_config.இலக்கம்_banks; i++) {
		uint8_t reg_addr;
		uint8_t data;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		எனில் (ret != PX4_OK) {
			திரும்பு ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, data);

		எனில் (ret != PX4_OK || data != curr_olat) {
			திரும்பு PX4_ERROR;
		}
	}

	திரும்பு ret;
}

முழு MCP230XX::read_reg(மாறிலி uint8_t address, uint8_t &data)
{
	முழு ret = transfer(&address, 1, &data, 1);

	எனில் (ret != PX4_OK) {
		perf_மொத்தம்(_comms_errors);
	}

	திரும்பு ret;
}

முழு MCP230XX::write_reg(மாறிலி uint8_t address, மாறிலி uint8_t value)
{
	uint8_t data[2] = {address, value};
	முழு ret = transfer(data, 2, nullptr, 0);

	எனில் (ret != PX4_OK) {
		perf_மொத்தம்(_comms_errors);
	}

	திரும்பு ret;
}

முழு MCP230XX::configure(மாறிலி uint16_t mask, PinType type)
{
	தேர்வு (type) {
	நிலை PinType::உள்ளீடு_2:
		_iodir |= mask;
		_gppu &= ~mask;
		நிறுத்து;

	நிலை PinType::உள்ளீடு_Pull_Up:
		_iodir |= mask;
		_gppu |= mask;
		நிறுத்து;

	நிலை PinType::Output:
		_iodir &= ~mask;
		நிறுத்து;

	இயல்பு:
		திரும்பு -EINVAL;
	}

	முழு ret = PX4_OK;

	க்கு (முழு i = 0; i < mcp_config.இலக்கம்_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		எனில் (ret != PX4_OK) {
			திரும்பு ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		எனில் (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			திரும்பு ret;
		}
	}

	திரும்பு PX4_OK;
}

முழு MCP230XX::init()
{
	முழு ret = I2C::init();

	எனில் (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		திரும்பு ret;
	}

	ScheduleNow();
	திரும்பு PX4_OK;
}

முழு MCP230XX::init_communication()
{
	முழு ret = mcp230XX_register_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.first_minor, _iodir, mcp_config.இலக்கம்_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	திரும்பு ret;
}

முழு MCP230XX::set_up()
{
	முழு ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	க்கு (முழு i = 0; i < mcp_config.இலக்கம்_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		எனில் (ret != PX4_OK) {
			திரும்பு ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		எனில் (ret != PX4_OK) {
			திரும்பு ret;
		}
	}

	திரும்பு ret;
}

முழு MCP230XX::sanity_check()
{
	முழு ret = PX4_OK;

	க்கு (முழு i = 0; i < mcp_config.இலக்கம்_banks; i++) {
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

		எனில் (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_மொத்தம்(_register_check);
			திரும்பு PX4_ERROR;
		}
	}

	திரும்பு ret;
}

முழு MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t data;
	uint8_t addr;
	முழு ret = PX4_OK;

	க்கு (முழு i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		எனில் (ret == PX4_OK) {
			ret = read_reg(addr, data);

			எனில் (ret == PX4_OK) {
				திரும்பு PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	திரும்பு PX4_ERROR;
}

வெறுமை MCP230XX::RunImpl()
{
	எனில் (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		திரும்பு;
	}

	முழு ret = PX4_OK;

	தேர்வு (_curr_state) {
	நிலை STATE::INIT_COMMUNICATION:
		ret = init_communication();

		எனில் (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			ScheduleNow();

		} இல்லையெனில் {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		நிறுத்து;

	நிலை STATE::CONFIGURE:
		ret = set_up();

		எனில் (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			ScheduleNow();

		} இல்லையெனில் {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		நிறுத்து;

	நிலை STATE::CHECK:
		ret = sanity_check();

		எனில் (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			ScheduleOnInterval(mcp_config.interval * 1000);

		} இல்லையெனில் {
			_curr_state = STATE::CONFIGURE;
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		நிறுத்து;

	நிலை STATE::RUNNING:
		perf_begin(_cycle_perf);
		gpio_config_s config;

		எனில் (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::உள்ளீடு_2;

			தேர்வு (config.config) {
			நிலை config.உள்ளீடு_PULLUP:	type = PinType::உள்ளீடு_Pull_Up; நிறுத்து;

			நிலை config.OUTPUT:		type = PinType::Output; நிறுத்து;
			}

			ret |= லிக(config.state, config.mask);
			ret |= configure(config.mask, type);
		}

		gpio_out_s output;

		எனில் (_gpio_out_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= லிக(output.state, output.mask);
		}

		{
			gpio_in_s _gpio_in;
			_gpio_in.timestamp = hrt_absolute_time();
			_gpio_in.device_id = get_device_id();
			uint16_t உள்ளீடு = 0;

			எனில் (பட(&உள்ளீடு) == PX4_OK) {
				_gpio_in.state = உள்ளீடு;
				_to_gpio_in.publish(_gpio_in);
			}
		}

		மொத்தம்_3++;

		எனில் (மொத்தம்_3 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			மொத்தம்_3 = 0;
			ScheduleClear();
			ScheduleNow();
		}

		perf_end(_cycle_perf);
	}
}

வெறுமை MCP230XX::print_status()
{
	I2CSPIDriverBase::print_status();
	perf_print_counter(_cycle_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(_register_check);
}

வெறுமை MCP230XX::custom_method(மாறிலி BusCLIArguments &cli)
{
	தேர்வு (cli.custom1) {
	நிலை 0: {
			// write: only pins configured as outputs are affected
			மாறிலி uint16_t outputs = (uint16_t)~_iodir;
			மாறிலி uint16_t state = (uint16_t)cli.custom2;

			எனில் (லிக(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			நிறுத்து;
		}
	}
}

வெறுமை MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(உண்மை, பொய்);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", உண்மை);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", உண்மை);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", உண்மை);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", உண்மை);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", உண்மை);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2CSPIDriverBase *MCP230XX::instantiate(மாறிலி I2CSPIDriverConfig &config, முழு runtime_instance)
{
	MCP230XX *instance = nullptr;

	தேர்வு (config.devid_driver_index) {
	நிலை DRV_GPIO_DEVTYPE_MCP23009:
		instance = புதிய MCP23009(config);
		நிறுத்து;

	நிலை DRV_GPIO_DEVTYPE_MCP23017:
		instance = புதிய MCP23017(config);
		நிறுத்து;

	இயல்பு:
		instance = nullptr;
		நிறுத்து;
	}

	எனில் (!instance) {
		PX4_ERR("alloc failed");
		திரும்பு nullptr;
	}

	MCP230XX_config_t tmp_config = *static_cast<மாறிலி MCP230XX_config_t *>(config.custom_data);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_address;

	எனில் (OK != instance->init()) {
		delete instance;
		திரும்பு nullptr;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	திரும்பு instance;
}
