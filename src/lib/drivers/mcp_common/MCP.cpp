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
	چکر_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_گنتی, MODULE_NAME": comms errors")),
	رجسٹر_check(perf_alloc(PC_گنتی, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	جدول_وقت_Clear();
	perf_free(چکر_perf);
	perf_free(_comms_errors);
	perf_free(رجسٹر_check);
}

صحیح MCP230XX::init_uorb()
{
	اگر (!_gpio_config_sub.رجسٹر_Callback() ||
	    !gpio_برآمد_sub.رجسٹر_Callback()) {
		PX4_ERR("callback registration failed");
		واپس PX4_ERROR;
	}

	واپس PX4_OK;
}

خالی MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	gpio_برآمد_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.پہلا_minor, mcp_config.عدد_pins, mcp_config._gpio_handle);
}

صحیح MCP230XX::اقرأ(uint16_t *نقاب)
{
	*نقاب = 0;
	صحیح ret = PX4_OK;

	برائے (صحیح i = 0; i < mcp_config.عدد_banks; i++) {
		uint8_t gpio_addr;
		uint8_t ڈیٹا;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, ڈیٹا);

		*نقاب |= (((uint16_t) ڈیٹا) << (8 * i));
	}

	واپس ret;
}

صحیح MCP230XX::اكتب(ثابت uint16_t نقاب_سیٹ, ثابت uint16_t نقاب_clear)
{
	صحیح ret = PX4_OK;
	_olat = (_olat & ~نقاب_clear) | نقاب_سیٹ;

	برائے (صحیح i = 0; i < mcp_config.عدد_banks; i++) {
		uint8_t reg_addr;
		uint8_t ڈیٹا;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		اگر (ret != PX4_OK) {
			واپس ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, ڈیٹا);

		اگر (ret != PX4_OK || ڈیٹا != curr_olat) {
			واپس PX4_ERROR;
		}
	}

	واپس ret;
}

صحیح MCP230XX::read_reg(ثابت uint8_t پتہ, uint8_t &ڈیٹا)
{
	صحیح ret = transfer(&پتہ, 1, &ڈیٹا, 1);

	اگر (ret != PX4_OK) {
		perf_گنتی(_comms_errors);
	}

	واپس ret;
}

صحیح MCP230XX::write_reg(ثابت uint8_t پتہ, ثابت uint8_t value)
{
	uint8_t ڈیٹا[2] = {پتہ, value};
	صحیح ret = transfer(ڈیٹا, 2, خالی_اشاریہ, 0);

	اگر (ret != PX4_OK) {
		perf_گنتی(_comms_errors);
	}

	واپس ret;
}

