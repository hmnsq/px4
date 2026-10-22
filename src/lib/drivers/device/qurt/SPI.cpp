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

#ଅନ୍ତର୍ଭାଵ "SPI.hpp"

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/px4_config.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/i2c_spi_buses.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*register_interrupt_callback_func)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ଶୂନ୍ୟ *), ଶୂନ୍ୟ *arg) = ଖାଲି;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା px4_arch_gpiosetevent(spi_drdy_gpio_t pin, ତାର୍କିକ r, ତାର୍କିକ f, ତାର୍କିକ e, ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*func)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ଶୂନ୍ୟ *), ଶୂନ୍ୟ *arg)
{

	ଯଦି ((register_interrupt_callback_func != ଖାଲି) && (func != ଖାଲି) && (arg != ଖାଲି)) {
		PX4_INFO("Register interrupt %p %p %p", register_interrupt_callback_func, func, arg);
		ଫେରନ୍ତୁ register_interrupt_callback_func(func, arg);
	}

	ଫେରନ୍ତୁ -1;
}

ଶୂନ୍ୟ register_interrupt_callback_initalizer(ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*func)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ଶୂନ୍ୟ *), ଶୂନ୍ୟ *arg))
{
	register_interrupt_callback_func = func;
}

ନାମାକାଶ device
{

SPI::_config_spi_bus_func_t  SPI::_config_spi_bus  = ଖାଲି;
SPI::_spi_transfer_func_t    SPI::_spi_transfer    = ଖାଲି;

pthread_mutex_t SPI::_mutex = PTHREAD_MUTEX_INITIALIZER;

SPI::SPI(uint8_t device_type, ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bus, uint32_t device, ଗଣନା spi_mode_e mode, uint32_t frequency) :
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

SPI::SPI(ଅଚଳ I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_index, config.module_name, config.bus, config.spi_devid, config.spi_mode,
	      config.bus_frequency)
{
	set_external(config.external);
}

SPI::~SPI()
{
	ଯଦି (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
SPI::init()
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = PX4_ERROR;

	ଯଦି (_config_spi_bus == ଖାଲି) {
		PX4_ERR("NULL spi init function");
		ଫେରନ୍ତୁ ret;
	}

	pthread_mutex_lock(&_mutex);
	_fd = _config_spi_bus();
	pthread_mutex_unlock(&_mutex);

	ଯଦି (_fd == PX4_ERROR) {
		PX4_ERR("spi init failed");
		ଫେରନ୍ତୁ ret;
	}

	/* call the probe function to check whether the device is present */
	ret = probe();

	ଯଦି (ret != OK) {
		PX4_INFO("SPI probe failed");
		ଫେରନ୍ତୁ ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	ଯଦି (ret != OK) {
		PX4_ERR("cdev init failed");
		ଫେରନ୍ତୁ ret;
	}

	/* tell the world where we are */
	PX4_INFO("on SPI bus %d", get_device_bus());

	ଫେରନ୍ତୁ PX4_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
SPI::transfer(uint8_t *send, uint8_t *recv, unsigned len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = PX4_ERROR;
	unsigned retry_ଗଣନା = 1;

	ଯଦି ((_fd != PX4_ERROR) && (_spi_transfer != ଖାଲି)) {
		କର {
			// PX4_DEBUG("SPI transfer out %p in %p len %u", send, recv, len);

			ଯଦି (_spi_transfer != ଖାଲି) {
				pthread_mutex_lock(&_mutex);

				ଯଦି (recv) { ret = _spi_transfer(_fd, send, recv, len); }

				ଅନ୍ୟଥା { ret = _spi_transfer(_fd, send, send, len); }

				pthread_mutex_unlock(&_mutex);

			} ଅନ୍ୟଥା {
				PX4_ERR("SPI transfer function is NULL");
			}

			ଯଦି (ret != PX4_ERROR) { ଭାଙ୍ଗ; }

		} ଯେତେବେଳେ (retry_ଗଣନା++ < _retries);
	}

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
SPI::transferhword(uint16_t *send, uint16_t *recv, unsigned len)
{
	// Not supported on SLPI
	ଫେରନ୍ତୁ PX4_ERROR;
}

} // namespace device
