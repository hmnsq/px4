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

#إذا 0 // enable non-verbose debugging
#تعریف_کړه GHST_DEBUG PX4_WARN
#وإلا
#تعریف_کړه GHST_DEBUG(...)
#د_که_پای

#إذا 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#تعریف_کړه GHST_VERBOSE PX4_WARN
#وإلا
#تعریف_کړه GHST_VERBOSE(...)
#د_که_پای

#ضمن <drivers/drv_hrt.h>
#ضمن <termios.h>
#ضمن <string.h>
#ضمن <unistd.h>
#ضمن <math.h>

// TODO: include RSSI dBm to percentage conversion for ghost receiver
#ضمن "spektrum_rssi.h"

#ضمن "ghst.hpp"
#ضمن "common_rc.h"

#تعریف_کړه MIN(a,b) (((a)<(b))?(a):(b))

#تعریف_کړه GHST_چوکاټ_PAYLOAD_اندازه_TELEMETRY	(10U)
#تعریف_کړه GHST_چوکاټ_دوراني_ازموینه_اندازه			(1U)
#تعریف_کړه GHST_چوکاټ_TYPE_اندازه			(1U)
#تعریف_کړه GHST_TYPE_معلومات_دوراني_ازموینه_اندازه			(12U)
#تعریف_کړه GHST_ADDR_FC				(130U)
#تعریف_کړه GHST_MAX_عدد_CHANNELS			(16)

تعداد صنف ghst_parser_state_t : uint8_t {
	unsynced = 0U,
	synced
};

// only RSSI frame contains value of RSSI, if it is not received, send last received RSSI/LQ
ساكن ghstLinkStatistics_t وروستی_link_stats = { .rssi_pct = -1, .rssi_dbm = NAN, .link_quality = 0 };

ساكن ghst_چوکاټ_t &ghst_چوکاټ = rc_decode_buf.ghst_چوکاټ;
ساكن uint32_t current_چوکاټ_position = 0U;
ساكن ghst_parser_state_t parser_state = ghst_parser_state_t::unsynced;

ساكن uint16_t prev_rc_vals[GHST_MAX_عدد_CHANNELS];

/**
 * parse the current ghst_frame buffer
 */
ساكن منطقي ghst_parse_بفر(uint16_t *ارزښتونه_2, ghstLinkStatistics_t *link_stats, uint16_t *عدد_ارزښتونه,
			      uint16_t max_channels);

