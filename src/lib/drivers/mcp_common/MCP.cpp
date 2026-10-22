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
#ضمن "MCP.hpp"
#ضمن <drivers/gpio/mcp23009/MCP23009.hpp>
#ضمن <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(ثابت I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	_cycle_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_تعداد, MODULE_NAME": comms errors")),
	_register_check(perf_alloc(PC_تعداد, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	ScheduleClear();
	perf_free(_cycle_perf);
	perf_free(_comms_errors);
	perf_free(_register_check);
}

صحيح MCP230XX::init_uorb()
{
	إذا (!_gpio_config_sub.registerCallback() ||
	    !_gpio_out_sub.registerCallback()) {
		PX4_ERR("callback registration failed");
		إرجاع PX4_ERROR;
	}

	إرجاع PX4_OK;
}

فراغ MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	_gpio_out_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.first_minor, mcp_config.رقم_pins, mcp_config._gpio_handle);
}

صحيح MCP230XX::اقرأ(uint16_t *mask)
{
	*mask = 0;
	صحيح ret = PX4_OK;

	لكل (صحيح i = 0; i < mcp_config.رقم_banks; i++) {
		uint8_t gpio_addr;
		uint8_t data;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, data);

		*mask |= (((uint16_t) data) << (8 * i));
	}

	إرجاع ret;
}

صحيح MCP230XX::اكتب(ثابت uint16_t mask_set, ثابت uint16_t mask_clear)
{
	صحيح ret = PX4_OK;
	_olat = (_olat & ~mask_clear) | mask_set;

	لكل (صحيح i = 0; i < mcp_config.رقم_banks; i++) {
		uint8_t reg_addr;
		uint8_t data;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		إذا (ret != PX4_OK) {
			إرجاع ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, data);

		إذا (ret != PX4_OK || data != curr_olat) {
			إرجاع PX4_ERROR;
		}
	}

	إرجاع ret;
}

صحيح MCP230XX::read_reg(ثابت uint8_t address, uint8_t &data)
{
	صحيح ret = transfer(&address, 1, &data, 1);

	إذا (ret != PX4_OK) {
		perf_تعداد(_comms_errors);
	}

	إرجاع ret;
}

صحيح MCP230XX::write_reg(ثابت uint8_t address, ثابت uint8_t value)
{
	uint8_t data[2] = {address, value};
	صحيح ret = transfer(data, 2, nullptr, 0);

	إذا (ret != PX4_OK) {
		perf_تعداد(_comms_errors);
	}

	إرجاع ret;
}

