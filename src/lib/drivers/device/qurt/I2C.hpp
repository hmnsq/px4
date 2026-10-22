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

#অন্তর্ভাব "../CDev.hpp"
#অন্তর্ভাব <px4_platform_common/i2c.h>

#যদি defined(CONFIG_I2C)

কাঠামো I2CSPIDriverConfig;

নামাকাশ device __EXPORT
{

/**
 * Abstract class for character device on I2C
 */
শ্রেণী __EXPORT I2C : সর্বজনীন CDev
{

সর্বজনীন:

	// no copy, assignment, move, move assignment
	I2C(ধ্রুবক I2C &) = delete;
	I2C &operator=(ধ্রুবক I2C &) = delete;
	I2C(I2C &&) = delete;
	I2C &operator=(I2C &&) = delete;

	আভাসী পূর্ণসংখ্যা	init() override;

	প্রকারলেখ পূর্ণসংখ্যা (*_config_i2c_bus_func_t)(uint8_t, uint8_t, uint32_t);
	প্রকারলেখ পূর্ণসংখ্যা (*_set_i2c_address_func_t)(পূর্ণসংখ্যা, uint8_t);
	প্রকারলেখ পূর্ণসংখ্যা (*_i2c_transfer_func_t)(পূর্ণসংখ্যা, ধ্রুবক uint8_t *, ধ্রুবক unsigned, uint8_t *, ধ্রুবক unsigned);

	স্থির শূন্য configure_callbacks(_config_i2c_bus_func_t config_func,
					_set_i2c_address_func_t addr_func,
					_i2c_transfer_func_t transfer_func)
	{
		_config_i2c_bus = config_func;
		_set_i2c_address = addr_func;
		_i2c_transfer = transfer_func;
	}

সুরক্ষিত:
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
	I2C(uint8_t device_type, ধ্রুবক অক্ষর *name, ধ্রুবক পূর্ণসংখ্যা bus, ধ্রুবক uint16_t address, ধ্রুবক uint32_t frequency);
	I2C(ধ্রুবক I2CSPIDriverConfig &config);
	আভাসী ~I2C();

	/**
	 * Check for the presence of the device on the bus.
	 */
	আভাসী পূর্ণসংখ্যা	probe() { ফেরত PX4_OK; }

	আভাসী শূন্য set_device_address(পূর্ণসংখ্যা address, যৌক্তিক log = সত্য);

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
	পূর্ণসংখ্যা		transfer(ধ্রুবক uint8_t *send, ধ্রুবক unsigned send_len, uint8_t *recv, ধ্রুবক unsigned recv_len);

ব্যক্তিগত:
	uint32_t		               _frequency{0};
	পূর্ণসংখ্যা                            _i2c_fd{-1};
	pthread_mutex_t               *_mutex{nullptr};

	স্থির ধ্রুবক পূর্ণসংখ্যা MAX_I2C_BUS{4};

	স্থির _config_i2c_bus_func_t  _config_i2c_bus;
	স্থির _set_i2c_address_func_t _set_i2c_address;
	স্থির _i2c_transfer_func_t    _i2c_transfer;

	স্থির কাঠামো _bus_mutex_t {
		পূর্ণসংখ্যা _bus;
		pthread_mutex_t _mutex{PTHREAD_MUTEX_INITIALIZER};
	} _bus_mutex[MAX_I2C_BUS];
};

} // namespace device

#endif /* _DEVICE_I2C_H */
