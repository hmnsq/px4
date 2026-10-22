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

#اگر 0 // enable non-verbose debugging
#تعریف_کرو GHST_DEBUG PX4_WARN
#ورنہ
#تعریف_کرو GHST_DEBUG(...)
#اگر_ختم

#اگر 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#تعریف_کرو GHST_VERBOSE PX4_WARN
#ورنہ
#تعریف_کرو GHST_VERBOSE(...)
#اگر_ختم

#شامل <drivers/drv_hrt.h>
#شامل <termios.h>
#شامل <string.h>
#شامل <unistd.h>
#شامل <math.h>

// TODO: include RSSI dBm to percentage conversion for ghost receiver
#شامل "spektrum_rssi.h"

#شامل "ghst.hpp"
#شامل "common_rc.h"

#تعریف_کرو MIN(a,b) (((a)<(b))?(a):(b))

#تعریف_کرو GHST_چوکھٹا_PAYLOAD_سائز_TELEMETRY	(10U)
#تعریف_کرو GHST_چوکھٹا_چکری_جانچ_سائز			(1U)
#تعریف_کرو GHST_چوکھٹا_TYPE_سائز			(1U)
#تعریف_کرو GHST_TYPE_ڈیٹا_چکری_جانچ_سائز			(12U)
#تعریف_کرو GHST_ADDR_FC				(130U)
#تعریف_کرو GHST_MAX_عدد_CHANNELS			(16)

شمار جماعت ghst_parser_state_t : uint8_t {
	unsynced = 0U,
	synced
};

// only RSSI frame contains value of RSSI, if it is not received, send last received RSSI/LQ
ساکن ghstLinkStatistics_t آخری_link_stats = { .rssi_pct = -1, .rssi_dbm = NAN, .link_quality = 0 };

ساکن ghst_چوکھٹا_t &ghst_چوکھٹا = rc_decode_buf.ghst_چوکھٹا;
ساکن uint32_t current_چوکھٹا_position = 0U;
ساکن ghst_parser_state_t parser_state = ghst_parser_state_t::unsynced;

ساکن uint16_t پچھلا_rc_vals[GHST_MAX_عدد_CHANNELS];

/**
 * parse the current ghst_frame buffer
 */
ساکن منطقی ghst_parse_بفر(uint16_t *اقدار_2, ghstLinkStatistics_t *link_stats, uint16_t *عدد_اقدار,
			      uint16_t max_channels);

