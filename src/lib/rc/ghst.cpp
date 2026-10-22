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

#എങ്കിൽ 0 // enable non-verbose debugging
#define GHST_DEBUG PX4_WARN
#അല്ലെങ്കിൽ
#define GHST_DEBUG(...)
#endif

#എങ്കിൽ 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#define GHST_VERBOSE PX4_WARN
#അല്ലെങ്കിൽ
#define GHST_VERBOSE(...)
#endif

#അന്തര്ഭാവ <drivers/drv_hrt.h>
#അന്തര്ഭാവ <termios.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <unistd.h>
#അന്തര്ഭാവ <math.h>

// TODO: include RSSI dBm to percentage conversion for ghost receiver
#അന്തര്ഭാവ "spektrum_rssi.h"

#അന്തര്ഭാവ "ghst.hpp"
#അന്തര്ഭാവ "common_rc.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

#define GHST_FRAME_PAYLOAD_SIZE_TELEMETRY	(10U)
#define GHST_FRAME_CRC_SIZE			(1U)
#define GHST_FRAME_TYPE_SIZE			(1U)
#define GHST_TYPE_DATA_CRC_SIZE			(12U)
#define GHST_ADDR_FC				(130U)
#define GHST_MAX_അക്കം_CHANNELS			(16)

എണ്ണൽ വർഗ്ഗം ghst_parser_state_t : uint8_t {
	unsynced = 0U,
	synced
};

// only RSSI frame contains value of RSSI, if it is not received, send last received RSSI/LQ
സ്ഥിതം ghstLinkStatistics_t last_link_stats = { .rssi_pct = -1, .rssi_dbm = NAN, .link_quality = 0 };

സ്ഥിതം ghst_frame_t &ghst_frame = rc_decode_buf.ghst_frame;
സ്ഥിതം uint32_t current_frame_position = 0U;
സ്ഥിതം ghst_parser_state_t parser_state = ghst_parser_state_t::unsynced;

സ്ഥിതം uint16_t prev_rc_vals[GHST_MAX_അക്കം_CHANNELS];

/**
 * parse the current ghst_frame buffer
 */
സ്ഥിതം തർക്കം ghst_parse_buffer(uint16_t *മൂല്യങ്ങൾ_2, ghstLinkStatistics_t *link_stats, uint16_t *അക്കം_മൂല്യങ്ങൾ,
			      uint16_t max_channels);

