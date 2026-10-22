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

#ਜੇ 0 // enable non-verbose debugging
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GHST_DEBUG PX4_WARN
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GHST_DEBUG(...)
#ਜੇ_ਅੰਤ

#ਜੇ 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GHST_VERBOSE PX4_WARN
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GHST_VERBOSE(...)
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <termios.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>

// TODO: include RSSI dBm to percentage conversion for ghost receiver
#ਅਨ੍ਤਰ੍ਭਾਵ "spektrum_rssi.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "ghst.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "common_rc.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIN(a,b) (((a)<(b))?(a):(b))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GHST_ਚੌਖਟਾ_PAYLOAD_ਮਾਪ_TELEMETRY	(10U)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GHST_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ_ਮਾਪ			(1U)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GHST_ਚੌਖਟਾ_TYPE_ਮਾਪ			(1U)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GHST_TYPE_ਅੰਕੜੇ_ਚੱਕਰੀ_ਜਾਂਚ_ਮਾਪ			(12U)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GHST_ADDR_FC				(130U)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GHST_MAX_ਅੰਕ_CHANNELS			(16)

ਗਿਣਤੀ ਵਰਗ ghst_parser_state_t : uint8_t {
	unsynced = 0U,
	synced
};

// only RSSI frame contains value of RSSI, if it is not received, send last received RSSI/LQ
ਸਥਿਰ ghstLinkStatistics_t ਆਖਰੀ_link_stats = { .rssi_pct = -1, .rssi_dbm = NAN, .link_quality = 0 };

ਸਥਿਰ ghst_ਚੌਖਟਾ_t &ghst_ਚੌਖਟਾ = rc_decode_buf.ghst_ਚੌਖਟਾ;
ਸਥਿਰ uint32_t current_ਚੌਖਟਾ_position = 0U;
ਸਥਿਰ ghst_parser_state_t parser_state = ghst_parser_state_t::unsynced;

ਸਥਿਰ uint16_t prev_rc_vals[GHST_MAX_ਅੰਕ_CHANNELS];

/**
 * parse the current ghst_frame buffer
 */
ਸਥਿਰ ਤਾਰਕਿਕ ghst_parse_ਬਫਰ(uint16_t *ਮੁੱਲਾਂ_2, ghstLinkStatistics_t *link_stats, uint16_t *ਅੰਕ_ਮੁੱਲਾਂ,
			      uint16_t max_channels);

ਪੂਰਨ ghst_config(ਪੂਰਨ uart_fd)
{
	ਬਣਤਰ termios t;
	ਪੂਰਨ ret_val;

	// no parity, one stop bit
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, GHST_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	memset(prev_rc_vals, static_cast<ਪੂਰਨ>(UINT16_MAX), ਆਕਾਰ(uint16_t) * GHST_MAX_ਅੰਕ_CHANNELS);
	ret_val = tcsetattr(uart_fd, TCSANOW, &t);
	ਵਾਪਸ ret_val;
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
ਸਥਿਰ uint16_t convert_channel_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ chan_ਮੁੱਲ);


ਤਾਰਕਿਕ ghst_parse(ਅਚਲ uint64_t now, ਅਚਲ uint8_t *ਚੌਖਟਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ len, uint16_t *ਮੁੱਲਾਂ_2,
		ghstLinkStatistics_t *link_stats, uint16_t *ਅੰਕ_ਮੁੱਲਾਂ, uint16_t max_channels)
{
	ਤਾਰਕਿਕ success = ਝੂਠ;
	uint8_t *ghst_ਚੌਖਟਾ_ptr = (uint8_t *)&ghst_ਚੌਖਟਾ;

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਮੁੱਲਾਂ_2, prev_rc_vals, ਆਕਾਰ(uint16_t) * GHST_MAX_ਅੰਕ_CHANNELS);

	ਜਦੋਂ (len > 0U) {

		// fill in the ghst_buffer, as much as we can
		ਅਚਲ uint32_t current_len = MIN(len, ਆਕਾਰ(ghst_ਚੌਖਟਾ_t) - current_ਚੌਖਟਾ_position);
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(ghst_ਚੌਖਟਾ_ptr + current_ਚੌਖਟਾ_position, ਚੌਖਟਾ, current_len);
		current_ਚੌਖਟਾ_position += current_len;

		// protection to guarantee parsing progress
		ਜੇ (current_len == 0U) {
			GHST_DEBUG("========== parser bug: no progress (%u) ===========", len);

			ਲਈ (uint32_t i = 0U; i < current_ਚੌਖਟਾ_position; ++i) {
				GHST_DEBUG("ghst_frame_ptr[%u]: 0x%x", i, ghst_ਚੌਖਟਾ_ptr[i]);
			}

			// reset the parser
			current_ਚੌਖਟਾ_position = 0U;
			parser_state = ghst_parser_state_t::unsynced;
			success = ਝੂਠ;

		} ਨਹੀਂਤਾਂ {
			len -= current_len;
			ਚੌਖਟਾ += current_len;

			ਜੇ (ghst_parse_ਬਫਰ(ਮੁੱਲਾਂ_2, link_stats, ਅੰਕ_ਮੁੱਲਾਂ, max_channels)) {
				success = ਸੱਚ;
			}
		}
	}

	ਵਾਪਸ success;
}

