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

#ଯଦି 0 // enable non-verbose debugging
#define GHST_DEBUG PX4_WARN
#ଅନ୍ୟଥା
#define GHST_DEBUG(...)
#endif

#ଯଦି 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#define GHST_VERBOSE PX4_WARN
#ଅନ୍ୟଥା
#define GHST_VERBOSE(...)
#endif

#ଅନ୍ତର୍ଭାଵ <drivers/drv_hrt.h>
#ଅନ୍ତର୍ଭାଵ <termios.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <math.h>

// TODO: include RSSI dBm to percentage conversion for ghost receiver
#ଅନ୍ତର୍ଭାଵ "spektrum_rssi.h"

#ଅନ୍ତର୍ଭାଵ "ghst.hpp"
#ଅନ୍ତର୍ଭାଵ "common_rc.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

#define GHST_FRAME_PAYLOAD_SIZE_TELEMETRY	(10U)
#define GHST_FRAME_CRC_SIZE			(1U)
#define GHST_FRAME_TYPE_SIZE			(1U)
#define GHST_TYPE_DATA_CRC_SIZE			(12U)
#define GHST_ADDR_FC				(130U)
#define GHST_MAX_ଅଙ୍କ_CHANNELS			(16)

ଗଣନା ଶ୍ରେଣୀ ghst_parser_state_t : uint8_t {
	unsynced = 0U,
	synced
};

// only RSSI frame contains value of RSSI, if it is not received, send last received RSSI/LQ
ସ୍ଥିର ghstLinkStatistics_t last_link_stats = { .rssi_pct = -1, .rssi_dbm = NAN, .link_quality = 0 };

ସ୍ଥିର ghst_frame_t &ghst_frame = rc_decode_buf.ghst_frame;
ସ୍ଥିର uint32_t current_frame_position = 0U;
ସ୍ଥିର ghst_parser_state_t parser_state = ghst_parser_state_t::unsynced;

ସ୍ଥିର uint16_t prev_rc_vals[GHST_MAX_ଅଙ୍କ_CHANNELS];

/**
 * parse the current ghst_frame buffer
 */
ସ୍ଥିର ତାର୍କିକ ghst_parse_buffer(uint16_t *ମୂଲ୍ୟଗୁଡ଼ିକ_2, ghstLinkStatistics_t *link_stats, uint16_t *ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ,
			      uint16_t max_channels);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ghst_config(ପୂର୍ଣ୍ଣସଂଖ୍ୟା uart_fd)
{
	ସଂରଚନା termios t;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret_val;

	// no parity, one stop bit
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, GHST_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	memset(prev_rc_vals, static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(UINT16_MAX), ଆକାର(uint16_t) * GHST_MAX_ଅଙ୍କ_CHANNELS);
	ret_val = tcsetattr(uart_fd, TCSANOW, &t);
	ଫେରନ୍ତୁ ret_val;
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
ସ୍ଥିର uint16_t convert_channel_ମୂଲ୍ୟ(unsigned chan_ମୂଲ୍ୟ);


ତାର୍କିକ ghst_parse(ଅଚଳ uint64_t now, ଅଚଳ uint8_t *frame, unsigned len, uint16_t *ମୂଲ୍ୟଗୁଡ଼ିକ_2,
		ghstLinkStatistics_t *link_stats, uint16_t *ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ, uint16_t max_channels)
{
	ତାର୍କିକ success = ମିଥ୍ୟା;
	uint8_t *ghst_frame_ptr = (uint8_t *)&ghst_frame;

	memcpy(ମୂଲ୍ୟଗୁଡ଼ିକ_2, prev_rc_vals, ଆକାର(uint16_t) * GHST_MAX_ଅଙ୍କ_CHANNELS);

	ଯେତେବେଳେ (len > 0U) {

		// fill in the ghst_buffer, as much as we can
		ଅଚଳ uint32_t current_len = MIN(len, ଆକାର(ghst_frame_t) - current_frame_position);
		memcpy(ghst_frame_ptr + current_frame_position, frame, current_len);
		current_frame_position += current_len;

		// protection to guarantee parsing progress
		ଯଦି (current_len == 0U) {
			GHST_DEBUG("========== parser bug: no progress (%u) ===========", len);

			ପାଇଁ (uint32_t i = 0U; i < current_frame_position; ++i) {
				GHST_DEBUG("ghst_frame_ptr[%u]: 0x%x", i, ghst_frame_ptr[i]);
			}

			// reset the parser
			current_frame_position = 0U;
			parser_state = ghst_parser_state_t::unsynced;
			success = ମିଥ୍ୟା;

		} ଅନ୍ୟଥା {
			len -= current_len;
			frame += current_len;

			ଯଦି (ghst_parse_buffer(ମୂଲ୍ୟଗୁଡ଼ିକ_2, link_stats, ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ, max_channels)) {
				success = ସତ୍ୟ;
			}
		}
	}

	ଫେରନ୍ତୁ success;
}

uint8_t ghst_frame_CRC(ଅଚଳ ghst_frame_t &frame)
{
	uint8_t crc = crc8_dvb_s2(0U, frame.type);

	ପାଇଁ (uint32_t i = 0U; i < frame.header.length - GHST_FRAME_CRC_SIZE - GHST_FRAME_TYPE_SIZE; ++i) {
		crc = crc8_dvb_s2(crc, frame.payload[i]);
	}

	ଫେରନ୍ତୁ crc;
}

ସ୍ଥିର uint16_t convert_channel_ମୂଲ୍ୟ(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା chan_ମୂଲ୍ୟ)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	ସ୍ଥିର constexpr ଦଶମିକ scale = (2012.0F - 988.0F) / (1811.0F - 172.0F);
	ସ୍ଥିର constexpr ଦଶମିକ offset = 988.0F - (172.0F * scale);
	ଦଶମିକ scaled_chan_ମୂଲ୍ୟ = scale * static_cast<ଦଶମିକ>(chan_ମୂଲ୍ୟ);
	ଦଶମିକ scaled_chan_ମୂଲ୍ୟ_with_offset = scaled_chan_ମୂଲ୍ୟ + offset;
	uint16_t converted_chan_ମୂଲ୍ୟ = static_cast<uint16_t>(scaled_chan_ମୂଲ୍ୟ_with_offset);

	ଫେରନ୍ତୁ converted_chan_ମୂଲ୍ୟ;
}

