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

#ಆದರೆ 0 // enable non-verbose debugging
#ವ್ಯಾಖ್ಯಾನಿಸು CRSF_DEBUG PX4_WARN
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CRSF_DEBUG(...)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#ವ್ಯಾಖ್ಯಾನಿಸು CRSF_VERBOSE PX4_WARN
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CRSF_VERBOSE(...)
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>
#ಅನ್ತರ್ಭಾವ <termios.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "crsf.h"
#ಅನ್ತರ್ಭಾವ "common_rc.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MIN(a,b) (((a)<(b))?(a):(b))
#ವ್ಯಾಖ್ಯಾನಿಸು MAX(a,b) (((a)>(b))?(a):(b))


#ವ್ಯಾಖ್ಯಾನಿಸು CRSF_BAUDRATE 420000

#ವ್ಯಾಖ್ಯಾನಿಸು CRSF_SYNC_BYTE 0xC8

ಎಣಿಕೆ ವರ್ಗ crsf_ಚೌಕಟ್ಟು_type_t : uint8_t {
	gps = 0x02,
	battery_sensor = 0x08,
	baro_altitude = 0x09,
	link_statistics = 0x14,
	rc_channels_packed = 0x16,
	attitude = 0x1E,
	flight_ಬಹುಲಕ = 0x21,

	// Extended Header Frames, range: 0x28 to 0x96
	device_ping = 0x28,
	device_info = 0x29,
	parameter_settings_ನಮೂದು = 0x2B,
	parameter_read = 0x2C,
	parameter_write = 0x2D,
	command = 0x32
};

ಎಣಿಕೆ ವರ್ಗ crsf_payload_ಗಾತ್ರ_t : uint8_t {
	gps = 15,
	battery_sensor = 8,
	baro_altitude = 4, ///< altitude (uint16) + vertical speed (int16)
	link_statistics = 10,
	rc_channels = 22, ///< 11 bits per channel * 16 channels = 22 bytes.
	attitude = 6,
};


ಎಣಿಕೆ ವರ್ಗ crsf_ವಿಳಾಸ_t : uint8_t {
	ಪ್ರಸಾರ = 0x00,
	usb = 0x10,
	tbs_core_pnp_pro = 0x80,
	reserved1 = 0x8A,
	current_sensor = 0xC0,
	gps = 0xC2,
	tbs_blackbox = 0xC4,
	flight_controller = 0xC8,
	reserved2 = 0xCA,
	race_ಟ್ಯಾಗ್ = 0xCC,
	radio_transmitter = 0xEA,
	crsf_receiver = 0xEC,
	crsf_transmitter = 0xEE
};

#ಸೂಚನೆ pack(ತಳ್ಳು, 1)
ರಚನೆ crsf_payload_RC_channels_packed_t {
	// 176 bits of data (11 bits per channel * 16 channels) = 22 bytes
	ಚಿಹ್ನೆರಹಿತ chan0 : 11;
	ಚಿಹ್ನೆರಹಿತ chan1 : 11;
	ಚಿಹ್ನೆರಹಿತ chan2 : 11;
	ಚಿಹ್ನೆರಹಿತ chan3 : 11;
	ಚಿಹ್ನೆರಹಿತ chan4 : 11;
	ಚಿಹ್ನೆರಹಿತ chan5 : 11;
	ಚಿಹ್ನೆರಹಿತ chan6 : 11;
	ಚಿಹ್ನೆರಹಿತ chan7 : 11;
	ಚಿಹ್ನೆರಹಿತ chan8 : 11;
	ಚಿಹ್ನೆರಹಿತ chan9 : 11;
	ಚಿಹ್ನೆರಹಿತ chan10 : 11;
	ಚಿಹ್ನೆರಹಿತ chan11 : 11;
	ಚಿಹ್ನೆರಹಿತ chan12 : 11;
	ಚಿಹ್ನೆರಹಿತ chan13 : 11;
	ಚಿಹ್ನೆರಹಿತ chan14 : 11;
	ಚಿಹ್ನೆರಹಿತ chan15 : 11;
};