പൂർണ്ണസംഖ്യ ghst_config(പൂർണ്ണസംഖ്യ uart_fd)
{
	ഘടന termios t;
	പൂർണ്ണസംഖ്യ ret_val;

	// no parity, one stop bit
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, GHST_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	memset(prev_rc_vals, static_cast<പൂർണ്ണസംഖ്യ>(UINT16_MAX), വലുപ്പം(uint16_t) * GHST_MAX_അക്കം_CHANNELS);
	ret_val = tcsetattr(uart_fd, TCSANOW, &t);
	തിരികെ ret_val;
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
സ്ഥിതം uint16_t convert_channel_മൂല്യം(unsigned chan_മൂല്യം);


തർക്കം ghst_parse(സ്ഥിരം uint64_t now, സ്ഥിരം uint8_t *frame, unsigned len, uint16_t *മൂല്യങ്ങൾ_2,
		ghstLinkStatistics_t *link_stats, uint16_t *അക്കം_മൂല്യങ്ങൾ, uint16_t max_channels)
{
	തർക്കം success = അസത്യം;
	uint8_t *ghst_frame_ptr = (uint8_t *)&ghst_frame;

	memcpy(മൂല്യങ്ങൾ_2, prev_rc_vals, വലുപ്പം(uint16_t) * GHST_MAX_അക്കം_CHANNELS);

	ആകുമ്പോൾ (len > 0U) {

		// fill in the ghst_buffer, as much as we can
		സ്ഥിരം uint32_t current_len = MIN(len, വലുപ്പം(ghst_frame_t) - current_frame_position);
		memcpy(ghst_frame_ptr + current_frame_position, frame, current_len);
		current_frame_position += current_len;

		// protection to guarantee parsing progress
		എങ്കിൽ (current_len == 0U) {
			GHST_DEBUG("========== parser bug: no progress (%u) ===========", len);

			വേണ്ടി (uint32_t i = 0U; i < current_frame_position; ++i) {
				GHST_DEBUG("ghst_frame_ptr[%u]: 0x%x", i, ghst_frame_ptr[i]);
			}

			// reset the parser
			current_frame_position = 0U;
			parser_state = ghst_parser_state_t::unsynced;
			success = അസത്യം;

		} അല്ലെങ്കിൽ {
			len -= current_len;
			frame += current_len;

			എങ്കിൽ (ghst_parse_buffer(മൂല്യങ്ങൾ_2, link_stats, അക്കം_മൂല്യങ്ങൾ, max_channels)) {
				success = സത്യം;
			}
		}
	}

	തിരികെ success;
}

uint8_t ghst_frame_CRC(സ്ഥിരം ghst_frame_t &frame)
{
	uint8_t crc = crc8_dvb_s2(0U, frame.type);

	വേണ്ടി (uint32_t i = 0U; i < frame.header.length - GHST_FRAME_CRC_SIZE - GHST_FRAME_TYPE_SIZE; ++i) {
		crc = crc8_dvb_s2(crc, frame.payload[i]);
	}

	തിരികെ crc;
}

സ്ഥിതം uint16_t convert_channel_മൂല്യം(unsigned പൂർണ്ണസംഖ്യ chan_മൂല്യം)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	സ്ഥിതം constexpr ദശാംശം scale = (2012.0F - 988.0F) / (1811.0F - 172.0F);
	സ്ഥിതം constexpr ദശാംശം offset = 988.0F - (172.0F * scale);
	ദശാംശം scaled_chan_മൂല്യം = scale * static_cast<ദശാംശം>(chan_മൂല്യം);
	ദശാംശം scaled_chan_മൂല്യം_with_offset = scaled_chan_മൂല്യം + offset;
	uint16_t converted_chan_മൂല്യം = static_cast<uint16_t>(scaled_chan_മൂല്യം_with_offset);

	തിരികെ converted_chan_മൂല്യം;
}

