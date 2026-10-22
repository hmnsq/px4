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

#അന്തര്ഭാവ "SPI.hpp"

#എങ്കിൽ defined(CONFIG_SPI)

#അന്തര്ഭാവ <px4_platform_common/i2c_spi_buses.h>
#അന്തര്ഭാവ <px4_platform_common/px4_config.h>
#അന്തര്ഭാവ <nuttx/arch.h>

#ifndef CONFIG_SPI_EXCHANGE
# error This driver requires CONFIG_SPI_EXCHANGE
#endif

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
	// Use the 2. LSB byte as SPI address, which is non-zero for multiple instances of the same device on a bus
	_device_id.devid_s.address = (uint8_t)(device >> 8);

	എങ്കിൽ (!px4_spi_bus_requires_locking(bus)) {
		_locking_mode = LOCK_NONE;
	}
}

SPI::SPI(സ്ഥിരം I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_index, config.module_name, config.bus, config.spi_devid, config.spi_mode,
	      config.bus_frequency)
{
	set_external(config.external);
}

SPI::~SPI()
{
	// XXX no way to let go of the bus...
}

പൂർണ്ണസംഖ്യ
SPI::init()
{
	/* attach to the spi bus */
	എങ്കിൽ (_dev == nullptr) {
		പൂർണ്ണസംഖ്യ bus = get_device_bus();

		എങ്കിൽ (!board_has_bus(BOARD_SPI_BUS, bus)) {
			തിരികെ -ENOENT;
		}

		_dev = px4_spibus_initialize(bus);
	}

	എങ്കിൽ (_dev == nullptr) {
		DEVICE_DEBUG("failed to init SPI");
		തിരികെ -ENOENT;
	}

	/* deselect device to ensure high to low transition of pin select */
	SPI_SELECT(_dev, _device, അസത്യം);

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
	DEVICE_DEBUG("on SPI bus %d at %"  PRId32 " (%"  PRId32 " KHz)", get_device_bus(), PX4_SPI_DEV_ID(_device),
		     _frequency / 1000);

	തിരികെ PX4_OK;
}

പൂർണ്ണസംഖ്യ
SPI::transfer(uint8_t *send, uint8_t *recv, unsigned len)
{
	പൂർണ്ണസംഖ്യ result;

	എങ്കിൽ ((send == nullptr) && (recv == nullptr)) {
		തിരികെ -EINVAL;
	}

	LockMode mode = up_interrupt_context() ? LOCK_NONE : _locking_mode;

	/* lock the bus as required */
	തിരഞ്ഞെടുപ്പ് (mode) {
	സ്ഥിരസ്ഥിതി:
	അവസ്ഥ LOCK_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			result = _transfer(send, recv, len);
			px4_leave_critical_section(state);
		}
		നിർത്തുക;

	അവസ്ഥ LOCK_THREADS:
		SPI_LOCK(_dev, സത്യം);
		result = _transfer(send, recv, len);
		SPI_LOCK(_dev, അസത്യം);
		നിർത്തുക;

	അവസ്ഥ LOCK_NONE:
		result = _transfer(send, recv, len);
		നിർത്തുക;
	}

	തിരികെ result;
}

പൂർണ്ണസംഖ്യ
SPI::_transfer(uint8_t *send, uint8_t *recv, unsigned len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 8);
	SPI_SELECT(_dev, _device, സത്യം);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, അസത്യം);

	തിരികെ PX4_OK;
}

പൂർണ്ണസംഖ്യ
SPI::transferhword(uint16_t *send, uint16_t *recv, unsigned len)
{
	പൂർണ്ണസംഖ്യ result;

	എങ്കിൽ ((send == nullptr) && (recv == nullptr)) {
		തിരികെ -EINVAL;
	}

	LockMode mode = up_interrupt_context() ? LOCK_NONE : _locking_mode;

	/* lock the bus as required */
	തിരഞ്ഞെടുപ്പ് (mode) {
	സ്ഥിരസ്ഥിതി:
	അവസ്ഥ LOCK_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			result = _transferhword(send, recv, len);
			px4_leave_critical_section(state);
		}
		നിർത്തുക;

	അവസ്ഥ LOCK_THREADS:
		SPI_LOCK(_dev, സത്യം);
		result = _transferhword(send, recv, len);
		SPI_LOCK(_dev, അസത്യം);
		നിർത്തുക;

	അവസ്ഥ LOCK_NONE:
		result = _transferhword(send, recv, len);
		നിർത്തുക;
	}

	തിരികെ result;
}

പൂർണ്ണസംഖ്യ
SPI::_transferhword(uint16_t *send, uint16_t *recv, unsigned len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 16);			/* 16 bit transfer */
	SPI_SELECT(_dev, _device, സത്യം);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, അസത്യം);

	തിരികെ PX4_OK;
}

} // namespace device
#endif // CONFIG_SPI
