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

#ہدایت once

#شامل "../CDev.hpp"
#شامل <px4_platform_common/i2c.h>

#اگر defined(CONFIG_I2C)

ساخت I2CSPIDriverConfig;

نامخلا device __EXPORT
{

/**
 * Abstract class for character device on I2C
 */
جماعت __EXPORT I2C : عوامی CDev
{

عوامی:

	// no copy, assignment, move, move assignment
	I2C(ثابت I2C &) = حذف;
	I2C &عامل=(ثابت I2C &) = حذف;
	I2C(I2C &&) = حذف;
	I2C &عامل=(I2C &&) = حذف;

	مجازی صحیح	init() override;

	قسمتعریف صحیح (*_config_i2c_bus_func_t)(uint8_t, uint8_t, uint32_t);
	قسمتعریف صحیح (*سیٹ_i2c_پتہ_func_t)(صحیح, uint8_t);
	قسمتعریف صحیح (*_i2c_transfer_func_t)(صحیح, ثابت uint8_t *, ثابت غیرعلامتی, uint8_t *, ثابت غیرعلامتی);

	ساکن خالی configure_callbacks(_config_i2c_bus_func_t config_func,
					سیٹ_i2c_پتہ_func_t addr_func,
					_i2c_transfer_func_t transfer_func)
	{
		_config_i2c_bus = config_func;
		سیٹ_i2c_پتہ = addr_func;
		_i2c_transfer = transfer_func;
	}

محفوظ:
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
	I2C(uint8_t device_type, ثابت حرف *name, ثابت صحیح bus, ثابت uint16_t پتہ, ثابت uint32_t frequency);
	I2C(ثابت I2CSPIDriverConfig &config);
	مجازی ~I2C();

	/**
	 * Check for the presence of the device on the bus.
	 */
	مجازی صحیح	probe() { واپس PX4_OK; }

	مجازی خالی سیٹ_device_پتہ(صحیح پتہ, منطقی لوگارتھم = سچ);

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
	صحیح		transfer(ثابت uint8_t *send, ثابت غیرعلامتی send_len, uint8_t *recv, ثابت غیرعلامتی recv_len);

نجی:
	uint32_t		               _frequency{0};
	صحیح                            _i2c_fd{-1};
	pthread_mutex_t               *_mutex{خالی_اشاریہ};

	ساکن ثابت صحیح MAX_I2C_BUS{4};

	ساکن _config_i2c_bus_func_t  _config_i2c_bus;
	ساکن سیٹ_i2c_پتہ_func_t سیٹ_i2c_پتہ;
	ساکن _i2c_transfer_func_t    _i2c_transfer;

	ساکن ساخت _bus_mutex_t {
		صحیح _bus;
		pthread_mutex_t _mutex{PTHREAD_MUTEX_INITIALIZER};
	} _bus_mutex[MAX_I2C_BUS];
};

} // namespace device

#اگر_ختم /* _DEVICE_I2C_H */
