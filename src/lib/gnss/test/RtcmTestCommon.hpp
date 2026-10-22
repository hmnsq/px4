/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

#هدايت once

#ضمن <gtest/gtest.h>
#ضمن "correction_framer.h"
#ضمن <vector>
#ضمن <cstring>
#ضمن <numeric>
#ضمن <random>

استعمال نطاق gnss;

صنف RtcmTest : عام ::testing::Test
{
محمي:
	CorrectionFramer parser;
	Gps_Rtcm_پيغام_Assembler gps_rtcm_assembler;

	uint8_t buildGpsRtcmFlags(منطقي fragmented, uint8_t fragment_id = 0, uint8_t sequence_id = 0)
	{
		uint8_t flags = fragmented ? GPS_RTCM_FLAG_FRAGMENTED : 0;
		flags |= (fragment_id & GPS_RTCM_FLAG_FRAGMENT_ID_نقاب) << GPS_RTCM_FLAG_FRAGMENT_ID_سرڪايو;
		flags |= (sequence_id & GPS_RTCM_FLAG_SEQUENCE_ID_نقاب) << GPS_RTCM_FLAG_SEQUENCE_ID_سرڪايو;
		أرجع flags;
	}

	// Helper to build a valid RTCM3 frame with proper CRC
	std::ويڪٽر_2<uint8_t> build_Valid_چوکٺو(uint16_t نياپو_type, ثابت std::ويڪٽر_2<uint8_t> &payload_معلومات)
	{
		// Payload = 12-bit message type + rest of payload_data
		// Message type is stored in first 12 bits of payload
		std::ويڪٽر_2<uint8_t> payload;
		payload.push_back((نياپو_type >> 4) & 0xFF);
		payload.push_back(((نياپو_type & 0x0F) << 4) | (payload_معلومات.خالي_جايون() ? 0 : (payload_معلومات[0] >> 4)));

		لكل (size_t i = 0; i < payload_معلومات.سائيز(); i++) {
			uint8_t current = (payload_معلومات[i] << 4) & 0xF0;

			إذا (i + 1 < payload_معلومات.سائيز()) {
				current |= (payload_معلومات[i + 1] >> 4) & 0x0F;
			}

			payload.push_back(current);
		}

		أرجع build_Raw_چوکٺو(payload);
	}

	// Helper to build a frame with raw payload bytes (no message type encoding)
	//
	// Keep this helper out-of-line to avoid a GCC false positive on the
	// inlined std::vector::push_back.
	// See https://github.com/PX4/PX4-Autopilot/issues/26875 for details.
	__attribute__((noinline)) std::ويڪٽر_2<uint8_t> build_Raw_چوکٺو(ثابت std::ويڪٽر_2<uint8_t> &payload)
	{
		std::ويڪٽر_2<uint8_t> چوکٺو;
		size_t payload_len = payload.سائيز();

		// Header: preamble + length
		چوکٺو.push_back(RTCM3_PREAMBLE);
		چوکٺو.push_back((payload_len >> 8) & 0x03);  // Upper 2 bits of length (reserved bits = 0)
		چوکٺو.push_back(payload_len & 0xFF);         // Lower 8 bits of length

		// Payload
		چوکٺو.وجهو(چوکٺو.پڇاڙي(), payload.begin(), payload.پڇاڙي());

		//چڪري_جاچ
		uint32_t چڪري_جاچ = rtcm3_crc24q(چوکٺو.معلومات(), چوکٺو.سائيز());
		چوکٺو.push_back((چڪري_جاچ >> 16) & 0xFF);
		چوکٺو.push_back((چڪري_جاچ >> 8) & 0xFF);
		چوکٺو.push_back(چڪري_جاچ & 0xFF);

		أرجع چوکٺو;
	}

	// Helper to build a frame with bad CRC
	std::ويڪٽر_2<uint8_t> build_Bad_چڪري_جاچ_چوکٺو(ثابت std::ويڪٽر_2<uint8_t> &payload)
	{
		خودڪار چوکٺو = build_Raw_چوکٺو(payload);
		// Corrupt the CRC
		چوکٺو[چوکٺو.سائيز() - 1] ^= 0xFF;
		أرجع چوکٺو;
	}
};
