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
#تعریف_کن GHST_DEBUG PX4_WARN
#وإلا
#تعریف_کن GHST_DEBUG(...)
#پایان_اگر

#إذا 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#تعریف_کن GHST_VERBOSE PX4_WARN
#وإلا
#تعریف_کن GHST_VERBOSE(...)
#پایان_اگر

#ضمن <drivers/drv_hrt.h>
#ضمن <termios.h>
#ضمن <string.h>
#ضمن <unistd.h>
#ضمن <math.h>

// TODO: include RSSI dBm to percentage conversion for ghost receiver
#ضمن "spektrum_rssi.h"

#ضمن "ghst.hpp"
#ضمن "common_rc.h"

#تعریف_کن MIN(a,b) (((a)<(b))?(a):(b))

#تعریف_کن GHST_قاب_PAYLOAD_اندازه_TELEMETRY	(10U)
#تعریف_کن GHST_قاب_بررسی_چرخهای_اندازه			(1U)
#تعریف_کن GHST_قاب_TYPE_اندازه			(1U)
#تعریف_کن GHST_TYPE_داده_بررسی_چرخهای_اندازه			(12U)
#تعریف_کن GHST_ADDR_FC				(130U)
#تعریف_کن GHST_MAX_رقم_CHANNELS			(16)

تعداد صنف ghst_parser_state_t : uint8_t {
	unsynced = 0U,
	synced
};

// only RSSI frame contains value of RSSI, if it is not received, send last received RSSI/LQ
ساكن ghstLinkStatistics_t آخرین_link_stats = { .rssi_pct = -1, .rssi_dbm = NAN, .link_quality = 0 };

ساكن ghst_قاب_t &ghst_قاب = rc_decode_buf.ghst_قاب;
ساكن uint32_t current_قاب_position = 0U;
ساكن ghst_parser_state_t parser_state = ghst_parser_state_t::unsynced;

ساكن uint16_t prev_rc_vals[GHST_MAX_رقم_CHANNELS];

/**
 * parse the current ghst_frame buffer
 */
ساكن منطقي ghst_parse_میانگیر(uint16_t *مقادیر_2, ghstLinkStatistics_t *link_stats, uint16_t *رقم_مقادیر,
			      uint16_t max_channels);

صحيح ghst_config(صحيح uart_fd)
{
	بنية termios t;
	صحيح ret_val;

	// no parity, one stop bit
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, GHST_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	memset(prev_rc_vals, static_cast<صحيح>(UINT16_MAX), حجم(uint16_t) * GHST_MAX_رقم_CHANNELS);
	ret_val = tcsetattr(uart_fd, TCSANOW, &t);
	أرجع ret_val;
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
ساكن uint16_t convert_channel_مقدار(بیعلامت chan_مقدار);


منطقي ghst_parse(ثابت uint64_t now, ثابت uint8_t *قاب, بیعلامت len, uint16_t *مقادیر_2,
		ghstLinkStatistics_t *link_stats, uint16_t *رقم_مقادیر, uint16_t max_channels)
{
	منطقي success = خطأ;
	uint8_t *ghst_قاب_ptr = (uint8_t *)&ghst_قاب;

	کپی_حافظه(مقادیر_2, prev_rc_vals, حجم(uint16_t) * GHST_MAX_رقم_CHANNELS);

	طالما (len > 0U) {

		// fill in the ghst_buffer, as much as we can
		ثابت uint32_t current_len = MIN(len, حجم(ghst_قاب_t) - current_قاب_position);
		کپی_حافظه(ghst_قاب_ptr + current_قاب_position, قاب, current_len);
		current_قاب_position += current_len;

		// protection to guarantee parsing progress
		إذا (current_len == 0U) {
			GHST_DEBUG("========== parser bug: no progress (%u) ===========", len);

			لكل (uint32_t i = 0U; i < current_قاب_position; ++i) {
				GHST_DEBUG("ghst_frame_ptr[%u]: 0x%x", i, ghst_قاب_ptr[i]);
			}

			// reset the parser
			current_قاب_position = 0U;
			parser_state = ghst_parser_state_t::unsynced;
			success = خطأ;

		} وإلا {
			len -= current_len;
			قاب += current_len;

			إذا (ghst_parse_میانگیر(مقادیر_2, link_stats, رقم_مقادیر, max_channels)) {
				success = درست;
			}
		}
	}

	أرجع success;
}

uint8_t ghst_قاب_بررسی_چرخهای(ثابت ghst_قاب_t &قاب)
{
	uint8_t بررسی_چرخهای = crc8_dvb_s2(0U, قاب.type);

	لكل (uint32_t i = 0U; i < قاب.سرآیند.درازی - GHST_قاب_بررسی_چرخهای_اندازه - GHST_قاب_TYPE_اندازه; ++i) {
		بررسی_چرخهای = crc8_dvb_s2(بررسی_چرخهای, قاب.payload[i]);
	}

	أرجع بررسی_چرخهای;
}

ساكن uint16_t convert_channel_مقدار(بیعلامت صحيح chan_مقدار)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	ساكن constexpr عائم scale = (2012.0F - 988.0F) / (1811.0F - 172.0F);
	ساكن constexpr عائم جابه_جایی = 988.0F - (172.0F * scale);
	عائم scaled_chan_مقدار = scale * static_cast<عائم>(chan_مقدار);
	عائم scaled_chan_مقدار_with_جابه_جایی = scaled_chan_مقدار + جابه_جایی;
	uint16_t converted_chan_مقدار = static_cast<uint16_t>(scaled_chan_مقدار_with_جابه_جایی);

	أرجع converted_chan_مقدار;
}

