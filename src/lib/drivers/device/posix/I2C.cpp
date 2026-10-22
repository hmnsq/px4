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

#अन्तर्भाव "I2C.hpp"

#यदि defined(CONFIG_I2C)

#ifdef __PX4_LINUX

#अन्तर्भाव <linux/i2c.h>
#अन्तर्भाव <linux/i2c-dev.h>

#अन्तर्भाव <px4_platform_common/i2c_spi_buses.h>

नामाकाश device
{

I2C::I2C(uint8_t device_type, ध्रुव वर्ण *name, ध्रुव पूर्णाङ्क bus, ध्रुव uint16_t address, ध्रुव uint32_t frequency) :
	CDev(name, nullptr)
{
	// fill in _device_id fields for a I2C device
	_device_id.devid_s.devtype = device_type;
	_device_id.devid_s.bus_type = DeviceBusType_I2C;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.address = address;
}

I2C::I2C(ध्रुव I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_index, config.module_name, config.bus, config.i2c_address, config.bus_frequency)
{
	set_external(config.external);
}

I2C::~I2C()
{
	यदि (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

पूर्णाङ्क
I2C::init()
{
	पूर्णाङ्क ret = PX4_ERROR;

	यदि (_fd >= 0) {
		प्रत्यागम PX4_OK;
	}

	// Open the actual I2C device
	वर्ण dev_path[16] {};
	snprintf(dev_path, परिमाण(dev_path), "/dev/i2c-%i", get_device_bus());
	_fd = ::open(dev_path, O_RDWR);

	यदि (_fd < 0) {
		DEVICE_DEBUG("failed to init I2C");
		ret = -ENOENT;
		गच्छ out;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	यदि (ret != OK) {
		DEVICE_DEBUG("probe failed");
		गच्छ out;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	यदि (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		गच्छ out;
	}

	// tell the world where we are
	DEVICE_DEBUG("on I2C bus %d at 0x%02x", get_device_bus(), get_device_address());

out:

	यदि ((ret != OK) && !(_fd < 0)) {
		::close(_fd);
		_fd = -1;
	}

	प्रत्यागम ret;
}

पूर्णाङ्क
I2C::transfer(ध्रुव uint8_t *send, ध्रुव unsigned send_len, uint8_t *recv, ध्रुव unsigned recv_len)
{
	पूर्णाङ्क ret = PX4_ERROR;
	unsigned retry_गिनती = 0;

	यदि (_fd < 0) {
		PX4_ERR("I2C device not opened");
		प्रत्यागम PX4_ERROR;
	}

	कुरु {
		DEVICE_DEBUG("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

		unsigned msgs = 0;
		संरचना i2c_संदेश msgv[2] {};

		यदि (send_len > 0) {
			msgv[msgs].addr = get_device_address();
			msgv[msgs].flags = 0;
			msgv[msgs].buf = const_cast<uint8_t *>(send);
			msgv[msgs].len = send_len;
			msgs++;
		}

		यदि (recv_len > 0) {
			msgv[msgs].addr = get_device_address();
			msgv[msgs].flags = I2C_M_RD;
			msgv[msgs].buf = recv;
			msgv[msgs].len = recv_len;
			msgs++;
		}

		यदि (msgs == 0) {
			प्रत्यागम -EINVAL;
		}

		i2c_rdwr_ioctl_data packets{};
		packets.msgs  = msgv;
		packets.nmsgs = msgs;

		पूर्णाङ्क ret_ioctl = ::ioctl(_fd, I2C_RDWR, (unsigned long)&packets);

		यदि (ret_ioctl == -1) {
			DEVICE_DEBUG("I2C transfer failed");
			ret = PX4_ERROR;

		} अन्यथा {
			// success
			ret = PX4_OK;
			भङ्ग;
		}

	} यावत् (retry_गिनती++ < _retries);

	प्रत्यागम ret;
}

} // namespace device

#endif // __PX4_LINUX

#endif // CONFIG_I2C
