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
#অন্তর্ভাৱ "MCP.hpp"
#অন্তর্ভাৱ <drivers/gpio/mcp23009/MCP23009.hpp>
#অন্তর্ভাৱ <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(ধ্রুৱ I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	_cycle_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_গুনতি, MODULE_NAME": comms errors")),
	_register_check(perf_alloc(PC_গুনতি, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	ScheduleClear();
	perf_free(_cycle_perf);
	perf_free(_comms_errors);
	perf_free(_register_check);
}

পূর্ণাঙ্ক MCP230XX::init_uorb()
{
	যদি (!_gpio_config_sub.registerCallback() ||
	    !_gpio_out_sub.registerCallback()) {
		PX4_ERR("callback registration failed");
		ঘূৰাই PX4_ERROR;
	}

	ঘূৰাই PX4_OK;
}

শূন্য MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	_gpio_out_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.first_minor, mcp_config.অঙ্ক_pins, mcp_config._gpio_handle);
}

পূর্ণাঙ্ক MCP230XX::পঠ(uint16_t *mask)
{
	*mask = 0;
	পূর্ণাঙ্ক ret = PX4_OK;

	বাবে (পূর্ণাঙ্ক i = 0; i < mcp_config.অঙ্ক_banks; i++) {
		uint8_t gpio_addr;
		uint8_t data;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, data);

		*mask |= (((uint16_t) data) << (8 * i));
	}

	ঘূৰাই ret;
}

পূর্ণাঙ্ক MCP230XX::লিখ(ধ্রুৱ uint16_t mask_set, ধ্রুৱ uint16_t mask_clear)
{
	পূর্ণাঙ্ক ret = PX4_OK;
	_olat = (_olat & ~mask_clear) | mask_set;

	বাবে (পূর্ণাঙ্ক i = 0; i < mcp_config.অঙ্ক_banks; i++) {
		uint8_t reg_addr;
		uint8_t data;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		যদি (ret != PX4_OK) {
			ঘূৰাই ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, data);

		যদি (ret != PX4_OK || data != curr_olat) {
			ঘূৰাই PX4_ERROR;
		}
	}

	ঘূৰাই ret;
}

পূর্ণাঙ্ক MCP230XX::read_reg(ধ্রুৱ uint8_t address, uint8_t &data)
{
	পূর্ণাঙ্ক ret = transfer(&address, 1, &data, 1);

	যদি (ret != PX4_OK) {
		perf_গুনতি(_comms_errors);
	}

	ঘূৰাই ret;
}

পূর্ণাঙ্ক MCP230XX::write_reg(ধ্রুৱ uint8_t address, ধ্রুৱ uint8_t value)
{
	uint8_t data[2] = {address, value};
	পূর্ণাঙ্ক ret = transfer(data, 2, nullptr, 0);

	যদি (ret != PX4_OK) {
		perf_গুনতি(_comms_errors);
	}

	ঘূৰাই ret;
}

