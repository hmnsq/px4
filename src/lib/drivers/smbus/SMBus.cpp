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

SMBus::SMBus(uint8_t device_id, ಪೂರ್ಣಾಙ್ಕ bus_ಅಂಕೆ, uint16_t address) :
	I2C(device_id, MODULE_NAME, bus_ಅಂಕೆ, address, 100000)
{
}

SMBus::~SMBus()
{
	perf_free(_interface_errors);
}

ಪೂರ್ಣಾಙ್ಕ SMBus::read_word(ಧ್ರುವ uint8_t cmd_code, uint16_t &data)
{
	uint8_t buf[6];
	// 2 data bytes + pec byte
	ಪೂರ್ಣಾಙ್ಕ result = transfer(&cmd_code, 1, buf + 3, 3);

	ಯದಿ (result == PX4_OK) {
		data = buf[3] | ((uint16_t)buf[4] << 8);
		// Check PEC.
		uint8_t addr = get_device_address() << 1;
		buf[0] = addr | 0x00;
		buf[1] = cmd_code;
		buf[2] = addr | 0x01;

		uint8_t pec = get_pec(buf, ಪರಿಮಾಣ(buf) - 1);

		ಯದಿ (pec != buf[ಪರಿಮಾಣ(buf) - 1]) {
			result = -EINVAL;
			perf_ಎಣಿಕೆ(_interface_errors);
		}

	} ಅನ್ಯಥಾ {
		perf_ಎಣಿಕೆ(_interface_errors);
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

ಪೂರ್ಣಾಙ್ಕ SMBus::write_word(ಧ್ರುವ uint8_t cmd_code, uint16_t data)
{
	// 2 data bytes + pec byte
	uint8_t buf[5];
	buf[0] = (get_device_address() << 1) | 0x10;
	buf[1] = cmd_code;
	buf[2] = data & 0xff;
	buf[3] = (data >> 8) & 0xff;

	buf[4] = get_pec(buf, 4);

	ಪೂರ್ಣಾಙ್ಕ result = transfer(&buf[1], 4, nullptr, 0);

	ಯದಿ (result != PX4_OK) {
		perf_ಎಣಿಕೆ(_interface_errors);
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

ಪೂರ್ಣಾಙ್ಕ SMBus::block_read(ಧ್ರುವ uint8_t cmd_code, ಶೂನ್ಯ *data, ಧ್ರುವ uint8_t length, ಧ್ರುವ ತಾರ್ಕಿಕ use_pec)
{
	uint8_t byte_ಎಣಿಕೆ = 0;
	// addr(wr), cmd_code, addr(r), byte_count, data (MAX_BLOCK_LEN bytes max), pec
	uint8_t rx_data[MAX_BLOCK_LEN + 5];

	ಯದಿ (length > MAX_BLOCK_LEN) {
		ಪ್ರತ್ಯಾಗಮ -EINVAL;
	}

	ಪೂರ್ಣಾಙ್ಕ result = transfer(&cmd_code, 1, (uint8_t *)&rx_data[3], length + 2);

	ಯದಿ (result != PX4_OK) {
		perf_ಎಣಿಕೆ(_interface_errors);
		ಪ್ರತ್ಯಾಗಮ result;
	}

	uint8_t device_address = get_device_address();
	rx_data[0] = (device_address << 1) | 0x00;
	rx_data[1] = cmd_code;
	rx_data[2] = (device_address << 1) | 0x01;
	byte_ಎಣಿಕೆ = math::min(rx_data[3], MAX_BLOCK_LEN);

	// ensure data is not longer than given buffer
	memcpy(data, &rx_data[4], math::min(byte_ಎಣಿಕೆ, length));

	ಯದಿ (use_pec) {
		uint8_t pec = get_pec(rx_data, byte_ಎಣಿಕೆ + 4);

		ಯದಿ (pec != rx_data[byte_ಎಣಿಕೆ + 4]) {
			result = -EIO;
			perf_ಎಣಿಕೆ(_interface_errors);
		}
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

ಪೂರ್ಣಾಙ್ಕ SMBus::block_write(ಧ್ರುವ uint8_t cmd_code, ಧ್ರುವ ಶೂನ್ಯ *data, uint8_t byte_ಎಣಿಕೆ, ಧ್ರುವ ತಾರ್ಕಿಕ use_pec)
{
	// cmd code[1], byte count[1], data[byte_count] (MAX_BLOCK_LEN max), pec[1] (optional)
	uint8_t buf[MAX_BLOCK_LEN + 2];

	ಯದಿ (byte_ಎಣಿಕೆ > MAX_BLOCK_LEN) {
		ಪ್ರತ್ಯಾಗಮ -EINVAL;
	}

	buf[0] = cmd_code;
	buf[1] = (uint8_t)byte_ಎಣಿಕೆ;
	memcpy(&buf[2], data, byte_ಎಣಿಕೆ);

	ಯದಿ (use_pec) {
		uint8_t pec = get_pec(buf, byte_ಎಣಿಕೆ + 2);
		buf[byte_ಎಣಿಕೆ + 2] = pec;
		byte_ಎಣಿಕೆ++;
	}

	unsigned i = 0;
	ಪೂರ್ಣಾಙ್ಕ result = 0;

	// If block_write fails, try up to 10 times.
	ಯಾವತ್ (i < 10 && ((result = transfer((uint8_t *)buf, byte_ಎಣಿಕೆ + 2, nullptr, 0)) != PX4_OK)) {
		perf_ಎಣಿಕೆ(_interface_errors);
		i++;
	}

	ಯದಿ (i == 10 || result) {
		result = -EINVAL;
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

uint8_t SMBus::get_pec(uint8_t *buff, ಧ್ರುವ uint8_t len)
{
	// TODO: use "return crc8ccitt(buff, len);"

	// Initialise CRC to zero.
	uint8_t crc = 0;
	uint8_t shift_register = 0;
	ತಾರ್ಕಿಕ invert_crc;

	// Calculate crc for each byte in the stream.
	ಪ್ರತಿ (uint8_t i = 0; i < len; i++) {
		// Load next data byte into the shift register
		shift_register = buff[i];

		// Calculate crc for each bit in the current byte.
		ಪ್ರತಿ (uint8_t j = 0; j < 8; j++) {
			invert_crc = (crc ^ shift_register) & 0x80;
			crc <<= 1;
			shift_register <<= 1;

			ಯದಿ (invert_crc) {
				crc ^= SMBUS_PEC_POLYNOMIAL;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ crc;
}
