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

#यदि 0 // enable non-verbose debugging
#परिभाषय GHST_DEBUG PX4_WARN
#अन्यथा
#परिभाषय GHST_DEBUG(...)
#यदि_अन्तः

#यदि 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#परिभाषय GHST_VERBOSE PX4_WARN
#अन्यथा
#परिभाषय GHST_VERBOSE(...)
#यदि_अन्तः

#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <termios.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <math.h>

// TODO: include RSSI dBm to percentage conversion for ghost receiver
#अन्तर्भाव "spektrum_rssi.h"

#अन्तर्भाव "ghst.hpp"
#अन्तर्भाव "common_rc.h"

#परिभाषय MIN(a,b) (((a)<(b))?(a):(b))

#परिभाषय GHST_FRAME_PAYLOAD_आकार_TELEMETRY	(10U)
#परिभाषय GHST_FRAME_CRC_आकार			(1U)
#परिभाषय GHST_FRAME_TYPE_आकार			(1U)
#परिभाषय GHST_TYPE_दत्त_CRC_आकार			(12U)
#परिभाषय GHST_ADDR_FC				(130U)
#परिभाषय GHST_MAX_अङ्कः_CHANNELS			(16)

गणना वर्ग ghst_parser_state_t : uint8_t {
	unsynced = 0U,
	synced
};

// only RSSI frame contains value of RSSI, if it is not received, send last received RSSI/LQ
स्थिर ghstLinkStatistics_t चरमः_link_stats = { .rssi_pct = -1, .rssi_dbm = NAN, .link_quality = 0 };

स्थिर ghst_frame_t &ghst_frame = rc_decode_buf.ghst_frame;
स्थिर uint32_t current_frame_position = 0U;
स्थिर ghst_parser_state_t parser_state = ghst_parser_state_t::unsynced;

स्थिर uint16_t पूर्व_rc_vals[GHST_MAX_अङ्कः_CHANNELS];

/**
 * parse the current ghst_frame buffer
 */
स्थिर तार्किक ghst_parse_बफर(uint16_t *मानानि_2, ghstLinkStatistics_t *link_stats, uint16_t *अङ्कः_मानानि,
			      uint16_t max_channels);

पूर्णाङ्क ghst_config(पूर्णाङ्क uart_fd)
{
	संरचना termios t;
	पूर्णाङ्क ret_val;

	// no parity, one stop bit
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, GHST_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	स्मृतिं_पूरय(पूर्व_rc_vals, static_cast<पूर्णाङ्क>(UINT16_MAX), परिमाण(uint16_t) * GHST_MAX_अङ्कः_CHANNELS);
	ret_val = tcsetattr(uart_fd, TCSANOW, &t);
	प्रत्यागम ret_val;
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
स्थिर uint16_t convert_channel_मानम्(अचिह्नित chan_मानम्);


तार्किक ghst_parse(ध्रुव uint64_t now, ध्रुव uint8_t *frame, अचिह्नित len, uint16_t *मानानि_2,
		ghstLinkStatistics_t *link_stats, uint16_t *अङ्कः_मानानि, uint16_t max_channels)
{
	तार्किक success = असत्य;
	uint8_t *ghst_frame_ptr = (uint8_t *)&ghst_frame;

	स्मृतिं_प्रतिलिख(मानानि_2, पूर्व_rc_vals, परिमाण(uint16_t) * GHST_MAX_अङ्कः_CHANNELS);

	यावत् (len > 0U) {

		// fill in the ghst_buffer, as much as we can
		ध्रुव uint32_t current_len = MIN(len, परिमाण(ghst_frame_t) - current_frame_position);
		स्मृतिं_प्रतिलिख(ghst_frame_ptr + current_frame_position, frame, current_len);
		current_frame_position += current_len;

		// protection to guarantee parsing progress
		यदि (current_len == 0U) {
			GHST_DEBUG("========== parser bug: no progress (%u) ===========", len);

			प्रति (uint32_t i = 0U; i < current_frame_position; ++i) {
				GHST_DEBUG("ghst_frame_ptr[%u]: 0x%x", i, ghst_frame_ptr[i]);
			}

			// reset the parser
			current_frame_position = 0U;
			parser_state = ghst_parser_state_t::unsynced;
			success = असत्य;

		} अन्यथा {
			len -= current_len;
			frame += current_len;

			यदि (ghst_parse_बफर(मानानि_2, link_stats, अङ्कः_मानानि, max_channels)) {
				success = सत्य;
			}
		}
	}

	प्रत्यागम success;
}

uint8_t ghst_frame_CRC(ध्रुव ghst_frame_t &frame)
{
	uint8_t crc = crc8_dvb_s2(0U, frame.type);

	प्रति (uint32_t i = 0U; i < frame.शीर्षकम्.दीर्घता_2 - GHST_FRAME_CRC_आकार - GHST_FRAME_TYPE_आकार; ++i) {
		crc = crc8_dvb_s2(crc, frame.payload[i]);
	}

	प्रत्यागम crc;
}

स्थिर uint16_t convert_channel_मानम्(अचिह्नित पूर्णाङ्क chan_मानम्)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	स्थिर constexpr भिन्न scale = (2012.0F - 988.0F) / (1811.0F - 172.0F);
	स्थिर constexpr भिन्न विस्थापनम् = 988.0F - (172.0F * scale);
	भिन्न scaled_chan_मानम् = scale * static_cast<भिन्न>(chan_मानम्);
	भिन्न scaled_chan_मानम्_with_विस्थापनम् = scaled_chan_मानम् + विस्थापनम्;
	uint16_t converted_chan_मानम् = static_cast<uint16_t>(scaled_chan_मानम्_with_विस्थापनम्);

	प्रत्यागम converted_chan_मानम्;
}

