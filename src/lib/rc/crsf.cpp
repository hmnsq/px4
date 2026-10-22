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

#ਜੇ 0 // enable non-verbose debugging
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CRSF_DEBUG PX4_WARN
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CRSF_DEBUG(...)
#ਜੇ_ਅੰਤ

#ਜੇ 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CRSF_VERBOSE PX4_WARN
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CRSF_VERBOSE(...)
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <termios.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "crsf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "common_rc.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIN(a,b) (((a)<(b))?(a):(b))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX(a,b) (((a)>(b))?(a):(b))


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CRSF_BAUDRATE 420000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CRSF_SYNC_BYTE 0xC8

ਗਿਣਤੀ ਵਰਗ crsf_ਚੌਖਟਾ_type_t : uint8_t {
	gps = 0x02,
	battery_sensor = 0x08,
	baro_altitude = 0x09,
	link_statistics = 0x14,
	rc_channels_packed = 0x16,
	attitude = 0x1E,
	flight_ਬਹੁਲਕ = 0x21,

	// Extended Header Frames, range: 0x28 to 0x96
	device_ping = 0x28,
	device_info = 0x29,
	parameter_settings_ਇੰਦਰਾਜ = 0x2B,
	parameter_read = 0x2C,
	parameter_write = 0x2D,
	command = 0x32
};

ਗਿਣਤੀ ਵਰਗ crsf_payload_ਮਾਪ_t : uint8_t {
	gps = 15,
	battery_sensor = 8,
	baro_altitude = 4, ///< altitude (uint16) + vertical speed (int16)
	link_statistics = 10,
	rc_channels = 22, ///< 11 bits per channel * 16 channels = 22 bytes.
	attitude = 6,
};


ਗਿਣਤੀ ਵਰਗ crsf_ਪਤਾ_t : uint8_t {
	ਪ੍ਰਸਾਰਣ = 0x00,
	usb = 0x10,
	tbs_core_pnp_pro = 0x80,
	reserved1 = 0x8A,
	current_sensor = 0xC0,
	gps = 0xC2,
	tbs_blackbox = 0xC4,
	flight_controller = 0xC8,
	reserved2 = 0xCA,
	race_ਟੈਗ = 0xCC,
	radio_transmitter = 0xEA,
	crsf_receiver = 0xEC,
	crsf_transmitter = 0xEE
};

#ਹਦਾਇਤ pack(ਧੱਕੋ, 1)
ਬਣਤਰ crsf_payload_RC_channels_packed_t {
	// 176 bits of data (11 bits per channel * 16 channels) = 22 bytes
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan0 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan1 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan2 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan3 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan4 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan5 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan6 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan7 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan8 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan9 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan10 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan11 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan12 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan13 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan14 : 11;
	ਬਿਨਾਨਿਸ਼ਾਨੀ chan15 : 11;
};

#ਹਦਾਇਤ pack(ਕੱਢੋ)

ਗਿਣਤੀ ਵਰਗ crsf_parser_state_t : uint8_t {
	unsynced = 0,
	synced
};

ਸਥਿਰ crsf_ਚੌਖਟਾ_t	 &crsf_ਚੌਖਟਾ = rc_decode_buf.crsf_ਚੌਖਟਾ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ current_ਚੌਖਟਾ_position = 0;
ਸਥਿਰ crsf_parser_state_t parser_state = crsf_parser_state_t::unsynced;

/**
 * parse the current crsf_frame buffer
 */
ਸਥਿਰ ਤਾਰਕਿਕ crsf_parse_ਬਫਰ(uint16_t *ਮੁੱਲਾਂ_2, uint16_t *ਅੰਕ_ਮੁੱਲਾਂ, uint16_t max_channels);

uint8_t crsf_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(ਅਚਲ crsf_ਚੌਖਟਾ_t &ਚੌਖਟਾ);


ਪੂਰਨ
crsf_config(ਪੂਰਨ uart_fd)
{
	ਬਣਤਰ termios t;

	/* no parity, one stop bit */
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, CRSF_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	ਵਾਪਸ tcsetattr(uart_fd, TCSANOW, &t);
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
ਸਥਿਰ uint16_t convert_channel_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ chan_ਮੁੱਲ);