സ്ഥിതം തർക്കം ghst_parse_buffer(uint16_t *മൂല്യങ്ങൾ_2, ghstLinkStatistics_t *link_stats, uint16_t *അക്കം_മൂല്യങ്ങൾ,
			      uint16_t max_channels)
{
	uint8_t *ghst_frame_ptr = (uint8_t *)&ghst_frame;

	എങ്കിൽ (parser_state == ghst_parser_state_t::unsynced) {
		// there is no sync yet, try to find an RC packet by searching for a matching frame length and type
		വേണ്ടി (uint32_t i = 1U; i < current_frame_position - 1U; ++i) {
			എങ്കിൽ ((ghst_frame_ptr[i + 1U] >= static_cast<uint8_t>(ghstFrameType::frameTypeFirst)) &&
			    (ghst_frame_ptr[i + 1U] <= static_cast<uint8_t>(ghstFrameType::frameTypeLast))) {
				എങ്കിൽ (ghst_frame_ptr[i] == GHST_TYPE_DATA_CRC_SIZE) {
					parser_state = ghst_parser_state_t::synced;
					uint32_t frame_offset = i - 1U;
					GHST_VERBOSE("RC channels found at offset %u", frame_offset);

					// move the rest of the buffer to the beginning
					എങ്കിൽ (frame_offset != 0U) {
						memmove(ghst_frame_ptr, ghst_frame_ptr + frame_offset, current_frame_position - frame_offset);
						current_frame_position -= frame_offset;
					}

					നിർത്തുക;
				}
			}
		}
	}

	എങ്കിൽ (parser_state != ghst_parser_state_t::synced) {
		എങ്കിൽ (current_frame_position >= വലുപ്പം(ghst_frame_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_frame_position = 3U;

			memcpy(ghst_frame_ptr, ghst_frame_ptr + വലുപ്പം(ghst_frame_t) - current_frame_position, current_frame_position);

			GHST_VERBOSE("Discarding buffer");
		}

		തിരികെ അസത്യം;
	}


	എങ്കിൽ (current_frame_position < 3U) {
		// wait until we have the address, length and type
		തിരികെ അസത്യം;
	}

	// now we have at least the header and the type

	സ്ഥിരം unsigned current_frame_length = ghst_frame.header.length + വലുപ്പം(ghst_frame_header_t);

	എങ്കിൽ ((current_frame_length > വലുപ്പം(ghst_frame_t)) || (current_frame_length < 4U)) {
		// frame too long or bogus (frame length should be longer than 4, at least 1 address, 1 length, 1 type, 1 data, 1 crc)
		// discard everything and go into unsynced state
		current_frame_position = 0U;
		parser_state = ghst_parser_state_t::unsynced;
		GHST_DEBUG("Frame too long/bogus (%u, type=%u) -> unsync", current_frame_length, ghst_frame.type);
		തിരികെ അസത്യം;
	}

	എങ്കിൽ (current_frame_position < current_frame_length) {
		// we do not have the full frame yet -> wait for more data
		GHST_VERBOSE("waiting for more data (%u < %u)", current_frame_position, current_frame_length);
		തിരികെ അസത്യം;
	}

	തർക്കം ret = അസത്യം;

	// now we have the full frame

	എങ്കിൽ ((ghst_frame.type >= static_cast<uint8_t>(ghstFrameType::frameTypeFirst)) &&
	    (ghst_frame.type <= static_cast<uint8_t>(ghstFrameType::frameTypeLast)) &&
	    (ghst_frame.header.length == GHST_TYPE_DATA_CRC_SIZE) &&
	    (ghst_frame.header.device_address == GHST_ADDR_FC)) {
		സ്ഥിരം uint8_t crc = ghst_frame.payload[ghst_frame.header.length - 2U];

		എങ്കിൽ (crc == ghst_frame_CRC(ghst_frame)) {
			സ്ഥിരം ghstPayloadData_t *സ്ഥിരം rcChannels = (ghstPayloadData_t *)&ghst_frame.payload;
			*അക്കം_മൂല്യങ്ങൾ = MIN(max_channels, GHST_MAX_അക്കം_CHANNELS);

			// all frames contain data from chan1to4
			എങ്കിൽ (max_channels > 0U) { മൂല്യങ്ങൾ_2[0] = convert_channel_മൂല്യം(rcChannels->chan1to4.chan1 >> 1U); }

			എങ്കിൽ (max_channels > 1U) { മൂല്യങ്ങൾ_2[1] = convert_channel_മൂല്യം(rcChannels->chan1to4.chan2 >> 1U); }

			എങ്കിൽ (max_channels > 2U) { മൂല്യങ്ങൾ_2[2] = convert_channel_മൂല്യം(rcChannels->chan1to4.chan3 >> 1U); }

			എങ്കിൽ (max_channels > 3U) { മൂല്യങ്ങൾ_2[3] = convert_channel_മൂല്യം(rcChannels->chan1to4.chan4 >> 1U); }

			എങ്കിൽ (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType5to8)) {
				എങ്കിൽ (max_channels > 4U) { മൂല്യങ്ങൾ_2[4] = convert_channel_മൂല്യം(rcChannels->chanA << 3U); }

				എങ്കിൽ (max_channels > 5U) { മൂല്യങ്ങൾ_2[5] = convert_channel_മൂല്യം(rcChannels->chanB << 3U); }

				എങ്കിൽ (max_channels > 6U) { മൂല്യങ്ങൾ_2[6] = convert_channel_മൂല്യം(rcChannels->chanC << 3U); }

				എങ്കിൽ (max_channels > 7U) { മൂല്യങ്ങൾ_2[7] = convert_channel_മൂല്യം(rcChannels->chanD << 3U); }

			} അല്ലെങ്കിൽ എങ്കിൽ (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType9to12)) {
				എങ്കിൽ (max_channels > 8U) { മൂല്യങ്ങൾ_2[8] = convert_channel_മൂല്യം(rcChannels->chanA << 3U); }

				എങ്കിൽ (max_channels > 9U) { മൂല്യങ്ങൾ_2[9] = convert_channel_മൂല്യം(rcChannels->chanB << 3U); }

				എങ്കിൽ (max_channels > 10U) { മൂല്യങ്ങൾ_2[10] = convert_channel_മൂല്യം(rcChannels->chanC << 3U); }

				എങ്കിൽ (max_channels > 11U) { മൂല്യങ്ങൾ_2[11] = convert_channel_മൂല്യം(rcChannels->chanD << 3U); }

			} അല്ലെങ്കിൽ എങ്കിൽ (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType13to16)) {
				എങ്കിൽ (max_channels > 12U) { മൂല്യങ്ങൾ_2[12] = convert_channel_മൂല്യം(rcChannels->chanA << 3U); }

				എങ്കിൽ (max_channels > 13U) { മൂല്യങ്ങൾ_2[13] = convert_channel_മൂല്യം(rcChannels->chanB << 3U); }

				എങ്കിൽ (max_channels > 14U) { മൂല്യങ്ങൾ_2[14] = convert_channel_മൂല്യം(rcChannels->chanC << 3U); }

				എങ്കിൽ (max_channels > 15U) { മൂല്യങ്ങൾ_2[15] = convert_channel_മൂല്യം(rcChannels->chanD << 3U); }

			} അല്ലെങ്കിൽ എങ്കിൽ (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameTypeRssi)) {
				സ്ഥിരം ghstPayloadRssi_t *സ്ഥിരം rssi_മൂല്യങ്ങൾ = (ghstPayloadRssi_t *)&ghst_frame.payload;
				// TODO: call function for RSSI dBm to percentage conversion for ghost receiver
				last_link_stats.rssi_pct = spek_dbm_to_percent(static_cast<int8_t>
							   (rssi_മൂല്യങ്ങൾ->rssidBm));	// rssidBm sign inverted (90 = -90dBm)
				last_link_stats.rssi_dbm = -rssi_മൂല്യങ്ങൾ->rssidBm;
				last_link_stats.link_quality = rssi_മൂല്യങ്ങൾ->lq;						// 0 - 100

			} അല്ലെങ്കിൽ {
				GHST_DEBUG("Frame type: %u", ghst_frame.type);
			}

			*link_stats = last_link_stats;

			memcpy(prev_rc_vals, മൂല്യങ്ങൾ_2, വലുപ്പം(uint16_t) * GHST_MAX_അക്കം_CHANNELS);

			GHST_VERBOSE("Got Channels");

			ret = സത്യം;

		} അല്ലെങ്കിൽ {
			GHST_DEBUG("CRC check failed");
		}

	} അല്ലെങ്കിൽ {
		GHST_DEBUG("Got Non-RC frame (len=%u, type=%u)", current_frame_length, ghst_frame.type);
	}

	// either reset or move the rest of the buffer
	എങ്കിൽ (current_frame_position > current_frame_length) {
		GHST_VERBOSE("Moving buffer (%u > %u)", current_frame_position, current_frame_length);
		memmove(ghst_frame_ptr, ghst_frame_ptr + current_frame_length, current_frame_position - current_frame_length);
		current_frame_position -= current_frame_length;

	} അല്ലെങ്കിൽ {
		current_frame_position = 0U;
	}

	തിരികെ ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
