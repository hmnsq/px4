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

#ضمن "SPI.hpp"

#إذا defined(CONFIG_SPI)

#ضمن <px4_platform_common/i2c_spi_buses.h>
#ضمن <px4_platform_common/px4_config.h>
#ضمن <nuttx/arch.h>

#که_نه_تعریف_شوی CONFIG_SPI_EXCHANGE
# error This driver requires CONFIG_SPI_EXCHANGE
#د_که_پای

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
	// Use the 2. LSB byte as SPI address, which is non-zero for multiple instances of the same device on a bus
	_device_id.devid_s.پته = (uint8_t)(device >> 8);

	إذا (!px4_spi_bus_requires_locking(bus)) {
		locking_ډېرپېښ = قلف_NONE;
	}
}

SPI::SPI(ثابت I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_شاخص, config.module_name, config.bus, config.spi_devid, config.spi_ډېرپېښ,
	      config.bus_frequency)
{
	سېټ_external(config.external);
}

SPI::~SPI()
{
	// XXX no way to let go of the bus...
}

صحيح
SPI::init()
{
	/* attach to the spi bus */
	إذا (_dev == تش_اشاره) {
		صحيح bus = get_device_bus();

		إذا (!board_has_bus(BOARD_SPI_BUS, bus)) {
			أرجع -ENOENT;
		}

		_dev = px4_spibus_initialize(bus);
	}

	إذا (_dev == تش_اشاره) {
		DEVICE_DEBUG("failed to init SPI");
		أرجع -ENOENT;
	}

	/* deselect device to ensure high to low transition of pin select */
	SPI_SELECT(_dev, _device, خطأ);

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
	DEVICE_DEBUG("on SPI bus %d at %"  PRId32 " (%"  PRId32 " KHz)", get_device_bus(), PX4_SPI_DEV_ID(_device),
		     _frequency / 1000);

	أرجع PX4_OK;
}

صحيح
SPI::transfer(uint8_t *send, uint8_t *recv, بینښه len)
{
	صحيح پایله;

	إذا ((send == تش_اشاره) && (recv == تش_اشاره)) {
		أرجع -EINVAL;
	}

	قلف_ډېرپېښ ډېرپېښ = پورته_مداخله_context() ? قلف_NONE : locking_ډېرپېښ;

	/* lock the bus as required */
	بدل (ډېرپېښ) {
	افتراضي:
	حالة قلف_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			پایله = _transfer(send, recv, len);
			px4_leave_critical_section(state);
		}
		اكسر;

	حالة قلف_تارونه:
		SPI_قلف(_dev, ریښتیا);
		پایله = _transfer(send, recv, len);
		SPI_قلف(_dev, خطأ);
		اكسر;

	حالة قلف_NONE:
		پایله = _transfer(send, recv, len);
		اكسر;
	}

	أرجع پایله;
}

صحيح
SPI::_transfer(uint8_t *send, uint8_t *recv, بینښه len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 8);
	SPI_SELECT(_dev, _device, ریښتیا);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, خطأ);

	أرجع PX4_OK;
}

صحيح
SPI::transferhword(uint16_t *send, uint16_t *recv, بینښه len)
{
	صحيح پایله;

	إذا ((send == تش_اشاره) && (recv == تش_اشاره)) {
		أرجع -EINVAL;
	}

	قلف_ډېرپېښ ډېرپېښ = پورته_مداخله_context() ? قلف_NONE : locking_ډېرپېښ;

	/* lock the bus as required */
	بدل (ډېرپېښ) {
	افتراضي:
	حالة قلف_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			پایله = _transferhword(send, recv, len);
			px4_leave_critical_section(state);
		}
		اكسر;

	حالة قلف_تارونه:
		SPI_قلف(_dev, ریښتیا);
		پایله = _transferhword(send, recv, len);
		SPI_قلف(_dev, خطأ);
		اكسر;

	حالة قلف_NONE:
		پایله = _transferhword(send, recv, len);
		اكسر;
	}

	أرجع پایله;
}

صحيح
SPI::_transferhword(uint16_t *send, uint16_t *recv, بینښه len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 16);			/* 16 bit transfer */
	SPI_SELECT(_dev, _device, ریښتیا);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, خطأ);

	أرجع PX4_OK;
}

} // namespace device
#د_که_پای // CONFIG_SPI
