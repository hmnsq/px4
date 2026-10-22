/****************************************************************************
 *
 *   Copyright (C) 2019 PX4 Development Team. All rights reserved.
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
 * @file SPI.cpp
 *
 * Base class for devices connected via SPI.
 *
 */

#అన్తర్భావ "SPI.hpp"

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <px4_platform_common/px4_config.h>
#అన్తర్భావ <px4_platform_common/i2c_spi_buses.h>

స్థిర పూర్ణాంకం (*రిజిస్టరు_అంతరాయం_callback_func)(పూర్ణాంకం (*)(పూర్ణాంకం, శూన్య *, శూన్య *), శూన్య *arg) = ఖాళీ;

పూర్ణాంకం px4_arch_gpiosetevent(spi_drdy_gpio_t pin, తార్కిక r, తార్కిక f, తార్కిక e, పూర్ణాంకం (*func)(పూర్ణాంకం, శూన్య *, శూన్య *), శూన్య *arg)
{

	ఒకవేళ ((రిజిస్టరు_అంతరాయం_callback_func != ఖాళీ) && (func != ఖాళీ) && (arg != ఖాళీ)) {
		PX4_INFO("Register interrupt %p %p %p", రిజిస్టరు_అంతరాయం_callback_func, func, arg);
		తిరిగి రిజిస్టరు_అంతరాయం_callback_func(func, arg);
	}

	తిరిగి -1;
}

శూన్య రిజిస్టరు_అంతరాయం_callback_initalizer(పూర్ణాంకం (*func)(పూర్ణాంకం (*)(పూర్ణాంకం, శూన్య *, శూన్య *), శూన్య *arg))
{
	రిజిస్టరు_అంతరాయం_callback_func = func;
}

నామాకాశ device
{

SPI::_config_spi_bus_func_t  SPI::_config_spi_bus  = ఖాళీ;
SPI::_spi_transfer_func_t    SPI::_spi_transfer    = ఖాళీ;

pthread_mutex_t SPI::_mutex = PTHREAD_MUTEX_INITIALIZER;

SPI::SPI(uint8_t device_type, స్థిరరాశి అక్షరం *name, పూర్ణాంకం bus, uint32_t device, గణన spi_బహుళకం_e బహుళకం, uint32_t frequency) :
	CDev(name, శూన్యసూచిక)
	// CDev(name, nullptr),
	// _device(device),
	// _mode(mode),
	// _frequency(frequency)
{
	_device_id.devid = 0;

	_device_id.devid_s.devtype = device_type;
	// fill in _device_id fields for a SPI device
	_device_id.devid_s.bus_type = DeviceBusType_SPI;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.చిరునామా = (uint8_t)device;

	PX4_INFO("*** SPI Device ID 0x%x %d", _device_id.devid, _device_id.devid);
}

SPI::SPI(స్థిరరాశి I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_సూచీ, config.module_name, config.bus, config.spi_devid, config.spi_బహుళకం,
	      config.bus_frequency)
{
	సమితి_external(config.external);
}

SPI::~SPI()
{
	ఒకవేళ (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

పూర్ణాంకం
SPI::init()
{
	పూర్ణాంకం ret = PX4_ERROR;

	ఒకవేళ (_config_spi_bus == ఖాళీ) {
		PX4_ERR("NULL spi init function");
		తిరిగి ret;
	}

	pthread_mutex_lock(&_mutex);
	_fd = _config_spi_bus();
	pthread_mutex_unlock(&_mutex);

	ఒకవేళ (_fd == PX4_ERROR) {
		PX4_ERR("spi init failed");
		తిరిగి ret;
	}

	/* call the probe function to check whether the device is present */
	ret = probe();

	ఒకవేళ (ret != OK) {
		PX4_INFO("SPI probe failed");
		తిరిగి ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	ఒకవేళ (ret != OK) {
		PX4_ERR("cdev init failed");
		తిరిగి ret;
	}

	/* tell the world where we are */
	PX4_INFO("on SPI bus %d", get_device_bus());

	తిరిగి PX4_OK;
}

పూర్ణాంకం
SPI::transfer(uint8_t *send, uint8_t *recv, గుర్తులేని len)
{
	పూర్ణాంకం ret = PX4_ERROR;
	గుర్తులేని retry_లెక్క = 1;

	ఒకవేళ ((_fd != PX4_ERROR) && (_spi_transfer != ఖాళీ)) {
		చేయి {
			// PX4_DEBUG("SPI transfer out %p in %p len %u", send, recv, len);

			ఒకవేళ (_spi_transfer != ఖాళీ) {
				pthread_mutex_lock(&_mutex);

				ఒకవేళ (recv) { ret = _spi_transfer(_fd, send, recv, len); }

				లేకపోతే { ret = _spi_transfer(_fd, send, send, len); }

				pthread_mutex_unlock(&_mutex);

			} లేకపోతే {
				PX4_ERR("SPI transfer function is NULL");
			}

			ఒకవేళ (ret != PX4_ERROR) { ఆపు; }

		} అయితే (retry_లెక్క++ < _retries);
	}

	తిరిగి ret;
}

పూర్ణాంకం
SPI::transferhword(uint16_t *send, uint16_t *recv, గుర్తులేని len)
{
	// Not supported on SLPI
	తిరిగి PX4_ERROR;
}

} // namespace device
