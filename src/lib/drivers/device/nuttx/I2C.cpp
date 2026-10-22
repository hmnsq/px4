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

#అన్తర్భావ "I2C.hpp"

#ఒకవేళ defined(CONFIG_I2C)

#అన్తర్భావ <px4_platform_common/i2c_spi_buses.h>
#అన్తర్భావ <nuttx/i2c/i2c_master.h>

నామాకాశ device
{
/*
 *  N.B. By defaulting the value of _bus_clocks to non Zero
 *  All calls to init() will NOT set the buss frequency
 */

గుర్తులేని పూర్ణాంకం I2C::_bus_clocks[PX4_సంఖ్య_I2_C_BUSES] = PX4_I2C_BUS_CLOCK_INIT;

I2C::I2C(uint8_t device_type, స్థిరరాశి అక్షరం *name, స్థిరరాశి పూర్ణాంకం bus, స్థిరరాశి uint16_t చిరునామా, స్థిరరాశి uint32_t frequency) :
	CDev(name, శూన్యసూచిక),
	_frequency(frequency)
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
	ఒకవేళ (_dev) {
		px4_i2cbus_uninitialize(_dev);
		_dev = శూన్యసూచిక;
	}
}

పూర్ణాంకం
I2C::సమితి_bus_clock(గుర్తులేని bus, గుర్తులేని clock_hz)
{
	పూర్ణాంకం సూచీ = bus - 1;

	ఒకవేళ (సూచీ < 0 || సూచీ >= static_cast<పూర్ణాంకం>(పరిమాణం(_bus_clocks) / పరిమాణం(_bus_clocks[0]))) {
		తిరిగి -EINVAL;
	}

	ఒకవేళ (_bus_clocks[సూచీ] > 0) {
		// DEVICE_DEBUG("overriding clock of %u with %u Hz\n", _bus_clocks[index], clock_hz);
	}

	_bus_clocks[సూచీ] = clock_hz;

	తిరిగి OK;
}

పూర్ణాంకం
I2C::init()
{
	పూర్ణాంకం ret = PX4_ERROR;
	గుర్తులేని bus_సూచీ;

	ఒకవేళ (_dev != శూన్యసూచిక) {
		తిరిగి PX4_OK;
	}

	// attach to the i2c bus
	_dev = px4_i2cbus_initialize(get_device_bus());

	ఒకవేళ (_dev == శూన్యసూచిక) {
		DEVICE_DEBUG("failed to init I2C");
		ret = -ENOENT;
		వెళ్ళు బహిర్గతం;
	}

	// the above call fails for a non-existing bus index,
	// so the index math here is safe.
	bus_సూచీ = get_device_bus() - 1;

	// abort if the max frequency we allow (the frequency we ask)
	// is smaller than the bus frequency
	ఒకవేళ (_bus_clocks[bus_సూచీ] > _frequency) {
		(శూన్య)px4_i2cbus_uninitialize(_dev);
		_dev = శూన్యసూచిక;
		DEVICE_LOG("FAIL: too slow for bus #%u: %u KHz, device max: %" PRIu32 " KHz)",
			   get_device_bus(), _bus_clocks[bus_సూచీ] / 1000, _frequency / 1000);
		ret = -EINVAL;
		వెళ్ళు బహిర్గతం;
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
	ఒకవేళ (_bus_clocks[bus_సూచీ] == 0) {
		_bus_clocks[bus_సూచీ] = _frequency;
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
	DEVICE_DEBUG("on I2C bus %d at 0x%02x (bus: %u KHz, max: %" PRIu32 " KHz)",
		     get_device_bus(), get_device_చిరునామా(), _bus_clocks[bus_సూచీ] / 1000, _frequency / 1000);

బహిర్గతం:

	ఒకవేళ ((ret != OK) && (_dev != శూన్యసూచిక)) {
		px4_i2cbus_uninitialize(_dev);
		_dev = శూన్యసూచిక;
	}

	తిరిగి ret;
}

పూర్ణాంకం
I2C::transfer(స్థిరరాశి uint8_t *send, స్థిరరాశి గుర్తులేని send_len, uint8_t *recv, స్థిరరాశి గుర్తులేని recv_len)
{
	పూర్ణాంకం ret = PX4_ERROR;
	గుర్తులేని retry_లెక్క = 0;

	ఒకవేళ (_dev == శూన్యసూచిక) {
		PX4_ERR("I2C device not opened");
		తిరిగి PX4_ERROR;
	}

	చేయి {
		DEVICE_DEBUG("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

		i2c_సందేశం_s msgv[2] {};
		గుర్తులేని msgs = 0;

		ఒకవేళ (send_len > 0) {
			msgv[msgs].frequency = _bus_clocks[get_device_bus() - 1];
			msgv[msgs].addr = get_device_చిరునామా();
			msgv[msgs].flags = 0;
			msgv[msgs].బఫరు_2 = const_cast<uint8_t *>(send);
			msgv[msgs].నిడివి = send_len;
			msgs++;
		}

		ఒకవేళ (recv_len > 0) {
			msgv[msgs].frequency = _bus_clocks[get_device_bus() - 1];
			msgv[msgs].addr = get_device_చిరునామా();
			msgv[msgs].flags = I2C_M_READ;
			msgv[msgs].బఫరు_2 = recv;
			msgv[msgs].నిడివి = recv_len;
			msgs++;
		}

		ఒకవేళ (msgs == 0) {
			తిరిగి -EINVAL;
		}

		పూర్ణాంకం ret_transfer = I2C_TRANSFER(_dev, &msgv[0], msgs);

		ఒకవేళ (ret_transfer != 0) {
			DEVICE_DEBUG("I2C transfer failed, result %d", ret_transfer);
			ret = PX4_ERROR;

		} లేకపోతే {
			// success
			ret = PX4_OK;
			ఆపు;
		}

		// if we have already retried once, and we aren't going to give up, then reset the bus
		ఒకవేళ ((_retries > 0) && (retry_లెక్క < _retries)) {
#ఒకవేళ defined(CONFIG_I2C_RESET)
			DEVICE_DEBUG("I2C bus: %d, Addr: %X, I2C_RESET %d/%d",
				     get_device_bus(), get_device_చిరునామా(), retry_లెక్క + 1, _retries);
			I2C_RESET(_dev);
#అయితే_ముగింపు // CONFIG_I2C_RESET
		}

	} అయితే (retry_లెక్క++ < _retries);

	తిరిగి ret;
}

} // namespace device

#అయితే_ముగింపు // CONFIG_I2C