صحيح ghst_config(صحيح uart_fd)
{
	بنية termios t;
	صحيح ret_val;

	// no parity, one stop bit
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, GHST_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	memset(prev_rc_vals, static_cast<صحيح>(UINT16_MAX), حجم(uint16_t) * GHST_MAX_عدد_CHANNELS);
	ret_val = tcsetattr(uart_fd, TCSANOW, &t);
	أرجع ret_val;
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
ساكن uint16_t convert_channel_ارزښت(بینښه chan_ارزښت);


منطقي ghst_parse(ثابت uint64_t now, ثابت uint8_t *چوکاټ, بینښه len, uint16_t *ارزښتونه_2,
		ghstLinkStatistics_t *link_stats, uint16_t *عدد_ارزښتونه, uint16_t max_channels)
{
	منطقي success = خطأ;
	uint8_t *ghst_چوکاټ_ptr = (uint8_t *)&ghst_چوکاټ;

	حافظه_نقل(ارزښتونه_2, prev_rc_vals, حجم(uint16_t) * GHST_MAX_عدد_CHANNELS);

	طالما (len > 0U) {

		// fill in the ghst_buffer, as much as we can
		ثابت uint32_t current_len = MIN(len, حجم(ghst_چوکاټ_t) - current_چوکاټ_position);
		حافظه_نقل(ghst_چوکاټ_ptr + current_چوکاټ_position, چوکاټ, current_len);
		current_چوکاټ_position += current_len;

		// protection to guarantee parsing progress
		إذا (current_len == 0U) {
			GHST_DEBUG("========== parser bug: no progress (%u) ===========", len);

			لكل (uint32_t i = 0U; i < current_چوکاټ_position; ++i) {
				GHST_DEBUG("ghst_frame_ptr[%u]: 0x%x", i, ghst_چوکاټ_ptr[i]);
			}

			// reset the parser
			current_چوکاټ_position = 0U;
			parser_state = ghst_parser_state_t::unsynced;
			success = خطأ;

		} وإلا {
			len -= current_len;
			چوکاټ += current_len;

			إذا (ghst_parse_بفر(ارزښتونه_2, link_stats, عدد_ارزښتونه, max_channels)) {
				success = ریښتیا;
			}
		}
	}

	أرجع success;
}

uint8_t ghst_چوکاټ_دوراني_ازموینه(ثابت ghst_چوکاټ_t &چوکاټ)
{
	uint8_t دوراني_ازموینه = crc8_dvb_s2(0U, چوکاټ.type);

	لكل (uint32_t i = 0U; i < چوکاټ.سرلیک.اوږدتیا - GHST_چوکاټ_دوراني_ازموینه_اندازه - GHST_چوکاټ_TYPE_اندازه; ++i) {
		دوراني_ازموینه = crc8_dvb_s2(دوراني_ازموینه, چوکاټ.payload[i]);
	}

	أرجع دوراني_ازموینه;
}

ساكن uint16_t convert_channel_ارزښت(بینښه صحيح chan_ارزښت)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	ساكن constexpr عائم scale = (2012.0F - 988.0F) / (1811.0F - 172.0F);
	ساكن constexpr عائم دننه_واټن = 988.0F - (172.0F * scale);
	عائم scaled_chan_ارزښت = scale * static_cast<عائم>(chan_ارزښت);
	عائم scaled_chan_ارزښت_with_دننه_واټن = scaled_chan_ارزښت + دننه_واټن;
	uint16_t converted_chan_ارزښت = static_cast<uint16_t>(scaled_chan_ارزښت_with_دننه_واټن);

	أرجع converted_chan_ارزښت;
}

