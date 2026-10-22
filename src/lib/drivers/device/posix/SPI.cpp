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

#అన్తర్భావ "SPI.hpp"

#ఒకవేళ defined(CONFIG_SPI)

#నిర్వచితమైతే __PX4_LINUX

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <linux/types.h>
#అన్తర్భావ <linux/spi/spidev.h>

#అన్తర్భావ <px4_platform_common/i2c_spi_buses.h>
#అన్తర్భావ <px4_platform_common/px4_config.h>

నామాకాశ device
{

SPI::SPI(uint8_t device_type, స్థిరరాశి అక్షరం *name, పూర్ణాంకం bus, uint32_t device, గణన spi_బహుళకం_e బహుళకం, uint32_t frequency) :
	CDev(name, శూన్యసూచిక),
	_device(device),
	_mode(బహుళకం),
	_frequency(frequency)
{
	_device_id.devid_s.devtype = device_type;
	// fill in _device_id fields for a SPI device
	_device_id.devid_s.bus_type = DeviceBusType_SPI;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.చిరునామా = (uint8_t)device;
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
	// Open the actual SPI device
	అక్షరం dev_మార్గం[16];
	snprintf(dev_మార్గం, పరిమాణం(dev_మార్గం), "/dev/spidev%i.%i", get_device_bus(), PX4_SPI_DEV_ID(_device));
	DEVICE_DEBUG("%s", dev_మార్గం);
	_fd = ::open(dev_మార్గం, O_RDWR);

	ఒకవేళ (_fd < 0) {
		PX4_ERR("could not open %s", dev_మార్గం);
		తిరిగి PX4_ERROR;
	}

	/* call the probe function to check whether the device is present */
	పూర్ణాంకం ret = probe();

	ఒకవేళ (ret != OK) {
		DEVICE_DEBUG("probe failed");
		తిరిగి ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	ఒకవేళ (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		తిరిగి ret;
	}

	/* tell the world where we are */
	DEVICE_DEBUG("on SPI bus %d at %d (%u KHz)", get_device_bus(), PX4_SPI_DEV_ID(_device), _frequency / 1000);

	తిరిగి PX4_OK;
}

పూర్ణాంకం
SPI::transfer(uint8_t *send, uint8_t *recv, గుర్తులేని len)
{
	ఒకవేళ ((send == శూన్యసూచిక) && (recv == శూన్యసూచిక)) {
		తిరిగి -EINVAL;
	}

	// set write mode of SPI
	పూర్ణాంకం ఫలితం = ::ioctl(_fd, SPI_IOC_WR_బహుళకం, &_mode);

	ఒకవేళ (ఫలితం == -1) {
		PX4_ERR("can’t set spi mode");
		తిరిగి PX4_ERROR;
	}

	spi_ioc_transfer spi_transfer{};

	spi_transfer.tx_buf = (uint64_t)send;
	spi_transfer.rx_buf = (uint64_t)recv;
	spi_transfer.len = len;
	spi_transfer.వేగం_hz = _frequency;
	spi_transfer.బిట్లు_చొప్పున_మాట = 8;

	ఫలితం = ::ioctl(_fd, SPI_IOC_సందేశం(1), &spi_transfer);

	ఒకవేళ (ఫలితం != (పూర్ణాంకం)len) {
		PX4_ERR("write failed. Reported %d bytes written (%s)", ఫలితం, strerror(errno));
		తిరిగి PX4_ERROR;
	}

	తిరిగి PX4_OK;
}

పూర్ణాంకం
SPI::transferhword(uint16_t *send, uint16_t *recv, గుర్తులేని len)
{
	ఒకవేళ ((send == శూన్యసూచిక) && (recv == శూన్యసూచిక)) {
		తిరిగి -EINVAL;
	}

	// set write mode of SPI
	పూర్ణాంకం ఫలితం = ::ioctl(_fd, SPI_IOC_WR_బహుళకం, &_mode);

	ఒకవేళ (ఫలితం == -1) {
		PX4_ERR("can’t set spi mode");
		తిరిగి PX4_ERROR;
	}

	పూర్ణాంకం బిట్లు = 16;
	ఫలితం = ::ioctl(_fd, SPI_IOC_WR_బిట్లు_చొప్పున_మాట, &బిట్లు);

	ఒకవేళ (ఫలితం == -1) {
		PX4_ERR("can’t set 16 bit spi mode");
		తిరిగి PX4_ERROR;
	}

	spi_ioc_transfer spi_transfer[1] {};

	spi_transfer[0].tx_buf = (uint64_t)send;
	spi_transfer[0].rx_buf = (uint64_t)recv;
	spi_transfer[0].len = len * 2;
	spi_transfer[0].వేగం_hz = _frequency;
	//spi_transfer[0].bits_per_word = 8;
	//spi_transfer[0].delay_usecs = 10;
	spi_transfer[0].cs_change = నిజం;

	ఫలితం = ::ioctl(_fd, SPI_IOC_సందేశం(1), &spi_transfer);

	ఒకవేళ (ఫలితం != (పూర్ణాంకం)(len * 2)) {
		PX4_ERR("write failed. Reported %d bytes written (%s)", ఫలితం, strerror(errno));
		తిరిగి PX4_ERROR;
	}

	తిరిగి PX4_OK;
}

} // namespace device

#అయితే_ముగింపు // __PX4_LINUX
#అయితే_ముగింపు // CONFIG_SPI
