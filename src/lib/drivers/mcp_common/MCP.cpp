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
#شامل "MCP.hpp"
#شامل <drivers/gpio/mcp23009/MCP23009.hpp>
#شامل <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(ثابت I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	_cycle_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_شمار, MODULE_NAME": comms errors")),
	_register_check(perf_alloc(PC_شمار, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	ScheduleClear();
	perf_free(_cycle_perf);
	perf_free(_comms_errors);
	perf_free(_register_check);
}

صحیح MCP230XX::init_uorb()
{
	اگر (!_gpio_config_sub.registerCallback() ||
	    !_gpio_out_sub.registerCallback()) {
		PX4_ERR("callback registration failed");
		بازگشت PX4_ERROR;
	}

	بازگشت PX4_OK;
}

تهی MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	_gpio_out_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.first_minor, mcp_config.رقم_pins, mcp_config._gpio_handle);
}

صحیح MCP230XX::اقرأ(uint16_t *mask)
{
	*mask = 0;
	صحیح ret = PX4_OK;

	برای (صحیح i = 0; i < mcp_config.رقم_banks; i++) {
		uint8_t gpio_addr;
		uint8_t data;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, data);

		*mask |= (((uint16_t) data) << (8 * i));
	}

	بازگشت ret;
}

صحیح MCP230XX::اكتب(ثابت uint16_t mask_set, ثابت uint16_t mask_clear)
{
	صحیح ret = PX4_OK;
	_olat = (_olat & ~mask_clear) | mask_set;

	برای (صحیح i = 0; i < mcp_config.رقم_banks; i++) {
		uint8_t reg_addr;
		uint8_t data;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		اگر (ret != PX4_OK) {
			بازگشت ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, data);

		اگر (ret != PX4_OK || data != curr_olat) {
			بازگشت PX4_ERROR;
		}
	}

	بازگشت ret;
}

صحیح MCP230XX::read_reg(ثابت uint8_t address, uint8_t &data)
{
	صحیح ret = transfer(&address, 1, &data, 1);

	اگر (ret != PX4_OK) {
		perf_شمار(_comms_errors);
	}

	بازگشت ret;
}

صحیح MCP230XX::write_reg(ثابت uint8_t address, ثابت uint8_t value)
{
	uint8_t data[2] = {address, value};
	صحیح ret = transfer(data, 2, nullptr, 0);

	اگر (ret != PX4_OK) {
		perf_شمار(_comms_errors);
	}

	بازگشت ret;
}

