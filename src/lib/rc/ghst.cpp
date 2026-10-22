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

#ಯದಿ 0 // enable non-verbose debugging
#define GHST_DEBUG PX4_WARN
#ಅನ್ಯಥಾ
#define GHST_DEBUG(...)
#endif

#ಯದಿ 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#define GHST_VERBOSE PX4_WARN
#ಅನ್ಯಥಾ
#define GHST_VERBOSE(...)
#endif

#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>
#ಅನ್ತರ್ಭಾವ <termios.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <math.h>

// TODO: include RSSI dBm to percentage conversion for ghost receiver
#ಅನ್ತರ್ಭಾವ "spektrum_rssi.h"

#ಅನ್ತರ್ಭಾವ "ghst.hpp"
#ಅನ್ತರ್ಭಾವ "common_rc.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

#define GHST_FRAME_PAYLOAD_SIZE_TELEMETRY	(10U)
#define GHST_FRAME_CRC_SIZE			(1U)
#define GHST_FRAME_TYPE_SIZE			(1U)
#define GHST_TYPE_DATA_CRC_SIZE			(12U)
#define GHST_ADDR_FC				(130U)
#define GHST_MAX_ಅಂಕೆ_CHANNELS			(16)

ಗಣನಾ ವರ್ಗ ghst_parser_state_t : uint8_t {
	unsynced = 0U,
	synced
};

// only RSSI frame contains value of RSSI, if it is not received, send last received RSSI/LQ
ಸ್ಥಿರ ghstLinkStatistics_t last_link_stats = { .rssi_pct = -1, .rssi_dbm = NAN, .link_quality = 0 };

ಸ್ಥಿರ ghst_frame_t &ghst_frame = rc_decode_buf.ghst_frame;
ಸ್ಥಿರ uint32_t current_frame_position = 0U;
ಸ್ಥಿರ ghst_parser_state_t parser_state = ghst_parser_state_t::unsynced;

ಸ್ಥಿರ uint16_t prev_rc_vals[GHST_MAX_ಅಂಕೆ_CHANNELS];

/**
 * parse the current ghst_frame buffer
 */
ಸ್ಥಿರ ತಾರ್ಕಿಕ ghst_parse_buffer(uint16_t *ಮೌಲ್ಯಗಳು_2, ghstLinkStatistics_t *link_stats, uint16_t *ಅಂಕೆ_ಮೌಲ್ಯಗಳು,
			      uint16_t max_channels);

