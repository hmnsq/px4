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

#ಅನ್ತರ್ಭಾವ "I2C.hpp"

#ಆದರೆ defined(CONFIG_I2C)

#ಅನ್ತರ್ಭಾವ <px4_platform_common/time.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/px4_config.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/i2c_spi_buses.h>

ಬಾಹ್ಯ "C" {
	__EXPORT ಶೂನ್ಯ fc_uninitialize_i2c_bus(ಪೂರ್ಣಾಂಕ fd);
}

ನಾಮಾಕಾಶ device
{

I2C::_config_i2c_bus_func_t  I2C::_config_i2c_bus  = ಖಾಲಿ;
I2C::ಗಣ_i2c_ವಿಳಾಸ_func_t I2C::ಗಣ_i2c_ವಿಳಾಸ = ಖಾಲಿ;
I2C::_i2c_transfer_func_t    I2C::_i2c_transfer    = ಖಾಲಿ;

// There is a mutex per I2C bus. A mutex isn't required with normal
// use since per bus I2C accesses are made via work item tied to a per bus thread.
// But it is here anyways in case someone decides to use the bus in some different
// custom code that has a separate thread.
ರಚನೆ I2C::_bus_mutex_t I2C::_bus_mutex[I2C::MAX_I2C_BUS] = {
	{1, PTHREAD_MUTEX_INITIALIZER},
	{2, PTHREAD_MUTEX_INITIALIZER},
	{4, PTHREAD_MUTEX_INITIALIZER},
	{5, PTHREAD_MUTEX_INITIALIZER}
};

I2C::I2C(uint8_t device_type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ bus, ಸ್ಥಿರಾಂಕ uint16_t ವಿಳಾಸ, ಸ್ಥಿರಾಂಕ uint32_t frequency) :
	CDev(name, ಶೂನ್ಯಸೂಚಕ),
	_frequency(frequency / 1000)
{
	_device_id.devid = 0;

	// fill in _device_id fields for a I2C device
	_device_id.devid_s.devtype = device_type;
	_device_id.devid_s.bus_type = DeviceBusType_I2C;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.ವಿಳಾಸ = ವಿಳಾಸ;

	PX4_INFO("*** I2C Device ID 0x%x %d", _device_id.devid, _device_id.devid);
}

I2C::I2C(ಸ್ಥಿರಾಂಕ I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_ಸೂಚ್ಯಂಕ, config.module_name, config.bus, config.i2c_ವಿಳಾಸ, config.bus_frequency)
{
	ಗಣ_external(config.external);
}

I2C::~I2C()
{
}

ಪೂರ್ಣಾಂಕ
I2C::init()
{
	ಪೂರ್ಣಾಂಕ ret = PX4_ERROR;

	ಆದರೆ (_i2c_fd != -1) {
		ಹಿಂತಿರುಗಿ PX4_OK;
	}

	ಆದರೆ (_config_i2c_bus == ಖಾಲಿ) {
		PX4_ERR("NULL i2c init function");
		ಹೋಗು ಹೊರಹೋಗು;
	}

	ಆದರೆ (_mutex == ಶೂನ್ಯಸೂಚಕ) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < MAX_I2C_BUS; i++) {
			ಆದರೆ (get_device_bus() == _bus_mutex[i]._bus) {
				_mutex = &_bus_mutex[i]._mutex;
				ನಿಲ್ಲಿಸು;
			}
		}

		ಆದರೆ (_mutex == ಶೂನ್ಯಸೂಚಕ) {
			PX4_ERR("NULL i2c bus mutex");
			ಹೋಗು ಹೊರಹೋಗು;

		} ಇಲ್ಲದಿದ್ದರೆ {
			PX4_INFO("Set up I2C bus mutex for bus %d", get_device_bus());
		}
	}

	pthread_mutex_lock(_mutex);
	// Open the actual I2C device
	_i2c_fd = _config_i2c_bus(get_device_bus(), get_device_ವಿಳಾಸ(), _frequency);
	pthread_mutex_unlock(_mutex);

	ಆದರೆ (_i2c_fd == -1) {
		PX4_ERR("i2c init failed");
		ಹೋಗು ಹೊರಹೋಗು;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	ಆದರೆ (ret != OK) {
		PX4_ERR("i2c probe failed");
		fc_uninitialize_i2c_bus(_i2c_fd);
		_i2c_fd = -1;
		ಹೋಗು ಹೊರಹೋಗು;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	ಆದರೆ (ret != OK) {
		PX4_ERR("i2c cdev init failed");
		ಹೋಗು ಹೊರಹೋಗು;
	}

	// tell the world where we are
	// PX4_INFO("on I2C bus %d at 0x%02x", get_device_bus(), get_device_address());

ಹೊರಹೋಗು:

	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ
I2C::ಗಣ_device_ವಿಳಾಸ(ಪೂರ್ಣಾಂಕ ವಿಳಾಸ, ತಾರ್ಕಿಕ ಲಘುಗಣಕ)
{
	ಆದರೆ ((_i2c_fd != PX4_ERROR) && (ಗಣ_i2c_ವಿಳಾಸ != ಖಾಲಿ)) {
		ಆದರೆ (ಲಘುಗಣಕ) {
			PX4_INFO("Set i2c address 0x%x, fd %d", ವಿಳಾಸ, _i2c_fd);
		}

		pthread_mutex_lock(_mutex);
		ಗಣ_i2c_ವಿಳಾಸ(_i2c_fd, ವಿಳಾಸ);
		pthread_mutex_unlock(_mutex);

		Device::ಗಣ_device_ವಿಳಾಸ(ವಿಳಾಸ);
	}
}


ಪೂರ್ಣಾಂಕ
I2C::transfer(ಸ್ಥಿರಾಂಕ uint8_t *send, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ send_len, uint8_t *recv, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ recv_len)
{
	ಪೂರ್ಣಾಂಕ ret = PX4_ERROR;
	ಚಿಹ್ನೆರಹಿತ retry_ಎಣಿಕೆ = 1;

	ಆದರೆ ((_i2c_fd != PX4_ERROR) && (_i2c_transfer != ಖಾಲಿ)) {
		ಮಾಡು {
			// PX4_INFO("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

			pthread_mutex_lock(_mutex);
			ret = _i2c_transfer(_i2c_fd, send, send_len, recv, recv_len);
			pthread_mutex_unlock(_mutex);

			ಆದರೆ (ret != PX4_ERROR) { ನಿಲ್ಲಿಸು; }

			px4_usleep(1000);

		} ಆಗ (retry_ಎಣಿಕೆ++ < _retries);
	}

	ಹಿಂತಿರುಗಿ ret;
}

} // namespace device

#ಆದರೆ_ಅಂತ್ಯ
