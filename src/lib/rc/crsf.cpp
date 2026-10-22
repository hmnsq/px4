/****************************************************************************
 *
 *   Copyright (c) 2018 PX4 Development Team. All rights reserved.
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

#ఒకవేళ 0 // enable non-verbose debugging
#నిర్వచించు CRSF_DEBUG PX4_WARN
#లేకపోతే
#నిర్వచించు CRSF_DEBUG(...)
#అయితే_ముగింపు

#ఒకవేళ 0 // verbose debugging. Careful when enabling: it leads to too much output, causing dropped bytes
#నిర్వచించు CRSF_VERBOSE PX4_WARN
#లేకపోతే
#నిర్వచించు CRSF_VERBOSE(...)
#అయితే_ముగింపు

#అన్తర్భావ <drivers/drv_hrt.h>
#అన్తర్భావ <termios.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "crsf.h"
#అన్తర్భావ "common_rc.h"

#నిర్వచించు MIN(a,b) (((a)<(b))?(a):(b))
#నిర్వచించు MAX(a,b) (((a)>(b))?(a):(b))


#నిర్వచించు CRSF_BAUDRATE 420000

#నిర్వచించు CRSF_SYNC_BYTE 0xC8

గణన తరగతి crsf_చట్రం_type_t : uint8_t {
	gps = 0x02,
	battery_sensor = 0x08,
	baro_altitude = 0x09,
	link_statistics = 0x14,
	rc_channels_packed = 0x16,
	attitude = 0x1E,
	flight_బహుళకం = 0x21,

	// Extended Header Frames, range: 0x28 to 0x96
	device_ping = 0x28,
	device_info = 0x29,
	parameter_settings_నమోదు = 0x2B,
	parameter_read = 0x2C,
	parameter_write = 0x2D,
	command = 0x32
};

గణన తరగతి crsf_payload_కొలత_t : uint8_t {
	gps = 15,
	battery_sensor = 8,
	baro_altitude = 4, ///< altitude (uint16) + vertical speed (int16)
	link_statistics = 10,
	rc_channels = 22, ///< 11 bits per channel * 16 channels = 22 bytes.
	attitude = 6,
};


గణన తరగతి crsf_చిరునామా_t : uint8_t {
	ప్రసారం = 0x00,
	usb = 0x10,
	tbs_core_pnp_pro = 0x80,
	reserved1 = 0x8A,
	current_sensor = 0xC0,
	gps = 0xC2,
	tbs_blackbox = 0xC4,
	flight_controller = 0xC8,
	reserved2 = 0xCA,
	race_ట్యాగ్ = 0xCC,
	radio_transmitter = 0xEA,
	crsf_receiver = 0xEC,
	crsf_transmitter = 0xEE
};

#సూచన pack(నెట్టు, 1)
నిర్మాణం crsf_payload_RC_channels_packed_t {
	// 176 bits of data (11 bits per channel * 16 channels) = 22 bytes
	గుర్తులేని chan0 : 11;
	గుర్తులేని chan1 : 11;
	గుర్తులేని chan2 : 11;
	గుర్తులేని chan3 : 11;
	గుర్తులేని chan4 : 11;
	గుర్తులేని chan5 : 11;
	గుర్తులేని chan6 : 11;
	గుర్తులేని chan7 : 11;
	గుర్తులేని chan8 : 11;
	గుర్తులేని chan9 : 11;
	గుర్తులేని chan10 : 11;
	గుర్తులేని chan11 : 11;
	గుర్తులేని chan12 : 11;
	గుర్తులేని chan13 : 11;
	గుర్తులేని chan14 : 11;
	గుర్తులేని chan15 : 11;
};

#సూచన pack(తీయి)

గణన తరగతి crsf_parser_state_t : uint8_t {
	unsynced = 0,
	synced
};

స్థిర crsf_చట్రం_t	 &crsf_చట్రం = rc_decode_buf.crsf_చట్రం;
స్థిర గుర్తులేని current_చట్రం_position = 0;
స్థిర crsf_parser_state_t parser_state = crsf_parser_state_t::unsynced;

/**
 * parse the current crsf_frame buffer
 */