ಪೂರ್ಣಾಙ್ಕ ghst_config(ಪೂರ್ಣಾಙ್ಕ uart_fd)
{
	ಸಂರಚನಾ termios t;
	ಪೂರ್ಣಾಙ್ಕ ret_val;

	// no parity, one stop bit
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, GHST_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	memset(prev_rc_vals, static_cast<ಪೂರ್ಣಾಙ್ಕ>(UINT16_MAX), ಪರಿಮಾಣ(uint16_t) * GHST_MAX_ಅಂಕೆ_CHANNELS);
	ret_val = tcsetattr(uart_fd, TCSANOW, &t);
	ಪ್ರತ್ಯಾಗಮ ret_val;
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
ಸ್ಥಿರ uint16_t convert_channel_ಮೌಲ್ಯ(unsigned chan_ಮೌಲ್ಯ);


ತಾರ್ಕಿಕ ghst_parse(ಧ್ರುವ uint64_t now, ಧ್ರುವ uint8_t *frame, unsigned len, uint16_t *ಮೌಲ್ಯಗಳು_2,
		ghstLinkStatistics_t *link_stats, uint16_t *ಅಂಕೆ_ಮೌಲ್ಯಗಳು, uint16_t max_channels)
{
	ತಾರ್ಕಿಕ success = ಅಸತ್ಯ;
	uint8_t *ghst_frame_ptr = (uint8_t *)&ghst_frame;

	memcpy(ಮೌಲ್ಯಗಳು_2, prev_rc_vals, ಪರಿಮಾಣ(uint16_t) * GHST_MAX_ಅಂಕೆ_CHANNELS);

	ಯಾವತ್ (len > 0U) {

		// fill in the ghst_buffer, as much as we can
		ಧ್ರುವ uint32_t current_len = MIN(len, ಪರಿಮಾಣ(ghst_frame_t) - current_frame_position);
		memcpy(ghst_frame_ptr + current_frame_position, frame, current_len);
		current_frame_position += current_len;

		// protection to guarantee parsing progress
		ಯದಿ (current_len == 0U) {
			GHST_DEBUG("========== parser bug: no progress (%u) ===========", len);

			ಪ್ರತಿ (uint32_t i = 0U; i < current_frame_position; ++i) {
				GHST_DEBUG("ghst_frame_ptr[%u]: 0x%x", i, ghst_frame_ptr[i]);
			}

			// reset the parser
			current_frame_position = 0U;
			parser_state = ghst_parser_state_t::unsynced;
			success = ಅಸತ್ಯ;

		} ಅನ್ಯಥಾ {
			len -= current_len;
			frame += current_len;

			ಯದಿ (ghst_parse_buffer(ಮೌಲ್ಯಗಳು_2, link_stats, ಅಂಕೆ_ಮೌಲ್ಯಗಳು, max_channels)) {
				success = ಸತ್ಯ;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ success;
}

uint8_t ghst_frame_CRC(ಧ್ರುವ ghst_frame_t &frame)
{
	uint8_t crc = crc8_dvb_s2(0U, frame.type);

	ಪ್ರತಿ (uint32_t i = 0U; i < frame.header.length - GHST_FRAME_CRC_SIZE - GHST_FRAME_TYPE_SIZE; ++i) {
		crc = crc8_dvb_s2(crc, frame.payload[i]);
	}

	ಪ್ರತ್ಯಾಗಮ crc;
}

ಸ್ಥಿರ uint16_t convert_channel_ಮೌಲ್ಯ(unsigned ಪೂರ್ಣಾಙ್ಕ chan_ಮೌಲ್ಯ)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	ಸ್ಥಿರ constexpr ಭಿನ್ನ scale = (2012.0F - 988.0F) / (1811.0F - 172.0F);
	ಸ್ಥಿರ constexpr ಭಿನ್ನ offset = 988.0F - (172.0F * scale);
	ಭಿನ್ನ scaled_chan_ಮೌಲ್ಯ = scale * static_cast<ಭಿನ್ನ>(chan_ಮೌಲ್ಯ);
	ಭಿನ್ನ scaled_chan_ಮೌಲ್ಯ_with_offset = scaled_chan_ಮೌಲ್ಯ + offset;
	uint16_t converted_chan_ಮೌಲ್ಯ = static_cast<uint16_t>(scaled_chan_ಮೌಲ್ಯ_with_offset);

	ಪ್ರತ್ಯಾಗಮ converted_chan_ಮೌಲ್ಯ;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ ghst_parse_buffer(uint16_t *ಮೌಲ್ಯಗಳು_2, ghstLinkStatistics_t *link_stats, uint16_t *ಅಂಕೆ_ಮೌಲ್ಯಗಳು,
			      uint16_t max_channels)
{
	uint8_t *ghst_frame_ptr = (uint8_t *)&ghst_frame;

	ಯದಿ (parser_state == ghst_parser_state_t::unsynced) {
		// there is no sync yet, try to find an RC packet by searching for a matching frame length and type
		ಪ್ರತಿ (uint32_t i = 1U; i < current_frame_position - 1U; ++i) {
			ಯದಿ ((ghst_frame_ptr[i + 1U] >= static_cast<uint8_t>(ghstFrameType::frameTypeFirst)) &&
			    (ghst_frame_ptr[i + 1U] <= static_cast<uint8_t>(ghstFrameType::frameTypeLast))) {
				ಯದಿ (ghst_frame_ptr[i] == GHST_TYPE_DATA_CRC_SIZE) {
					parser_state = ghst_parser_state_t::synced;
					uint32_t frame_offset = i - 1U;
					GHST_VERBOSE("RC channels found at offset %u", frame_offset);

					// move the rest of the buffer to the beginning
					ಯದಿ (frame_offset != 0U) {
						memmove(ghst_frame_ptr, ghst_frame_ptr + frame_offset, current_frame_position - frame_offset);
						current_frame_position -= frame_offset;
					}

					ಭಙ್ಗ;
				}
			}
		}
	}

	ಯದಿ (parser_state != ghst_parser_state_t::synced) {
		ಯದಿ (current_frame_position >= ಪರಿಮಾಣ(ghst_frame_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_frame_position = 3U;

			memcpy(ghst_frame_ptr, ghst_frame_ptr + ಪರಿಮಾಣ(ghst_frame_t) - current_frame_position, current_frame_position);

			GHST_VERBOSE("Discarding buffer");
		}

		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}


	ಯದಿ (current_frame_position < 3U) {
		// wait until we have the address, length and type
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	// now we have at least the header and the type

	ಧ್ರುವ unsigned current_frame_length = ghst_frame.header.length + ಪರಿಮಾಣ(ghst_frame_header_t);

	ಯದಿ ((current_frame_length > ಪರಿಮಾಣ(ghst_frame_t)) || (current_frame_length < 4U)) {
		// frame too long or bogus (frame length should be longer than 4, at least 1 address, 1 length, 1 type, 1 data, 1 crc)
		// discard everything and go into unsynced state
		current_frame_position = 0U;
		parser_state = ghst_parser_state_t::unsynced;
		GHST_DEBUG("Frame too long/bogus (%u, type=%u) -> unsync", current_frame_length, ghst_frame.type);
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಯದಿ (current_frame_position < current_frame_length) {
		// we do not have the full frame yet -> wait for more data
		GHST_VERBOSE("waiting for more data (%u < %u)", current_frame_position, current_frame_length);
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ತಾರ್ಕಿಕ ret = ಅಸತ್ಯ;

	// now we have the full frame

	ಯದಿ ((ghst_frame.type >= static_cast<uint8_t>(ghstFrameType::frameTypeFirst)) &&
	    (ghst_frame.type <= static_cast<uint8_t>(ghstFrameType::frameTypeLast)) &&
	    (ghst_frame.header.length == GHST_TYPE_DATA_CRC_SIZE) &&
	    (ghst_frame.header.device_address == GHST_ADDR_FC)) {
		ಧ್ರುವ uint8_t crc = ghst_frame.payload[ghst_frame.header.length - 2U];

		ಯದಿ (crc == ghst_frame_CRC(ghst_frame)) {
			ಧ್ರುವ ghstPayloadData_t *ಧ್ರುವ rcChannels = (ghstPayloadData_t *)&ghst_frame.payload;
			*ಅಂಕೆ_ಮೌಲ್ಯಗಳು = MIN(max_channels, GHST_MAX_ಅಂಕೆ_CHANNELS);

			// all frames contain data from chan1to4
			ಯದಿ (max_channels > 0U) { ಮೌಲ್ಯಗಳು_2[0] = convert_channel_ಮೌಲ್ಯ(rcChannels->chan1to4.chan1 >> 1U); }

			ಯದಿ (max_channels > 1U) { ಮೌಲ್ಯಗಳು_2[1] = convert_channel_ಮೌಲ್ಯ(rcChannels->chan1to4.chan2 >> 1U); }

			ಯದಿ (max_channels > 2U) { ಮೌಲ್ಯಗಳು_2[2] = convert_channel_ಮೌಲ್ಯ(rcChannels->chan1to4.chan3 >> 1U); }

			ಯದಿ (max_channels > 3U) { ಮೌಲ್ಯಗಳು_2[3] = convert_channel_ಮೌಲ್ಯ(rcChannels->chan1to4.chan4 >> 1U); }

			ಯದಿ (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType5to8)) {
				ಯದಿ (max_channels > 4U) { ಮೌಲ್ಯಗಳು_2[4] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanA << 3U); }

				ಯದಿ (max_channels > 5U) { ಮೌಲ್ಯಗಳು_2[5] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanB << 3U); }

				ಯದಿ (max_channels > 6U) { ಮೌಲ್ಯಗಳು_2[6] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanC << 3U); }

				ಯದಿ (max_channels > 7U) { ಮೌಲ್ಯಗಳು_2[7] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanD << 3U); }

			} ಅನ್ಯಥಾ ಯದಿ (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType9to12)) {
				ಯದಿ (max_channels > 8U) { ಮೌಲ್ಯಗಳು_2[8] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanA << 3U); }

				ಯದಿ (max_channels > 9U) { ಮೌಲ್ಯಗಳು_2[9] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanB << 3U); }

				ಯದಿ (max_channels > 10U) { ಮೌಲ್ಯಗಳು_2[10] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanC << 3U); }

				ಯದಿ (max_channels > 11U) { ಮೌಲ್ಯಗಳು_2[11] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanD << 3U); }

			} ಅನ್ಯಥಾ ಯದಿ (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType13to16)) {
				ಯದಿ (max_channels > 12U) { ಮೌಲ್ಯಗಳು_2[12] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanA << 3U); }

				ಯದಿ (max_channels > 13U) { ಮೌಲ್ಯಗಳು_2[13] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanB << 3U); }

				ಯದಿ (max_channels > 14U) { ಮೌಲ್ಯಗಳು_2[14] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanC << 3U); }

				ಯದಿ (max_channels > 15U) { ಮೌಲ್ಯಗಳು_2[15] = convert_channel_ಮೌಲ್ಯ(rcChannels->chanD << 3U); }

			} ಅನ್ಯಥಾ ಯದಿ (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameTypeRssi)) {
				ಧ್ರುವ ghstPayloadRssi_t *ಧ್ರುವ rssi_ಮೌಲ್ಯಗಳು = (ghstPayloadRssi_t *)&ghst_frame.payload;
				// TODO: call function for RSSI dBm to percentage conversion for ghost receiver
				last_link_stats.rssi_pct = spek_dbm_to_percent(static_cast<int8_t>
							   (rssi_ಮೌಲ್ಯಗಳು->rssidBm));	// rssidBm sign inverted (90 = -90dBm)
				last_link_stats.rssi_dbm = -rssi_ಮೌಲ್ಯಗಳು->rssidBm;
				last_link_stats.link_quality = rssi_ಮೌಲ್ಯಗಳು->lq;						// 0 - 100

			} ಅನ್ಯಥಾ {
				GHST_DEBUG("Frame type: %u", ghst_frame.type);
			}

			*link_stats = last_link_stats;

			memcpy(prev_rc_vals, ಮೌಲ್ಯಗಳು_2, ಪರಿಮಾಣ(uint16_t) * GHST_MAX_ಅಂಕೆ_CHANNELS);

			GHST_VERBOSE("Got Channels");

			ret = ಸತ್ಯ;

		} ಅನ್ಯಥಾ {
			GHST_DEBUG("CRC check failed");
		}

	} ಅನ್ಯಥಾ {
		GHST_DEBUG("Got Non-RC frame (len=%u, type=%u)", current_frame_length, ghst_frame.type);
	}

	// either reset or move the rest of the buffer
	ಯದಿ (current_frame_position > current_frame_length) {
		GHST_VERBOSE("Moving buffer (%u > %u)", current_frame_position, current_frame_length);
		memmove(ghst_frame_ptr, ghst_frame_ptr + current_frame_length, current_frame_position - current_frame_length);
		current_frame_position -= current_frame_length;

	} ಅನ್ಯಥಾ {
		current_frame_position = 0U;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_uint8_t(uint8_t *buf, ಪೂರ್ಣಾಙ್ಕ &offset, uint8_t value)
{
	buf[offset++] = value;
}

/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_uint16_t(uint8_t *buf, ಪೂರ್ಣಾಙ್ಕ &offset, uint16_t value)
{
	buf[offset] = value & 0xFFU;
	buf[offset + 1] = value >> 8U;
	offset += 2;
}

/**
 * write an uint32_t value to a buffer at a given offset and increment the offset
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_uint32_t(uint8_t *buf, ಪೂರ್ಣಾಙ್ಕ &offset, uint32_t value)
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
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_frame_header(uint8_t *buf, ಪೂರ್ಣಾಙ್ಕ &offset, ghstTelemetryType type, uint8_t payload_size)
{
	write_uint8_t(buf, offset, static_cast<uint8_t>(ghstAddress::rxAddress));
	write_uint8_t(buf, offset, payload_size + GHST_FRAME_CRC_SIZE + GHST_FRAME_TYPE_SIZE);
	write_uint8_t(buf, offset, static_cast<uint8_t>(type));
}

/**
 * write frame CRC
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_frame_crc(uint8_t *buf, ಪೂರ್ಣಾಙ್ಕ &offset, ಪೂರ್ಣಾಙ್ಕ buf_size)
{
	write_uint8_t(buf, offset, crc8_dvb_s2_buf(buf + 2U, buf_size - 3));
}

ತಾರ್ಕಿಕ ghst_send_telemetry_battery_status(ಪೂರ್ಣಾಙ್ಕ uart_fd, uint16_t voltage_in_10mV,
					uint16_t current_in_10mA, uint16_t fuel_in_10mAh)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_SIZE_TELEMETRY + 4U]; // address, frame length, type, crc
	ಪೂರ್ಣಾಙ್ಕ offset = 0;
	write_frame_header(buf, offset, ghstTelemetryType::batteryPack, GHST_FRAME_PAYLOAD_SIZE_TELEMETRY);
	write_uint16_t(buf, offset, voltage_in_10mV);
	write_uint16_t(buf, offset, current_in_10mA);
	write_uint16_t(buf, offset, fuel_in_10mAh);
	write_uint8_t(buf, offset, 0x00U); // empty
	write_uint8_t(buf, offset, 0x00U); // empty
	write_uint8_t(buf, offset, 0x00U); // empty
	write_uint8_t(buf, offset, 0x00U); // empty
	write_frame_crc(buf, offset, ಪರಿಮಾಣ(buf));

	ಪ್ರತ್ಯಾಗಮ ಲಿಖ(uart_fd, buf, offset) == offset;
}

ತಾರ್ಕಿಕ ghst_send_telemetry_gps1_status(ಪೂರ್ಣಾಙ್ಕ uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_SIZE_TELEMETRY + 4U]; // address, frame length, type, crc
	ಪೂರ್ಣಾಙ್ಕ offset = 0;
	write_frame_header(buf, offset, ghstTelemetryType::gpsPrimary, GHST_FRAME_PAYLOAD_SIZE_TELEMETRY);
	write_uint32_t(buf, offset, latitude);
	write_uint32_t(buf, offset, longitude);
	write_uint16_t(buf, offset, altitude);
	write_frame_crc(buf, offset, ಪರಿಮಾಣ(buf));

	ಪ್ರತ್ಯಾಗಮ ಲಿಖ(uart_fd, buf, offset) == offset;
}

ತಾರ್ಕಿಕ ghst_send_telemetry_gps2_status(ಪೂರ್ಣಾಙ್ಕ uart_fd, uint16_t ground_speed, uint16_t ground_course, uint8_t ಅಂಕೆ_Sats,
				     uint16_t home_dist, uint16_t home_dir, uint8_t flags)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_SIZE_TELEMETRY + 4U]; // address, frame length, type, crc
	ಪೂರ್ಣಾಙ್ಕ offset = 0;
	write_frame_header(buf, offset, ghstTelemetryType::gpsSecondary, GHST_FRAME_PAYLOAD_SIZE_TELEMETRY);
	write_uint16_t(buf, offset, ground_speed);
	write_uint16_t(buf, offset, ground_course);
	write_uint8_t(buf, offset, ಅಂಕೆ_Sats);
	write_uint16_t(buf, offset, home_dist);
	write_uint16_t(buf, offset, home_dir);
	write_uint8_t(buf, offset, flags);
	write_frame_crc(buf, offset, ಪರಿಮಾಣ(buf));

	ಪ್ರತ್ಯಾಗಮ ಲಿಖ(uart_fd, buf, offset) == offset;
}
