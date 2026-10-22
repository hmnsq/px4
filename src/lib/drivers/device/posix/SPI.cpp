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

#അന്തര്ഭാവ "SPI.hpp"

#എങ്കിൽ defined(CONFIG_SPI)

#ifdef __PX4_LINUX

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <unistd.h>
#അന്തര്ഭാവ <sys/ioctl.h>
#അന്തര്ഭാവ <linux/types.h>
#അന്തര്ഭാവ <linux/spi/spidev.h>

#അന്തര്ഭാവ <px4_platform_common/i2c_spi_buses.h>
#അന്തര്ഭാവ <px4_platform_common/px4_config.h>

നാമാകാശ device
{

SPI::SPI(uint8_t device_type, സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ bus, uint32_t device, എണ്ണൽ spi_mode_e mode, uint32_t frequency) :
	CDev(name, nullptr),
	_device(device),
	_mode(mode),
	_frequency(frequency)
{
	_device_id.devid_s.devtype = device_type;
	// fill in _device_id fields for a SPI device
	_device_id.devid_s.bus_type = DeviceBusType_SPI;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.address = (uint8_t)device;
}

SPI::SPI(സ്ഥിരം I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_index, config.module_name, config.bus, config.spi_devid, config.spi_mode,
	      config.bus_frequency)
{
	set_external(config.external);
}

SPI::~SPI()
{
	എങ്കിൽ (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

പൂർണ്ണസംഖ്യ
SPI::init()
{
	// Open the actual SPI device
	അക്ഷരം dev_path[16];
	snprintf(dev_path, വലുപ്പം(dev_path), "/dev/spidev%i.%i", get_device_bus(), PX4_SPI_DEV_ID(_device));
	DEVICE_DEBUG("%s", dev_path);
	_fd = ::open(dev_path, O_RDWR);

	എങ്കിൽ (_fd < 0) {
		PX4_ERR("could not open %s", dev_path);
		തിരികെ PX4_ERROR;
	}

	/* call the probe function to check whether the device is present */
	പൂർണ്ണസംഖ്യ ret = probe();

	എങ്കിൽ (ret != OK) {
		DEVICE_DEBUG("probe failed");
		തിരികെ ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	എങ്കിൽ (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		തിരികെ ret;
	}

	/* tell the world where we are */
	DEVICE_DEBUG("on SPI bus %d at %d (%u KHz)", get_device_bus(), PX4_SPI_DEV_ID(_device), _frequency / 1000);

	തിരികെ PX4_OK;
}

പൂർണ്ണസംഖ്യ
SPI::transfer(uint8_t *send, uint8_t *recv, unsigned len)
{
	എങ്കിൽ ((send == nullptr) && (recv == nullptr)) {
		തിരികെ -EINVAL;
	}

	// set write mode of SPI
	പൂർണ്ണസംഖ്യ result = ::ioctl(_fd, SPI_IOC_WR_MODE, &_mode);

	എങ്കിൽ (result == -1) {
		PX4_ERR("can’t set spi mode");
		തിരികെ PX4_ERROR;
	}

	spi_ioc_transfer spi_transfer{};

	spi_transfer.tx_buf = (uint64_t)send;
	spi_transfer.rx_buf = (uint64_t)recv;
	spi_transfer.len = len;
	spi_transfer.speed_hz = _frequency;
	spi_transfer.bits_per_word = 8;

	result = ::ioctl(_fd, SPI_IOC_MESSAGE(1), &spi_transfer);

	എങ്കിൽ (result != (പൂർണ്ണസംഖ്യ)len) {
		PX4_ERR("write failed. Reported %d bytes written (%s)", result, strerror(errno));
		തിരികെ PX4_ERROR;
	}

	തിരികെ PX4_OK;
}

പൂർണ്ണസംഖ്യ
SPI::transferhword(uint16_t *send, uint16_t *recv, unsigned len)
{
	എങ്കിൽ ((send == nullptr) && (recv == nullptr)) {
		തിരികെ -EINVAL;
	}

	// set write mode of SPI
	പൂർണ്ണസംഖ്യ result = ::ioctl(_fd, SPI_IOC_WR_MODE, &_mode);

	എങ്കിൽ (result == -1) {
		PX4_ERR("can’t set spi mode");
		തിരികെ PX4_ERROR;
	}

	പൂർണ്ണസംഖ്യ bits = 16;
	result = ::ioctl(_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);

	എങ്കിൽ (result == -1) {
		PX4_ERR("can’t set 16 bit spi mode");
		തിരികെ PX4_ERROR;
	}

	spi_ioc_transfer spi_transfer[1] {};

	spi_transfer[0].tx_buf = (uint64_t)send;
	spi_transfer[0].rx_buf = (uint64_t)recv;
	spi_transfer[0].len = len * 2;
	spi_transfer[0].speed_hz = _frequency;
	//spi_transfer[0].bits_per_word = 8;
	//spi_transfer[0].delay_usecs = 10;
	spi_transfer[0].cs_change = സത്യം;

	result = ::ioctl(_fd, SPI_IOC_MESSAGE(1), &spi_transfer);

	എങ്കിൽ (result != (പൂർണ്ണസംഖ്യ)(len * 2)) {
		PX4_ERR("write failed. Reported %d bytes written (%s)", result, strerror(errno));
		തിരികെ PX4_ERROR;
	}

	തിരികെ PX4_OK;
}

} // namespace device

#endif // __PX4_LINUX
#endif // CONFIG_SPI