स्थिर तार्किक ghst_parse_बफर(uint16_t *मानानि_2, ghstLinkStatistics_t *link_stats, uint16_t *अङ्कः_मानानि,
			      uint16_t max_channels)
{
	uint8_t *ghst_frame_ptr = (uint8_t *)&ghst_frame;

	यदि (parser_state == ghst_parser_state_t::unsynced) {
		// there is no sync yet, try to find an RC packet by searching for a matching frame length and type
		प्रति (uint32_t i = 1U; i < current_frame_position - 1U; ++i) {
			यदि ((ghst_frame_ptr[i + 1U] >= static_cast<uint8_t>(ghstFrameType::frameTypeFirst)) &&
			    (ghst_frame_ptr[i + 1U] <= static_cast<uint8_t>(ghstFrameType::frame_Type_चरमः))) {
				यदि (ghst_frame_ptr[i] == GHST_TYPE_दत्त_CRC_आकार) {
					parser_state = ghst_parser_state_t::synced;
					uint32_t frame_विस्थापनम् = i - 1U;
					GHST_VERBOSE("RC channels found at offset %u", frame_विस्थापनम्);

					// move the rest of the buffer to the beginning
					यदि (frame_विस्थापनम् != 0U) {
						memmove(ghst_frame_ptr, ghst_frame_ptr + frame_विस्थापनम्, current_frame_position - frame_विस्थापनम्);
						current_frame_position -= frame_विस्थापनम्;
					}

					भङ्ग;
				}
			}
		}
	}

	यदि (parser_state != ghst_parser_state_t::synced) {
		यदि (current_frame_position >= परिमाण(ghst_frame_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_frame_position = 3U;

			स्मृतिं_प्रतिलिख(ghst_frame_ptr, ghst_frame_ptr + परिमाण(ghst_frame_t) - current_frame_position, current_frame_position);

			GHST_VERBOSE("Discarding buffer");
		}

		प्रत्यागम असत्य;
	}


	यदि (current_frame_position < 3U) {
		// wait until we have the address, length and type
		प्रत्यागम असत्य;
	}

	// now we have at least the header and the type

	ध्रुव अचिह्नित current_frame_दीर्घता = ghst_frame.शीर्षकम्.दीर्घता_2 + परिमाण(ghst_frame_शीर्षकम्_t);

	यदि ((current_frame_दीर्घता > परिमाण(ghst_frame_t)) || (current_frame_दीर्घता < 4U)) {
		// frame too long or bogus (frame length should be longer than 4, at least 1 address, 1 length, 1 type, 1 data, 1 crc)
		// discard everything and go into unsynced state
		current_frame_position = 0U;
		parser_state = ghst_parser_state_t::unsynced;
		GHST_DEBUG("Frame too long/bogus (%u, type=%u) -> unsync", current_frame_दीर्घता, ghst_frame.type);
		प्रत्यागम असत्य;
	}

	यदि (current_frame_position < current_frame_दीर्घता) {
		// we do not have the full frame yet -> wait for more data
		GHST_VERBOSE("waiting for more data (%u < %u)", current_frame_position, current_frame_दीर्घता);
		प्रत्यागम असत्य;
	}

	तार्किक ret = असत्य;

	// now we have the full frame

	यदि ((ghst_frame.type >= static_cast<uint8_t>(ghstFrameType::frameTypeFirst)) &&
	    (ghst_frame.type <= static_cast<uint8_t>(ghstFrameType::frame_Type_चरमः)) &&
	    (ghst_frame.शीर्षकम्.दीर्घता_2 == GHST_TYPE_दत्त_CRC_आकार) &&
	    (ghst_frame.शीर्षकम्.device_स्थानाङ्कः == GHST_ADDR_FC)) {
		ध्रुव uint8_t crc = ghst_frame.payload[ghst_frame.शीर्षकम्.दीर्घता_2 - 2U];

		यदि (crc == ghst_frame_CRC(ghst_frame)) {
			ध्रुव ghst_Payload_दत्त_t *ध्रुव rcChannels = (ghst_Payload_दत्त_t *)&ghst_frame.payload;
			*अङ्कः_मानानि = MIN(max_channels, GHST_MAX_अङ्कः_CHANNELS);

			// all frames contain data from chan1to4
			यदि (max_channels > 0U) { मानानि_2[0] = convert_channel_मानम्(rcChannels->chan1to4.chan1 >> 1U); }

			यदि (max_channels > 1U) { मानानि_2[1] = convert_channel_मानम्(rcChannels->chan1to4.chan2 >> 1U); }

			यदि (max_channels > 2U) { मानानि_2[2] = convert_channel_मानम्(rcChannels->chan1to4.chan3 >> 1U); }

			यदि (max_channels > 3U) { मानानि_2[3] = convert_channel_मानम्(rcChannels->chan1to4.chan4 >> 1U); }

			यदि (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType5to8)) {
				यदि (max_channels > 4U) { मानानि_2[4] = convert_channel_मानम्(rcChannels->chanA << 3U); }

				यदि (max_channels > 5U) { मानानि_2[5] = convert_channel_मानम्(rcChannels->chanB << 3U); }

				यदि (max_channels > 6U) { मानानि_2[6] = convert_channel_मानम्(rcChannels->chanC << 3U); }

				यदि (max_channels > 7U) { मानानि_2[7] = convert_channel_मानम्(rcChannels->chanD << 3U); }

			} अन्यथा यदि (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType9to12)) {
				यदि (max_channels > 8U) { मानानि_2[8] = convert_channel_मानम्(rcChannels->chanA << 3U); }

				यदि (max_channels > 9U) { मानानि_2[9] = convert_channel_मानम्(rcChannels->chanB << 3U); }

				यदि (max_channels > 10U) { मानानि_2[10] = convert_channel_मानम्(rcChannels->chanC << 3U); }

				यदि (max_channels > 11U) { मानानि_2[11] = convert_channel_मानम्(rcChannels->chanD << 3U); }

			} अन्यथा यदि (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameType13to16)) {
				यदि (max_channels > 12U) { मानानि_2[12] = convert_channel_मानम्(rcChannels->chanA << 3U); }

				यदि (max_channels > 13U) { मानानि_2[13] = convert_channel_मानम्(rcChannels->chanB << 3U); }

				यदि (max_channels > 14U) { मानानि_2[14] = convert_channel_मानम्(rcChannels->chanC << 3U); }

				यदि (max_channels > 15U) { मानानि_2[15] = convert_channel_मानम्(rcChannels->chanD << 3U); }

			} अन्यथा यदि (ghst_frame.type == static_cast<uint8_t>(ghstFrameType::frameTypeRssi)) {
				ध्रुव ghstPayloadRssi_t *ध्रुव rssi_मानानि = (ghstPayloadRssi_t *)&ghst_frame.payload;
				// TODO: call function for RSSI dBm to percentage conversion for ghost receiver
				चरमः_link_stats.rssi_pct = spek_dbm_गन्तव्यम्_percent(static_cast<int8_t>
							   (rssi_मानानि->rssidBm));	// rssidBm sign inverted (90 = -90dBm)
				चरमः_link_stats.rssi_dbm = -rssi_मानानि->rssidBm;
				चरमः_link_stats.link_quality = rssi_मानानि->lq;						// 0 - 100

			} अन्यथा {
				GHST_DEBUG("Frame type: %u", ghst_frame.type);
			}

			*link_stats = चरमः_link_stats;

			स्मृतिं_प्रतिलिख(पूर्व_rc_vals, मानानि_2, परिमाण(uint16_t) * GHST_MAX_अङ्कः_CHANNELS);

			GHST_VERBOSE("Got Channels");

			ret = सत्य;

		} अन्यथा {
			GHST_DEBUG("CRC check failed");
		}

	} अन्यथा {
		GHST_DEBUG("Got Non-RC frame (len=%u, type=%u)", current_frame_दीर्घता, ghst_frame.type);
	}

	// either reset or move the rest of the buffer
	यदि (current_frame_position > current_frame_दीर्घता) {
		GHST_VERBOSE("Moving buffer (%u > %u)", current_frame_position, current_frame_दीर्घता);
		memmove(ghst_frame_ptr, ghst_frame_ptr + current_frame_दीर्घता, current_frame_position - current_frame_दीर्घता);
		current_frame_position -= current_frame_दीर्घता;

	} अन्यथा {
		current_frame_position = 0U;
	}

	प्रत्यागम ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
