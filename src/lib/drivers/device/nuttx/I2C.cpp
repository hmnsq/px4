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

#অন্তর্ভাৱ "I2C.hpp"

#যদি defined(CONFIG_I2C)

#অন্তর্ভাৱ <px4_platform_common/i2c_spi_buses.h>
#অন্তর্ভাৱ <nuttx/i2c/i2c_master.h>

নামাকাশ device
{
/*
 *  N.B. By defaulting the value of _bus_clocks to non Zero
 *  All calls to init() will NOT set the buss frequency
 */

unsigned পূর্ণাঙ্ক I2C::_bus_clocks[PX4_সংখ্যা_I2_C_BUSES] = PX4_I2C_BUS_CLOCK_INIT;

I2C::I2C(uint8_t device_type, ধ্রুৱ ৱর্ণ *name, ধ্রুৱ পূর্ণাঙ্ক bus, ধ্রুৱ uint16_t address, ধ্রুৱ uint32_t frequency) :
	CDev(name, nullptr),
	_frequency(frequency)
{
	// fill in _device_id fields for a I2C device
	_device_id.devid_s.devtype = device_type;
	_device_id.devid_s.bus_type = DeviceBusType_I2C;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.address = address;
}

I2C::I2C(ধ্রুৱ I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_index, config.module_name, config.bus, config.i2c_address, config.bus_frequency)
{
	set_external(config.external);
}

I2C::~I2C()
{
	যদি (_dev) {
		px4_i2cbus_uninitialize(_dev);
		_dev = nullptr;
	}
}

পূর্ণাঙ্ক
I2C::set_bus_clock(unsigned bus, unsigned clock_hz)
{
	পূর্ণাঙ্ক index = bus - 1;

	যদি (index < 0 || index >= static_cast<পূর্ণাঙ্ক>(পরিমাণ(_bus_clocks) / পরিমাণ(_bus_clocks[0]))) {
		ঘূৰাই -EINVAL;
	}

	যদি (_bus_clocks[index] > 0) {
		// DEVICE_DEBUG("overriding clock of %u with %u Hz\n", _bus_clocks[index], clock_hz);
	}

	_bus_clocks[index] = clock_hz;

	ঘূৰাই OK;
}

পূর্ণাঙ্ক
I2C::init()
{
	পূর্ণাঙ্ক ret = PX4_ERROR;
	unsigned bus_index;

	যদি (_dev != nullptr) {
		ঘূৰাই PX4_OK;
	}

	// attach to the i2c bus
	_dev = px4_i2cbus_initialize(get_device_bus());

	যদি (_dev == nullptr) {
		DEVICE_DEBUG("failed to init I2C");
		ret = -ENOENT;
		গচ্ছ out;
	}

	// the above call fails for a non-existing bus index,
	// so the index math here is safe.
	bus_index = get_device_bus() - 1;

	// abort if the max frequency we allow (the frequency we ask)
	// is smaller than the bus frequency
	যদি (_bus_clocks[bus_index] > _frequency) {
		(শূন্য)px4_i2cbus_uninitialize(_dev);
		_dev = nullptr;
		DEVICE_LOG("FAIL: too slow for bus #%u: %u KHz, device max: %" PRIu32 " KHz)",
			   get_device_bus(), _bus_clocks[bus_index] / 1000, _frequency / 1000);
		ret = -EINVAL;
		গচ্ছ out;
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
	যদি (_bus_clocks[bus_index] == 0) {
		_bus_clocks[bus_index] = _frequency;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	যদি (ret != OK) {
		DEVICE_DEBUG("probe failed");
		গচ্ছ out;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	যদি (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		গচ্ছ out;
	}

	// tell the world where we are
	DEVICE_DEBUG("on I2C bus %d at 0x%02x (bus: %u KHz, max: %" PRIu32 " KHz)",
		     get_device_bus(), get_device_address(), _bus_clocks[bus_index] / 1000, _frequency / 1000);

out:

	যদি ((ret != OK) && (_dev != nullptr)) {
		px4_i2cbus_uninitialize(_dev);
		_dev = nullptr;
	}

	ঘূৰাই ret;
}

পূর্ণাঙ্ক
I2C::transfer(ধ্রুৱ uint8_t *send, ধ্রুৱ unsigned send_len, uint8_t *recv, ধ্রুৱ unsigned recv_len)
{
	পূর্ণাঙ্ক ret = PX4_ERROR;
	unsigned retry_গুনতি = 0;

	যদি (_dev == nullptr) {
		PX4_ERR("I2C device not opened");
		ঘূৰাই PX4_ERROR;
	}

	কৰা {
		DEVICE_DEBUG("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

		i2c_বাৰ্তা_s msgv[2] {};
		unsigned msgs = 0;

		যদি (send_len > 0) {
			msgv[msgs].frequency = _bus_clocks[get_device_bus() - 1];
			msgv[msgs].addr = get_device_address();
			msgv[msgs].flags = 0;
			msgv[msgs].buffer = const_cast<uint8_t *>(send);
			msgv[msgs].length = send_len;
			msgs++;
		}

		যদি (recv_len > 0) {
			msgv[msgs].frequency = _bus_clocks[get_device_bus() - 1];
			msgv[msgs].addr = get_device_address();
			msgv[msgs].flags = I2C_M_READ;
			msgv[msgs].buffer = recv;
			msgv[msgs].length = recv_len;
			msgs++;
		}

		যদি (msgs == 0) {
			ঘূৰাই -EINVAL;
		}

		পূর্ণাঙ্ক ret_transfer = I2C_TRANSFER(_dev, &msgv[0], msgs);

		যদি (ret_transfer != 0) {
			DEVICE_DEBUG("I2C transfer failed, result %d", ret_transfer);
			ret = PX4_ERROR;

		} অন্যথা {
			// success
			ret = PX4_OK;
			ভঙ্গ;
		}

		// if we have already retried once, and we aren't going to give up, then reset the bus
		যদি ((_retries > 0) && (retry_গুনতি < _retries)) {
#যদি defined(CONFIG_I2C_RESET)
			DEVICE_DEBUG("I2C bus: %d, Addr: %X, I2C_RESET %d/%d",
				     get_device_bus(), get_device_address(), retry_গুনতি + 1, _retries);
			I2C_RESET(_dev);
#endif // CONFIG_I2C_RESET
		}

	} যেতিয়ালৈ (retry_গুনতি++ < _retries);

	ঘূৰাই ret;
}

} // namespace device

#endif // CONFIG_I2C
