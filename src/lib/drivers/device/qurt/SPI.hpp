/****************************************************************************
 *
 *   Copyright (C) 2019 PX4 Development Team. All rights reserved.
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
 * @file SPI.hpp
 *
 * Base class for devices connected via SPI.
 */

#pragma once

#અન્તર્ભાવ "../CDev.hpp"

// #include "dev_fs_lib_spi.h"

#અન્તર્ભાવ <px4_platform_common/spi.h>

// Perhaps not the best place for these but they are only used for the IMU SPI driver on Qurt
પૂર્ણાંક px4_arch_gpiosetevent(spi_drdy_gpio_t pin, તાર્કિક r, તાર્કિક f, તાર્કિક e, પૂર્ણાંક (*func)(પૂર્ણાંક, શૂન્ય *, શૂન્ય *), શૂન્ય *arg);
શૂન્ય register_interrupt_callback_initalizer(પૂર્ણાંક (*)(પૂર્ણાંક (*)(પૂર્ણાંક, શૂન્ય *, શૂન્ય *), શૂન્ય *arg));

ગણના spi_mode_e {
	SPIDEV_MODE0 = 0, /* CPOL=0 CHPHA=0 */
	SPIDEV_MODE1 = 1, /* CPOL=0 CHPHA=1 */
	SPIDEV_MODE2 = 2, /* CPOL=1 CHPHA=0 */
	SPIDEV_MODE3 = 3  /* CPOL=1 CHPHA=1 */
};

સંરચના I2CSPIDriverConfig;

નામાકાશ device __EXPORT
{

/**
 * Abstract class for character device on SPI
 */
વર્ગ __EXPORT SPI : જાહેર CDev
{
જાહેર:
	પ્રકારવ્યાખ્યા પૂર્ણાંક (*_config_spi_bus_func_t)();
	પ્રકારવ્યાખ્યા પૂર્ણાંક (*_spi_transfer_func_t)(પૂર્ણાંક, અચલ uint8_t *, uint8_t *, અચલ unsigned);

	સ્થિર શૂન્ય configure_callbacks(_config_spi_bus_func_t config_func,
					_spi_transfer_func_t transfer_func)
	{
		_config_spi_bus = config_func;
		_spi_transfer = transfer_func;
	}

સુરક્ષિત:
	/**
	 * Constructor
	 *
	 * @param device_type	The device type (see drv_sensor.h)
	 * @param name		Driver name
	 * @param bus		SPI bus on which the device lives
	 * @param device	Device handle (used by SPI_SELECT)
	 * @param mode		SPI clock/data mode
	 * @param frequency	SPI clock frequency
	 */
	SPI(uint8_t device_type, અચલ અક્ષર *name, પૂર્ણાંક bus, uint32_t device, ગણના spi_mode_e mode, uint32_t frequency);

	SPI(અચલ I2CSPIDriverConfig &config);

	આભાસી ~SPI();

	/**
	 * Locking modes supported by the driver.
	 */
	ગણના LockMode {
		LOCK_PREEMPTION,	/**< the default; lock against all forms of preemption. */
		LOCK_THREADS,		/**< lock only against other threads, using SPI_LOCK */
		LOCK_NONE		/**< perform no locking, only safe if the bus is entirely private */
	};

	આભાસી પૂર્ણાંક	init();

	/**
	 * Check for the presence of the device on the bus.
	 */
	આભાસી પૂર્ણાંક	probe() { પાછા PX4_OK; }

	/**
	 * Perform a SPI transfer.
	 *
	 * If called from interrupt context, this interface does not lock
	 * the bus and may interfere with non-interrupt-context callers.
	 *
	 * Clients in a mixed interrupt/non-interrupt configuration must
	 * ensure appropriate interlocking.
	 *
	 * At least one of send or recv must be non-null.
	 *
	 * @param send		Bytes to send to the device, or nullptr if
	 *			no data is to be sent.
	 * @param recv		Buffer for receiving bytes from the device,
	 *			or nullptr if no bytes are to be received.
	 * @param len		Number of bytes to transfer.
	 * @return		OK if the exchange was successful, -errno
	 *			otherwise.
	 */
	પૂર્ણાંક		transfer(uint8_t *send, uint8_t *recv, unsigned len);

	/**
	 * Perform a SPI 16 bit transfer.
	 *
	 * If called from interrupt context, this interface does not lock
	 * the bus and may interfere with non-interrupt-context callers.
	 *
	 * Clients in a mixed interrupt/non-interrupt configuration must
	 * ensure appropriate interlocking.
	 *
	 * At least one of send or recv must be non-null.
	 *
	 * @param send		Words to send to the device, or nullptr if
	 *			no data is to be sent.
	 * @param recv		Words for receiving bytes from the device,
	 *			or nullptr if no bytes are to be received.
	 * @param len		Number of words to transfer.
	 * @return		OK if the exchange was successful, -errno
	 *			otherwise.
	 */
	પૂર્ણાંક		transferhword(uint16_t *send, uint16_t *recv, unsigned len);

	/**
	 * Set the SPI bus frequency
	 * This is used to change frequency on the fly. Some sensors
	 * (such as the MPU6000) need a lower frequency for setup
	 * registers and can handle higher frequency for sensor
	 * value registers
	 *
	 * @param frequency	Frequency to set (Hz)
	 */
	શૂન્ય		set_frequency(uint32_t frequency) {}
	uint32_t	get_frequency() { પાછા 0; }

	/**
	 * Set the SPI bus locking mode
	 *
	 * This set the SPI locking mode. For devices competing with NuttX SPI
	 * drivers on a bus the right lock mode is LOCK_THREADS.
	 *
	 * @param mode	Locking mode
	 */
	શૂન્ય		set_lockmode(ગણના LockMode mode) {}

ખાનગી:
	પૂર્ણાંક 			_fd{-1};

	સ્થિર _config_spi_bus_func_t  _config_spi_bus;
	સ્થિર _spi_transfer_func_t    _spi_transfer;

	સ્થિર pthread_mutex_t         _mutex;

સુરક્ષિત:

	/**
	 * The number of times a read or write operation will be retried on
	 * error.
	 */
	uint8_t		_retries{0};

};

} // namespace device
