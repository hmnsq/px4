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

SMBus::SMBus(uint8_t device_id, صحيح bus_عدد, uint16_t پتہ) :
	I2C(device_id, MODULE_NAME, bus_عدد, پتہ, 100000)
{
}

SMBus::~SMBus()
{
	perf_free(_interface_errors);
}

صحيح SMBus::read_لفظ(ثابت uint8_t cmd_کوڈ, uint16_t &انکڑے)
{
	uint8_t buf[6];
	// 2 data bytes + pec byte
	صحيح نتیجہ = transfer(&cmd_کوڈ, 1, buf + 3, 3);

	إذا (نتیجہ == PX4_OK) {
		انکڑے = buf[3] | ((uint16_t)buf[4] << 8);
		// Check PEC.
		uint8_t addr = get_device_پتہ() << 1;
		buf[0] = addr | 0x00;
		buf[1] = cmd_کوڈ;
		buf[2] = addr | 0x01;

		uint8_t pec = get_pec(buf, حجم(buf) - 1);

		إذا (pec != buf[حجم(buf) - 1]) {
			نتیجہ = -EINVAL;
			perf_گنتی(_interface_errors);
		}

	} وإلا {
		perf_گنتی(_interface_errors);
	}

	أرجع نتیجہ;
}

صحيح SMBus::write_لفظ(ثابت uint8_t cmd_کوڈ, uint16_t انکڑے)
{
	// 2 data bytes + pec byte
	uint8_t buf[5];
	buf[0] = (get_device_پتہ() << 1) | 0x10;
	buf[1] = cmd_کوڈ;
	buf[2] = انکڑے & 0xff;
	buf[3] = (انکڑے >> 8) & 0xff;

	buf[4] = get_pec(buf, 4);

	صحيح نتیجہ = transfer(&buf[1], 4, خالی_سوچک, 0);

	إذا (نتیجہ != PX4_OK) {
		perf_گنتی(_interface_errors);
	}

	أرجع نتیجہ;
}

صحيح SMBus::بلاک_read(ثابت uint8_t cmd_کوڈ, فراغ *انکڑے, ثابت uint8_t لمائی, ثابت منطقي use_pec)
{
	uint8_t byte_گنتی = 0;
	// addr(wr), cmd_code, addr(r), byte_count, data (MAX_BLOCK_LEN bytes max), pec
	uint8_t rx_انکڑے[MAX_بلاک_LEN + 5];

	إذا (لمائی > MAX_بلاک_LEN) {
		أرجع -EINVAL;
	}

	صحيح نتیجہ = transfer(&cmd_کوڈ, 1, (uint8_t *)&rx_انکڑے[3], لمائی + 2);

	إذا (نتیجہ != PX4_OK) {
		perf_گنتی(_interface_errors);
		أرجع نتیجہ;
	}

	uint8_t device_پتہ = get_device_پتہ();
	rx_انکڑے[0] = (device_پتہ << 1) | 0x00;
	rx_انکڑے[1] = cmd_کوڈ;
	rx_انکڑے[2] = (device_پتہ << 1) | 0x01;
	byte_گنتی = math::min(rx_انکڑے[3], MAX_بلاک_LEN);

	// ensure data is not longer than given buffer
	حافظہ_نقل(انکڑے, &rx_انکڑے[4], math::min(byte_گنتی, لمائی));

	إذا (use_pec) {
		uint8_t pec = get_pec(rx_انکڑے, byte_گنتی + 4);

		إذا (pec != rx_انکڑے[byte_گنتی + 4]) {
			نتیجہ = -EIO;
			perf_گنتی(_interface_errors);
		}
	}

	أرجع نتیجہ;
}

صحيح SMBus::بلاک_write(ثابت uint8_t cmd_کوڈ, ثابت فراغ *انکڑے, uint8_t byte_گنتی, ثابت منطقي use_pec)
{
	// cmd code[1], byte count[1], data[byte_count] (MAX_BLOCK_LEN max), pec[1] (optional)
	uint8_t buf[MAX_بلاک_LEN + 2];

	إذا (byte_گنتی > MAX_بلاک_LEN) {
		أرجع -EINVAL;
	}

	buf[0] = cmd_کوڈ;
	buf[1] = (uint8_t)byte_گنتی;
	حافظہ_نقل(&buf[2], انکڑے, byte_گنتی);

	إذا (use_pec) {
		uint8_t pec = get_pec(buf, byte_گنتی + 2);
		buf[byte_گنتی + 2] = pec;
		byte_گنتی++;
	}

	بنانشانی i = 0;
	صحيح نتیجہ = 0;

	// If block_write fails, try up to 10 times.
	طالما (i < 10 && ((نتیجہ = transfer((uint8_t *)buf, byte_گنتی + 2, خالی_سوچک, 0)) != PX4_OK)) {
		perf_گنتی(_interface_errors);
		i++;
	}

	إذا (i == 10 || نتیجہ) {
		نتیجہ = -EINVAL;
	}

	أرجع نتیجہ;
}

uint8_t SMBus::get_pec(uint8_t *buff, ثابت uint8_t len)
{
	// TODO: use "return crc8ccitt(buff, len);"

	// Initialise CRC to zero.
	uint8_t چکری_جانچ = 0;
	uint8_t کھسکاؤ_رجسٹر = 0;
	منطقي invert_چکری_جانچ;

	// Calculate crc for each byte in the stream.
	لكل (uint8_t i = 0; i < len; i++) {
		// Load next data byte into the shift register
		کھسکاؤ_رجسٹر = buff[i];

		// Calculate crc for each bit in the current byte.
		لكل (uint8_t j = 0; j < 8; j++) {
			invert_چکری_جانچ = (چکری_جانچ ^ کھسکاؤ_رجسٹر) & 0x80;
			چکری_جانچ <<= 1;
			کھسکاؤ_رجسٹر <<= 1;

			إذا (invert_چکری_جانچ) {
				چکری_جانچ ^= SMBUS_PEC_POLYNOMIAL;
			}
		}
	}

	أرجع چکری_جانچ;
}
