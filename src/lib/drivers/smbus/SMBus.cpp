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

#अन्तर्भाव "SMBus.hpp"
#अन्तर्भाव <mathlib/mathlib.h>

SMBus::SMBus(uint8_t device_id, पूर्णाङ्क bus_अङ्कः, uint16_t स्थानाङ्कः) :
	I2C(device_id, MODULE_NAME, bus_अङ्कः, स्थानाङ्कः, 100000)
{
}

SMBus::~SMBus()
{
	perf_free(_interface_errors);
}

पूर्णाङ्क SMBus::read_word(ध्रुव uint8_t cmd_संहिता, uint16_t &दत्त)
{
	uint8_t buf[6];
	// 2 data bytes + pec byte
	पूर्णाङ्क परिणाम = transfer(&cmd_संहिता, 1, buf + 3, 3);

	यदि (परिणाम == PX4_OK) {
		दत्त = buf[3] | ((uint16_t)buf[4] << 8);
		// Check PEC.
		uint8_t addr = get_device_स्थानाङ्कः() << 1;
		buf[0] = addr | 0x00;
		buf[1] = cmd_संहिता;
		buf[2] = addr | 0x01;

		uint8_t pec = get_pec(buf, परिमाण(buf) - 1);

		यदि (pec != buf[परिमाण(buf) - 1]) {
			परिणाम = -EINVAL;
			perf_गणनम्(_interface_errors);
		}

	} अन्यथा {
		perf_गणनम्(_interface_errors);
	}

	प्रत्यागम परिणाम;
}

पूर्णाङ्क SMBus::write_word(ध्रुव uint8_t cmd_संहिता, uint16_t दत्त)
{
	// 2 data bytes + pec byte
	uint8_t buf[5];
	buf[0] = (get_device_स्थानाङ्कः() << 1) | 0x10;
	buf[1] = cmd_संहिता;
	buf[2] = दत्त & 0xff;
	buf[3] = (दत्त >> 8) & 0xff;

	buf[4] = get_pec(buf, 4);

	पूर्णाङ्क परिणाम = transfer(&buf[1], 4, शून्यसूचकः, 0);

	यदि (परिणाम != PX4_OK) {
		perf_गणनम्(_interface_errors);
	}

	प्रत्यागम परिणाम;
}

पूर्णाङ्क SMBus::खण्डम्_read(ध्रुव uint8_t cmd_संहिता, शून्य *दत्त, ध्रुव uint8_t दीर्घता_2, ध्रुव तार्किक use_pec)
{
	uint8_t byte_गणनम् = 0;
	// addr(wr), cmd_code, addr(r), byte_count, data (MAX_BLOCK_LEN bytes max), pec
	uint8_t rx_दत्त[MAX_खण्डम्_LEN + 5];

	यदि (दीर्घता_2 > MAX_खण्डम्_LEN) {
		प्रत्यागम -EINVAL;
	}

	पूर्णाङ्क परिणाम = transfer(&cmd_संहिता, 1, (uint8_t *)&rx_दत्त[3], दीर्घता_2 + 2);

	यदि (परिणाम != PX4_OK) {
		perf_गणनम्(_interface_errors);
		प्रत्यागम परिणाम;
	}

	uint8_t device_स्थानाङ्कः = get_device_स्थानाङ्कः();
	rx_दत्त[0] = (device_स्थानाङ्कः << 1) | 0x00;
	rx_दत्त[1] = cmd_संहिता;
	rx_दत्त[2] = (device_स्थानाङ्कः << 1) | 0x01;
	byte_गणनम् = math::min(rx_दत्त[3], MAX_खण्डम्_LEN);

	// ensure data is not longer than given buffer
	स्मृतिं_प्रतिलिख(दत्त, &rx_दत्त[4], math::min(byte_गणनम्, दीर्घता_2));

	यदि (use_pec) {
		uint8_t pec = get_pec(rx_दत्त, byte_गणनम् + 4);

		यदि (pec != rx_दत्त[byte_गणनम् + 4]) {
			परिणाम = -EIO;
			perf_गणनम्(_interface_errors);
		}
	}

	प्रत्यागम परिणाम;
}

पूर्णाङ्क SMBus::खण्डम्_write(ध्रुव uint8_t cmd_संहिता, ध्रुव शून्य *दत्त, uint8_t byte_गणनम्, ध्रुव तार्किक use_pec)
{
	// cmd code[1], byte count[1], data[byte_count] (MAX_BLOCK_LEN max), pec[1] (optional)
	uint8_t buf[MAX_खण्डम्_LEN + 2];

	यदि (byte_गणनम् > MAX_खण्डम्_LEN) {
		प्रत्यागम -EINVAL;
	}

	buf[0] = cmd_संहिता;
	buf[1] = (uint8_t)byte_गणनम्;
	स्मृतिं_प्रतिलिख(&buf[2], दत्त, byte_गणनम्);

	यदि (use_pec) {
		uint8_t pec = get_pec(buf, byte_गणनम् + 2);
		buf[byte_गणनम् + 2] = pec;
		byte_गणनम्++;
	}

	अचिह्नित i = 0;
	पूर्णाङ्क परिणाम = 0;

	// If block_write fails, try up to 10 times.
	यावत् (i < 10 && ((परिणाम = transfer((uint8_t *)buf, byte_गणनम् + 2, शून्यसूचकः, 0)) != PX4_OK)) {
		perf_गणनम्(_interface_errors);
		i++;
	}

	यदि (i == 10 || परिणाम) {
		परिणाम = -EINVAL;
	}

	प्रत्यागम परिणाम;
}

uint8_t SMBus::get_pec(uint8_t *buff, ध्रुव uint8_t len)
{
	// TODO: use "return crc8ccitt(buff, len);"

	// Initialise CRC to zero.
	uint8_t crc = 0;
	uint8_t सरणम्_पञ्जिकाकोशः = 0;
	तार्किक invert_crc;

	// Calculate crc for each byte in the stream.
	प्रति (uint8_t i = 0; i < len; i++) {
		// Load next data byte into the shift register
		सरणम्_पञ्जिकाकोशः = buff[i];

		// Calculate crc for each bit in the current byte.
		प्रति (uint8_t j = 0; j < 8; j++) {
			invert_crc = (crc ^ सरणम्_पञ्जिकाकोशः) & 0x80;
			crc <<= 1;
			सरणम्_पञ्जिकाकोशः <<= 1;

			यदि (invert_crc) {
				crc ^= SMBUS_PEC_POLYNOMIAL;
			}
		}
	}

	प्रत्यागम crc;
}