#ಸೂಚನೆ pack(ತೆಗೆ_2)

ಎಣಿಕೆ ವರ್ಗ crsf_parser_state_t : uint8_t {
	unsynced = 0,
	synced
};

ಸ್ಥಿರ crsf_ಚೌಕಟ್ಟು_t	 &crsf_ಚೌಕಟ್ಟು = rc_decode_buf.crsf_ಚೌಕಟ್ಟು;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ current_ಚೌಕಟ್ಟು_position = 0;
ಸ್ಥಿರ crsf_parser_state_t parser_state = crsf_parser_state_t::unsynced;

/**
 * parse the current crsf_frame buffer
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ crsf_parse_ಬಫರ್(uint16_t *ಮೌಲ್ಯಗಳು_2, uint16_t *ಅಂಕೆ_ಮೌಲ್ಯಗಳು, uint16_t max_channels);

uint8_t crsf_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ಸ್ಥಿರಾಂಕ crsf_ಚೌಕಟ್ಟು_t &ಚೌಕಟ್ಟು);


ಪೂರ್ಣಾಂಕ
crsf_config(ಪೂರ್ಣಾಂಕ uart_fd)
{
	ರಚನೆ termios t;

	/* no parity, one stop bit */
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, CRSF_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	ಹಿಂತಿರುಗಿ tcsetattr(uart_fd, TCSANOW, &t);
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
ಸ್ಥಿರ uint16_t convert_channel_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ chan_ಮೌಲ್ಯ);


ತಾರ್ಕಿಕ crsf_parse(ಸ್ಥಿರಾಂಕ uint64_t now, ಸ್ಥಿರಾಂಕ uint8_t *ಚೌಕಟ್ಟು, ಚಿಹ್ನೆರಹಿತ len, uint16_t *ಮೌಲ್ಯಗಳು_2,
		uint16_t *ಅಂಕೆ_ಮೌಲ್ಯಗಳು, uint16_t max_channels)
{
	ತಾರ್ಕಿಕ ret = ಸುಳ್ಳು;
	uint8_t *crsf_ಚೌಕಟ್ಟು_ptr = (uint8_t *)&crsf_ಚೌಕಟ್ಟು;

	ಆಗ (len > 0) {

		// fill in the crsf_buffer, as much as we can
		ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ current_len = MIN(len, ಗಾತ್ರ(crsf_ಚೌಕಟ್ಟು_t) - current_ಚೌಕಟ್ಟು_position);
		ಸ್ಮರಣೆ_ನಕಲು(crsf_ಚೌಕಟ್ಟು_ptr + current_ಚೌಕಟ್ಟು_position, ಚೌಕಟ್ಟು, current_len);
		current_ಚೌಕಟ್ಟು_position += current_len;

		// protection to guarantee parsing progress
		ಆದರೆ (current_len == 0) {
			CRSF_DEBUG("========== parser bug: no progress (%i) ===========", len);

			ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i < current_ಚೌಕಟ್ಟು_position; ++i) {
				CRSF_DEBUG("crsf_frame_ptr[%i]: 0x%x", i, (ಪೂರ್ಣಾಂಕ)crsf_ಚೌಕಟ್ಟು_ptr[i]);
			}

			// reset the parser
			current_ಚೌಕಟ್ಟು_position = 0;
			parser_state = crsf_parser_state_t::unsynced;
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}

		len -= current_len;
		ಚೌಕಟ್ಟು += current_len;

		ಆದರೆ (crsf_parse_ಬಫರ್(ಮೌಲ್ಯಗಳು_2, ಅಂಕೆ_ಮೌಲ್ಯಗಳು, max_channels)) {
			ret = ನಿಜ;
		}
	}


	ಹಿಂತಿರುಗಿ ret;
}

uint8_t crsf_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ಸ್ಥಿರಾಂಕ crsf_ಚೌಕಟ್ಟು_t &ಚೌಕಟ್ಟು)
{
	// CRC includes type and payload
	uint8_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = crc8_dvb_s2(0, ಚೌಕಟ್ಟು.type);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಚೌಕಟ್ಟು.ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ - 2; ++i) {
		ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = crc8_dvb_s2(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ, ಚೌಕಟ್ಟು.payload[i]);
	}

	ಹಿಂತಿರುಗಿ ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ;
}

