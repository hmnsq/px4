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

#ضمن "SMBus.hpp"
#ضمن <mathlib/mathlib.h>

SMBus::SMBus(uint8_t device_id, صحيح bus_رقم, uint16_t آدرس) :
	I2C(device_id, MODULE_NAME, bus_رقم, آدرس, 100000)
{
}

SMBus::~SMBus()
{
	perf_free(_interface_errors);
}

صحيح SMBus::read_کلمه(ثابت uint8_t cmd_کد, uint16_t &داده)
{
	uint8_t buf[6];
	// 2 data bytes + pec byte
	صحيح نتیجه = transfer(&cmd_کد, 1, buf + 3, 3);

	إذا (نتیجه == PX4_OK) {
		داده = buf[3] | ((uint16_t)buf[4] << 8);
		// Check PEC.
		uint8_t addr = get_device_آدرس() << 1;
		buf[0] = addr | 0x00;
		buf[1] = cmd_کد;
		buf[2] = addr | 0x01;

		uint8_t pec = get_pec(buf, حجم(buf) - 1);

		إذا (pec != buf[حجم(buf) - 1]) {
			نتیجه = -EINVAL;
			perf_شمار(_interface_errors);
		}

	} وإلا {
		perf_شمار(_interface_errors);
	}

	أرجع نتیجه;
}

صحيح SMBus::write_کلمه(ثابت uint8_t cmd_کد, uint16_t داده)
{
	// 2 data bytes + pec byte
	uint8_t buf[5];
	buf[0] = (get_device_آدرس() << 1) | 0x10;
	buf[1] = cmd_کد;
	buf[2] = داده & 0xff;
	buf[3] = (داده >> 8) & 0xff;

	buf[4] = get_pec(buf, 4);

	صحيح نتیجه = transfer(&buf[1], 4, اشاره_تهی, 0);

	إذا (نتیجه != PX4_OK) {
		perf_شمار(_interface_errors);
	}

	أرجع نتیجه;
}

صحيح SMBus::بلوک_read(ثابت uint8_t cmd_کد, فراغ *داده, ثابت uint8_t درازی, ثابت منطقي use_pec)
{
	uint8_t byte_شمار = 0;
	// addr(wr), cmd_code, addr(r), byte_count, data (MAX_BLOCK_LEN bytes max), pec
	uint8_t rx_داده[MAX_بلوک_LEN + 5];

	إذا (درازی > MAX_بلوک_LEN) {
		أرجع -EINVAL;
	}

	صحيح نتیجه = transfer(&cmd_کد, 1, (uint8_t *)&rx_داده[3], درازی + 2);

	إذا (نتیجه != PX4_OK) {
		perf_شمار(_interface_errors);
		أرجع نتیجه;
	}

	uint8_t device_آدرس = get_device_آدرس();
	rx_داده[0] = (device_آدرس << 1) | 0x00;
	rx_داده[1] = cmd_کد;
	rx_داده[2] = (device_آدرس << 1) | 0x01;
	byte_شمار = math::min(rx_داده[3], MAX_بلوک_LEN);

	// ensure data is not longer than given buffer
	کپی_حافظه(داده, &rx_داده[4], math::min(byte_شمار, درازی));

	إذا (use_pec) {
		uint8_t pec = get_pec(rx_داده, byte_شمار + 4);

		إذا (pec != rx_داده[byte_شمار + 4]) {
			نتیجه = -EIO;
			perf_شمار(_interface_errors);
		}
	}

	أرجع نتیجه;
}

صحيح SMBus::بلوک_write(ثابت uint8_t cmd_کد, ثابت فراغ *داده, uint8_t byte_شمار, ثابت منطقي use_pec)
{
	// cmd code[1], byte count[1], data[byte_count] (MAX_BLOCK_LEN max), pec[1] (optional)
	uint8_t buf[MAX_بلوک_LEN + 2];

	إذا (byte_شمار > MAX_بلوک_LEN) {
		أرجع -EINVAL;
	}

	buf[0] = cmd_کد;
	buf[1] = (uint8_t)byte_شمار;
	کپی_حافظه(&buf[2], داده, byte_شمار);

	إذا (use_pec) {
		uint8_t pec = get_pec(buf, byte_شمار + 2);
		buf[byte_شمار + 2] = pec;
		byte_شمار++;
	}

	بیعلامت i = 0;
	صحيح نتیجه = 0;

	// If block_write fails, try up to 10 times.
	طالما (i < 10 && ((نتیجه = transfer((uint8_t *)buf, byte_شمار + 2, اشاره_تهی, 0)) != PX4_OK)) {
		perf_شمار(_interface_errors);
		i++;
	}

	إذا (i == 10 || نتیجه) {
		نتیجه = -EINVAL;
	}

	أرجع نتیجه;
}

uint8_t SMBus::get_pec(uint8_t *buff, ثابت uint8_t len)
{
	// TODO: use "return crc8ccitt(buff, len);"

	// Initialise CRC to zero.
	uint8_t بررسی_چرخهای = 0;
	uint8_t لغزش_ثبات = 0;
	منطقي invert_بررسی_چرخهای;

	// Calculate crc for each byte in the stream.
	لكل (uint8_t i = 0; i < len; i++) {
		// Load next data byte into the shift register
		لغزش_ثبات = buff[i];

		// Calculate crc for each bit in the current byte.
		لكل (uint8_t j = 0; j < 8; j++) {
			invert_بررسی_چرخهای = (بررسی_چرخهای ^ لغزش_ثبات) & 0x80;
			بررسی_چرخهای <<= 1;
			لغزش_ثبات <<= 1;

			إذا (invert_بررسی_چرخهای) {
				بررسی_چرخهای ^= SMBUS_PEC_POLYNOMIAL;
			}
		}
	}

	أرجع بررسی_چرخهای;
}
