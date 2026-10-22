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

#অন্তর্ভাব "I2C.hpp"

#যদি defined(CONFIG_I2C)

#অন্তর্ভাব <px4_platform_common/time.h>
#অন্তর্ভাব <px4_platform_common/px4_config.h>
#অন্তর্ভাব <px4_platform_common/i2c_spi_buses.h>

বাহ্যিক "C" {
	__EXPORT শূন্য fc_uninitialize_i2c_bus(পূর্ণসংখ্যা fd);
}

নামাকাশ device
{

I2C::_config_i2c_bus_func_t  I2C::_config_i2c_bus  = ফাঁকা;
I2C::_set_i2c_address_func_t I2C::_set_i2c_address = ফাঁকা;
I2C::_i2c_transfer_func_t    I2C::_i2c_transfer    = ফাঁকা;

// There is a mutex per I2C bus. A mutex isn't required with normal
// use since per bus I2C accesses are made via work item tied to a per bus thread.
// But it is here anyways in case someone decides to use the bus in some different
// custom code that has a separate thread.
কাঠামো I2C::_bus_mutex_t I2C::_bus_mutex[I2C::MAX_I2C_BUS] = {
	{1, PTHREAD_MUTEX_INITIALIZER},
	{2, PTHREAD_MUTEX_INITIALIZER},
	{4, PTHREAD_MUTEX_INITIALIZER},
	{5, PTHREAD_MUTEX_INITIALIZER}
};

I2C::I2C(uint8_t device_type, ধ্রুবক অক্ষর *name, ধ্রুবক পূর্ণসংখ্যা bus, ধ্রুবক uint16_t address, ধ্রুবক uint32_t frequency) :
	CDev(name, nullptr),
	_frequency(frequency / 1000)
{
	_device_id.devid = 0;

	// fill in _device_id fields for a I2C device
	_device_id.devid_s.devtype = device_type;
	_device_id.devid_s.bus_type = DeviceBusType_I2C;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.address = address;

	PX4_INFO("*** I2C Device ID 0x%x %d", _device_id.devid, _device_id.devid);
}

I2C::I2C(ধ্রুবক I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_সূচক, config.module_name, config.bus, config.i2c_address, config.bus_frequency)
{
	set_external(config.external);
}

I2C::~I2C()
{
}

পূর্ণসংখ্যা
I2C::init()
{
	পূর্ণসংখ্যা ret = PX4_ERROR;

	যদি (_i2c_fd != -1) {
		ফেরত PX4_OK;
	}

	যদি (_config_i2c_bus == ফাঁকা) {
		PX4_ERR("NULL i2c init function");
		যাও out;
	}

	যদি (_mutex == nullptr) {
		জন্য (পূর্ণসংখ্যা i = 0; i < MAX_I2C_BUS; i++) {
			যদি (get_device_bus() == _bus_mutex[i]._bus) {
				_mutex = &_bus_mutex[i]._mutex;
				বিরতি;
			}
		}

		যদি (_mutex == nullptr) {
			PX4_ERR("NULL i2c bus mutex");
			যাও out;

		} অন্যথা {
			PX4_INFO("Set up I2C bus mutex for bus %d", get_device_bus());
		}
	}

	pthread_mutex_lock(_mutex);
	// Open the actual I2C device
	_i2c_fd = _config_i2c_bus(get_device_bus(), get_device_address(), _frequency);
	pthread_mutex_unlock(_mutex);

	যদি (_i2c_fd == -1) {
		PX4_ERR("i2c init failed");
		যাও out;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	যদি (ret != OK) {
		PX4_ERR("i2c probe failed");
		fc_uninitialize_i2c_bus(_i2c_fd);
		_i2c_fd = -1;
		যাও out;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	যদি (ret != OK) {
		PX4_ERR("i2c cdev init failed");
		যাও out;
	}

	// tell the world where we are
	// PX4_INFO("on I2C bus %d at 0x%02x", get_device_bus(), get_device_address());

out:

	ফেরত ret;
}

শূন্য
I2C::set_device_address(পূর্ণসংখ্যা address, যৌক্তিক log)
{
	যদি ((_i2c_fd != PX4_ERROR) && (_set_i2c_address != ফাঁকা)) {
		যদি (log) {
			PX4_INFO("Set i2c address 0x%x, fd %d", address, _i2c_fd);
		}

		pthread_mutex_lock(_mutex);
		_set_i2c_address(_i2c_fd, address);
		pthread_mutex_unlock(_mutex);

		Device::set_device_address(address);
	}
}


পূর্ণসংখ্যা
I2C::transfer(ধ্রুবক uint8_t *send, ধ্রুবক unsigned send_len, uint8_t *recv, ধ্রুবক unsigned recv_len)
{
	পূর্ণসংখ্যা ret = PX4_ERROR;
	unsigned retry_গুনতি = 1;

	যদি ((_i2c_fd != PX4_ERROR) && (_i2c_transfer != ফাঁকা)) {
		করো {
			// PX4_INFO("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

			pthread_mutex_lock(_mutex);
			ret = _i2c_transfer(_i2c_fd, send, send_len, recv, recv_len);
			pthread_mutex_unlock(_mutex);

			যদি (ret != PX4_ERROR) { বিরতি; }

			px4_usleep(1000);

		} যখন (retry_গুনতি++ < _retries);
	}

	ফেরত ret;
}

} // namespace device

#endif