ਤਾਰਕਿਕ crsf_parse(ਅਚਲ uint64_t now, ਅਚਲ uint8_t *ਚੌਖਟਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ len, uint16_t *ਮੁੱਲਾਂ_2,
		uint16_t *ਅੰਕ_ਮੁੱਲਾਂ, uint16_t max_channels)
{
	ਤਾਰਕਿਕ ret = ਝੂਠ;
	uint8_t *crsf_ਚੌਖਟਾ_ptr = (uint8_t *)&crsf_ਚੌਖਟਾ;

	ਜਦੋਂ (len > 0) {

		// fill in the crsf_buffer, as much as we can
		ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ current_len = MIN(len, ਆਕਾਰ(crsf_ਚੌਖਟਾ_t) - current_ਚੌਖਟਾ_position);
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(crsf_ਚੌਖਟਾ_ptr + current_ਚੌਖਟਾ_position, ਚੌਖਟਾ, current_len);
		current_ਚੌਖਟਾ_position += current_len;

		// protection to guarantee parsing progress
		ਜੇ (current_len == 0) {
			CRSF_DEBUG("========== parser bug: no progress (%i) ===========", len);

			ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < current_ਚੌਖਟਾ_position; ++i) {
				CRSF_DEBUG("crsf_frame_ptr[%i]: 0x%x", i, (ਪੂਰਨ)crsf_ਚੌਖਟਾ_ptr[i]);
			}

			// reset the parser
			current_ਚੌਖਟਾ_position = 0;
			parser_state = crsf_parser_state_t::unsynced;
			ਵਾਪਸ ਝੂਠ;
		}

		len -= current_len;
		ਚੌਖਟਾ += current_len;

		ਜੇ (crsf_parse_ਬਫਰ(ਮੁੱਲਾਂ_2, ਅੰਕ_ਮੁੱਲਾਂ, max_channels)) {
			ret = ਸੱਚ;
		}
	}


	ਵਾਪਸ ret;
}

uint8_t crsf_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(ਅਚਲ crsf_ਚੌਖਟਾ_t &ਚੌਖਟਾ)
{
	// CRC includes type and payload
	uint8_t ਚੱਕਰੀ_ਜਾਂਚ = crc8_dvb_s2(0, ਚੌਖਟਾ.type);

	ਲਈ (ਪੂਰਨ i = 0; i < ਚੌਖਟਾ.ਸਿਰਲੇਖ.ਲੰਮਾਈ - 2; ++i) {
		ਚੱਕਰੀ_ਜਾਂਚ = crc8_dvb_s2(ਚੱਕਰੀ_ਜਾਂਚ, ਚੌਖਟਾ.payload[i]);
	}

	ਵਾਪਸ ਚੱਕਰੀ_ਜਾਂਚ;
}

ਸਥਿਰ uint16_t convert_channel_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ chan_ਮੁੱਲ)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ scale = (2012.f - 988.f) / (1811.f - 172.f);
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ ਵਿੱਥ = 988.f - 172.f * scale;
	ਵਾਪਸ (scale * chan_ਮੁੱਲ) + ਵਿੱਥ;
}

