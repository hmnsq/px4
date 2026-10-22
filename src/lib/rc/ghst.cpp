/****************************************************************************
 *
 *   Copyright (c) 2021 PX4 Development Team. All rights reserved.
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
 * @file ghst.cpp
 *
 * RC protocol definition for IRC Ghost (Immersion RC Ghost).
 *
 * @author Igor Misic <igy1000mb@gmail.com>
 * @author Juraj Ciberlin <jciberlin1@gmail.com>
 */

#ಆದರೆ 0 // enable non-verbose debugging
#ವ್ಯಾಖ್ಯಾನಿಸು GHST_DEBUG PX4_WARN
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು GHST_DEBUG(...)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#ವ್ಯಾಖ್ಯಾನಿಸು GHST_VERBOSE PX4_WARN
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು GHST_VERBOSE(...)
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>
#ಅನ್ತರ್ಭಾವ <termios.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <math.h>

// TODO: include RSSI dBm to percentage conversion for ghost receiver
#ಅನ್ತರ್ಭಾವ "spektrum_rssi.h"

#ಅನ್ತರ್ಭಾವ "ghst.hpp"
#ಅನ್ತರ್ಭಾವ "common_rc.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MIN(a,b) (((a)<(b))?(a):(b))

#ವ್ಯಾಖ್ಯಾನಿಸು GHST_ಚೌಕಟ್ಟು_PAYLOAD_ಗಾತ್ರ_TELEMETRY	(10U)
#ವ್ಯಾಖ್ಯಾನಿಸು GHST_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಗಾತ್ರ			(1U)
#ವ್ಯಾಖ್ಯಾನಿಸು GHST_ಚೌಕಟ್ಟು_TYPE_ಗಾತ್ರ			(1U)
#ವ್ಯಾಖ್ಯಾನಿಸು GHST_TYPE_ದತ್ತಾಂಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಗಾತ್ರ			(12U)
#ವ್ಯಾಖ್ಯಾನಿಸು GHST_ADDR_FC				(130U)
#ವ್ಯಾಖ್ಯಾನಿಸು GHST_MAX_ಅಂಕೆ_CHANNELS			(16)

ಎಣಿಕೆ ವರ್ಗ ghst_parser_state_t : uint8_t {
	unsynced = 0U,
	synced
};

// only RSSI frame contains value of RSSI, if it is not received, send last received RSSI/LQ
ಸ್ಥಿರ ghstLinkStatistics_t ಕೊನೆಯ_link_stats = { .rssi_pct = -1, .rssi_dbm = NAN, .link_quality = 0 };

ಸ್ಥಿರ ghst_ಚೌಕಟ್ಟು_t &ghst_ಚೌಕಟ್ಟು = rc_decode_buf.ghst_ಚೌಕಟ್ಟು;
ಸ್ಥಿರ uint32_t current_ಚೌಕಟ್ಟು_position = 0U;
ಸ್ಥಿರ ghst_parser_state_t parser_state = ghst_parser_state_t::unsynced;

ಸ್ಥಿರ uint16_t prev_rc_vals[GHST_MAX_ಅಂಕೆ_CHANNELS];

