/****************************************************************************
 *
 *   Copyright (c) 2018 PX4 Development Team. All rights reserved.
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
 * @file SMBus.cpp
 * SMBus v2.0 protocol implementation.
 *
 * @author Jacob Dahl <dahl.jakejacob@gmail.com>
 * @author Bazooka Joe <BazookaJoe1900@gmail.com>
 *
 * TODO
 *  - Enable SMBus mode at the NuttX level. This may be tricky sharing the bus with i2c.
 *  	i.e STM32f4 see platforms/nuttx/Nuttx/nuttx/arch/arm/src/stm32/stm32f40xxx_i2c.c
 *  - Add remaining SMBus protocol messages as needed
 */

#ಅನ್ತರ್ಭಾವ "SMBus.hpp"
#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>

SMBus::SMBus(uint8_t device_id, ಪೂರ್ಣಾಂಕ bus_ಅಂಕೆ, uint16_t ವಿಳಾಸ) :
	I2C(device_id, MODULE_NAME, bus_ಅಂಕೆ, ವಿಳಾಸ, 100000)
{
}

SMBus::~SMBus()
{
	perf_free(_interface_errors);
}

ಪೂರ್ಣಾಂಕ SMBus::read_ಶಬ್ದ(ಸ್ಥಿರಾಂಕ uint8_t cmd_ಸಂಕೇತ, uint16_t &ದತ್ತಾಂಶ)
{
	uint8_t buf[6];
	// 2 data bytes + pec byte
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = transfer(&cmd_ಸಂಕೇತ, 1, buf + 3, 3);

	ಆದರೆ (ಫಲಿತಾಂಶ == PX4_OK) {
		ದತ್ತಾಂಶ = buf[3] | ((uint16_t)buf[4] << 8);
		// Check PEC.
		uint8_t addr = get_device_ವಿಳಾಸ() << 1;
		buf[0] = addr | 0x00;
		buf[1] = cmd_ಸಂಕೇತ;
		buf[2] = addr | 0x01;

		uint8_t pec = get_pec(buf, ಗಾತ್ರ(buf) - 1);

		ಆದರೆ (pec != buf[ಗಾತ್ರ(buf) - 1]) {
			ಫಲಿತಾಂಶ = -EINVAL;
			perf_ಎಣಿಕೆ(_interface_errors);
		}

	} ಇಲ್ಲದಿದ್ದರೆ {
		perf_ಎಣಿಕೆ(_interface_errors);
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪೂರ್ಣಾಂಕ SMBus::write_ಶಬ್ದ(ಸ್ಥಿರಾಂಕ uint8_t cmd_ಸಂಕೇತ, uint16_t ದತ್ತಾಂಶ)
{
	// 2 data bytes + pec byte
	uint8_t buf[5];
	buf[0] = (get_device_ವಿಳಾಸ() << 1) | 0x10;
	buf[1] = cmd_ಸಂಕೇತ;
	buf[2] = ದತ್ತಾಂಶ & 0xff;
	buf[3] = (ದತ್ತಾಂಶ >> 8) & 0xff;

	buf[4] = get_pec(buf, 4);

	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = transfer(&buf[1], 4, ಶೂನ್ಯಸೂಚಕ, 0);

	ಆದರೆ (ಫಲಿತಾಂಶ != PX4_OK) {
		perf_ಎಣಿಕೆ(_interface_errors);
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪೂರ್ಣಾಂಕ SMBus::ಖಂಡ_read(ಸ್ಥಿರಾಂಕ uint8_t cmd_ಸಂಕೇತ, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ uint8_t ಉದ್ದಳತೆ, ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ use_pec)
{
	uint8_t byte_ಎಣಿಕೆ = 0;
	// addr(wr), cmd_code, addr(r), byte_count, data (MAX_BLOCK_LEN bytes max), pec
	uint8_t rx_ದತ್ತಾಂಶ[MAX_ಖಂಡ_LEN + 5];

	ಆದರೆ (ಉದ್ದಳತೆ > MAX_ಖಂಡ_LEN) {
		ಹಿಂತಿರುಗಿ -EINVAL;
	}

	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = transfer(&cmd_ಸಂಕೇತ, 1, (uint8_t *)&rx_ದತ್ತಾಂಶ[3], ಉದ್ದಳತೆ + 2);

	ಆದರೆ (ಫಲಿತಾಂಶ != PX4_OK) {
		perf_ಎಣಿಕೆ(_interface_errors);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	uint8_t device_ವಿಳಾಸ = get_device_ವಿಳಾಸ();
	rx_ದತ್ತಾಂಶ[0] = (device_ವಿಳಾಸ << 1) | 0x00;
	rx_ದತ್ತಾಂಶ[1] = cmd_ಸಂಕೇತ;
	rx_ದತ್ತಾಂಶ[2] = (device_ವಿಳಾಸ << 1) | 0x01;
	byte_ಎಣಿಕೆ = math::min(rx_ದತ್ತಾಂಶ[3], MAX_ಖಂಡ_LEN);

	// ensure data is not longer than given buffer
	ಸ್ಮರಣೆ_ನಕಲು(ದತ್ತಾಂಶ, &rx_ದತ್ತಾಂಶ[4], math::min(byte_ಎಣಿಕೆ, ಉದ್ದಳತೆ));

	ಆದರೆ (use_pec) {
		uint8_t pec = get_pec(rx_ದತ್ತಾಂಶ, byte_ಎಣಿಕೆ + 4);

		ಆದರೆ (pec != rx_ದತ್ತಾಂಶ[byte_ಎಣಿಕೆ + 4]) {
			ಫಲಿತಾಂಶ = -EIO;
			perf_ಎಣಿಕೆ(_interface_errors);
		}
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪೂರ್ಣಾಂಕ SMBus::ಖಂಡ_write(ಸ್ಥಿರಾಂಕ uint8_t cmd_ಸಂಕೇತ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, uint8_t byte_ಎಣಿಕೆ, ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ use_pec)
{
	// cmd code[1], byte count[1], data[byte_count] (MAX_BLOCK_LEN max), pec[1] (optional)
	uint8_t buf[MAX_ಖಂಡ_LEN + 2];

	ಆದರೆ (byte_ಎಣಿಕೆ > MAX_ಖಂಡ_LEN) {
		ಹಿಂತಿರುಗಿ -EINVAL;
	}

	buf[0] = cmd_ಸಂಕೇತ;
	buf[1] = (uint8_t)byte_ಎಣಿಕೆ;
	ಸ್ಮರಣೆ_ನಕಲು(&buf[2], ದತ್ತಾಂಶ, byte_ಎಣಿಕೆ);

	ಆದರೆ (use_pec) {
		uint8_t pec = get_pec(buf, byte_ಎಣಿಕೆ + 2);
		buf[byte_ಎಣಿಕೆ + 2] = pec;
		byte_ಎಣಿಕೆ++;
	}

	ಚಿಹ್ನೆರಹಿತ i = 0;
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = 0;

	// If block_write fails, try up to 10 times.
	ಆಗ (i < 10 && ((ಫಲಿತಾಂಶ = transfer((uint8_t *)buf, byte_ಎಣಿಕೆ + 2, ಶೂನ್ಯಸೂಚಕ, 0)) != PX4_OK)) {
		perf_ಎಣಿಕೆ(_interface_errors);
		i++;
	}

	ಆದರೆ (i == 10 || ಫಲಿತಾಂಶ) {
		ಫಲಿತಾಂಶ = -EINVAL;
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

uint8_t SMBus::get_pec(uint8_t *buff, ಸ್ಥಿರಾಂಕ uint8_t len)
{
	// TODO: use "return crc8ccitt(buff, len);"

	// Initialise CRC to zero.
	uint8_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = 0;
	uint8_t ಸರಿಸು_ರಿಜಿಸ್ಟರ್ = 0;
	ತಾರ್ಕಿಕ invert_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ;

	// Calculate crc for each byte in the stream.
	ಗಾಗಿ (uint8_t i = 0; i < len; i++) {
		// Load next data byte into the shift register
		ಸರಿಸು_ರಿಜಿಸ್ಟರ್ = buff[i];

		// Calculate crc for each bit in the current byte.
		ಗಾಗಿ (uint8_t j = 0; j < 8; j++) {
			invert_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ ^ ಸರಿಸು_ರಿಜಿಸ್ಟರ್) & 0x80;
			ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ <<= 1;
			ಸರಿಸು_ರಿಜಿಸ್ಟರ್ <<= 1;

			ಆದರೆ (invert_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ) {
				ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ ^= SMBUS_PEC_POLYNOMIAL;
			}
		}
	}

	ಹಿಂತಿರುಗಿ ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ;
}
