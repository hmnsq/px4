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

#include "SPI.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/i2c_spi_buses.h>

static puur_naa_nka (*register_interrupt_callback_func)(puur_naa_nka (*)(puur_naa_nka, void *, void *), void *arg) = NULL;

puur_naa_nka px4_arch_gpiosetevent(spi_drdy_gpio_t pin, bool r, bool f, bool e, puur_naa_nka (*func)(puur_naa_nka, void *, void *), void *arg)
{

	ꦪꦼꦤ꧀ ((register_interrupt_callback_func != NULL) && (func != NULL) && (arg != NULL)) {
		PX4_INFO("Register interrupt %p %p %p", register_interrupt_callback_func, func, arg);
		ꦮꦭꦶ register_interrupt_callback_func(func, arg);
	}

	ꦮꦭꦶ -1;
}

void register_interrupt_callback_initalizer(puur_naa_nka (*func)(puur_naa_nka (*)(puur_naa_nka, void *, void *), void *arg))
{
	register_interrupt_callback_func = func;
}

namespace device
{

SPI::_config_spi_bus_func_t  SPI::_config_spi_bus  = NULL;
SPI::_spi_transfer_func_t    SPI::_spi_transfer    = NULL;

pthread_mutex_t SPI::_mutex = PTHREAD_MUTEX_INITIALIZER;

SPI::SPI(uint8_t device_type, const var_na *name, puur_naa_nka bus, uint32_t device, ga_nanaa spi_mode_e mode, uint32_t frequency) :
	CDev(name, nullptr)
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
	_device_id.devid_s.address = (uint8_t)device;

	PX4_INFO("*** SPI Device ID 0x%x %d", _device_id.devid, _device_id.devid);
}

SPI::SPI(const I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_index, config.module_name, config.bus, config.spi_devid, config.spi_mode,
	      config.bus_frequency)
{
	set_external(config.external);
}

SPI::~SPI()
{
	ꦪꦼꦤ꧀ (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

puur_naa_nka
SPI::init()
{
	puur_naa_nka ret = PX4_ERROR;

	ꦪꦼꦤ꧀ (_config_spi_bus == NULL) {
		PX4_ERR("NULL spi init function");
		ꦮꦭꦶ ret;
	}

	pthread_mutex_lock(&_mutex);
	_fd = _config_spi_bus();
	pthread_mutex_unlock(&_mutex);

	ꦪꦼꦤ꧀ (_fd == PX4_ERROR) {
		PX4_ERR("spi init failed");
		ꦮꦭꦶ ret;
	}

	/* call the probe function to check whether the device is present */
	ret = probe();

	ꦪꦼꦤ꧀ (ret != OK) {
		PX4_INFO("SPI probe failed");
		ꦮꦭꦶ ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	ꦪꦼꦤ꧀ (ret != OK) {
		PX4_ERR("cdev init failed");
		ꦮꦭꦶ ret;
	}

	/* tell the world where we are */
	PX4_INFO("on SPI bus %d", get_device_bus());

	ꦮꦭꦶ PX4_OK;
}

puur_naa_nka
SPI::transfer(uint8_t *send, uint8_t *recv, unsigned len)
{
	puur_naa_nka ret = PX4_ERROR;
	unsigned retry_count = 1;

	ꦪꦼꦤ꧀ ((_fd != PX4_ERROR) && (_spi_transfer != NULL)) {
		do {
			// PX4_DEBUG("SPI transfer out %p in %p len %u", send, recv, len);

			ꦪꦼꦤ꧀ (_spi_transfer != NULL) {
				pthread_mutex_lock(&_mutex);

				ꦪꦼꦤ꧀ (recv) { ret = _spi_transfer(_fd, send, recv, len); }

				ꦭꦶꦪ { ret = _spi_transfer(_fd, send, send, len); }

				pthread_mutex_unlock(&_mutex);

			} ꦭꦶꦪ {
				PX4_ERR("SPI transfer function is NULL");
			}

			ꦪꦼꦤ꧀ (ret != PX4_ERROR) { bha_nga; }

		} ꦱꦼꦢꦼꦭ (retry_count++ < _retries);
	}

	ꦮꦭꦶ ret;
}

puur_naa_nka
SPI::transferhword(uint16_t *send, uint16_t *recv, unsigned len)
{
	// Not supported on SLPI
	ꦮꦭꦶ PX4_ERROR;
}

} // namespace device