स्थिर अन्तःस्थ शून्य write_uint8_t(uint8_t *buf, पूर्णाङ्क &विस्थापनम्, uint8_t value)
{
	buf[विस्थापनम्++] = value;
}

/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
स्थिर अन्तःस्थ शून्य write_uint16_t(uint8_t *buf, पूर्णाङ्क &विस्थापनम्, uint16_t value)
{
	buf[विस्थापनम्] = value & 0xFFU;
	buf[विस्थापनम् + 1] = value >> 8U;
	विस्थापनम् += 2;
}

/**
 * write an uint32_t value to a buffer at a given offset and increment the offset
 */
स्थिर अन्तःस्थ शून्य write_uint32_t(uint8_t *buf, पूर्णाङ्क &विस्थापनम्, uint32_t value)
{
	// Little Endian
	buf[विस्थापनम्] = value & 0xFFU;
	buf[विस्थापनम् + 1] = (value & 0xFF00) >> 8U;
	buf[विस्थापनम् + 2] = (value & 0xFF0000) >> 16U;
	buf[विस्थापनम् + 3] = (value & 0xFF000000) >> 24U;
	विस्थापनम् += 4;
}

/**
 * write frame header
 */
स्थिर अन्तःस्थ शून्य write_frame_शीर्षकम्(uint8_t *buf, पूर्णाङ्क &विस्थापनम्, ghstTelemetryType type, uint8_t payload_आकार)
{
	write_uint8_t(buf, विस्थापनम्, static_cast<uint8_t>(ghst_स्थानाङ्कः::rx_स्थानाङ्कः));
	write_uint8_t(buf, विस्थापनम्, payload_आकार + GHST_FRAME_CRC_आकार + GHST_FRAME_TYPE_आकार);
	write_uint8_t(buf, विस्थापनम्, static_cast<uint8_t>(type));
}

