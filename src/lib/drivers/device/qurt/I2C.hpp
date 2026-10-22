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

#ಅನ್ತರ್ಭಾವ "../CDev.hpp"
#ಅನ್ತರ್ಭಾವ <px4_platform_common/i2c.h>

#ಯದಿ defined(CONFIG_I2C)

ಸಂರಚನಾ I2CSPIDriverConfig;

ನಾಮಾಕಾಶ device __EXPORT
{

/**
 * Abstract class for character device on I2C
 */
ವರ್ಗ __EXPORT I2C : ಸಾರ್ವಜನಿಕ CDev
{

ಸಾರ್ವಜನಿಕ:

	// no copy, assignment, move, move assignment
	I2C(ಧ್ರುವ I2C &) = delete;
	I2C &operator=(ಧ್ರುವ I2C &) = delete;
	I2C(I2C &&) = delete;
	I2C &operator=(I2C &&) = delete;

	ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ	init() override;

	ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*_config_i2c_bus_func_t)(uint8_t, uint8_t, uint32_t);
	ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*_set_i2c_address_func_t)(ಪೂರ್ಣಾಙ್ಕ, uint8_t);
	ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*_i2c_transfer_func_t)(ಪೂರ್ಣಾಙ್ಕ, ಧ್ರುವ uint8_t *, ಧ್ರುವ unsigned, uint8_t *, ಧ್ರುವ unsigned);

	ಸ್ಥಿರ ಶೂನ್ಯ configure_callbacks(_config_i2c_bus_func_t config_func,
					_set_i2c_address_func_t addr_func,
					_i2c_transfer_func_t transfer_func)
	{
		_config_i2c_bus = config_func;
		_set_i2c_address = addr_func;
		_i2c_transfer = transfer_func;
	}

ರಕ್ಷಿತ:
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
	I2C(uint8_t device_type, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ bus, ಧ್ರುವ uint16_t address, ಧ್ರುವ uint32_t frequency);
	I2C(ಧ್ರುವ I2CSPIDriverConfig &config);
	ಆಭಾಸ ~I2C();

	/**
	 * Check for the presence of the device on the bus.
	 */
	ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ	probe() { ಪ್ರತ್ಯಾಗಮ PX4_OK; }

	ಆಭಾಸ ಶೂನ್ಯ set_device_address(ಪೂರ್ಣಾಙ್ಕ address, ತಾರ್ಕಿಕ log = ಸತ್ಯ);

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
	ಪೂರ್ಣಾಙ್ಕ		transfer(ಧ್ರುವ uint8_t *send, ಧ್ರುವ unsigned send_len, uint8_t *recv, ಧ್ರುವ unsigned recv_len);

ಗುಪ್ತ:
	uint32_t		               _frequency{0};
	ಪೂರ್ಣಾಙ್ಕ                            _i2c_fd{-1};
	pthread_mutex_t               *_mutex{nullptr};

	ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ MAX_I2C_BUS{4};

	ಸ್ಥಿರ _config_i2c_bus_func_t  _config_i2c_bus;
	ಸ್ಥಿರ _set_i2c_address_func_t _set_i2c_address;
	ಸ್ಥಿರ _i2c_transfer_func_t    _i2c_transfer;

	ಸ್ಥಿರ ಸಂರಚನಾ _bus_mutex_t {
		ಪೂರ್ಣಾಙ್ಕ _bus;
		pthread_mutex_t _mutex{PTHREAD_MUTEX_INITIALIZER};
	} _bus_mutex[MAX_I2C_BUS];
};

} // namespace device

#endif /* _DEVICE_I2C_H */
