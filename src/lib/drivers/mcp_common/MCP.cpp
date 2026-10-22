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
#अन्तर्भाव "MCP.hpp"
#अन्तर्भाव <drivers/gpio/mcp23009/MCP23009.hpp>
#अन्तर्भाव <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(ध्रुव I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	आवृत्तिः_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_गणनम्, MODULE_NAME": comms errors")),
	पञ्जिकाकोशः_check(perf_alloc(PC_गणनम्, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	कालसारणी_Clear();
	perf_free(आवृत्तिः_perf);
	perf_free(_comms_errors);
	perf_free(पञ्जिकाकोशः_check);
}

पूर्णाङ्क MCP230XX::init_uorb()
{
	यदि (!_gpio_config_sub.पञ्जिकाकोशः_Callback() ||
	    !gpio_बहिर्गमनम्_sub.पञ्जिकाकोशः_Callback()) {
		PX4_ERR("callback registration failed");
		प्रत्यागम PX4_ERROR;
	}

	प्रत्यागम PX4_OK;
}

शून्य MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	gpio_बहिर्गमनम्_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.first_minor, mcp_config.अङ्कः_pins, mcp_config._gpio_handle);
}

पूर्णाङ्क MCP230XX::पठ(uint16_t *आवरणम्)
{
	*आवरणम् = 0;
	पूर्णाङ्क ret = PX4_OK;

	प्रति (पूर्णाङ्क i = 0; i < mcp_config.अङ्कः_banks; i++) {
		uint8_t gpio_addr;
		uint8_t दत्त;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, दत्त);

		*आवरणम् |= (((uint16_t) दत्त) << (8 * i));
	}

	प्रत्यागम ret;
}

पूर्णाङ्क MCP230XX::लिख(ध्रुव uint16_t आवरणम्_समुच्चयः, ध्रुव uint16_t आवरणम्_clear)
{
	पूर्णाङ्क ret = PX4_OK;
	_olat = (_olat & ~आवरणम्_clear) | आवरणम्_समुच्चयः;

	प्रति (पूर्णाङ्क i = 0; i < mcp_config.अङ्कः_banks; i++) {
		uint8_t reg_addr;
		uint8_t दत्त;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		यदि (ret != PX4_OK) {
			प्रत्यागम ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, दत्त);

		यदि (ret != PX4_OK || दत्त != curr_olat) {
			प्रत्यागम PX4_ERROR;
		}
	}

	प्रत्यागम ret;
}

पूर्णाङ्क MCP230XX::read_reg(ध्रुव uint8_t स्थानाङ्कः, uint8_t &दत्त)
{
	पूर्णाङ्क ret = transfer(&स्थानाङ्कः, 1, &दत्त, 1);

	यदि (ret != PX4_OK) {
		perf_गणनम्(_comms_errors);
	}

	प्रत्यागम ret;
}

पूर्णाङ्क MCP230XX::write_reg(ध्रुव uint8_t स्थानाङ्कः, ध्रुव uint8_t value)
{
	uint8_t दत्त[2] = {स्थानाङ्कः, value};
	पूर्णाङ्क ret = transfer(दत्त, 2, शून्यसूचकः, 0);

	यदि (ret != PX4_OK) {
		perf_गणनम्(_comms_errors);
	}

	प्रत्यागम ret;
}

