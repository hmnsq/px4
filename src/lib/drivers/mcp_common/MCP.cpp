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
#ಅನ್ತರ್ಭಾವ "MCP.hpp"
#ಅನ್ತರ್ಭಾವ <drivers/gpio/mcp23009/MCP23009.hpp>
#ಅನ್ತರ್ಭಾವ <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(ಧ್ರುವ I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	_cycle_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_ಎಣಿಕೆ, MODULE_NAME": comms errors")),
	_register_check(perf_alloc(PC_ಎಣಿಕೆ, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	ScheduleClear();
	perf_free(_cycle_perf);
	perf_free(_comms_errors);
	perf_free(_register_check);
}

ಪೂರ್ಣಾಙ್ಕ MCP230XX::init_uorb()
{
	ಯದಿ (!_gpio_config_sub.registerCallback() ||
	    !_gpio_out_sub.registerCallback()) {
		PX4_ERR("callback registration failed");
		ಪ್ರತ್ಯಾಗಮ PX4_ERROR;
	}

	ಪ್ರತ್ಯಾಗಮ PX4_OK;
}

ಶೂನ್ಯ MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	_gpio_out_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.first_minor, mcp_config.ಅಂಕೆ_pins, mcp_config._gpio_handle);
}

ಪೂರ್ಣಾಙ್ಕ MCP230XX::ಪಠ(uint16_t *mask)
{
	*mask = 0;
	ಪೂರ್ಣಾಙ್ಕ ret = PX4_OK;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < mcp_config.ಅಂಕೆ_banks; i++) {
		uint8_t gpio_addr;
		uint8_t data;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, data);

		*mask |= (((uint16_t) data) << (8 * i));
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ MCP230XX::ಲಿಖ(ಧ್ರುವ uint16_t mask_set, ಧ್ರುವ uint16_t mask_clear)
{
	ಪೂರ್ಣಾಙ್ಕ ret = PX4_OK;
	_olat = (_olat & ~mask_clear) | mask_set;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < mcp_config.ಅಂಕೆ_banks; i++) {
		uint8_t reg_addr;
		uint8_t data;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		ಯದಿ (ret != PX4_OK) {
			ಪ್ರತ್ಯಾಗಮ ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, data);

		ಯದಿ (ret != PX4_OK || data != curr_olat) {
			ಪ್ರತ್ಯಾಗಮ PX4_ERROR;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ MCP230XX::read_reg(ಧ್ರುವ uint8_t address, uint8_t &data)
{
	ಪೂರ್ಣಾಙ್ಕ ret = transfer(&address, 1, &data, 1);

	ಯದಿ (ret != PX4_OK) {
		perf_ಎಣಿಕೆ(_comms_errors);
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ MCP230XX::write_reg(ಧ್ರುವ uint8_t address, ಧ್ರುವ uint8_t value)
{
	uint8_t data[2] = {address, value};
	ಪೂರ್ಣಾಙ್ಕ ret = transfer(data, 2, nullptr, 0);

	ಯದಿ (ret != PX4_OK) {
		perf_ಎಣಿಕೆ(_comms_errors);
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ MCP230XX::configure(ಧ್ರುವ uint16_t mask, PinType type)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ PinType::ಒಳಹರಿವು_2:
		_iodir |= mask;
		_gppu &= ~mask;
		ಭಙ್ಗ;

	ಪಕ್ಷ PinType::ಒಳಹರಿವು_Pull_Up:
		_iodir |= mask;
		_gppu |= mask;
		ಭಙ್ಗ;

	ಪಕ್ಷ PinType::Output:
		_iodir &= ~mask;
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ -EINVAL;
	}

	ಪೂರ್ಣಾಙ್ಕ ret = PX4_OK;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < mcp_config.ಅಂಕೆ_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		ಯದಿ (ret != PX4_OK) {
			ಪ್ರತ್ಯಾಗಮ ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		ಯದಿ (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			ಪ್ರತ್ಯಾಗಮ ret;
		}
	}

	ಪ್ರತ್ಯಾಗಮ PX4_OK;
}

ಪೂರ್ಣಾಙ್ಕ MCP230XX::init()
{
	ಪೂರ್ಣಾಙ್ಕ ret = I2C::init();

	ಯದಿ (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		ಪ್ರತ್ಯಾಗಮ ret;
	}

	ScheduleNow();
	ಪ್ರತ್ಯಾಗಮ PX4_OK;
}

ಪೂರ್ಣಾಙ್ಕ MCP230XX::init_communication()
{
	ಪೂರ್ಣಾಙ್ಕ ret = mcp230XX_register_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.first_minor, _iodir, mcp_config.ಅಂಕೆ_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ MCP230XX::set_up()
{
	ಪೂರ್ಣಾಙ್ಕ ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < mcp_config.ಅಂಕೆ_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		ಯದಿ (ret != PX4_OK) {
			ಪ್ರತ್ಯಾಗಮ ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		ಯದಿ (ret != PX4_OK) {
			ಪ್ರತ್ಯಾಗಮ ret;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ MCP230XX::sanity_check()
{
	ಪೂರ್ಣಾಙ್ಕ ret = PX4_OK;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < mcp_config.ಅಂಕೆ_banks; i++) {
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

		ಯದಿ (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_ಎಣಿಕೆ(_register_check);
			ಪ್ರತ್ಯಾಗಮ PX4_ERROR;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t data;
	uint8_t addr;
	ಪೂರ್ಣಾಙ್ಕ ret = PX4_OK;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		ಯದಿ (ret == PX4_OK) {
			ret = read_reg(addr, data);

			ಯದಿ (ret == PX4_OK) {
				ಪ್ರತ್ಯಾಗಮ PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	ಪ್ರತ್ಯಾಗಮ PX4_ERROR;
}

ಶೂನ್ಯ MCP230XX::RunImpl()
{
	ಯದಿ (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಪೂರ್ಣಾಙ್ಕ ret = PX4_OK;

	ವಿಕಲ್ಪ (_curr_state) {
	ಪಕ್ಷ STATE::INIT_COMMUNICATION:
		ret = init_communication();

		ಯದಿ (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			ScheduleNow();

		} ಅನ್ಯಥಾ {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		ಭಙ್ಗ;

	ಪಕ್ಷ STATE::CONFIGURE:
		ret = set_up();

		ಯದಿ (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			ScheduleNow();

		} ಅನ್ಯಥಾ {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		ಭಙ್ಗ;

	ಪಕ್ಷ STATE::CHECK:
		ret = sanity_check();

		ಯದಿ (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			ScheduleOnInterval(mcp_config.interval * 1000);

		} ಅನ್ಯಥಾ {
			_curr_state = STATE::CONFIGURE;
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		ಭಙ್ಗ;

	ಪಕ್ಷ STATE::RUNNING:
		perf_begin(_cycle_perf);
		gpio_config_s config;

		ಯದಿ (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::ಒಳಹರಿವು_2;

			ವಿಕಲ್ಪ (config.config) {
			ಪಕ್ಷ config.ಒಳಹರಿವು_PULLUP:	type = PinType::ಒಳಹರಿವು_Pull_Up; ಭಙ್ಗ;

			ಪಕ್ಷ config.OUTPUT:		type = PinType::Output; ಭಙ್ಗ;
			}

			ret |= ಲಿಖ(config.state, config.mask);
			ret |= configure(config.mask, type);
		}

		gpio_out_s output;

		ಯದಿ (_gpio_out_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= ಲಿಖ(output.state, output.mask);
		}

		{
			gpio_in_s _gpio_in;
			_gpio_in.timestamp = hrt_absolute_time();
			_gpio_in.device_id = get_device_id();
			uint16_t ಒಳಹರಿವು = 0;

			ಯದಿ (ಪಠ(&ಒಳಹರಿವು) == PX4_OK) {
				_gpio_in.state = ಒಳಹರಿವು;
				_to_gpio_in.publish(_gpio_in);
			}
		}

		ಎಣಿಕೆ_3++;

		ಯದಿ (ಎಣಿಕೆ_3 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			ಎಣಿಕೆ_3 = 0;
			ScheduleClear();
			ScheduleNow();
		}

		perf_end(_cycle_perf);
	}
}

ಶೂನ್ಯ MCP230XX::print_status()
{
	I2CSPIDriverBase::print_status();
	perf_print_counter(_cycle_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(_register_check);
}

ಶೂನ್ಯ MCP230XX::custom_method(ಧ್ರುವ BusCLIArguments &cli)
{
	ವಿಕಲ್ಪ (cli.custom1) {
	ಪಕ್ಷ 0: {
			// write: only pins configured as outputs are affected
			ಧ್ರುವ uint16_t outputs = (uint16_t)~_iodir;
			ಧ್ರುವ uint16_t state = (uint16_t)cli.custom2;

			ಯದಿ (ಲಿಖ(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			ಭಙ್ಗ;
		}
	}
}

ಶೂನ್ಯ MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(ಸತ್ಯ, ಅಸತ್ಯ);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", ಸತ್ಯ);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", ಸತ್ಯ);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", ಸತ್ಯ);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", ಸತ್ಯ);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", ಸತ್ಯ);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2CSPIDriverBase *MCP230XX::instantiate(ಧ್ರುವ I2CSPIDriverConfig &config, ಪೂರ್ಣಾಙ್ಕ runtime_instance)
{
	MCP230XX *instance = nullptr;

	ವಿಕಲ್ಪ (config.devid_driver_index) {
	ಪಕ್ಷ DRV_GPIO_DEVTYPE_MCP23009:
		instance = ನೂತನ MCP23009(config);
		ಭಙ್ಗ;

	ಪಕ್ಷ DRV_GPIO_DEVTYPE_MCP23017:
		instance = ನೂತನ MCP23017(config);
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		instance = nullptr;
		ಭಙ್ಗ;
	}

	ಯದಿ (!instance) {
		PX4_ERR("alloc failed");
		ಪ್ರತ್ಯಾಗಮ nullptr;
	}

	MCP230XX_config_t tmp_config = *static_cast<ಧ್ರುವ MCP230XX_config_t *>(config.custom_data);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_address;

	ಯದಿ (OK != instance->init()) {
		delete instance;
		ಪ್ರತ್ಯಾಗಮ nullptr;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	ಪ್ರತ್ಯಾಗಮ instance;
}
