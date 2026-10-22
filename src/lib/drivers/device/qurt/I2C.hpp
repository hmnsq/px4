/****************************************************************************
 *
 *   Copyright (C) 2016-2020 PX4 Development Team. All rights reserved.
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

/**
 * @file I2C.hpp
 *
 * Base class for devices connected via I2C.
 */

#pragma once

#അന്തര്ഭാവ "../CDev.hpp"
#അന്തര്ഭാവ <px4_platform_common/i2c.h>

#എങ്കിൽ defined(CONFIG_I2C)

ഘടന I2CSPIDriverConfig;

നാമാകാശ device __EXPORT
{

/**
 * Abstract class for character device on I2C
 */
വർഗ്ഗം __EXPORT I2C : പൊതു CDev
{

പൊതു:

	// no copy, assignment, move, move assignment
	I2C(സ്ഥിരം I2C &) = delete;
	I2C &operator=(സ്ഥിരം I2C &) = delete;
	I2C(I2C &&) = delete;
	I2C &operator=(I2C &&) = delete;

	മായാ പൂർണ്ണസംഖ്യ	init() override;

	തരനിർവചനം പൂർണ്ണസംഖ്യ (*_config_i2c_bus_func_t)(uint8_t, uint8_t, uint32_t);
	തരനിർവചനം പൂർണ്ണസംഖ്യ (*_set_i2c_address_func_t)(പൂർണ്ണസംഖ്യ, uint8_t);
	തരനിർവചനം പൂർണ്ണസംഖ്യ (*_i2c_transfer_func_t)(പൂർണ്ണസംഖ്യ, സ്ഥിരം uint8_t *, സ്ഥിരം unsigned, uint8_t *, സ്ഥിരം unsigned);

	സ്ഥിതം ശൂന്യം configure_callbacks(_config_i2c_bus_func_t config_func,
					_set_i2c_address_func_t addr_func,
					_i2c_transfer_func_t transfer_func)
	{
		_config_i2c_bus = config_func;
		_set_i2c_address = addr_func;
		_i2c_transfer = transfer_func;
	}

സംരക്ഷിത:
	/**
	 * The number of times a read or write operation will be retried on
	 * error.
	 */
	uint8_t		_retries{0};

	/**
	 * @ Constructor
	 *
	 * @param device_type	The device type (see drv_sensor.h)
	 * @param name		Driver name
	 * @param bus		I2C bus on which the device lives
	 * @param address	I2C bus address, or zero if set_address will be used
	 * @param frequency	I2C bus frequency for the device (currently not used)
	 */
	I2C(uint8_t device_type, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം പൂർണ്ണസംഖ്യ bus, സ്ഥിരം uint16_t address, സ്ഥിരം uint32_t frequency);
	I2C(സ്ഥിരം I2CSPIDriverConfig &config);
	മായാ ~I2C();

	/**
	 * Check for the presence of the device on the bus.
	 */
	മായാ പൂർണ്ണസംഖ്യ	probe() { തിരികെ PX4_OK; }

	മായാ ശൂന്യം set_device_address(പൂർണ്ണസംഖ്യ address, തർക്കം log = സത്യം);

	/**
	 * Perform an I2C transaction to the device.
	 *
	 * At least one of send_len and recv_len must be non-zero.
	 *
	 * @param send		Pointer to bytes to send.
	 * @param send_len	Number of bytes to send.
	 * @param recv		Pointer to buffer for bytes received.
	 * @param recv_len	Number of bytes to receive.
	 * @return		OK if the transfer was successful, -errno
	 *			otherwise.
	 */
	പൂർണ്ണസംഖ്യ		transfer(സ്ഥിരം uint8_t *send, സ്ഥിരം unsigned send_len, uint8_t *recv, സ്ഥിരം unsigned recv_len);

സ്വകാര്യ:
	uint32_t		               _frequency{0};
	പൂർണ്ണസംഖ്യ                            _i2c_fd{-1};
	pthread_mutex_t               *_mutex{nullptr};

	സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ MAX_I2C_BUS{4};

	സ്ഥിതം _config_i2c_bus_func_t  _config_i2c_bus;
	സ്ഥിതം _set_i2c_address_func_t _set_i2c_address;
	സ്ഥിതം _i2c_transfer_func_t    _i2c_transfer;

	സ്ഥിതം ഘടന _bus_mutex_t {
		പൂർണ്ണസംഖ്യ _bus;
		pthread_mutex_t _mutex{PTHREAD_MUTEX_INITIALIZER};
	} _bus_mutex[MAX_I2C_BUS];
};

} // namespace device

#endif /* _DEVICE_I2C_H */
