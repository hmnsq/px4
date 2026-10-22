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

#pragma once

/**
 * @file SPI.hpp
 *
 * Base class for devices connected via SPI.
 */

#அந்தர்பாவ "../CDev.hpp"
#அந்தர்பாவ <px4_platform_common/spi.h>

#எனில் defined(CONFIG_SPI)

#ifdef __PX4_LINUX

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <sys/ioctl.h>
#அந்தர்பாவ <linux/types.h>
#அந்தர்பாவ <linux/spi/spidev.h>

எண்ணிக்கை spi_mode_e {
	SPIDEV_MODE0 = SPI_MODE_0, /* CPOL=0 CHPHA=0 */
	SPIDEV_MODE1 = SPI_MODE_1, /* CPOL=0 CHPHA=1 */
	SPIDEV_MODE2 = SPI_MODE_2, /* CPOL=1 CHPHA=0 */
	SPIDEV_MODE3 = SPI_MODE_3  /* CPOL=1 CHPHA=1 */
};

கட்டமைப்பு I2CSPIDriverConfig;

நாமாகாஶ device __EXPORT
{

/**
 * Abstract class for character device on SPI
 */
வகுப்பு __EXPORT SPI : பொது CDev
{
பொது:
	// no copy, assignment, move, move assignment
	SPI(மாறிலி SPI &) = delete;
	SPI &operator=(மாறிலி SPI &) = delete;
	SPI(SPI &&) = delete;
	SPI &operator=(SPI &&) = delete;

பாதுகாக்கப்பட்ட:
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
	SPI(uint8_t device_type, மாறிலி எழுத்து *name, முழு bus, uint32_t device, எண்ணிக்கை spi_mode_e mode, uint32_t frequency);

	SPI(மாறிலி I2CSPIDriverConfig &config);

	மெய்நிகர் ~SPI();

	/**
	 * Locking modes supported by the driver.
	 */
	எண்ணிக்கை LockMode {
		LOCK_PREEMPTION,	/**< the default; lock against all forms of preemption. */
		LOCK_THREADS,		/**< lock only against other threads, using SPI_LOCK */
		LOCK_NONE		/**< perform no locking, only safe if the bus is entirely private */
	};

	மெய்நிகர் முழு	init() override;

	/**
	 * Check for the presence of the device on the bus.
	 */
	மெய்நிகர் முழு	probe() { திரும்பு PX4_OK; }

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
	முழு		transfer(uint8_t *send, uint8_t *recv, unsigned len);

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
	முழு		transferhword(uint16_t *send, uint16_t *recv, unsigned len);

	/**
	 * Set the SPI bus frequency
	 * This is used to change frequency on the fly. Some sensors
	 * (such as the MPU6000) need a lower frequency for setup
	 * registers and can handle higher frequency for sensor
	 * value registers
	 *
	 * @param frequency	Frequency to set (Hz)
	 */
	வெறுமை		set_frequency(uint32_t frequency) { _frequency = frequency; }
	uint32_t	get_frequency() { திரும்பு _frequency; }

	/**
	 * Set the SPI bus locking mode
	 *
	 * This set the SPI locking mode. For devices competing with NuttX SPI
	 * drivers on a bus the right lock mode is LOCK_THREADS.
	 *
	 * @param mode	Locking mode
	 */
	வெறுமை		set_lockmode(எண்ணிக்கை LockMode mode) { _locking_mode = mode; }

தனிப்பட்ட:
	uint32_t		_device;
	எண்ணிக்கை spi_mode_e		_mode;
	uint32_t		_frequency;
	முழு 			_fd{-1};

	LockMode		_locking_mode{LOCK_THREADS};	/**< selected locking mode */

பாதுகாக்கப்பட்ட:
	முழு	_transfer(uint8_t *send, uint8_t *recv, unsigned len);

	முழு	_transferhword(uint16_t *send, uint16_t *recv, unsigned len);

};

} // namespace device

#இல்லையெனில்

எண்ணிக்கை spi_mode_e {
	SPIDEV_MODE0 = 0, /* CPOL=0 CHPHA=0 */
	SPIDEV_MODE1 = 1, /* CPOL=0 CHPHA=1 */
	SPIDEV_MODE2 = 2, /* CPOL=1 CHPHA=0 */
	SPIDEV_MODE3 = 3  /* CPOL=1 CHPHA=1 */
};
#endif // __PX4_LINUX

#endif // CONFIG_SPI