ساكن منطقي ghst_parse_بفر(uint16_t *ارزښتونه_2, ghstLinkStatistics_t *link_stats, uint16_t *عدد_ارزښتونه,
			      uint16_t max_channels)
{
	uint8_t *ghst_چوکاټ_ptr = (uint8_t *)&ghst_چوکاټ;

	إذا (parser_state == ghst_parser_state_t::unsynced) {
		// there is no sync yet, try to find an RC packet by searching for a matching frame length and type
		لكل (uint32_t i = 1U; i < current_چوکاټ_position - 1U; ++i) {
			إذا ((ghst_چوکاټ_ptr[i + 1U] >= static_cast<uint8_t>(ghst_چوکاټ_Type::چوکاټ_Type_لومړی)) &&
			    (ghst_چوکاټ_ptr[i + 1U] <= static_cast<uint8_t>(ghst_چوکاټ_Type::چوکاټ_Type_وروستی))) {
				إذا (ghst_چوکاټ_ptr[i] == GHST_TYPE_معلومات_دوراني_ازموینه_اندازه) {
					parser_state = ghst_parser_state_t::synced;
					uint32_t چوکاټ_دننه_واټن = i - 1U;
					GHST_VERBOSE("RC channels found at offset %u", چوکاټ_دننه_واټن);

					// move the rest of the buffer to the beginning
					إذا (چوکاټ_دننه_واټن != 0U) {
						memmove(ghst_چوکاټ_ptr, ghst_چوکاټ_ptr + چوکاټ_دننه_واټن, current_چوکاټ_position - چوکاټ_دننه_واټن);
						current_چوکاټ_position -= چوکاټ_دننه_واټن;
					}

					اكسر;
				}
			}
		}
	}

	إذا (parser_state != ghst_parser_state_t::synced) {
		إذا (current_چوکاټ_position >= حجم(ghst_چوکاټ_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_چوکاټ_position = 3U;

			حافظه_نقل(ghst_چوکاټ_ptr, ghst_چوکاټ_ptr + حجم(ghst_چوکاټ_t) - current_چوکاټ_position, current_چوکاټ_position);

			GHST_VERBOSE("Discarding buffer");
		}

		أرجع خطأ;
	}


	إذا (current_چوکاټ_position < 3U) {
		// wait until we have the address, length and type
		أرجع خطأ;
	}

	// now we have at least the header and the type

	ثابت بینښه current_چوکاټ_اوږدتیا = ghst_چوکاټ.سرلیک.اوږدتیا + حجم(ghst_چوکاټ_سرلیک_t);

	إذا ((current_چوکاټ_اوږدتیا > حجم(ghst_چوکاټ_t)) || (current_چوکاټ_اوږدتیا < 4U)) {
		// frame too long or bogus (frame length should be longer than 4, at least 1 address, 1 length, 1 type, 1 data, 1 crc)
		// discard everything and go into unsynced state
		current_چوکاټ_position = 0U;
		parser_state = ghst_parser_state_t::unsynced;
		GHST_DEBUG("Frame too long/bogus (%u, type=%u) -> unsync", current_چوکاټ_اوږدتیا, ghst_چوکاټ.type);
		أرجع خطأ;
	}

	إذا (current_چوکاټ_position < current_چوکاټ_اوږدتیا) {
		// we do not have the full frame yet -> wait for more data
		GHST_VERBOSE("waiting for more data (%u < %u)", current_چوکاټ_position, current_چوکاټ_اوږدتیا);
		أرجع خطأ;
	}

	منطقي ret = خطأ;

	// now we have the full frame

	إذا ((ghst_چوکاټ.type >= static_cast<uint8_t>(ghst_چوکاټ_Type::چوکاټ_Type_لومړی)) &&
	    (ghst_چوکاټ.type <= static_cast<uint8_t>(ghst_چوکاټ_Type::چوکاټ_Type_وروستی)) &&
	    (ghst_چوکاټ.سرلیک.اوږدتیا == GHST_TYPE_معلومات_دوراني_ازموینه_اندازه) &&
	    (ghst_چوکاټ.سرلیک.device_پته == GHST_ADDR_FC)) {
		ثابت uint8_t دوراني_ازموینه = ghst_چوکاټ.payload[ghst_چوکاټ.سرلیک.اوږدتیا - 2U];

		إذا (دوراني_ازموینه == ghst_چوکاټ_دوراني_ازموینه(ghst_چوکاټ)) {
			ثابت ghst_Payload_معلومات_t *ثابت rcChannels = (ghst_Payload_معلومات_t *)&ghst_چوکاټ.payload;
			*عدد_ارزښتونه = MIN(max_channels, GHST_MAX_عدد_CHANNELS);

			// all frames contain data from chan1to4
			إذا (max_channels > 0U) { ارزښتونه_2[0] = convert_channel_ارزښت(rcChannels->chan1to4.chan1 >> 1U); }

			إذا (max_channels > 1U) { ارزښتونه_2[1] = convert_channel_ارزښت(rcChannels->chan1to4.chan2 >> 1U); }

			إذا (max_channels > 2U) { ارزښتونه_2[2] = convert_channel_ارزښت(rcChannels->chan1to4.chan3 >> 1U); }

			إذا (max_channels > 3U) { ارزښتونه_2[3] = convert_channel_ارزښت(rcChannels->chan1to4.chan4 >> 1U); }

			إذا (ghst_چوکاټ.type == static_cast<uint8_t>(ghst_چوکاټ_Type::چوکاټ_Type5to8)) {
				إذا (max_channels > 4U) { ارزښتونه_2[4] = convert_channel_ارزښت(rcChannels->chanA << 3U); }

				إذا (max_channels > 5U) { ارزښتونه_2[5] = convert_channel_ارزښت(rcChannels->chanB << 3U); }

				إذا (max_channels > 6U) { ارزښتونه_2[6] = convert_channel_ارزښت(rcChannels->chanC << 3U); }

				إذا (max_channels > 7U) { ارزښتونه_2[7] = convert_channel_ارزښت(rcChannels->chanD << 3U); }

			} وإلا إذا (ghst_چوکاټ.type == static_cast<uint8_t>(ghst_چوکاټ_Type::چوکاټ_Type9to12)) {
				إذا (max_channels > 8U) { ارزښتونه_2[8] = convert_channel_ارزښت(rcChannels->chanA << 3U); }

				إذا (max_channels > 9U) { ارزښتونه_2[9] = convert_channel_ارزښت(rcChannels->chanB << 3U); }

				إذا (max_channels > 10U) { ارزښتونه_2[10] = convert_channel_ارزښت(rcChannels->chanC << 3U); }

				إذا (max_channels > 11U) { ارزښتونه_2[11] = convert_channel_ارزښت(rcChannels->chanD << 3U); }

			} وإلا إذا (ghst_چوکاټ.type == static_cast<uint8_t>(ghst_چوکاټ_Type::چوکاټ_Type13to16)) {
				إذا (max_channels > 12U) { ارزښتونه_2[12] = convert_channel_ارزښت(rcChannels->chanA << 3U); }

				إذا (max_channels > 13U) { ارزښتونه_2[13] = convert_channel_ارزښت(rcChannels->chanB << 3U); }

				إذا (max_channels > 14U) { ارزښتونه_2[14] = convert_channel_ارزښت(rcChannels->chanC << 3U); }

				إذا (max_channels > 15U) { ارزښتونه_2[15] = convert_channel_ارزښت(rcChannels->chanD << 3U); }

			} وإلا إذا (ghst_چوکاټ.type == static_cast<uint8_t>(ghst_چوکاټ_Type::چوکاټ_Type_Rssi)) {
				ثابت ghstPayloadRssi_t *ثابت rssi_ارزښتونه = (ghstPayloadRssi_t *)&ghst_چوکاټ.payload;
				// TODO: call function for RSSI dBm to percentage conversion for ghost receiver
				وروستی_link_stats.rssi_pct = spek_dbm_منزل_percent(static_cast<int8_t>
							   (rssi_ارزښتونه->rssidBm));	// rssidBm sign inverted (90 = -90dBm)
				وروستی_link_stats.rssi_dbm = -rssi_ارزښتونه->rssidBm;
				وروستی_link_stats.link_quality = rssi_ارزښتونه->lq;						// 0 - 100

			} وإلا {
				GHST_DEBUG("Frame type: %u", ghst_چوکاټ.type);
			}

			*link_stats = وروستی_link_stats;

			حافظه_نقل(prev_rc_vals, ارزښتونه_2, حجم(uint16_t) * GHST_MAX_عدد_CHANNELS);

			GHST_VERBOSE("Got Channels");

			ret = ریښتیا;

		} وإلا {
			GHST_DEBUG("CRC check failed");
		}

	} وإلا {
		GHST_DEBUG("Got Non-RC frame (len=%u, type=%u)", current_چوکاټ_اوږدتیا, ghst_چوکاټ.type);
	}

	// either reset or move the rest of the buffer
	إذا (current_چوکاټ_position > current_چوکاټ_اوږدتیا) {
		GHST_VERBOSE("Moving buffer (%u > %u)", current_چوکاټ_position, current_چوکاټ_اوږدتیا);
		memmove(ghst_چوکاټ_ptr, ghst_چوکاټ_ptr + current_چوکاټ_اوږدتیا, current_چوکاټ_position - current_چوکاټ_اوږدتیا);
		current_چوکاټ_position -= current_چوکاټ_اوږدتیا;

	} وإلا {
		current_چوکاټ_position = 0U;
	}

	أرجع ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint8_t(uint8_t *buf, صحيح &دننه_واټن, uint8_t value)
{
	buf[دننه_واټن++] = value;
}

/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint16_t(uint8_t *buf, صحيح &دننه_واټن, uint16_t value)
{
	buf[دننه_واټن] = value & 0xFFU;
	buf[دننه_واټن + 1] = value >> 8U;
	دننه_واټن += 2;
}

/**
 * write an uint32_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint32_t(uint8_t *buf, صحيح &دننه_واټن, uint32_t value)
{
	// Little Endian
	buf[دننه_واټن] = value & 0xFFU;
	buf[دننه_واټن + 1] = (value & 0xFF00) >> 8U;
	buf[دننه_واټن + 2] = (value & 0xFF0000) >> 16U;
	buf[دننه_واټن + 3] = (value & 0xFF000000) >> 24U;
	دننه_واټن += 4;
}

/**
 * write frame header
 */
ساكن مضمن فراغ write_چوکاټ_سرلیک(uint8_t *buf, صحيح &دننه_واټن, ghstTelemetryType type, uint8_t payload_اندازه)
{
	write_uint8_t(buf, دننه_واټن, static_cast<uint8_t>(ghst_پته::rx_پته));
	write_uint8_t(buf, دننه_واټن, payload_اندازه + GHST_چوکاټ_دوراني_ازموینه_اندازه + GHST_چوکاټ_TYPE_اندازه);
	write_uint8_t(buf, دننه_واټن, static_cast<uint8_t>(type));
}

/**
 * write frame CRC
 */
ساكن مضمن فراغ write_چوکاټ_دوراني_ازموینه(uint8_t *buf, صحيح &دننه_واټن, صحيح buf_اندازه)
{
	write_uint8_t(buf, دننه_واټن, crc8_dvb_s2_buf(buf + 2U, buf_اندازه - 3));
}

منطقي ghst_send_telemetry_battery_حالت(صحيح uart_fd, uint16_t voltage_دننه_10m_V,
					uint16_t current_دننه_10m_A, uint16_t fuel_دننه_10m_Ah)
{
	uint8_t buf[GHST_چوکاټ_PAYLOAD_اندازه_TELEMETRY + 4U]; // address, frame length, type, crc
	صحيح دننه_واټن = 0;
	write_چوکاټ_سرلیک(buf, دننه_واټن, ghstTelemetryType::batteryPack, GHST_چوکاټ_PAYLOAD_اندازه_TELEMETRY);
	write_uint16_t(buf, دننه_واټن, voltage_دننه_10m_V);
	write_uint16_t(buf, دننه_واټن, current_دننه_10m_A);
	write_uint16_t(buf, دننه_واټن, fuel_دننه_10m_Ah);
	write_uint8_t(buf, دننه_واټن, 0x00U); //تشه
	write_uint8_t(buf, دننه_واټن, 0x00U); //تشه
	write_uint8_t(buf, دننه_واټن, 0x00U); //تشه
	write_uint8_t(buf, دننه_واټن, 0x00U); //تشه
	write_چوکاټ_دوراني_ازموینه(buf, دننه_واټن, حجم(buf));

	أرجع اكتب(uart_fd, buf, دننه_واټن) == دننه_واټن;
}

منطقي ghst_send_telemetry_gps1_حالت(صحيح uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude)
{
	uint8_t buf[GHST_چوکاټ_PAYLOAD_اندازه_TELEMETRY + 4U]; // address, frame length, type, crc
	صحيح دننه_واټن = 0;
	write_چوکاټ_سرلیک(buf, دننه_واټن, ghstTelemetryType::gpsPrimary, GHST_چوکاټ_PAYLOAD_اندازه_TELEMETRY);
	write_uint32_t(buf, دننه_واټن, latitude);
	write_uint32_t(buf, دننه_واټن, longitude);
	write_uint16_t(buf, دننه_واټن, altitude);
	write_چوکاټ_دوراني_ازموینه(buf, دننه_واټن, حجم(buf));

	أرجع اكتب(uart_fd, buf, دننه_واټن) == دننه_واټن;
}

منطقي ghst_send_telemetry_gps2_حالت(صحيح uart_fd, uint16_t ground_چټکتیا, uint16_t ground_کورس, uint8_t عدد_Sats,
				     uint16_t home_dist, uint16_t home_dir, uint8_t flags)
{
	uint8_t buf[GHST_چوکاټ_PAYLOAD_اندازه_TELEMETRY + 4U]; // address, frame length, type, crc
	صحيح دننه_واټن = 0;
	write_چوکاټ_سرلیک(buf, دننه_واټن, ghstTelemetryType::gpsSecondary, GHST_چوکاټ_PAYLOAD_اندازه_TELEMETRY);
	write_uint16_t(buf, دننه_واټن, ground_چټکتیا);
	write_uint16_t(buf, دننه_واټن, ground_کورس);
	write_uint8_t(buf, دننه_واټن, عدد_Sats);
	write_uint16_t(buf, دننه_واټن, home_dist);
	write_uint16_t(buf, دننه_واټن, home_dir);
	write_uint8_t(buf, دننه_واټن, flags);
	write_چوکاټ_دوراني_ازموینه(buf, دننه_واټن, حجم(buf));

	أرجع اكتب(uart_fd, buf, دننه_واټن) == دننه_واټن;
}
