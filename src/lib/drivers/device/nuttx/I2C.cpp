/****************************************************************************
 *
 *   Copyright (c) 2012-2021 PX4 Development Team. All rights reserved.
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

#അന്തര്ഭാവ "I2C.hpp"

#എങ്കിൽ defined(CONFIG_I2C)

#അന്തര്ഭാവ <px4_platform_common/i2c_spi_buses.h>
#അന്തര്ഭാവ <nuttx/i2c/i2c_master.h>

നാമാകാശ device
{
/*
 *  N.B. By defaulting the value of _bus_clocks to non Zero
 *  All calls to init() will NOT set the buss frequency
 */

unsigned പൂർണ്ണസംഖ്യ I2C::_bus_clocks[PX4_സംഖ്യ_I2_C_BUSES] = PX4_I2C_BUS_CLOCK_INIT;

I2C::I2C(uint8_t device_type, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം പൂർണ്ണസംഖ്യ bus, സ്ഥിരം uint16_t address, സ്ഥിരം uint32_t frequency) :
	CDev(name, nullptr),
	_frequency(frequency)
{
	// fill in _device_id fields for a I2C device
	_device_id.devid_s.devtype = device_type;
	_device_id.devid_s.bus_type = DeviceBusType_I2C;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.address = address;
}

I2C::I2C(സ്ഥിരം I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_index, config.module_name, config.bus, config.i2c_address, config.bus_frequency)
{
	set_external(config.external);
}

I2C::~I2C()
{
	എങ്കിൽ (_dev) {
		px4_i2cbus_uninitialize(_dev);
		_dev = nullptr;
	}
}

പൂർണ്ണസംഖ്യ
I2C::set_bus_clock(unsigned bus, unsigned clock_hz)
{
	പൂർണ്ണസംഖ്യ index = bus - 1;

	എങ്കിൽ (index < 0 || index >= static_cast<പൂർണ്ണസംഖ്യ>(വലുപ്പം(_bus_clocks) / വലുപ്പം(_bus_clocks[0]))) {
		തിരികെ -EINVAL;
	}

	എങ്കിൽ (_bus_clocks[index] > 0) {
		// DEVICE_DEBUG("overriding clock of %u with %u Hz\n", _bus_clocks[index], clock_hz);
	}

	_bus_clocks[index] = clock_hz;

	തിരികെ OK;
}

പൂർണ്ണസംഖ്യ
I2C::init()
{
	പൂർണ്ണസംഖ്യ ret = PX4_ERROR;
	unsigned bus_index;

	എങ്കിൽ (_dev != nullptr) {
		തിരികെ PX4_OK;
	}

	// attach to the i2c bus
	_dev = px4_i2cbus_initialize(get_device_bus());

	എങ്കിൽ (_dev == nullptr) {
		DEVICE_DEBUG("failed to init I2C");
		ret = -ENOENT;
		പോകുക out;
	}

	// the above call fails for a non-existing bus index,
	// so the index math here is safe.
	bus_index = get_device_bus() - 1;

	// abort if the max frequency we allow (the frequency we ask)
	// is smaller than the bus frequency
	എങ്കിൽ (_bus_clocks[bus_index] > _frequency) {
		(ശൂന്യം)px4_i2cbus_uninitialize(_dev);
		_dev = nullptr;
		DEVICE_LOG("FAIL: too slow for bus #%u: %u KHz, device max: %" PRIu32 " KHz)",
			   get_device_bus(), _bus_clocks[bus_index] / 1000, _frequency / 1000);
		ret = -EINVAL;
		പോകുക out;
	}

	// set frequency for this instance once to the bus speed
	// the bus speed is the maximum supported by all devices on the bus,
	// as we have to prioritize performance over compatibility.
	// If a new device requires a lower clock speed, this has to be
	// manually set via "fmu i2c <bus> <clock>" before starting any
	// drivers.
	// This is necessary as automatically lowering the bus speed
	// for maximum compatibility could induce timing issues on
	// critical sensors the adopter might be unaware of.

	// set the bus frequency on the first access if it has
	// not been set yet
	എങ്കിൽ (_bus_clocks[bus_index] == 0) {
		_bus_clocks[bus_index] = _frequency;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	എങ്കിൽ (ret != OK) {
		DEVICE_DEBUG("probe failed");
		പോകുക out;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	എങ്കിൽ (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		പോകുക out;
	}

	// tell the world where we are
	DEVICE_DEBUG("on I2C bus %d at 0x%02x (bus: %u KHz, max: %" PRIu32 " KHz)",
		     get_device_bus(), get_device_address(), _bus_clocks[bus_index] / 1000, _frequency / 1000);

out:

	എങ്കിൽ ((ret != OK) && (_dev != nullptr)) {
		px4_i2cbus_uninitialize(_dev);
		_dev = nullptr;
	}

	തിരികെ ret;
}

പൂർണ്ണസംഖ്യ
I2C::transfer(സ്ഥിരം uint8_t *send, സ്ഥിരം unsigned send_len, uint8_t *recv, സ്ഥിരം unsigned recv_len)
{
	പൂർണ്ണസംഖ്യ ret = PX4_ERROR;
	unsigned retry_എണ്ണം = 0;

	എങ്കിൽ (_dev == nullptr) {
		PX4_ERR("I2C device not opened");
		തിരികെ PX4_ERROR;
	}

	ചെയ്യുക {
		DEVICE_DEBUG("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

		i2c_സന്ദേശം_s msgv[2] {};
		unsigned msgs = 0;

		എങ്കിൽ (send_len > 0) {
			msgv[msgs].frequency = _bus_clocks[get_device_bus() - 1];
			msgv[msgs].addr = get_device_address();
			msgv[msgs].flags = 0;
			msgv[msgs].buffer = const_cast<uint8_t *>(send);
			msgv[msgs].length = send_len;
			msgs++;
		}

		എങ്കിൽ (recv_len > 0) {
			msgv[msgs].frequency = _bus_clocks[get_device_bus() - 1];
			msgv[msgs].addr = get_device_address();
			msgv[msgs].flags = I2C_M_READ;
			msgv[msgs].buffer = recv;
			msgv[msgs].length = recv_len;
			msgs++;
		}

		എങ്കിൽ (msgs == 0) {
			തിരികെ -EINVAL;
		}

		പൂർണ്ണസംഖ്യ ret_transfer = I2C_TRANSFER(_dev, &msgv[0], msgs);

		എങ്കിൽ (ret_transfer != 0) {
			DEVICE_DEBUG("I2C transfer failed, result %d", ret_transfer);
			ret = PX4_ERROR;

		} അല്ലെങ്കിൽ {
			// success
			ret = PX4_OK;
			നിർത്തുക;
		}

		// if we have already retried once, and we aren't going to give up, then reset the bus
		എങ്കിൽ ((_retries > 0) && (retry_എണ്ണം < _retries)) {
#എങ്കിൽ defined(CONFIG_I2C_RESET)
			DEVICE_DEBUG("I2C bus: %d, Addr: %X, I2C_RESET %d/%d",
				     get_device_bus(), get_device_address(), retry_എണ്ണം + 1, _retries);
			I2C_RESET(_dev);
#endif // CONFIG_I2C_RESET
		}

	} ആകുമ്പോൾ (retry_എണ്ണം++ < _retries);

	തിരികെ ret;
}

} // namespace device

#endif // CONFIG_I2C