صحیح ghst_config(صحیح uart_fd)
{
	ساخت termios t;
	صحیح ret_val;

	// no parity, one stop bit
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, GHST_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	memset(پچھلا_rc_vals, static_cast<صحیح>(UINT16_MAX), حجم(uint16_t) * GHST_MAX_عدد_CHANNELS);
	ret_val = tcsetattr(uart_fd, TCSANOW, &t);
	واپس ret_val;
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
ساکن uint16_t convert_channel_قدر(غیرعلامتی chan_قدر);


منطقی ghst_parse(ثابت uint64_t now, ثابت uint8_t *چوکھٹا, غیرعلامتی len, uint16_t *اقدار_2,
		ghstLinkStatistics_t *link_stats, uint16_t *عدد_اقدار, uint16_t max_channels)
{
	منطقی success = جھوٹ;
	uint8_t *ghst_چوکھٹا_ptr = (uint8_t *)&ghst_چوکھٹا;

	حافظہ_نقل(اقدار_2, پچھلا_rc_vals, حجم(uint16_t) * GHST_MAX_عدد_CHANNELS);

	جب (len > 0U) {

		// fill in the ghst_buffer, as much as we can
		ثابت uint32_t current_len = MIN(len, حجم(ghst_چوکھٹا_t) - current_چوکھٹا_position);
		حافظہ_نقل(ghst_چوکھٹا_ptr + current_چوکھٹا_position, چوکھٹا, current_len);
		current_چوکھٹا_position += current_len;

		// protection to guarantee parsing progress
		اگر (current_len == 0U) {
			GHST_DEBUG("========== parser bug: no progress (%u) ===========", len);

			برائے (uint32_t i = 0U; i < current_چوکھٹا_position; ++i) {
				GHST_DEBUG("ghst_frame_ptr[%u]: 0x%x", i, ghst_چوکھٹا_ptr[i]);
			}

			// reset the parser
			current_چوکھٹا_position = 0U;
			parser_state = ghst_parser_state_t::unsynced;
			success = جھوٹ;

		} ورنہ {
			len -= current_len;
			چوکھٹا += current_len;

			اگر (ghst_parse_بفر(اقدار_2, link_stats, عدد_اقدار, max_channels)) {
				success = سچ;
			}
		}
	}

	واپس success;
}

uint8_t ghst_چوکھٹا_چکری_جانچ(ثابت ghst_چوکھٹا_t &چوکھٹا)
{
	uint8_t چکری_جانچ = crc8_dvb_s2(0U, چوکھٹا.type);

	برائے (uint32_t i = 0U; i < چوکھٹا.سرخی.لمبائی_2 - GHST_چوکھٹا_چکری_جانچ_سائز - GHST_چوکھٹا_TYPE_سائز; ++i) {
		چکری_جانچ = crc8_dvb_s2(چکری_جانچ, چوکھٹا.payload[i]);
	}

	واپس چکری_جانچ;
}

ساکن uint16_t convert_channel_قدر(غیرعلامتی صحیح chan_قدر)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	ساکن constexpr اعشاری scale = (2012.0F - 988.0F) / (1811.0F - 172.0F);
	ساکن constexpr اعشاری انحراف = 988.0F - (172.0F * scale);
	اعشاری scaled_chan_قدر = scale * static_cast<اعشاری>(chan_قدر);
	اعشاری scaled_chan_قدر_with_انحراف = scaled_chan_قدر + انحراف;
	uint16_t converted_chan_قدر = static_cast<uint16_t>(scaled_chan_قدر_with_انحراف);

	واپس converted_chan_قدر;
}

