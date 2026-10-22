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

#ਅਨ੍ਤਰ੍ਭਾਵ "SMBus.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/mathlib.h>

SMBus::SMBus(uint8_t device_id, ਪੂਰਨ bus_ਅੰਕ, uint16_t ਪਤਾ) :
	I2C(device_id, MODULE_NAME, bus_ਅੰਕ, ਪਤਾ, 100000)
{
}

SMBus::~SMBus()
{
	perf_free(_interface_errors);
}

ਪੂਰਨ SMBus::read_ਸ਼ਬਦ(ਅਚਲ uint8_t cmd_ਕੋਡ, uint16_t &ਅੰਕੜੇ)
{
	uint8_t buf[6];
	// 2 data bytes + pec byte
	ਪੂਰਨ ਨਤੀਜਾ = transfer(&cmd_ਕੋਡ, 1, buf + 3, 3);

	ਜੇ (ਨਤੀਜਾ == PX4_OK) {
		ਅੰਕੜੇ = buf[3] | ((uint16_t)buf[4] << 8);
		// Check PEC.
		uint8_t addr = get_device_ਪਤਾ() << 1;
		buf[0] = addr | 0x00;
		buf[1] = cmd_ਕੋਡ;
		buf[2] = addr | 0x01;

		uint8_t pec = get_pec(buf, ਆਕਾਰ(buf) - 1);

		ਜੇ (pec != buf[ਆਕਾਰ(buf) - 1]) {
			ਨਤੀਜਾ = -EINVAL;
			perf_ਗਿਣਤੀ(_interface_errors);
		}

	} ਨਹੀਂਤਾਂ {
		perf_ਗਿਣਤੀ(_interface_errors);
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ SMBus::write_ਸ਼ਬਦ(ਅਚਲ uint8_t cmd_ਕੋਡ, uint16_t ਅੰਕੜੇ)
{
	// 2 data bytes + pec byte
	uint8_t buf[5];
	buf[0] = (get_device_ਪਤਾ() << 1) | 0x10;
	buf[1] = cmd_ਕੋਡ;
	buf[2] = ਅੰਕੜੇ & 0xff;
	buf[3] = (ਅੰਕੜੇ >> 8) & 0xff;

	buf[4] = get_pec(buf, 4);

	ਪੂਰਨ ਨਤੀਜਾ = transfer(&buf[1], 4, ਖਾਲੀ_ਸੂਚਕ, 0);

	ਜੇ (ਨਤੀਜਾ != PX4_OK) {
		perf_ਗਿਣਤੀ(_interface_errors);
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ SMBus::ਖੰਡ_read(ਅਚਲ uint8_t cmd_ਕੋਡ, ਖਾਲੀ *ਅੰਕੜੇ, ਅਚਲ uint8_t ਲੰਮਾਈ, ਅਚਲ ਤਾਰਕਿਕ use_pec)
{
	uint8_t byte_ਗਿਣਤੀ = 0;
	// addr(wr), cmd_code, addr(r), byte_count, data (MAX_BLOCK_LEN bytes max), pec
	uint8_t rx_ਅੰਕੜੇ[MAX_ਖੰਡ_LEN + 5];

	ਜੇ (ਲੰਮਾਈ > MAX_ਖੰਡ_LEN) {
		ਵਾਪਸ -EINVAL;
	}

	ਪੂਰਨ ਨਤੀਜਾ = transfer(&cmd_ਕੋਡ, 1, (uint8_t *)&rx_ਅੰਕੜੇ[3], ਲੰਮਾਈ + 2);

	ਜੇ (ਨਤੀਜਾ != PX4_OK) {
		perf_ਗਿਣਤੀ(_interface_errors);
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	uint8_t device_ਪਤਾ = get_device_ਪਤਾ();
	rx_ਅੰਕੜੇ[0] = (device_ਪਤਾ << 1) | 0x00;
	rx_ਅੰਕੜੇ[1] = cmd_ਕੋਡ;
	rx_ਅੰਕੜੇ[2] = (device_ਪਤਾ << 1) | 0x01;
	byte_ਗਿਣਤੀ = math::min(rx_ਅੰਕੜੇ[3], MAX_ਖੰਡ_LEN);

	// ensure data is not longer than given buffer
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਅੰਕੜੇ, &rx_ਅੰਕੜੇ[4], math::min(byte_ਗਿਣਤੀ, ਲੰਮਾਈ));

	ਜੇ (use_pec) {
		uint8_t pec = get_pec(rx_ਅੰਕੜੇ, byte_ਗਿਣਤੀ + 4);

		ਜੇ (pec != rx_ਅੰਕੜੇ[byte_ਗਿਣਤੀ + 4]) {
			ਨਤੀਜਾ = -EIO;
			perf_ਗਿਣਤੀ(_interface_errors);
		}
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ SMBus::ਖੰਡ_write(ਅਚਲ uint8_t cmd_ਕੋਡ, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, uint8_t byte_ਗਿਣਤੀ, ਅਚਲ ਤਾਰਕਿਕ use_pec)
{
	// cmd code[1], byte count[1], data[byte_count] (MAX_BLOCK_LEN max), pec[1] (optional)
	uint8_t buf[MAX_ਖੰਡ_LEN + 2];

	ਜੇ (byte_ਗਿਣਤੀ > MAX_ਖੰਡ_LEN) {
		ਵਾਪਸ -EINVAL;
	}

	buf[0] = cmd_ਕੋਡ;
	buf[1] = (uint8_t)byte_ਗਿਣਤੀ;
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(&buf[2], ਅੰਕੜੇ, byte_ਗਿਣਤੀ);

	ਜੇ (use_pec) {
		uint8_t pec = get_pec(buf, byte_ਗਿਣਤੀ + 2);
		buf[byte_ਗਿਣਤੀ + 2] = pec;
		byte_ਗਿਣਤੀ++;
	}

	ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0;
	ਪੂਰਨ ਨਤੀਜਾ = 0;

	// If block_write fails, try up to 10 times.
	ਜਦੋਂ (i < 10 && ((ਨਤੀਜਾ = transfer((uint8_t *)buf, byte_ਗਿਣਤੀ + 2, ਖਾਲੀ_ਸੂਚਕ, 0)) != PX4_OK)) {
		perf_ਗਿਣਤੀ(_interface_errors);
		i++;
	}

	ਜੇ (i == 10 || ਨਤੀਜਾ) {
		ਨਤੀਜਾ = -EINVAL;
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

uint8_t SMBus::get_pec(uint8_t *buff, ਅਚਲ uint8_t len)
{
	// TODO: use "return crc8ccitt(buff, len);"

	// Initialise CRC to zero.
	uint8_t ਚੱਕਰੀ_ਜਾਂਚ = 0;
	uint8_t ਖਿਸਕਾਓ_ਰਜਿਸਟਰ = 0;
	ਤਾਰਕਿਕ invert_ਚੱਕਰੀ_ਜਾਂਚ;

	// Calculate crc for each byte in the stream.
	ਲਈ (uint8_t i = 0; i < len; i++) {
		// Load next data byte into the shift register
		ਖਿਸਕਾਓ_ਰਜਿਸਟਰ = buff[i];

		// Calculate crc for each bit in the current byte.
		ਲਈ (uint8_t j = 0; j < 8; j++) {
			invert_ਚੱਕਰੀ_ਜਾਂਚ = (ਚੱਕਰੀ_ਜਾਂਚ ^ ਖਿਸਕਾਓ_ਰਜਿਸਟਰ) & 0x80;
			ਚੱਕਰੀ_ਜਾਂਚ <<= 1;
			ਖਿਸਕਾਓ_ਰਜਿਸਟਰ <<= 1;

			ਜੇ (invert_ਚੱਕਰੀ_ਜਾਂਚ) {
				ਚੱਕਰੀ_ਜਾਂਚ ^= SMBUS_PEC_POLYNOMIAL;
			}
		}
	}

	ਵਾਪਸ ਚੱਕਰੀ_ਜਾਂਚ;
}
