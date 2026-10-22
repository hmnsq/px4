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

#ifndef CONFIG_SPI_EXCHANGE
# error This driver requires CONFIG_SPI_EXCHANGE
#endif

نطاق device
{

SPI::SPI(uint8_t device_type, ثابت حرف *name, صحيح bus, uint32_t device, تعداد spi_mode_e mode, uint32_t frequency) :
	CDev(name, nullptr),
	_device(device),
	_mode(mode),
	_frequency(frequency)
{
	_device_id.devid_s.devtype = device_type;
	// fill in _device_id fields for a SPI device
	_device_id.devid_s.bus_type = DeviceBusType_SPI;
	_device_id.devid_s.bus = bus;
	// Use the 2. LSB byte as SPI address, which is non-zero for multiple instances of the same device on a bus
	_device_id.devid_s.address = (uint8_t)(device >> 8);

	إذا (!px4_spi_bus_requires_locking(bus)) {
		_locking_mode = LOCK_NONE;
	}
}

SPI::SPI(ثابت I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_index, config.module_name, config.bus, config.spi_devid, config.spi_mode,
	      config.bus_frequency)
{
	set_external(config.external);
}

SPI::~SPI()
{
	// XXX no way to let go of the bus...
}

صحيح
SPI::init()
{
	/* attach to the spi bus */
	إذا (_dev == nullptr) {
		صحيح bus = get_device_bus();

		إذا (!board_has_bus(BOARD_SPI_BUS, bus)) {
			إرجاع -ENOENT;
		}

		_dev = px4_spibus_initialize(bus);
	}

	إذا (_dev == nullptr) {
		DEVICE_DEBUG("failed to init SPI");
		إرجاع -ENOENT;
	}

	/* deselect device to ensure high to low transition of pin select */
	SPI_SELECT(_dev, _device, خطأ);

	/* call the probe function to check whether the device is present */
	صحيح ret = probe();

	إذا (ret != OK) {
		DEVICE_DEBUG("probe failed");
		إرجاع ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	إذا (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		إرجاع ret;
	}

	/* tell the world where we are */
	DEVICE_DEBUG("on SPI bus %d at %"  PRId32 " (%"  PRId32 " KHz)", get_device_bus(), PX4_SPI_DEV_ID(_device),
		     _frequency / 1000);

	إرجاع PX4_OK;
}

صحيح
SPI::transfer(uint8_t *send, uint8_t *recv, unsigned len)
{
	صحيح result;

	إذا ((send == nullptr) && (recv == nullptr)) {
		إرجاع -EINVAL;
	}

	LockMode mode = up_interrupt_context() ? LOCK_NONE : _locking_mode;

	/* lock the bus as required */
	بدل (mode) {
	افتراضي:
	حالة LOCK_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			result = _transfer(send, recv, len);
			px4_leave_critical_section(state);
		}
		اكسر;

	حالة LOCK_THREADS:
		SPI_LOCK(_dev, true);
		result = _transfer(send, recv, len);
		SPI_LOCK(_dev, خطأ);
		اكسر;

	حالة LOCK_NONE:
		result = _transfer(send, recv, len);
		اكسر;
	}

	إرجاع result;
}

صحيح
SPI::_transfer(uint8_t *send, uint8_t *recv, unsigned len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 8);
	SPI_SELECT(_dev, _device, true);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, خطأ);

	إرجاع PX4_OK;
}

صحيح
SPI::transferhword(uint16_t *send, uint16_t *recv, unsigned len)
{
	صحيح result;

	إذا ((send == nullptr) && (recv == nullptr)) {
		إرجاع -EINVAL;
	}

	LockMode mode = up_interrupt_context() ? LOCK_NONE : _locking_mode;

	/* lock the bus as required */
	بدل (mode) {
	افتراضي:
	حالة LOCK_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			result = _transferhword(send, recv, len);
			px4_leave_critical_section(state);
		}
		اكسر;

	حالة LOCK_THREADS:
		SPI_LOCK(_dev, true);
		result = _transferhword(send, recv, len);
		SPI_LOCK(_dev, خطأ);
		اكسر;

	حالة LOCK_NONE:
		result = _transferhword(send, recv, len);
		اكسر;
	}

	إرجاع result;
}

صحيح
SPI::_transferhword(uint16_t *send, uint16_t *recv, unsigned len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 16);			/* 16 bit transfer */
	SPI_SELECT(_dev, _device, true);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, خطأ);

	إرجاع PX4_OK;
}

} // namespace device
#endif // CONFIG_SPI