পূর্ণাঙ্ক MCP230XX::configure(ধ্রুৱ uint16_t mask, PinType type)
{
	ৱিকল্প (type) {
	পক্ষ PinType::প্ৰবেশ_2:
		_iodir |= mask;
		_gppu &= ~mask;
		ভঙ্গ;

	পক্ষ PinType::প্ৰবেশ_Pull_Up:
		_iodir |= mask;
		_gppu |= mask;
		ভঙ্গ;

	পক্ষ PinType::Output:
		_iodir &= ~mask;
		ভঙ্গ;

	স্ৱভাৱ:
		ঘূৰাই -EINVAL;
	}

	পূর্ণাঙ্ক ret = PX4_OK;

	বাবে (পূর্ণাঙ্ক i = 0; i < mcp_config.অঙ্ক_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		যদি (ret != PX4_OK) {
			ঘূৰাই ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		যদি (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			ঘূৰাই ret;
		}
	}

	ঘূৰাই PX4_OK;
}

পূর্ণাঙ্ক MCP230XX::init()
{
	পূর্ণাঙ্ক ret = I2C::init();

	যদি (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		ঘূৰাই ret;
	}

	ScheduleNow();
	ঘূৰাই PX4_OK;
}

পূর্ণাঙ্ক MCP230XX::init_communication()
{
	পূর্ণাঙ্ক ret = mcp230XX_register_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.first_minor, _iodir, mcp_config.অঙ্ক_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	ঘূৰাই ret;
}

পূর্ণাঙ্ক MCP230XX::set_up()
{
	পূর্ণাঙ্ক ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	বাবে (পূর্ণাঙ্ক i = 0; i < mcp_config.অঙ্ক_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		যদি (ret != PX4_OK) {
			ঘূৰাই ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		যদি (ret != PX4_OK) {
			ঘূৰাই ret;
		}
	}

	ঘূৰাই ret;
}

পূর্ণাঙ্ক MCP230XX::sanity_check()
{
	পূর্ণাঙ্ক ret = PX4_OK;

	বাবে (পূর্ণাঙ্ক i = 0; i < mcp_config.অঙ্ক_banks; i++) {
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

		যদি (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_গুনতি(_register_check);
			ঘূৰাই PX4_ERROR;
		}
	}

	ঘূৰাই ret;
}

পূর্ণাঙ্ক MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t data;
	uint8_t addr;
	পূর্ণাঙ্ক ret = PX4_OK;

	বাবে (পূর্ণাঙ্ক i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		যদি (ret == PX4_OK) {
			ret = read_reg(addr, data);

			যদি (ret == PX4_OK) {
				ঘূৰাই PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	ঘূৰাই PX4_ERROR;
}

শূন্য MCP230XX::RunImpl()
{
	যদি (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		ঘূৰাই;
	}

	পূর্ণাঙ্ক ret = PX4_OK;

	ৱিকল্প (_curr_state) {
	পক্ষ STATE::INIT_COMMUNICATION:
		ret = init_communication();

		যদি (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			ScheduleNow();

		} অন্যথা {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		ভঙ্গ;

	পক্ষ STATE::CONFIGURE:
		ret = set_up();

		যদি (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			ScheduleNow();

		} অন্যথা {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		ভঙ্গ;

	পক্ষ STATE::CHECK:
		ret = sanity_check();

		যদি (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			ScheduleOnInterval(mcp_config.interval * 1000);

		} অন্যথা {
			_curr_state = STATE::CONFIGURE;
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		ভঙ্গ;

	পক্ষ STATE::RUNNING:
		perf_begin(_cycle_perf);
		gpio_config_s config;

		যদি (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::প্ৰবেশ_2;

			ৱিকল্প (config.config) {
			পক্ষ config.প্ৰবেশ_PULLUP:	type = PinType::প্ৰবেশ_Pull_Up; ভঙ্গ;

			পক্ষ config.OUTPUT:		type = PinType::Output; ভঙ্গ;
			}

			ret |= লিখ(config.state, config.mask);
			ret |= configure(config.mask, type);
		}

		gpio_out_s output;

		যদি (_gpio_out_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= লিখ(output.state, output.mask);
		}

		{
			gpio_in_s _gpio_in;
			_gpio_in.timestamp = hrt_absolute_time();
			_gpio_in.device_id = get_device_id();
			uint16_t প্ৰবেশ = 0;

			যদি (পঠ(&প্ৰবেশ) == PX4_OK) {
				_gpio_in.state = প্ৰবেশ;
				_to_gpio_in.publish(_gpio_in);
			}
		}

		গুনতি_3++;

		যদি (গুনতি_3 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			গুনতি_3 = 0;
			ScheduleClear();
			ScheduleNow();
		}

		perf_end(_cycle_perf);
	}
}

শূন্য MCP230XX::print_status()
{
	I2CSPIDriverBase::print_status();
	perf_print_counter(_cycle_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(_register_check);
}

শূন্য MCP230XX::custom_method(ধ্রুৱ BusCLIArguments &cli)
{
	ৱিকল্প (cli.custom1) {
	পক্ষ 0: {
			// write: only pins configured as outputs are affected
			ধ্রুৱ uint16_t outputs = (uint16_t)~_iodir;
			ধ্রুৱ uint16_t state = (uint16_t)cli.custom2;

			যদি (লিখ(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			ভঙ্গ;
		}
	}
}

শূন্য MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(সঁচা, মিছা);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", সঁচা);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", সঁচা);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", সঁচা);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", সঁচা);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", সঁচা);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2CSPIDriverBase *MCP230XX::instantiate(ধ্রুৱ I2CSPIDriverConfig &config, পূর্ণাঙ্ক runtime_instance)
{
	MCP230XX *instance = nullptr;

	ৱিকল্প (config.devid_driver_index) {
	পক্ষ DRV_GPIO_DEVTYPE_MCP23009:
		instance = নূতন MCP23009(config);
		ভঙ্গ;

	পক্ষ DRV_GPIO_DEVTYPE_MCP23017:
		instance = নূতন MCP23017(config);
		ভঙ্গ;

	স্ৱভাৱ:
		instance = nullptr;
		ভঙ্গ;
	}

	যদি (!instance) {
		PX4_ERR("alloc failed");
		ঘূৰাই nullptr;
	}

	MCP230XX_config_t tmp_config = *static_cast<ধ্রুৱ MCP230XX_config_t *>(config.custom_data);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_address;

	যদি (OK != instance->init()) {
		delete instance;
		ঘূৰাই nullptr;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	ঘূৰাই instance;
}