స్థిర తార్కిక crsf_parse_బఫరు(uint16_t *విలువలు_2, uint16_t *అంకె_విలువలు, uint16_t max_channels);

uint8_t crsf_చట్రం_చక్రీయ_తనిఖీ(స్థిరరాశి crsf_చట్రం_t &చట్రం);


పూర్ణాంకం
crsf_config(పూర్ణాంకం uart_fd)
{
	నిర్మాణం termios t;

	/* no parity, one stop bit */
	tcgetattr(uart_fd, &t);
	cfsetspeed(&t, CRSF_BAUDRATE);
	t.c_cflag &= ~(CSTOPB | PARENB);
	తిరిగి tcsetattr(uart_fd, TCSANOW, &t);
}

/**
 * Convert from RC to PWM value
 * @param chan_value channel value in [172, 1811]
 * @return PWM channel value in [1000, 2000]
 */
స్థిర uint16_t convert_channel_విలువ(గుర్తులేని chan_విలువ);


తార్కిక crsf_parse(స్థిరరాశి uint64_t now, స్థిరరాశి uint8_t *చట్రం, గుర్తులేని len, uint16_t *విలువలు_2,
		uint16_t *అంకె_విలువలు, uint16_t max_channels)
{
	తార్కిక ret = అబద్ధం;
	uint8_t *crsf_చట్రం_ptr = (uint8_t *)&crsf_చట్రం;

	అయితే (len > 0) {

		// fill in the crsf_buffer, as much as we can
		స్థిరరాశి గుర్తులేని current_len = MIN(len, పరిమాణం(crsf_చట్రం_t) - current_చట్రం_position);
		జ్ఞాపకం_నకలు(crsf_చట్రం_ptr + current_చట్రం_position, చట్రం, current_len);
		current_చట్రం_position += current_len;

		// protection to guarantee parsing progress
		ఒకవేళ (current_len == 0) {
			CRSF_DEBUG("========== parser bug: no progress (%i) ===========", len);

			కొరకు (గుర్తులేని i = 0; i < current_చట్రం_position; ++i) {
				CRSF_DEBUG("crsf_frame_ptr[%i]: 0x%x", i, (పూర్ణాంకం)crsf_చట్రం_ptr[i]);
			}

			// reset the parser
			current_చట్రం_position = 0;
			parser_state = crsf_parser_state_t::unsynced;
			తిరిగి అబద్ధం;
		}

		len -= current_len;
		చట్రం += current_len;

		ఒకవేళ (crsf_parse_బఫరు(విలువలు_2, అంకె_విలువలు, max_channels)) {
			ret = నిజం;
		}
	}


	తిరిగి ret;
}

uint8_t crsf_చట్రం_చక్రీయ_తనిఖీ(స్థిరరాశి crsf_చట్రం_t &చట్రం)
{
	// CRC includes type and payload
	uint8_t చక్రీయ_తనిఖీ = crc8_dvb_s2(0, చట్రం.type);

	కొరకు (పూర్ణాంకం i = 0; i < చట్రం.శీర్షిక.నిడివి - 2; ++i) {
		చక్రీయ_తనిఖీ = crc8_dvb_s2(చక్రీయ_తనిఖీ, చట్రం.payload[i]);
	}

	తిరిగి చక్రీయ_తనిఖీ;
}

స్థిర uint16_t convert_channel_విలువ(గుర్తులేని chan_విలువ)
{
	/*
	 *       RC     PWM
	 * min  172 ->  988us
	 * mid  992 -> 1500us
	 * max 1811 -> 2012us
	 */
	స్థిర constexpr దశాంశం scale = (2012.f - 988.f) / (1811.f - 172.f);
	స్థిర constexpr దశాంశం స్థానభ్రంశం = 988.f - 172.f * scale;
	తిరిగి (scale * chan_విలువ) + స్థానభ్రంశం;
}

