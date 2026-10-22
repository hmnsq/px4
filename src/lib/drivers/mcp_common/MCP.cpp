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
#include "MCP.hpp"
#include <drivers/gpio/mcp23009/MCP23009.hpp>
#include <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(const I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	_cycle_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_COUNT, MODULE_NAME": comms errors")),
	_register_check(perf_alloc(PC_COUNT, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	ScheduleClear();
	perf_free(_cycle_perf);
	perf_free(_comms_errors);
	perf_free(_register_check);
}

ཧྲིལ MCP230XX::init_uorb()
{
	གལ་སྲིད (!_gpio_config_sub.registerCallback() ||
	    !_gpio_out_sub.registerCallback()) {
		PX4_ERR("callback registration failed");
		ལོག PX4_ERROR;
	}

	ལོག PX4_OK;
}

void MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	_gpio_out_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.first_minor, mcp_config.num_pins, mcp_config._gpio_handle);
}

ཧྲིལ MCP230XX::read(uint16_t *mask)
{
	*mask = 0;
	ཧྲིལ ret = PX4_OK;

	དོན་ལུ (ཧྲིལ i = 0; i < mcp_config.num_banks; i++) {
		uint8_t gpio_addr;
		uint8_t data;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, data);

		*mask |= (((uint16_t) data) << (8 * i));
	}

	ལོག ret;
}

ཧྲིལ MCP230XX::write(const uint16_t mask_set, const uint16_t mask_clear)
{
	ཧྲིལ ret = PX4_OK;
	_olat = (_olat & ~mask_clear) | mask_set;

	དོན་ལུ (ཧྲིལ i = 0; i < mcp_config.num_banks; i++) {
		uint8_t reg_addr;
		uint8_t data;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		གལ་སྲིད (ret != PX4_OK) {
			ལོག ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, data);

		གལ་སྲིད (ret != PX4_OK || data != curr_olat) {
			ལོག PX4_ERROR;
		}
	}

	ལོག ret;
}

ཧྲིལ MCP230XX::read_reg(const uint8_t address, uint8_t &data)
{
	ཧྲིལ ret = transfer(&address, 1, &data, 1);

	གལ་སྲིད (ret != PX4_OK) {
		perf_count(_comms_errors);
	}

	ལོག ret;
}

ཧྲིལ MCP230XX::write_reg(const uint8_t address, const uint8_t value)
{
	uint8_t data[2] = {address, value};
	ཧྲིལ ret = transfer(data, 2, nullptr, 0);

	གལ་སྲིད (ret != PX4_OK) {
		perf_count(_comms_errors);
	}

	ལོག ret;
}

