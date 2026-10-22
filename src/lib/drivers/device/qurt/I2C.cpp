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

#ضمن "I2C.hpp"

#إذا defined(CONFIG_I2C)

#ضمن <px4_platform_common/time.h>
#ضمن <px4_platform_common/px4_config.h>
#ضمن <px4_platform_common/i2c_spi_buses.h>

خارجي "C" {
	__EXPORT فراغ fc_uninitialize_i2c_bus(صحيح fd);
}

نطاق device
{

I2C::_config_i2c_bus_func_t  I2C::_config_i2c_bus  = عدم;
I2C::_set_i2c_address_func_t I2C::_set_i2c_address = عدم;
I2C::_i2c_transfer_func_t    I2C::_i2c_transfer    = عدم;

// There is a mutex per I2C bus. A mutex isn't required with normal
// use since per bus I2C accesses are made via work item tied to a per bus thread.
// But it is here anyways in case someone decides to use the bus in some different
// custom code that has a separate thread.
بنية I2C::_bus_mutex_t I2C::_bus_mutex[I2C::MAX_I2C_BUS] = {
	{1, PTHREAD_MUTEX_INITIALIZER},
	{2, PTHREAD_MUTEX_INITIALIZER},
	{4, PTHREAD_MUTEX_INITIALIZER},
	{5, PTHREAD_MUTEX_INITIALIZER}
};

I2C::I2C(uint8_t device_type, ثابت حرف *name, ثابت صحيح bus, ثابت uint16_t address, ثابت uint32_t frequency) :
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

I2C::I2C(ثابت I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_index, config.module_name, config.bus, config.i2c_address, config.bus_frequency)
{
	set_external(config.external);
}

I2C::~I2C()
{
}

صحيح
I2C::init()
{
	صحيح ret = PX4_ERROR;

	إذا (_i2c_fd != -1) {
		أرجع PX4_OK;
	}

	إذا (_config_i2c_bus == عدم) {
		PX4_ERR("NULL i2c init function");
		اذهب out;
	}

	إذا (_mutex == nullptr) {
		لكل (صحيح i = 0; i < MAX_I2C_BUS; i++) {
			إذا (get_device_bus() == _bus_mutex[i]._bus) {
				_mutex = &_bus_mutex[i]._mutex;
				اكسر;
			}
		}

		إذا (_mutex == nullptr) {
			PX4_ERR("NULL i2c bus mutex");
			اذهب out;

		} وإلا {
			PX4_INFO("Set up I2C bus mutex for bus %d", get_device_bus());
		}
	}

	pthread_mutex_lock(_mutex);
	// Open the actual I2C device
	_i2c_fd = _config_i2c_bus(get_device_bus(), get_device_address(), _frequency);
	pthread_mutex_unlock(_mutex);

	إذا (_i2c_fd == -1) {
		PX4_ERR("i2c init failed");
		اذهب out;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	إذا (ret != OK) {
		PX4_ERR("i2c probe failed");
		fc_uninitialize_i2c_bus(_i2c_fd);
		_i2c_fd = -1;
		اذهب out;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	إذا (ret != OK) {
		PX4_ERR("i2c cdev init failed");
		اذهب out;
	}

	// tell the world where we are
	// PX4_INFO("on I2C bus %d at 0x%02x", get_device_bus(), get_device_address());

out:

	أرجع ret;
}

فراغ
I2C::set_device_address(صحيح address, منطقي log)
{
	إذا ((_i2c_fd != PX4_ERROR) && (_set_i2c_address != عدم)) {
		إذا (log) {
			PX4_INFO("Set i2c address 0x%x, fd %d", address, _i2c_fd);
		}

		pthread_mutex_lock(_mutex);
		_set_i2c_address(_i2c_fd, address);
		pthread_mutex_unlock(_mutex);

		Device::set_device_address(address);
	}
}


صحيح
I2C::transfer(ثابت uint8_t *send, ثابت unsigned send_len, uint8_t *recv, ثابت unsigned recv_len)
{
	صحيح ret = PX4_ERROR;
	unsigned retry_كدي = 1;

	إذا ((_i2c_fd != PX4_ERROR) && (_i2c_transfer != عدم)) {
		افعل {
			// PX4_INFO("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

			pthread_mutex_lock(_mutex);
			ret = _i2c_transfer(_i2c_fd, send, send_len, recv, recv_len);
			pthread_mutex_unlock(_mutex);

			إذا (ret != PX4_ERROR) { اكسر; }

			px4_usleep(1000);

		} طالما (retry_كدي++ < _retries);
	}

	أرجع ret;
}

} // namespace device

#endif
