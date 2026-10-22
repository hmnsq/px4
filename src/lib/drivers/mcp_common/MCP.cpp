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

MCP230XX::MCP230XX(ಸ್ಥಿರಾಂಕ I2CSPIDriverConfig &config) :
	I2C(config),
	I2CSPIDriver(config),
	ಚಕ್ರ_perf(perf_alloc(PC_ELAPSED, MODULE_NAME": single-sample")),
	_comms_errors(perf_alloc(PC_ಎಣಿಕೆ, MODULE_NAME": comms errors")),
	ರಿಜಿಸ್ಟರ್_check(perf_alloc(PC_ಎಣಿಕೆ, MODULE_NAME": register check"))
{
}

MCP230XX::~MCP230XX()
{
	ವೇಳಾಪಟ್ಟಿ_Clear();
	perf_free(ಚಕ್ರ_perf);
	perf_free(_comms_errors);
	perf_free(ರಿಜಿಸ್ಟರ್_check);
}

ಪೂರ್ಣಾಂಕ MCP230XX::init_uorb()
{
	ಆದರೆ (!_gpio_config_sub.ರಿಜಿಸ್ಟರ್_Callback() ||
	    !gpio_ಹೊರಹೋಗು_sub.ರಿಜಿಸ್ಟರ್_Callback()) {
		PX4_ERR("callback registration failed");
		ಹಿಂತಿರುಗಿ PX4_ERROR;
	}

	ಹಿಂತಿರುಗಿ PX4_OK;
}

ಶೂನ್ಯ MCP230XX::cleanup_uorb()
{
	_gpio_config_sub.unregisterCallback();
	gpio_ಹೊರಹೋಗು_sub.unregisterCallback();
	mcp230XX_unregister_gpios(mcp_config.ಮೊದಲ_minor, mcp_config.ಅಂಕೆ_pins, mcp_config._gpio_handle);
}