uint8_t ghst_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(ਅਚਲ ghst_ਚੌਖਟਾ_t &ਚੌਖਟਾ)
{
	uint8_t ਚੱਕਰੀ_ਜਾਂਚ = crc8_dvb_s2(0U, ਚੌਖਟਾ.type);

	ਲਈ (uint32_t i = 0U; i < ਚੌਖਟਾ.ਸਿਰਲੇਖ.ਲੰਮਾਈ - GHST_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ_ਮਾਪ - GHST_ਚੌਖਟਾ_TYPE_ਮਾਪ; ++i) {
		ਚੱਕਰੀ_ਜਾਂਚ = crc8_dvb_s2(ਚੱਕਰੀ_ਜਾਂਚ, ਚੌਖਟਾ.payload[i]);
	}

	ਵਾਪਸ ਚੱਕਰੀ_ਜਾਂਚ;
}

ਸਥਿਰ uint16_t convert_channel_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ chan_ਮੁੱਲ)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ scale = (2012.0F - 988.0F) / (1811.0F - 172.0F);
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ ਵਿੱਥ = 988.0F - (172.0F * scale);
	ਦਸ਼ਮਲਵ scaled_chan_ਮੁੱਲ = scale * static_cast<ਦਸ਼ਮਲਵ>(chan_ਮੁੱਲ);
	ਦਸ਼ਮਲਵ scaled_chan_ਮੁੱਲ_with_ਵਿੱਥ = scaled_chan_ਮੁੱਲ + ਵਿੱਥ;
	uint16_t converted_chan_ਮੁੱਲ = static_cast<uint16_t>(scaled_chan_ਮੁੱਲ_with_ਵਿੱਥ);

	ਵਾਪਸ converted_chan_ਮੁੱਲ;
}