ସ୍ଥିର ତାର୍କିକ ghst_parse_buffer(uint16_t *ମୂଲ୍ୟଗୁଡ଼ିକ_2, ghstLinkStatistics_t *link_stats, uint16_t *ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ,
			      uint16_t max_channels)
{
	uint8_t *ghst_frame_ptr = (uint8_t *)&ghst_frame;

	ଯଦି (parser_state == ghst_parser_state_t::unsynced) {
		// there is no sync yet, try to find an RC packet by searching for a matching frame length and type
		ପାଇଁ (uint32_t i = 1U; i < current_frame_position - 1U; ++i) {
			ଯଦି ((ghst_frame_ptr[i + 1U] >= static_cast<uint8_t>(ghstFrameType::frameTypeFirst)) &&
			    (ghst_frame_ptr[i + 1U] <= static_cast<uint8_t>(ghstFrameType::frameTypeLast))) {
				ଯଦି (ghst_frame_ptr[i] == GHST_TYPE_DATA_CRC_SIZE) {
					parser_state = ghst_parser_state_t::synced;
					uint32_t frame_offset = i - 1U;
					GHST_VERBOSE("RC channels found at offset %u", frame_offset);

					// move the rest of the buffer to the beginning
					ଯଦି (frame_offset != 0U) {
						memmove(ghst_frame_ptr, ghst_frame_ptr + frame_offset, current_frame_position - frame_offset);
						current_frame_position -= frame_offset;
					}

					ଭାଙ୍ଗ;
				}
			}
		}
	}

	ଯଦି (parser_state != ghst_parser_state_t::synced) {
		ଯଦି (current_frame_position >= ଆକାର(ghst_frame_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_frame_position = 3U;

			memcpy(ghst_frame_ptr, ghst_frame_ptr + ଆକାର(ghst_frame_t) - current_frame_position, current_frame_position);

			GHST_VERBOSE("Discarding buffer");
		}

		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}


	ଯଦି (current_frame_position < 3U) {
		// wait until we have the address, length and type
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	// now we have at least the header and the type

	ଅଚଳ unsigned current_frame_length = ghst_frame.header.length + ଆକାର(ghst_frame_header_t);

	ଯଦି ((current_frame_length > ଆକାର(ghst_frame_t)) || (current_frame_length < 4U)) {
		// frame too long or bogus (frame length should be longer than 4, at least 1 address, 1 length, 1 type, 1 data, 1 crc)
		// discard everything and go into unsynced state
		current_frame_position = 0U;
		parser_state = ghst_parser_state_t::unsynced;
		GHST_DEBUG("Frame too long/bogus (%u, type=%u) -> unsync", current_frame_length, ghst_frame.type);
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଯଦି (current_frame_position < current_frame_length) {
		// we do not have the full frame yet -> wait for more data
		GHST_VERBOSE("waiting for more data (%u < %u)", current_frame_position, current_frame_length);
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ତାର୍କିକ ret = ମିଥ୍ୟା;

	// now we have the full frame

	ଯଦି ((ghst_frame.type >= static_cast<uint8_t>(ghstFrameType::frameTypeFirst)) &&
	    (ghst_frame.type <= static_cast<uint8_t>(ghstFrameType::frameTypeLast)) &&
	    (ghst_frame.header.length == GHST_TYPE_DATA_CRC_SIZE) &&
	    (ghst_frame.header.device_address == GHST_ADDR_FC)) {
		ଅଚଳ uint8_t crc = ghst_frame.payload[ghst_frame.header.length - 2U];

		ଯଦି (crc == ghst_frame_CRC(ghst_frame)) {
			ଅଚଳ ghstPayloadData_t *ଅଚଳ rcChannels = (ghstPayloadData_t *)&ghst_frame.payload;
			*ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ = MIN(max_channels, GHST_MAX_ଅଙ୍କ_CHANNELS);

			// all frames contain data from chan1to4
			ଯଦି (max_channels > 0U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[0] = convert_channel_ମୂଲ୍ୟ(rcChannels->chan1to4.chan1 >> 1U); }

			ଯଦି (max_channels > 1U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[1] = convert_channel_ମୂଲ୍ୟ(rcChannels->chan1to4.chan2 >> 1U); }

			ଯଦି (max_channels > 2U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[2] = convert_channel_ମୂଲ୍ୟ(rcChannels->chan1to4.chan3 >> 1U); }

			ଯଦି (max_channels > 3U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[3] = convert_channel_ମୂଲ୍ୟ(rcChannels->chan1to4.chan4 >> 1U); }

			ଯଦି (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType5to8)) {
				ଯଦି (max_channels > 4U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[4] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanA << 3U); }

				ଯଦି (max_channels > 5U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[5] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanB << 3U); }

				ଯଦି (max_channels > 6U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[6] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanC << 3U); }

				ଯଦି (max_channels > 7U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[7] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanD << 3U); }

			} ଅନ୍ୟଥା ଯଦି (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType9to12)) {
				ଯଦି (max_channels > 8U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[8] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanA << 3U); }

				ଯଦି (max_channels > 9U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[9] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanB << 3U); }

				ଯଦି (max_channels > 10U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[10] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanC << 3U); }

				ଯଦି (max_channels > 11U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[11] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanD << 3U); }

			} ଅନ୍ୟଥା ଯଦି (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType13to16)) {
				ଯଦି (max_channels > 12U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[12] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanA << 3U); }

				ଯଦି (max_channels > 13U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[13] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanB << 3U); }

				ଯଦି (max_channels > 14U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[14] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanC << 3U); }

				ଯଦି (max_channels > 15U) { ମୂଲ୍ୟଗୁଡ଼ିକ_2[15] = convert_channel_ମୂଲ୍ୟ(rcChannels->chanD << 3U); }

			} ଅନ୍ୟଥା ଯଦି (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameTypeRssi)) {
				ଅଚଳ ghstPayloadRssi_t *ଅଚଳ rssi_ମୂଲ୍ୟଗୁଡ଼ିକ = (ghstPayloadRssi_t *)&ghst_frame.payload;
				// TODO: call function for RSSI dBm to percentage conversion for ghost receiver
				last_link_stats.rssi_pct = spek_dbm_to_percent(static_cast<int8_t>
							   (rssi_ମୂଲ୍ୟଗୁଡ଼ିକ->rssidBm));	// rssidBm sign inverted (90 = -90dBm)
				last_link_stats.rssi_dbm = -rssi_ମୂଲ୍ୟଗୁଡ଼ିକ->rssidBm;
				last_link_stats.link_quality = rssi_ମୂଲ୍ୟଗୁଡ଼ିକ->lq;						// 0 - 100

			} ଅନ୍ୟଥା {
				GHST_DEBUG("Frame type: %u", ghst_frame.type);
			}

			*link_stats = last_link_stats;

			memcpy(prev_rc_vals, ମୂଲ୍ୟଗୁଡ଼ିକ_2, ଆକାର(uint16_t) * GHST_MAX_ଅଙ୍କ_CHANNELS);

			GHST_VERBOSE("Got Channels");

			ret = ସତ୍ୟ;

		} ଅନ୍ୟଥା {
			GHST_DEBUG("CRC check failed");
		}

	} ଅନ୍ୟଥା {
		GHST_DEBUG("Got Non-RC frame (len=%u, type=%u)", current_frame_length, ghst_frame.type);
	}

	// either reset or move the rest of the buffer
	ଯଦି (current_frame_position > current_frame_length) {
		GHST_VERBOSE("Moving buffer (%u > %u)", current_frame_position, current_frame_length);
		memmove(ghst_frame_ptr, ghst_frame_ptr + current_frame_length, current_frame_position - current_frame_length);
		current_frame_position -= current_frame_length;

	} ଅନ୍ୟଥା {
		current_frame_position = 0U;
	}

	ଫେରନ୍ତୁ ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_uint8_t(uint8_t *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା &offset, uint8_t value)
{
	buf[offset++] = value;
}