ಪೂರ್ಣಾಂಕ MCP230XX::ಪಠ(uint16_t *ಮುಖವಾಡ)
{
	*ಮುಖವಾಡ = 0;
	ಪೂರ್ಣಾಂಕ ret = PX4_OK;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < mcp_config.ಅಂಕೆ_banks; i++) {
		uint8_t gpio_addr;
		uint8_t ದತ್ತಾಂಶ;

		ret |= get_gpio(i, &gpio_addr);
		ret |= read_reg(gpio_addr, ದತ್ತಾಂಶ);

		*ಮುಖವಾಡ |= (((uint16_t) ದತ್ತಾಂಶ) << (8 * i));
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ MCP230XX::ಲಿಖ(ಸ್ಥಿರಾಂಕ uint16_t ಮುಖವಾಡ_ಗಣ, ಸ್ಥಿರಾಂಕ uint16_t ಮುಖವಾಡ_clear)
{
	ಪೂರ್ಣಾಂಕ ret = PX4_OK;
	_olat = (_olat & ~ಮುಖವಾಡ_clear) | ಮುಖವಾಡ_ಗಣ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < mcp_config.ಅಂಕೆ_banks; i++) {
		uint8_t reg_addr;
		uint8_t ದತ್ತಾಂಶ;
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);

		ret = get_olat(i, &reg_addr);

		ಆದರೆ (ret != PX4_OK) {
			ಹಿಂತಿರುಗಿ ret;
		}

		ret |= write_reg(reg_addr, curr_olat);
		ret |= read_reg(reg_addr, ದತ್ತಾಂಶ);

		ಆದರೆ (ret != PX4_OK || ದತ್ತಾಂಶ != curr_olat) {
			ಹಿಂತಿರುಗಿ PX4_ERROR;
		}
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ MCP230XX::read_reg(ಸ್ಥಿರಾಂಕ uint8_t ವಿಳಾಸ, uint8_t &ದತ್ತಾಂಶ)
{
	ಪೂರ್ಣಾಂಕ ret = transfer(&ವಿಳಾಸ, 1, &ದತ್ತಾಂಶ, 1);

	ಆದರೆ (ret != PX4_OK) {
		perf_ಎಣಿಕೆ(_comms_errors);
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ MCP230XX::write_reg(ಸ್ಥಿರಾಂಕ uint8_t ವಿಳಾಸ, ಸ್ಥಿರಾಂಕ uint8_t value)
{
	uint8_t ದತ್ತಾಂಶ[2] = {ವಿಳಾಸ, value};
	ಪೂರ್ಣಾಂಕ ret = transfer(ದತ್ತಾಂಶ, 2, ಶೂನ್ಯಸೂಚಕ, 0);

	ಆದರೆ (ret != PX4_OK) {
		perf_ಎಣಿಕೆ(_comms_errors);
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ MCP230XX::configure(ಸ್ಥಿರಾಂಕ uint16_t ಮುಖವಾಡ, PinType type)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ PinType::ಒಳಹರಿವು_2:
		_iodir |= ಮುಖವಾಡ;
		_gppu &= ~ಮುಖವಾಡ;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PinType::ಒಳಹರಿವು_Pull_ಮೇಲೆ:
		_iodir |= ಮುಖವಾಡ;
		_gppu |= ಮುಖವಾಡ;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PinType::Output:
		_iodir &= ~ಮುಖವಾಡ;
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ -EINVAL;
	}

	ಪೂರ್ಣಾಂಕ ret = PX4_OK;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < mcp_config.ಅಂಕೆ_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);
		uint8_t iodir_addr;
		uint8_t gppu_addr;

		ret = get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		ಆದರೆ (ret != PX4_OK) {
			ಹಿಂತಿರುಗಿ ret;
		}

		ret = write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(gppu_addr, curr_gppu);

		ಆದರೆ (ret != PX4_OK) {
			PX4_ERR("MCP configure failed");
			ಹಿಂತಿರುಗಿ ret;
		}
	}

	ಹಿಂತಿರುಗಿ PX4_OK;
}

ಪೂರ್ಣಾಂಕ MCP230XX::init()
{
	ಪೂರ್ಣಾಂಕ ret = I2C::init();

	ಆದರೆ (ret != PX4_OK) {
		PX4_ERR("I2C init failed");
		ಹಿಂತಿರುಗಿ ret;
	}

	ವೇಳಾಪಟ್ಟಿ_Now();
	ಹಿಂತಿರುಗಿ PX4_OK;
}

ಪೂರ್ಣಾಂಕ MCP230XX::init_communication()
{
	ಪೂರ್ಣಾಂಕ ret = mcp230_XX_ರಿಜಿಸ್ಟರ್_gpios(mcp_config.i2c_bus, mcp_config.i2c_addr, mcp_config.ಮೊದಲ_minor, _iodir, mcp_config.ಅಂಕೆ_pins,
					  mcp_config.device_type, mcp_config._gpio_handle);
	ret |= init_uorb();
	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ MCP230XX::ಗಣ_ಮೇಲೆ_2()
{
	ಪೂರ್ಣಾಂಕ ret = PX4_OK;
	uint8_t iodir_addr;
	uint8_t olat_addr;
	uint8_t gppu_addr;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < mcp_config.ಅಂಕೆ_banks; i++) {
		uint8_t curr_iodir = (uint8_t)((_iodir >> (8 * i)) & 0x00FF);
		uint8_t curr_olat = (uint8_t)((_olat >> (8 * i)) & 0x00FF);
		uint8_t curr_gppu = (uint8_t)((_gppu >> (8 * i)) & 0x00FF);

		ret |= get_olat(i, &olat_addr);
		ret |= get_iodir(i, &iodir_addr);
		ret |= get_gppu(i, &gppu_addr);

		ಆದರೆ (ret != PX4_OK) {
			ಹಿಂತಿರುಗಿ ret;
		}

		ret |= write_reg(iodir_addr, curr_iodir);
		ret |= write_reg(olat_addr, curr_olat);
		ret |= write_reg(gppu_addr, curr_gppu);

		ಆದರೆ (ret != PX4_OK) {
			ಹಿಂತಿರುಗಿ ret;
		}
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ MCP230XX::sanity_check()
{
	ಪೂರ್ಣಾಂಕ ret = PX4_OK;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < mcp_config.ಅಂಕೆ_banks; i++) {
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

		ಆದರೆ (read_iodir != curr_iodir || read_olat != curr_olat || read_gppu != curr_gppu) {
			perf_ಎಣಿಕೆ(ರಿಜಿಸ್ಟರ್_check);
			ಹಿಂತಿರುಗಿ PX4_ERROR;
		}
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ MCP230XX::probe()
{
	// no whoami, try to read IOCONA
	uint8_t ದತ್ತಾಂಶ;
	uint8_t addr;
	ಪೂರ್ಣಾಂಕ ret = PX4_OK;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 10; i++) {
		ret = get_probe_reg(&addr);

		ಆದರೆ (ret == PX4_OK) {
			ret = read_reg(addr, ದತ್ತಾಂಶ);

			ಆದರೆ (ret == PX4_OK) {
				ಹಿಂತಿರುಗಿ PX4_OK;
			}
		}

		px4_usleep(10'000);
	}

	ಹಿಂತಿರುಗಿ PX4_ERROR;
}

ಶೂನ್ಯ MCP230XX::ಸತತ_Impl()
{
	ಆದರೆ (should_exit()) {
		cleanup_uorb();
		exit_and_cleanup();
		ಹಿಂತಿರುಗಿ;
	}

	ಪೂರ್ಣಾಂಕ ret = PX4_OK;

	ಆಯ್ಕೆ (_curr_state) {
	ಸ್ಥಿತಿ STATE::INIT_COMMUNICATION:
		ret = init_communication();

		ಆದರೆ (ret == PX4_OK) {
			_curr_state = STATE::CONFIGURE;
			ವೇಳಾಪಟ್ಟಿ_Now();

		} ಇಲ್ಲದಿದ್ದರೆ {
			ವೇಳಾಪಟ್ಟಿ_Delayed(mcp_config.interval * 1000);
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE::CONFIGURE:
		ret = ಗಣ_ಮೇಲೆ_2();

		ಆದರೆ (ret == PX4_OK) {
			_curr_state = STATE::CHECK;
			ವೇಳಾಪಟ್ಟಿ_Now();

		} ಇಲ್ಲದಿದ್ದರೆ {
			ವೇಳಾಪಟ್ಟಿ_Delayed(mcp_config.interval * 1000);
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE::CHECK:
		ret = sanity_check();

		ಆದರೆ (ret == PX4_OK) {
			_curr_state = STATE::RUNNING;
			ವೇಳಾಪಟ್ಟಿ_On_Interval(mcp_config.interval * 1000);

		} ಇಲ್ಲದಿದ್ದರೆ {
			_curr_state = STATE::CONFIGURE;
			ವೇಳಾಪಟ್ಟಿ_Delayed(mcp_config.interval * 1000);
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STATE::RUNNING:
		perf_begin(ಚಕ್ರ_perf);
		gpio_config_s config;

		ಆದರೆ (_gpio_config_sub.update(&config) && config.device_id == get_device_id()) {
			PinType type = PinType::ಒಳಹರಿವು_2;

			ಆಯ್ಕೆ (config.config) {
			ಸ್ಥಿತಿ config.ಒಳಹರಿವು_PULLUP:	type = PinType::ಒಳಹರಿವು_Pull_ಮೇಲೆ; ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ config.OUTPUT:		type = PinType::Output; ನಿಲ್ಲಿಸು;
			}

			ret |= ಲಿಖ(config.state, config.ಮುಖವಾಡ);
			ret |= configure(config.ಮುಖವಾಡ, type);
		}

		gpio_ಹೊರಹೋಗು_s output;

		ಆದರೆ (gpio_ಹೊರಹೋಗು_sub.update(&output) && output.device_id == get_device_id()) {
			ret |= ಲಿಖ(output.state, output.ಮುಖವಾಡ);
		}

		{
			gpio_ಒಳ_s gpio_ಒಳ;
			gpio_ಒಳ.timestamp = hrt_absolute_ಸಮಯ();
			gpio_ಒಳ.device_id = get_device_id();
			uint16_t ಒಳಹರಿವು = 0;

			ಆದರೆ (ಪಠ(&ಒಳಹರಿವು) == PX4_OK) {
				gpio_ಒಳ.state = ಒಳಹರಿವು;
				ಗಮ್ಯ_gpio_ಒಳ.publish(gpio_ಒಳ);
			}
		}

		ಎಣಿಕೆ_4++;

		ಆದರೆ (ಎಣಿಕೆ_4 >= _checking_freq || ret != PX4_OK) {
			_curr_state = STATE::CHECK;
			ಎಣಿಕೆ_4 = 0;
			ವೇಳಾಪಟ್ಟಿ_Clear();
			ವೇಳಾಪಟ್ಟಿ_Now();
		}

		perf_ಕೊನೆ(ಚಕ್ರ_perf);
	}
}

ಶೂನ್ಯ MCP230XX::print_ಸ್ಥಿತಿ()
{
	I2_CSPIDriver_ತಳ::print_ಸ್ಥಿತಿ();
	perf_print_counter(ಚಕ್ರ_perf);
	perf_print_counter(_comms_errors);
	perf_print_counter(ರಿಜಿಸ್ಟರ್_check);
}

ಶೂನ್ಯ MCP230XX::custom_ವಿಧಾನ(ಸ್ಥಿರಾಂಕ BusCLIArguments &cli)
{
	ಆಯ್ಕೆ (cli.custom1) {
	ಸ್ಥಿತಿ 0: {
			// write: only pins configured as outputs are affected
			ಸ್ಥಿರಾಂಕ uint16_t outputs = (uint16_t)~_iodir;
			ಸ್ಥಿರಾಂಕ uint16_t state = (uint16_t)cli.custom2;

			ಆದರೆ (ಲಿಖ(state & outputs, (uint16_t)~state & outputs) != PX4_OK) {
				PX4_ERR("write failed");
			}

			ನಿಲ್ಲಿಸು;
		}
	}
}

ಶೂನ್ಯ MCP230XX::print_usage()
{
	PRINT_MODULE_USAGE_NAME("mcp230xx", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(ನಿಜ, ಸುಳ್ಳು);
	PRINT_MODULE_USAGE_PARAMS_I2_C_ವಿಳಾಸ(0x27);
	PRINT_MODULE_USAGE_PARAM_INT('D', 0, 0, 65535, "Direction (1=Input, 0=Output)", ನಿಜ);
	PRINT_MODULE_USAGE_PARAM_INT('O', 0, 0, 65535, "Output", ನಿಜ);
	PRINT_MODULE_USAGE_PARAM_INT('P', 0, 0, 65535, "Pullups", ನಿಜ);
	PRINT_MODULE_USAGE_PARAM_INT('U', 0, 0, 1000, "Update Interval [ms]", ನಿಜ);
	PRINT_MODULE_USAGE_PARAM_INT('M', 0, 0, 255, "First minor number", ನಿಜ);
	PRINT_MODULE_USAGE_COMMAND_DESCR("write", "Set the output pins to the state given with -O");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2_CSPIDriver_ತಳ *MCP230XX::instantiate(ಸ್ಥಿರಾಂಕ I2CSPIDriverConfig &config, ಪೂರ್ಣಾಂಕ runtime_instance)
{
	MCP230XX *instance = ಶೂನ್ಯಸೂಚಕ;

	ಆಯ್ಕೆ (config.devid_driver_ಸೂಚ್ಯಂಕ) {
	ಸ್ಥಿತಿ DRV_GPIO_DEVTYPE_MCP23009:
		instance = ಹೊಸ MCP23009(config);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ DRV_GPIO_DEVTYPE_MCP23017:
		instance = ಹೊಸ MCP23017(config);
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		instance = ಶೂನ್ಯಸೂಚಕ;
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (!instance) {
		PX4_ERR("alloc failed");
		ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
	}

	MCP230XX_config_t tmp_config = *static_cast<ಸ್ಥಿರಾಂಕ MCP230XX_config_t *>(config.custom_ದತ್ತಾಂಶ);
	instance->mcp_config = tmp_config;
	instance->mcp_config.i2c_bus = config.bus;
	instance->mcp_config.i2c_addr = config.i2c_ವಿಳಾಸ;

	ಆದರೆ (OK != instance->init()) {
		ಅಳಿಸು instance;
		ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
	}

	instance->_iodir = instance->mcp_config.direction;
	instance->_olat = instance->mcp_config.state;
	instance->_gppu = instance->mcp_config.pullup;

	ಹಿಂತಿರುಗಿ instance;
}