ਸਥਿਰ ਤਾਰਕਿਕ ghst_parse_ਬਫਰ(uint16_t *ਮੁੱਲਾਂ_2, ghstLinkStatistics_t *link_stats, uint16_t *ਅੰਕ_ਮੁੱਲਾਂ,
			      uint16_t max_channels)
{
	uint8_t *ghst_ਚੌਖਟਾ_ptr = (uint8_t *)&ghst_ਚੌਖਟਾ;

	ਜੇ (parser_state == ghst_parser_state_t::unsynced) {
		// there is no sync yet, try to find an RC packet by searching for a matching frame length and type
		ਲਈ (uint32_t i = 1U; i < current_ਚੌਖਟਾ_position - 1U; ++i) {
			ਜੇ ((ghst_ਚੌਖਟਾ_ptr[i + 1U] >= static_cast<uint8_t>(ghst_ਚੌਖਟਾ_Type::ਚੌਖਟਾ_Type_ਪਹਿਲਾ)) &&
			    (ghst_ਚੌਖਟਾ_ptr[i + 1U] <= static_cast<uint8_t>(ghst_ਚੌਖਟਾ_Type::ਚੌਖਟਾ_Type_ਆਖਰੀ))) {
				ਜੇ (ghst_ਚੌਖਟਾ_ptr[i] == GHST_TYPE_ਅੰਕੜੇ_ਚੱਕਰੀ_ਜਾਂਚ_ਮਾਪ) {
					parser_state = ghst_parser_state_t::synced;
					uint32_t ਚੌਖਟਾ_ਵਿੱਥ = i - 1U;
					GHST_VERBOSE("RC channels found at offset %u", ਚੌਖਟਾ_ਵਿੱਥ);

					// move the rest of the buffer to the beginning
					ਜੇ (ਚੌਖਟਾ_ਵਿੱਥ != 0U) {
						memmove(ghst_ਚੌਖਟਾ_ptr, ghst_ਚੌਖਟਾ_ptr + ਚੌਖਟਾ_ਵਿੱਥ, current_ਚੌਖਟਾ_position - ਚੌਖਟਾ_ਵਿੱਥ);
						current_ਚੌਖਟਾ_position -= ਚੌਖਟਾ_ਵਿੱਥ;
					}

					ਰੋਕੋ;
				}
			}
		}
	}

	ਜੇ (parser_state != ghst_parser_state_t::synced) {
		ਜੇ (current_ਚੌਖਟਾ_position >= ਆਕਾਰ(ghst_ਚੌਖਟਾ_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_ਚੌਖਟਾ_position = 3U;

			ਸਮ੍ਰਿਤੀ_ਨਕਲ(ghst_ਚੌਖਟਾ_ptr, ghst_ਚੌਖਟਾ_ptr + ਆਕਾਰ(ghst_ਚੌਖਟਾ_t) - current_ਚੌਖਟਾ_position, current_ਚੌਖਟਾ_position);

			GHST_VERBOSE("Discarding buffer");
		}

		ਵਾਪਸ ਝੂਠ;
	}


	ਜੇ (current_ਚੌਖਟਾ_position < 3U) {
		// wait until we have the address, length and type
		ਵਾਪਸ ਝੂਠ;
	}

	// now we have at least the header and the type

	ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ current_ਚੌਖਟਾ_ਲੰਮਾਈ = ghst_ਚੌਖਟਾ.ਸਿਰਲੇਖ.ਲੰਮਾਈ + ਆਕਾਰ(ghst_ਚੌਖਟਾ_ਸਿਰਲੇਖ_t);

	ਜੇ ((current_ਚੌਖਟਾ_ਲੰਮਾਈ > ਆਕਾਰ(ghst_ਚੌਖਟਾ_t)) || (current_ਚੌਖਟਾ_ਲੰਮਾਈ < 4U)) {
		// frame too long or bogus (frame length should be longer than 4, at least 1 address, 1 length, 1 type, 1 data, 1 crc)
		// discard everything and go into unsynced state
		current_ਚੌਖਟਾ_position = 0U;
		parser_state = ghst_parser_state_t::unsynced;
		GHST_DEBUG("Frame too long/bogus (%u, type=%u) -> unsync", current_ਚੌਖਟਾ_ਲੰਮਾਈ, ghst_ਚੌਖਟਾ.type);
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (current_ਚੌਖਟਾ_position < current_ਚੌਖਟਾ_ਲੰਮਾਈ) {
		// we do not have the full frame yet -> wait for more data
		GHST_VERBOSE("waiting for more data (%u < %u)", current_ਚੌਖਟਾ_position, current_ਚੌਖਟਾ_ਲੰਮਾਈ);
		ਵਾਪਸ ਝੂਠ;
	}

	ਤਾਰਕਿਕ ret = ਝੂਠ;

	// now we have the full frame

	ਜੇ ((ghst_ਚੌਖਟਾ.type >= static_cast<uint8_t>(ghst_ਚੌਖਟਾ_Type::ਚੌਖਟਾ_Type_ਪਹਿਲਾ)) &&
	    (ghst_ਚੌਖਟਾ.type <= static_cast<uint8_t>(ghst_ਚੌਖਟਾ_Type::ਚੌਖਟਾ_Type_ਆਖਰੀ)) &&
	    (ghst_ਚੌਖਟਾ.ਸਿਰਲੇਖ.ਲੰਮਾਈ == GHST_TYPE_ਅੰਕੜੇ_ਚੱਕਰੀ_ਜਾਂਚ_ਮਾਪ) &&
	    (ghst_ਚੌਖਟਾ.ਸਿਰਲੇਖ.device_ਪਤਾ == GHST_ADDR_FC)) {
		ਅਚਲ uint8_t ਚੱਕਰੀ_ਜਾਂਚ = ghst_ਚੌਖਟਾ.payload[ghst_ਚੌਖਟਾ.ਸਿਰਲੇਖ.ਲੰਮਾਈ - 2U];

		ਜੇ (ਚੱਕਰੀ_ਜਾਂਚ == ghst_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(ghst_ਚੌਖਟਾ)) {
			ਅਚਲ ghst_Payload_ਅੰਕੜੇ_t *ਅਚਲ rcChannels = (ghst_Payload_ਅੰਕੜੇ_t *)&ghst_ਚੌਖਟਾ.payload;
			*ਅੰਕ_ਮੁੱਲਾਂ = MIN(max_channels, GHST_MAX_ਅੰਕ_CHANNELS);

			// all frames contain data from chan1to4
			ਜੇ (max_channels > 0U) { ਮੁੱਲਾਂ_2[0] = convert_channel_ਮੁੱਲ(rcChannels->chan1to4.chan1 >> 1U); }

			ਜੇ (max_channels > 1U) { ਮੁੱਲਾਂ_2[1] = convert_channel_ਮੁੱਲ(rcChannels->chan1to4.chan2 >> 1U); }

			ਜੇ (max_channels > 2U) { ਮੁੱਲਾਂ_2[2] = convert_channel_ਮੁੱਲ(rcChannels->chan1to4.chan3 >> 1U); }

			ਜੇ (max_channels > 3U) { ਮੁੱਲਾਂ_2[3] = convert_channel_ਮੁੱਲ(rcChannels->chan1to4.chan4 >> 1U); }

			ਜੇ (ghst_ਚੌਖਟਾ.type == static_cast<uint8_t>(ghst_ਚੌਖਟਾ_Type::ਚੌਖਟਾ_Type5to8)) {
				ਜੇ (max_channels > 4U) { ਮੁੱਲਾਂ_2[4] = convert_channel_ਮੁੱਲ(rcChannels->chanA << 3U); }

				ਜੇ (max_channels > 5U) { ਮੁੱਲਾਂ_2[5] = convert_channel_ਮੁੱਲ(rcChannels->chanB << 3U); }

				ਜੇ (max_channels > 6U) { ਮੁੱਲਾਂ_2[6] = convert_channel_ਮੁੱਲ(rcChannels->chanC << 3U); }

				ਜੇ (max_channels > 7U) { ਮੁੱਲਾਂ_2[7] = convert_channel_ਮੁੱਲ(rcChannels->chanD << 3U); }

			} ਨਹੀਂਤਾਂ ਜੇ (ghst_ਚੌਖਟਾ.type == static_cast<uint8_t>(ghst_ਚੌਖਟਾ_Type::ਚੌਖਟਾ_Type9to12)) {
				ਜੇ (max_channels > 8U) { ਮੁੱਲਾਂ_2[8] = convert_channel_ਮੁੱਲ(rcChannels->chanA << 3U); }

				ਜੇ (max_channels > 9U) { ਮੁੱਲਾਂ_2[9] = convert_channel_ਮੁੱਲ(rcChannels->chanB << 3U); }

				ਜੇ (max_channels > 10U) { ਮੁੱਲਾਂ_2[10] = convert_channel_ਮੁੱਲ(rcChannels->chanC << 3U); }

				ਜੇ (max_channels > 11U) { ਮੁੱਲਾਂ_2[11] = convert_channel_ਮੁੱਲ(rcChannels->chanD << 3U); }

			} ਨਹੀਂਤਾਂ ਜੇ (ghst_ਚੌਖਟਾ.type == static_cast<uint8_t>(ghst_ਚੌਖਟਾ_Type::ਚੌਖਟਾ_Type13to16)) {
				ਜੇ (max_channels > 12U) { ਮੁੱਲਾਂ_2[12] = convert_channel_ਮੁੱਲ(rcChannels->chanA << 3U); }

				ਜੇ (max_channels > 13U) { ਮੁੱਲਾਂ_2[13] = convert_channel_ਮੁੱਲ(rcChannels->chanB << 3U); }

				ਜੇ (max_channels > 14U) { ਮੁੱਲਾਂ_2[14] = convert_channel_ਮੁੱਲ(rcChannels->chanC << 3U); }

				ਜੇ (max_channels > 15U) { ਮੁੱਲਾਂ_2[15] = convert_channel_ਮੁੱਲ(rcChannels->chanD << 3U); }

			} ਨਹੀਂਤਾਂ ਜੇ (ghst_ਚੌਖਟਾ.type == static_cast<uint8_t>(ghst_ਚੌਖਟਾ_Type::ਚੌਖਟਾ_Type_Rssi)) {
				ਅਚਲ ghstPayloadRssi_t *ਅਚਲ rssi_ਮੁੱਲਾਂ = (ghstPayloadRssi_t *)&ghst_ਚੌਖਟਾ.payload;
				// TODO: call function for RSSI dBm to percentage conversion for ghost receiver
				ਆਖਰੀ_link_stats.rssi_pct = spek_dbm_ਮੰਜ਼ਿਲ_percent(static_cast<int8_t>
							   (rssi_ਮੁੱਲਾਂ->rssidBm));	// rssidBm sign inverted (90 = -90dBm)
				ਆਖਰੀ_link_stats.rssi_dbm = -rssi_ਮੁੱਲਾਂ->rssidBm;
				ਆਖਰੀ_link_stats.link_quality = rssi_ਮੁੱਲਾਂ->lq;						// 0 - 100

			} ਨਹੀਂਤਾਂ {
				GHST_DEBUG("Frame type: %u", ghst_ਚੌਖਟਾ.type);
			}

			*link_stats = ਆਖਰੀ_link_stats;

			ਸਮ੍ਰਿਤੀ_ਨਕਲ(prev_rc_vals, ਮੁੱਲਾਂ_2, ਆਕਾਰ(uint16_t) * GHST_MAX_ਅੰਕ_CHANNELS);

			GHST_VERBOSE("Got Channels");

			ret = ਸੱਚ;

		} ਨਹੀਂਤਾਂ {
			GHST_DEBUG("CRC check failed");
		}

	} ਨਹੀਂਤਾਂ {
		GHST_DEBUG("Got Non-RC frame (len=%u, type=%u)", current_ਚੌਖਟਾ_ਲੰਮਾਈ, ghst_ਚੌਖਟਾ.type);
	}

	// either reset or move the rest of the buffer
	ਜੇ (current_ਚੌਖਟਾ_position > current_ਚੌਖਟਾ_ਲੰਮਾਈ) {
		GHST_VERBOSE("Moving buffer (%u > %u)", current_ਚੌਖਟਾ_position, current_ਚੌਖਟਾ_ਲੰਮਾਈ);
		memmove(ghst_ਚੌਖਟਾ_ptr, ghst_ਚੌਖਟਾ_ptr + current_ਚੌਖਟਾ_ਲੰਮਾਈ, current_ਚੌਖਟਾ_position - current_ਚੌਖਟਾ_ਲੰਮਾਈ);
		current_ਚੌਖਟਾ_position -= current_ਚੌਖਟਾ_ਲੰਮਾਈ;

	} ਨਹੀਂਤਾਂ {
		current_ਚੌਖਟਾ_position = 0U;
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
	buf[ਵਿੱਥ] = value & 0xFFU;
	buf[ਵਿੱਥ + 1] = value >> 8U;
	ਵਿੱਥ += 2;
}

/**
 * write an uint32_t value to a buffer at a given offset and increment the offset
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_uint32_t(uint8_t *buf, ਪੂਰਨ &ਵਿੱਥ, uint32_t value)
{
	// Little Endian
	buf[ਵਿੱਥ] = value & 0xFFU;
	buf[ਵਿੱਥ + 1] = (value & 0xFF00) >> 8U;
	buf[ਵਿੱਥ + 2] = (value & 0xFF0000) >> 16U;
	buf[ਵਿੱਥ + 3] = (value & 0xFF000000) >> 24U;
	ਵਿੱਥ += 4;
}

/**
 * write frame header
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_ਚੌਖਟਾ_ਸਿਰਲੇਖ(uint8_t *buf, ਪੂਰਨ &ਵਿੱਥ, ghstTelemetryType type, uint8_t payload_ਮਾਪ)
{
	write_uint8_t(buf, ਵਿੱਥ, static_cast<uint8_t>(ghst_ਪਤਾ::rx_ਪਤਾ));
	write_uint8_t(buf, ਵਿੱਥ, payload_ਮਾਪ + GHST_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ_ਮਾਪ + GHST_ਚੌਖਟਾ_TYPE_ਮਾਪ);
	write_uint8_t(buf, ਵਿੱਥ, static_cast<uint8_t>(type));
}

/**
 * write frame CRC
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ write_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(uint8_t *buf, ਪੂਰਨ &ਵਿੱਥ, ਪੂਰਨ buf_ਮਾਪ)
{
	write_uint8_t(buf, ਵਿੱਥ, crc8_dvb_s2_buf(buf + 2U, buf_ਮਾਪ - 3));
}

ਤਾਰਕਿਕ ghst_send_telemetry_battery_ਸਥਿਤੀ(ਪੂਰਨ uart_fd, uint16_t voltage_ਵਿੱਚ_10m_V,
					uint16_t current_ਵਿੱਚ_10m_A, uint16_t fuel_ਵਿੱਚ_10m_Ah)
{
	uint8_t buf[GHST_ਚੌਖਟਾ_PAYLOAD_ਮਾਪ_TELEMETRY + 4U]; // address, frame length, type, crc
	ਪੂਰਨ ਵਿੱਥ = 0;
	write_ਚੌਖਟਾ_ਸਿਰਲੇਖ(buf, ਵਿੱਥ, ghstTelemetryType::batteryPack, GHST_ਚੌਖਟਾ_PAYLOAD_ਮਾਪ_TELEMETRY);
	write_uint16_t(buf, ਵਿੱਥ, voltage_ਵਿੱਚ_10m_V);
	write_uint16_t(buf, ਵਿੱਥ, current_ਵਿੱਚ_10m_A);
	write_uint16_t(buf, ਵਿੱਥ, fuel_ਵਿੱਚ_10m_Ah);
	write_uint8_t(buf, ਵਿੱਥ, 0x00U); //ਖਾਲੀ_ਥਾਵਾਂ
	write_uint8_t(buf, ਵਿੱਥ, 0x00U); //ਖਾਲੀ_ਥਾਵਾਂ
	write_uint8_t(buf, ਵਿੱਥ, 0x00U); //ਖਾਲੀ_ਥਾਵਾਂ
	write_uint8_t(buf, ਵਿੱਥ, 0x00U); //ਖਾਲੀ_ਥਾਵਾਂ
	write_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(buf, ਵਿੱਥ, ਆਕਾਰ(buf));

	ਵਾਪਸ ਲਿਖ(uart_fd, buf, ਵਿੱਥ) == ਵਿੱਥ;
}

ਤਾਰਕਿਕ ghst_send_telemetry_gps1_ਸਥਿਤੀ(ਪੂਰਨ uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude)
{
	uint8_t buf[GHST_ਚੌਖਟਾ_PAYLOAD_ਮਾਪ_TELEMETRY + 4U]; // address, frame length, type, crc
	ਪੂਰਨ ਵਿੱਥ = 0;
	write_ਚੌਖਟਾ_ਸਿਰਲੇਖ(buf, ਵਿੱਥ, ghstTelemetryType::gpsPrimary, GHST_ਚੌਖਟਾ_PAYLOAD_ਮਾਪ_TELEMETRY);
	write_uint32_t(buf, ਵਿੱਥ, latitude);
	write_uint32_t(buf, ਵਿੱਥ, longitude);
	write_uint16_t(buf, ਵਿੱਥ, altitude);
	write_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(buf, ਵਿੱਥ, ਆਕਾਰ(buf));

	ਵਾਪਸ ਲਿਖ(uart_fd, buf, ਵਿੱਥ) == ਵਿੱਥ;
}

ਤਾਰਕਿਕ ghst_send_telemetry_gps2_ਸਥਿਤੀ(ਪੂਰਨ uart_fd, uint16_t ground_ਰਫਤਾਰ, uint16_t ground_ਕੋਰਸ, uint8_t ਅੰਕ_Sats,
				     uint16_t home_dist, uint16_t home_dir, uint8_t flags)
{
	uint8_t buf[GHST_ਚੌਖਟਾ_PAYLOAD_ਮਾਪ_TELEMETRY + 4U]; // address, frame length, type, crc
	ਪੂਰਨ ਵਿੱਥ = 0;
	write_ਚੌਖਟਾ_ਸਿਰਲੇਖ(buf, ਵਿੱਥ, ghstTelemetryType::gpsSecondary, GHST_ਚੌਖਟਾ_PAYLOAD_ਮਾਪ_TELEMETRY);
	write_uint16_t(buf, ਵਿੱਥ, ground_ਰਫਤਾਰ);
	write_uint16_t(buf, ਵਿੱਥ, ground_ਕੋਰਸ);
	write_uint8_t(buf, ਵਿੱਥ, ਅੰਕ_Sats);
	write_uint16_t(buf, ਵਿੱਥ, home_dist);
	write_uint16_t(buf, ਵਿੱਥ, home_dir);
	write_uint8_t(buf, ਵਿੱਥ, flags);
	write_ਚੌਖਟਾ_ਚੱਕਰੀ_ਜਾਂਚ(buf, ਵਿੱਥ, ਆਕਾਰ(buf));

	ਵਾਪਸ ਲਿਖ(uart_fd, buf, ਵਿੱਥ) == ਵਿੱਥ;
}
