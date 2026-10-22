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

#ਅਨ੍ਤਰ੍ਭਾਵ "SPI.hpp"

#ਜੇ defined(CONFIG_SPI)

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/i2c_spi_buses.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <nuttx/arch.h>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_SPI_EXCHANGE
# error This driver requires CONFIG_SPI_EXCHANGE
#ਜੇ_ਅੰਤ

ਨਾਮਾਕਾਸ਼ device
{

SPI::SPI(uint8_t device_type, ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ bus, uint32_t device, ਗਿਣਤੀ spi_ਬਹੁਲਕ_e ਬਹੁਲਕ, uint32_t frequency) :
	CDev(name, ਖਾਲੀ_ਸੂਚਕ),
	_device(device),
	_mode(ਬਹੁਲਕ),
	_frequency(frequency)
{
	_device_id.devid_s.devtype = device_type;
	// fill in _device_id fields for a SPI device
	_device_id.devid_s.bus_type = DeviceBusType_SPI;
	_device_id.devid_s.bus = bus;
	// Use the 2. LSB byte as SPI address, which is non-zero for multiple instances of the same device on a bus
	_device_id.devid_s.ਪਤਾ = (uint8_t)(device >> 8);

	ਜੇ (!px4_spi_bus_requires_locking(bus)) {
		locking_ਬਹੁਲਕ = ਤਾਲਾ_NONE;
	}
}

SPI::SPI(ਅਚਲ I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_ਸੂਚਕਾਂਕ, config.module_name, config.bus, config.spi_devid, config.spi_ਬਹੁਲਕ,
	      config.bus_frequency)
{
	ਸੈੱਟ_external(config.external);
}

SPI::~SPI()
{
	// XXX no way to let go of the bus...
}

ਪੂਰਨ
SPI::init()
{
	/* attach to the spi bus */
	ਜੇ (_dev == ਖਾਲੀ_ਸੂਚਕ) {
		ਪੂਰਨ bus = get_device_bus();

		ਜੇ (!board_has_bus(BOARD_SPI_BUS, bus)) {
			ਵਾਪਸ -ENOENT;
		}

		_dev = px4_spibus_initialize(bus);
	}

	ਜੇ (_dev == ਖਾਲੀ_ਸੂਚਕ) {
		DEVICE_DEBUG("failed to init SPI");
		ਵਾਪਸ -ENOENT;
	}

	/* deselect device to ensure high to low transition of pin select */
	SPI_SELECT(_dev, _device, ਝੂਠ);

	/* call the probe function to check whether the device is present */
	ਪੂਰਨ ret = probe();

	ਜੇ (ret != OK) {
		DEVICE_DEBUG("probe failed");
		ਵਾਪਸ ret;
	}

	/* do base class init, which will create the device node, etc. */
	ret = CDev::init();

	ਜੇ (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		ਵਾਪਸ ret;
	}

	/* tell the world where we are */
	DEVICE_DEBUG("on SPI bus %d at %"  PRId32 " (%"  PRId32 " KHz)", get_device_bus(), PX4_SPI_DEV_ID(_device),
		     _frequency / 1000);

	ਵਾਪਸ PX4_OK;
}

ਪੂਰਨ
SPI::transfer(uint8_t *send, uint8_t *recv, ਬਿਨਾਨਿਸ਼ਾਨੀ len)
{
	ਪੂਰਨ ਨਤੀਜਾ;

	ਜੇ ((send == ਖਾਲੀ_ਸੂਚਕ) && (recv == ਖਾਲੀ_ਸੂਚਕ)) {
		ਵਾਪਸ -EINVAL;
	}

	ਤਾਲਾ_ਬਹੁਲਕ ਬਹੁਲਕ = ਉੱਪਰ_ਰੁਕਾਵਟ_context() ? ਤਾਲਾ_NONE : locking_ਬਹੁਲਕ;

	/* lock the bus as required */
	ਚੋਣ (ਬਹੁਲਕ) {
	ਮੂਲ:
	ਹਾਲਤ ਤਾਲਾ_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			ਨਤੀਜਾ = _transfer(send, recv, len);
			px4_leave_critical_section(state);
		}
		ਰੋਕੋ;

	ਹਾਲਤ ਤਾਲਾ_ਧਾਗੇ:
		SPI_ਤਾਲਾ(_dev, ਸੱਚ);
		ਨਤੀਜਾ = _transfer(send, recv, len);
		SPI_ਤਾਲਾ(_dev, ਝੂਠ);
		ਰੋਕੋ;

	ਹਾਲਤ ਤਾਲਾ_NONE:
		ਨਤੀਜਾ = _transfer(send, recv, len);
		ਰੋਕੋ;
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ
SPI::_transfer(uint8_t *send, uint8_t *recv, ਬਿਨਾਨਿਸ਼ਾਨੀ len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 8);
	SPI_SELECT(_dev, _device, ਸੱਚ);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, ਝੂਠ);

	ਵਾਪਸ PX4_OK;
}

ਪੂਰਨ
SPI::transferhword(uint16_t *send, uint16_t *recv, ਬਿਨਾਨਿਸ਼ਾਨੀ len)
{
	ਪੂਰਨ ਨਤੀਜਾ;

	ਜੇ ((send == ਖਾਲੀ_ਸੂਚਕ) && (recv == ਖਾਲੀ_ਸੂਚਕ)) {
		ਵਾਪਸ -EINVAL;
	}

	ਤਾਲਾ_ਬਹੁਲਕ ਬਹੁਲਕ = ਉੱਪਰ_ਰੁਕਾਵਟ_context() ? ਤਾਲਾ_NONE : locking_ਬਹੁਲਕ;

	/* lock the bus as required */
	ਚੋਣ (ਬਹੁਲਕ) {
	ਮੂਲ:
	ਹਾਲਤ ਤਾਲਾ_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			ਨਤੀਜਾ = _transferhword(send, recv, len);
			px4_leave_critical_section(state);
		}
		ਰੋਕੋ;

	ਹਾਲਤ ਤਾਲਾ_ਧਾਗੇ:
		SPI_ਤਾਲਾ(_dev, ਸੱਚ);
		ਨਤੀਜਾ = _transferhword(send, recv, len);
		SPI_ਤਾਲਾ(_dev, ਝੂਠ);
		ਰੋਕੋ;

	ਹਾਲਤ ਤਾਲਾ_NONE:
		ਨਤੀਜਾ = _transferhword(send, recv, len);
		ਰੋਕੋ;
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ
SPI::_transferhword(uint16_t *send, uint16_t *recv, ਬਿਨਾਨਿਸ਼ਾਨੀ len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 16);			/* 16 bit transfer */
	SPI_SELECT(_dev, _device, ਸੱਚ);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, ਝੂਠ);

	ਵਾਪਸ PX4_OK;
}

} // namespace device
#ਜੇ_ਅੰਤ // CONFIG_SPI
