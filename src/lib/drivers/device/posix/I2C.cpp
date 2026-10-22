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

#ਅਨ੍ਤਰ੍ਭਾਵ "I2C.hpp"

#ਜੇ defined(CONFIG_I2C)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_LINUX

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/i2c.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/i2c-dev.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/i2c_spi_buses.h>

ਨਾਮਾਕਾਸ਼ device
{

I2C::I2C(uint8_t device_type, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਪੂਰਨ bus, ਅਚਲ uint16_t ਪਤਾ, ਅਚਲ uint32_t frequency) :
	CDev(name, ਖਾਲੀ_ਸੂਚਕ)
{
	// fill in _device_id fields for a I2C device
	_device_id.devid_s.devtype = device_type;
	_device_id.devid_s.bus_type = DeviceBusType_I2C;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.ਪਤਾ = ਪਤਾ;
}

I2C::I2C(ਅਚਲ I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_ਸੂਚਕਾਂਕ, config.module_name, config.bus, config.i2c_ਪਤਾ, config.bus_frequency)
{
	ਸੈੱਟ_external(config.external);
}

I2C::~I2C()
{
	ਜੇ (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

ਪੂਰਨ
I2C::init()
{
	ਪੂਰਨ ret = PX4_ERROR;

	ਜੇ (_fd >= 0) {
		ਵਾਪਸ PX4_OK;
	}

	// Open the actual I2C device
	ਅੱਖਰ dev_ਰਸਤਾ[16] {};
	snprintf(dev_ਰਸਤਾ, ਆਕਾਰ(dev_ਰਸਤਾ), "/dev/i2c-%i", get_device_bus());
	_fd = ::open(dev_ਰਸਤਾ, O_RDWR);

	ਜੇ (_fd < 0) {
		DEVICE_DEBUG("failed to init I2C");
		ret = -ENOENT;
		ਜਾਓ ਬਾਹਰਲਾ;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	ਜੇ (ret != OK) {
		DEVICE_DEBUG("probe failed");
		ਜਾਓ ਬਾਹਰਲਾ;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	ਜੇ (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		ਜਾਓ ਬਾਹਰਲਾ;
	}

	// tell the world where we are
	DEVICE_DEBUG("on I2C bus %d at 0x%02x", get_device_bus(), get_device_ਪਤਾ());

ਬਾਹਰਲਾ:

	ਜੇ ((ret != OK) && !(_fd < 0)) {
		::close(_fd);
		_fd = -1;
	}

	ਵਾਪਸ ret;
}

ਪੂਰਨ
I2C::transfer(ਅਚਲ uint8_t *send, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ send_len, uint8_t *recv, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ recv_len)
{
	ਪੂਰਨ ret = PX4_ERROR;
	ਬਿਨਾਨਿਸ਼ਾਨੀ retry_ਗਿਣਤੀ = 0;

	ਜੇ (_fd < 0) {
		PX4_ERR("I2C device not opened");
		ਵਾਪਸ PX4_ERROR;
	}

	ਕਰੋ {
		DEVICE_DEBUG("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

		ਬਿਨਾਨਿਸ਼ਾਨੀ msgs = 0;
		ਬਣਤਰ i2c_ਸੁਨੇਹਾ msgv[2] {};

		ਜੇ (send_len > 0) {
			msgv[msgs].addr = get_device_ਪਤਾ();
			msgv[msgs].flags = 0;
			msgv[msgs].buf = const_cast<uint8_t *>(send);
			msgv[msgs].len = send_len;
			msgs++;
		}

		ਜੇ (recv_len > 0) {
			msgv[msgs].addr = get_device_ਪਤਾ();
			msgv[msgs].flags = I2C_M_RD;
			msgv[msgs].buf = recv;
			msgv[msgs].len = recv_len;
			msgs++;
		}

		ਜੇ (msgs == 0) {
			ਵਾਪਸ -EINVAL;
		}

		i2c_rdwr_ioctl_ਅੰਕੜੇ packets{};
		packets.msgs  = msgv;
		packets.nmsgs = msgs;

		ਪੂਰਨ ret_ioctl = ::ioctl(_fd, I2C_RDWR, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)&packets);

		ਜੇ (ret_ioctl == -1) {
			DEVICE_DEBUG("I2C transfer failed");
			ret = PX4_ERROR;

		} ਨਹੀਂਤਾਂ {
			// success
			ret = PX4_OK;
			ਰੋਕੋ;
		}

	} ਜਦੋਂ (retry_ਗਿਣਤੀ++ < _retries);

	ਵਾਪਸ ret;
}

} // namespace device

#ਜੇ_ਅੰਤ // __PX4_LINUX

#ਜੇ_ਅੰਤ // CONFIG_I2C
