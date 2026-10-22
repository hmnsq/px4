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
#અન્તર્ભાવ "MCP.hpp"
#અન્તર્ભાવ <drivers/gpio/mcp23009/MCP23009.hpp>
#અન્તર્ભાવ <drivers/gpio/mcp23017/MCP23017.hpp>

MCP230XX::MCP230XX(અચલ I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	_cycle_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_ગણતરી, MODULE_NAME": comms errors")),
	_register_check(perf_alloc(PC_ગણતરી, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	ScheduleClear();
	perf_free(_cycle_perf);
	perf_free(_comms_errors);
	perf_free(_register_check);
}

પૂર્ણાંક MCP230XX::init_uorb()
{
	જો (!_gpio_config_sub.registerCallback() ||
	    !_gpio_out_sub.registerCallback()) {
		PX4_ERR("callback registration failed");
		પાછા PX4_ERROR;
	}

	પાછા PX4_OK;
}

શૂન્ય MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	_gpio_out_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.first_minor, mcp_config.અંક_pins, mcp_config._gpio_handle);
}

પૂર્ણાંક MCP230XX::પઠ(uint16_t *mask)
{
	*mask = 0;
	પૂર્ણાંક ret = PX4_OK;

	માટે (પૂર્ણાંક i = 0; i < mcp_config.અંક_banks; i++) {
		uint8_t gpio_addr;
		uint8_t data;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, data);

		*mask |= (((uint16_t) data) << (8 * i));
	}

	પાછા ret;
}

પૂર્ણાંક MCP230XX::લિખ(અચલ uint16_t mask_set, અચલ uint16_t mask_clear)
{
	પૂર્ણાંક ret = PX4_OK;
	_olat = (_olat & ~mask_clear) | mask_set;

	માટે (પૂર્ણાંક i = 0; i < mcp_config.અંક_banks; i++) {
		uint8_t reg_addr;
		uint8_t data;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		જો (ret != PX4_OK) {
			પાછા ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, data);

		જો (ret != PX4_OK || data != curr_olat) {
			પાછા PX4_ERROR;
		}
	}

	પાછા ret;
}

પૂર્ણાંક MCP230XX::read_reg(અચલ uint8_t address, uint8_t &data)
{
	પૂર્ણાંક ret = transfer(&address, 1, &data, 1);

	જો (ret != PX4_OK) {
		perf_ગણતરી(_comms_errors);
	}

	પાછા ret;
}

પૂર્ણાંક MCP230XX::write_reg(અચલ uint8_t address, અચલ uint8_t value)
{
	uint8_t data[2] = {address, value};
	પૂર્ણાંક ret = transfer(data, 2, nullptr, 0);

	જો (ret != PX4_OK) {
		perf_ગણતરી(_comms_errors);
	}

	પાછા ret;
}

