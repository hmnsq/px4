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

#رهنمود once

#ضمن "../CDev.hpp"
#ضمن <px4_platform_common/i2c.h>

#إذا defined(CONFIG_I2C)

بنية I2CSPIDriverConfig;

نطاق device __EXPORT
{

/**
 * Abstract class for character device on I2C
 */
صنف __EXPORT I2C : عام CDev
{

عام:

	// no copy, assignment, move, move assignment
	I2C(ثابت I2C &) = حذف;
	I2C &عملگر=(ثابت I2C &) = حذف;
	I2C(I2C &&) = حذف;
	I2C &عملگر=(I2C &&) = حذف;

	مجازی صحيح	init() override;

	تعريفنوع صحيح (*_config_i2c_bus_func_t)(uint8_t, uint8_t, uint32_t);
	تعريفنوع صحيح (*مجموعه_i2c_آدرس_func_t)(صحيح, uint8_t);
	تعريفنوع صحيح (*_i2c_transfer_func_t)(صحيح, ثابت uint8_t *, ثابت بیعلامت, uint8_t *, ثابت بیعلامت);

	ساكن فراغ configure_callbacks(_config_i2c_bus_func_t config_func,
					مجموعه_i2c_آدرس_func_t addr_func,
					_i2c_transfer_func_t transfer_func)
	{
		_config_i2c_bus = config_func;
		مجموعه_i2c_آدرس = addr_func;
		_i2c_transfer = transfer_func;
	}

محمي:
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
	I2C(uint8_t device_type, ثابت حرف *name, ثابت صحيح bus, ثابت uint16_t آدرس, ثابت uint32_t frequency);
	I2C(ثابت I2CSPIDriverConfig &config);
	مجازی ~I2C();

	/**
	 * Check for the presence of the device on the bus.
	 */
	مجازی صحيح	probe() { أرجع PX4_OK; }

	مجازی فراغ مجموعه_device_آدرس(صحيح آدرس, منطقي لگاریتم = درست);

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
	صحيح		transfer(ثابت uint8_t *send, ثابت بیعلامت send_len, uint8_t *recv, ثابت بیعلامت recv_len);

خاص:
	uint32_t		               _frequency{0};
	صحيح                            _i2c_fd{-1};
	pthread_mutex_t               *_mutex{اشاره_تهی};

	ساكن ثابت صحيح MAX_I2C_BUS{4};

	ساكن _config_i2c_bus_func_t  _config_i2c_bus;
	ساكن مجموعه_i2c_آدرس_func_t مجموعه_i2c_آدرس;
	ساكن _i2c_transfer_func_t    _i2c_transfer;

	ساكن بنية _bus_mutex_t {
		صحيح _bus;
		pthread_mutex_t _mutex{PTHREAD_MUTEX_INITIALIZER};
	} _bus_mutex[MAX_I2C_BUS];
};

} // namespace device

#پایان_اگر /* _DEVICE_I2C_H */