ཧྲིལ MCP230XX::configure(const uint16_t mask, PinType type)
{
	switch (type) {
	pak_sa PinType::Input:
		_iodir |= mask;
		_gppu &= ~mask;
		bha_nga;

	pak_sa PinType::InputPullUp:
		_iodir |= mask;
		_gppu |= mask;
		bha_nga;

	pak_sa PinType::Output:
		_iodir &= ~mask;
		bha_nga;

	default:
		ལོག -EINVAL;
	}

	ཧྲིལ ret = PX4_OK;

	དོན་ལུ (ཧྲིལ i = 0; i < mcp_config.num_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		གལ་སྲིད (ret != PX4_OK) {
			ལོག ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		གལ་སྲིད (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			ལོག ret;
		}
	}

	ལོག PX4_OK;
}

ཧྲིལ MCP230XX::init()
{
	ཧྲིལ ret = I2C::init();

	གལ་སྲིད (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		ལོག ret;
	}

	ScheduleNow();
	ལོག PX4_OK;
}

ཧྲིལ MCP230XX::init_communication()
{
	ཧྲིལ ret = mcp230XX_register_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.first_minor, _iodir, mcp_config.num_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	ལོག ret;
}

ཧྲིལ MCP230XX::set_up()
{
	ཧྲིལ ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	དོན་ལུ (ཧྲིལ i = 0; i < mcp_config.num_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		གལ་སྲིད (ret != PX4_OK) {
			ལོག ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		གལ་སྲིད (ret != PX4_OK) {
			ལོག ret;
		}
	}

	ལོག ret;
}

ཧྲིལ MCP230XX::sanity_check()
{
	ཧྲིལ ret = PX4_OK;

	དོན་ལུ (ཧྲིལ i = 0; i < mcp_config.num_banks; i++) {
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

		གལ་སྲིད (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_count(_register_check);
			ལོག PX4_ERROR;
		}
	}

	ལོག ret;
}

ཧྲིལ MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t data;
	uint8_t addr;
	ཧྲིལ ret = PX4_OK;

	དོན་ལུ (ཧྲིལ i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		གལ་སྲིད (ret == PX4_OK) {
			ret = read_reg(addr, data);

			གལ་སྲིད (ret == PX4_OK) {
				ལོག PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	ལོག PX4_ERROR;
}

void MCP230XX::RunImpl()
{
	གལ་སྲིད (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		ལོག;
	}

	ཧྲིལ ret = PX4_OK;

	switch (_curr_state) {
	pak_sa STATE::INIT_COMMUNICATION:
		ret = init_communication();

		གལ་སྲིད (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			ScheduleNow();

		} དེ་མིན {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		bha_nga;

	pak_sa STATE::CONFIGURE:
		ret = set_up();

		གལ་སྲིད (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			ScheduleNow();

		} དེ་མིན {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		bha_nga;

	pak_sa STATE::CHECK:
		ret = sanity_check();

		གལ་སྲིད (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			ScheduleOnInterval(mcp_config.interval * 1000);

		} དེ་མིན {
			_curr_state = STATE::CONFIGURE;
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		bha_nga;

	pak_sa STATE::RUNNING:
		perf_begin(_cycle_perf);
		gpio_config_s config;

		གལ་སྲིད (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::Input;

			switch (config.config) {
			pak_sa config.INPUT_PULLUP:	type = PinType::InputPullUp; bha_nga;

			pak_sa config.OUTPUT:		type = PinType::Output; bha_nga;
			}

			ret |= write(config.state, config.mask);
			ret |= configure(config.mask, type);
		}

		gpio_out_s output;

		གལ་སྲིད (_gpio_out_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= write(output.state, output.mask);
		}

		{
			gpio_in_s _gpio_in;
			_gpio_in.timestamp = hrt_absolute_time();
			_gpio_in.device_id = get_device_id();
			uint16_t input = 0;

			གལ་སྲིད (read(&input) == PX4_OK) {
				_gpio_in.state = input;
				_to_gpio_in.publish(_gpio_in);
			}
		}

		_count++;

		གལ་སྲིད (_count >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			_count = 0;
			ScheduleClear();
			ScheduleNow();
		}

		perf_end(_cycle_perf);
	}
}

void MCP230XX::print_status()
{
	I2CSPIDriverBase::print_status();
	perf_print_counter(_cycle_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(_register_check);
}

void MCP230XX::custom_method(const BusCLIArguments &cli)
{
	switch (cli.custom1) {
	pak_sa 0: {
			// write: only pins configured as outputs are affected
			const uint16_t outputs = (uint16_t)~_iodir;
			const uint16_t state = (uint16_t)cli.custom2;

			གལ་སྲིད (write(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			bha_nga;
		}
	}
}

void MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, false);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", true);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", true);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", true);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", true);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", true);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2CSPIDriverBase *MCP230XX::instantiate(const I2CSPIDriverConfig &config, ཧྲིལ runtime_instance)
{
	MCP230XX *instance = nullptr;

	switch (config.devid_driver_index) {
	pak_sa DRV_GPIO_DEVTYPE_MCP23009:
		instance = new MCP23009(config);
		bha_nga;

	pak_sa DRV_GPIO_DEVTYPE_MCP23017:
		instance = new MCP23017(config);
		bha_nga;

	default:
		instance = nullptr;
		bha_nga;
	}

	གལ་སྲིད (!instance) {
		PX4_ERR("alloc failed");
		ལོག nullptr;
	}

	MCP230XX_config_t tmp_config = *static_cast<const MCP230XX_config_t *>(config.custom_data);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_address;

	གལ་སྲིད (OK != instance->init()) {
		delete instance;
		ལོག nullptr;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	ལོག instance;
}
