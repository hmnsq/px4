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

#निर्देशः once

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव "correction_framer.h"
#अन्तर्भाव <vector>
#अन्तर्भाव <cstring>
#अन्तर्भाव <numeric>
#अन्तर्भाव <random>

उपयुज्य नामाकाश gnss;

वर्ग RtcmTest : सार्वजनिक ::testing::Test
{
रक्षित:
	CorrectionFramer parser;
	Gps_Rtcm_सन्देशः_Assembler gps_rtcm_assembler;

	uint8_t buildGpsRtcmFlags(तार्किक fragmented, uint8_t fragment_id = 0, uint8_t sequence_id = 0)
	{
		uint8_t flags = fragmented ? GPS_RTCM_FLAG_FRAGMENTED : 0;
		flags |= (fragment_id & GPS_RTCM_FLAG_FRAGMENT_ID_आवरणम्) << GPS_RTCM_FLAG_FRAGMENT_ID_सरणम्;
		flags |= (sequence_id & GPS_RTCM_FLAG_SEQUENCE_ID_आवरणम्) << GPS_RTCM_FLAG_SEQUENCE_ID_सरणम्;
		प्रत्यागम flags;
	}

	// Helper to build a valid RTCM3 frame with proper CRC
	std::सदिशम्_2<uint8_t> buildValidFrame(uint16_t सन्देशः_type_2, ध्रुव std::सदिशम्_2<uint8_t> &payload_दत्त)
	{
		// Payload = 12-bit message type + rest of payload_data
		// Message type is stored in first 12 bits of payload
		std::सदिशम्_2<uint8_t> payload;
		payload.push_back((सन्देशः_type_2 >> 4) & 0xFF);
		payload.push_back(((सन्देशः_type_2 & 0x0F) << 4) | (payload_दत्त.रिक्तानि() ? 0 : (payload_दत्त[0] >> 4)));

		प्रति (size_t i = 0; i < payload_दत्त.आकार(); i++) {
			uint8_t current = (payload_दत्त[i] << 4) & 0xF0;

			यदि (i + 1 < payload_दत्त.आकार()) {
				current |= (payload_दत्त[i + 1] >> 4) & 0x0F;
			}

			payload.push_back(current);
		}

		प्रत्यागम buildRawFrame(payload);
	}

	// Helper to build a frame with raw payload bytes (no message type encoding)
	//
	// Keep this helper out-of-line to avoid a GCC false positive on the
	// inlined std::vector::push_back.
	// See https://github.com/PX4/PX4-Autopilot/issues/26875 for details.
	__attribute__((noinline)) std::सदिशम्_2<uint8_t> buildRawFrame(ध्रुव std::सदिशम्_2<uint8_t> &payload)
	{
		std::सदिशम्_2<uint8_t> frame;
		size_t payload_len = payload.आकार();

		// Header: preamble + length
		frame.push_back(RTCM3_PREAMBLE);
		frame.push_back((payload_len >> 8) & 0x03);  // Upper 2 bits of length (reserved bits = 0)
		frame.push_back(payload_len & 0xFF);         // Lower 8 bits of length

		// Payload
		frame.निवेशय(frame.अन्तः_2(), payload.begin(), payload.अन्तः_2());

		// CRC
		uint32_t crc = rtcm3_crc24q(frame.दत्त(), frame.आकार());
		frame.push_back((crc >> 16) & 0xFF);
		frame.push_back((crc >> 8) & 0xFF);
		frame.push_back(crc & 0xFF);

		प्रत्यागम frame;
	}

	// Helper to build a frame with bad CRC
	std::सदिशम्_2<uint8_t> buildBadCrcFrame(ध्रुव std::सदिशम्_2<uint8_t> &payload)
	{
		स्वचालित frame = buildRawFrame(payload);
		// Corrupt the CRC
		frame[frame.आकार() - 1] ^= 0xFF;
		प्रत्यागम frame;
	}
};
