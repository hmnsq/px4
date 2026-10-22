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

#अन्तर्भाव "SPI.hpp"

#यदि defined(CONFIG_SPI)

#अन्तर्भाव <px4_platform_common/i2c_spi_buses.h>
#अन्तर्भाव <px4_platform_common/px4_config.h>
#अन्तर्भाव <nuttx/arch.h>

#यदि_अपरिभाषितम् CONFIG_SPI_EXCHANGE
# error This driver requires CONFIG_SPI_EXCHANGE
#यदि_अन्तः

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
	// Use the 2. LSB byte as SPI address, which is non-zero for multiple instances of the same device on a bus
	_device_id.devid_s.स्थानाङ्कः = (uint8_t)(device >> 8);

	यदि (!px4_spi_bus_requires_locking(bus)) {
		locking_बहुलकम् = तालकम्_NONE;
	}
}

SPI::SPI(ध्रुव I2CSPIDriverConfig &config)
	: SPI(config.devid_driver_सूचकांक, config.module_name, config.bus, config.spi_devid, config.spi_बहुलकम्,
	      config.bus_frequency)
{
	समुच्चयः_external(config.external);
}

SPI::~SPI()
{
	// XXX no way to let go of the bus...
}

पूर्णाङ्क
SPI::init()
{
	/* attach to the spi bus */
	यदि (_dev == शून्यसूचकः) {
		पूर्णाङ्क bus = get_device_bus();

		यदि (!board_has_bus(BOARD_SPI_BUS, bus)) {
			प्रत्यागम -ENOENT;
		}

		_dev = px4_spibus_initialize(bus);
	}

	यदि (_dev == शून्यसूचकः) {
		DEVICE_DEBUG("failed to init SPI");
		प्रत्यागम -ENOENT;
	}

	/* deselect device to ensure high to low transition of pin select */
	SPI_SELECT(_dev, _device, असत्य);

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
	DEVICE_DEBUG("on SPI bus %d at %"  PRId32 " (%"  PRId32 " KHz)", get_device_bus(), PX4_SPI_DEV_ID(_device),
		     _frequency / 1000);

	प्रत्यागम PX4_OK;
}

पूर्णाङ्क
SPI::transfer(uint8_t *send, uint8_t *recv, अचिह्नित len)
{
	पूर्णाङ्क परिणाम;

	यदि ((send == शून्यसूचकः) && (recv == शून्यसूचकः)) {
		प्रत्यागम -EINVAL;
	}

	तालकम्_बहुलकम् बहुलकम् = ऊर्ध्वम्_व्यवधानम्_context() ? तालकम्_NONE : locking_बहुलकम्;

	/* lock the bus as required */
	विकल्प (बहुलकम्) {
	स्वभाव:
	पक्ष तालकम्_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			परिणाम = _transfer(send, recv, len);
			px4_leave_critical_section(state);
		}
		भङ्ग;

	पक्ष तालकम्_तन्तवः:
		SPI_तालकम्(_dev, सत्य);
		परिणाम = _transfer(send, recv, len);
		SPI_तालकम्(_dev, असत्य);
		भङ्ग;

	पक्ष तालकम्_NONE:
		परिणाम = _transfer(send, recv, len);
		भङ्ग;
	}

	प्रत्यागम परिणाम;
}

पूर्णाङ्क
SPI::_transfer(uint8_t *send, uint8_t *recv, अचिह्नित len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 8);
	SPI_SELECT(_dev, _device, सत्य);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, असत्य);

	प्रत्यागम PX4_OK;
}

पूर्णाङ्क
SPI::transferhword(uint16_t *send, uint16_t *recv, अचिह्नित len)
{
	पूर्णाङ्क परिणाम;

	यदि ((send == शून्यसूचकः) && (recv == शून्यसूचकः)) {
		प्रत्यागम -EINVAL;
	}

	तालकम्_बहुलकम् बहुलकम् = ऊर्ध्वम्_व्यवधानम्_context() ? तालकम्_NONE : locking_बहुलकम्;

	/* lock the bus as required */
	विकल्प (बहुलकम्) {
	स्वभाव:
	पक्ष तालकम्_PREEMPTION: {
			irqstate_t state = px4_enter_critical_section();
			परिणाम = _transferhword(send, recv, len);
			px4_leave_critical_section(state);
		}
		भङ्ग;

	पक्ष तालकम्_तन्तवः:
		SPI_तालकम्(_dev, सत्य);
		परिणाम = _transferhword(send, recv, len);
		SPI_तालकम्(_dev, असत्य);
		भङ्ग;

	पक्ष तालकम्_NONE:
		परिणाम = _transferhword(send, recv, len);
		भङ्ग;
	}

	प्रत्यागम परिणाम;
}

पूर्णाङ्क
SPI::_transferhword(uint16_t *send, uint16_t *recv, अचिह्नित len)
{
	SPI_SETFREQUENCY(_dev, _frequency);
	SPI_SETMODE(_dev, _mode);
	SPI_SETBITS(_dev, 16);			/* 16 bit transfer */
	SPI_SELECT(_dev, _device, सत्य);

	/* do the transfer */
	SPI_EXCHANGE(_dev, send, recv, len);

	/* and clean up */
	SPI_SELECT(_dev, _device, असत्य);

	प्रत्यागम PX4_OK;
}

} // namespace device
#यदि_अन्तः // CONFIG_SPI
