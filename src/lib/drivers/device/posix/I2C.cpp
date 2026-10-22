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

#అన్తర్భావ "I2C.hpp"

#ఒకవేళ defined(CONFIG_I2C)

#నిర్వచితమైతే __PX4_LINUX

#అన్తర్భావ <linux/i2c.h>
#అన్తర్భావ <linux/i2c-dev.h>

#అన్తర్భావ <px4_platform_common/i2c_spi_buses.h>

నామాకాశ device
{

I2C::I2C(uint8_t device_type, స్థిరరాశి అక్షరం *name, స్థిరరాశి పూర్ణాంకం bus, స్థిరరాశి uint16_t చిరునామా, స్థిరరాశి uint32_t frequency) :
	CDev(name, శూన్యసూచిక)
{
	// fill in _device_id fields for a I2C device
	_device_id.devid_s.devtype = device_type;
	_device_id.devid_s.bus_type = DeviceBusType_I2C;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.చిరునామా = చిరునామా;
}

I2C::I2C(స్థిరరాశి I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_సూచీ, config.module_name, config.bus, config.i2c_చిరునామా, config.bus_frequency)
{
	సమితి_external(config.external);
}

I2C::~I2C()
{
	ఒకవేళ (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

పూర్ణాంకం
I2C::init()
{
	పూర్ణాంకం ret = PX4_ERROR;

	ఒకవేళ (_fd >= 0) {
		తిరిగి PX4_OK;
	}

	// Open the actual I2C device
	అక్షరం dev_మార్గం[16] {};
	snprintf(dev_మార్గం, పరిమాణం(dev_మార్గం), "/dev/i2c-%i", get_device_bus());
	_fd = ::open(dev_మార్గం, O_RDWR);

	ఒకవేళ (_fd < 0) {
		DEVICE_DEBUG("failed to init I2C");
		ret = -ENOENT;
		వెళ్ళు బహిర్గతం;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	ఒకవేళ (ret != OK) {
		DEVICE_DEBUG("probe failed");
		వెళ్ళు బహిర్గతం;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	ఒకవేళ (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		వెళ్ళు బహిర్గతం;
	}

	// tell the world where we are
	DEVICE_DEBUG("on I2C bus %d at 0x%02x", get_device_bus(), get_device_చిరునామా());

బహిర్గతం:

	ఒకవేళ ((ret != OK) && !(_fd < 0)) {
		::close(_fd);
		_fd = -1;
	}

	తిరిగి ret;
}

పూర్ణాంకం
I2C::transfer(స్థిరరాశి uint8_t *send, స్థిరరాశి గుర్తులేని send_len, uint8_t *recv, స్థిరరాశి గుర్తులేని recv_len)
{
	పూర్ణాంకం ret = PX4_ERROR;
	గుర్తులేని retry_లెక్క = 0;

	ఒకవేళ (_fd < 0) {
		PX4_ERR("I2C device not opened");
		తిరిగి PX4_ERROR;
	}

	చేయి {
		DEVICE_DEBUG("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

		గుర్తులేని msgs = 0;
		నిర్మాణం i2c_సందేశం msgv[2] {};

		ఒకవేళ (send_len > 0) {
			msgv[msgs].addr = get_device_చిరునామా();
			msgv[msgs].flags = 0;
			msgv[msgs].buf = const_cast<uint8_t *>(send);
			msgv[msgs].len = send_len;
			msgs++;
		}

		ఒకవేళ (recv_len > 0) {
			msgv[msgs].addr = get_device_చిరునామా();
			msgv[msgs].flags = I2C_M_RD;
			msgv[msgs].buf = recv;
			msgv[msgs].len = recv_len;
			msgs++;
		}

		ఒకవేళ (msgs == 0) {
			తిరిగి -EINVAL;
		}

		i2c_rdwr_ioctl_దత్తాంశం packets{};
		packets.msgs  = msgv;
		packets.nmsgs = msgs;

		పూర్ణాంకం ret_ioctl = ::ioctl(_fd, I2C_RDWR, (గుర్తులేని దీర్ఘ)&packets);

		ఒకవేళ (ret_ioctl == -1) {
			DEVICE_DEBUG("I2C transfer failed");
			ret = PX4_ERROR;

		} లేకపోతే {
			// success
			ret = PX4_OK;
			ఆపు;
		}

	} అయితే (retry_లెక్క++ < _retries);

	తిరిగి ret;
}

} // namespace device

#అయితే_ముగింపు // __PX4_LINUX

#అయితే_ముగింపు // CONFIG_I2C
