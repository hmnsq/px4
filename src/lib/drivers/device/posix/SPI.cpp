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

#अन्तर्भाव "SPI.hpp"

#यदि defined(CONFIG_SPI)

#यदि_परिभाषितम् __PX4_LINUX

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <linux/types.h>
#अन्तर्भाव <linux/spi/spidev.h>

#अन्तर्भाव <px4_platform_common/i2c_spi_buses.h>
#अन्तर्भाव <px4_platform_common/px4_config.h>

नामाकाश device
{

SPI::SPI(uint8_t device_type, ध्रुव वर्ण *name, पूर्णाङ्क bus, uint32_t device, गणना spi_बहुलकम्_e बहुलकम्, uint32_t frequency) :
	CDev(name, शून्यसूचकः),
	_device(device),
	_mode(बहुलकम्),
	_frequency(frequency)
{
	_device_id.devid_s.devtype = device_type;
	// fill in _device_id fields for a SPI device
	_device_id.devid_s.bus_type = DeviceBusType_SPI;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.स्थानाङ्कः = (uint8_t)device;
}

SPI::SPI(ध्रुव I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_सूचकांक, config.module_name, config.bus, config.spi_devid, config.spi_बहुलकम्,
	      config.bus_frequency)
{
	समुच्चयः_external(config.external);
}

SPI::~SPI()
{
	यदि (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}

पूर्णाङ्क
SPI::init()
{
	// Open the actual SPI device
	वर्ण dev_मार्गः[16];
	snprintf(dev_मार्गः, परिमाण(dev_मार्गः), "/dev/spidev%i.%i", get_device_bus(), PX4_SPI_DEV_ID(_device));
	DEVICE_DEBUG("%s", dev_मार्गः);
	_fd = ::उद्घाटय(dev_मार्गः, O_RDWR);

	यदि (_fd < 0) {
		PX4_ERR("could not open %s", dev_मार्गः);
		प्रत्यागम PX4_ERROR;
	}

	/* call the probe function to check whether the device is present */
	पूर्णाङ्क ret = probe();

	यदि (ret != OK) {
		DEVICE_DEBUG("probe failed");
		प्रत्यागम ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	यदि (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		प्रत्यागम ret;
	}

	/* tell the world where we are */
	DEVICE_DEBUG("on SPI bus %d at %d (%u KHz)", get_device_bus(), PX4_SPI_DEV_ID(_device), _frequency / 1000);

	प्रत्यागम PX4_OK;
}

पूर्णाङ्क
SPI::transfer(uint8_t *send, uint8_t *recv, अचिह्नित len)
{
	यदि ((send == शून्यसूचकः) && (recv == शून्यसूचकः)) {
		प्रत्यागम -EINVAL;
	}

	// set write mode of SPI
	पूर्णाङ्क परिणाम = ::ioctl(_fd, SPI_IOC_WR_बहुलकम्, &_mode);

	यदि (परिणाम == -1) {
		PX4_ERR("can’t set spi mode");
		प्रत्यागम PX4_ERROR;
	}

	spi_ioc_transfer spi_transfer{};

	spi_transfer.tx_buf = (uint64_t)send;
	spi_transfer.rx_buf = (uint64_t)recv;
	spi_transfer.len = len;
	spi_transfer.जवः_hz = _frequency;
	spi_transfer.द्वयङ्काः_प्रत्येकम्_word = 8;

	परिणाम = ::ioctl(_fd, SPI_IOC_सन्देशः(1), &spi_transfer);

	यदि (परिणाम != (पूर्णाङ्क)len) {
		PX4_ERR("write failed. Reported %d bytes written (%s)", परिणाम, strerror(errno));
		प्रत्यागम PX4_ERROR;
	}

	प्रत्यागम PX4_OK;
}

पूर्णाङ्क
SPI::transferhword(uint16_t *send, uint16_t *recv, अचिह्नित len)
{
	यदि ((send == शून्यसूचकः) && (recv == शून्यसूचकः)) {
		प्रत्यागम -EINVAL;
	}

	// set write mode of SPI
	पूर्णाङ्क परिणाम = ::ioctl(_fd, SPI_IOC_WR_बहुलकम्, &_mode);

	यदि (परिणाम == -1) {
		PX4_ERR("can’t set spi mode");
		प्रत्यागम PX4_ERROR;
	}

	पूर्णाङ्क द्वयङ्काः = 16;
	परिणाम = ::ioctl(_fd, SPI_IOC_WR_द्वयङ्काः_प्रत्येकम्_WORD, &द्वयङ्काः);

	यदि (परिणाम == -1) {
		PX4_ERR("can’t set 16 bit spi mode");
		प्रत्यागम PX4_ERROR;
	}

	spi_ioc_transfer spi_transfer[1] {};

	spi_transfer[0].tx_buf = (uint64_t)send;
	spi_transfer[0].rx_buf = (uint64_t)recv;
	spi_transfer[0].len = len * 2;
	spi_transfer[0].जवः_hz = _frequency;
	//spi_transfer[0].bits_per_word = 8;
	//spi_transfer[0].delay_usecs = 10;
	spi_transfer[0].cs_change = सत्य;

	परिणाम = ::ioctl(_fd, SPI_IOC_सन्देशः(1), &spi_transfer);

	यदि (परिणाम != (पूर्णाङ्क)(len * 2)) {
		PX4_ERR("write failed. Reported %d bytes written (%s)", परिणाम, strerror(errno));
		प्रत्यागम PX4_ERROR;
	}

	प्रत्यागम PX4_OK;
}

} // namespace device

#यदि_अन्तः // __PX4_LINUX
#यदि_अन्तः // CONFIG_SPI
