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
#وصف_ڪريو GHST_DEBUG PX4_WARN
#وإلا
#وصف_ڪريو GHST_DEBUG(...)
#جيڪڏهن_پورو

#إذا 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#وصف_ڪريو GHST_VERBOSE PX4_WARN
#وإلا
#وصف_ڪريو GHST_VERBOSE(...)
#جيڪڏهن_پورو

#ضمن <drivers/drv_hrt.h>
#ضمن <termios.h>
#ضمن <string.h>
#ضمن <unistd.h>
#ضمن <math.h>

// TODO: include RSSI dBm to percentage conversion for ghost receiver
#ضمن "spektrum_rssi.h"

#ضمن "ghst.hpp"
#ضمن "common_rc.h"

#وصف_ڪريو MIN(a,b) (((a)<(b))?(a):(b))

#وصف_ڪريو GHST_چوکٺو_PAYLOAD_سائيز_TELEMETRY	(10U)
#وصف_ڪريو GHST_چوکٺو_چڪري_جاچ_سائيز			(1U)
#وصف_ڪريو GHST_چوکٺو_TYPE_سائيز			(1U)
#وصف_ڪريو GHST_TYPE_معلومات_چڪري_جاچ_سائيز			(12U)
#وصف_ڪريو GHST_ADDR_FC				(130U)
#وصف_ڪريو GHST_MAX_انگ_CHANNELS			(16)

تعداد صنف ghst_parser_state_t : uint8_t {
	unsynced = 0U,
	synced
};

// only RSSI frame contains value of RSSI, if it is not received, send last received RSSI/LQ
ساكن ghstLinkStatistics_t آخري_link_stats = { .rssi_pct = -1, .rssi_dbm = NAN, .link_quality = 0 };

ساكن ghst_چوکٺو_t &ghst_چوکٺو = rc_decode_buf.ghst_چوکٺو;
ساكن uint32_t current_چوکٺو_position = 0U;
ساكن ghst_parser_state_t parser_state = ghst_parser_state_t::unsynced;

ساكن uint16_t prev_rc_vals[GHST_MAX_انگ_CHANNELS];

/**
 * parse the current ghst_frame buffer
 */
ساكن منطقي ghst_parse_بفر(uint16_t *قدرون_2, ghstLinkStatistics_t *link_stats, uint16_t *انگ_قدرون,
			      uint16_t max_channels);

