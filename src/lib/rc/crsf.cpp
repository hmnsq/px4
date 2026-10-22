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

#जर 0 // enable non-verbose debugging
#define CRSF_DEBUG PX4_WARN
#अन्यथा
#define CRSF_DEBUG(...)
#endif

#जर 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#define CRSF_VERBOSE PX4_WARN
#अन्यथा
#define CRSF_VERBOSE(...)
#endif

#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <termios.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "crsf.h"
#अन्तर्भाव "common_rc.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


#define CRSF_BAUDRATE 420000

#define CRSF_SYNC_BYTE 0xC8

गणना वर्ग crsf_frame_type_t : uint8_t {
	gps = 0x02,
	battery_sensor = 0x08,
	baro_altitude = 0x09,
	link_statistics = 0x14,
	rc_channels_packed = 0x16,
	attitude = 0x1E,
	flight_mode = 0x21,

	// Extended Header Frames, range: 0x28 to 0x96
	device_ping = 0x28,
	device_info = 0x29,
	parameter_settings_entry = 0x2B,
	parameter_read = 0x2C,
	parameter_write = 0x2D,
	command = 0x32
};

गणना वर्ग crsf_payload_size_t : uint8_t {
	gps = 15,
	battery_sensor = 8,
	baro_altitude = 4, ///< altitude (uint16) + vertical speed (int16)
	link_statistics = 10,
	rc_channels = 22, ///< 11 bits per channel * 16 channels = 22 bytes.
	attitude = 6,
};


गणना वर्ग crsf_address_t : uint8_t {
	broadcast = 0x00,
	usb = 0x10,
	tbs_core_pnp_pro = 0x80,
	reserved1 = 0x8A,
	current_sensor = 0xC0,
	gps = 0xC2,
	tbs_blackbox = 0xC4,
	flight_controller = 0xC8,
	reserved2 = 0xCA,
	race_tag = 0xCC,
	radio_transmitter = 0xEA,
	crsf_receiver = 0xEC,
	crsf_transmitter = 0xEE
};

#pragma pack(ढकला, 1)
संरचना crsf_payload_RC_channels_packed_t {
	// 176 bits of data (11 bits per channel * 16 channels) = 22 bytes
	अचिन्हित chan0 : 11;
	अचिन्हित chan1 : 11;
	अचिन्हित chan2 : 11;
	अचिन्हित chan3 : 11;
	अचिन्हित chan4 : 11;
	अचिन्हित chan5 : 11;
	अचिन्हित chan6 : 11;
	अचिन्हित chan7 : 11;
	अचिन्हित chan8 : 11;
	अचिन्हित chan9 : 11;
	अचिन्हित chan10 : 11;
	अचिन्हित chan11 : 11;
	अचिन्हित chan12 : 11;
	अचिन्हित chan13 : 11;
	अचिन्हित chan14 : 11;
	अचिन्हित chan15 : 11;
};

#pragma pack(काढा)

गणना वर्ग crsf_parser_state_t : uint8_t {
	unsynced = 0,
	synced
};

स्थिर crsf_frame_t	 &crsf_frame = rc_decode_buf.crsf_frame;
स्थिर अचिन्हित current_frame_position = 0;
स्थिर crsf_parser_state_t parser_state = crsf_parser_state_t::unsynced;

/**
 * parse the current crsf_frame buffer
 */
स्थिर तार्किक crsf_parse_buffer(uint16_t *मूल्ये_2, uint16_t *अंक_मूल्ये, uint16_t max_channels);

uint8_t crsf_frame_CRC(स्थिरांक crsf_frame_t &frame);