પૂર્ણાંક MCP230XX::configure(અચલ uint16_t mask, PinType type)
{
	પસંદગી (type) {
	સ્થિતિ PinType::આગત_2:
		_iodir |= mask;
		_gppu &= ~mask;
		તોડો;

	સ્થિતિ PinType::આગત_Pull_Up:
		_iodir |= mask;
		_gppu |= mask;
		તોડો;

	સ્થિતિ PinType::Output:
		_iodir &= ~mask;
		તોડો;

	મૂળભૂત:
		પાછા -EINVAL;
	}

	પૂર્ણાંક ret = PX4_OK;

	માટે (પૂર્ણાંક i = 0; i < mcp_config.અંક_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		જો (ret != PX4_OK) {
			પાછા ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		જો (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			પાછા ret;
		}
	}

	પાછા PX4_OK;
}

પૂર્ણાંક MCP230XX::init()
{
	પૂર્ણાંક ret = I2C::init();

	જો (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		પાછા ret;
	}

	ScheduleNow();
	પાછા PX4_OK;
}

પૂર્ણાંક MCP230XX::init_communication()
{
	પૂર્ણાંક ret = mcp230XX_register_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.first_minor, _iodir, mcp_config.અંક_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	પાછા ret;
}

પૂર્ણાંક MCP230XX::set_up()
{
	પૂર્ણાંક ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	માટે (પૂર્ણાંક i = 0; i < mcp_config.અંક_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		જો (ret != PX4_OK) {
			પાછા ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		જો (ret != PX4_OK) {
			પાછા ret;
		}
	}

	પાછા ret;
}

પૂર્ણાંક MCP230XX::sanity_check()
{
	પૂર્ણાંક ret = PX4_OK;

	માટે (પૂર્ણાંક i = 0; i < mcp_config.અંક_banks; i++) {
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

		જો (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_ગણતરી(_register_check);
			પાછા PX4_ERROR;
		}
	}

	પાછા ret;
}

પૂર્ણાંક MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t data;
	uint8_t addr;
	પૂર્ણાંક ret = PX4_OK;

	માટે (પૂર્ણાંક i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		જો (ret == PX4_OK) {
			ret = read_reg(addr, data);

			જો (ret == PX4_OK) {
				પાછા PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	પાછા PX4_ERROR;
}

શૂન્ય MCP230XX::RunImpl()
{
	જો (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		પાછા;
	}

	પૂર્ણાંક ret = PX4_OK;

	પસંદગી (_curr_state) {
	સ્થિતિ STATE::INIT_COMMUNICATION:
		ret = init_communication();

		જો (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			ScheduleNow();

		} નહીંતર {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		તોડો;

	સ્થિતિ STATE::CONFIGURE:
		ret = set_up();

		જો (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			ScheduleNow();

		} નહીંતર {
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		તોડો;

	સ્થિતિ STATE::CHECK:
		ret = sanity_check();

		જો (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			ScheduleOnInterval(mcp_config.interval * 1000);

		} નહીંતર {
			_curr_state = STATE::CONFIGURE;
			ScheduleDelayed(mcp_config.interval * 1000);
		}

		તોડો;

	સ્થિતિ STATE::RUNNING:
		perf_begin(_cycle_perf);
		gpio_config_s config;

		જો (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::આગત_2;

			પસંદગી (config.config) {
			સ્થિતિ config.આગત_PULLUP:	type = PinType::આગત_Pull_Up; તોડો;

			સ્થિતિ config.OUTPUT:		type = PinType::Output; તોડો;
			}

			ret |= લિખ(config.state, config.mask);
			ret |= configure(config.mask, type);
		}

		gpio_out_s output;

		જો (_gpio_out_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= લિખ(output.state, output.mask);
		}

		{
			gpio_in_s _gpio_in;
			_gpio_in.timestamp = hrt_absolute_time();
			_gpio_in.device_id = get_device_id();
			uint16_t આગત = 0;

			જો (પઠ(&આગત) == PX4_OK) {
				_gpio_in.state = આગત;
				_to_gpio_in.publish(_gpio_in);
			}
		}

		ગણતરી_3++;

		જો (ગણતરી_3 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			ગણતરી_3 = 0;
			ScheduleClear();
			ScheduleNow();
		}

		perf_end(_cycle_perf);
	}
}

શૂન્ય MCP230XX::print_status()
{
	I2CSPIDriverBase::print_status();
	perf_print_counter(_cycle_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(_register_check);
}

શૂન્ય MCP230XX::custom_method(અચલ BusCLIArguments &cli)
{
	પસંદગી (cli.custom1) {
	સ્થિતિ 0: {
			// write: only pins configured as outputs are affected
			અચલ uint16_t outputs = (uint16_t)~_iodir;
			અચલ uint16_t state = (uint16_t)cli.custom2;

			જો (લિખ(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			તોડો;
		}
	}
}

શૂન્ય MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(સાચું, ખોટું);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", સાચું);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", સાચું);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", સાચું);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", સાચું);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", સાચું);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2CSPIDriverBase *MCP230XX::instantiate(અચલ I2CSPIDriverConfig &config, પૂર્ણાંક runtime_instance)
{
	MCP230XX *instance = nullptr;

	પસંદગી (config.devid_driver_index) {
	સ્થિતિ DRV_GPIO_DEVTYPE_MCP23009:
		instance = નવું MCP23009(config);
		તોડો;

	સ્થિતિ DRV_GPIO_DEVTYPE_MCP23017:
		instance = નવું MCP23017(config);
		તોડો;

	મૂળભૂત:
		instance = nullptr;
		તોડો;
	}

	જો (!instance) {
		PX4_ERR("alloc failed");
		પાછા nullptr;
	}

	MCP230XX_config_t tmp_config = *static_cast<અચલ MCP230XX_config_t *>(config.custom_data);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_address;

	જો (OK != instance->init()) {
		delete instance;
		પાછા nullptr;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	પાછા instance;
}