സ്ഥിതം വരിയിൽ ശൂന്യം write_uint8_t(uint8_t *buf, പൂർണ്ണസംഖ്യ &offset, uint8_t value)
{
	buf[offset++] = value;
}

/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
സ്ഥിതം വരിയിൽ ശൂന്യം write_uint16_t(uint8_t *buf, പൂർണ്ണസംഖ്യ &offset, uint16_t value)
{
	buf[offset] = value & 0xFFU;
	buf[offset + 1] = value >> 8U;
	offset += 2;
}

/**
 * write an uint32_t value to a buffer at a given offset and increment the offset
 */
സ്ഥിതം വരിയിൽ ശൂന്യം write_uint32_t(uint8_t *buf, പൂർണ്ണസംഖ്യ &offset, uint32_t value)
{
	// Little Endian
	buf[offset] = value & 0xFFU;
	buf[offset + 1] = (value & 0xFF00) >> 8U;
	buf[offset + 2] = (value & 0xFF0000) >> 16U;
	buf[offset + 3] = (value & 0xFF000000) >> 24U;
	offset += 4;
}

/**
 * write frame header
 */
സ്ഥിതം വരിയിൽ ശൂന്യം write_frame_header(uint8_t *buf, പൂർണ്ണസംഖ്യ &offset, ghstTelemetryType type, uint8_t payload_size)
{
	write_uint8_t(buf, offset, static_cast<uint8_t>(ghstAddress::rxAddress));
	write_uint8_t(buf, offset, payload_size + GHST_FRAME_CRC_SIZE + GHST_FRAME_TYPE_SIZE);
	write_uint8_t(buf, offset, static_cast<uint8_t>(type));
}

