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

#إذا 0 // enable non-verbose debugging
#تعریف_کن CRSF_DEBUG PX4_WARN
#وإلا
#تعریف_کن CRSF_DEBUG(...)
#پایان_اگر

#إذا 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#تعریف_کن CRSF_VERBOSE PX4_WARN
#وإلا
#تعریف_کن CRSF_VERBOSE(...)
#پایان_اگر

#ضمن <drivers/drv_hrt.h>
#ضمن <termios.h>
#ضمن <string.h>
#ضمن <unistd.h>

#ضمن "crsf.h"
#ضمن "common_rc.h"

#تعریف_کن MIN(a,b) (((a)<(b))?(a):(b))
#تعریف_کن MAX(a,b) (((a)>(b))?(a):(b))


#تعریف_کن CRSF_BAUDRATE 420000

#تعریف_کن CRSF_SYNC_BYTE 0xC8

تعداد صنف crsf_قاب_type_t : uint8_t {
	gps = 0x02,
	battery_sensor = 0x08,
	baro_altitude = 0x09,
	link_statistics = 0x14,
	rc_channels_packed = 0x16,
	attitude = 0x1E,
	flight_نما = 0x21,

	// Extended Header Frames, range: 0x28 to 0x96
	device_ping = 0x28,
	device_info = 0x29,
	parameter_settings_مدخل = 0x2B,
	parameter_read = 0x2C,
	parameter_write = 0x2D,
	command = 0x32
};

تعداد صنف crsf_payload_اندازه_t : uint8_t {
	gps = 15,
	battery_sensor = 8,
	baro_altitude = 4, ///< altitude (uint16) + vertical speed (int16)
	link_statistics = 10,
	rc_channels = 22, ///< 11 bits per channel * 16 channels = 22 bytes.
	attitude = 6,
};


تعداد صنف crsf_آدرس_t : uint8_t {
	پخش = 0x00,
	usb = 0x10,
	tbs_core_pnp_pro = 0x80,
	reserved1 = 0x8A,
	current_sensor = 0xC0,
	gps = 0xC2,
	tbs_blackbox = 0xC4,
	flight_controller = 0xC8,
	reserved2 = 0xCA,
	race_برچسب = 0xCC,
	radio_transmitter = 0xEA,
	crsf_receiver = 0xEC,
	crsf_transmitter = 0xEE
};

#رهنمود pack(افزودن, 1)
بنية crsf_payload_RC_channels_packed_t {
	// 176 bits of data (11 bits per channel * 16 channels) = 22 bytes
	بیعلامت chan0 : 11;
	بیعلامت chan1 : 11;
	بیعلامت chan2 : 11;
	بیعلامت chan3 : 11;
	بیعلامت chan4 : 11;
	بیعلامت chan5 : 11;
	بیعلامت chan6 : 11;
	بیعلامت chan7 : 11;
	بیعلامت chan8 : 11;
	بیعلامت chan9 : 11;
	بیعلامت chan10 : 11;
	بیعلامت chan11 : 11;
	بیعلامت chan12 : 11;
	بیعلامت chan13 : 11;
	بیعلامت chan14 : 11;
	بیعلامت chan15 : 11;
};

#رهنمود pack(برداشت)

تعداد صنف crsf_parser_state_t : uint8_t {
	unsynced = 0,
	synced
};

ساكن crsf_قاب_t	 &crsf_قاب = rc_decode_buf.crsf_قاب;
ساكن بیعلامت current_قاب_position = 0;
ساكن crsf_parser_state_t parser_state = crsf_parser_state_t::unsynced;

/**
 * parse the current crsf_frame buffer
 */
ساكن منطقي crsf_parse_میانگیر(uint16_t *مقادیر_2, uint16_t *رقم_مقادیر, uint16_t max_channels);

uint8_t crsf_قاب_بررسی_چرخهای(ثابت crsf_قاب_t &قاب);


