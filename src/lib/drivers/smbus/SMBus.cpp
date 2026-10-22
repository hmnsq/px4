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

#অন্তর্ভাব "SMBus.hpp"
#অন্তর্ভাব <mathlib/mathlib.h>

SMBus::SMBus(uint8_t device_id, পূর্ণসংখ্যা bus_অঙ্ক, uint16_t address) :
	I2C(device_id, MODULE_NAME, bus_অঙ্ক, address, 100000)
{
}

SMBus::~SMBus()
{
	perf_free(_interface_errors);
}

পূর্ণসংখ্যা SMBus::read_word(ধ্রুবক uint8_t cmd_code, uint16_t &তথ্য)
{
	uint8_t buf[6];
	// 2 data bytes + pec byte
	পূর্ণসংখ্যা ফলাফল = transfer(&cmd_code, 1, buf + 3, 3);

	যদি (ফলাফল == PX4_OK) {
		তথ্য = buf[3] | ((uint16_t)buf[4] << 8);
		// Check PEC.
		uint8_t addr = get_device_address() << 1;
		buf[0] = addr | 0x00;
		buf[1] = cmd_code;
		buf[2] = addr | 0x01;

		uint8_t pec = get_pec(buf, আকার(buf) - 1);

		যদি (pec != buf[আকার(buf) - 1]) {
			ফলাফল = -EINVAL;
			perf_গুনতি(_interface_errors);
		}

	} অন্যথা {
		perf_গুনতি(_interface_errors);
	}

	ফেরত ফলাফল;
}

পূর্ণসংখ্যা SMBus::write_word(ধ্রুবক uint8_t cmd_code, uint16_t তথ্য)
{
	// 2 data bytes + pec byte
	uint8_t buf[5];
	buf[0] = (get_device_address() << 1) | 0x10;
	buf[1] = cmd_code;
	buf[2] = তথ্য & 0xff;
	buf[3] = (তথ্য >> 8) & 0xff;

	buf[4] = get_pec(buf, 4);

	পূর্ণসংখ্যা ফলাফল = transfer(&buf[1], 4, nullptr, 0);

	যদি (ফলাফল != PX4_OK) {
		perf_গুনতি(_interface_errors);
	}

	ফেরত ফলাফল;
}

পূর্ণসংখ্যা SMBus::block_read(ধ্রুবক uint8_t cmd_code, শূন্য *তথ্য, ধ্রুবক uint8_t দৈর্ঘ্য, ধ্রুবক যৌক্তিক use_pec)
{
	uint8_t byte_গুনতি = 0;
	// addr(wr), cmd_code, addr(r), byte_count, data (MAX_BLOCK_LEN bytes max), pec
	uint8_t rx_তথ্য[MAX_BLOCK_LEN + 5];

	যদি (দৈর্ঘ্য > MAX_BLOCK_LEN) {
		ফেরত -EINVAL;
	}

	পূর্ণসংখ্যা ফলাফল = transfer(&cmd_code, 1, (uint8_t *)&rx_তথ্য[3], দৈর্ঘ্য + 2);

	যদি (ফলাফল != PX4_OK) {
		perf_গুনতি(_interface_errors);
		ফেরত ফলাফল;
	}

	uint8_t device_address = get_device_address();
	rx_তথ্য[0] = (device_address << 1) | 0x00;
	rx_তথ্য[1] = cmd_code;
	rx_তথ্য[2] = (device_address << 1) | 0x01;
	byte_গুনতি = math::min(rx_তথ্য[3], MAX_BLOCK_LEN);

	// ensure data is not longer than given buffer
	memcpy(তথ্য, &rx_তথ্য[4], math::min(byte_গুনতি, দৈর্ঘ্য));

	যদি (use_pec) {
		uint8_t pec = get_pec(rx_তথ্য, byte_গুনতি + 4);

		যদি (pec != rx_তথ্য[byte_গুনতি + 4]) {
			ফলাফল = -EIO;
			perf_গুনতি(_interface_errors);
		}
	}

	ফেরত ফলাফল;
}

পূর্ণসংখ্যা SMBus::block_write(ধ্রুবক uint8_t cmd_code, ধ্রুবক শূন্য *তথ্য, uint8_t byte_গুনতি, ধ্রুবক যৌক্তিক use_pec)
{
	// cmd code[1], byte count[1], data[byte_count] (MAX_BLOCK_LEN max), pec[1] (optional)
	uint8_t buf[MAX_BLOCK_LEN + 2];

	যদি (byte_গুনতি > MAX_BLOCK_LEN) {
		ফেরত -EINVAL;
	}

	buf[0] = cmd_code;
	buf[1] = (uint8_t)byte_গুনতি;
	memcpy(&buf[2], তথ্য, byte_গুনতি);

	যদি (use_pec) {
		uint8_t pec = get_pec(buf, byte_গুনতি + 2);
		buf[byte_গুনতি + 2] = pec;
		byte_গুনতি++;
	}

	unsigned i = 0;
	পূর্ণসংখ্যা ফলাফল = 0;

	// If block_write fails, try up to 10 times.
	যখন (i < 10 && ((ফলাফল = transfer((uint8_t *)buf, byte_গুনতি + 2, nullptr, 0)) != PX4_OK)) {
		perf_গুনতি(_interface_errors);
		i++;
	}

	যদি (i == 10 || ফলাফল) {
		ফলাফল = -EINVAL;
	}

	ফেরত ফলাফল;
}

uint8_t SMBus::get_pec(uint8_t *buff, ধ্রুবক uint8_t len)
{
	// TODO: use "return crc8ccitt(buff, len);"

	// Initialise CRC to zero.
	uint8_t crc = 0;
	uint8_t shift_register = 0;
	যৌক্তিক invert_crc;

	// Calculate crc for each byte in the stream.
	জন্য (uint8_t i = 0; i < len; i++) {
		// Load next data byte into the shift register
		shift_register = buff[i];

		// Calculate crc for each bit in the current byte.
		জন্য (uint8_t j = 0; j < 8; j++) {
			invert_crc = (crc ^ shift_register) & 0x80;
			crc <<= 1;
			shift_register <<= 1;

			যদি (invert_crc) {
				crc ^= SMBUS_PEC_POLYNOMIAL;
			}
		}
	}

	ফেরত crc;
}
