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

#ఒకవేళ 0 // enable non-verbose debugging
#నిర్వచించు GHST_DEBUG PX4_WARN
#లేకపోతే
#నిర్వచించు GHST_DEBUG(...)
#అయితే_ముగింపు

#ఒకవేళ 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#నిర్వచించు GHST_VERBOSE PX4_WARN
#లేకపోతే
#నిర్వచించు GHST_VERBOSE(...)
#అయితే_ముగింపు

#అన్తర్భావ <drivers/drv_hrt.h>
#అన్తర్భావ <termios.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <math.h>

// TODO: include RSSI dBm to percentage conversion for ghost receiver
#అన్తర్భావ "spektrum_rssi.h"

#అన్తర్భావ "ghst.hpp"
#అన్తర్భావ "common_rc.h"

#నిర్వచించు MIN(a,b) (((a)<(b))?(a):(b))

#నిర్వచించు GHST_చట్రం_PAYLOAD_కొలత_TELEMETRY	(10U)
#నిర్వచించు GHST_చట్రం_చక్రీయ_తనిఖీ_కొలత			(1U)
#నిర్వచించు GHST_చట్రం_TYPE_కొలత			(1U)
#నిర్వచించు GHST_TYPE_దత్తాంశం_చక్రీయ_తనిఖీ_కొలత			(12U)
#నిర్వచించు GHST_ADDR_FC				(130U)
#నిర్వచించు GHST_MAX_అంకె_CHANNELS			(16)

గణన తరగతి ghst_parser_state_t : uint8_t {
	unsynced = 0U,
	synced
};

// only RSSI frame contains value of RSSI, if it is not received, send last received RSSI/LQ
స్థిర ghstLinkStatistics_t చివరి_link_stats = { .rssi_pct = -1, .rssi_dbm = NAN, .link_quality = 0 };

స్థిర ghst_చట్రం_t &ghst_చట్రం = rc_decode_buf.ghst_చట్రం;
స్థిర uint32_t current_చట్రం_position = 0U;
స్థిర ghst_parser_state_t parser_state = ghst_parser_state_t::unsynced;

స్థిర uint16_t prev_rc_vals[GHST_MAX_అంకె_CHANNELS];

/**
 * parse the current ghst_frame buffer
 */
స్థిర తార్కిక ghst_parse_బఫరు(uint16_t *విలువలు_2, ghstLinkStatistics_t *link_stats, uint16_t *అంకె_విలువలు,
			      uint16_t max_channels);

పూర్ణాంకం ghst_config(పూర్ణాంకం uart_fd)
{
	నిర్మాణం termios t;
	పూర్ణాంకం ret_val;

	// no parity, one stop bit
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, GHST_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	memset(prev_rc_vals, static_cast<పూర్ణాంకం>(UINT16_MAX), పరిమాణం(uint16_t) * GHST_MAX_అంకె_CHANNELS);
	ret_val = tcsetattr(uart_fd, TCSANOW, &t);
	తిరిగి ret_val;
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
స్థిర uint16_t convert_channel_విలువ(గుర్తులేని chan_విలువ);


తార్కిక ghst_parse(స్థిరరాశి uint64_t now, స్థిరరాశి uint8_t *చట్రం, గుర్తులేని len, uint16_t *విలువలు_2,
		ghstLinkStatistics_t *link_stats, uint16_t *అంకె_విలువలు, uint16_t max_channels)
{
	తార్కిక success = అబద్ధం;
	uint8_t *ghst_చట్రం_ptr = (uint8_t *)&ghst_చట్రం;

	జ్ఞాపకం_నకలు(విలువలు_2, prev_rc_vals, పరిమాణం(uint16_t) * GHST_MAX_అంకె_CHANNELS);

	అయితే (len > 0U) {

		// fill in the ghst_buffer, as much as we can
		స్థిరరాశి uint32_t current_len = MIN(len, పరిమాణం(ghst_చట్రం_t) - current_చట్రం_position);
		జ్ఞాపకం_నకలు(ghst_చట్రం_ptr + current_చట్రం_position, చట్రం, current_len);
		current_చట్రం_position += current_len;

		// protection to guarantee parsing progress
		ఒకవేళ (current_len == 0U) {
			GHST_DEBUG("========== parser bug: no progress (%u) ===========", len);

			కొరకు (uint32_t i = 0U; i < current_చట్రం_position; ++i) {
				GHST_DEBUG("ghst_frame_ptr[%u]: 0x%x", i, ghst_చట్రం_ptr[i]);
			}

			// reset the parser
			current_చట్రం_position = 0U;
			parser_state = ghst_parser_state_t::unsynced;
			success = అబద్ధం;

		} లేకపోతే {
			len -= current_len;
			చట్రం += current_len;

			ఒకవేళ (ghst_parse_బఫరు(విలువలు_2, link_stats, అంకె_విలువలు, max_channels)) {
				success = నిజం;
			}
		}
	}

	తిరిగి success;
}

uint8_t ghst_చట్రం_చక్రీయ_తనిఖీ(స్థిరరాశి ghst_చట్రం_t &చట్రం)
{
	uint8_t చక్రీయ_తనిఖీ = crc8_dvb_s2(0U, చట్రం.type);

	కొరకు (uint32_t i = 0U; i < చట్రం.శీర్షిక.నిడివి - GHST_చట్రం_చక్రీయ_తనిఖీ_కొలత - GHST_చట్రం_TYPE_కొలత; ++i) {
		చక్రీయ_తనిఖీ = crc8_dvb_s2(చక్రీయ_తనిఖీ, చట్రం.payload[i]);
	}

	తిరిగి చక్రీయ_తనిఖీ;
}

స్థిర uint16_t convert_channel_విలువ(గుర్తులేని పూర్ణాంకం chan_విలువ)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	స్థిర constexpr దశాంశం scale = (2012.0F - 988.0F) / (1811.0F - 172.0F);
	స్థిర constexpr దశాంశం స్థానభ్రంశం = 988.0F - (172.0F * scale);
	దశాంశం scaled_chan_విలువ = scale * static_cast<దశాంశం>(chan_విలువ);
	దశాంశం scaled_chan_విలువ_with_స్థానభ్రంశం = scaled_chan_విలువ + స్థానభ్రంశం;
	uint16_t converted_chan_విలువ = static_cast<uint16_t>(scaled_chan_విలువ_with_స్థానభ్రంశం);

	తిరిగి converted_chan_విలువ;
}

