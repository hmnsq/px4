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

#సూచన once

/**
 * @file SPI.hpp
 *
 * Base class for devices connected via SPI.
 */

#అన్తర్భావ "../CDev.hpp"
#అన్తర్భావ <px4_platform_common/spi.h>

#ఒకవేళ defined(CONFIG_SPI)

#నిర్వచితమైతే __PX4_LINUX

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <linux/types.h>
#అన్తర్భావ <linux/spi/spidev.h>

గణన spi_బహుళకం_e {
	SPIDEV_MODE0 = SPI_బహుళకం_0, /* CPOL=0 CHPHA=0 */
	SPIDEV_MODE1 = SPI_బహుళకం_1, /* CPOL=0 CHPHA=1 */
	SPIDEV_MODE2 = SPI_బహుళకం_2, /* CPOL=1 CHPHA=0 */
	SPIDEV_MODE3 = SPI_బహుళకం_3  /* CPOL=1 CHPHA=1 */
};

నిర్మాణం I2CSPIDriverConfig;

నామాకాశ device __EXPORT
{

/**
 * Abstract class for character device on SPI
 */
తరగతి __EXPORT SPI : బహిరంగ CDev
{
బహిరంగ:
	// no copy, assignment, move, move assignment
	SPI(స్థిరరాశి SPI &) = తొలగించు;
	SPI &క్రియాకారకం=(స్థిరరాశి SPI &) = తొలగించు;
	SPI(SPI &&) = తొలగించు;
	SPI &క్రియాకారకం=(SPI &&) = తొలగించు;

రక్షిత:
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
	SPI(uint8_t device_type, స్థిరరాశి అక్షరం *name, పూర్ణాంకం bus, uint32_t device, గణన spi_బహుళకం_e బహుళకం, uint32_t frequency);

	SPI(స్థిరరాశి I2CSPIDriverConfig &config);

	మాయా ~SPI();

	/**
	 * Locking modes supported by the driver.
	 */
	గణన తాళం_బహుళకం {
		తాళం_PREEMPTION,	/**< the default; lock against all forms of preemption. */
		తాళం_దారాలు,		/**< lock only against other threads, using SPI_LOCK */
		తాళం_NONE		/**< perform no locking, only safe if the bus is entirely private */
	};

	మాయా పూర్ణాంకం	init() override;

	/**
	 * Check for the presence of the device on the bus.
	 */
	మాయా పూర్ణాంకం	probe() { తిరిగి PX4_OK; }

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
	పూర్ణాంకం		transfer(uint8_t *send, uint8_t *recv, గుర్తులేని len);

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
	పూర్ణాంకం		transferhword(uint16_t *send, uint16_t *recv, గుర్తులేని len);

	/**
	 * Set the SPI bus frequency
	 * This is used to change frequency on the fly. Some sensors
	 * (such as the MPU6000) need a lower frequency for setup
	 * registers and can handle higher frequency for sensor
	 * value registers
	 *
	 * @param frequency	Frequency to set (Hz)
	 */
	శూన్య		సమితి_frequency(uint32_t frequency) { _frequency = frequency; }
	uint32_t	get_frequency() { తిరిగి _frequency; }

	/**
	 * Set the SPI bus locking mode
	 *
	 * This set the SPI locking mode. For devices competing with NuttX SPI
	 * drivers on a bus the right lock mode is LOCK_THREADS.
	 *
	 * @param mode	Locking mode
	 */
	శూన్య		సమితి_lockmode(గణన తాళం_బహుళకం బహుళకం) { locking_బహుళకం = బహుళకం; }

అంతరంగ:
	uint32_t		_device;
	గణన spi_బహుళకం_e		_mode;
	uint32_t		_frequency;
	పూర్ణాంకం 			_fd{-1};

	తాళం_బహుళకం		locking_బహుళకం{తాళం_దారాలు};	/**< selected locking mode */

రక్షిత:
	పూర్ణాంకం	_transfer(uint8_t *send, uint8_t *recv, గుర్తులేని len);

	పూర్ణాంకం	_transferhword(uint16_t *send, uint16_t *recv, గుర్తులేని len);

};

} // namespace device

#లేకపోతే

గణన spi_బహుళకం_e {
	SPIDEV_MODE0 = 0, /* CPOL=0 CHPHA=0 */
	SPIDEV_MODE1 = 1, /* CPOL=0 CHPHA=1 */
	SPIDEV_MODE2 = 2, /* CPOL=1 CHPHA=0 */
	SPIDEV_MODE3 = 3  /* CPOL=1 CHPHA=1 */
};
#అయితే_ముగింపు // __PX4_LINUX

#అయితే_ముగింపు // CONFIG_SPI
