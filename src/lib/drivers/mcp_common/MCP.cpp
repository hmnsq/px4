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
#అన్తర్భావ "MCP.hpp"
#అన్తర్భావ <drivers/gpio/mcp23009/MCP23009.hpp>
#అన్తర్భావ <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(స్థిరరాశి I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	చక్రం_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_లెక్క, MODULE_NAME": comms errors")),
	రిజిస్టరు_check(perf_alloc(PC_లెక్క, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	కాలపట్టిక_Clear();
	perf_free(చక్రం_perf);
	perf_free(_comms_errors);
	perf_free(రిజిస్టరు_check);
}

పూర్ణాంకం MCP230XX::init_uorb()
{
	ఒకవేళ (!_gpio_config_sub.రిజిస్టరు_Callback() ||
	    !gpio_బహిర్గతం_sub.రిజిస్టరు_Callback()) {
		PX4_ERR("callback registration failed");
		తిరిగి PX4_ERROR;
	}

	తిరిగి PX4_OK;
}

శూన్య MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	gpio_బహిర్గతం_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.మొదటి_minor, mcp_config.అంకె_pins, mcp_config._gpio_handle);
}

పూర్ణాంకం MCP230XX::పఠ(uint16_t *ముసుగు)
{
	*ముసుగు = 0;
	పూర్ణాంకం ret = PX4_OK;

	కొరకు (పూర్ణాంకం i = 0; i < mcp_config.అంకె_banks; i++) {
		uint8_t gpio_addr;
		uint8_t దత్తాంశం;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, దత్తాంశం);

		*ముసుగు |= (((uint16_t) దత్తాంశం) << (8 * i));
	}

	తిరిగి ret;
}

పూర్ణాంకం MCP230XX::లిఖ(స్థిరరాశి uint16_t ముసుగు_సమితి, స్థిరరాశి uint16_t ముసుగు_clear)
{
	పూర్ణాంకం ret = PX4_OK;
	_olat = (_olat & ~ముసుగు_clear) | ముసుగు_సమితి;

	కొరకు (పూర్ణాంకం i = 0; i < mcp_config.అంకె_banks; i++) {
		uint8_t reg_addr;
		uint8_t దత్తాంశం;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		ఒకవేళ (ret != PX4_OK) {
			తిరిగి ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, దత్తాంశం);

		ఒకవేళ (ret != PX4_OK || దత్తాంశం != curr_olat) {
			తిరిగి PX4_ERROR;
		}
	}

	తిరిగి ret;
}

పూర్ణాంకం MCP230XX::read_reg(స్థిరరాశి uint8_t చిరునామా, uint8_t &దత్తాంశం)
{
	పూర్ణాంకం ret = transfer(&చిరునామా, 1, &దత్తాంశం, 1);

	ఒకవేళ (ret != PX4_OK) {
		perf_లెక్క(_comms_errors);
	}

	తిరిగి ret;
}

పూర్ణాంకం MCP230XX::write_reg(స్థిరరాశి uint8_t చిరునామా, స్థిరరాశి uint8_t value)
{
	uint8_t దత్తాంశం[2] = {చిరునామా, value};
	పూర్ణాంకం ret = transfer(దత్తాంశం, 2, శూన్యసూచిక, 0);

	ఒకవేళ (ret != PX4_OK) {
		perf_లెక్క(_comms_errors);
	}

	తిరిగి ret;
}

