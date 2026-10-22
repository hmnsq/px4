/****************************************************************************
 *
 *   Copyright (c) 2016-2020 PX4 Development Team. All rights reserved.
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

#అన్తర్భావ <px4_platform_common/time.h>
#అన్తర్భావ <px4_platform_common/px4_config.h>
#అన్తర్భావ <px4_platform_common/i2c_spi_buses.h>

బాహ్య "C" {
	__EXPORT శూన్య fc_uninitialize_i2c_bus(పూర్ణాంకం fd);
}

నామాకాశ device
{

I2C::_config_i2c_bus_func_t  I2C::_config_i2c_bus  = ఖాళీ;
I2C::సమితి_i2c_చిరునామా_func_t I2C::సమితి_i2c_చిరునామా = ఖాళీ;
I2C::_i2c_transfer_func_t    I2C::_i2c_transfer    = ఖాళీ;

// There is a mutex per I2C bus. A mutex isn't required with normal
// use since per bus I2C accesses are made via work item tied to a per bus thread.
// But it is here anyways in case someone decides to use the bus in some different
// custom code that has a separate thread.
నిర్మాణం I2C::_bus_mutex_t I2C::_bus_mutex[I2C::MAX_I2C_BUS] = {
	{1, PTHREAD_MUTEX_INITIALIZER},
	{2, PTHREAD_MUTEX_INITIALIZER},
	{4, PTHREAD_MUTEX_INITIALIZER},
	{5, PTHREAD_MUTEX_INITIALIZER}
};

I2C::I2C(uint8_t device_type, స్థిరరాశి అక్షరం *name, స్థిరరాశి పూర్ణాంకం bus, స్థిరరాశి uint16_t చిరునామా, స్థిరరాశి uint32_t frequency) :
	CDev(name, శూన్యసూచిక),
	_frequency(frequency / 1000)
{
	_device_id.devid = 0;

	// fill in _device_id fields for a I2C device
	_device_id.devid_s.devtype = device_type;
	_device_id.devid_s.bus_type = DeviceBusType_I2C;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.చిరునామా = చిరునామా;

	PX4_INFO("*** I2C Device ID 0x%x %d", _device_id.devid, _device_id.devid);
}

I2C::I2C(స్థిరరాశి I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_సూచీ, config.module_name, config.bus, config.i2c_చిరునామా, config.bus_frequency)
{
	సమితి_external(config.external);
}

I2C::~I2C()
{
}

పూర్ణాంకం
I2C::init()
{
	పూర్ణాంకం ret = PX4_ERROR;

	ఒకవేళ (_i2c_fd != -1) {
		తిరిగి PX4_OK;
	}

	ఒకవేళ (_config_i2c_bus == ఖాళీ) {
		PX4_ERR("NULL i2c init function");
		వెళ్ళు బహిర్గతం;
	}

	ఒకవేళ (_mutex == శూన్యసూచిక) {
		కొరకు (పూర్ణాంకం i = 0; i < MAX_I2C_BUS; i++) {
			ఒకవేళ (get_device_bus() == _bus_mutex[i]._bus) {
				_mutex = &_bus_mutex[i]._mutex;
				ఆపు;
			}
		}

		ఒకవేళ (_mutex == శూన్యసూచిక) {
			PX4_ERR("NULL i2c bus mutex");
			వెళ్ళు బహిర్గతం;

		} లేకపోతే {
			PX4_INFO("Set up I2C bus mutex for bus %d", get_device_bus());
		}
	}

	pthread_mutex_lock(_mutex);
	// Open the actual I2C device
	_i2c_fd = _config_i2c_bus(get_device_bus(), get_device_చిరునామా(), _frequency);
	pthread_mutex_unlock(_mutex);

	ఒకవేళ (_i2c_fd == -1) {
		PX4_ERR("i2c init failed");
		వెళ్ళు బహిర్గతం;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	ఒకవేళ (ret != OK) {
		PX4_ERR("i2c probe failed");
		fc_uninitialize_i2c_bus(_i2c_fd);
		_i2c_fd = -1;
		వెళ్ళు బహిర్గతం;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	ఒకవేళ (ret != OK) {
		PX4_ERR("i2c cdev init failed");
		వెళ్ళు బహిర్గతం;
	}

	// tell the world where we are
	// PX4_INFO("on I2C bus %d at 0x%02x", get_device_bus(), get_device_address());

బహిర్గతం:

	తిరిగి ret;
}

శూన్య
I2C::సమితి_device_చిరునామా(పూర్ణాంకం చిరునామా, తార్కిక సంవర్గమానం)
{
	ఒకవేళ ((_i2c_fd != PX4_ERROR) && (సమితి_i2c_చిరునామా != ఖాళీ)) {
		ఒకవేళ (సంవర్గమానం) {
			PX4_INFO("Set i2c address 0x%x, fd %d", చిరునామా, _i2c_fd);
		}

		pthread_mutex_lock(_mutex);
		సమితి_i2c_చిరునామా(_i2c_fd, చిరునామా);
		pthread_mutex_unlock(_mutex);

		Device::సమితి_device_చిరునామా(చిరునామా);
	}
}


పూర్ణాంకం
I2C::transfer(స్థిరరాశి uint8_t *send, స్థిరరాశి గుర్తులేని send_len, uint8_t *recv, స్థిరరాశి గుర్తులేని recv_len)
{
	పూర్ణాంకం ret = PX4_ERROR;
	గుర్తులేని retry_లెక్క = 1;

	ఒకవేళ ((_i2c_fd != PX4_ERROR) && (_i2c_transfer != ఖాళీ)) {
		చేయి {
			// PX4_INFO("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

			pthread_mutex_lock(_mutex);
			ret = _i2c_transfer(_i2c_fd, send, send_len, recv, recv_len);
			pthread_mutex_unlock(_mutex);

			ఒకవేళ (ret != PX4_ERROR) { ఆపు; }

			px4_usleep(1000);

		} అయితే (retry_లెక్క++ < _retries);
	}

	తిరిగి ret;
}

} // namespace device

#అయితే_ముగింపు