ساکن منطقی ghst_parse_بفر(uint16_t *اقدار_2, ghstLinkStatistics_t *link_stats, uint16_t *عدد_اقدار,
			      uint16_t max_channels)
{
	uint8_t *ghst_چوکھٹا_ptr = (uint8_t *)&ghst_چوکھٹا;

	اگر (parser_state == ghst_parser_state_t::unsynced) {
		// there is no sync yet, try to find an RC packet by searching for a matching frame length and type
		برائے (uint32_t i = 1U; i < current_چوکھٹا_position - 1U; ++i) {
			اگر ((ghst_چوکھٹا_ptr[i + 1U] >= static_cast<uint8_t>(ghst_چوکھٹا_Type::چوکھٹا_Type_پہلا)) &&
			    (ghst_چوکھٹا_ptr[i + 1U] <= static_cast<uint8_t>(ghst_چوکھٹا_Type::چوکھٹا_Type_آخری))) {
				اگر (ghst_چوکھٹا_ptr[i] == GHST_TYPE_ڈیٹا_چکری_جانچ_سائز) {
					parser_state = ghst_parser_state_t::synced;
					uint32_t چوکھٹا_انحراف = i - 1U;
					GHST_VERBOSE("RC channels found at offset %u", چوکھٹا_انحراف);

					// move the rest of the buffer to the beginning
					اگر (چوکھٹا_انحراف != 0U) {
						memmove(ghst_چوکھٹا_ptr, ghst_چوکھٹا_ptr + چوکھٹا_انحراف, current_چوکھٹا_position - چوکھٹا_انحراف);
						current_چوکھٹا_position -= چوکھٹا_انحراف;
					}

					توڑو;
				}
			}
		}
	}

	اگر (parser_state != ghst_parser_state_t::synced) {
		اگر (current_چوکھٹا_position >= حجم(ghst_چوکھٹا_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_چوکھٹا_position = 3U;

			حافظہ_نقل(ghst_چوکھٹا_ptr, ghst_چوکھٹا_ptr + حجم(ghst_چوکھٹا_t) - current_چوکھٹا_position, current_چوکھٹا_position);

			GHST_VERBOSE("Discarding buffer");
		}

		واپس جھوٹ;
	}


	اگر (current_چوکھٹا_position < 3U) {
		// wait until we have the address, length and type
		واپس جھوٹ;
	}

	// now we have at least the header and the type

	ثابت غیرعلامتی current_چوکھٹا_لمبائی = ghst_چوکھٹا.سرخی.لمبائی_2 + حجم(ghst_چوکھٹا_سرخی_t);

	اگر ((current_چوکھٹا_لمبائی > حجم(ghst_چوکھٹا_t)) || (current_چوکھٹا_لمبائی < 4U)) {
		// frame too long or bogus (frame length should be longer than 4, at least 1 address, 1 length, 1 type, 1 data, 1 crc)
		// discard everything and go into unsynced state
		current_چوکھٹا_position = 0U;
		parser_state = ghst_parser_state_t::unsynced;
		GHST_DEBUG("Frame too long/bogus (%u, type=%u) -> unsync", current_چوکھٹا_لمبائی, ghst_چوکھٹا.type);
		واپس جھوٹ;
	}

	اگر (current_چوکھٹا_position < current_چوکھٹا_لمبائی) {
		// we do not have the full frame yet -> wait for more data
		GHST_VERBOSE("waiting for more data (%u < %u)", current_چوکھٹا_position, current_چوکھٹا_لمبائی);
		واپس جھوٹ;
	}

	منطقی ret = جھوٹ;

	// now we have the full frame

	اگر ((ghst_چوکھٹا.type >= static_cast<uint8_t>(ghst_چوکھٹا_Type::چوکھٹا_Type_پہلا)) &&
	    (ghst_چوکھٹا.type <= static_cast<uint8_t>(ghst_چوکھٹا_Type::چوکھٹا_Type_آخری)) &&
	    (ghst_چوکھٹا.سرخی.لمبائی_2 == GHST_TYPE_ڈیٹا_چکری_جانچ_سائز) &&
	    (ghst_چوکھٹا.سرخی.device_پتہ == GHST_ADDR_FC)) {
		ثابت uint8_t چکری_جانچ = ghst_چوکھٹا.payload[ghst_چوکھٹا.سرخی.لمبائی_2 - 2U];

		اگر (چکری_جانچ == ghst_چوکھٹا_چکری_جانچ(ghst_چوکھٹا)) {
			ثابت ghst_Payload_ڈیٹا_t *ثابت rcChannels = (ghst_Payload_ڈیٹا_t *)&ghst_چوکھٹا.payload;
			*عدد_اقدار = MIN(max_channels, GHST_MAX_عدد_CHANNELS);

			// all frames contain data from chan1to4
			اگر (max_channels > 0U) { اقدار_2[0] = convert_channel_قدر(rcChannels->chan1to4.chan1 >> 1U); }

			اگر (max_channels > 1U) { اقدار_2[1] = convert_channel_قدر(rcChannels->chan1to4.chan2 >> 1U); }

			اگر (max_channels > 2U) { اقدار_2[2] = convert_channel_قدر(rcChannels->chan1to4.chan3 >> 1U); }

			اگر (max_channels > 3U) { اقدار_2[3] = convert_channel_قدر(rcChannels->chan1to4.chan4 >> 1U); }

			اگر (ghst_چوکھٹا.type == static_cast<uint8_t>(ghst_چوکھٹا_Type::چوکھٹا_Type5to8)) {
				اگر (max_channels > 4U) { اقدار_2[4] = convert_channel_قدر(rcChannels->chanA << 3U); }

				اگر (max_channels > 5U) { اقدار_2[5] = convert_channel_قدر(rcChannels->chanB << 3U); }

				اگر (max_channels > 6U) { اقدار_2[6] = convert_channel_قدر(rcChannels->chanC << 3U); }

				اگر (max_channels > 7U) { اقدار_2[7] = convert_channel_قدر(rcChannels->chanD << 3U); }

			} ورنہ اگر (ghst_چوکھٹا.type == static_cast<uint8_t>(ghst_چوکھٹا_Type::چوکھٹا_Type9to12)) {
				اگر (max_channels > 8U) { اقدار_2[8] = convert_channel_قدر(rcChannels->chanA << 3U); }

				اگر (max_channels > 9U) { اقدار_2[9] = convert_channel_قدر(rcChannels->chanB << 3U); }

				اگر (max_channels > 10U) { اقدار_2[10] = convert_channel_قدر(rcChannels->chanC << 3U); }

				اگر (max_channels > 11U) { اقدار_2[11] = convert_channel_قدر(rcChannels->chanD << 3U); }

			} ورنہ اگر (ghst_چوکھٹا.type == static_cast<uint8_t>(ghst_چوکھٹا_Type::چوکھٹا_Type13to16)) {
				اگر (max_channels > 12U) { اقدار_2[12] = convert_channel_قدر(rcChannels->chanA << 3U); }

				اگر (max_channels > 13U) { اقدار_2[13] = convert_channel_قدر(rcChannels->chanB << 3U); }

				اگر (max_channels > 14U) { اقدار_2[14] = convert_channel_قدر(rcChannels->chanC << 3U); }

				اگر (max_channels > 15U) { اقدار_2[15] = convert_channel_قدر(rcChannels->chanD << 3U); }

			} ورنہ اگر (ghst_چوکھٹا.type == static_cast<uint8_t>(ghst_چوکھٹا_Type::چوکھٹا_Type_Rssi)) {
				ثابت ghstPayloadRssi_t *ثابت rssi_اقدار = (ghstPayloadRssi_t *)&ghst_چوکھٹا.payload;
				// TODO: call function for RSSI dBm to percentage conversion for ghost receiver
				آخری_link_stats.rssi_pct = spek_dbm_مقصد_percent(static_cast<int8_t>
							   (rssi_اقدار->rssidBm));	// rssidBm sign inverted (90 = -90dBm)
				آخری_link_stats.rssi_dbm = -rssi_اقدار->rssidBm;
				آخری_link_stats.link_quality = rssi_اقدار->lq;						// 0 - 100

			} ورنہ {
				GHST_DEBUG("Frame type: %u", ghst_چوکھٹا.type);
			}

			*link_stats = آخری_link_stats;

			حافظہ_نقل(پچھلا_rc_vals, اقدار_2, حجم(uint16_t) * GHST_MAX_عدد_CHANNELS);

			GHST_VERBOSE("Got Channels");

			ret = سچ;

		} ورنہ {
			GHST_DEBUG("CRC check failed");
		}

	} ورنہ {
		GHST_DEBUG("Got Non-RC frame (len=%u, type=%u)", current_چوکھٹا_لمبائی, ghst_چوکھٹا.type);
	}

	// either reset or move the rest of the buffer
	اگر (current_چوکھٹا_position > current_چوکھٹا_لمبائی) {
		GHST_VERBOSE("Moving buffer (%u > %u)", current_چوکھٹا_position, current_چوکھٹا_لمبائی);
		memmove(ghst_چوکھٹا_ptr, ghst_چوکھٹا_ptr + current_چوکھٹا_لمبائی, current_چوکھٹا_position - current_چوکھٹا_لمبائی);
		current_چوکھٹا_position -= current_چوکھٹا_لمبائی;

	} ورنہ {
		current_چوکھٹا_position = 0U;
	}

	واپس ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
ساکن اندرونی خالی write_uint8_t(uint8_t *buf, صحیح &انحراف, uint8_t value)
{
	buf[انحراف++] = value;
}

/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
ساکن اندرونی خالی write_uint16_t(uint8_t *buf, صحیح &انحراف, uint16_t value)
{
	buf[انحراف] = value & 0xFFU;
	buf[انحراف + 1] = value >> 8U;
	انحراف += 2;
}

/**
 * write an uint32_t value to a buffer at a given offset and increment the offset
 */
ساکن اندرونی خالی write_uint32_t(uint8_t *buf, صحیح &انحراف, uint32_t value)
{
	// Little Endian
	buf[انحراف] = value & 0xFFU;
	buf[انحراف + 1] = (value & 0xFF00) >> 8U;
	buf[انحراف + 2] = (value & 0xFF0000) >> 16U;
	buf[انحراف + 3] = (value & 0xFF000000) >> 24U;
	انحراف += 4;
}

/**
 * write frame header
 */
ساکن اندرونی خالی write_چوکھٹا_سرخی(uint8_t *buf, صحیح &انحراف, ghstTelemetryType type, uint8_t payload_سائز)
{
	write_uint8_t(buf, انحراف, static_cast<uint8_t>(ghst_پتہ::rx_پتہ));
	write_uint8_t(buf, انحراف, payload_سائز + GHST_چوکھٹا_چکری_جانچ_سائز + GHST_چوکھٹا_TYPE_سائز);
	write_uint8_t(buf, انحراف, static_cast<uint8_t>(type));
}

/**
 * write frame CRC
 */
ساکن اندرونی خالی write_چوکھٹا_چکری_جانچ(uint8_t *buf, صحیح &انحراف, صحیح buf_سائز)
{
	write_uint8_t(buf, انحراف, crc8_dvb_s2_buf(buf + 2U, buf_سائز - 3));
}

منطقی ghst_send_telemetry_battery_حالت(صحیح uart_fd, uint16_t voltage_اندر_10m_V,
					uint16_t current_اندر_10m_A, uint16_t fuel_اندر_10m_Ah)
{
	uint8_t buf[GHST_چوکھٹا_PAYLOAD_سائز_TELEMETRY + 4U]; // address, frame length, type, crc
	صحیح انحراف = 0;
	write_چوکھٹا_سرخی(buf, انحراف, ghstTelemetryType::batteryPack, GHST_چوکھٹا_PAYLOAD_سائز_TELEMETRY);
	write_uint16_t(buf, انحراف, voltage_اندر_10m_V);
	write_uint16_t(buf, انحراف, current_اندر_10m_A);
	write_uint16_t(buf, انحراف, fuel_اندر_10m_Ah);
	write_uint8_t(buf, انحراف, 0x00U); //خالی_خانے
	write_uint8_t(buf, انحراف, 0x00U); //خالی_خانے
	write_uint8_t(buf, انحراف, 0x00U); //خالی_خانے
	write_uint8_t(buf, انحراف, 0x00U); //خالی_خانے
	write_چوکھٹا_چکری_جانچ(buf, انحراف, حجم(buf));

	واپس اكتب(uart_fd, buf, انحراف) == انحراف;
}

منطقی ghst_send_telemetry_gps1_حالت(صحیح uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude)
{
	uint8_t buf[GHST_چوکھٹا_PAYLOAD_سائز_TELEMETRY + 4U]; // address, frame length, type, crc
	صحیح انحراف = 0;
	write_چوکھٹا_سرخی(buf, انحراف, ghstTelemetryType::gpsPrimary, GHST_چوکھٹا_PAYLOAD_سائز_TELEMETRY);
	write_uint32_t(buf, انحراف, latitude);
	write_uint32_t(buf, انحراف, longitude);
	write_uint16_t(buf, انحراف, altitude);
	write_چوکھٹا_چکری_جانچ(buf, انحراف, حجم(buf));

	واپس اكتب(uart_fd, buf, انحراف) == انحراف;
}

منطقی ghst_send_telemetry_gps2_حالت(صحیح uart_fd, uint16_t ground_رفتار, uint16_t ground_نصاب, uint8_t عدد_Sats,
				     uint16_t home_dist, uint16_t home_dir, uint8_t flags)
{
	uint8_t buf[GHST_چوکھٹا_PAYLOAD_سائز_TELEMETRY + 4U]; // address, frame length, type, crc
	صحیح انحراف = 0;
	write_چوکھٹا_سرخی(buf, انحراف, ghstTelemetryType::gpsSecondary, GHST_چوکھٹا_PAYLOAD_سائز_TELEMETRY);
	write_uint16_t(buf, انحراف, ground_رفتار);
	write_uint16_t(buf, انحراف, ground_نصاب);
	write_uint8_t(buf, انحراف, عدد_Sats);
	write_uint16_t(buf, انحراف, home_dist);
	write_uint16_t(buf, انحراف, home_dir);
	write_uint8_t(buf, انحراف, flags);
	write_چوکھٹا_چکری_جانچ(buf, انحراف, حجم(buf));

	واپس اكتب(uart_fd, buf, انحراف) == انحراف;
}
