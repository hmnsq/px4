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
	چرخه_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_شمار, MODULE_NAME": comms errors")),
	ثبات_check(perf_alloc(PC_شمار, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	زمانبندی_Clear();
	perf_free(چرخه_perf);
	perf_free(_comms_errors);
	perf_free(ثبات_check);
}

صحيح MCP230XX::init_uorb()
{
	إذا (!_gpio_config_sub.ثبات_Callback() ||
	    !gpio_بیرون_sub.ثبات_Callback()) {
		PX4_ERR("callback registration failed");
		أرجع PX4_ERROR;
	}

	أرجع PX4_OK;
}

فراغ MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	gpio_بیرون_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.اولی_minor, mcp_config.رقم_pins, mcp_config._gpio_handle);
}

صحيح MCP230XX::اقرأ(uint16_t *ماسک)
{
	*ماسک = 0;
	صحيح ret = PX4_OK;

	لكل (صحيح i = 0; i < mcp_config.رقم_banks; i++) {
		uint8_t gpio_addr;
		uint8_t داده;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, داده);

		*ماسک |= (((uint16_t) داده) << (8 * i));
	}

	أرجع ret;
}

صحيح MCP230XX::اكتب(ثابت uint16_t ماسک_مجموعه, ثابت uint16_t ماسک_clear)
{
	صحيح ret = PX4_OK;
	_olat = (_olat & ~ماسک_clear) | ماسک_مجموعه;

	لكل (صحيح i = 0; i < mcp_config.رقم_banks; i++) {
		uint8_t reg_addr;
		uint8_t داده;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		إذا (ret != PX4_OK) {
			أرجع ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, داده);

		إذا (ret != PX4_OK || داده != curr_olat) {
			أرجع PX4_ERROR;
		}
	}

	أرجع ret;
}

صحيح MCP230XX::read_reg(ثابت uint8_t آدرس, uint8_t &داده)
{
	صحيح ret = transfer(&آدرس, 1, &داده, 1);

	إذا (ret != PX4_OK) {
		perf_شمار(_comms_errors);
	}

	أرجع ret;
}

صحيح MCP230XX::write_reg(ثابت uint8_t آدرس, ثابت uint8_t value)
{
	uint8_t داده[2] = {آدرس, value};
	صحيح ret = transfer(داده, 2, اشاره_تهی, 0);

	إذا (ret != PX4_OK) {
		perf_شمار(_comms_errors);
	}

	أرجع ret;
}

صحيح MCP230XX::configure(ثابت uint16_t ماسک, PinType type)
{
	بدل (type) {
	حالة PinType::درون_داد_2:
		_iodir |= ماسک;
		_gppu &= ~ماسک;
		اكسر;

	حالة PinType::درون_داد_Pull_به_بالا:
		_iodir |= ماسک;
		_gppu |= ماسک;
		اكسر;

	حالة PinType::Output:
		_iodir &= ~ماسک;
		اكسر;

	افتراضي:
		أرجع -EINVAL;
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
			أرجع ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		إذا (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			أرجع ret;
		}
	}

	أرجع PX4_OK;
}

صحيح MCP230XX::init()
{
	صحيح ret = I2C::init();

	إذا (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		أرجع ret;
	}

	زمانبندی_Now();
	أرجع PX4_OK;
}

صحيح MCP230XX::init_communication()
{
	صحيح ret = mcp230_XX_ثبات_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.اولی_minor, _iodir, mcp_config.رقم_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	أرجع ret;
}

صحيح MCP230XX::مجموعه_به_بالا_2()
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
			أرجع ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		إذا (ret != PX4_OK) {
			أرجع ret;
		}
	}

	أرجع ret;
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
			perf_شمار(ثبات_check);
			أرجع PX4_ERROR;
		}
	}

	أرجع ret;
}

صحيح MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t داده;
	uint8_t addr;
	صحيح ret = PX4_OK;

	لكل (صحيح i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		إذا (ret == PX4_OK) {
			ret = read_reg(addr, داده);

			إذا (ret == PX4_OK) {
				أرجع PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	أرجع PX4_ERROR;
}

فراغ MCP230XX::پیاپی_Impl()
{
	إذا (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		أرجع;
	}

	صحيح ret = PX4_OK;

	بدل (_curr_state) {
	حالة STATE::INIT_COMMUNICATION:
		ret = init_communication();

		إذا (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			زمانبندی_Now();

		} وإلا {
			زمانبندی_Delayed(mcp_config.interval * 1000);
		}

		اكسر;

	حالة STATE::CONFIGURE:
		ret = مجموعه_به_بالا_2();

		إذا (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			زمانبندی_Now();

		} وإلا {
			زمانبندی_Delayed(mcp_config.interval * 1000);
		}

		اكسر;

	حالة STATE::CHECK:
		ret = sanity_check();

		إذا (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			زمانبندی_On_Interval(mcp_config.interval * 1000);

		} وإلا {
			_curr_state = STATE::CONFIGURE;
			زمانبندی_Delayed(mcp_config.interval * 1000);
		}

		اكسر;

	حالة STATE::RUNNING:
		perf_begin(چرخه_perf);
		gpio_config_s config;

		إذا (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::درون_داد_2;

			بدل (config.config) {
			حالة config.درون_داد_PULLUP:	type = PinType::درون_داد_Pull_به_بالا; اكسر;

			حالة config.OUTPUT:		type = PinType::Output; اكسر;
			}

			ret |= اكتب(config.state, config.ماسک);
			ret |= configure(config.ماسک, type);
		}

		gpio_بیرون_s output;

		إذا (gpio_بیرون_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= اكتب(output.state, output.ماسک);
		}

		{
			gpio_درون_s gpio_درون;
			gpio_درون.timestamp = hrt_absolute_زمان();
			gpio_درون.device_id = get_device_id();
			uint16_t درون_داد = 0;

			إذا (اقرأ(&درون_داد) == PX4_OK) {
				gpio_درون.state = درون_داد;
				مقصد_gpio_درون.publish(gpio_درون);
			}
		}

		شمار_3++;

		إذا (شمار_3 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			شمار_3 = 0;
			زمانبندی_Clear();
			زمانبندی_Now();
		}

		perf_انتها(چرخه_perf);
	}
}

فراغ MCP230XX::print_وضعیت()
{
	I2_CSPIDriver_پایه::print_وضعیت();
	perf_print_counter(چرخه_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(ثبات_check);
}

فراغ MCP230XX::custom_روش(ثابت BusCLIArguments &cli)
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
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(درست, خطأ);
	PRINT_MODULE_USAGE_PARAMS_I2_C_آدرس(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", درست);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", درست);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", درست);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", درست);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", درست);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2_CSPIDriver_پایه *MCP230XX::instantiate(ثابت I2CSPIDriverConfig &config, صحيح runtime_instance)
{
	MCP230XX *instance = اشاره_تهی;

	بدل (config.devid_driver_شاخص) {
	حالة DRV_GPIO_DEVTYPE_MCP23009:
		instance = جديد MCP23009(config);
		اكسر;

	حالة DRV_GPIO_DEVTYPE_MCP23017:
		instance = جديد MCP23017(config);
		اكسر;

	افتراضي:
		instance = اشاره_تهی;
		اكسر;
	}

	إذا (!instance) {
		PX4_ERR("alloc failed");
		أرجع اشاره_تهی;
	}

	MCP230XX_config_t tmp_config = *static_cast<ثابت MCP230XX_config_t *>(config.custom_داده);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_آدرس;

	إذا (OK != instance->init()) {
		حذف instance;
		أرجع اشاره_تهی;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	أرجع instance;
}
