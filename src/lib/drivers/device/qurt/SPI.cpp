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

#ضمن "SPI.hpp"

#ضمن <fcntl.h>
#ضمن <unistd.h>
#ضمن <sys/ioctl.h>
#ضمن <px4_platform_common/px4_config.h>
#ضمن <px4_platform_common/i2c_spi_buses.h>

ساكن صحيح (*register_interrupt_callback_func)(صحيح (*)(صحيح, فراغ *, فراغ *), فراغ *arg) = عدم;

صحيح px4_arch_gpiosetevent(spi_drdy_gpio_t pin, منطقي r, منطقي f, منطقي e, صحيح (*func)(صحيح, فراغ *, فراغ *), فراغ *arg)
{

	𐡄𐡍 ((register_interrupt_callback_func != عدم) && (func != عدم) && (arg != عدم)) {
		PX4_INFO("Register interrupt %p %p %p", register_interrupt_callback_func, func, arg);
		أرجع register_interrupt_callback_func(func, arg);
	}

	أرجع -1;
}

فراغ register_interrupt_callback_initalizer(صحيح (*func)(صحيح (*)(صحيح, فراغ *, فراغ *), فراغ *arg))
{
	register_interrupt_callback_func = func;
}

نطاق device
{

SPI::_config_spi_bus_func_t  SPI::_config_spi_bus  = عدم;
SPI::_spi_transfer_func_t    SPI::_spi_transfer    = عدم;

pthread_mutex_t SPI::_mutex = PTHREAD_MUTEX_INITIALIZER;

SPI::SPI(uint8_t device_type, ثابت حرف *name, صحيح bus, uint32_t device, تعداد spi_mode_e mode, uint32_t frequency) :
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

SPI::SPI(ثابت I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_index, config.module_name, config.bus, config.spi_devid, config.spi_mode,
	      config.bus_frequency)
{
	set_external(config.external);
}

SPI::~SPI()
{
	𐡄𐡍 (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

صحيح
SPI::init()
{
	صحيح ret = PX4_ERROR;

	𐡄𐡍 (_config_spi_bus == عدم) {
		PX4_ERR("NULL spi init function");
		أرجع ret;
	}

	pthread_mutex_lock(&_mutex);
	_fd = _config_spi_bus();
	pthread_mutex_unlock(&_mutex);

	𐡄𐡍 (_fd == PX4_ERROR) {
		PX4_ERR("spi init failed");
		أرجع ret;
	}

	/* call the probe function to check whether the device is present */
	ret = probe();

	𐡄𐡍 (ret != OK) {
		PX4_INFO("SPI probe failed");
		أرجع ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	𐡄𐡍 (ret != OK) {
		PX4_ERR("cdev init failed");
		أرجع ret;
	}

	/* tell the world where we are */
	PX4_INFO("on SPI bus %d", get_device_bus());

	أرجع PX4_OK;
}

صحيح
SPI::transfer(uint8_t *send, uint8_t *recv, unsigned len)
{
	صحيح ret = PX4_ERROR;
	unsigned retry_count = 1;

	𐡄𐡍 ((_fd != PX4_ERROR) && (_spi_transfer != عدم)) {
		𐡏𐡁𐡃 {
			// PX4_DEBUG("SPI transfer out %p in %p len %u", send, recv, len);

			𐡄𐡍 (_spi_transfer != عدم) {
				pthread_mutex_lock(&_mutex);

				𐡄𐡍 (recv) { ret = _spi_transfer(_fd, send, recv, len); }

				وإلا { ret = _spi_transfer(_fd, send, send, len); }

				pthread_mutex_unlock(&_mutex);

			} وإلا {
				PX4_ERR("SPI transfer function is NULL");
			}

			𐡄𐡍 (ret != PX4_ERROR) { اكسر; }

		} طالما (retry_count++ < _retries);
	}

	أرجع ret;
}

صحيح
SPI::transferhword(uint16_t *send, uint16_t *recv, unsigned len)
{
	// Not supported on SLPI
	أرجع PX4_ERROR;
}

} // namespace device