/**
 * write frame CRC
 */
स्थिर अन्तःस्थ शून्य write_frame_crc(uint8_t *buf, पूर्णाङ्क &विस्थापनम्, पूर्णाङ्क buf_आकार)
{
	write_uint8_t(buf, विस्थापनम्, crc8_dvb_s2_buf(buf + 2U, buf_आकार - 3));
}

तार्किक ghst_send_telemetry_battery_स्थितिः(पूर्णाङ्क uart_fd, uint16_t voltage_अन्तर्_10m_V,
					uint16_t current_अन्तर्_10m_A, uint16_t fuel_अन्तर्_10m_Ah)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_आकार_TELEMETRY + 4U]; // address, frame length, type, crc
	पूर्णाङ्क विस्थापनम् = 0;
	write_frame_शीर्षकम्(buf, विस्थापनम्, ghstTelemetryType::batteryPack, GHST_FRAME_PAYLOAD_आकार_TELEMETRY);
	write_uint16_t(buf, विस्थापनम्, voltage_अन्तर्_10m_V);
	write_uint16_t(buf, विस्थापनम्, current_अन्तर्_10m_A);
	write_uint16_t(buf, विस्थापनम्, fuel_अन्तर्_10m_Ah);
	write_uint8_t(buf, विस्थापनम्, 0x00U); //रिक्तानि
	write_uint8_t(buf, विस्थापनम्, 0x00U); //रिक्तानि
	write_uint8_t(buf, विस्थापनम्, 0x00U); //रिक्तानि
	write_uint8_t(buf, विस्थापनम्, 0x00U); //रिक्तानि
	write_frame_crc(buf, विस्थापनम्, परिमाण(buf));

	प्रत्यागम लिख(uart_fd, buf, विस्थापनम्) == विस्थापनम्;
}