ساكن منطقي ghst_parse_میانگیر(uint16_t *مقادیر_2, ghstLinkStatistics_t *link_stats, uint16_t *رقم_مقادیر,
			      uint16_t max_channels)
{
	uint8_t *ghst_قاب_ptr = (uint8_t *)&ghst_قاب;

	إذا (parser_state == ghst_parser_state_t::unsynced) {
		// there is no sync yet, try to find an RC packet by searching for a matching frame length and type
		لكل (uint32_t i = 1U; i < current_قاب_position - 1U; ++i) {
			إذا ((ghst_قاب_ptr[i + 1U] >= static_cast<uint8_t>(ghst_قاب_Type::قاب_Type_اولی)) &&
			    (ghst_قاب_ptr[i + 1U] <= static_cast<uint8_t>(ghst_قاب_Type::قاب_Type_آخرین))) {
				إذا (ghst_قاب_ptr[i] == GHST_TYPE_داده_بررسی_چرخهای_اندازه) {
					parser_state = ghst_parser_state_t::synced;
					uint32_t قاب_جابه_جایی = i - 1U;
					GHST_VERBOSE("RC channels found at offset %u", قاب_جابه_جایی);

					// move the rest of the buffer to the beginning
					إذا (قاب_جابه_جایی != 0U) {
						memmove(ghst_قاب_ptr, ghst_قاب_ptr + قاب_جابه_جایی, current_قاب_position - قاب_جابه_جایی);
						current_قاب_position -= قاب_جابه_جایی;
					}

					اكسر;
				}
			}
		}
	}

	إذا (parser_state != ghst_parser_state_t::synced) {
		إذا (current_قاب_position >= حجم(ghst_قاب_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_قاب_position = 3U;

			کپی_حافظه(ghst_قاب_ptr, ghst_قاب_ptr + حجم(ghst_قاب_t) - current_قاب_position, current_قاب_position);

			GHST_VERBOSE("Discarding buffer");
		}

		أرجع خطأ;
	}


	إذا (current_قاب_position < 3U) {
		// wait until we have the address, length and type
		أرجع خطأ;
	}

	// now we have at least the header and the type

	ثابت بیعلامت current_قاب_درازی = ghst_قاب.سرآیند.درازی + حجم(ghst_قاب_سرآیند_t);

	إذا ((current_قاب_درازی > حجم(ghst_قاب_t)) || (current_قاب_درازی < 4U)) {
		// frame too long or bogus (frame length should be longer than 4, at least 1 address, 1 length, 1 type, 1 data, 1 crc)
		// discard everything and go into unsynced state
		current_قاب_position = 0U;
		parser_state = ghst_parser_state_t::unsynced;
		GHST_DEBUG("Frame too long/bogus (%u, type=%u) -> unsync", current_قاب_درازی, ghst_قاب.type);
		أرجع خطأ;
	}

	إذا (current_قاب_position < current_قاب_درازی) {
		// we do not have the full frame yet -> wait for more data
		GHST_VERBOSE("waiting for more data (%u < %u)", current_قاب_position, current_قاب_درازی);
		أرجع خطأ;
	}

	منطقي ret = خطأ;

	// now we have the full frame

	إذا ((ghst_قاب.type >= static_cast<uint8_t>(ghst_قاب_Type::قاب_Type_اولی)) &&
	    (ghst_قاب.type <= static_cast<uint8_t>(ghst_قاب_Type::قاب_Type_آخرین)) &&
	    (ghst_قاب.سرآیند.درازی == GHST_TYPE_داده_بررسی_چرخهای_اندازه) &&
	    (ghst_قاب.سرآیند.device_آدرس == GHST_ADDR_FC)) {
		ثابت uint8_t بررسی_چرخهای = ghst_قاب.payload[ghst_قاب.سرآیند.درازی - 2U];

		إذا (بررسی_چرخهای == ghst_قاب_بررسی_چرخهای(ghst_قاب)) {
			ثابت ghst_Payload_داده_t *ثابت rcChannels = (ghst_Payload_داده_t *)&ghst_قاب.payload;
			*رقم_مقادیر = MIN(max_channels, GHST_MAX_رقم_CHANNELS);

			// all frames contain data from chan1to4
			إذا (max_channels > 0U) { مقادیر_2[0] = convert_channel_مقدار(rcChannels->chan1to4.chan1 >> 1U); }

			إذا (max_channels > 1U) { مقادیر_2[1] = convert_channel_مقدار(rcChannels->chan1to4.chan2 >> 1U); }

			إذا (max_channels > 2U) { مقادیر_2[2] = convert_channel_مقدار(rcChannels->chan1to4.chan3 >> 1U); }

			إذا (max_channels > 3U) { مقادیر_2[3] = convert_channel_مقدار(rcChannels->chan1to4.chan4 >> 1U); }

			إذا (ghst_قاب.type == static_cast<uint8_t>(ghst_قاب_Type::قاب_Type5to8)) {
				إذا (max_channels > 4U) { مقادیر_2[4] = convert_channel_مقدار(rcChannels->chanA << 3U); }

				إذا (max_channels > 5U) { مقادیر_2[5] = convert_channel_مقدار(rcChannels->chanB << 3U); }

				إذا (max_channels > 6U) { مقادیر_2[6] = convert_channel_مقدار(rcChannels->chanC << 3U); }

				إذا (max_channels > 7U) { مقادیر_2[7] = convert_channel_مقدار(rcChannels->chanD << 3U); }

			} وإلا إذا (ghst_قاب.type == static_cast<uint8_t>(ghst_قاب_Type::قاب_Type9to12)) {
				إذا (max_channels > 8U) { مقادیر_2[8] = convert_channel_مقدار(rcChannels->chanA << 3U); }

				إذا (max_channels > 9U) { مقادیر_2[9] = convert_channel_مقدار(rcChannels->chanB << 3U); }

				إذا (max_channels > 10U) { مقادیر_2[10] = convert_channel_مقدار(rcChannels->chanC << 3U); }

				إذا (max_channels > 11U) { مقادیر_2[11] = convert_channel_مقدار(rcChannels->chanD << 3U); }

			} وإلا إذا (ghst_قاب.type == static_cast<uint8_t>(ghst_قاب_Type::قاب_Type13to16)) {
				إذا (max_channels > 12U) { مقادیر_2[12] = convert_channel_مقدار(rcChannels->chanA << 3U); }

				إذا (max_channels > 13U) { مقادیر_2[13] = convert_channel_مقدار(rcChannels->chanB << 3U); }

				إذا (max_channels > 14U) { مقادیر_2[14] = convert_channel_مقدار(rcChannels->chanC << 3U); }

				إذا (max_channels > 15U) { مقادیر_2[15] = convert_channel_مقدار(rcChannels->chanD << 3U); }

			} وإلا إذا (ghst_قاب.type == static_cast<uint8_t>(ghst_قاب_Type::قاب_Type_Rssi)) {
				ثابت ghstPayloadRssi_t *ثابت rssi_مقادیر = (ghstPayloadRssi_t *)&ghst_قاب.payload;
				// TODO: call function for RSSI dBm to percentage conversion for ghost receiver
				آخرین_link_stats.rssi_pct = spek_dbm_مقصد_percent(static_cast<int8_t>
							   (rssi_مقادیر->rssidBm));	// rssidBm sign inverted (90 = -90dBm)
				آخرین_link_stats.rssi_dbm = -rssi_مقادیر->rssidBm;
				آخرین_link_stats.link_quality = rssi_مقادیر->lq;						// 0 - 100

			} وإلا {
				GHST_DEBUG("Frame type: %u", ghst_قاب.type);
			}

			*link_stats = آخرین_link_stats;

			کپی_حافظه(prev_rc_vals, مقادیر_2, حجم(uint16_t) * GHST_MAX_رقم_CHANNELS);

			GHST_VERBOSE("Got Channels");

			ret = درست;

		} وإلا {
			GHST_DEBUG("CRC check failed");
		}

	} وإلا {
		GHST_DEBUG("Got Non-RC frame (len=%u, type=%u)", current_قاب_درازی, ghst_قاب.type);
	}

	// either reset or move the rest of the buffer
	إذا (current_قاب_position > current_قاب_درازی) {
		GHST_VERBOSE("Moving buffer (%u > %u)", current_قاب_position, current_قاب_درازی);
		memmove(ghst_قاب_ptr, ghst_قاب_ptr + current_قاب_درازی, current_قاب_position - current_قاب_درازی);
		current_قاب_position -= current_قاب_درازی;

	} وإلا {
		current_قاب_position = 0U;
	}

	أرجع ret;
}

/**
 * write an uint8_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint8_t(uint8_t *buf, صحيح &جابه_جایی, uint8_t value)
{
	buf[جابه_جایی++] = value;
}

/**
 * write an uint16_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint16_t(uint8_t *buf, صحيح &جابه_جایی, uint16_t value)
{
	buf[جابه_جایی] = value & 0xFFU;
	buf[جابه_جایی + 1] = value >> 8U;
	جابه_جایی += 2;
}

/**
 * write an uint32_t value to a buffer at a given offset and increment the offset
 */
ساكن مضمن فراغ write_uint32_t(uint8_t *buf, صحيح &جابه_جایی, uint32_t value)
{
	// Little Endian
	buf[جابه_جایی] = value & 0xFFU;
	buf[جابه_جایی + 1] = (value & 0xFF00) >> 8U;
	buf[جابه_جایی + 2] = (value & 0xFF0000) >> 16U;
	buf[جابه_جایی + 3] = (value & 0xFF000000) >> 24U;
	جابه_جایی += 4;
}

/**
 * write frame header
 */
ساكن مضمن فراغ write_قاب_سرآیند(uint8_t *buf, صحيح &جابه_جایی, ghstTelemetryType type, uint8_t payload_اندازه)
{
	write_uint8_t(buf, جابه_جایی, static_cast<uint8_t>(ghst_آدرس::rx_آدرس));
	write_uint8_t(buf, جابه_جایی, payload_اندازه + GHST_قاب_بررسی_چرخهای_اندازه + GHST_قاب_TYPE_اندازه);
	write_uint8_t(buf, جابه_جایی, static_cast<uint8_t>(type));
}

/**
 * write frame CRC
 */
ساكن مضمن فراغ write_قاب_بررسی_چرخهای(uint8_t *buf, صحيح &جابه_جایی, صحيح buf_اندازه)
{
	write_uint8_t(buf, جابه_جایی, crc8_dvb_s2_buf(buf + 2U, buf_اندازه - 3));
}

منطقي ghst_send_telemetry_battery_وضعیت(صحيح uart_fd, uint16_t voltage_درون_10m_V,
					uint16_t current_درون_10m_A, uint16_t fuel_درون_10m_Ah)
{
	uint8_t buf[GHST_قاب_PAYLOAD_اندازه_TELEMETRY + 4U]; // address, frame length, type, crc
	صحيح جابه_جایی = 0;
	write_قاب_سرآیند(buf, جابه_جایی, ghstTelemetryType::batteryPack, GHST_قاب_PAYLOAD_اندازه_TELEMETRY);
	write_uint16_t(buf, جابه_جایی, voltage_درون_10m_V);
	write_uint16_t(buf, جابه_جایی, current_درون_10m_A);
	write_uint16_t(buf, جابه_جایی, fuel_درون_10m_Ah);
	write_uint8_t(buf, جابه_جایی, 0x00U); //خالی
	write_uint8_t(buf, جابه_جایی, 0x00U); //خالی
	write_uint8_t(buf, جابه_جایی, 0x00U); //خالی
	write_uint8_t(buf, جابه_جایی, 0x00U); //خالی
	write_قاب_بررسی_چرخهای(buf, جابه_جایی, حجم(buf));

	أرجع اكتب(uart_fd, buf, جابه_جایی) == جابه_جایی;
}

منطقي ghst_send_telemetry_gps1_وضعیت(صحيح uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude)
{
	uint8_t buf[GHST_قاب_PAYLOAD_اندازه_TELEMETRY + 4U]; // address, frame length, type, crc
	صحيح جابه_جایی = 0;
	write_قاب_سرآیند(buf, جابه_جایی, ghstTelemetryType::gpsPrimary, GHST_قاب_PAYLOAD_اندازه_TELEMETRY);
	write_uint32_t(buf, جابه_جایی, latitude);
	write_uint32_t(buf, جابه_جایی, longitude);
	write_uint16_t(buf, جابه_جایی, altitude);
	write_قاب_بررسی_چرخهای(buf, جابه_جایی, حجم(buf));

	أرجع اكتب(uart_fd, buf, جابه_جایی) == جابه_جایی;
}

منطقي ghst_send_telemetry_gps2_وضعیت(صحيح uart_fd, uint16_t ground_سرعت, uint16_t ground_دوره, uint8_t رقم_Sats,
				     uint16_t home_dist, uint16_t home_dir, uint8_t flags)
{
	uint8_t buf[GHST_قاب_PAYLOAD_اندازه_TELEMETRY + 4U]; // address, frame length, type, crc
	صحيح جابه_جایی = 0;
	write_قاب_سرآیند(buf, جابه_جایی, ghstTelemetryType::gpsSecondary, GHST_قاب_PAYLOAD_اندازه_TELEMETRY);
	write_uint16_t(buf, جابه_جایی, ground_سرعت);
	write_uint16_t(buf, جابه_جایی, ground_دوره);
	write_uint8_t(buf, جابه_جایی, رقم_Sats);
	write_uint16_t(buf, جابه_جایی, home_dist);
	write_uint16_t(buf, جابه_جایی, home_dir);
	write_uint8_t(buf, جابه_جایی, flags);
	write_قاب_بررسی_چرخهای(buf, جابه_جایی, حجم(buf));

	أرجع اكتب(uart_fd, buf, جابه_جایی) == جابه_جایی;
}