/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_uint16_t(uint8_t *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା &offset, uint16_t value)
{
	buf[offset] = value & 0xFFU;
	buf[offset + 1] = value >> 8U;
	offset += 2;
}

/**
 * write an uint32_t value to a buffer at a given offset and increment the offset
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_uint32_t(uint8_t *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା &offset, uint32_t value)
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
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_frame_header(uint8_t *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା &offset, ghstTelemetryType type, uint8_t payload_size)
{
	write_uint8_t(buf, offset, static_cast<uint8_t>(ghstAddress::rxAddress));
	write_uint8_t(buf, offset, payload_size + GHST_FRAME_CRC_SIZE + GHST_FRAME_TYPE_SIZE);
	write_uint8_t(buf, offset, static_cast<uint8_t>(type));
}

/**
 * write frame CRC
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_frame_crc(uint8_t *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା &offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା buf_size)
{
	write_uint8_t(buf, offset, crc8_dvb_s2_buf(buf + 2U, buf_size - 3));
}

ତାର୍କିକ ghst_send_telemetry_battery_status(ପୂର୍ଣ୍ଣସଂଖ୍ୟା uart_fd, uint16_t voltage_in_10mV,
					uint16_t current_in_10mA, uint16_t fuel_in_10mAh)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_SIZE_TELEMETRY + 4U]; // address, frame length, type, crc
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = 0;
	write_frame_header(buf, offset, ghstTelemetryType::batteryPack, GHST_FRAME_PAYLOAD_SIZE_TELEMETRY);
	write_uint16_t(buf, offset, voltage_in_10mV);
	write_uint16_t(buf, offset, current_in_10mA);
	write_uint16_t(buf, offset, fuel_in_10mAh);
	write_uint8_t(buf, offset, 0x00U); // empty
	write_uint8_t(buf, offset, 0x00U); // empty
	write_uint8_t(buf, offset, 0x00U); // empty
	write_uint8_t(buf, offset, 0x00U); // empty
	write_frame_crc(buf, offset, ଆକାର(buf));

	ଫେରନ୍ତୁ ଲିଖ(uart_fd, buf, offset) == offset;
}

ତାର୍କିକ ghst_send_telemetry_gps1_status(ପୂର୍ଣ୍ଣସଂଖ୍ୟା uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_SIZE_TELEMETRY + 4U]; // address, frame length, type, crc
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = 0;
	write_frame_header(buf, offset, ghstTelemetryType::gpsPrimary, GHST_FRAME_PAYLOAD_SIZE_TELEMETRY);
	write_uint32_t(buf, offset, latitude);
	write_uint32_t(buf, offset, longitude);
	write_uint16_t(buf, offset, altitude);
	write_frame_crc(buf, offset, ଆକାର(buf));

	ଫେରନ୍ତୁ ଲିଖ(uart_fd, buf, offset) == offset;
}

ତାର୍କିକ ghst_send_telemetry_gps2_status(ପୂର୍ଣ୍ଣସଂଖ୍ୟା uart_fd, uint16_t ground_speed, uint16_t ground_course, uint8_t ଅଙ୍କ_Sats,
				     uint16_t home_dist, uint16_t home_dir, uint8_t flags)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_SIZE_TELEMETRY + 4U]; // address, frame length, type, crc
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = 0;
	write_frame_header(buf, offset, ghstTelemetryType::gpsSecondary, GHST_FRAME_PAYLOAD_SIZE_TELEMETRY);
	write_uint16_t(buf, offset, ground_speed);
	write_uint16_t(buf, offset, ground_course);
	write_uint8_t(buf, offset, ଅଙ୍କ_Sats);
	write_uint16_t(buf, offset, home_dist);
	write_uint16_t(buf, offset, home_dir);
	write_uint8_t(buf, offset, flags);
	write_frame_crc(buf, offset, ଆକାର(buf));

	ଫେରନ୍ତୁ ଲିଖ(uart_fd, buf, offset) == offset;
}
