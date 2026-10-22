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

#شامل "SPI.hpp"

#اگر defined(CONFIG_SPI)

#اگر_تعریف_شدہ __PX4_LINUX

#شامل <fcntl.h>
#شامل <unistd.h>
#شامل <sys/ioctl.h>
#شامل <linux/types.h>
#شامل <linux/spi/spidev.h>

#شامل <px4_platform_common/i2c_spi_buses.h>
#شامل <px4_platform_common/px4_config.h>

نامخلا device
{

SPI::SPI(uint8_t device_type, ثابت حرف *name, صحیح bus, uint32_t device, شمار spi_منوال_e منوال, uint32_t frequency) :
	CDev(name, خالی_اشاریہ),
	_device(device),
	_mode(منوال),
	_frequency(frequency)
{
	_device_id.devid_s.devtype = device_type;
	// fill in _device_id fields for a SPI device
	_device_id.devid_s.bus_type = DeviceBusType_SPI;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.پتہ = (uint8_t)device;
}

SPI::SPI(ثابت I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_اشاریہ, config.module_name, config.bus, config.spi_devid, config.spi_منوال,
	      config.bus_frequency)
{
	سیٹ_external(config.external);
}

SPI::~SPI()
{
	اگر (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

صحیح
SPI::init()
{
	// Open the actual SPI device
	حرف dev_راستہ[16];
	snprintf(dev_راستہ, حجم(dev_راستہ), "/dev/spidev%i.%i", get_device_bus(), PX4_SPI_DEV_ID(_device));
	DEVICE_DEBUG("%s", dev_راستہ);
	_fd = ::open(dev_راستہ, O_RDWR);

	اگر (_fd < 0) {
		PX4_ERR("could not open %s", dev_راستہ);
		واپس PX4_ERROR;
	}

	/* call the probe function to check whether the device is present */
	صحیح ret = probe();

	اگر (ret != OK) {
		DEVICE_DEBUG("probe failed");
		واپس ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	اگر (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		واپس ret;
	}

	/* tell the world where we are */
	DEVICE_DEBUG("on SPI bus %d at %d (%u KHz)", get_device_bus(), PX4_SPI_DEV_ID(_device), _frequency / 1000);

	واپس PX4_OK;
}

صحیح
SPI::transfer(uint8_t *send, uint8_t *recv, غیرعلامتی len)
{
	اگر ((send == خالی_اشاریہ) && (recv == خالی_اشاریہ)) {
		واپس -EINVAL;
	}

	// set write mode of SPI
	صحیح نتیجہ = ::ioctl(_fd, SPI_IOC_WR_منوال, &_mode);

	اگر (نتیجہ == -1) {
		PX4_ERR("can’t set spi mode");
		واپس PX4_ERROR;
	}

	spi_ioc_transfer spi_transfer{};

	spi_transfer.tx_buf = (uint64_t)send;
	spi_transfer.rx_buf = (uint64_t)recv;
	spi_transfer.len = len;
	spi_transfer.رفتار_hz = _frequency;
	spi_transfer.بٹ_فی_لفظ = 8;

	نتیجہ = ::ioctl(_fd, SPI_IOC_پیغام(1), &spi_transfer);

	اگر (نتیجہ != (صحیح)len) {
		PX4_ERR("write failed. Reported %d bytes written (%s)", نتیجہ, strerror(errno));
		واپس PX4_ERROR;
	}

	واپس PX4_OK;
}

صحیح
SPI::transferhword(uint16_t *send, uint16_t *recv, غیرعلامتی len)
{
	اگر ((send == خالی_اشاریہ) && (recv == خالی_اشاریہ)) {
		واپس -EINVAL;
	}

	// set write mode of SPI
	صحیح نتیجہ = ::ioctl(_fd, SPI_IOC_WR_منوال, &_mode);

	اگر (نتیجہ == -1) {
		PX4_ERR("can’t set spi mode");
		واپس PX4_ERROR;
	}

	صحیح بٹ = 16;
	نتیجہ = ::ioctl(_fd, SPI_IOC_WR_بٹ_فی_لفظ, &بٹ);

	اگر (نتیجہ == -1) {
		PX4_ERR("can’t set 16 bit spi mode");
		واپس PX4_ERROR;
	}

	spi_ioc_transfer spi_transfer[1] {};

	spi_transfer[0].tx_buf = (uint64_t)send;
	spi_transfer[0].rx_buf = (uint64_t)recv;
	spi_transfer[0].len = len * 2;
	spi_transfer[0].رفتار_hz = _frequency;
	//spi_transfer[0].bits_per_word = 8;
	//spi_transfer[0].delay_usecs = 10;
	spi_transfer[0].cs_change = سچ;

	نتیجہ = ::ioctl(_fd, SPI_IOC_پیغام(1), &spi_transfer);

	اگر (نتیجہ != (صحیح)(len * 2)) {
		PX4_ERR("write failed. Reported %d bytes written (%s)", نتیجہ, strerror(errno));
		واپس PX4_ERROR;
	}

	واپس PX4_OK;
}

} // namespace device

#اگر_ختم // __PX4_LINUX
#اگر_ختم // CONFIG_SPI