पूर्णाङ्क MCP230XX::configure(ध्रुव uint16_t आवरणम्, PinType type)
{
	विकल्प (type) {
	पक्ष PinType::निवेशः_2:
		_iodir |= आवरणम्;
		_gppu &= ~आवरणम्;
		भङ्ग;

	पक्ष PinType::निवेशः_Pull_ऊर्ध्वम्:
		_iodir |= आवरणम्;
		_gppu |= आवरणम्;
		भङ्ग;

	पक्ष PinType::Output:
		_iodir &= ~आवरणम्;
		भङ्ग;

	स्वभाव:
		प्रत्यागम -EINVAL;
	}

	पूर्णाङ्क ret = PX4_OK;

	प्रति (पूर्णाङ्क i = 0; i < mcp_config.अङ्कः_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		यदि (ret != PX4_OK) {
			प्रत्यागम ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		यदि (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			प्रत्यागम ret;
		}
	}

	प्रत्यागम PX4_OK;
}

पूर्णाङ्क MCP230XX::init()
{
	पूर्णाङ्क ret = I2C::init();

	यदि (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		प्रत्यागम ret;
	}

	कालसारणी_Now();
	प्रत्यागम PX4_OK;
}

पूर्णाङ्क MCP230XX::init_communication()
{
	पूर्णाङ्क ret = mcp230_XX_पञ्जिकाकोशः_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.first_minor, _iodir, mcp_config.अङ्कः_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	प्रत्यागम ret;
}

पूर्णाङ्क MCP230XX::समुच्चयः_ऊर्ध्वम्_2()
{
	पूर्णाङ्क ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	प्रति (पूर्णाङ्क i = 0; i < mcp_config.अङ्कः_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		यदि (ret != PX4_OK) {
			प्रत्यागम ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		यदि (ret != PX4_OK) {
			प्रत्यागम ret;
		}
	}

	प्रत्यागम ret;
}

पूर्णाङ्क MCP230XX::sanity_check()
{
	पूर्णाङ्क ret = PX4_OK;

	प्रति (पूर्णाङ्क i = 0; i < mcp_config.अङ्कः_banks; i++) {
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

		यदि (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_गणनम्(पञ्जिकाकोशः_check);
			प्रत्यागम PX4_ERROR;
		}
	}

	प्रत्यागम ret;
}

पूर्णाङ्क MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t दत्त;
	uint8_t addr;
	पूर्णाङ्क ret = PX4_OK;

	प्रति (पूर्णाङ्क i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		यदि (ret == PX4_OK) {
			ret = read_reg(addr, दत्त);

			यदि (ret == PX4_OK) {
				प्रत्यागम PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	प्रत्यागम PX4_ERROR;
}

शून्य MCP230XX::सततम्_Impl()
{
	यदि (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		प्रत्यागम;
	}

	पूर्णाङ्क ret = PX4_OK;

	विकल्प (_curr_state) {
	पक्ष STATE::INIT_COMMUNICATION:
		ret = init_communication();

		यदि (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			कालसारणी_Now();

		} अन्यथा {
			कालसारणी_Delayed(mcp_config.interval * 1000);
		}

		भङ्ग;

	पक्ष STATE::CONFIGURE:
		ret = समुच्चयः_ऊर्ध्वम्_2();

		यदि (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			कालसारणी_Now();

		} अन्यथा {
			कालसारणी_Delayed(mcp_config.interval * 1000);
		}

		भङ्ग;

	पक्ष STATE::CHECK:
		ret = sanity_check();

		यदि (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			कालसारणी_On_Interval(mcp_config.interval * 1000);

		} अन्यथा {
			_curr_state = STATE::CONFIGURE;
			कालसारणी_Delayed(mcp_config.interval * 1000);
		}

		भङ्ग;

	पक्ष STATE::RUNNING:
		perf_begin(आवृत्तिः_perf);
		gpio_config_s config;

		यदि (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::निवेशः_2;

			विकल्प (config.config) {
			पक्ष config.निवेशः_PULLUP:	type = PinType::निवेशः_Pull_ऊर्ध्वम्; भङ्ग;

			पक्ष config.OUTPUT:		type = PinType::Output; भङ्ग;
			}

			ret |= लिख(config.state, config.आवरणम्);
			ret |= configure(config.आवरणम्, type);
		}

		gpio_बहिर्गमनम्_s output;

		यदि (gpio_बहिर्गमनम्_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= लिख(output.state, output.आवरणम्);
		}

		{
			gpio_अन्तर्_s gpio_अन्तर्;
			gpio_अन्तर्.timestamp = hrt_absolute_time();
			gpio_अन्तर्.device_id = get_device_id();
			uint16_t निवेशः = 0;

			यदि (पठ(&निवेशः) == PX4_OK) {
				gpio_अन्तर्.state = निवेशः;
				गन्तव्यम्_gpio_अन्तर्.publish(gpio_अन्तर्);
			}
		}

		गणनम्_3++;

		यदि (गणनम्_3 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			गणनम्_3 = 0;
			कालसारणी_Clear();
			कालसारणी_Now();
		}

		perf_अन्तः(आवृत्तिः_perf);
	}
}

शून्य MCP230XX::print_स्थितिः()
{
	I2_CSPIDriver_आधारः::print_स्थितिः();
	perf_print_गणक(आवृत्तिः_perf);
	perf_print_गणक(_comms_errors);
	perf_print_गणक(पञ्जिकाकोशः_check);
}

शून्य MCP230XX::custom_विधानम्(ध्रुव BusCLIArguments &cli)
{
	विकल्प (cli.custom1) {
	पक्ष 0: {
			// write: only pins configured as outputs are affected
			ध्रुव uint16_t outputs = (uint16_t)~_iodir;
			ध्रुव uint16_t state = (uint16_t)cli.custom2;

			यदि (लिख(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			भङ्ग;
		}
	}
}

शून्य MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(सत्य, असत्य);
	PRINT_MODULE_USAGE_PARAMS_I2_C_स्थानाङ्कः(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", सत्य);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", सत्य);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", सत्य);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", सत्य);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", सत्य);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2_CSPIDriver_आधारः *MCP230XX::instantiate(ध्रुव I2CSPIDriverConfig &config, पूर्णाङ्क runtime_instance)
{
	MCP230XX *instance = शून्यसूचकः;

	विकल्प (config.devid_driver_सूचकांक) {
	पक्ष DRV_GPIO_DEVTYPE_MCP23009:
		instance = नूतन MCP23009(config);
		भङ्ग;

	पक्ष DRV_GPIO_DEVTYPE_MCP23017:
		instance = नूतन MCP23017(config);
		भङ्ग;

	स्वभाव:
		instance = शून्यसूचकः;
		भङ्ग;
	}

	यदि (!instance) {
		PX4_ERR("alloc failed");
		प्रत्यागम शून्यसूचकः;
	}

	MCP230XX_config_t tmp_config = *static_cast<ध्रुव MCP230XX_config_t *>(config.custom_दत्त);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_स्थानाङ्कः;

	यदि (OK != instance->init()) {
		विलोपय instance;
		प्रत्यागम शून्यसूचकः;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	प्रत्यागम instance;
}