صحيح
crsf_config(صحيح uart_fd)
{
	بنية termios t;

	/* no parity, one stop bit */
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, CRSF_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	أرجع tcsetattr(uart_fd, TCSANOW, &t);
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
ساكن uint16_t convert_channel_مقدار(بیعلامت chan_مقدار);


منطقي crsf_parse(ثابت uint64_t now, ثابت uint8_t *قاب, بیعلامت len, uint16_t *مقادیر_2,
		uint16_t *رقم_مقادیر, uint16_t max_channels)
{
	منطقي ret = خطأ;
	uint8_t *crsf_قاب_ptr = (uint8_t *)&crsf_قاب;

	طالما (len > 0) {

		// fill in the crsf_buffer, as much as we can
		ثابت بیعلامت current_len = MIN(len, حجم(crsf_قاب_t) - current_قاب_position);
		کپی_حافظه(crsf_قاب_ptr + current_قاب_position, قاب, current_len);
		current_قاب_position += current_len;

		// protection to guarantee parsing progress
		إذا (current_len == 0) {
			CRSF_DEBUG("========== parser bug: no progress (%i) ===========", len);

			لكل (بیعلامت i = 0; i < current_قاب_position; ++i) {
				CRSF_DEBUG("crsf_frame_ptr[%i]: 0x%x", i, (صحيح)crsf_قاب_ptr[i]);
			}

			// reset the parser
			current_قاب_position = 0;
			parser_state = crsf_parser_state_t::unsynced;
			أرجع خطأ;
		}

		len -= current_len;
		قاب += current_len;

		إذا (crsf_parse_میانگیر(مقادیر_2, رقم_مقادیر, max_channels)) {
			ret = درست;
		}
	}


	أرجع ret;
}

uint8_t crsf_قاب_بررسی_چرخهای(ثابت crsf_قاب_t &قاب)
{
	// CRC includes type and payload
	uint8_t بررسی_چرخهای = crc8_dvb_s2(0, قاب.type);

	لكل (صحيح i = 0; i < قاب.سرآیند.درازی - 2; ++i) {
		بررسی_چرخهای = crc8_dvb_s2(بررسی_چرخهای, قاب.payload[i]);
	}

	أرجع بررسی_چرخهای;
}

ساكن uint16_t convert_channel_مقدار(بیعلامت chan_مقدار)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	ساكن constexpr عائم scale = (2012.f - 988.f) / (1811.f - 172.f);
	ساكن constexpr عائم جابه_جایی = 988.f - 172.f * scale;
	أرجع (scale * chan_مقدار) + جابه_جایی;
}