तार्किक ghst_send_telemetry_gps1_स्थितिः(पूर्णाङ्क uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_आकार_TELEMETRY + 4U]; // address, frame length, type, crc
	पूर्णाङ्क विस्थापनम् = 0;
	write_frame_शीर्षकम्(buf, विस्थापनम्, ghstTelemetryType::gpsPrimary, GHST_FRAME_PAYLOAD_आकार_TELEMETRY);
	write_uint32_t(buf, विस्थापनम्, latitude);
	write_uint32_t(buf, विस्थापनम्, longitude);
	write_uint16_t(buf, विस्थापनम्, altitude);
	write_frame_crc(buf, विस्थापनम्, परिमाण(buf));

	प्रत्यागम लिख(uart_fd, buf, विस्थापनम्) == विस्थापनम्;
}

तार्किक ghst_send_telemetry_gps2_स्थितिः(पूर्णाङ्क uart_fd, uint16_t ground_जवः, uint16_t ground_course, uint8_t अङ्कः_Sats,
				     uint16_t home_dist, uint16_t home_dir, uint8_t flags)
{
	uint8_t buf[GHST_FRAME_PAYLOAD_आकार_TELEMETRY + 4U]; // address, frame length, type, crc
	पूर्णाङ्क विस्थापनम् = 0;
	write_frame_शीर्षकम्(buf, विस्थापनम्, ghstTelemetryType::gpsSecondary, GHST_FRAME_PAYLOAD_आकार_TELEMETRY);
	write_uint16_t(buf, विस्थापनम्, ground_जवः);
	write_uint16_t(buf, विस्थापनम्, ground_course);
	write_uint8_t(buf, विस्थापनम्, अङ्कः_Sats);
	write_uint16_t(buf, विस्थापनम्, home_dist);
	write_uint16_t(buf, विस्थापनम्, home_dir);
	write_uint8_t(buf, विस्थापनम्, flags);
	write_frame_crc(buf, विस्थापनम्, परिमाण(buf));

	प्रत्यागम लिख(uart_fd, buf, विस्थापनम्) == विस्थापनम्;
}