పూర్ణాంకం MCP230XX::configure(స్థిరరాశి uint16_t ముసుగు, PinType type)
{
	ఎంపిక (type) {
	స్థితి PinType::ఆదానం_2:
		_iodir |= ముసుగు;
		_gppu &= ~ముసుగు;
		ఆపు;

	స్థితి PinType::ఆదానం_Pull_పైకి:
		_iodir |= ముసుగు;
		_gppu |= ముసుగు;
		ఆపు;

	స్థితి PinType::Output:
		_iodir &= ~ముసుగు;
		ఆపు;

	అప్రమేయ:
		తిరిగి -EINVAL;
	}

	పూర్ణాంకం ret = PX4_OK;

	కొరకు (పూర్ణాంకం i = 0; i < mcp_config.అంకె_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		ఒకవేళ (ret != PX4_OK) {
			తిరిగి ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		ఒకవేళ (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			తిరిగి ret;
		}
	}

	తిరిగి PX4_OK;
}

పూర్ణాంకం MCP230XX::init()
{
	పూర్ణాంకం ret = I2C::init();

	ఒకవేళ (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		తిరిగి ret;
	}

	కాలపట్టిక_Now();
	తిరిగి PX4_OK;
}

పూర్ణాంకం MCP230XX::init_communication()
{
	పూర్ణాంకం ret = mcp230_XX_రిజిస్టరు_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.మొదటి_minor, _iodir, mcp_config.అంకె_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	తిరిగి ret;
}

పూర్ణాంకం MCP230XX::సమితి_పైకి_2()
{
	పూర్ణాంకం ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	కొరకు (పూర్ణాంకం i = 0; i < mcp_config.అంకె_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		ఒకవేళ (ret != PX4_OK) {
			తిరిగి ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		ఒకవేళ (ret != PX4_OK) {
			తిరిగి ret;
		}
	}

	తిరిగి ret;
}

పూర్ణాంకం MCP230XX::sanity_check()
{
	పూర్ణాంకం ret = PX4_OK;

	కొరకు (పూర్ణాంకం i = 0; i < mcp_config.అంకె_banks; i++) {
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

		ఒకవేళ (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_లెక్క(రిజిస్టరు_check);
			తిరిగి PX4_ERROR;
		}
	}

	తిరిగి ret;
}

పూర్ణాంకం MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t దత్తాంశం;
	uint8_t addr;
	పూర్ణాంకం ret = PX4_OK;

	కొరకు (పూర్ణాంకం i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		ఒకవేళ (ret == PX4_OK) {
			ret = read_reg(addr, దత్తాంశం);

			ఒకవేళ (ret == PX4_OK) {
				తిరిగి PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	తిరిగి PX4_ERROR;
}

శూన్య MCP230XX::నిరంతరం_Impl()
{
	ఒకవేళ (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		తిరిగి;
	}

	పూర్ణాంకం ret = PX4_OK;

	ఎంపిక (_curr_state) {
	స్థితి STATE::INIT_COMMUNICATION:
		ret = init_communication();

		ఒకవేళ (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			కాలపట్టిక_Now();

		} లేకపోతే {
			కాలపట్టిక_Delayed(mcp_config.interval * 1000);
		}

		ఆపు;

	స్థితి STATE::CONFIGURE:
		ret = సమితి_పైకి_2();

		ఒకవేళ (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			కాలపట్టిక_Now();

		} లేకపోతే {
			కాలపట్టిక_Delayed(mcp_config.interval * 1000);
		}

		ఆపు;

	స్థితి STATE::CHECK:
		ret = sanity_check();

		ఒకవేళ (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			కాలపట్టిక_On_Interval(mcp_config.interval * 1000);

		} లేకపోతే {
			_curr_state = STATE::CONFIGURE;
			కాలపట్టిక_Delayed(mcp_config.interval * 1000);
		}

		ఆపు;

	స్థితి STATE::RUNNING:
		perf_begin(చక్రం_perf);
		gpio_config_s config;

		ఒకవేళ (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::ఆదానం_2;

			ఎంపిక (config.config) {
			స్థితి config.ఆదానం_PULLUP:	type = PinType::ఆదానం_Pull_పైకి; ఆపు;

			స్థితి config.OUTPUT:		type = PinType::Output; ఆపు;
			}

			ret |= లిఖ(config.state, config.ముసుగు);
			ret |= configure(config.ముసుగు, type);
		}

		gpio_బహిర్గతం_s output;

		ఒకవేళ (gpio_బహిర్గతం_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= లిఖ(output.state, output.ముసుగు);
		}

		{
			gpio_లోపల_s gpio_లోపల;
			gpio_లోపల.timestamp = hrt_absolute_సమయం();
			gpio_లోపల.device_id = get_device_id();
			uint16_t ఆదానం = 0;

			ఒకవేళ (పఠ(&ఆదానం) == PX4_OK) {
				gpio_లోపల.state = ఆదానం;
				గమ్యం_gpio_లోపల.publish(gpio_లోపల);
			}
		}

		లెక్క_3++;

		ఒకవేళ (లెక్క_3 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			లెక్క_3 = 0;
			కాలపట్టిక_Clear();
			కాలపట్టిక_Now();
		}

		perf_చివర(చక్రం_perf);
	}
}

శూన్య MCP230XX::print_స్థితి()
{
	I2_CSPIDriver_పునాది::print_స్థితి();
	perf_print_counter(చక్రం_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(రిజిస్టరు_check);
}

శూన్య MCP230XX::custom_పద్ధతి(స్థిరరాశి BusCLIArguments &cli)
{
	ఎంపిక (cli.custom1) {
	స్థితి 0: {
			// write: only pins configured as outputs are affected
			స్థిరరాశి uint16_t outputs = (uint16_t)~_iodir;
			స్థిరరాశి uint16_t state = (uint16_t)cli.custom2;

			ఒకవేళ (లిఖ(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			ఆపు;
		}
	}
}

శూన్య MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(నిజం, అబద్ధం);
	PRINT_MODULE_USAGE_PARAMS_I2_C_చిరునామా(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", నిజం);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", నిజం);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", నిజం);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", నిజం);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", నిజం);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2_CSPIDriver_పునాది *MCP230XX::instantiate(స్థిరరాశి I2CSPIDriverConfig &config, పూర్ణాంకం runtime_instance)
{
	MCP230XX *instance = శూన్యసూచిక;

	ఎంపిక (config.devid_driver_సూచీ) {
	స్థితి DRV_GPIO_DEVTYPE_MCP23009:
		instance = కొత్త MCP23009(config);
		ఆపు;

	స్థితి DRV_GPIO_DEVTYPE_MCP23017:
		instance = కొత్త MCP23017(config);
		ఆపు;

	అప్రమేయ:
		instance = శూన్యసూచిక;
		ఆపు;
	}

	ఒకవేళ (!instance) {
		PX4_ERR("alloc failed");
		తిరిగి శూన్యసూచిక;
	}

	MCP230XX_config_t tmp_config = *static_cast<స్థిరరాశి MCP230XX_config_t *>(config.custom_దత్తాంశం);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_చిరునామా;

	ఒకవేళ (OK != instance->init()) {
		తొలగించు instance;
		తిరిగి శూన్యసూచిక;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	తిరిగి instance;
}
