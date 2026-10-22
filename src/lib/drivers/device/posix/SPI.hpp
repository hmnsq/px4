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

#निर्देशः once

/**
 * @file SPI.hpp
 *
 * Base class for devices connected via SPI.
 */

#अन्तर्भाव "../CDev.hpp"
#अन्तर्भाव <px4_platform_common/spi.h>

#यदि defined(CONFIG_SPI)

#यदि_परिभाषितम् __PX4_LINUX

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <linux/types.h>
#अन्तर्भाव <linux/spi/spidev.h>

गणना spi_बहुलकम्_e {
	SPIDEV_MODE0 = SPI_बहुलकम्_0, /* CPOL=0 CHPHA=0 */
	SPIDEV_MODE1 = SPI_बहुलकम्_1, /* CPOL=0 CHPHA=1 */
	SPIDEV_MODE2 = SPI_बहुलकम्_2, /* CPOL=1 CHPHA=0 */
	SPIDEV_MODE3 = SPI_बहुलकम्_3  /* CPOL=1 CHPHA=1 */
};

संरचना I2CSPIDriverConfig;

नामाकाश device __EXPORT
{

/**
 * Abstract class for character device on SPI
 */
वर्ग __EXPORT SPI : सार्वजनिक CDev
{
सार्वजनिक:
	// no copy, assignment, move, move assignment
	SPI(ध्रुव SPI &) = विलोपय;
	SPI &संक्रियकः=(ध्रुव SPI &) = विलोपय;
	SPI(SPI &&) = विलोपय;
	SPI &संक्रियकः=(SPI &&) = विलोपय;

रक्षित:
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
	SPI(uint8_t device_type, ध्रुव वर्ण *name, पूर्णाङ्क bus, uint32_t device, गणना spi_बहुलकम्_e बहुलकम्, uint32_t frequency);

	SPI(ध्रुव I2CSPIDriverConfig &config);

	आभास ~SPI();

	/**
	 * Locking modes supported by the driver.
	 */
	गणना तालकम्_बहुलकम् {
		तालकम्_PREEMPTION,	/**< the default; lock against all forms of preemption. */
		तालकम्_तन्तवः,		/**< lock only against other threads, using SPI_LOCK */
		तालकम्_NONE		/**< perform no locking, only safe if the bus is entirely private */
	};

	आभास पूर्णाङ्क	init() override;

	/**
	 * Check for the presence of the device on the bus.
	 */
	आभास पूर्णाङ्क	probe() { प्रत्यागम PX4_OK; }

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
	पूर्णाङ्क		transfer(uint8_t *send, uint8_t *recv, अचिह्नित len);

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
	पूर्णाङ्क		transferhword(uint16_t *send, uint16_t *recv, अचिह्नित len);

	/**
	 * Set the SPI bus frequency
	 * This is used to change frequency on the fly. Some sensors
	 * (such as the MPU6000) need a lower frequency for setup
	 * registers and can handle higher frequency for sensor
	 * value registers
	 *
	 * @param frequency	Frequency to set (Hz)
	 */
	शून्य		समुच्चयः_frequency(uint32_t frequency) { _frequency = frequency; }
	uint32_t	get_frequency() { प्रत्यागम _frequency; }

	/**
	 * Set the SPI bus locking mode
	 *
	 * This set the SPI locking mode. For devices competing with NuttX SPI
	 * drivers on a bus the right lock mode is LOCK_THREADS.
	 *
	 * @param mode	Locking mode
	 */
	शून्य		समुच्चयः_lockmode(गणना तालकम्_बहुलकम् बहुलकम्) { locking_बहुलकम् = बहुलकम्; }

गुप्त:
	uint32_t		_device;
	गणना spi_बहुलकम्_e		_mode;
	uint32_t		_frequency;
	पूर्णाङ्क 			_fd{-1};

	तालकम्_बहुलकम्		locking_बहुलकम्{तालकम्_तन्तवः};	/**< selected locking mode */

रक्षित:
	पूर्णाङ्क	_transfer(uint8_t *send, uint8_t *recv, अचिह्नित len);

	पूर्णाङ्क	_transferhword(uint16_t *send, uint16_t *recv, अचिह्नित len);

};

} // namespace device

#अन्यथा

गणना spi_बहुलकम्_e {
	SPIDEV_MODE0 = 0, /* CPOL=0 CHPHA=0 */
	SPIDEV_MODE1 = 1, /* CPOL=0 CHPHA=1 */
	SPIDEV_MODE2 = 2, /* CPOL=1 CHPHA=0 */
	SPIDEV_MODE3 = 3  /* CPOL=1 CHPHA=1 */
};
#यदि_अन्तः // __PX4_LINUX

#यदि_अन्तः // CONFIG_SPI