صحيح MCP230XX::configure(ثابت uint16_t mask, PinType type)
{
	بدل (type) {
	حالة PinType::إدخال_2:
		_iodir |= mask;
		_gppu &= ~mask;
		اكسر;

	حالة PinType::إدخال_Pull_Up:
		_iodir |= mask;
		_gppu |= mask;
		اكسر;

	حالة PinType::Output:
		_iodir &= ~mask;
		اكسر;

	افتراضي:
		إرجاع -EINVAL;
	}

	صحيح ret = PX4_OK;

	لكل (صحيح i = 0; i < mcp_config.رقم_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		إذا (ret != PX4_OK) {
			إرجاع ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		إذا (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			إرجاع ret;
		}
	}

	إرجاع PX4_OK;
}

صحيح MCP230XX::init()
{
	صحيح ret = I2C::init();

	إذا (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		إرجاع ret;
	}

	ScheduleNow();
	إرجاع PX4_OK;
}

صحيح MCP230XX::init_communication()
{
	صحيح ret = mcp230XX_register_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.first_minor, _iodir, mcp_config.رقم_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	إرجاع ret;
}

صحيح MCP230XX::set_up()
{
	صحيح ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	لكل (صحيح i = 0; i < mcp_config.رقم_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		إذا (ret != PX4_OK) {
			إرجاع ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		إذا (ret != PX4_OK) {
			إرجاع ret;
		}
	}

	إرجاع ret;
}

صحيح MCP230XX::sanity_check()
{
	صحيح ret = PX4_OK;

	لكل (صحيح i = 0; i < mcp_config.رقم_banks; i++) {
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

		إذا (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_تعداد(_register_check);
			إرجاع PX4_ERROR;
		}
	}

	إرجاع ret;
}

صحيح MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t data;
	uint8_t addr;
	صحيح ret = PX4_OK;

	لكل (صحيح i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		إذا (ret == PX4_OK) {
			ret = read_reg(addr, data);

			إذا (ret == PX4_OK) {
				إرجاع PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	إرجاع PX4_ERROR;
}

فراغ MCP230XX::RunImpl()
{
	إذا (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		إرجاع;
	}

	صحيح ret = PX4_OK;

	بدل (_curr_state) {
	حالة STATE::INIT_COMMUNICATION:
		ret = init_communication();

		إذا (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			ScheduleNow();

		} وإلا {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		اكسر;

	حالة STATE::CONFIGURE:
		ret = set_up();

		إذا (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			ScheduleNow();

		} وإلا {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		اكسر;

	حالة STATE::CHECK:
		ret = sanity_check();

		إذا (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			ScheduleOnInterval(mcp_config.interval * 1000);

		} وإلا {
			_curr_state = STATE::CONFIGURE;
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		اكسر;

	حالة STATE::RUNNING:
		perf_begin(_cycle_perf);
		gpio_config_s config;

		إذا (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::إدخال_2;

			بدل (config.config) {
			حالة config.إدخال_PULLUP:	type = PinType::إدخال_Pull_Up; اكسر;

			حالة config.OUTPUT:		type = PinType::Output; اكسر;
			}

			ret |= اكتب(config.state, config.mask);
			ret |= configure(config.mask, type);
		}

		gpio_out_s output;

		إذا (_gpio_out_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= اكتب(output.state, output.mask);
		}

		{
			gpio_in_s _gpio_in;
			_gpio_in.timestamp = hrt_absolute_time();
			_gpio_in.device_id = get_device_id();
			uint16_t إدخال = 0;

			إذا (اقرأ(&إدخال) == PX4_OK) {
				_gpio_in.state = إدخال;
				_to_gpio_in.publish(_gpio_in);
			}
		}

		تعداد_4++;

		إذا (تعداد_4 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			تعداد_4 = 0;
			ScheduleClear();
			ScheduleNow();
		}

		perf_end(_cycle_perf);
	}
}

فراغ MCP230XX::print_status()
{
	I2CSPIDriverBase::print_status();
	perf_print_counter(_cycle_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(_register_check);
}

فراغ MCP230XX::custom_method(ثابت BusCLIArguments &cli)
{
	بدل (cli.custom1) {
	حالة 0: {
			// write: only pins configured as outputs are affected
			ثابت uint16_t outputs = (uint16_t)~_iodir;
			ثابت uint16_t state = (uint16_t)cli.custom2;

			إذا (اكتب(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			اكسر;
		}
	}
}

فراغ MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, خطأ);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", true);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", true);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", true);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", true);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", true);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2CSPIDriverBase *MCP230XX::instantiate(ثابت I2CSPIDriverConfig &config, صحيح runtime_instance)
{
	MCP230XX *instance = nullptr;

	بدل (config.devid_driver_index) {
	حالة DRV_GPIO_DEVTYPE_MCP23009:
		instance = جديد MCP23009(config);
		اكسر;

	حالة DRV_GPIO_DEVTYPE_MCP23017:
		instance = جديد MCP23017(config);
		اكسر;

	افتراضي:
		instance = nullptr;
		اكسر;
	}

	إذا (!instance) {
		PX4_ERR("alloc failed");
		إرجاع nullptr;
	}

	MCP230XX_config_t tmp_config = *static_cast<ثابت MCP230XX_config_t *>(config.custom_data);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_address;

	إذا (OK != instance->init()) {
		delete instance;
		إرجاع nullptr;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	إرجاع instance;
}
