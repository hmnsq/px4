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

#ਅਨ੍ਤਰ੍ਭਾਵ "I2C.hpp"

#ਜੇ defined(CONFIG_I2C)

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/i2c_spi_buses.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <nuttx/i2c/i2c_master.h>

ਨਾਮਾਕਾਸ਼ device
{
/*
 *  N.B. By defaulting the value of _bus_clocks to non Zero
 *  All calls to init() will NOT set the buss frequency
 */

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ I2C::_bus_clocks[PX4_ਸੰਖਿਆ_I2_C_BUSES] = PX4_I2C_BUS_CLOCK_INIT;

I2C::I2C(uint8_t device_type, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਪੂਰਨ bus, ਅਚਲ uint16_t ਪਤਾ, ਅਚਲ uint32_t frequency) :
	CDev(name, ਖਾਲੀ_ਸੂਚਕ),
	_frequency(frequency)
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
	ਜੇ (_dev) {
		px4_i2cbus_uninitialize(_dev);
		_dev = ਖਾਲੀ_ਸੂਚਕ;
	}
}

ਪੂਰਨ
I2C::ਸੈੱਟ_bus_clock(ਬਿਨਾਨਿਸ਼ਾਨੀ bus, ਬਿਨਾਨਿਸ਼ਾਨੀ clock_hz)
{
	ਪੂਰਨ ਸੂਚਕਾਂਕ = bus - 1;

	ਜੇ (ਸੂਚਕਾਂਕ < 0 || ਸੂਚਕਾਂਕ >= static_cast<ਪੂਰਨ>(ਆਕਾਰ(_bus_clocks) / ਆਕਾਰ(_bus_clocks[0]))) {
		ਵਾਪਸ -EINVAL;
	}

	ਜੇ (_bus_clocks[ਸੂਚਕਾਂਕ] > 0) {
		// DEVICE_DEBUG("overriding clock of %u with %u Hz\n", _bus_clocks[index], clock_hz);
	}

	_bus_clocks[ਸੂਚਕਾਂਕ] = clock_hz;

	ਵਾਪਸ OK;
}

ਪੂਰਨ
I2C::init()
{
	ਪੂਰਨ ret = PX4_ERROR;
	ਬਿਨਾਨਿਸ਼ਾਨੀ bus_ਸੂਚਕਾਂਕ;

	ਜੇ (_dev != ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ PX4_OK;
	}

	// attach to the i2c bus
	_dev = px4_i2cbus_initialize(get_device_bus());

	ਜੇ (_dev == ਖਾਲੀ_ਸੂਚਕ) {
		DEVICE_DEBUG("failed to init I2C");
		ret = -ENOENT;
		ਜਾਓ ਬਾਹਰਲਾ;
	}

	// the above call fails for a non-existing bus index,
	// so the index math here is safe.
	bus_ਸੂਚਕਾਂਕ = get_device_bus() - 1;

	// abort if the max frequency we allow (the frequency we ask)
	// is smaller than the bus frequency
	ਜੇ (_bus_clocks[bus_ਸੂਚਕਾਂਕ] > _frequency) {
		(ਖਾਲੀ)px4_i2cbus_uninitialize(_dev);
		_dev = ਖਾਲੀ_ਸੂਚਕ;
		DEVICE_LOG("FAIL: too slow for bus #%u: %u KHz, device max: %" PRIu32 " KHz)",
			   get_device_bus(), _bus_clocks[bus_ਸੂਚਕਾਂਕ] / 1000, _frequency / 1000);
		ret = -EINVAL;
		ਜਾਓ ਬਾਹਰਲਾ;
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
	ਜੇ (_bus_clocks[bus_ਸੂਚਕਾਂਕ] == 0) {
		_bus_clocks[bus_ਸੂਚਕਾਂਕ] = _frequency;
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
	DEVICE_DEBUG("on I2C bus %d at 0x%02x (bus: %u KHz, max: %" PRIu32 " KHz)",
		     get_device_bus(), get_device_ਪਤਾ(), _bus_clocks[bus_ਸੂਚਕਾਂਕ] / 1000, _frequency / 1000);

ਬਾਹਰਲਾ:

	ਜੇ ((ret != OK) && (_dev != ਖਾਲੀ_ਸੂਚਕ)) {
		px4_i2cbus_uninitialize(_dev);
		_dev = ਖਾਲੀ_ਸੂਚਕ;
	}

	ਵਾਪਸ ret;
}

ਪੂਰਨ
I2C::transfer(ਅਚਲ uint8_t *send, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ send_len, uint8_t *recv, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ recv_len)
{
	ਪੂਰਨ ret = PX4_ERROR;
	ਬਿਨਾਨਿਸ਼ਾਨੀ retry_ਗਿਣਤੀ = 0;

	ਜੇ (_dev == ਖਾਲੀ_ਸੂਚਕ) {
		PX4_ERR("I2C device not opened");
		ਵਾਪਸ PX4_ERROR;
	}

	ਕਰੋ {
		DEVICE_DEBUG("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

		i2c_ਸੁਨੇਹਾ_s msgv[2] {};
		ਬਿਨਾਨਿਸ਼ਾਨੀ msgs = 0;

		ਜੇ (send_len > 0) {
			msgv[msgs].frequency = _bus_clocks[get_device_bus() - 1];
			msgv[msgs].addr = get_device_ਪਤਾ();
			msgv[msgs].flags = 0;
			msgv[msgs].ਬਫਰ_2 = const_cast<uint8_t *>(send);
			msgv[msgs].ਲੰਮਾਈ = send_len;
			msgs++;
		}

		ਜੇ (recv_len > 0) {
			msgv[msgs].frequency = _bus_clocks[get_device_bus() - 1];
			msgv[msgs].addr = get_device_ਪਤਾ();
			msgv[msgs].flags = I2C_M_READ;
			msgv[msgs].ਬਫਰ_2 = recv;
			msgv[msgs].ਲੰਮਾਈ = recv_len;
			msgs++;
		}

		ਜੇ (msgs == 0) {
			ਵਾਪਸ -EINVAL;
		}

		ਪੂਰਨ ret_transfer = I2C_TRANSFER(_dev, &msgv[0], msgs);

		ਜੇ (ret_transfer != 0) {
			DEVICE_DEBUG("I2C transfer failed, result %d", ret_transfer);
			ret = PX4_ERROR;

		} ਨਹੀਂਤਾਂ {
			// success
			ret = PX4_OK;
			ਰੋਕੋ;
		}

		// if we have already retried once, and we aren't going to give up, then reset the bus
		ਜੇ ((_retries > 0) && (retry_ਗਿਣਤੀ < _retries)) {
#ਜੇ defined(CONFIG_I2C_RESET)
			DEVICE_DEBUG("I2C bus: %d, Addr: %X, I2C_RESET %d/%d",
				     get_device_bus(), get_device_ਪਤਾ(), retry_ਗਿਣਤੀ + 1, _retries);
			I2C_RESET(_dev);
#ਜੇ_ਅੰਤ // CONFIG_I2C_RESET
		}

	} ਜਦੋਂ (retry_ਗਿਣਤੀ++ < _retries);

	ਵਾਪਸ ret;
}

} // namespace device

#ਜੇ_ਅੰਤ // CONFIG_I2C