صحيح ghst_config(صحيح uart_fd)
{
	بنية termios t;
	صحيح ret_val;

	// no parity, one stop bit
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, GHST_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	memset(prev_rc_vals, static_cast<صحيح>(UINT16_MAX), حجم(uint16_t) * GHST_MAX_انگ_CHANNELS);
	ret_val = tcsetattr(uart_fd, TCSANOW, &t);
	أرجع ret_val;
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
ساكن uint16_t convert_channel_قدر(بنانشان chan_قدر);


منطقي ghst_parse(ثابت uint64_t now, ثابت uint8_t *چوکٺو, بنانشان len, uint16_t *قدرون_2,
		ghstLinkStatistics_t *link_stats, uint16_t *انگ_قدرون, uint16_t max_channels)
{
	منطقي success = خطأ;
	uint8_t *ghst_چوکٺو_ptr = (uint8_t *)&ghst_چوکٺو;

	يادگيري_نقل(قدرون_2, prev_rc_vals, حجم(uint16_t) * GHST_MAX_انگ_CHANNELS);

	طالما (len > 0U) {

		// fill in the ghst_buffer, as much as we can
		ثابت uint32_t current_len = MIN(len, حجم(ghst_چوکٺو_t) - current_چوکٺو_position);
		يادگيري_نقل(ghst_چوکٺو_ptr + current_چوکٺو_position, چوکٺو, current_len);
		current_چوکٺو_position += current_len;

		// protection to guarantee parsing progress
		إذا (current_len == 0U) {
			GHST_DEBUG("========== parser bug: no progress (%u) ===========", len);

			لكل (uint32_t i = 0U; i < current_چوکٺو_position; ++i) {
				GHST_DEBUG("ghst_frame_ptr[%u]: 0x%x", i, ghst_چوکٺو_ptr[i]);
			}

			// reset the parser
			current_چوکٺو_position = 0U;
			parser_state = ghst_parser_state_t::unsynced;
			success = خطأ;

		} وإلا {
			len -= current_len;
			چوکٺو += current_len;

			إذا (ghst_parse_بفر(قدرون_2, link_stats, انگ_قدرون, max_channels)) {
				success = سچ;
			}
		}
	}

	أرجع success;
}

uint8_t ghst_چوکٺو_چڪري_جاچ(ثابت ghst_چوکٺو_t &چوکٺو)
{
	uint8_t چڪري_جاچ = crc8_dvb_s2(0U, چوکٺو.type);

	لكل (uint32_t i = 0U; i < چوکٺو.سرو.ڊيگهائي - GHST_چوکٺو_چڪري_جاچ_سائيز - GHST_چوکٺو_TYPE_سائيز; ++i) {
		چڪري_جاچ = crc8_dvb_s2(چڪري_جاچ, چوکٺو.payload[i]);
	}

	أرجع چڪري_جاچ;
}

ساكن uint16_t convert_channel_قدر(بنانشان صحيح chan_قدر)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	ساكن constexpr عائم scale = (2012.0F - 988.0F) / (1811.0F - 172.0F);
	ساكن constexpr عائم وٿي = 988.0F - (172.0F * scale);
	عائم scaled_chan_قدر = scale * static_cast<عائم>(chan_قدر);
	عائم scaled_chan_قدر_with_وٿي = scaled_chan_قدر + وٿي;
	uint16_t converted_chan_قدر = static_cast<uint16_t>(scaled_chan_قدر_with_وٿي);

	أرجع converted_chan_قدر;
}

