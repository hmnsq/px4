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

#અન્તર્ભાવ "SPI.hpp"

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <sys/ioctl.h>
#અન્તર્ભાવ <px4_platform_common/px4_config.h>
#અન્તર્ભાવ <px4_platform_common/i2c_spi_buses.h>

સ્થિર પૂર્ણાંક (*register_interrupt_callback_func)(પૂર્ણાંક (*)(પૂર્ણાંક, શૂન્ય *, શૂન્ય *), શૂન્ય *arg) = ખાલી;

પૂર્ણાંક px4_arch_gpiosetevent(spi_drdy_gpio_t pin, તાર્કિક r, તાર્કિક f, તાર્કિક e, પૂર્ણાંક (*func)(પૂર્ણાંક, શૂન્ય *, શૂન્ય *), શૂન્ય *arg)
{

	જો ((register_interrupt_callback_func != ખાલી) && (func != ખાલી) && (arg != ખાલી)) {
		PX4_INFO("Register interrupt %p %p %p", register_interrupt_callback_func, func, arg);
		પાછા register_interrupt_callback_func(func, arg);
	}

	પાછા -1;
}

શૂન્ય register_interrupt_callback_initalizer(પૂર્ણાંક (*func)(પૂર્ણાંક (*)(પૂર્ણાંક, શૂન્ય *, શૂન્ય *), શૂન્ય *arg))
{
	register_interrupt_callback_func = func;
}

નામાકાશ device
{

SPI::_config_spi_bus_func_t  SPI::_config_spi_bus  = ખાલી;
SPI::_spi_transfer_func_t    SPI::_spi_transfer    = ખાલી;

pthread_mutex_t SPI::_mutex = PTHREAD_MUTEX_INITIALIZER;

SPI::SPI(uint8_t device_type, અચલ અક્ષર *name, પૂર્ણાંક bus, uint32_t device, ગણના spi_mode_e mode, uint32_t frequency) :
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

SPI::SPI(અચલ I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_index, config.module_name, config.bus, config.spi_devid, config.spi_mode,
	      config.bus_frequency)
{
	set_external(config.external);
}

SPI::~SPI()
{
	જો (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

પૂર્ણાંક
SPI::init()
{
	પૂર્ણાંક ret = PX4_ERROR;

	જો (_config_spi_bus == ખાલી) {
		PX4_ERR("NULL spi init function");
		પાછા ret;
	}

	pthread_mutex_lock(&_mutex);
	_fd = _config_spi_bus();
	pthread_mutex_unlock(&_mutex);

	જો (_fd == PX4_ERROR) {
		PX4_ERR("spi init failed");
		પાછા ret;
	}

	/* call the probe function to check whether the device is present */
	ret = probe();

	જો (ret != OK) {
		PX4_INFO("SPI probe failed");
		પાછા ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	જો (ret != OK) {
		PX4_ERR("cdev init failed");
		પાછા ret;
	}

	/* tell the world where we are */
	PX4_INFO("on SPI bus %d", get_device_bus());

	પાછા PX4_OK;
}

પૂર્ણાંક
SPI::transfer(uint8_t *send, uint8_t *recv, unsigned len)
{
	પૂર્ણાંક ret = PX4_ERROR;
	unsigned retry_ગણતરી = 1;

	જો ((_fd != PX4_ERROR) && (_spi_transfer != ખાલી)) {
		કરો {
			// PX4_DEBUG("SPI transfer out %p in %p len %u", send, recv, len);

			જો (_spi_transfer != ખાલી) {
				pthread_mutex_lock(&_mutex);

				જો (recv) { ret = _spi_transfer(_fd, send, recv, len); }

				નહીંતર { ret = _spi_transfer(_fd, send, send, len); }

				pthread_mutex_unlock(&_mutex);

			} નહીંતર {
				PX4_ERR("SPI transfer function is NULL");
			}

			જો (ret != PX4_ERROR) { તોડો; }

		} જ્યારે (retry_ગણતરી++ < _retries);
	}

	પાછા ret;
}

પૂર્ણાંક
SPI::transferhword(uint16_t *send, uint16_t *recv, unsigned len)
{
	// Not supported on SLPI
	પાછા PX4_ERROR;
}

} // namespace device