पूर्णांक
crsf_config(पूर्णांक uart_fd)
{
	संरचना termios t;

	/* no parity, one stop bit */
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, CRSF_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	परत tcsetattr(uart_fd, TCSANOW, &t);
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
स्थिर uint16_t convert_channel_मूल्य(अचिन्हित chan_मूल्य);


तार्किक crsf_parse(स्थिरांक uint64_t now, स्थिरांक uint8_t *frame, अचिन्हित len, uint16_t *मूल्ये_2,
		uint16_t *अंक_मूल्ये, uint16_t max_channels)
{
	तार्किक ret = असत्य;
	uint8_t *crsf_frame_ptr = (uint8_t *)&crsf_frame;

	पर्यंत (len > 0) {

		// fill in the crsf_buffer, as much as we can
		स्थिरांक अचिन्हित current_len = MIN(len, आकार(crsf_frame_t) - current_frame_position);
		स_नकल(crsf_frame_ptr + current_frame_position, frame, current_len);
		current_frame_position += current_len;

		// protection to guarantee parsing progress
		जर (current_len == 0) {
			CRSF_DEBUG("========== parser bug: no progress (%i) ===========", len);

			साठी (अचिन्हित i = 0; i < current_frame_position; ++i) {
				CRSF_DEBUG("crsf_frame_ptr[%i]: 0x%x", i, (पूर्णांक)crsf_frame_ptr[i]);
			}

			// reset the parser
			current_frame_position = 0;
			parser_state = crsf_parser_state_t::unsynced;
			परत असत्य;
		}

		len -= current_len;
		frame += current_len;

		जर (crsf_parse_buffer(मूल्ये_2, अंक_मूल्ये, max_channels)) {
			ret = सत्य;
		}
	}


	परत ret;
}

uint8_t crsf_frame_CRC(स्थिरांक crsf_frame_t &frame)
{
	// CRC includes type and payload
	uint8_t crc = crc8_dvb_s2(0, frame.type);

	साठी (पूर्णांक i = 0; i < frame.header.length - 2; ++i) {
		crc = crc8_dvb_s2(crc, frame.payload[i]);
	}

	परत crc;
}

स्थिर uint16_t convert_channel_मूल्य(अचिन्हित chan_मूल्य)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	स्थिर constexpr दशांश scale = (2012.f - 988.f) / (1811.f - 172.f);
	स्थिर constexpr दशांश offset = 988.f - 172.f * scale;
	परत (scale * chan_मूल्य) + offset;
}