స్థిర తార్కిక ghst_parse_బఫరు(uint16_t *విలువలు_2, ghstLinkStatistics_t *link_stats, uint16_t *అంకె_విలువలు,
			      uint16_t max_channels)
{
	uint8_t *ghst_చట్రం_ptr = (uint8_t *)&ghst_చట్రం;

	ఒకవేళ (parser_state == ghst_parser_state_t::unsynced) {
		// there is no sync yet, try to find an RC packet by searching for a matching frame length and type
		కొరకు (uint32_t i = 1U; i < current_చట్రం_position - 1U; ++i) {
			ఒకవేళ ((ghst_చట్రం_ptr[i + 1U] >= static_cast<uint8_t>(ghst_చట్రం_Type::చట్రం_Type_మొదటి)) &&
			    (ghst_చట్రం_ptr[i + 1U] <= static_cast<uint8_t>(ghst_చట్రం_Type::చట్రం_Type_చివరి))) {
				ఒకవేళ (ghst_చట్రం_ptr[i] == GHST_TYPE_దత్తాంశం_చక్రీయ_తనిఖీ_కొలత) {
					parser_state = ghst_parser_state_t::synced;
					uint32_t చట్రం_స్థానభ్రంశం = i - 1U;
					GHST_VERBOSE("RC channels found at offset %u", చట్రం_స్థానభ్రంశం);

					// move the rest of the buffer to the beginning
					ఒకవేళ (చట్రం_స్థానభ్రంశం != 0U) {
						memmove(ghst_చట్రం_ptr, ghst_చట్రం_ptr + చట్రం_స్థానభ్రంశం, current_చట్రం_position - చట్రం_స్థానభ్రంశం);
						current_చట్రం_position -= చట్రం_స్థానభ్రంశం;
					}

					ఆపు;
				}
			}
		}
	}

	ఒకవేళ (parser_state != ghst_parser_state_t::synced) {
		ఒకవేళ (current_చట్రం_position >= పరిమాణం(ghst_చట్రం_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_చట్రం_position = 3U;

			జ్ఞాపకం_నకలు(ghst_చట్రం_ptr, ghst_చట్రం_ptr + పరిమాణం(ghst_చట్రం_t) - current_చట్రం_position, current_చట్రం_position);

			GHST_VERBOSE("Discarding buffer");
		}

		తిరిగి అబద్ధం;
	}


	ఒకవేళ (current_చట్రం_position < 3U) {
		// wait until we have the address, length and type
		తిరిగి అబద్ధం;
	}

	// now we have at least the header and the type

	స్థిరరాశి గుర్తులేని current_చట్రం_నిడివి = ghst_చట్రం.శీర్షిక.నిడివి + పరిమాణం(ghst_చట్రం_శీర్షిక_t);

	ఒకవేళ ((current_చట్రం_నిడివి > పరిమాణం(ghst_చట్రం_t)) || (current_చట్రం_నిడివి < 4U)) {
		// frame too long or bogus (frame length should be longer than 4, at least 1 address, 1 length, 1 type, 1 data, 1 crc)
		// discard everything and go into unsynced state
		current_చట్రం_position = 0U;
		parser_state = ghst_parser_state_t::unsynced;
		GHST_DEBUG("Frame too long/bogus (%u, type=%u) -> unsync", current_చట్రం_నిడివి, ghst_చట్రం.type);
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (current_చట్రం_position < current_చట్రం_నిడివి) {
		// we do not have the full frame yet -> wait for more data
		GHST_VERBOSE("waiting for more data (%u < %u)", current_చట్రం_position, current_చట్రం_నిడివి);
		తిరిగి అబద్ధం;
	}

	తార్కిక ret = అబద్ధం;

	// now we have the full frame

	ఒకవేళ ((ghst_చట్రం.type >= static_cast<uint8_t>(ghst_చట్రం_Type::చట్రం_Type_మొదటి)) &&
	    (ghst_చట్రం.type <= static_cast<uint8_t>(ghst_చట్రం_Type::చట్రం_Type_చివరి)) &&
	    (ghst_చట్రం.శీర్షిక.నిడివి == GHST_TYPE_దత్తాంశం_చక్రీయ_తనిఖీ_కొలత) &&
	    (ghst_చట్రం.శీర్షిక.device_చిరునామా == GHST_ADDR_FC)) {
		స్థిరరాశి uint8_t చక్రీయ_తనిఖీ = ghst_చట్రం.payload[ghst_చట్రం.శీర్షిక.నిడివి - 2U];

		ఒకవేళ (చక్రీయ_తనిఖీ == ghst_చట్రం_చక్రీయ_తనిఖీ(ghst_చట్రం)) {
			స్థిరరాశి ghst_Payload_దత్తాంశం_t *స్థిరరాశి rcChannels = (ghst_Payload_దత్తాంశం_t *)&ghst_చట్రం.payload;
			*అంకె_విలువలు = MIN(max_channels, GHST_MAX_అంకె_CHANNELS);

			// all frames contain data from chan1to4
			ఒకవేళ (max_channels > 0U) { విలువలు_2[0] = convert_channel_విలువ(rcChannels->chan1to4.chan1 >> 1U); }

			ఒకవేళ (max_channels > 1U) { విలువలు_2[1] = convert_channel_విలువ(rcChannels->chan1to4.chan2 >> 1U); }

			ఒకవేళ (max_channels > 2U) { విలువలు_2[2] = convert_channel_విలువ(rcChannels->chan1to4.chan3 >> 1U); }

			ఒకవేళ (max_channels > 3U) { విలువలు_2[3] = convert_channel_విలువ(rcChannels->chan1to4.chan4 >> 1U); }

			ఒకవేళ (ghst_చట్రం.type == static_cast<uint8_t>(ghst_చట్రం_Type::చట్రం_Type5to8)) {
				ఒకవేళ (max_channels > 4U) { విలువలు_2[4] = convert_channel_విలువ(rcChannels->chanA << 3U); }

				ఒకవేళ (max_channels > 5U) { విలువలు_2[5] = convert_channel_విలువ(rcChannels->chanB << 3U); }

				ఒకవేళ (max_channels > 6U) { విలువలు_2[6] = convert_channel_విలువ(rcChannels->chanC << 3U); }

				ఒకవేళ (max_channels > 7U) { విలువలు_2[7] = convert_channel_విలువ(rcChannels->chanD << 3U); }

			} లేకపోతే ఒకవేళ (ghst_చట్రం.type == static_cast<uint8_t>(ghst_చట్రం_Type::చట్రం_Type9to12)) {
				ఒకవేళ (max_channels > 8U) { విలువలు_2[8] = convert_channel_విలువ(rcChannels->chanA << 3U); }

				ఒకవేళ (max_channels > 9U) { విలువలు_2[9] = convert_channel_విలువ(rcChannels->chanB << 3U); }

				ఒకవేళ (max_channels > 10U) { విలువలు_2[10] = convert_channel_విలువ(rcChannels->chanC << 3U); }

				ఒకవేళ (max_channels > 11U) { విలువలు_2[11] = convert_channel_విలువ(rcChannels->chanD << 3U); }

			} లేకపోతే ఒకవేళ (ghst_చట్రం.type == static_cast<uint8_t>(ghst_చట్రం_Type::చట్రం_Type13to16)) {
				ఒకవేళ (max_channels > 12U) { విలువలు_2[12] = convert_channel_విలువ(rcChannels->chanA << 3U); }

				ఒకవేళ (max_channels > 13U) { విలువలు_2[13] = convert_channel_విలువ(rcChannels->chanB << 3U); }

				ఒకవేళ (max_channels > 14U) { విలువలు_2[14] = convert_channel_విలువ(rcChannels->chanC << 3U); }

				ఒకవేళ (max_channels > 15U) { విలువలు_2[15] = convert_channel_విలువ(rcChannels->chanD << 3U); }

			} లేకపోతే ఒకవేళ (ghst_చట్రం.type == static_cast<uint8_t>(ghst_చట్రం_Type::చట్రం_Type_Rssi)) {
				స్థిరరాశి ghstPayloadRssi_t *స్థిరరాశి rssi_విలువలు = (ghstPayloadRssi_t *)&ghst_చట్రం.payload;
				// TODO: call function for RSSI dBm to percentage conversion for ghost receiver
				చివరి_link_stats.rssi_pct = spek_dbm_గమ్యం_percent(static_cast<int8_t>
							   (rssi_విలువలు->rssidBm));	// rssidBm sign inverted (90 = -90dBm)
				చివరి_link_stats.rssi_dbm = -rssi_విలువలు->rssidBm;
				చివరి_link_stats.link_quality = rssi_విలువలు->lq;						// 0 - 100

			} లేకపోతే {
				GHST_DEBUG("Frame type: %u", ghst_చట్రం.type);
			}

			*link_stats = చివరి_link_stats;

			జ్ఞాపకం_నకలు(prev_rc_vals, విలువలు_2, పరిమాణం(uint16_t) * GHST_MAX_అంకె_CHANNELS);

			GHST_VERBOSE("Got Channels");

			ret = నిజం;

		} లేకపోతే {
			GHST_DEBUG("CRC check failed");
		}

	} లేకపోతే {
		GHST_DEBUG("Got Non-RC frame (len=%u, type=%u)", current_చట్రం_నిడివి, ghst_చట్రం.type);
	}

	// either reset or move the rest of the buffer
	ఒకవేళ (current_చట్రం_position > current_చట్రం_నిడివి) {
		GHST_VERBOSE("Moving buffer (%u > %u)", current_చట్రం_position, current_చట్రం_నిడివి);
		memmove(ghst_చట్రం_ptr, ghst_చట్రం_ptr + current_చట్రం_నిడివి, current_చట్రం_position - current_చట్రం_నిడివి);
		current_చట్రం_position -= current_చట్రం_నిడివి;

	} లేకపోతే {
		current_చట్రం_position = 0U;
	}

	తిరిగి ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
స్థిర అంతఃవరుస శూన్య write_uint8_t(uint8_t *buf, పూర్ణాంకం &స్థానభ్రంశం, uint8_t value)
{
	buf[స్థానభ్రంశం++] = value;
}

/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
స్థిర అంతఃవరుస శూన్య write_uint16_t(uint8_t *buf, పూర్ణాంకం &స్థానభ్రంశం, uint16_t value)
{
	buf[స్థానభ్రంశం] = value & 0xFFU;
	buf[స్థానభ్రంశం + 1] = value >> 8U;
	స్థానభ్రంశం += 2;
}

/**
 * write an uint32_t value to a buffer at a given offset and increment the offset
 */
స్థిర అంతఃవరుస శూన్య write_uint32_t(uint8_t *buf, పూర్ణాంకం &స్థానభ్రంశం, uint32_t value)
{
	// Little Endian
	buf[స్థానభ్రంశం] = value & 0xFFU;
	buf[స్థానభ్రంశం + 1] = (value & 0xFF00) >> 8U;
	buf[స్థానభ్రంశం + 2] = (value & 0xFF0000) >> 16U;
	buf[స్థానభ్రంశం + 3] = (value & 0xFF000000) >> 24U;
	స్థానభ్రంశం += 4;
}

/**
 * write frame header
 */
స్థిర అంతఃవరుస శూన్య write_చట్రం_శీర్షిక(uint8_t *buf, పూర్ణాంకం &స్థానభ్రంశం, ghstTelemetryType type, uint8_t payload_కొలత)
{
	write_uint8_t(buf, స్థానభ్రంశం, static_cast<uint8_t>(ghst_చిరునామా::rx_చిరునామా));
	write_uint8_t(buf, స్థానభ్రంశం, payload_కొలత + GHST_చట్రం_చక్రీయ_తనిఖీ_కొలత + GHST_చట్రం_TYPE_కొలత);
	write_uint8_t(buf, స్థానభ్రంశం, static_cast<uint8_t>(type));
}

/**
 * write frame CRC
 */
స్థిర అంతఃవరుస శూన్య write_చట్రం_చక్రీయ_తనిఖీ(uint8_t *buf, పూర్ణాంకం &స్థానభ్రంశం, పూర్ణాంకం buf_కొలత)
{
	write_uint8_t(buf, స్థానభ్రంశం, crc8_dvb_s2_buf(buf + 2U, buf_కొలత - 3));
}

తార్కిక ghst_send_telemetry_battery_స్థితి(పూర్ణాంకం uart_fd, uint16_t voltage_లోపల_10m_V,
					uint16_t current_లోపల_10m_A, uint16_t fuel_లోపల_10m_Ah)
{
	uint8_t buf[GHST_చట్రం_PAYLOAD_కొలత_TELEMETRY + 4U]; // address, frame length, type, crc
	పూర్ణాంకం స్థానభ్రంశం = 0;
	write_చట్రం_శీర్షిక(buf, స్థానభ్రంశం, ghstTelemetryType::batteryPack, GHST_చట్రం_PAYLOAD_కొలత_TELEMETRY);
	write_uint16_t(buf, స్థానభ్రంశం, voltage_లోపల_10m_V);
	write_uint16_t(buf, స్థానభ్రంశం, current_లోపల_10m_A);
	write_uint16_t(buf, స్థానభ్రంశం, fuel_లోపల_10m_Ah);
	write_uint8_t(buf, స్థానభ్రంశం, 0x00U); //ఖాళీలు
	write_uint8_t(buf, స్థానభ్రంశం, 0x00U); //ఖాళీలు
	write_uint8_t(buf, స్థానభ్రంశం, 0x00U); //ఖాళీలు
	write_uint8_t(buf, స్థానభ్రంశం, 0x00U); //ఖాళీలు
	write_చట్రం_చక్రీయ_తనిఖీ(buf, స్థానభ్రంశం, పరిమాణం(buf));

	తిరిగి లిఖ(uart_fd, buf, స్థానభ్రంశం) == స్థానభ్రంశం;
}

తార్కిక ghst_send_telemetry_gps1_స్థితి(పూర్ణాంకం uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude)
{
	uint8_t buf[GHST_చట్రం_PAYLOAD_కొలత_TELEMETRY + 4U]; // address, frame length, type, crc
	పూర్ణాంకం స్థానభ్రంశం = 0;
	write_చట్రం_శీర్షిక(buf, స్థానభ్రంశం, ghstTelemetryType::gpsPrimary, GHST_చట్రం_PAYLOAD_కొలత_TELEMETRY);
	write_uint32_t(buf, స్థానభ్రంశం, latitude);
	write_uint32_t(buf, స్థానభ్రంశం, longitude);
	write_uint16_t(buf, స్థానభ్రంశం, altitude);
	write_చట్రం_చక్రీయ_తనిఖీ(buf, స్థానభ్రంశం, పరిమాణం(buf));

	తిరిగి లిఖ(uart_fd, buf, స్థానభ్రంశం) == స్థానభ్రంశం;
}

తార్కిక ghst_send_telemetry_gps2_స్థితి(పూర్ణాంకం uart_fd, uint16_t ground_వేగం, uint16_t ground_కోర్సు, uint8_t అంకె_Sats,
				     uint16_t home_dist, uint16_t home_dir, uint8_t flags)
{
	uint8_t buf[GHST_చట్రం_PAYLOAD_కొలత_TELEMETRY + 4U]; // address, frame length, type, crc
	పూర్ణాంకం స్థానభ్రంశం = 0;
	write_చట్రం_శీర్షిక(buf, స్థానభ్రంశం, ghstTelemetryType::gpsSecondary, GHST_చట్రం_PAYLOAD_కొలత_TELEMETRY);
	write_uint16_t(buf, స్థానభ్రంశం, ground_వేగం);
	write_uint16_t(buf, స్థానభ్రంశం, ground_కోర్సు);
	write_uint8_t(buf, స్థానభ్రంశం, అంకె_Sats);
	write_uint16_t(buf, స్థానభ్రంశం, home_dist);
	write_uint16_t(buf, స్థానభ్రంశం, home_dir);
	write_uint8_t(buf, స్థానభ్రంశం, flags);
	write_చట్రం_చక్రీయ_తనిఖీ(buf, స్థానభ్రంశం, పరిమాణం(buf));

	తిరిగి లిఖ(uart_fd, buf, స్థానభ్రంశం) == స్థానభ్రంశం;
}
