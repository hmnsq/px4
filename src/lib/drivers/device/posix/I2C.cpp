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

#شامل "I2C.hpp"

#اگر defined(CONFIG_I2C)

#اگر_تعریف_شدہ __PX4_LINUX

#شامل <linux/i2c.h>
#شامل <linux/i2c-dev.h>

#شامل <px4_platform_common/i2c_spi_buses.h>

نامخلا device
{

I2C::I2C(uint8_t device_type, ثابت حرف *name, ثابت صحیح bus, ثابت uint16_t پتہ, ثابت uint32_t frequency) :
	CDev(name, خالی_اشاریہ)
{
	// fill in _device_id fields for a I2C device
	_device_id.devid_s.devtype = device_type;
	_device_id.devid_s.bus_type = DeviceBusType_I2C;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.پتہ = پتہ;
}

I2C::I2C(ثابت I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_اشاریہ, config.module_name, config.bus, config.i2c_پتہ, config.bus_frequency)
{
	سیٹ_external(config.external);
}

I2C::~I2C()
{
	اگر (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

صحیح
I2C::init()
{
	صحیح ret = PX4_ERROR;

	اگر (_fd >= 0) {
		واپس PX4_OK;
	}

	// Open the actual I2C device
	حرف dev_راستہ[16] {};
	snprintf(dev_راستہ, حجم(dev_راستہ), "/dev/i2c-%i", get_device_bus());
	_fd = ::open(dev_راستہ, O_RDWR);

	اگر (_fd < 0) {
		DEVICE_DEBUG("failed to init I2C");
		ret = -ENOENT;
		جاؤ برآمد;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	اگر (ret != OK) {
		DEVICE_DEBUG("probe failed");
		جاؤ برآمد;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	اگر (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		جاؤ برآمد;
	}

	// tell the world where we are
	DEVICE_DEBUG("on I2C bus %d at 0x%02x", get_device_bus(), get_device_پتہ());

برآمد:

	اگر ((ret != OK) && !(_fd < 0)) {
		::close(_fd);
		_fd = -1;
	}

	واپس ret;
}

صحیح
I2C::transfer(ثابت uint8_t *send, ثابت غیرعلامتی send_len, uint8_t *recv, ثابت غیرعلامتی recv_len)
{
	صحیح ret = PX4_ERROR;
	غیرعلامتی retry_گنتی = 0;

	اگر (_fd < 0) {
		PX4_ERR("I2C device not opened");
		واپس PX4_ERROR;
	}

	کرو {
		DEVICE_DEBUG("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

		غیرعلامتی msgs = 0;
		ساخت i2c_پیغام msgv[2] {};

		اگر (send_len > 0) {
			msgv[msgs].addr = get_device_پتہ();
			msgv[msgs].flags = 0;
			msgv[msgs].buf = const_cast<uint8_t *>(send);
			msgv[msgs].len = send_len;
			msgs++;
		}

		اگر (recv_len > 0) {
			msgv[msgs].addr = get_device_پتہ();
			msgv[msgs].flags = I2C_M_RD;
			msgv[msgs].buf = recv;
			msgv[msgs].len = recv_len;
			msgs++;
		}

		اگر (msgs == 0) {
			واپس -EINVAL;
		}

		i2c_rdwr_ioctl_ڈیٹا packets{};
		packets.msgs  = msgv;
		packets.nmsgs = msgs;

		صحیح ret_ioctl = ::ioctl(_fd, I2C_RDWR, (غیرعلامتی طویل)&packets);

		اگر (ret_ioctl == -1) {
			DEVICE_DEBUG("I2C transfer failed");
			ret = PX4_ERROR;

		} ورنہ {
			// success
			ret = PX4_OK;
			توڑو;
		}

	} جب (retry_گنتی++ < _retries);

	واپس ret;
}

} // namespace device

#اگر_ختم // __PX4_LINUX

#اگر_ختم // CONFIG_I2C