स्थिर तार्किक crsf_parse_buffer(uint16_t *मूल्ये_2, uint16_t *अंक_मूल्ये, uint16_t max_channels)
{
	uint8_t *crsf_frame_ptr = (uint8_t *)&crsf_frame;

	जर (parser_state == crsf_parser_state_t::unsynced) {
		// there is no sync byte, try to find an RC packet by searching for a matching frame length and type
		साठी (अचिन्हित i = 1; i < current_frame_position - 1; ++i) {
			जर (crsf_frame_ptr[i] == (uint8_t)crsf_payload_size_t::rc_channels + 2 &&
			    crsf_frame_ptr[i + 1] == (uint8_t)crsf_frame_type_t::rc_channels_packed) {
				parser_state = crsf_parser_state_t::synced;
				अचिन्हित frame_offset = i - 1;
				CRSF_VERBOSE("RC channels found at offset %i", frame_offset);

				// move the rest of the buffer to the beginning
				जर (frame_offset != 0) {
					स_हटाओ(crsf_frame_ptr, crsf_frame_ptr + frame_offset, current_frame_position - frame_offset);
					current_frame_position -= frame_offset;
				}

				थांबा;
			}
		}
	}

	जर (parser_state != crsf_parser_state_t::synced) {
		जर (current_frame_position >= आकार(crsf_frame_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_frame_position = 3;

			साठी (अचिन्हित i = 0; i < current_frame_position; ++i) {
				crsf_frame_ptr[i] = crsf_frame_ptr[आकार(crsf_frame_t) - current_frame_position + i];
			}

			CRSF_VERBOSE("Discarding buffer");
		}

		परत असत्य;
	}


	जर (current_frame_position < 3) {
		// wait until we have the header & type
		परत असत्य;
	}

	// Now we have at least the header and the type

	स्थिरांक अचिन्हित current_frame_length = crsf_frame.header.length + आकार(crsf_frame_header_t);

	जर (current_frame_length > आकार(crsf_frame_t) || current_frame_length < 4) {
		// frame too long or bogus -> discard everything and go into unsynced state
		current_frame_position = 0;
		parser_state = crsf_parser_state_t::unsynced;
		CRSF_DEBUG("Frame too long/bogus (%i, type=%i) -> unsync", current_frame_length, crsf_frame.type);
		परत असत्य;
	}

	जर (current_frame_position < current_frame_length) {
		// we don't have the full frame yet -> wait for more data
		CRSF_VERBOSE("waiting for more data (%i < %i)", current_frame_position, current_frame_length);
		परत असत्य;
	}

	तार्किक ret = असत्य;

	// Now we have the full frame

	जर (crsf_frame.type == (uint8_t)crsf_frame_type_t::rc_channels_packed &&
	    crsf_frame.header.length == (uint8_t)crsf_payload_size_t::rc_channels + 2) {
		स्थिरांक uint8_t crc = crsf_frame.payload[crsf_frame.header.length - 2];

		जर (crc == crsf_frame_CRC(crsf_frame)) {
			स्थिरांक crsf_payload_RC_channels_packed_t *स्थिरांक rc_channels =
				(crsf_payload_RC_channels_packed_t *)&crsf_frame.payload;
			*अंक_मूल्ये = MIN(max_channels, 16);

			जर (max_channels > 0) { मूल्ये_2[0] = convert_channel_मूल्य(rc_channels->chan0); }

			जर (max_channels > 1) { मूल्ये_2[1] = convert_channel_मूल्य(rc_channels->chan1); }

			जर (max_channels > 2) { मूल्ये_2[2] = convert_channel_मूल्य(rc_channels->chan2); }

			जर (max_channels > 3) { मूल्ये_2[3] = convert_channel_मूल्य(rc_channels->chan3); }

			जर (max_channels > 4) { मूल्ये_2[4] = convert_channel_मूल्य(rc_channels->chan4); }

			जर (max_channels > 5) { मूल्ये_2[5] = convert_channel_मूल्य(rc_channels->chan5); }

			जर (max_channels > 6) { मूल्ये_2[6] = convert_channel_मूल्य(rc_channels->chan6); }

			जर (max_channels > 7) { मूल्ये_2[7] = convert_channel_मूल्य(rc_channels->chan7); }

			जर (max_channels > 8) { मूल्ये_2[8] = convert_channel_मूल्य(rc_channels->chan8); }

			जर (max_channels > 9) { मूल्ये_2[9] = convert_channel_मूल्य(rc_channels->chan9); }

			जर (max_channels > 10) { मूल्ये_2[10] = convert_channel_मूल्य(rc_channels->chan10); }

			जर (max_channels > 11) { मूल्ये_2[11] = convert_channel_मूल्य(rc_channels->chan11); }

			जर (max_channels > 12) { मूल्ये_2[12] = convert_channel_मूल्य(rc_channels->chan12); }

			जर (max_channels > 13) { मूल्ये_2[13] = convert_channel_मूल्य(rc_channels->chan13); }

			जर (max_channels > 14) { मूल्ये_2[14] = convert_channel_मूल्य(rc_channels->chan14); }

			जर (max_channels > 15) { मूल्ये_2[15] = convert_channel_मूल्य(rc_channels->chan15); }

			CRSF_VERBOSE("Got Channels");

			ret = सत्य;

		} अन्यथा {
			CRSF_DEBUG("CRC check failed");
		}

	} अन्यथा {
		CRSF_DEBUG("Got Non-RC frame (len=%i, type=%i)", current_frame_length, crsf_frame.type);
		// We could check the CRC here and reset the parser into unsynced state if it fails.
		// But in practise it's robust even without that.
	}

	// Either reset or move the rest of the buffer
	जर (current_frame_position > current_frame_length) {
		CRSF_VERBOSE("Moving buffer (%i > %i)", current_frame_position, current_frame_length);
		स_हटाओ(crsf_frame_ptr, crsf_frame_ptr + current_frame_length, current_frame_position - current_frame_length);
		current_frame_position -= current_frame_length;

	} अन्यथा {
		current_frame_position = 0;
	}

	परत ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
स्थिर ओळीत शून्य write_uint8_t(uint8_t *buf, पूर्णांक &offset, uint8_t value)
{
	buf[offset++] = value;
}
/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
स्थिर ओळीत शून्य write_uint16_t(uint8_t *buf, पूर्णांक &offset, uint16_t value)
{
	// Big endian
	buf[offset] = value >> 8;
	buf[offset + 1] = value & 0xff;
	offset += 2;
}
/**
 * write an uint24_t value to a buffer at a given offset and increment the offset
 */
स्थिर ओळीत शून्य write_uint24_t(uint8_t *buf, पूर्णांक &offset, पूर्णांक value)
{
	// Big endian
	buf[offset] = value >> 16;
	buf[offset + 1] = (value >> 8) & 0xff;
	buf[offset + 2] = value & 0xff;
	offset += 3;
}

/**
 * write an int32_t value to a buffer at a given offset and increment the offset
 */
स्थिर ओळीत शून्य write_int32_t(uint8_t *buf, पूर्णांक &offset, int32_t value)
{
	// Big endian
	buf[offset] = value >> 24;
	buf[offset + 1] = (value >> 16) & 0xff;
	buf[offset + 2] = (value >> 8) & 0xff;
	buf[offset + 3] = value & 0xff;
	offset += 4;
}

स्थिर ओळीत शून्य write_frame_header(uint8_t *buf, पूर्णांक &offset, crsf_frame_type_t type, uint8_t payload_size)
{
	write_uint8_t(buf, offset, CRSF_SYNC_BYTE); // this got changed from the address to the sync byte
	write_uint8_t(buf, offset, payload_size + 2);
	write_uint8_t(buf, offset, (uint8_t)type);
}
स्थिर ओळीत शून्य write_frame_crc(uint8_t *buf, पूर्णांक &offset, पूर्णांक buf_size)
{
	// CRC does not include the address and length
	write_uint8_t(buf, offset, crc8_dvb_s2_buf(buf + 2, buf_size - 3));

	// check correctness of buffer size (only needed during development)
	//if (buf_size != offset) { PX4_ERR("frame size mismatch (%i != %i)", buf_size, offset); }
}

तार्किक crsf_send_telemetry_battery(पूर्णांक uart_fd, uint16_t voltage, uint16_t current, पूर्णांक fuel, uint8_t remaining)
{
	uint8_t buf[(uint8_t)crsf_payload_size_t::battery_sensor + 4];
	पूर्णांक offset = 0;
	write_frame_header(buf, offset, crsf_frame_type_t::battery_sensor, (uint8_t)crsf_payload_size_t::battery_sensor);
	write_uint16_t(buf, offset, voltage);
	write_uint16_t(buf, offset, current);
	write_uint24_t(buf, offset, fuel);
	write_uint8_t(buf, offset, remaining);
	write_frame_crc(buf, offset, आकार(buf));
	परत लिख(uart_fd, buf, offset) == offset;
}

तार्किक crsf_send_telemetry_gps(पूर्णांक uart_fd, int32_t latitude, int32_t longitude, uint16_t groundspeed,
			     uint16_t gps_heading, uint16_t altitude, uint8_t अंक_satellites)
{
	uint8_t buf[(uint8_t)crsf_payload_size_t::gps + 4];
	पूर्णांक offset = 0;
	write_frame_header(buf, offset, crsf_frame_type_t::gps, (uint8_t)crsf_payload_size_t::gps);
	write_int32_t(buf, offset, latitude);
	write_int32_t(buf, offset, longitude);
	write_uint16_t(buf, offset, groundspeed);
	write_uint16_t(buf, offset, gps_heading);
	write_uint16_t(buf, offset, altitude);
	write_uint8_t(buf, offset, अंक_satellites);
	write_frame_crc(buf, offset, आकार(buf));
	परत लिख(uart_fd, buf, offset) == offset;
}

तार्किक crsf_send_telemetry_baro_altitude(पूर्णांक uart_fd, uint16_t altitude, int16_t vertical_speed)
{
	uint8_t buf[(uint8_t)crsf_payload_size_t::baro_altitude + 4];
	पूर्णांक offset = 0;
	write_frame_header(buf, offset, crsf_frame_type_t::baro_altitude, (uint8_t)crsf_payload_size_t::baro_altitude);
	write_uint16_t(buf, offset, altitude);
	write_uint16_t(buf, offset, (uint16_t)vertical_speed);
	write_frame_crc(buf, offset, आकार(buf));
	परत लिख(uart_fd, buf, offset) == offset;
}

तार्किक crsf_send_telemetry_attitude(पूर्णांक uart_fd, int16_t pitch, int16_t roll, int16_t yaw)
{
	uint8_t buf[(uint8_t)crsf_payload_size_t::attitude + 4];
	पूर्णांक offset = 0;
	write_frame_header(buf, offset, crsf_frame_type_t::attitude, (uint8_t)crsf_payload_size_t::attitude);
	write_uint16_t(buf, offset, pitch);
	write_uint16_t(buf, offset, roll);
	write_uint16_t(buf, offset, yaw);
	write_frame_crc(buf, offset, आकार(buf));
	परत लिख(uart_fd, buf, offset) == offset;
}

तार्किक crsf_send_telemetry_flight_mode(पूर्णांक uart_fd, स्थिरांक अक्षर *flight_mode)
{
	स्थिरांक पूर्णांक max_length = 16;
	पूर्णांक length = म_माप(flight_mode) + 1;

	जर (length > max_length) {
		length = max_length;
	}

	uint8_t buf[max_length + 4];
	पूर्णांक offset = 0;
	write_frame_header(buf, offset, crsf_frame_type_t::flight_mode, length);
	स_नकल(buf + offset, flight_mode, length);
	offset += length;
	buf[offset - 1] = 0; // ensure null-terminated string
	write_frame_crc(buf, offset, length + 4);
	परत लिख(uart_fd, buf, offset) == offset;
}
