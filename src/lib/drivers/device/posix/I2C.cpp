/****************************************************************************
 *
 *   Copyright (c) 2016-2021 PX4 Development Team. All rights reserved.
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
 * @file I2C.cpp
 *
 * Base class for devices attached via the I2C bus.
 *
 * @todo Bus frequency changes; currently we do nothing with the value
 *       that is supplied.  Should we just depend on the bus knowing?
 */

#ضمن "I2C.hpp"

#إذا defined(CONFIG_I2C)

#اگر_تعریف_شده __PX4_LINUX

#ضمن <linux/i2c.h>
#ضمن <linux/i2c-dev.h>

#ضمن <px4_platform_common/i2c_spi_buses.h>

نطاق device
{

I2C::I2C(uint8_t device_type, ثابت حرف *name, ثابت صحيح bus, ثابت uint16_t آدرس, ثابت uint32_t frequency) :
	CDev(name, اشاره_تهی)
{
	// fill in _device_id fields for a I2C device
	_device_id.devid_s.devtype = device_type;
	_device_id.devid_s.bus_type = DeviceBusType_I2C;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.آدرس = آدرس;
}

I2C::I2C(ثابت I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_شاخص, config.module_name, config.bus, config.i2c_آدرس, config.bus_frequency)
{
	مجموعه_external(config.external);
}

I2C::~I2C()
{
	إذا (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

صحيح
I2C::init()
{
	صحيح ret = PX4_ERROR;

	إذا (_fd >= 0) {
		أرجع PX4_OK;
	}

	// Open the actual I2C device
	حرف dev_مسیر[16] {};
	snprintf(dev_مسیر, حجم(dev_مسیر), "/dev/i2c-%i", get_device_bus());
	_fd = ::open(dev_مسیر, O_RDWR);

	إذا (_fd < 0) {
		DEVICE_DEBUG("failed to init I2C");
		ret = -ENOENT;
		اذهب بیرون;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	إذا (ret != OK) {
		DEVICE_DEBUG("probe failed");
		اذهب بیرون;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	إذا (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		اذهب بیرون;
	}

	// tell the world where we are
	DEVICE_DEBUG("on I2C bus %d at 0x%02x", get_device_bus(), get_device_آدرس());

بیرون:

	إذا ((ret != OK) && !(_fd < 0)) {
		::close(_fd);
		_fd = -1;
	}

	أرجع ret;
}

صحيح
I2C::transfer(ثابت uint8_t *send, ثابت بیعلامت send_len, uint8_t *recv, ثابت بیعلامت recv_len)
{
	صحيح ret = PX4_ERROR;
	بیعلامت retry_شمار = 0;

	إذا (_fd < 0) {
		PX4_ERR("I2C device not opened");
		أرجع PX4_ERROR;
	}

	افعل {
		DEVICE_DEBUG("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

		بیعلامت msgs = 0;
		بنية i2c_پیام msgv[2] {};

		إذا (send_len > 0) {
			msgv[msgs].addr = get_device_آدرس();
			msgv[msgs].flags = 0;
			msgv[msgs].buf = const_cast<uint8_t *>(send);
			msgv[msgs].len = send_len;
			msgs++;
		}

		إذا (recv_len > 0) {
			msgv[msgs].addr = get_device_آدرس();
			msgv[msgs].flags = I2C_M_RD;
			msgv[msgs].buf = recv;
			msgv[msgs].len = recv_len;
			msgs++;
		}

		إذا (msgs == 0) {
			أرجع -EINVAL;
		}

		i2c_rdwr_ioctl_داده packets{};
		packets.msgs  = msgv;
		packets.nmsgs = msgs;

		صحيح ret_ioctl = ::ioctl(_fd, I2C_RDWR, (بیعلامت دراز)&packets);

		إذا (ret_ioctl == -1) {
			DEVICE_DEBUG("I2C transfer failed");
			ret = PX4_ERROR;

		} وإلا {
			// success
			ret = PX4_OK;
			اكسر;
		}

	} طالما (retry_شمار++ < _retries);

	أرجع ret;
}

} // namespace device

#پایان_اگر // __PX4_LINUX

#پایان_اگر // CONFIG_I2C