ساكن منطقي crsf_parse_میانگیر(uint16_t *مقادیر_2, uint16_t *رقم_مقادیر, uint16_t max_channels)
{
	uint8_t *crsf_قاب_ptr = (uint8_t *)&crsf_قاب;

	إذا (parser_state == crsf_parser_state_t::unsynced) {
		// there is no sync byte, try to find an RC packet by searching for a matching frame length and type
		لكل (بیعلامت i = 1; i < current_قاب_position - 1; ++i) {
			إذا (crsf_قاب_ptr[i] == (uint8_t)crsf_payload_اندازه_t::rc_channels + 2 &&
			    crsf_قاب_ptr[i + 1] == (uint8_t)crsf_قاب_type_t::rc_channels_packed) {
				parser_state = crsf_parser_state_t::synced;
				بیعلامت قاب_جابه_جایی = i - 1;
				CRSF_VERBOSE("RC channels found at offset %i", قاب_جابه_جایی);

				// move the rest of the buffer to the beginning
				إذا (قاب_جابه_جایی != 0) {
					memmove(crsf_قاب_ptr, crsf_قاب_ptr + قاب_جابه_جایی, current_قاب_position - قاب_جابه_جایی);
					current_قاب_position -= قاب_جابه_جایی;
				}

				اكسر;
			}
		}
	}

	إذا (parser_state != crsf_parser_state_t::synced) {
		إذا (current_قاب_position >= حجم(crsf_قاب_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_قاب_position = 3;

			لكل (بیعلامت i = 0; i < current_قاب_position; ++i) {
				crsf_قاب_ptr[i] = crsf_قاب_ptr[حجم(crsf_قاب_t) - current_قاب_position + i];
			}

			CRSF_VERBOSE("Discarding buffer");
		}

		أرجع خطأ;
	}


	إذا (current_قاب_position < 3) {
		// wait until we have the header & type
		أرجع خطأ;
	}

	// Now we have at least the header and the type

	ثابت بیعلامت current_قاب_درازی = crsf_قاب.سرآیند.درازی + حجم(crsf_قاب_سرآیند_t);

	إذا (current_قاب_درازی > حجم(crsf_قاب_t) || current_قاب_درازی < 4) {
		// frame too long or bogus -> discard everything and go into unsynced state
		current_قاب_position = 0;
		parser_state = crsf_parser_state_t::unsynced;
		CRSF_DEBUG("Frame too long/bogus (%i, type=%i) -> unsync", current_قاب_درازی, crsf_قاب.type);
		أرجع خطأ;
	}

	إذا (current_قاب_position < current_قاب_درازی) {
		// we don't have the full frame yet -> wait for more data
		CRSF_VERBOSE("waiting for more data (%i < %i)", current_قاب_position, current_قاب_درازی);
		أرجع خطأ;
	}

	منطقي ret = خطأ;

	// Now we have the full frame

	إذا (crsf_قاب.type == (uint8_t)crsf_قاب_type_t::rc_channels_packed &&
	    crsf_قاب.سرآیند.درازی == (uint8_t)crsf_payload_اندازه_t::rc_channels + 2) {
		ثابت uint8_t بررسی_چرخهای = crsf_قاب.payload[crsf_قاب.سرآیند.درازی - 2];

		إذا (بررسی_چرخهای == crsf_قاب_بررسی_چرخهای(crsf_قاب)) {
			ثابت crsf_payload_RC_channels_packed_t *ثابت rc_channels =
				(crsf_payload_RC_channels_packed_t *)&crsf_قاب.payload;
			*رقم_مقادیر = MIN(max_channels, 16);

			إذا (max_channels > 0) { مقادیر_2[0] = convert_channel_مقدار(rc_channels->chan0); }

			إذا (max_channels > 1) { مقادیر_2[1] = convert_channel_مقدار(rc_channels->chan1); }

			إذا (max_channels > 2) { مقادیر_2[2] = convert_channel_مقدار(rc_channels->chan2); }

			إذا (max_channels > 3) { مقادیر_2[3] = convert_channel_مقدار(rc_channels->chan3); }

			إذا (max_channels > 4) { مقادیر_2[4] = convert_channel_مقدار(rc_channels->chan4); }

			إذا (max_channels > 5) { مقادیر_2[5] = convert_channel_مقدار(rc_channels->chan5); }

			إذا (max_channels > 6) { مقادیر_2[6] = convert_channel_مقدار(rc_channels->chan6); }

			إذا (max_channels > 7) { مقادیر_2[7] = convert_channel_مقدار(rc_channels->chan7); }

			إذا (max_channels > 8) { مقادیر_2[8] = convert_channel_مقدار(rc_channels->chan8); }

			إذا (max_channels > 9) { مقادیر_2[9] = convert_channel_مقدار(rc_channels->chan9); }

			إذا (max_channels > 10) { مقادیر_2[10] = convert_channel_مقدار(rc_channels->chan10); }

			إذا (max_channels > 11) { مقادیر_2[11] = convert_channel_مقدار(rc_channels->chan11); }

			إذا (max_channels > 12) { مقادیر_2[12] = convert_channel_مقدار(rc_channels->chan12); }

			إذا (max_channels > 13) { مقادیر_2[13] = convert_channel_مقدار(rc_channels->chan13); }

			إذا (max_channels > 14) { مقادیر_2[14] = convert_channel_مقدار(rc_channels->chan14); }

			إذا (max_channels > 15) { مقادیر_2[15] = convert_channel_مقدار(rc_channels->chan15); }

			CRSF_VERBOSE("Got Channels");

			ret = درست;

		} وإلا {
			CRSF_DEBUG("CRC check failed");
		}

	} وإلا {
		CRSF_DEBUG("Got Non-RC frame (len=%i, type=%i)", current_قاب_درازی, crsf_قاب.type);
		// We could check the CRC here and reset the parser into unsynced state if it fails.
		// But in practise it's robust even without that.
	}

	// Either reset or move the rest of the buffer
	إذا (current_قاب_position > current_قاب_درازی) {
		CRSF_VERBOSE("Moving buffer (%i > %i)", current_قاب_position, current_قاب_درازی);
		memmove(crsf_قاب_ptr, crsf_قاب_ptr + current_قاب_درازی, current_قاب_position - current_قاب_درازی);
		current_قاب_position -= current_قاب_درازی;

	} وإلا {
		current_قاب_position = 0;
	}

	أرجع ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint8_t(uint8_t *buf, صحيح &جابه_جایی, uint8_t value)
{
	buf[جابه_جایی++] = value;
}
/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint16_t(uint8_t *buf, صحيح &جابه_جایی, uint16_t value)
{
	// Big endian
	buf[جابه_جایی] = value >> 8;
	buf[جابه_جایی + 1] = value & 0xff;
	جابه_جایی += 2;
}
/**
 * write an uint24_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint24_t(uint8_t *buf, صحيح &جابه_جایی, صحيح value)
{
	// Big endian
	buf[جابه_جایی] = value >> 16;
	buf[جابه_جایی + 1] = (value >> 8) & 0xff;
	buf[جابه_جایی + 2] = value & 0xff;
	جابه_جایی += 3;
}

/**
 * write an int32_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_int32_t(uint8_t *buf, صحيح &جابه_جایی, int32_t value)
{
	// Big endian
	buf[جابه_جایی] = value >> 24;
	buf[جابه_جایی + 1] = (value >> 16) & 0xff;
	buf[جابه_جایی + 2] = (value >> 8) & 0xff;
	buf[جابه_جایی + 3] = value & 0xff;
	جابه_جایی += 4;
}

ساكن مضمن فراغ write_قاب_سرآیند(uint8_t *buf, صحيح &جابه_جایی, crsf_قاب_type_t type, uint8_t payload_اندازه)
{
	write_uint8_t(buf, جابه_جایی, CRSF_SYNC_BYTE); // this got changed from the address to the sync byte
	write_uint8_t(buf, جابه_جایی, payload_اندازه + 2);
	write_uint8_t(buf, جابه_جایی, (uint8_t)type);
}
ساكن مضمن فراغ write_قاب_بررسی_چرخهای(uint8_t *buf, صحيح &جابه_جایی, صحيح buf_اندازه)
{
	// CRC does not include the address and length
	write_uint8_t(buf, جابه_جایی, crc8_dvb_s2_buf(buf + 2, buf_اندازه - 3));

	// check correctness of buffer size (only needed during development)
	//if (buf_size != offset) { PX4_ERR("frame size mismatch (%i != %i)", buf_size, offset); }
}

منطقي crsf_send_telemetry_battery(صحيح uart_fd, uint16_t voltage, uint16_t current, صحيح fuel, uint8_t مانده)
{
	uint8_t buf[(uint8_t)crsf_payload_اندازه_t::battery_sensor + 4];
	صحيح جابه_جایی = 0;
	write_قاب_سرآیند(buf, جابه_جایی, crsf_قاب_type_t::battery_sensor, (uint8_t)crsf_payload_اندازه_t::battery_sensor);
	write_uint16_t(buf, جابه_جایی, voltage);
	write_uint16_t(buf, جابه_جایی, current);
	write_uint24_t(buf, جابه_جایی, fuel);
	write_uint8_t(buf, جابه_جایی, مانده);
	write_قاب_بررسی_چرخهای(buf, جابه_جایی, حجم(buf));
	أرجع اكتب(uart_fd, buf, جابه_جایی) == جابه_جایی;
}

منطقي crsf_send_telemetry_gps(صحيح uart_fd, int32_t latitude, int32_t longitude, uint16_t groundspeed,
			     uint16_t gps_heading, uint16_t altitude, uint8_t رقم_satellites)
{
	uint8_t buf[(uint8_t)crsf_payload_اندازه_t::gps + 4];
	صحيح جابه_جایی = 0;
	write_قاب_سرآیند(buf, جابه_جایی, crsf_قاب_type_t::gps, (uint8_t)crsf_payload_اندازه_t::gps);
	write_int32_t(buf, جابه_جایی, latitude);
	write_int32_t(buf, جابه_جایی, longitude);
	write_uint16_t(buf, جابه_جایی, groundspeed);
	write_uint16_t(buf, جابه_جایی, gps_heading);
	write_uint16_t(buf, جابه_جایی, altitude);
	write_uint8_t(buf, جابه_جایی, رقم_satellites);
	write_قاب_بررسی_چرخهای(buf, جابه_جایی, حجم(buf));
	أرجع اكتب(uart_fd, buf, جابه_جایی) == جابه_جایی;
}

منطقي crsf_send_telemetry_baro_altitude(صحيح uart_fd, uint16_t altitude, int16_t vertical_سرعت)
{
	uint8_t buf[(uint8_t)crsf_payload_اندازه_t::baro_altitude + 4];
	صحيح جابه_جایی = 0;
	write_قاب_سرآیند(buf, جابه_جایی, crsf_قاب_type_t::baro_altitude, (uint8_t)crsf_payload_اندازه_t::baro_altitude);
	write_uint16_t(buf, جابه_جایی, altitude);
	write_uint16_t(buf, جابه_جایی, (uint16_t)vertical_سرعت);
	write_قاب_بررسی_چرخهای(buf, جابه_جایی, حجم(buf));
	أرجع اكتب(uart_fd, buf, جابه_جایی) == جابه_جایی;
}

منطقي crsf_send_telemetry_attitude(صحيح uart_fd, int16_t pitch, int16_t roll, int16_t yaw)
{
	uint8_t buf[(uint8_t)crsf_payload_اندازه_t::attitude + 4];
	صحيح جابه_جایی = 0;
	write_قاب_سرآیند(buf, جابه_جایی, crsf_قاب_type_t::attitude, (uint8_t)crsf_payload_اندازه_t::attitude);
	write_uint16_t(buf, جابه_جایی, pitch);
	write_uint16_t(buf, جابه_جایی, roll);
	write_uint16_t(buf, جابه_جایی, yaw);
	write_قاب_بررسی_چرخهای(buf, جابه_جایی, حجم(buf));
	أرجع اكتب(uart_fd, buf, جابه_جایی) == جابه_جایی;
}

منطقي crsf_send_telemetry_flight_نما(صحيح uart_fd, ثابت حرف *flight_نما)
{
	ثابت صحيح max_درازی = 16;
	صحيح درازی = طول_رشته(flight_نما) + 1;

	إذا (درازی > max_درازی) {
		درازی = max_درازی;
	}

	uint8_t buf[max_درازی + 4];
	صحيح جابه_جایی = 0;
	write_قاب_سرآیند(buf, جابه_جایی, crsf_قاب_type_t::flight_نما, درازی);
	کپی_حافظه(buf + جابه_جایی, flight_نما, درازی);
	جابه_جایی += درازی;
	buf[جابه_جایی - 1] = 0; // ensure null-terminated string
	write_قاب_بررسی_چرخهای(buf, جابه_جایی, درازی + 4);
	أرجع اكتب(uart_fd, buf, جابه_جایی) == جابه_جایی;
}
