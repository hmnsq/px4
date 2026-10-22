/****************************************************************************
 *
 *   Copyright (C) 2012-2021 PX4 Development Team. All rights reserved.
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

#అన్తర్భావ <px4_platform_common/i2c_spi_buses.h>
#అన్తర్భావ <px4_platform_common/px4_config.h>
#అన్తర్భావ <nuttx/arch.h>

#నిర్వచితం_కాకపోతే CONFIG_SPI_EXCHANGE
# error This driver requires CONFIG_SPI_EXCHANGE
#అయితే_ముగింపు

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
	// Use the 2. LSB byte as SPI address, which is non-zero for multiple instances of the same device on a bus
	_device_id.devid_s.చిరునామా = (uint8_t)(device >> 8);

	ఒకవేళ (!px4_spi_bus_requires_locking(bus)) {
		locking_బహుళకం = తాళం_NONE;
	}
}

SPI::SPI(స్థిరరాశి I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_సూచీ, config.module_name, config.bus, config.spi_devid, config.spi_బహుళకం,
	      config.bus_frequency)
{
	సమితి_external(config.external);
}

SPI::~SPI()
{
	// XXX no way to let go of the bus...
}

పూర్ణాంకం
SPI::init()
{
	/* attach to the spi bus */
	ఒకవేళ (_dev == శూన్యసూచిక) {
		పూర్ణాంకం bus = get_device_bus();

		ఒకవేళ (!board_has_bus(BOARD_SPI_BUS, bus)) {
			తిరిగి -ENOENT;
		}

		_dev = px4_spibus_initialize(bus);
	}

	ఒకవేళ (_dev == శూన్యసూచిక) {
		DEVICE_DEBUG("failed to init SPI");
		తిరిగి -ENOENT;
	}

	/* deselect device to ensure high to low transition of pin select */
	SPI_SELECT(_dev, _device, అబద్ధం);

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
	DEVICE_DEBUG("on SPI bus %d at %"  PRId32 " (%"  PRId32 " KHz)", get_device_bus(), PX4_SPI_DEV_ID(_device),
		     _frequency / 1000);

	తిరిగి PX4_OK;
}

పూర్ణాంకం
SPI::transfer(uint8_t *send, uint8_t *recv, గుర్తులేని len)
{
	పూర్ణాంకం ఫలితం;

	ఒకవేళ ((send == శూన్యసూచిక) && (recv == శూన్యసూచిక)) {
		తిరిగి -EINVAL;
	}

	తాళం_బహుళకం బహుళకం = పైకి_అంతరాయం_context() ? తాళం_NONE : locking_బహుళకం;

	/* lock the bus as required */
	ఎంపిక (బహుళకం) {
	అప్రమేయ:
	స్థితి తాళం_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			ఫలితం = _transfer(send, recv, len);
			px4_leave_critical_section(state);
		}
		ఆపు;

	స్థితి తాళం_దారాలు:
		SPI_తాళం(_dev, నిజం);
		ఫలితం = _transfer(send, recv, len);
		SPI_తాళం(_dev, అబద్ధం);
		ఆపు;

	స్థితి తాళం_NONE:
		ఫలితం = _transfer(send, recv, len);
		ఆపు;
	}

	తిరిగి ఫలితం;
}

పూర్ణాంకం
SPI::_transfer(uint8_t *send, uint8_t *recv, గుర్తులేని len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 8);
	SPI_SELECT(_dev, _device, నిజం);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, అబద్ధం);

	తిరిగి PX4_OK;
}

పూర్ణాంకం
SPI::transferhword(uint16_t *send, uint16_t *recv, గుర్తులేని len)
{
	పూర్ణాంకం ఫలితం;

	ఒకవేళ ((send == శూన్యసూచిక) && (recv == శూన్యసూచిక)) {
		తిరిగి -EINVAL;
	}

	తాళం_బహుళకం బహుళకం = పైకి_అంతరాయం_context() ? తాళం_NONE : locking_బహుళకం;

	/* lock the bus as required */
	ఎంపిక (బహుళకం) {
	అప్రమేయ:
	స్థితి తాళం_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			ఫలితం = _transferhword(send, recv, len);
			px4_leave_critical_section(state);
		}
		ఆపు;

	స్థితి తాళం_దారాలు:
		SPI_తాళం(_dev, నిజం);
		ఫలితం = _transferhword(send, recv, len);
		SPI_తాళం(_dev, అబద్ధం);
		ఆపు;

	స్థితి తాళం_NONE:
		ఫలితం = _transferhword(send, recv, len);
		ఆపు;
	}

	తిరిగి ఫలితం;
}

పూర్ణాంకం
SPI::_transferhword(uint16_t *send, uint16_t *recv, గుర్తులేని len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 16);			/* 16 bit transfer */
	SPI_SELECT(_dev, _device, నిజం);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, అబద్ధం);

	తిరిగి PX4_OK;
}

} // namespace device
#అయితే_ముగింపు // CONFIG_SPI