/**
 * write frame CRC
 */
സ്ഥിതം വരിയിൽ ശൂന്യം write_frame_crc(uint8_t *buf, പൂർണ്ണസംഖ്യ &offset, പൂർണ്ണസംഖ്യ buf_size)
{
	write_uint8_t(buf, offset, crc8_dvb_s2_buf(buf + 2U, buf_size - 3));
}

തർക്കം ghst_send_telemetry_battery_status(പൂർണ്ണസംഖ്യ uart_fd, uint16_t voltage_in_10mV,
					uint16_t current_in_10mA, uint16_t fuel_in_10mAh)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_SIZE_TELEMETRY + 4U]; // address, frame length, type, crc
	പൂർണ്ണസംഖ്യ offset = 0;
	write_frame_header(buf, offset, ghstTelemetryType::batteryPack, GHST_FRAME_PAYLOAD_SIZE_TELEMETRY);
	write_uint16_t(buf, offset, voltage_in_10mV);
	write_uint16_t(buf, offset, current_in_10mA);
	write_uint16_t(buf, offset, fuel_in_10mAh);
	write_uint8_t(buf, offset, 0x00U); // empty
	write_uint8_t(buf, offset, 0x00U); // empty
	write_uint8_t(buf, offset, 0x00U); // empty
	write_uint8_t(buf, offset, 0x00U); // empty
	write_frame_crc(buf, offset, വലുപ്പം(buf));

	തിരികെ ലിഖ(uart_fd, buf, offset) == offset;
}

തർക്കം ghst_send_telemetry_gps1_status(പൂർണ്ണസംഖ്യ uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_SIZE_TELEMETRY + 4U]; // address, frame length, type, crc
	പൂർണ്ണസംഖ്യ offset = 0;
	write_frame_header(buf, offset, ghstTelemetryType::gpsPrimary, GHST_FRAME_PAYLOAD_SIZE_TELEMETRY);
	write_uint32_t(buf, offset, latitude);
	write_uint32_t(buf, offset, longitude);
	write_uint16_t(buf, offset, altitude);
	write_frame_crc(buf, offset, വലുപ്പം(buf));

	തിരികെ ലിഖ(uart_fd, buf, offset) == offset;
}

തർക്കം ghst_send_telemetry_gps2_status(പൂർണ്ണസംഖ്യ uart_fd, uint16_t ground_speed, uint16_t ground_course, uint8_t അക്കം_Sats,
				     uint16_t home_dist, uint16_t home_dir, uint8_t flags)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_SIZE_TELEMETRY + 4U]; // address, frame length, type, crc
	പൂർണ്ണസംഖ്യ offset = 0;
	write_frame_header(buf, offset, ghstTelemetryType::gpsSecondary, GHST_FRAME_PAYLOAD_SIZE_TELEMETRY);
	write_uint16_t(buf, offset, ground_speed);
	write_uint16_t(buf, offset, ground_course);
	write_uint8_t(buf, offset, അക്കം_Sats);
	write_uint16_t(buf, offset, home_dist);
	write_uint16_t(buf, offset, home_dir);
	write_uint8_t(buf, offset, flags);
	write_frame_crc(buf, offset, വലുപ്പം(buf));

	തിരികെ ലിഖ(uart_fd, buf, offset) == offset;
}