స్థిర తార్కిక crsf_parse_బఫరు(uint16_t *విలువలు_2, uint16_t *అంకె_విలువలు, uint16_t max_channels)
{
	uint8_t *crsf_చట్రం_ptr = (uint8_t *)&crsf_చట్రం;

	ఒకవేళ (parser_state == crsf_parser_state_t::unsynced) {
		// there is no sync byte, try to find an RC packet by searching for a matching frame length and type
		కొరకు (గుర్తులేని i = 1; i < current_చట్రం_position - 1; ++i) {
			ఒకవేళ (crsf_చట్రం_ptr[i] == (uint8_t)crsf_payload_కొలత_t::rc_channels + 2 &&
			    crsf_చట్రం_ptr[i + 1] == (uint8_t)crsf_చట్రం_type_t::rc_channels_packed) {
				parser_state = crsf_parser_state_t::synced;
				గుర్తులేని చట్రం_స్థానభ్రంశం = i - 1;
				CRSF_VERBOSE("RC channels found at offset %i", చట్రం_స్థానభ్రంశం);

				// move the rest of the buffer to the beginning
				ఒకవేళ (చట్రం_స్థానభ్రంశం != 0) {
					memmove(crsf_చట్రం_ptr, crsf_చట్రం_ptr + చట్రం_స్థానభ్రంశం, current_చట్రం_position - చట్రం_స్థానభ్రంశం);
					current_చట్రం_position -= చట్రం_స్థానభ్రంశం;
				}

				ఆపు;
			}
		}
	}

	ఒకవేళ (parser_state != crsf_parser_state_t::synced) {
		ఒకవేళ (current_చట్రం_position >= పరిమాణం(crsf_చట్రం_t)) {
			// discard most of the data, but keep the last 3 bytes (otherwise we could miss the frame start)
			current_చట్రం_position = 3;

			కొరకు (గుర్తులేని i = 0; i < current_చట్రం_position; ++i) {
				crsf_చట్రం_ptr[i] = crsf_చట్రం_ptr[పరిమాణం(crsf_చట్రం_t) - current_చట్రం_position + i];
			}

			CRSF_VERBOSE("Discarding buffer");
		}

		తిరిగి అబద్ధం;
	}


	ఒకవేళ (current_చట్రం_position < 3) {
		// wait until we have the header & type
		తిరిగి అబద్ధం;
	}

	// Now we have at least the header and the type

	స్థిరరాశి గుర్తులేని current_చట్రం_నిడివి = crsf_చట్రం.శీర్షిక.నిడివి + పరిమాణం(crsf_చట్రం_శీర్షిక_t);

	ఒకవేళ (current_చట్రం_నిడివి > పరిమాణం(crsf_చట్రం_t) || current_చట్రం_నిడివి < 4) {
		// frame too long or bogus -> discard everything and go into unsynced state
		current_చట్రం_position = 0;
		parser_state = crsf_parser_state_t::unsynced;
		CRSF_DEBUG("Frame too long/bogus (%i, type=%i) -> unsync", current_చట్రం_నిడివి, crsf_చట్రం.type);
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (current_చట్రం_position < current_చట్రం_నిడివి) {
		// we don't have the full frame yet -> wait for more data
		CRSF_VERBOSE("waiting for more data (%i < %i)", current_చట్రం_position, current_చట్రం_నిడివి);
		తిరిగి అబద్ధం;
	}

	తార్కిక ret = అబద్ధం;

	// Now we have the full frame

	ఒకవేళ (crsf_చట్రం.type == (uint8_t)crsf_చట్రం_type_t::rc_channels_packed &&
	    crsf_చట్రం.శీర్షిక.నిడివి == (uint8_t)crsf_payload_కొలత_t::rc_channels + 2) {
		స్థిరరాశి uint8_t చక్రీయ_తనిఖీ = crsf_చట్రం.payload[crsf_చట్రం.శీర్షిక.నిడివి - 2];

		ఒకవేళ (చక్రీయ_తనిఖీ == crsf_చట్రం_చక్రీయ_తనిఖీ(crsf_చట్రం)) {
			స్థిరరాశి crsf_payload_RC_channels_packed_t *స్థిరరాశి rc_channels =
				(crsf_payload_RC_channels_packed_t *)&crsf_చట్రం.payload;
			*అంకె_విలువలు = MIN(max_channels, 16);

			ఒకవేళ (max_channels > 0) { విలువలు_2[0] = convert_channel_విలువ(rc_channels->chan0); }

			ఒకవేళ (max_channels > 1) { విలువలు_2[1] = convert_channel_విలువ(rc_channels->chan1); }

			ఒకవేళ (max_channels > 2) { విలువలు_2[2] = convert_channel_విలువ(rc_channels->chan2); }

			ఒకవేళ (max_channels > 3) { విలువలు_2[3] = convert_channel_విలువ(rc_channels->chan3); }

			ఒకవేళ (max_channels > 4) { విలువలు_2[4] = convert_channel_విలువ(rc_channels->chan4); }

			ఒకవేళ (max_channels > 5) { విలువలు_2[5] = convert_channel_విలువ(rc_channels->chan5); }

			ఒకవేళ (max_channels > 6) { విలువలు_2[6] = convert_channel_విలువ(rc_channels->chan6); }

			ఒకవేళ (max_channels > 7) { విలువలు_2[7] = convert_channel_విలువ(rc_channels->chan7); }

			ఒకవేళ (max_channels > 8) { విలువలు_2[8] = convert_channel_విలువ(rc_channels->chan8); }

			ఒకవేళ (max_channels > 9) { విలువలు_2[9] = convert_channel_విలువ(rc_channels->chan9); }

			ఒకవేళ (max_channels > 10) { విలువలు_2[10] = convert_channel_విలువ(rc_channels->chan10); }

			ఒకవేళ (max_channels > 11) { విలువలు_2[11] = convert_channel_విలువ(rc_channels->chan11); }

			ఒకవేళ (max_channels > 12) { విలువలు_2[12] = convert_channel_విలువ(rc_channels->chan12); }

			ఒకవేళ (max_channels > 13) { విలువలు_2[13] = convert_channel_విలువ(rc_channels->chan13); }

			ఒకవేళ (max_channels > 14) { విలువలు_2[14] = convert_channel_విలువ(rc_channels->chan14); }

			ఒకవేళ (max_channels > 15) { విలువలు_2[15] = convert_channel_విలువ(rc_channels->chan15); }

			CRSF_VERBOSE("Got Channels");

			ret = నిజం;

		} లేకపోతే {
			CRSF_DEBUG("CRC check failed");
		}

	} లేకపోతే {
		CRSF_DEBUG("Got Non-RC frame (len=%i, type=%i)", current_చట్రం_నిడివి, crsf_చట్రం.type);
		// We could check the CRC here and reset the parser into unsynced state if it fails.
		// But in practise it's robust even without that.
	}

	// Either reset or move the rest of the buffer
	ఒకవేళ (current_చట్రం_position > current_చట్రం_నిడివి) {
		CRSF_VERBOSE("Moving buffer (%i > %i)", current_చట్రం_position, current_చట్రం_నిడివి);
		memmove(crsf_చట్రం_ptr, crsf_చట్రం_ptr + current_చట్రం_నిడివి, current_చట్రం_position - current_చట్రం_నిడివి);
		current_చట్రం_position -= current_చట్రం_నిడివి;

	} లేకపోతే {
		current_చట్రం_position = 0;
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
	// Big endian
	buf[స్థానభ్రంశం] = value >> 8;
	buf[స్థానభ్రంశం + 1] = value & 0xff;
	స్థానభ్రంశం += 2;
}
/**
 * write an uint24_t value to a buffer at a given offset and increment the offset
 */
స్థిర అంతఃవరుస శూన్య write_uint24_t(uint8_t *buf, పూర్ణాంకం &స్థానభ్రంశం, పూర్ణాంకం value)
{
	// Big endian
	buf[స్థానభ్రంశం] = value >> 16;
	buf[స్థానభ్రంశం + 1] = (value >> 8) & 0xff;
	buf[స్థానభ్రంశం + 2] = value & 0xff;
	స్థానభ్రంశం += 3;
}

/**
 * write an int32_t value to a buffer at a given offset and increment the offset
 */
స్థిర అంతఃవరుస శూన్య write_int32_t(uint8_t *buf, పూర్ణాంకం &స్థానభ్రంశం, int32_t value)
{
	// Big endian
	buf[స్థానభ్రంశం] = value >> 24;
	buf[స్థానభ్రంశం + 1] = (value >> 16) & 0xff;
	buf[స్థానభ్రంశం + 2] = (value >> 8) & 0xff;
	buf[స్థానభ్రంశం + 3] = value & 0xff;
	స్థానభ్రంశం += 4;
}

స్థిర అంతఃవరుస శూన్య write_చట్రం_శీర్షిక(uint8_t *buf, పూర్ణాంకం &స్థానభ్రంశం, crsf_చట్రం_type_t type, uint8_t payload_కొలత)
{
	write_uint8_t(buf, స్థానభ్రంశం, CRSF_SYNC_BYTE); // this got changed from the address to the sync byte
	write_uint8_t(buf, స్థానభ్రంశం, payload_కొలత + 2);
	write_uint8_t(buf, స్థానభ్రంశం, (uint8_t)type);
}
స్థిర అంతఃవరుస శూన్య write_చట్రం_చక్రీయ_తనిఖీ(uint8_t *buf, పూర్ణాంకం &స్థానభ్రంశం, పూర్ణాంకం buf_కొలత)
{
	// CRC does not include the address and length
	write_uint8_t(buf, స్థానభ్రంశం, crc8_dvb_s2_buf(buf + 2, buf_కొలత - 3));

	// check correctness of buffer size (only needed during development)
	//if (buf_size != offset) { PX4_ERR("frame size mismatch (%i != %i)", buf_size, offset); }
}

తార్కిక crsf_send_telemetry_battery(పూర్ణాంకం uart_fd, uint16_t voltage, uint16_t current, పూర్ణాంకం fuel, uint8_t మిగిలిన)
{
	uint8_t buf[(uint8_t)crsf_payload_కొలత_t::battery_sensor + 4];
	పూర్ణాంకం స్థానభ్రంశం = 0;
	write_చట్రం_శీర్షిక(buf, స్థానభ్రంశం, crsf_చట్రం_type_t::battery_sensor, (uint8_t)crsf_payload_కొలత_t::battery_sensor);
	write_uint16_t(buf, స్థానభ్రంశం, voltage);
	write_uint16_t(buf, స్థానభ్రంశం, current);
	write_uint24_t(buf, స్థానభ్రంశం, fuel);
	write_uint8_t(buf, స్థానభ్రంశం, మిగిలిన);
	write_చట్రం_చక్రీయ_తనిఖీ(buf, స్థానభ్రంశం, పరిమాణం(buf));
	తిరిగి లిఖ(uart_fd, buf, స్థానభ్రంశం) == స్థానభ్రంశం;
}

తార్కిక crsf_send_telemetry_gps(పూర్ణాంకం uart_fd, int32_t latitude, int32_t longitude, uint16_t groundspeed,
			     uint16_t gps_heading, uint16_t altitude, uint8_t అంకె_satellites)
{
	uint8_t buf[(uint8_t)crsf_payload_కొలత_t::gps + 4];
	పూర్ణాంకం స్థానభ్రంశం = 0;
	write_చట్రం_శీర్షిక(buf, స్థానభ్రంశం, crsf_చట్రం_type_t::gps, (uint8_t)crsf_payload_కొలత_t::gps);
	write_int32_t(buf, స్థానభ్రంశం, latitude);
	write_int32_t(buf, స్థానభ్రంశం, longitude);
	write_uint16_t(buf, స్థానభ్రంశం, groundspeed);
	write_uint16_t(buf, స్థానభ్రంశం, gps_heading);
	write_uint16_t(buf, స్థానభ్రంశం, altitude);
	write_uint8_t(buf, స్థానభ్రంశం, అంకె_satellites);
	write_చట్రం_చక్రీయ_తనిఖీ(buf, స్థానభ్రంశం, పరిమాణం(buf));
	తిరిగి లిఖ(uart_fd, buf, స్థానభ్రంశం) == స్థానభ్రంశం;
}

తార్కిక crsf_send_telemetry_baro_altitude(పూర్ణాంకం uart_fd, uint16_t altitude, int16_t vertical_వేగం)
{
	uint8_t buf[(uint8_t)crsf_payload_కొలత_t::baro_altitude + 4];
	పూర్ణాంకం స్థానభ్రంశం = 0;
	write_చట్రం_శీర్షిక(buf, స్థానభ్రంశం, crsf_చట్రం_type_t::baro_altitude, (uint8_t)crsf_payload_కొలత_t::baro_altitude);
	write_uint16_t(buf, స్థానభ్రంశం, altitude);
	write_uint16_t(buf, స్థానభ్రంశం, (uint16_t)vertical_వేగం);
	write_చట్రం_చక్రీయ_తనిఖీ(buf, స్థానభ్రంశం, పరిమాణం(buf));
	తిరిగి లిఖ(uart_fd, buf, స్థానభ్రంశం) == స్థానభ్రంశం;
}

తార్కిక crsf_send_telemetry_attitude(పూర్ణాంకం uart_fd, int16_t pitch, int16_t roll, int16_t yaw)
{
	uint8_t buf[(uint8_t)crsf_payload_కొలత_t::attitude + 4];
	పూర్ణాంకం స్థానభ్రంశం = 0;
	write_చట్రం_శీర్షిక(buf, స్థానభ్రంశం, crsf_చట్రం_type_t::attitude, (uint8_t)crsf_payload_కొలత_t::attitude);
	write_uint16_t(buf, స్థానభ్రంశం, pitch);
	write_uint16_t(buf, స్థానభ్రంశం, roll);
	write_uint16_t(buf, స్థానభ్రంశం, yaw);
	write_చట్రం_చక్రీయ_తనిఖీ(buf, స్థానభ్రంశం, పరిమాణం(buf));
	తిరిగి లిఖ(uart_fd, buf, స్థానభ్రంశం) == స్థానభ్రంశం;
}

తార్కిక crsf_send_telemetry_flight_బహుళకం(పూర్ణాంకం uart_fd, స్థిరరాశి అక్షరం *flight_బహుళకం)
{
	స్థిరరాశి పూర్ణాంకం max_నిడివి = 16;
	పూర్ణాంకం నిడివి = అక్షరశ్రేణి_పొడవు(flight_బహుళకం) + 1;

	ఒకవేళ (నిడివి > max_నిడివి) {
		నిడివి = max_నిడివి;
	}

	uint8_t buf[max_నిడివి + 4];
	పూర్ణాంకం స్థానభ్రంశం = 0;
	write_చట్రం_శీర్షిక(buf, స్థానభ్రంశం, crsf_చట్రం_type_t::flight_బహుళకం, నిడివి);
	జ్ఞాపకం_నకలు(buf + స్థానభ్రంశం, flight_బహుళకం, నిడివి);
	స్థానభ్రంశం += నిడివి;
	buf[స్థానభ్రంశం - 1] = 0; // ensure null-terminated string
	write_చట్రం_చక్రీయ_తనిఖీ(buf, స్థానభ్రంశం, నిడివి + 4);
	తిరిగి లిఖ(uart_fd, buf, స్థానభ్రంశం) == స్థానభ్రంశం;
}