ಸ್ಥಿರ uint16_t convert_channel_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ chan_ಮೌಲ್ಯ)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ scale = (2012.f - 988.f) / (1811.f - 172.f);
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ ಸ್ಥಾನಾಂತರ = 988.f - 172.f * scale;
	ಹಿಂತಿರುಗಿ (scale * chan_ಮೌಲ್ಯ) + ಸ್ಥಾನಾಂತರ;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ crsf_parse_ಬಫರ್(uint16_t *ಮೌಲ್ಯಗಳು_2, uint16_t *ಅಂಕೆ_ಮೌಲ್ಯಗಳು, uint16_t max_channels)
{
	uint8_t *crsf_ಚೌಕಟ್ಟು_ptr = (uint8_t *)&crsf_ಚೌಕಟ್ಟು;

	ಆದರೆ (parser_state == crsf_parser_state_t::unsynced) {
		// there is no sync byte, try to find an RC packet by searching for a matching frame length and type
		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 1; i < current_ಚೌಕಟ್ಟು_position - 1; ++i) {
			ಆದರೆ (crsf_ಚೌಕಟ್ಟು_ptr[i] == (uint8_t)crsf_payload_ಗಾತ್ರ_t::rc_channels + 2 &&
			    crsf_ಚೌಕಟ್ಟು_ptr[i + 1] == (uint8_t)crsf_ಚೌಕಟ್ಟು_type_t::rc_channels_packed) {
				parser_state = crsf_parser_state_t::synced;
				ಚಿಹ್ನೆರಹಿತ ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ = i - 1;
				CRSF_VERBOSE("RC channels found at offset %i", ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ);

				// move the rest of the buffer to the beginning
				ಆದರೆ (ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ != 0) {
					memmove(crsf_ಚೌಕಟ್ಟು_ptr, crsf_ಚೌಕಟ್ಟು_ptr + ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ, current_ಚೌಕಟ್ಟು_position - ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ);
					current_ಚೌಕಟ್ಟು_position -= ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ;
				}

				ನಿಲ್ಲಿಸು;
			}
		}
	}

	ಆದರೆ (parser_state != crsf_parser_state_t::synced) {
		ಆದರೆ (current_ಚೌಕಟ್ಟು_position >= ಗಾತ್ರ(crsf_ಚೌಕಟ್ಟು_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_ಚೌಕಟ್ಟು_position = 3;

			ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i < current_ಚೌಕಟ್ಟು_position; ++i) {
				crsf_ಚೌಕಟ್ಟು_ptr[i] = crsf_ಚೌಕಟ್ಟು_ptr[ಗಾತ್ರ(crsf_ಚೌಕಟ್ಟು_t) - current_ಚೌಕಟ್ಟು_position + i];
			}

			CRSF_VERBOSE("Discarding buffer");
		}

		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}


	ಆದರೆ (current_ಚೌಕಟ್ಟು_position < 3) {
		// wait until we have the header & type
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	// Now we have at least the header and the type

	ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ = crsf_ಚೌಕಟ್ಟು.ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ + ಗಾತ್ರ(crsf_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ_t);

	ಆದರೆ (current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ > ಗಾತ್ರ(crsf_ಚೌಕಟ್ಟು_t) || current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ < 4) {
		// frame too long or bogus -> discard everything and go into unsynced state
		current_ಚೌಕಟ್ಟು_position = 0;
		parser_state = crsf_parser_state_t::unsynced;
		CRSF_DEBUG("Frame too long/bogus (%i, type=%i) -> unsync", current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ, crsf_ಚೌಕಟ್ಟು.type);
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಆದರೆ (current_ಚೌಕಟ್ಟು_position < current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ) {
		// we don't have the full frame yet -> wait for more data
		CRSF_VERBOSE("waiting for more data (%i < %i)", current_ಚೌಕಟ್ಟು_position, current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ);
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ತಾರ್ಕಿಕ ret = ಸುಳ್ಳು;

	// Now we have the full frame

	ಆದರೆ (crsf_ಚೌಕಟ್ಟು.type == (uint8_t)crsf_ಚೌಕಟ್ಟು_type_t::rc_channels_packed &&
	    crsf_ಚೌಕಟ್ಟು.ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ == (uint8_t)crsf_payload_ಗಾತ್ರ_t::rc_channels + 2) {
		ಸ್ಥಿರಾಂಕ uint8_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = crsf_ಚೌಕಟ್ಟು.payload[crsf_ಚೌಕಟ್ಟು.ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ - 2];

		ಆದರೆ (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ == crsf_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(crsf_ಚೌಕಟ್ಟು)) {
			ಸ್ಥಿರಾಂಕ crsf_payload_RC_channels_packed_t *ಸ್ಥಿರಾಂಕ rc_channels =
				(crsf_payload_RC_channels_packed_t *)&crsf_ಚೌಕಟ್ಟು.payload;
			*ಅಂಕೆ_ಮೌಲ್ಯಗಳು = MIN(max_channels, 16);

			ಆದರೆ (max_channels > 0) { ಮೌಲ್ಯಗಳು_2[0] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan0); }

			ಆದರೆ (max_channels > 1) { ಮೌಲ್ಯಗಳು_2[1] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan1); }

			ಆದರೆ (max_channels > 2) { ಮೌಲ್ಯಗಳು_2[2] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan2); }

			ಆದರೆ (max_channels > 3) { ಮೌಲ್ಯಗಳು_2[3] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan3); }

			ಆದರೆ (max_channels > 4) { ಮೌಲ್ಯಗಳು_2[4] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan4); }

			ಆದರೆ (max_channels > 5) { ಮೌಲ್ಯಗಳು_2[5] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan5); }

			ಆದರೆ (max_channels > 6) { ಮೌಲ್ಯಗಳು_2[6] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan6); }

			ಆದರೆ (max_channels > 7) { ಮೌಲ್ಯಗಳು_2[7] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan7); }

			ಆದರೆ (max_channels > 8) { ಮೌಲ್ಯಗಳು_2[8] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan8); }

			ಆದರೆ (max_channels > 9) { ಮೌಲ್ಯಗಳು_2[9] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan9); }

			ಆದರೆ (max_channels > 10) { ಮೌಲ್ಯಗಳು_2[10] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan10); }

			ಆದರೆ (max_channels > 11) { ಮೌಲ್ಯಗಳು_2[11] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan11); }

			ಆದರೆ (max_channels > 12) { ಮೌಲ್ಯಗಳು_2[12] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan12); }

			ಆದರೆ (max_channels > 13) { ಮೌಲ್ಯಗಳು_2[13] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan13); }

			ಆದರೆ (max_channels > 14) { ಮೌಲ್ಯಗಳು_2[14] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan14); }

			ಆದರೆ (max_channels > 15) { ಮೌಲ್ಯಗಳು_2[15] = convert_channel_ಮೌಲ್ಯ(rc_channels->chan15); }

			CRSF_VERBOSE("Got Channels");

			ret = ನಿಜ;

		} ಇಲ್ಲದಿದ್ದರೆ {
			CRSF_DEBUG("CRC check failed");
		}

	} ಇಲ್ಲದಿದ್ದರೆ {
		CRSF_DEBUG("Got Non-RC frame (len=%i, type=%i)", current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ, crsf_ಚೌಕಟ್ಟು.type);
		// We could check the CRC here and reset the parser into unsynced state if it fails.
		// But in practise it's robust even without that.
	}

	// Either reset or move the rest of the buffer
	ಆದರೆ (current_ಚೌಕಟ್ಟು_position > current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ) {
		CRSF_VERBOSE("Moving buffer (%i > %i)", current_ಚೌಕಟ್ಟು_position, current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ);
		memmove(crsf_ಚೌಕಟ್ಟು_ptr, crsf_ಚೌಕಟ್ಟು_ptr + current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ, current_ಚೌಕಟ್ಟು_position - current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ);
		current_ಚೌಕಟ್ಟು_position -= current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		current_ಚೌಕಟ್ಟು_position = 0;
	}

	ಹಿಂತಿರುಗಿ ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_uint8_t(uint8_t *buf, ಪೂರ್ಣಾಂಕ &ಸ್ಥಾನಾಂತರ, uint8_t value)
{
	buf[ಸ್ಥಾನಾಂತರ++] = value;
}
/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_uint16_t(uint8_t *buf, ಪೂರ್ಣಾಂಕ &ಸ್ಥಾನಾಂತರ, uint16_t value)
{
	// Big endian
	buf[ಸ್ಥಾನಾಂತರ] = value >> 8;
	buf[ಸ್ಥಾನಾಂತರ + 1] = value & 0xff;
	ಸ್ಥಾನಾಂತರ += 2;
}
/**
 * write an uint24_t value to a buffer at a given offset and increment the offset
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_uint24_t(uint8_t *buf, ಪೂರ್ಣಾಂಕ &ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ value)
{
	// Big endian
	buf[ಸ್ಥಾನಾಂತರ] = value >> 16;
	buf[ಸ್ಥಾನಾಂತರ + 1] = (value >> 8) & 0xff;
	buf[ಸ್ಥಾನಾಂತರ + 2] = value & 0xff;
	ಸ್ಥಾನಾಂತರ += 3;
}

/**
 * write an int32_t value to a buffer at a given offset and increment the offset
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_int32_t(uint8_t *buf, ಪೂರ್ಣಾಂಕ &ಸ್ಥಾನಾಂತರ, int32_t value)
{
	// Big endian
	buf[ಸ್ಥಾನಾಂತರ] = value >> 24;
	buf[ಸ್ಥಾನಾಂತರ + 1] = (value >> 16) & 0xff;
	buf[ಸ್ಥಾನಾಂತರ + 2] = (value >> 8) & 0xff;
	buf[ಸ್ಥಾನಾಂತರ + 3] = value & 0xff;
	ಸ್ಥಾನಾಂತರ += 4;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ(uint8_t *buf, ಪೂರ್ಣಾಂಕ &ಸ್ಥಾನಾಂತರ, crsf_ಚೌಕಟ್ಟು_type_t type, uint8_t payload_ಗಾತ್ರ)
{
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, CRSF_SYNC_BYTE); // this got changed from the address to the sync byte
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, payload_ಗಾತ್ರ + 2);
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, (uint8_t)type);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(uint8_t *buf, ಪೂರ್ಣಾಂಕ &ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ buf_ಗಾತ್ರ)
{
	// CRC does not include the address and length
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, crc8_dvb_s2_buf(buf + 2, buf_ಗಾತ್ರ - 3));

	// check correctness of buffer size (only needed during development)
	//if (buf_size != offset) { PX4_ERR("frame size mismatch (%i != %i)", buf_size, offset); }
}

ತಾರ್ಕಿಕ crsf_send_telemetry_battery(ಪೂರ್ಣಾಂಕ uart_fd, uint16_t voltage, uint16_t current, ಪೂರ್ಣಾಂಕ fuel, uint8_t ಉಳಿದ)
{
	uint8_t buf[(uint8_t)crsf_payload_ಗಾತ್ರ_t::battery_sensor + 4];
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;
	write_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ(buf, ಸ್ಥಾನಾಂತರ, crsf_ಚೌಕಟ್ಟು_type_t::battery_sensor, (uint8_t)crsf_payload_ಗಾತ್ರ_t::battery_sensor);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, voltage);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, current);
	write_uint24_t(buf, ಸ್ಥಾನಾಂತರ, fuel);
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, ಉಳಿದ);
	write_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(buf, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ(buf));
	ಹಿಂತಿರುಗಿ ಲಿಖ(uart_fd, buf, ಸ್ಥಾನಾಂತರ) == ಸ್ಥಾನಾಂತರ;
}

ತಾರ್ಕಿಕ crsf_send_telemetry_gps(ಪೂರ್ಣಾಂಕ uart_fd, int32_t latitude, int32_t longitude, uint16_t groundspeed,
			     uint16_t gps_heading, uint16_t altitude, uint8_t ಅಂಕೆ_satellites)
{
	uint8_t buf[(uint8_t)crsf_payload_ಗಾತ್ರ_t::gps + 4];
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;
	write_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ(buf, ಸ್ಥಾನಾಂತರ, crsf_ಚೌಕಟ್ಟು_type_t::gps, (uint8_t)crsf_payload_ಗಾತ್ರ_t::gps);
	write_int32_t(buf, ಸ್ಥಾನಾಂತರ, latitude);
	write_int32_t(buf, ಸ್ಥಾನಾಂತರ, longitude);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, groundspeed);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, gps_heading);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, altitude);
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, ಅಂಕೆ_satellites);
	write_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(buf, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ(buf));
	ಹಿಂತಿರುಗಿ ಲಿಖ(uart_fd, buf, ಸ್ಥಾನಾಂತರ) == ಸ್ಥಾನಾಂತರ;
}

ತಾರ್ಕಿಕ crsf_send_telemetry_baro_altitude(ಪೂರ್ಣಾಂಕ uart_fd, uint16_t altitude, int16_t vertical_ವೇಗ)
{
	uint8_t buf[(uint8_t)crsf_payload_ಗಾತ್ರ_t::baro_altitude + 4];
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;
	write_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ(buf, ಸ್ಥಾನಾಂತರ, crsf_ಚೌಕಟ್ಟು_type_t::baro_altitude, (uint8_t)crsf_payload_ಗಾತ್ರ_t::baro_altitude);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, altitude);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, (uint16_t)vertical_ವೇಗ);
	write_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(buf, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ(buf));
	ಹಿಂತಿರುಗಿ ಲಿಖ(uart_fd, buf, ಸ್ಥಾನಾಂತರ) == ಸ್ಥಾನಾಂತರ;
}

ತಾರ್ಕಿಕ crsf_send_telemetry_attitude(ಪೂರ್ಣಾಂಕ uart_fd, int16_t pitch, int16_t roll, int16_t yaw)
{
	uint8_t buf[(uint8_t)crsf_payload_ಗಾತ್ರ_t::attitude + 4];
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;
	write_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ(buf, ಸ್ಥಾನಾಂತರ, crsf_ಚೌಕಟ್ಟು_type_t::attitude, (uint8_t)crsf_payload_ಗಾತ್ರ_t::attitude);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, pitch);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, roll);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, yaw);
	write_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(buf, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ(buf));
	ಹಿಂತಿರುಗಿ ಲಿಖ(uart_fd, buf, ಸ್ಥಾನಾಂತರ) == ಸ್ಥಾನಾಂತರ;
}

ತಾರ್ಕಿಕ crsf_send_telemetry_flight_ಬಹುಲಕ(ಪೂರ್ಣಾಂಕ uart_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *flight_ಬಹುಲಕ)
{
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ max_ಉದ್ದಳತೆ = 16;
	ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(flight_ಬಹುಲಕ) + 1;

	ಆದರೆ (ಉದ್ದಳತೆ > max_ಉದ್ದಳತೆ) {
		ಉದ್ದಳತೆ = max_ಉದ್ದಳತೆ;
	}

	uint8_t buf[max_ಉದ್ದಳತೆ + 4];
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;
	write_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ(buf, ಸ್ಥಾನಾಂತರ, crsf_ಚೌಕಟ್ಟು_type_t::flight_ಬಹುಲಕ, ಉದ್ದಳತೆ);
	ಸ್ಮರಣೆ_ನಕಲು(buf + ಸ್ಥಾನಾಂತರ, flight_ಬಹುಲಕ, ಉದ್ದಳತೆ);
	ಸ್ಥಾನಾಂತರ += ಉದ್ದಳತೆ;
	buf[ಸ್ಥಾನಾಂತರ - 1] = 0; // ensure null-terminated string
	write_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(buf, ಸ್ಥಾನಾಂತರ, ಉದ್ದಳತೆ + 4);
	ಹಿಂತಿರುಗಿ ಲಿಖ(uart_fd, buf, ಸ್ಥಾನಾಂತರ) == ಸ್ಥಾನಾಂತರ;
}
