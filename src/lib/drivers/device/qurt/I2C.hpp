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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "../CDev.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/i2c.h>

#ਜੇ defined(CONFIG_I2C)

ਬਣਤਰ I2CSPIDriverConfig;

ਨਾਮਾਕਾਸ਼ device __EXPORT
{

/**
 * Abstract class for character device on I2C
 */
ਵਰਗ __EXPORT I2C : ਜਨਤਕ CDev
{

ਜਨਤਕ:

	// no copy, assignment, move, move assignment
	I2C(ਅਚਲ I2C &) = ਮਿਟਾਓ;
	I2C &ਸੰਚਾਲਕ=(ਅਚਲ I2C &) = ਮਿਟਾਓ;
	I2C(I2C &&) = ਮਿਟਾਓ;
	I2C &ਸੰਚਾਲਕ=(I2C &&) = ਮਿਟਾਓ;

	ਆਭਾਸੀ ਪੂਰਨ	init() override;

	ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*_config_i2c_bus_func_t)(uint8_t, uint8_t, uint32_t);
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*ਸੈੱਟ_i2c_ਪਤਾ_func_t)(ਪੂਰਨ, uint8_t);
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*_i2c_transfer_func_t)(ਪੂਰਨ, ਅਚਲ uint8_t *, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ, uint8_t *, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ);

	ਸਥਿਰ ਖਾਲੀ configure_callbacks(_config_i2c_bus_func_t config_func,
					ਸੈੱਟ_i2c_ਪਤਾ_func_t addr_func,
					_i2c_transfer_func_t transfer_func)
	{
		_config_i2c_bus = config_func;
		ਸੈੱਟ_i2c_ਪਤਾ = addr_func;
		_i2c_transfer = transfer_func;
	}

ਸੁਰੱਖਿਅਤ:
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
	I2C(uint8_t device_type, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਪੂਰਨ bus, ਅਚਲ uint16_t ਪਤਾ, ਅਚਲ uint32_t frequency);
	I2C(ਅਚਲ I2CSPIDriverConfig &config);
	ਆਭਾਸੀ ~I2C();

	/**
	 * Check for the presence of the device on the bus.
	 */
	ਆਭਾਸੀ ਪੂਰਨ	probe() { ਵਾਪਸ PX4_OK; }

	ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_device_ਪਤਾ(ਪੂਰਨ ਪਤਾ, ਤਾਰਕਿਕ ਲਘੂਗਣਕ = ਸੱਚ);

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
	ਪੂਰਨ		transfer(ਅਚਲ uint8_t *send, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ send_len, uint8_t *recv, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ recv_len);

ਨਿਜੀ:
	uint32_t		               _frequency{0};
	ਪੂਰਨ                            _i2c_fd{-1};
	pthread_mutex_t               *_mutex{ਖਾਲੀ_ਸੂਚਕ};

	ਸਥਿਰ ਅਚਲ ਪੂਰਨ MAX_I2C_BUS{4};

	ਸਥਿਰ _config_i2c_bus_func_t  _config_i2c_bus;
	ਸਥਿਰ ਸੈੱਟ_i2c_ਪਤਾ_func_t ਸੈੱਟ_i2c_ਪਤਾ;
	ਸਥਿਰ _i2c_transfer_func_t    _i2c_transfer;

	ਸਥਿਰ ਬਣਤਰ _bus_mutex_t {
		ਪੂਰਨ _bus;
		pthread_mutex_t _mutex{PTHREAD_MUTEX_INITIALIZER};
	} _bus_mutex[MAX_I2C_BUS];
};

} // namespace device

#ਜੇ_ਅੰਤ /* _DEVICE_I2C_H */