/**
 * parse the current ghst_frame buffer
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ ghst_parse_ಬಫರ್(uint16_t *ಮೌಲ್ಯಗಳು_2, ghstLinkStatistics_t *link_stats, uint16_t *ಅಂಕೆ_ಮೌಲ್ಯಗಳು,
			      uint16_t max_channels);

ಪೂರ್ಣಾಂಕ ghst_config(ಪೂರ್ಣಾಂಕ uart_fd)
{
	ರಚನೆ termios t;
	ಪೂರ್ಣಾಂಕ ret_val;

	// no parity, one stop bit
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, GHST_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	memset(prev_rc_vals, static_cast<ಪೂರ್ಣಾಂಕ>(UINT16_MAX), ಗಾತ್ರ(uint16_t) * GHST_MAX_ಅಂಕೆ_CHANNELS);
	ret_val = tcsetattr(uart_fd, TCSANOW, &t);
	ಹಿಂತಿರುಗಿ ret_val;
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
ಸ್ಥಿರ uint16_t convert_channel_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ chan_ಮೌಲ್ಯ);


ತಾರ್ಕಿಕ ghst_parse(ಸ್ಥಿರಾಂಕ uint64_t now, ಸ್ಥಿರಾಂಕ uint8_t *ಚೌಕಟ್ಟು, ಚಿಹ್ನೆರಹಿತ len, uint16_t *ಮೌಲ್ಯಗಳು_2,
		ghstLinkStatistics_t *link_stats, uint16_t *ಅಂಕೆ_ಮೌಲ್ಯಗಳು, uint16_t max_channels)
{
	ತಾರ್ಕಿಕ success = ಸುಳ್ಳು;
	uint8_t *ghst_ಚೌಕಟ್ಟು_ptr = (uint8_t *)&ghst_ಚೌಕಟ್ಟು;

	ಸ್ಮರಣೆ_ನಕಲು(ಮೌಲ್ಯಗಳು_2, prev_rc_vals, ಗಾತ್ರ(uint16_t) * GHST_MAX_ಅಂಕೆ_CHANNELS);

	ಆಗ (len > 0U) {

		// fill in the ghst_buffer, as much as we can
		ಸ್ಥಿರಾಂಕ uint32_t current_len = MIN(len, ಗಾತ್ರ(ghst_ಚೌಕಟ್ಟು_t) - current_ಚೌಕಟ್ಟು_position);
		ಸ್ಮರಣೆ_ನಕಲು(ghst_ಚೌಕಟ್ಟು_ptr + current_ಚೌಕಟ್ಟು_position, ಚೌಕಟ್ಟು, current_len);
		current_ಚೌಕಟ್ಟು_position += current_len;

		// protection to guarantee parsing progress
		ಆದರೆ (current_len == 0U) {
			GHST_DEBUG("========== parser bug: no progress (%u) ===========", len);

			ಗಾಗಿ (uint32_t i = 0U; i < current_ಚೌಕಟ್ಟು_position; ++i) {
				GHST_DEBUG("ghst_frame_ptr[%u]: 0x%x", i, ghst_ಚೌಕಟ್ಟು_ptr[i]);
			}

			// reset the parser
			current_ಚೌಕಟ್ಟು_position = 0U;
			parser_state = ghst_parser_state_t::unsynced;
			success = ಸುಳ್ಳು;

		} ಇಲ್ಲದಿದ್ದರೆ {
			len -= current_len;
			ಚೌಕಟ್ಟು += current_len;

			ಆದರೆ (ghst_parse_ಬಫರ್(ಮೌಲ್ಯಗಳು_2, link_stats, ಅಂಕೆ_ಮೌಲ್ಯಗಳು, max_channels)) {
				success = ನಿಜ;
			}
		}
	}

	ಹಿಂತಿರುಗಿ success;
}

uint8_t ghst_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ಸ್ಥಿರಾಂಕ ghst_ಚೌಕಟ್ಟು_t &ಚೌಕಟ್ಟು)
{
	uint8_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = crc8_dvb_s2(0U, ಚೌಕಟ್ಟು.type);

	ಗಾಗಿ (uint32_t i = 0U; i < ಚೌಕಟ್ಟು.ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ - GHST_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಗಾತ್ರ - GHST_ಚೌಕಟ್ಟು_TYPE_ಗಾತ್ರ; ++i) {
		ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = crc8_dvb_s2(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ, ಚೌಕಟ್ಟು.payload[i]);
	}

	ಹಿಂತಿರುಗಿ ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ;
}

ಸ್ಥಿರ uint16_t convert_channel_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ chan_ಮೌಲ್ಯ)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ scale = (2012.0F - 988.0F) / (1811.0F - 172.0F);
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ ಸ್ಥಾನಾಂತರ = 988.0F - (172.0F * scale);
	ದಶಮಾಂಶ scaled_chan_ಮೌಲ್ಯ = scale * static_cast<ದಶಮಾಂಶ>(chan_ಮೌಲ್ಯ);
	ದಶಮಾಂಶ scaled_chan_ಮೌಲ್ಯ_with_ಸ್ಥಾನಾಂತರ = scaled_chan_ಮೌಲ್ಯ + ಸ್ಥಾನಾಂತರ;
	uint16_t converted_chan_ಮೌಲ್ಯ = static_cast<uint16_t>(scaled_chan_ಮೌಲ್ಯ_with_ಸ್ಥಾನಾಂತರ);

	ಹಿಂತಿರುಗಿ converted_chan_ಮೌಲ್ಯ;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ ghst_parse_ಬಫರ್(uint16_t *ಮೌಲ್ಯಗಳು_2, ghstLinkStatistics_t *link_stats, uint16_t *ಅಂಕೆ_ಮೌಲ್ಯಗಳು,
			      uint16_t max_channels)
{
	uint8_t *ghst_ಚೌಕಟ್ಟು_ptr = (uint8_t *)&ghst_ಚೌಕಟ್ಟು;

	ಆದರೆ (parser_state == ghst_parser_state_t::unsynced) {
		// there is no sync yet, try to find an RC packet by searching for a matching frame length and type
		ಗಾಗಿ (uint32_t i = 1U; i < current_ಚೌಕಟ್ಟು_position - 1U; ++i) {
			ಆದರೆ ((ghst_ಚೌಕಟ್ಟು_ptr[i + 1U] >= static_cast<uint8_t>(ghst_ಚೌಕಟ್ಟು_Type::ಚೌಕಟ್ಟು_Type_ಮೊದಲ)) &&
			    (ghst_ಚೌಕಟ್ಟು_ptr[i + 1U] <= static_cast<uint8_t>(ghst_ಚೌಕಟ್ಟು_Type::ಚೌಕಟ್ಟು_Type_ಕೊನೆಯ))) {
				ಆದರೆ (ghst_ಚೌಕಟ್ಟು_ptr[i] == GHST_TYPE_ದತ್ತಾಂಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಗಾತ್ರ) {
					parser_state = ghst_parser_state_t::synced;
					uint32_t ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ = i - 1U;
					GHST_VERBOSE("RC channels found at offset %u", ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ);

					// move the rest of the buffer to the beginning
					ಆದರೆ (ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ != 0U) {
						memmove(ghst_ಚೌಕಟ್ಟು_ptr, ghst_ಚೌಕಟ್ಟು_ptr + ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ, current_ಚೌಕಟ್ಟು_position - ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ);
						current_ಚೌಕಟ್ಟು_position -= ಚೌಕಟ್ಟು_ಸ್ಥಾನಾಂತರ;
					}

					ನಿಲ್ಲಿಸು;
				}
			}
		}
	}

	ಆದರೆ (parser_state != ghst_parser_state_t::synced) {
		ಆದರೆ (current_ಚೌಕಟ್ಟು_position >= ಗಾತ್ರ(ghst_ಚೌಕಟ್ಟು_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_ಚೌಕಟ್ಟು_position = 3U;

			ಸ್ಮರಣೆ_ನಕಲು(ghst_ಚೌಕಟ್ಟು_ptr, ghst_ಚೌಕಟ್ಟು_ptr + ಗಾತ್ರ(ghst_ಚೌಕಟ್ಟು_t) - current_ಚೌಕಟ್ಟು_position, current_ಚೌಕಟ್ಟು_position);

			GHST_VERBOSE("Discarding buffer");
		}

		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}


	ಆದರೆ (current_ಚೌಕಟ್ಟು_position < 3U) {
		// wait until we have the address, length and type
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	// now we have at least the header and the type

	ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ = ghst_ಚೌಕಟ್ಟು.ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ + ಗಾತ್ರ(ghst_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ_t);

	ಆದರೆ ((current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ > ಗಾತ್ರ(ghst_ಚೌಕಟ್ಟು_t)) || (current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ < 4U)) {
		// frame too long or bogus (frame length should be longer than 4, at least 1 address, 1 length, 1 type, 1 data, 1 crc)
		// discard everything and go into unsynced state
		current_ಚೌಕಟ್ಟು_position = 0U;
		parser_state = ghst_parser_state_t::unsynced;
		GHST_DEBUG("Frame too long/bogus (%u, type=%u) -> unsync", current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ, ghst_ಚೌಕಟ್ಟು.type);
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಆದರೆ (current_ಚೌಕಟ್ಟು_position < current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ) {
		// we do not have the full frame yet -> wait for more data
		GHST_VERBOSE("waiting for more data (%u < %u)", current_ಚೌಕಟ್ಟು_position, current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ);
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ತಾರ್ಕಿಕ ret = ಸುಳ್ಳು;

	// now we have the full frame

	ಆದರೆ ((ghst_ಚೌಕಟ್ಟು.type >= static_cast<uint8_t>(ghst_ಚೌಕಟ್ಟು_Type::ಚೌಕಟ್ಟು_Type_ಮೊದಲ)) &&
	    (ghst_ಚೌಕಟ್ಟು.type <= static_cast<uint8_t>(ghst_ಚೌಕಟ್ಟು_Type::ಚೌಕಟ್ಟು_Type_ಕೊನೆಯ)) &&
	    (ghst_ಚೌಕಟ್ಟು.ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ == GHST_TYPE_ದತ್ತಾಂಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಗಾತ್ರ) &&
	    (ghst_ಚೌಕಟ್ಟು.ಶೀರ್ಷಿಕೆ.device_ವಿಳಾಸ == GHST_ADDR_FC)) {
		ಸ್ಥಿರಾಂಕ uint8_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = ghst_ಚೌಕಟ್ಟು.payload[ghst_ಚೌಕಟ್ಟು.ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ - 2U];

		ಆದರೆ (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ == ghst_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ghst_ಚೌಕಟ್ಟು)) {
			ಸ್ಥಿರಾಂಕ ghst_Payload_ದತ್ತಾಂಶ_t *ಸ್ಥಿರಾಂಕ rcChannels = (ghst_Payload_ದತ್ತಾಂಶ_t *)&ghst_ಚೌಕಟ್ಟು.payload;
			*ಅಂಕೆ_ಮೌಲ್ಯಗಳು = MIN(max_channels, GHST_MAX_ಅಂಕೆ_CHANNELS);

			// all frames contain data from chan1to4
			ಆದರೆ (max_channels > 0U) { ಮೌಲ್ಯಗಳು_2[0] = convert_channel_ಮೌಲ್ಯ(rcChannels->chan1to4.chan1 >> 1U); }

			ಆದರೆ (max_channels > 1U) { ಮೌಲ್ಯಗಳು_2[1] = convert_channel_ಮೌಲ್ಯ(rcChannels->chan1to4.chan2 >> 1U); }

			ಆದರೆ (max_channels > 2U) { ಮೌಲ್ಯಗಳು_2[2] = convert_channel_ಮೌಲ್ಯ(rcChannels->chan1to4.chan3 >> 1U); }

			ಆದರೆ (max_channels > 3U) { ಮೌಲ್ಯಗಳು_2[3] = convert_channel_ಮೌಲ್ಯ(rcChannels->chan1to4.chan4 >> 1U); }

			ಆದರೆ (ghst_ಚೌಕಟ್ಟು.type == static_cast<uint8_t>(ghst_ಚೌಕಟ್ಟು_Type::ಚೌಕಟ್ಟು_Type5to8)) {
				ಆದರೆ (max_channels > 4U) { ಮೌಲ್ಯಗಳು_2[4] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanA << 3U); }

				ಆದರೆ (max_channels > 5U) { ಮೌಲ್ಯಗಳು_2[5] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanB << 3U); }

				ಆದರೆ (max_channels > 6U) { ಮೌಲ್ಯಗಳು_2[6] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanC << 3U); }

				ಆದರೆ (max_channels > 7U) { ಮೌಲ್ಯಗಳು_2[7] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanD << 3U); }

			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ghst_ಚೌಕಟ್ಟು.type == static_cast<uint8_t>(ghst_ಚೌಕಟ್ಟು_Type::ಚೌಕಟ್ಟು_Type9to12)) {
				ಆದರೆ (max_channels > 8U) { ಮೌಲ್ಯಗಳು_2[8] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanA << 3U); }

				ಆದರೆ (max_channels > 9U) { ಮೌಲ್ಯಗಳು_2[9] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanB << 3U); }

				ಆದರೆ (max_channels > 10U) { ಮೌಲ್ಯಗಳು_2[10] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanC << 3U); }

				ಆದರೆ (max_channels > 11U) { ಮೌಲ್ಯಗಳು_2[11] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanD << 3U); }

			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ghst_ಚೌಕಟ್ಟು.type == static_cast<uint8_t>(ghst_ಚೌಕಟ್ಟು_Type::ಚೌಕಟ್ಟು_Type13to16)) {
				ಆದರೆ (max_channels > 12U) { ಮೌಲ್ಯಗಳು_2[12] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanA << 3U); }

				ಆದರೆ (max_channels > 13U) { ಮೌಲ್ಯಗಳು_2[13] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanB << 3U); }

				ಆದರೆ (max_channels > 14U) { ಮೌಲ್ಯಗಳು_2[14] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanC << 3U); }

				ಆದರೆ (max_channels > 15U) { ಮೌಲ್ಯಗಳು_2[15] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanD << 3U); }

			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ghst_ಚೌಕಟ್ಟು.type == static_cast<uint8_t>(ghst_ಚೌಕಟ್ಟು_Type::ಚೌಕಟ್ಟು_Type_Rssi)) {
				ಸ್ಥಿರಾಂಕ ghstPayloadRssi_t *ಸ್ಥಿರಾಂಕ rssi_ಮೌಲ್ಯಗಳು = (ghstPayloadRssi_t *)&ghst_ಚೌಕಟ್ಟು.payload;
				// TODO: call function for RSSI dBm to percentage conversion for ghost receiver
				ಕೊನೆಯ_link_stats.rssi_pct = spek_dbm_ಗಮ್ಯ_percent(static_cast<int8_t>
							   (rssi_ಮೌಲ್ಯಗಳು->rssidBm));	// rssidBm sign inverted (90 = -90dBm)
				ಕೊನೆಯ_link_stats.rssi_dbm = -rssi_ಮೌಲ್ಯಗಳು->rssidBm;
				ಕೊನೆಯ_link_stats.link_quality = rssi_ಮೌಲ್ಯಗಳು->lq;						// 0 - 100

			} ಇಲ್ಲದಿದ್ದರೆ {
				GHST_DEBUG("Frame type: %u", ghst_ಚೌಕಟ್ಟು.type);
			}

			*link_stats = ಕೊನೆಯ_link_stats;

			ಸ್ಮರಣೆ_ನಕಲು(prev_rc_vals, ಮೌಲ್ಯಗಳು_2, ಗಾತ್ರ(uint16_t) * GHST_MAX_ಅಂಕೆ_CHANNELS);

			GHST_VERBOSE("Got Channels");

			ret = ನಿಜ;

		} ಇಲ್ಲದಿದ್ದರೆ {
			GHST_DEBUG("CRC check failed");
		}

	} ಇಲ್ಲದಿದ್ದರೆ {
		GHST_DEBUG("Got Non-RC frame (len=%u, type=%u)", current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ, ghst_ಚೌಕಟ್ಟು.type);
	}

	// either reset or move the rest of the buffer
	ಆದರೆ (current_ಚೌಕಟ್ಟು_position > current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ) {
		GHST_VERBOSE("Moving buffer (%u > %u)", current_ಚೌಕಟ್ಟು_position, current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ);
		memmove(ghst_ಚೌಕಟ್ಟು_ptr, ghst_ಚೌಕಟ್ಟು_ptr + current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ, current_ಚೌಕಟ್ಟು_position - current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ);
		current_ಚೌಕಟ್ಟು_position -= current_ಚೌಕಟ್ಟು_ಉದ್ದಳತೆ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		current_ಚೌಕಟ್ಟು_position = 0U;
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
	buf[ಸ್ಥಾನಾಂತರ] = value & 0xFFU;
	buf[ಸ್ಥಾನಾಂತರ + 1] = value >> 8U;
	ಸ್ಥಾನಾಂತರ += 2;
}

/**
 * write an uint32_t value to a buffer at a given offset and increment the offset
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_uint32_t(uint8_t *buf, ಪೂರ್ಣಾಂಕ &ಸ್ಥಾನಾಂತರ, uint32_t value)
{
	// Little Endian
	buf[ಸ್ಥಾನಾಂತರ] = value & 0xFFU;
	buf[ಸ್ಥಾನಾಂತರ + 1] = (value & 0xFF00) >> 8U;
	buf[ಸ್ಥಾನಾಂತರ + 2] = (value & 0xFF0000) >> 16U;
	buf[ಸ್ಥಾನಾಂತರ + 3] = (value & 0xFF000000) >> 24U;
	ಸ್ಥಾನಾಂತರ += 4;
}

/**
 * write frame header
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ(uint8_t *buf, ಪೂರ್ಣಾಂಕ &ಸ್ಥಾನಾಂತರ, ghstTelemetryType type, uint8_t payload_ಗಾತ್ರ)
{
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, static_cast<uint8_t>(ghst_ವಿಳಾಸ::rx_ವಿಳಾಸ));
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, payload_ಗಾತ್ರ + GHST_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಗಾತ್ರ + GHST_ಚೌಕಟ್ಟು_TYPE_ಗಾತ್ರ);
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, static_cast<uint8_t>(type));
}

/**
 * write frame CRC
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(uint8_t *buf, ಪೂರ್ಣಾಂಕ &ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ buf_ಗಾತ್ರ)
{
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, crc8_dvb_s2_buf(buf + 2U, buf_ಗಾತ್ರ - 3));
}

ತಾರ್ಕಿಕ ghst_send_telemetry_battery_ಸ್ಥಿತಿ(ಪೂರ್ಣಾಂಕ uart_fd, uint16_t voltage_ಒಳ_10m_V,
					uint16_t current_ಒಳ_10m_A, uint16_t fuel_ಒಳ_10m_Ah)
{
	uint8_t buf[GHST_ಚೌಕಟ್ಟು_PAYLOAD_ಗಾತ್ರ_TELEMETRY + 4U]; // address, frame length, type, crc
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;
	write_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ(buf, ಸ್ಥಾನಾಂತರ, ghstTelemetryType::batteryPack, GHST_ಚೌಕಟ್ಟು_PAYLOAD_ಗಾತ್ರ_TELEMETRY);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, voltage_ಒಳ_10m_V);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, current_ಒಳ_10m_A);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, fuel_ಒಳ_10m_Ah);
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, 0x00U); //ಖಾಲಿಗಳು
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, 0x00U); //ಖಾಲಿಗಳು
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, 0x00U); //ಖಾಲಿಗಳು
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, 0x00U); //ಖಾಲಿಗಳು
	write_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(buf, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ(buf));

	ಹಿಂತಿರುಗಿ ಲಿಖ(uart_fd, buf, ಸ್ಥಾನಾಂತರ) == ಸ್ಥಾನಾಂತರ;
}

ತಾರ್ಕಿಕ ghst_send_telemetry_gps1_ಸ್ಥಿತಿ(ಪೂರ್ಣಾಂಕ uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude)
{
	uint8_t buf[GHST_ಚೌಕಟ್ಟು_PAYLOAD_ಗಾತ್ರ_TELEMETRY + 4U]; // address, frame length, type, crc
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;
	write_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ(buf, ಸ್ಥಾನಾಂತರ, ghstTelemetryType::gpsPrimary, GHST_ಚೌಕಟ್ಟು_PAYLOAD_ಗಾತ್ರ_TELEMETRY);
	write_uint32_t(buf, ಸ್ಥಾನಾಂತರ, latitude);
	write_uint32_t(buf, ಸ್ಥಾನಾಂತರ, longitude);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, altitude);
	write_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(buf, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ(buf));

	ಹಿಂತಿರುಗಿ ಲಿಖ(uart_fd, buf, ಸ್ಥಾನಾಂತರ) == ಸ್ಥಾನಾಂತರ;
}

ತಾರ್ಕಿಕ ghst_send_telemetry_gps2_ಸ್ಥಿತಿ(ಪೂರ್ಣಾಂಕ uart_fd, uint16_t ground_ವೇಗ, uint16_t ground_ಕೋರ್ಸು, uint8_t ಅಂಕೆ_Sats,
				     uint16_t home_dist, uint16_t home_dir, uint8_t flags)
{
	uint8_t buf[GHST_ಚೌಕಟ್ಟು_PAYLOAD_ಗಾತ್ರ_TELEMETRY + 4U]; // address, frame length, type, crc
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;
	write_ಚೌಕಟ್ಟು_ಶೀರ್ಷಿಕೆ(buf, ಸ್ಥಾನಾಂತರ, ghstTelemetryType::gpsSecondary, GHST_ಚೌಕಟ್ಟು_PAYLOAD_ಗಾತ್ರ_TELEMETRY);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, ground_ವೇಗ);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, ground_ಕೋರ್ಸು);
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, ಅಂಕೆ_Sats);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, home_dist);
	write_uint16_t(buf, ಸ್ಥಾನಾಂತರ, home_dir);
	write_uint8_t(buf, ಸ್ಥಾನಾಂತರ, flags);
	write_ಚೌಕಟ್ಟು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(buf, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ(buf));

	ಹಿಂತಿರುಗಿ ಲಿಖ(uart_fd, buf, ಸ್ಥಾನಾಂತರ) == ಸ್ಥಾನಾಂತರ;
}