ساكن منطقي ghst_parse_بفر(uint16_t *قدرون_2, ghstLinkStatistics_t *link_stats, uint16_t *انگ_قدرون,
			      uint16_t max_channels)
{
	uint8_t *ghst_چوکٺو_ptr = (uint8_t *)&ghst_چوکٺو;

	إذا (parser_state == ghst_parser_state_t::unsynced) {
		// there is no sync yet, try to find an RC packet by searching for a matching frame length and type
		لكل (uint32_t i = 1U; i < current_چوکٺو_position - 1U; ++i) {
			إذا ((ghst_چوکٺو_ptr[i + 1U] >= static_cast<uint8_t>(ghst_چوکٺو_Type::چوکٺو_Type_پهريون)) &&
			    (ghst_چوکٺو_ptr[i + 1U] <= static_cast<uint8_t>(ghst_چوکٺو_Type::چوکٺو_Type_آخري))) {
				إذا (ghst_چوکٺو_ptr[i] == GHST_TYPE_معلومات_چڪري_جاچ_سائيز) {
					parser_state = ghst_parser_state_t::synced;
					uint32_t چوکٺو_وٿي = i - 1U;
					GHST_VERBOSE("RC channels found at offset %u", چوکٺو_وٿي);

					// move the rest of the buffer to the beginning
					إذا (چوکٺو_وٿي != 0U) {
						memmove(ghst_چوکٺو_ptr, ghst_چوکٺو_ptr + چوکٺو_وٿي, current_چوکٺو_position - چوکٺو_وٿي);
						current_چوکٺو_position -= چوکٺو_وٿي;
					}

					اكسر;
				}
			}
		}
	}

	إذا (parser_state != ghst_parser_state_t::synced) {
		إذا (current_چوکٺو_position >= حجم(ghst_چوکٺو_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_چوکٺو_position = 3U;

			يادگيري_نقل(ghst_چوکٺو_ptr, ghst_چوکٺو_ptr + حجم(ghst_چوکٺو_t) - current_چوکٺو_position, current_چوکٺو_position);

			GHST_VERBOSE("Discarding buffer");
		}

		أرجع خطأ;
	}


	إذا (current_چوکٺو_position < 3U) {
		// wait until we have the address, length and type
		أرجع خطأ;
	}

	// now we have at least the header and the type

	ثابت بنانشان current_چوکٺو_ڊيگهائي = ghst_چوکٺو.سرو.ڊيگهائي + حجم(ghst_چوکٺو_سرو_t);

	إذا ((current_چوکٺو_ڊيگهائي > حجم(ghst_چوکٺو_t)) || (current_چوکٺو_ڊيگهائي < 4U)) {
		// frame too long or bogus (frame length should be longer than 4, at least 1 address, 1 length, 1 type, 1 data, 1 crc)
		// discard everything and go into unsynced state
		current_چوکٺو_position = 0U;
		parser_state = ghst_parser_state_t::unsynced;
		GHST_DEBUG("Frame too long/bogus (%u, type=%u) -> unsync", current_چوکٺو_ڊيگهائي, ghst_چوکٺو.type);
		أرجع خطأ;
	}

	إذا (current_چوکٺو_position < current_چوکٺو_ڊيگهائي) {
		// we do not have the full frame yet -> wait for more data
		GHST_VERBOSE("waiting for more data (%u < %u)", current_چوکٺو_position, current_چوکٺو_ڊيگهائي);
		أرجع خطأ;
	}

	منطقي ret = خطأ;

	// now we have the full frame

	إذا ((ghst_چوکٺو.type >= static_cast<uint8_t>(ghst_چوکٺو_Type::چوکٺو_Type_پهريون)) &&
	    (ghst_چوکٺو.type <= static_cast<uint8_t>(ghst_چوکٺو_Type::چوکٺو_Type_آخري)) &&
	    (ghst_چوکٺو.سرو.ڊيگهائي == GHST_TYPE_معلومات_چڪري_جاچ_سائيز) &&
	    (ghst_چوکٺو.سرو.device_پتو == GHST_ADDR_FC)) {
		ثابت uint8_t چڪري_جاچ = ghst_چوکٺو.payload[ghst_چوکٺو.سرو.ڊيگهائي - 2U];

		إذا (چڪري_جاچ == ghst_چوکٺو_چڪري_جاچ(ghst_چوکٺو)) {
			ثابت ghst_Payload_معلومات_t *ثابت rcChannels = (ghst_Payload_معلومات_t *)&ghst_چوکٺو.payload;
			*انگ_قدرون = MIN(max_channels, GHST_MAX_انگ_CHANNELS);

			// all frames contain data from chan1to4
			إذا (max_channels > 0U) { قدرون_2[0] = convert_channel_قدر(rcChannels->chan1to4.chan1 >> 1U); }

			إذا (max_channels > 1U) { قدرون_2[1] = convert_channel_قدر(rcChannels->chan1to4.chan2 >> 1U); }

			إذا (max_channels > 2U) { قدرون_2[2] = convert_channel_قدر(rcChannels->chan1to4.chan3 >> 1U); }

			إذا (max_channels > 3U) { قدرون_2[3] = convert_channel_قدر(rcChannels->chan1to4.chan4 >> 1U); }

			إذا (ghst_چوکٺو.type == static_cast<uint8_t>(ghst_چوکٺو_Type::چوکٺو_Type5to8)) {
				إذا (max_channels > 4U) { قدرون_2[4] = convert_channel_قدر(rcChannels->chanA << 3U); }

				إذا (max_channels > 5U) { قدرون_2[5] = convert_channel_قدر(rcChannels->chanB << 3U); }

				إذا (max_channels > 6U) { قدرون_2[6] = convert_channel_قدر(rcChannels->chanC << 3U); }

				إذا (max_channels > 7U) { قدرون_2[7] = convert_channel_قدر(rcChannels->chanD << 3U); }

			} وإلا إذا (ghst_چوکٺو.type == static_cast<uint8_t>(ghst_چوکٺو_Type::چوکٺو_Type9to12)) {
				إذا (max_channels > 8U) { قدرون_2[8] = convert_channel_قدر(rcChannels->chanA << 3U); }

				إذا (max_channels > 9U) { قدرون_2[9] = convert_channel_قدر(rcChannels->chanB << 3U); }

				إذا (max_channels > 10U) { قدرون_2[10] = convert_channel_قدر(rcChannels->chanC << 3U); }

				إذا (max_channels > 11U) { قدرون_2[11] = convert_channel_قدر(rcChannels->chanD << 3U); }

			} وإلا إذا (ghst_چوکٺو.type == static_cast<uint8_t>(ghst_چوکٺو_Type::چوکٺو_Type13to16)) {
				إذا (max_channels > 12U) { قدرون_2[12] = convert_channel_قدر(rcChannels->chanA << 3U); }

				إذا (max_channels > 13U) { قدرون_2[13] = convert_channel_قدر(rcChannels->chanB << 3U); }

				إذا (max_channels > 14U) { قدرون_2[14] = convert_channel_قدر(rcChannels->chanC << 3U); }

				إذا (max_channels > 15U) { قدرون_2[15] = convert_channel_قدر(rcChannels->chanD << 3U); }

			} وإلا إذا (ghst_چوکٺو.type == static_cast<uint8_t>(ghst_چوکٺو_Type::چوکٺو_Type_Rssi)) {
				ثابت ghstPayloadRssi_t *ثابت rssi_قدرون = (ghstPayloadRssi_t *)&ghst_چوکٺو.payload;
				// TODO: call function for RSSI dBm to percentage conversion for ghost receiver
				آخري_link_stats.rssi_pct = spek_dbm_منزل_percent(static_cast<int8_t>
							   (rssi_قدرون->rssidBm));	// rssidBm sign inverted (90 = -90dBm)
				آخري_link_stats.rssi_dbm = -rssi_قدرون->rssidBm;
				آخري_link_stats.link_quality = rssi_قدرون->lq;						// 0 - 100

			} وإلا {
				GHST_DEBUG("Frame type: %u", ghst_چوکٺو.type);
			}

			*link_stats = آخري_link_stats;

			يادگيري_نقل(prev_rc_vals, قدرون_2, حجم(uint16_t) * GHST_MAX_انگ_CHANNELS);

			GHST_VERBOSE("Got Channels");

			ret = سچ;

		} وإلا {
			GHST_DEBUG("CRC check failed");
		}

	} وإلا {
		GHST_DEBUG("Got Non-RC frame (len=%u, type=%u)", current_چوکٺو_ڊيگهائي, ghst_چوکٺو.type);
	}

	// either reset or move the rest of the buffer
	إذا (current_چوکٺو_position > current_چوکٺو_ڊيگهائي) {
		GHST_VERBOSE("Moving buffer (%u > %u)", current_چوکٺو_position, current_چوکٺو_ڊيگهائي);
		memmove(ghst_چوکٺو_ptr, ghst_چوکٺو_ptr + current_چوکٺو_ڊيگهائي, current_چوکٺو_position - current_چوکٺو_ڊيگهائي);
		current_چوکٺو_position -= current_چوکٺو_ڊيگهائي;

	} وإلا {
		current_چوکٺو_position = 0U;
	}

	أرجع ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint8_t(uint8_t *buf, صحيح &وٿي, uint8_t value)
{
	buf[وٿي++] = value;
}

/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint16_t(uint8_t *buf, صحيح &وٿي, uint16_t value)
{
	buf[وٿي] = value & 0xFFU;
	buf[وٿي + 1] = value >> 8U;
	وٿي += 2;
}

/**
 * write an uint32_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint32_t(uint8_t *buf, صحيح &وٿي, uint32_t value)
{
	// Little Endian
	buf[وٿي] = value & 0xFFU;
	buf[وٿي + 1] = (value & 0xFF00) >> 8U;
	buf[وٿي + 2] = (value & 0xFF0000) >> 16U;
	buf[وٿي + 3] = (value & 0xFF000000) >> 24U;
	وٿي += 4;
}

/**
 * write frame header
 */
ساكن مضمن فراغ write_چوکٺو_سرو(uint8_t *buf, صحيح &وٿي, ghstTelemetryType type, uint8_t payload_سائيز)
{
	write_uint8_t(buf, وٿي, static_cast<uint8_t>(ghst_پتو::rx_پتو));
	write_uint8_t(buf, وٿي, payload_سائيز + GHST_چوکٺو_چڪري_جاچ_سائيز + GHST_چوکٺو_TYPE_سائيز);
	write_uint8_t(buf, وٿي, static_cast<uint8_t>(type));
}

/**
 * write frame CRC
 */
ساكن مضمن فراغ write_چوکٺو_چڪري_جاچ(uint8_t *buf, صحيح &وٿي, صحيح buf_سائيز)
{
	write_uint8_t(buf, وٿي, crc8_dvb_s2_buf(buf + 2U, buf_سائيز - 3));
}

منطقي ghst_send_telemetry_battery_حالت(صحيح uart_fd, uint16_t voltage_اندريون_10m_V,
					uint16_t current_اندريون_10m_A, uint16_t fuel_اندريون_10m_Ah)
{
	uint8_t buf[GHST_چوکٺو_PAYLOAD_سائيز_TELEMETRY + 4U]; // address, frame length, type, crc
	صحيح وٿي = 0;
	write_چوکٺو_سرو(buf, وٿي, ghstTelemetryType::batteryPack, GHST_چوکٺو_PAYLOAD_سائيز_TELEMETRY);
	write_uint16_t(buf, وٿي, voltage_اندريون_10m_V);
	write_uint16_t(buf, وٿي, current_اندريون_10m_A);
	write_uint16_t(buf, وٿي, fuel_اندريون_10m_Ah);
	write_uint8_t(buf, وٿي, 0x00U); //خالي_جايون
	write_uint8_t(buf, وٿي, 0x00U); //خالي_جايون
	write_uint8_t(buf, وٿي, 0x00U); //خالي_جايون
	write_uint8_t(buf, وٿي, 0x00U); //خالي_جايون
	write_چوکٺو_چڪري_جاچ(buf, وٿي, حجم(buf));

	أرجع اكتب(uart_fd, buf, وٿي) == وٿي;
}

منطقي ghst_send_telemetry_gps1_حالت(صحيح uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude)
{
	uint8_t buf[GHST_چوکٺو_PAYLOAD_سائيز_TELEMETRY + 4U]; // address, frame length, type, crc
	صحيح وٿي = 0;
	write_چوکٺو_سرو(buf, وٿي, ghstTelemetryType::gpsPrimary, GHST_چوکٺو_PAYLOAD_سائيز_TELEMETRY);
	write_uint32_t(buf, وٿي, latitude);
	write_uint32_t(buf, وٿي, longitude);
	write_uint16_t(buf, وٿي, altitude);
	write_چوکٺو_چڪري_جاچ(buf, وٿي, حجم(buf));

	أرجع اكتب(uart_fd, buf, وٿي) == وٿي;
}

منطقي ghst_send_telemetry_gps2_حالت(صحيح uart_fd, uint16_t ground_رفتار, uint16_t ground_ڪورس, uint8_t انگ_Sats,
				     uint16_t home_dist, uint16_t home_dir, uint8_t flags)
{
	uint8_t buf[GHST_چوکٺو_PAYLOAD_سائيز_TELEMETRY + 4U]; // address, frame length, type, crc
	صحيح وٿي = 0;
	write_چوکٺو_سرو(buf, وٿي, ghstTelemetryType::gpsSecondary, GHST_چوکٺو_PAYLOAD_سائيز_TELEMETRY);
	write_uint16_t(buf, وٿي, ground_رفتار);
	write_uint16_t(buf, وٿي, ground_ڪورس);
	write_uint8_t(buf, وٿي, انگ_Sats);
	write_uint16_t(buf, وٿي, home_dist);
	write_uint16_t(buf, وٿي, home_dir);
	write_uint8_t(buf, وٿي, flags);
	write_چوکٺو_چڪري_جاچ(buf, وٿي, حجم(buf));

	أرجع اكتب(uart_fd, buf, وٿي) == وٿي;
}
