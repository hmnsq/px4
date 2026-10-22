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

#pragma once

/**
 * @file SPI.hpp
 *
 * Base class for devices connected via SPI.
 */

#ಅನ್ತರ್ಭಾವ "../CDev.hpp"
#ಅನ್ತರ್ಭಾವ <px4_platform_common/spi.h>

#ಯದಿ defined(CONFIG_SPI)

#ಅನ್ತರ್ಭಾವ <nuttx/spi/spi.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/spi.h>

ಸಂರಚನಾ I2CSPIDriverConfig;

ನಾಮಾಕಾಶ device __EXPORT
{

/**
 * Abstract class for character device on SPI
 */
ವರ್ಗ __EXPORT SPI : ಸಾರ್ವಜನಿಕ CDev
{
ಸಾರ್ವಜನಿಕ:
	// no copy, assignment, move, move assignment
	SPI(ಧ್ರುವ SPI &) = delete;
	SPI &operator=(ಧ್ರುವ SPI &) = delete;
	SPI(SPI &&) = delete;
	SPI &operator=(SPI &&) = delete;

ರಕ್ಷಿತ:
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
	SPI(uint8_t device_type, ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ bus, uint32_t device, ಗಣನಾ spi_mode_e mode, uint32_t frequency);

	SPI(ಧ್ರುವ I2CSPIDriverConfig &config);

	ಆಭಾಸ ~SPI();

	/**
	 * Locking modes supported by the driver.
	 */
	ಗಣನಾ LockMode {
		LOCK_PREEMPTION,	/**< the default; lock against all forms of preemption. */
		LOCK_THREADS,		/**< lock only against other threads, using SPI_LOCK */
		LOCK_NONE		/**< perform no locking, only safe if the bus is entirely private */
	};

	ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ	init() override;

	/**
	 * Check for the presence of the device on the bus.
	 */
	ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ	probe() { ಪ್ರತ್ಯಾಗಮ PX4_OK; }

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
	ಪೂರ್ಣಾಙ್ಕ		transfer(uint8_t *send, uint8_t *recv, unsigned len);

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
	ಪೂರ್ಣಾಙ್ಕ		transferhword(uint16_t *send, uint16_t *recv, unsigned len);

	/**
	 * Set the SPI bus frequency
	 * This is used to change frequency on the fly. Some sensors
	 * (such as the MPU6000) need a lower frequency for setup
	 * registers and can handle higher frequency for sensor
	 * value registers
	 *
	 * @param frequency	Frequency to set (Hz)
	 */
	ಶೂನ್ಯ		set_frequency(uint32_t frequency) { _frequency = frequency; }
	uint32_t	get_frequency() { ಪ್ರತ್ಯಾಗಮ _frequency; }

	/**
	 * Set the SPI bus locking mode
	 *
	 * This set the SPI locking mode. For devices competing with NuttX SPI
	 * drivers on a bus the right lock mode is LOCK_THREADS.
	 *
	 * @param mode	Locking mode
	 */
	ಶೂನ್ಯ		set_lockmode(ಗಣನಾ LockMode mode) { _locking_mode = mode; }

ಗುಪ್ತ:
	uint32_t		_device;
	ಗಣನಾ spi_mode_e		_mode;
	uint32_t		_frequency;
	ಸಂರಚನಾ spi_dev_s	*_dev {nullptr};

	LockMode		_locking_mode{LOCK_THREADS};	/**< selected locking mode */

ರಕ್ಷಿತ:
	ಪೂರ್ಣಾಙ್ಕ	_transfer(uint8_t *send, uint8_t *recv, unsigned len);

	ಪೂರ್ಣಾಙ್ಕ	_transferhword(uint16_t *send, uint16_t *recv, unsigned len);

};

} // namespace device

#endif // CONFIG_SPI
