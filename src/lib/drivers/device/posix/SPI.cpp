/****************************************************************************
 *
 *   Copyright (C) 2019-2021 PX4 Development Team. All rights reserved.
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

#إذا defined(CONFIG_SPI)

#که_تعریف_شوی __PX4_LINUX

#ضمن <fcntl.h>
#ضمن <unistd.h>
#ضمن <sys/ioctl.h>
#ضمن <linux/types.h>
#ضمن <linux/spi/spidev.h>

#ضمن <px4_platform_common/i2c_spi_buses.h>
#ضمن <px4_platform_common/px4_config.h>

نطاق device
{

SPI::SPI(uint8_t device_type, ثابت حرف *name, صحيح bus, uint32_t device, تعداد spi_ډېرپېښ_e ډېرپېښ, uint32_t frequency) :
	CDev(name, تش_اشاره),
	_device(device),
	_mode(ډېرپېښ),
	_frequency(frequency)
{
	_device_id.devid_s.devtype = device_type;
	// fill in _device_id fields for a SPI device
	_device_id.devid_s.bus_type = DeviceBusType_SPI;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.پته = (uint8_t)device;
}

SPI::SPI(ثابت I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_شاخص, config.module_name, config.bus, config.spi_devid, config.spi_ډېرپېښ,
	      config.bus_frequency)
{
	سېټ_external(config.external);
}

SPI::~SPI()
{
	إذا (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

صحيح
SPI::init()
{
	// Open the actual SPI device
	حرف dev_لار[16];
	snprintf(dev_لار, حجم(dev_لار), "/dev/spidev%i.%i", get_device_bus(), PX4_SPI_DEV_ID(_device));
	DEVICE_DEBUG("%s", dev_لار);
	_fd = ::open(dev_لار, O_RDWR);

	إذا (_fd < 0) {
		PX4_ERR("could not open %s", dev_لار);
		أرجع PX4_ERROR;
	}

	/* call the probe function to check whether the device is present */
	صحيح ret = probe();

	إذا (ret != OK) {
		DEVICE_DEBUG("probe failed");
		أرجع ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	إذا (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		أرجع ret;
	}

	/* tell the world where we are */
	DEVICE_DEBUG("on SPI bus %d at %d (%u KHz)", get_device_bus(), PX4_SPI_DEV_ID(_device), _frequency / 1000);

	أرجع PX4_OK;
}

صحيح
SPI::transfer(uint8_t *send, uint8_t *recv, بینښه len)
{
	إذا ((send == تش_اشاره) && (recv == تش_اشاره)) {
		أرجع -EINVAL;
	}

	// set write mode of SPI
	صحيح پایله = ::ioctl(_fd, SPI_IOC_WR_ډېرپېښ, &_mode);

	إذا (پایله == -1) {
		PX4_ERR("can’t set spi mode");
		أرجع PX4_ERROR;
	}

	spi_ioc_transfer spi_transfer{};

	spi_transfer.tx_buf = (uint64_t)send;
	spi_transfer.rx_buf = (uint64_t)recv;
	spi_transfer.len = len;
	spi_transfer.چټکتیا_hz = _frequency;
	spi_transfer.بټونه_فی_کلمه = 8;

	پایله = ::ioctl(_fd, SPI_IOC_پیغام(1), &spi_transfer);

	إذا (پایله != (صحيح)len) {
		PX4_ERR("write failed. Reported %d bytes written (%s)", پایله, strerror(errno));
		أرجع PX4_ERROR;
	}

	أرجع PX4_OK;
}

صحيح
SPI::transferhword(uint16_t *send, uint16_t *recv, بینښه len)
{
	إذا ((send == تش_اشاره) && (recv == تش_اشاره)) {
		أرجع -EINVAL;
	}

	// set write mode of SPI
	صحيح پایله = ::ioctl(_fd, SPI_IOC_WR_ډېرپېښ, &_mode);

	إذا (پایله == -1) {
		PX4_ERR("can’t set spi mode");
		أرجع PX4_ERROR;
	}

	صحيح بټونه = 16;
	پایله = ::ioctl(_fd, SPI_IOC_WR_بټونه_فی_کلمه, &بټونه);

	إذا (پایله == -1) {
		PX4_ERR("can’t set 16 bit spi mode");
		أرجع PX4_ERROR;
	}

	spi_ioc_transfer spi_transfer[1] {};

	spi_transfer[0].tx_buf = (uint64_t)send;
	spi_transfer[0].rx_buf = (uint64_t)recv;
	spi_transfer[0].len = len * 2;
	spi_transfer[0].چټکتیا_hz = _frequency;
	//spi_transfer[0].bits_per_word = 8;
	//spi_transfer[0].delay_usecs = 10;
	spi_transfer[0].cs_change = ریښتیا;

	پایله = ::ioctl(_fd, SPI_IOC_پیغام(1), &spi_transfer);

	إذا (پایله != (صحيح)(len * 2)) {
		PX4_ERR("write failed. Reported %d bytes written (%s)", پایله, strerror(errno));
		أرجع PX4_ERROR;
	}

	أرجع PX4_OK;
}

} // namespace device

#د_که_پای // __PX4_LINUX
#د_که_پای // CONFIG_SPI