ਸਥਿਰ ਤਾਰਕਿਕ crsf_parse_ਬਫਰ(uint16_t *ਮੁੱਲਾਂ_2, uint16_t *ਅੰਕ_ਮੁੱਲਾਂ, uint16_t max_channels)
{
	uint8_t *crsf_ਚੌਖਟਾ_ptr = (uint8_t *)&crsf_ਚੌਖਟਾ;

	ਜੇ (parser_state == crsf_parser_state_t::unsynced) {
		// there is no sync byte, try to find an RC packet by searching for a matching frame length and type
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 1; i < current_ਚੌਖਟਾ_position - 1; ++i) {
			ਜੇ (crsf_ਚੌਖਟਾ_ptr[i] == (uint8_t)crsf_payload_ਮਾਪ_t::rc_channels + 2 &&
			    crsf_ਚੌਖਟਾ_ptr[i + 1] == (uint8_t)crsf_ਚੌਖਟਾ_type_t::rc_channels_packed) {
				parser_state = crsf_parser_state_t::synced;
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌਖਟਾ_ਵਿੱਥ = i - 1;
				CRSF_VERBOSE("RC channels found at offset %i", ਚੌਖਟਾ_ਵਿੱਥ);

				// move the rest of the buffer to the beginning
				ਜੇ (ਚੌਖਟਾ_ਵਿੱਥ != 0) {
					memmove(crsf_ਚੌਖਟਾ_ptr, crsf_ਚੌਖਟਾ_ptr + ਚੌਖਟਾ_ਵਿੱਥ, current_ਚੌਖਟਾ_position - ਚੌਖਟਾ_ਵਿੱਥ);
					current_ਚੌਖਟਾ_position -= ਚੌਖਟਾ_ਵਿੱਥ;
				}

				ਰੋਕੋ;
			}
		}
	}

	ਜੇ (parser_state != crsf_parser_state_t::synced) {
		ਜੇ (current_ਚੌਖਟਾ_position >= ਆਕਾਰ(crsf_ਚੌਖਟਾ_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_ਚੌਖਟਾ_position = 3;

			ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < current_ਚੌਖਟਾ_position; ++i) {
				crsf_ਚੌਖਟਾ_ptr[i] = crsf_ਚੌਖਟਾ_ptr[ਆਕਾਰ(crsf_ਚੌਖਟਾ_t) - current_ਚੌਖਟਾ_position + i];
			}

			CRSF_VERBOSE("Discarding buffer");
		}

		ਵਾਪਸ ਝੂਠ;
	}


	ਜੇ (current_ਚੌਖਟਾ_position < 3) {
		// wait until we have the header & type
		ਵਾਪਸ ਝੂਠ;
	}

	// Now we have at least the header and the type

	ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ current_ਚੌਖਟਾ_ਲੰਮਾਈ = crsf_ਚੌਖਟਾ.ਸਿਰਲੇਖ.ਲੰਮਾਈ + ਆਕਾਰ(crsf_ਚੌਖਟਾ_ਸਿਰਲੇਖ_t);

	ਜੇ (current_ਚੌਖਟਾ_ਲੰਮਾਈ > ਆਕਾਰ(crsf_ਚੌਖਟਾ_t) || current_ਚੌਖਟਾ_ਲੰਮਾਈ < 4) {
		// frame too long or bogus -> discard everything and go into unsynced state
		current_ਚੌਖਟਾ_position = 0;
		parser_state = crsf_parser_state_t::unsynced;
		CRSF_DEBUG("Frame too long/bogus (%i, type=%i) -> unsync", current_ਚੌਖਟਾ_ਲੰਮਾਈ, crsf_ਚੌਖਟਾ.type);
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (current_ਚੌਖਟਾ_position < current_ਚੌਖਟਾ_ਲੰਮਾਈ) {
		// we don't have the full frame yet -> wait for more data
		CRSF_VERBOSE("waiting for more data (%i < %i)", current_ਚੌਖਟਾ_position, current_ਚੌਖਟਾ_ਲੰਮਾਈ);
		ਵਾਪਸ ਝੂਠ;
	}

	ਤਾਰਕਿਕ ret = ਝੂਠ;

	// Now we have the full frame

	ਜੇ (crsf_ਚੌਖਟਾ.type == (uint8_t)crsf_ਚੌਖਟਾ_type_t::rc_channels_packed &&
	    crsf_ਚੌਖਟਾ.ਸਿਰਲੇਖ.ਲੰਮਾਈ == (uint8_t)crsf_payload_ਮਾਪ_t::rc_channels + 2) {
		ਅਚਲ uint8_t ਚੱਕਰੀ_ਜਾਂਚ = crsf_ਚੌਖਟਾ.payload[crsf_ਚੌਖਟਾ.ਸਿਰਲੇਖ.ਲੰਮਾਈ - 2];

		ਜੇ (ਚੱਕਰੀ_ਜਾਂਚ == crsf_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(crsf_ਚੌਖਟਾ)) {
			ਅਚਲ crsf_payload_RC_channels_packed_t *ਅਚਲ rc_channels =
				(crsf_payload_RC_channels_packed_t *)&crsf_ਚੌਖਟਾ.payload;
			*ਅੰਕ_ਮੁੱਲਾਂ = MIN(max_channels, 16);

			ਜੇ (max_channels > 0) { ਮੁੱਲਾਂ_2[0] = convert_channel_ਮੁੱਲ(rc_channels->chan0); }

			ਜੇ (max_channels > 1) { ਮੁੱਲਾਂ_2[1] = convert_channel_ਮੁੱਲ(rc_channels->chan1); }

			ਜੇ (max_channels > 2) { ਮੁੱਲਾਂ_2[2] = convert_channel_ਮੁੱਲ(rc_channels->chan2); }

			ਜੇ (max_channels > 3) { ਮੁੱਲਾਂ_2[3] = convert_channel_ਮੁੱਲ(rc_channels->chan3); }

			ਜੇ (max_channels > 4) { ਮੁੱਲਾਂ_2[4] = convert_channel_ਮੁੱਲ(rc_channels->chan4); }

			ਜੇ (max_channels > 5) { ਮੁੱਲਾਂ_2[5] = convert_channel_ਮੁੱਲ(rc_channels->chan5); }

			ਜੇ (max_channels > 6) { ਮੁੱਲਾਂ_2[6] = convert_channel_ਮੁੱਲ(rc_channels->chan6); }

			ਜੇ (max_channels > 7) { ਮੁੱਲਾਂ_2[7] = convert_channel_ਮੁੱਲ(rc_channels->chan7); }

			ਜੇ (max_channels > 8) { ਮੁੱਲਾਂ_2[8] = convert_channel_ਮੁੱਲ(rc_channels->chan8); }

			ਜੇ (max_channels > 9) { ਮੁੱਲਾਂ_2[9] = convert_channel_ਮੁੱਲ(rc_channels->chan9); }

			ਜੇ (max_channels > 10) { ਮੁੱਲਾਂ_2[10] = convert_channel_ਮੁੱਲ(rc_channels->chan10); }

			ਜੇ (max_channels > 11) { ਮੁੱਲਾਂ_2[11] = convert_channel_ਮੁੱਲ(rc_channels->chan11); }

			ਜੇ (max_channels > 12) { ਮੁੱਲਾਂ_2[12] = convert_channel_ਮੁੱਲ(rc_channels->chan12); }

			ਜੇ (max_channels > 13) { ਮੁੱਲਾਂ_2[13] = convert_channel_ਮੁੱਲ(rc_channels->chan13); }

			ਜੇ (max_channels > 14) { ਮੁੱਲਾਂ_2[14] = convert_channel_ਮੁੱਲ(rc_channels->chan14); }

			ਜੇ (max_channels > 15) { ਮੁੱਲਾਂ_2[15] = convert_channel_ਮੁੱਲ(rc_channels->chan15); }

			CRSF_VERBOSE("Got Channels");

			ret = ਸੱਚ;

		} ਨਹੀਂਤਾਂ {
			CRSF_DEBUG("CRC check failed");
		}

	} ਨਹੀਂਤਾਂ {
		CRSF_DEBUG("Got Non-RC frame (len=%i, type=%i)", current_ਚੌਖਟਾ_ਲੰਮਾਈ, crsf_ਚੌਖਟਾ.type);
		// We could check the CRC here and reset the parser into unsynced state if it fails.
		// But in practise it's robust even without that.
	}

	// Either reset or move the rest of the buffer
	ਜੇ (current_ਚੌਖਟਾ_position > current_ਚੌਖਟਾ_ਲੰਮਾਈ) {
		CRSF_VERBOSE("Moving buffer (%i > %i)", current_ਚੌਖਟਾ_position, current_ਚੌਖਟਾ_ਲੰਮਾਈ);
		memmove(crsf_ਚੌਖਟਾ_ptr, crsf_ਚੌਖਟਾ_ptr + current_ਚੌਖਟਾ_ਲੰਮਾਈ, current_ਚੌਖਟਾ_position - current_ਚੌਖਟਾ_ਲੰਮਾਈ);
		current_ਚੌਖਟਾ_position -= current_ਚੌਖਟਾ_ਲੰਮਾਈ;

	} ਨਹੀਂਤਾਂ {
		current_ਚੌਖਟਾ_position = 0;
	}

	ਵਾਪਸ ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_uint8_t(uint8_t *buf, ਪੂਰਨ &ਵਿੱਥ, uint8_t value)
{
	buf[ਵਿੱਥ++] = value;
}
/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_uint16_t(uint8_t *buf, ਪੂਰਨ &ਵਿੱਥ, uint16_t value)
{
	// Big endian
	buf[ਵਿੱਥ] = value >> 8;
	buf[ਵਿੱਥ + 1] = value & 0xff;
	ਵਿੱਥ += 2;
}
/**
 * write an uint24_t value to a buffer at a given offset and increment the offset
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_uint24_t(uint8_t *buf, ਪੂਰਨ &ਵਿੱਥ, ਪੂਰਨ value)
{
	// Big endian
	buf[ਵਿੱਥ] = value >> 16;
	buf[ਵਿੱਥ + 1] = (value >> 8) & 0xff;
	buf[ਵਿੱਥ + 2] = value & 0xff;
	ਵਿੱਥ += 3;
}

/**
 * write an int32_t value to a buffer at a given offset and increment the offset
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_int32_t(uint8_t *buf, ਪੂਰਨ &ਵਿੱਥ, int32_t value)
{
	// Big endian
	buf[ਵਿੱਥ] = value >> 24;
	buf[ਵਿੱਥ + 1] = (value >> 16) & 0xff;
	buf[ਵਿੱਥ + 2] = (value >> 8) & 0xff;
	buf[ਵਿੱਥ + 3] = value & 0xff;
	ਵਿੱਥ += 4;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_ਚੌਖਟਾ_ਸਿਰਲੇਖ(uint8_t *buf, ਪੂਰਨ &ਵਿੱਥ, crsf_ਚੌਖਟਾ_type_t type, uint8_t payload_ਮਾਪ)
{
	write_uint8_t(buf, ਵਿੱਥ, CRSF_SYNC_BYTE); // this got changed from the address to the sync byte
	write_uint8_t(buf, ਵਿੱਥ, payload_ਮਾਪ + 2);
	write_uint8_t(buf, ਵਿੱਥ, (uint8_t)type);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(uint8_t *buf, ਪੂਰਨ &ਵਿੱਥ, ਪੂਰਨ buf_ਮਾਪ)
{
	// CRC does not include the address and length
	write_uint8_t(buf, ਵਿੱਥ, crc8_dvb_s2_buf(buf + 2, buf_ਮਾਪ - 3));

	// check correctness of buffer size (only needed during development)
	//if (buf_size != offset) { PX4_ERR("frame size mismatch (%i != %i)", buf_size, offset); }
}

ਤਾਰਕਿਕ crsf_send_telemetry_battery(ਪੂਰਨ uart_fd, uint16_t voltage, uint16_t current, ਪੂਰਨ fuel, uint8_t ਬਚਿਆ)
{
	uint8_t buf[(uint8_t)crsf_payload_ਮਾਪ_t::battery_sensor + 4];
	ਪੂਰਨ ਵਿੱਥ = 0;
	write_ਚੌਖਟਾ_ਸਿਰਲੇਖ(buf, ਵਿੱਥ, crsf_ਚੌਖਟਾ_type_t::battery_sensor, (uint8_t)crsf_payload_ਮਾਪ_t::battery_sensor);
	write_uint16_t(buf, ਵਿੱਥ, voltage);
	write_uint16_t(buf, ਵਿੱਥ, current);
	write_uint24_t(buf, ਵਿੱਥ, fuel);
	write_uint8_t(buf, ਵਿੱਥ, ਬਚਿਆ);
	write_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(buf, ਵਿੱਥ, ਆਕਾਰ(buf));
	ਵਾਪਸ ਲਿਖ(uart_fd, buf, ਵਿੱਥ) == ਵਿੱਥ;
}

ਤਾਰਕਿਕ crsf_send_telemetry_gps(ਪੂਰਨ uart_fd, int32_t latitude, int32_t longitude, uint16_t groundspeed,
			     uint16_t gps_heading, uint16_t altitude, uint8_t ਅੰਕ_satellites)
{
	uint8_t buf[(uint8_t)crsf_payload_ਮਾਪ_t::gps + 4];
	ਪੂਰਨ ਵਿੱਥ = 0;
	write_ਚੌਖਟਾ_ਸਿਰਲੇਖ(buf, ਵਿੱਥ, crsf_ਚੌਖਟਾ_type_t::gps, (uint8_t)crsf_payload_ਮਾਪ_t::gps);
	write_int32_t(buf, ਵਿੱਥ, latitude);
	write_int32_t(buf, ਵਿੱਥ, longitude);
	write_uint16_t(buf, ਵਿੱਥ, groundspeed);
	write_uint16_t(buf, ਵਿੱਥ, gps_heading);
	write_uint16_t(buf, ਵਿੱਥ, altitude);
	write_uint8_t(buf, ਵਿੱਥ, ਅੰਕ_satellites);
	write_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(buf, ਵਿੱਥ, ਆਕਾਰ(buf));
	ਵਾਪਸ ਲਿਖ(uart_fd, buf, ਵਿੱਥ) == ਵਿੱਥ;
}

ਤਾਰਕਿਕ crsf_send_telemetry_baro_altitude(ਪੂਰਨ uart_fd, uint16_t altitude, int16_t vertical_ਰਫਤਾਰ)
{
	uint8_t buf[(uint8_t)crsf_payload_ਮਾਪ_t::baro_altitude + 4];
	ਪੂਰਨ ਵਿੱਥ = 0;
	write_ਚੌਖਟਾ_ਸਿਰਲੇਖ(buf, ਵਿੱਥ, crsf_ਚੌਖਟਾ_type_t::baro_altitude, (uint8_t)crsf_payload_ਮਾਪ_t::baro_altitude);
	write_uint16_t(buf, ਵਿੱਥ, altitude);
	write_uint16_t(buf, ਵਿੱਥ, (uint16_t)vertical_ਰਫਤਾਰ);
	write_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(buf, ਵਿੱਥ, ਆਕਾਰ(buf));
	ਵਾਪਸ ਲਿਖ(uart_fd, buf, ਵਿੱਥ) == ਵਿੱਥ;
}

ਤਾਰਕਿਕ crsf_send_telemetry_attitude(ਪੂਰਨ uart_fd, int16_t pitch, int16_t roll, int16_t yaw)
{
	uint8_t buf[(uint8_t)crsf_payload_ਮਾਪ_t::attitude + 4];
	ਪੂਰਨ ਵਿੱਥ = 0;
	write_ਚੌਖਟਾ_ਸਿਰਲੇਖ(buf, ਵਿੱਥ, crsf_ਚੌਖਟਾ_type_t::attitude, (uint8_t)crsf_payload_ਮਾਪ_t::attitude);
	write_uint16_t(buf, ਵਿੱਥ, pitch);
	write_uint16_t(buf, ਵਿੱਥ, roll);
	write_uint16_t(buf, ਵਿੱਥ, yaw);
	write_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(buf, ਵਿੱਥ, ਆਕਾਰ(buf));
	ਵਾਪਸ ਲਿਖ(uart_fd, buf, ਵਿੱਥ) == ਵਿੱਥ;
}

ਤਾਰਕਿਕ crsf_send_telemetry_flight_ਬਹੁਲਕ(ਪੂਰਨ uart_fd, ਅਚਲ ਅੱਖਰ *flight_ਬਹੁਲਕ)
{
	ਅਚਲ ਪੂਰਨ max_ਲੰਮਾਈ = 16;
	ਪੂਰਨ ਲੰਮਾਈ = ਲੜੀ_ਲੰਬਾਈ(flight_ਬਹੁਲਕ) + 1;

	ਜੇ (ਲੰਮਾਈ > max_ਲੰਮਾਈ) {
		ਲੰਮਾਈ = max_ਲੰਮਾਈ;
	}

	uint8_t buf[max_ਲੰਮਾਈ + 4];
	ਪੂਰਨ ਵਿੱਥ = 0;
	write_ਚੌਖਟਾ_ਸਿਰਲੇਖ(buf, ਵਿੱਥ, crsf_ਚੌਖਟਾ_type_t::flight_ਬਹੁਲਕ, ਲੰਮਾਈ);
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf + ਵਿੱਥ, flight_ਬਹੁਲਕ, ਲੰਮਾਈ);
	ਵਿੱਥ += ਲੰਮਾਈ;
	buf[ਵਿੱਥ - 1] = 0; // ensure null-terminated string
	write_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(buf, ਵਿੱਥ, ਲੰਮਾਈ + 4);
	ਵਾਪਸ ਲਿਖ(uart_fd, buf, ਵਿੱਥ) == ਵਿੱਥ;
}