صحیح MCP230XX::configure(ثابت uint16_t نقاب, PinType type)
{
	انتخاب (type) {
	حالت PinType::داخلہ_2:
		_iodir |= نقاب;
		_gppu &= ~نقاب;
		توڑو;

	حالت PinType::داخلہ_Pull_اوپر:
		_iodir |= نقاب;
		_gppu |= نقاب;
		توڑو;

	حالت PinType::Output:
		_iodir &= ~نقاب;
		توڑو;

	طےشدہ:
		واپس -EINVAL;
	}

	صحیح ret = PX4_OK;

	برائے (صحیح i = 0; i < mcp_config.عدد_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		اگر (ret != PX4_OK) {
			واپس ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		اگر (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			واپس ret;
		}
	}

	واپس PX4_OK;
}

صحیح MCP230XX::init()
{
	صحیح ret = I2C::init();

	اگر (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		واپس ret;
	}

	جدول_وقت_Now();
	واپس PX4_OK;
}

صحیح MCP230XX::init_communication()
{
	صحیح ret = mcp230_XX_رجسٹر_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.پہلا_minor, _iodir, mcp_config.عدد_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	واپس ret;
}

صحیح MCP230XX::سیٹ_اوپر_2()
{
	صحیح ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	برائے (صحیح i = 0; i < mcp_config.عدد_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		اگر (ret != PX4_OK) {
			واپس ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		اگر (ret != PX4_OK) {
			واپس ret;
		}
	}

	واپس ret;
}

صحیح MCP230XX::sanity_check()
{
	صحیح ret = PX4_OK;

	برائے (صحیح i = 0; i < mcp_config.عدد_banks; i++) {
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
			perf_گنتی(رجسٹر_check);
			واپس PX4_ERROR;
		}
	}

	واپس ret;
}

صحیح MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t ڈیٹا;
	uint8_t addr;
	صحیح ret = PX4_OK;

	برائے (صحیح i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		اگر (ret == PX4_OK) {
			ret = read_reg(addr, ڈیٹا);

			اگر (ret == PX4_OK) {
				واپس PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	واپس PX4_ERROR;
}

خالی MCP230XX::تسلسل_Impl()
{
	اگر (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		واپس;
	}

	صحیح ret = PX4_OK;

	انتخاب (_curr_state) {
	حالت STATE::INIT_COMMUNICATION:
		ret = init_communication();

		اگر (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			جدول_وقت_Now();

		} ورنہ {
			جدول_وقت_Delayed(mcp_config.interval * 1000);
		}

		توڑو;

	حالت STATE::CONFIGURE:
		ret = سیٹ_اوپر_2();

		اگر (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			جدول_وقت_Now();

		} ورنہ {
			جدول_وقت_Delayed(mcp_config.interval * 1000);
		}

		توڑو;

	حالت STATE::CHECK:
		ret = sanity_check();

		اگر (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			جدول_وقت_On_Interval(mcp_config.interval * 1000);

		} ورنہ {
			_curr_state = STATE::CONFIGURE;
			جدول_وقت_Delayed(mcp_config.interval * 1000);
		}

		توڑو;

	حالت STATE::RUNNING:
		perf_begin(چکر_perf);
		gpio_config_s config;

		اگر (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::داخلہ_2;

			انتخاب (config.config) {
			حالت config.داخلہ_PULLUP:	type = PinType::داخلہ_Pull_اوپر; توڑو;

			حالت config.OUTPUT:		type = PinType::Output; توڑو;
			}

			ret |= اكتب(config.state, config.نقاب);
			ret |= configure(config.نقاب, type);
		}

		gpio_برآمد_s output;

		اگر (gpio_برآمد_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= اكتب(output.state, output.نقاب);
		}

		{
			gpio_اندر_s gpio_اندر;
			gpio_اندر.timestamp = hrt_absolute_وقت();
			gpio_اندر.device_id = get_device_id();
			uint16_t داخلہ = 0;

			اگر (اقرأ(&داخلہ) == PX4_OK) {
				gpio_اندر.state = داخلہ;
				مقصد_gpio_اندر.publish(gpio_اندر);
			}
		}

		گنتی_3++;

		اگر (گنتی_3 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			گنتی_3 = 0;
			جدول_وقت_Clear();
			جدول_وقت_Now();
		}

		perf_انتہا(چکر_perf);
	}
}

خالی MCP230XX::print_حالت()
{
	I2_CSPIDriver_بنیاد::print_حالت();
	perf_print_شمار(چکر_perf);
	perf_print_شمار(_comms_errors);
	perf_print_شمار(رجسٹر_check);
}

خالی MCP230XX::custom_طریقہ(ثابت BusCLIArguments &cli)
{
	انتخاب (cli.custom1) {
	حالت 0: {
			// write: only pins configured as outputs are affected
			ثابت uint16_t outputs = (uint16_t)~_iodir;
			ثابت uint16_t state = (uint16_t)cli.custom2;

			اگر (اكتب(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			توڑو;
		}
	}
}

خالی MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(سچ, جھوٹ);
	PRINT_MODULE_USAGE_PARAMS_I2_C_پتہ(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", سچ);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", سچ);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", سچ);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", سچ);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", سچ);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2_CSPIDriver_بنیاد *MCP230XX::instantiate(ثابت I2CSPIDriverConfig &config, صحیح runtime_instance)
{
	MCP230XX *instance = خالی_اشاریہ;

	انتخاب (config.devid_driver_اشاریہ) {
	حالت DRV_GPIO_DEVTYPE_MCP23009:
		instance = نیا MCP23009(config);
		توڑو;

	حالت DRV_GPIO_DEVTYPE_MCP23017:
		instance = نیا MCP23017(config);
		توڑو;

	طےشدہ:
		instance = خالی_اشاریہ;
		توڑو;
	}

	اگر (!instance) {
		PX4_ERR("alloc failed");
		واپس خالی_اشاریہ;
	}

	MCP230XX_config_t tmp_config = *static_cast<ثابت MCP230XX_config_t *>(config.custom_ڈیٹا);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_پتہ;

	اگر (OK != instance->init()) {
		حذف instance;
		واپس خالی_اشاریہ;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	واپس instance;
}
