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

SMBus::SMBus(uint8_t device_id, صحيح bus_عدد, uint16_t پته) :
	I2C(device_id, MODULE_NAME, bus_عدد, پته, 100000)
{
}

SMBus::~SMBus()
{
	perf_free(_interface_errors);
}

صحيح SMBus::read_کلمه(ثابت uint8_t cmd_کوډ, uint16_t &معلومات)
{
	uint8_t buf[6];
	// 2 data bytes + pec byte
	صحيح پایله = transfer(&cmd_کوډ, 1, buf + 3, 3);

	إذا (پایله == PX4_OK) {
		معلومات = buf[3] | ((uint16_t)buf[4] << 8);
		// Check PEC.
		uint8_t addr = get_device_پته() << 1;
		buf[0] = addr | 0x00;
		buf[1] = cmd_کوډ;
		buf[2] = addr | 0x01;

		uint8_t pec = get_pec(buf, حجم(buf) - 1);

		إذا (pec != buf[حجم(buf) - 1]) {
			پایله = -EINVAL;
			perf_شمېر(_interface_errors);
		}

	} وإلا {
		perf_شمېر(_interface_errors);
	}

	أرجع پایله;
}

صحيح SMBus::write_کلمه(ثابت uint8_t cmd_کوډ, uint16_t معلومات)
{
	// 2 data bytes + pec byte
	uint8_t buf[5];
	buf[0] = (get_device_پته() << 1) | 0x10;
	buf[1] = cmd_کوډ;
	buf[2] = معلومات & 0xff;
	buf[3] = (معلومات >> 8) & 0xff;

	buf[4] = get_pec(buf, 4);

	صحيح پایله = transfer(&buf[1], 4, تش_اشاره, 0);

	إذا (پایله != PX4_OK) {
		perf_شمېر(_interface_errors);
	}

	أرجع پایله;
}

صحيح SMBus::بلاک_read(ثابت uint8_t cmd_کوډ, فراغ *معلومات, ثابت uint8_t اوږدتیا, ثابت منطقي use_pec)
{
	uint8_t byte_شمېر = 0;
	// addr(wr), cmd_code, addr(r), byte_count, data (MAX_BLOCK_LEN bytes max), pec
	uint8_t rx_معلومات[MAX_بلاک_LEN + 5];

	إذا (اوږدتیا > MAX_بلاک_LEN) {
		أرجع -EINVAL;
	}

	صحيح پایله = transfer(&cmd_کوډ, 1, (uint8_t *)&rx_معلومات[3], اوږدتیا + 2);

	إذا (پایله != PX4_OK) {
		perf_شمېر(_interface_errors);
		أرجع پایله;
	}

	uint8_t device_پته = get_device_پته();
	rx_معلومات[0] = (device_پته << 1) | 0x00;
	rx_معلومات[1] = cmd_کوډ;
	rx_معلومات[2] = (device_پته << 1) | 0x01;
	byte_شمېر = math::min(rx_معلومات[3], MAX_بلاک_LEN);

	// ensure data is not longer than given buffer
	حافظه_نقل(معلومات, &rx_معلومات[4], math::min(byte_شمېر, اوږدتیا));

	إذا (use_pec) {
		uint8_t pec = get_pec(rx_معلومات, byte_شمېر + 4);

		إذا (pec != rx_معلومات[byte_شمېر + 4]) {
			پایله = -EIO;
			perf_شمېر(_interface_errors);
		}
	}

	أرجع پایله;
}

صحيح SMBus::بلاک_write(ثابت uint8_t cmd_کوډ, ثابت فراغ *معلومات, uint8_t byte_شمېر, ثابت منطقي use_pec)
{
	// cmd code[1], byte count[1], data[byte_count] (MAX_BLOCK_LEN max), pec[1] (optional)
	uint8_t buf[MAX_بلاک_LEN + 2];

	إذا (byte_شمېر > MAX_بلاک_LEN) {
		أرجع -EINVAL;
	}

	buf[0] = cmd_کوډ;
	buf[1] = (uint8_t)byte_شمېر;
	حافظه_نقل(&buf[2], معلومات, byte_شمېر);

	إذا (use_pec) {
		uint8_t pec = get_pec(buf, byte_شمېر + 2);
		buf[byte_شمېر + 2] = pec;
		byte_شمېر++;
	}

	بینښه i = 0;
	صحيح پایله = 0;

	// If block_write fails, try up to 10 times.
	طالما (i < 10 && ((پایله = transfer((uint8_t *)buf, byte_شمېر + 2, تش_اشاره, 0)) != PX4_OK)) {
		perf_شمېر(_interface_errors);
		i++;
	}

	إذا (i == 10 || پایله) {
		پایله = -EINVAL;
	}

	أرجع پایله;
}

uint8_t SMBus::get_pec(uint8_t *buff, ثابت uint8_t len)
{
	// TODO: use "return crc8ccitt(buff, len);"

	// Initialise CRC to zero.
	uint8_t دوراني_ازموینه = 0;
	uint8_t ښويدنه_ثبتوونکی = 0;
	منطقي invert_دوراني_ازموینه;

	// Calculate crc for each byte in the stream.
	لكل (uint8_t i = 0; i < len; i++) {
		// Load next data byte into the shift register
		ښويدنه_ثبتوونکی = buff[i];

		// Calculate crc for each bit in the current byte.
		لكل (uint8_t j = 0; j < 8; j++) {
			invert_دوراني_ازموینه = (دوراني_ازموینه ^ ښويدنه_ثبتوونکی) & 0x80;
			دوراني_ازموینه <<= 1;
			ښويدنه_ثبتوونکی <<= 1;

			إذا (invert_دوراني_ازموینه) {
				دوراني_ازموینه ^= SMBUS_PEC_POLYNOMIAL;
			}
		}
	}

	أرجع دوراني_ازموینه;
}