صحیح MCP230XX::configure(ثابت uint16_t mask, PinType type)
{
	انتخاب (type) {
	حالت PinType::درون_داد_2:
		_iodir |= mask;
		_gppu &= ~mask;
		بشکن;

	حالت PinType::درون_داد_Pull_Up:
		_iodir |= mask;
		_gppu |= mask;
		بشکن;

	حالت PinType::Output:
		_iodir &= ~mask;
		بشکن;

	پیشفرض:
		بازگشت -EINVAL;
	}

	صحیح ret = PX4_OK;

	برای (صحیح i = 0; i < mcp_config.رقم_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		اگر (ret != PX4_OK) {
			بازگشت ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		اگر (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			بازگشت ret;
		}
	}

	بازگشت PX4_OK;
}

صحیح MCP230XX::init()
{
	صحیح ret = I2C::init();

	اگر (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		بازگشت ret;
	}

	ScheduleNow();
	بازگشت PX4_OK;
}

صحیح MCP230XX::init_communication()
{
	صحیح ret = mcp230XX_register_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.first_minor, _iodir, mcp_config.رقم_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	بازگشت ret;
}

صحیح MCP230XX::set_up()
{
	صحیح ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	برای (صحیح i = 0; i < mcp_config.رقم_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		اگر (ret != PX4_OK) {
			بازگشت ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		اگر (ret != PX4_OK) {
			بازگشت ret;
		}
	}

	بازگشت ret;
}

صحیح MCP230XX::sanity_check()
{
	صحیح ret = PX4_OK;

	برای (صحیح i = 0; i < mcp_config.رقم_banks; i++) {
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

		اگر (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_شمار(_register_check);
			بازگشت PX4_ERROR;
		}
	}

	بازگشت ret;
}

صحیح MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t data;
	uint8_t addr;
	صحیح ret = PX4_OK;

	برای (صحیح i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		اگر (ret == PX4_OK) {
			ret = read_reg(addr, data);

			اگر (ret == PX4_OK) {
				بازگشت PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	بازگشت PX4_ERROR;
}

تهی MCP230XX::RunImpl()
{
	اگر (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		بازگشت;
	}

	صحیح ret = PX4_OK;

	انتخاب (_curr_state) {
	حالت STATE::INIT_COMMUNICATION:
		ret = init_communication();

		اگر (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			ScheduleNow();

		} وگرنه {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		بشکن;

	حالت STATE::CONFIGURE:
		ret = set_up();

		اگر (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			ScheduleNow();

		} وگرنه {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		بشکن;

	حالت STATE::CHECK:
		ret = sanity_check();

		اگر (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			ScheduleOnInterval(mcp_config.interval * 1000);

		} وگرنه {
			_curr_state = STATE::CONFIGURE;
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		بشکن;

	حالت STATE::RUNNING:
		perf_begin(_cycle_perf);
		gpio_config_s config;

		اگر (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::درون_داد_2;

			انتخاب (config.config) {
			حالت config.درون_داد_PULLUP:	type = PinType::درون_داد_Pull_Up; بشکن;

			حالت config.OUTPUT:		type = PinType::Output; بشکن;
			}

			ret |= اكتب(config.state, config.mask);
			ret |= configure(config.mask, type);
		}

		gpio_out_s output;

		اگر (_gpio_out_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= اكتب(output.state, output.mask);
		}

		{
			gpio_in_s _gpio_in;
			_gpio_in.timestamp = hrt_absolute_time();
			_gpio_in.device_id = get_device_id();
			uint16_t درون_داد = 0;

			اگر (اقرأ(&درون_داد) == PX4_OK) {
				_gpio_in.state = درون_داد;
				_to_gpio_in.publish(_gpio_in);
			}
		}

		شمار_3++;

		اگر (شمار_3 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			شمار_3 = 0;
			ScheduleClear();
			ScheduleNow();
		}

		perf_end(_cycle_perf);
	}
}

تهی MCP230XX::print_status()
{
	I2CSPIDriverBase::print_status();
	perf_print_counter(_cycle_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(_register_check);
}

تهی MCP230XX::custom_method(ثابت BusCLIArguments &cli)
{
	انتخاب (cli.custom1) {
	حالت 0: {
			// write: only pins configured as outputs are affected
			ثابت uint16_t outputs = (uint16_t)~_iodir;
			ثابت uint16_t state = (uint16_t)cli.custom2;

			اگر (اكتب(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			بشکن;
		}
	}
}

تهی MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(درست, نادرست);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", درست);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", درست);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", درست);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", درست);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", درست);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2CSPIDriverBase *MCP230XX::instantiate(ثابت I2CSPIDriverConfig &config, صحیح runtime_instance)
{
	MCP230XX *instance = nullptr;

	انتخاب (config.devid_driver_index) {
	حالت DRV_GPIO_DEVTYPE_MCP23009:
		instance = جدید MCP23009(config);
		بشکن;

	حالت DRV_GPIO_DEVTYPE_MCP23017:
		instance = جدید MCP23017(config);
		بشکن;

	پیشفرض:
		instance = nullptr;
		بشکن;
	}

	اگر (!instance) {
		PX4_ERR("alloc failed");
		بازگشت nullptr;
	}

	MCP230XX_config_t tmp_config = *static_cast<ثابت MCP230XX_config_t *>(config.custom_data);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_address;

	اگر (OK != instance->init()) {
		delete instance;
		بازگشت nullptr;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	بازگشت instance;
}
