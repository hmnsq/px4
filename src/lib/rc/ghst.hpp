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
 * @file ghst.hpp
 *
 * RC protocol definition for IRC Ghost (Immersion RC Ghost).
 *
 * @author Igor Misic <igy1000mb@gmail.com>
 * @author Juraj Ciberlin <jciberlin1@gmail.com>
 */

#pragma once

#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <px4_platform_common/defines.h>

__BEGIN_DECLS

#define GHST_BAUDRATE		(420000u)
#define GHST_PAYLOAD_MAX_SIZE	(14u)

എണ്ണൽ വർഗ്ഗം ghstAddress {
	rxAddress = 0x89	// Rx address
};

എണ്ണൽ വർഗ്ഗം ghstFrameType {
	frameTypeFirst	= 0x10,	// first frame type
	frameType5to8	= 0x10,	// channel 5-8
	frameType9to12	= 0x11,	// channel 9-12
	frameType13to16 = 0x12,	// channel 13-16
	frameTypeRssi	= 0x13,	// RSSI frame type, contains LQ, RSSI, RF mode, Tx power
	frameTypeLast	= 0x1f	// last frame type
};

എണ്ണൽ വർഗ്ഗം ghstTelemetryType {
	batteryPack = 0x23,	// battery pack status frame type
	gpsPrimary = 0x25,	// GPS primary data (lat/long/alt)
	gpsSecondary = 0x26	// GPS secondary data (course, dist, flags)
};

ഘടന ghst_frame_header_t {
	uint8_t device_address;			// device address
	uint8_t length;				// length
};

ഘടന ghst_frame_t {
	ghst_frame_header_t header;			// header
	uint8_t type;					// frame type
	uint8_t payload[GHST_PAYLOAD_MAX_SIZE + 1U];	// payload data including 1 byte CRC at the end
};

// Channel data (1-4)
തരനിർവചനം ഘടന {
	unsigned പൂർണ്ണസംഖ്യ chan1: 12;
	unsigned പൂർണ്ണസംഖ്യ chan2: 12;
	unsigned പൂർണ്ണസംഖ്യ chan3: 12;
	unsigned പൂർണ്ണസംഖ്യ chan4: 12;
} __attribute__((__packed__)) ghstChannelData_t;

// Payload data
തരനിർവചനം ഘടന {
	ghstChannelData_t chan1to4;
	unsigned പൂർണ്ണസംഖ്യ chanA: 8;
	unsigned പൂർണ്ണസംഖ്യ chanB: 8;
	unsigned പൂർണ്ണസംഖ്യ chanC: 8;
	unsigned പൂർണ്ണസംഖ്യ chanD: 8;
} __attribute__((__packed__)) ghstPayloadData_t;

// Payload data - RSSI frame type
തരനിർവചനം ഘടന {
	ghstChannelData_t chan1to4;
	unsigned പൂർണ്ണസംഖ്യ lq: 8;		// link quality
	unsigned പൂർണ്ണസംഖ്യ rssidBm: 8;	// RSSI [dBm]
	unsigned പൂർണ്ണസംഖ്യ rfMode: 8;		// RF mode
	പൂർണ്ണസംഖ്യ txPowerdBm: 8;		// Tx power [dBm]
} __attribute__((__packed__)) ghstPayloadRssi_t;

// Link statistics for internal transport
തരനിർവചനം ഘടന {
	int8_t rssi_pct;
	ദശാംശം rssi_dbm;
	int8_t link_quality;
} ghstLinkStatistics_t;

/**
 * Configure an UART port to be used for GHST
 * @param uart_fd UART file descriptor
 * @return 0 on success, -errno otherwise
 */
__EXPORT പൂർണ്ണസംഖ്യ ghst_config(പൂർണ്ണസംഖ്യ uart_fd);


/**
 * Parse the GHST protocol and extract RC channel data.
 *
 * @param now current time
 * @param frame data to parse
 * @param len length of frame
 * @param values output channel values
 * @param rssi received signal strength indicator
 * @param num_values set to the number of parsed channels in values
 * @param max_channels maximum length of values
 * @return true if channels successfully decoded
 */
__EXPORT തർക്കം ghst_parse(സ്ഥിരം uint64_t now, സ്ഥിരം uint8_t *frame, unsigned len, uint16_t *മൂല്യങ്ങൾ_2,
			 ghstLinkStatistics_t *link_stats, uint16_t *അക്കം_മൂല്യങ്ങൾ, uint16_t max_channels);


/**
 * Send telemetry battery information
 * @param uart_fd UART file descriptor
 * @param voltage_in_10mV Voltage [10 mV]
 * @param current_in_10mA Current [10 mA]
 * @param fuel_in_10mAh Fuel [10 mAh]
 * @return true on success
 */
__EXPORT തർക്കം ghst_send_telemetry_battery_status(പൂർണ്ണസംഖ്യ uart_fd, uint16_t voltage_in_10mV,
		uint16_t current_in_10mA, uint16_t fuel_in_10mAh);

/**
 * Send primary GPS information
 * @param uart_fd UART file descriptor
 * @param latitude GPS latitude [1e-7 degrees]
 * @param longitude GPS longitude [1e-7 degrees]
 * @param altitude GPS altitude [1m]
 * @return true on success
 */
__EXPORT തർക്കം ghst_send_telemetry_gps1_status(പൂർണ്ണസംഖ്യ uart_fd, uint32_t latitude, uint32_t longitude, uint16_t altitude);

/**
 * Send secondary GPS information
 * @param uart_fd UART file descriptor
 * @param ground_speed Ground Speed [1 km/h]
 * @param ground_course Ground Course [1e-7 degrees]
 * @param num_sats GPS Satellite count
 * @param home_dist Distance to Home [10 m]
 * @param home_dir Direction to Home [1e-7 degrees]
 * @param flags GPS Flags
 * @return true on success
 */
__EXPORT തർക്കം ghst_send_telemetry_gps2_status(പൂർണ്ണസംഖ്യ uart_fd, uint16_t ground_speed, uint16_t ground_course,
		uint8_t അക്കം_sats, uint16_t home_dist, uint16_t home_dir, uint8_t flags);
__END_DECLS
