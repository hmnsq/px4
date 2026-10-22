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

#لارښوونه once

#ضمن <gtest/gtest.h>
#ضمن "correction_framer.h"
#ضمن <vector>
#ضمن <cstring>
#ضمن <numeric>
#ضمن <random>

کارول نطاق gnss;

صنف RtcmTest : عام ::testing::Test
{
محمي:
	CorrectionFramer parser;
	Gps_Rtcm_پیغام_Assembler gps_rtcm_assembler;

	uint8_t buildGpsRtcmFlags(منطقي fragmented, uint8_t fragment_id = 0, uint8_t sequence_id = 0)
	{
		uint8_t flags = fragmented ? GPS_RTCM_FLAG_FRAGMENTED : 0;
		flags |= (fragment_id & GPS_RTCM_FLAG_FRAGMENT_ID_ماسک) << GPS_RTCM_FLAG_FRAGMENT_ID_ښويدنه;
		flags |= (sequence_id & GPS_RTCM_FLAG_SEQUENCE_ID_ماسک) << GPS_RTCM_FLAG_SEQUENCE_ID_ښويدنه;
		أرجع flags;
	}

	// Helper to build a valid RTCM3 frame with proper CRC
	std::ویکټور_2<uint8_t> build_Valid_چوکاټ(uint16_t پیغام_type_2, ثابت std::ویکټور_2<uint8_t> &payload_معلومات)
	{
		// Payload = 12-bit message type + rest of payload_data
		// Message type is stored in first 12 bits of payload
		std::ویکټور_2<uint8_t> payload;
		payload.push_back((پیغام_type_2 >> 4) & 0xFF);
		payload.push_back(((پیغام_type_2 & 0x0F) << 4) | (payload_معلومات.تشه() ? 0 : (payload_معلومات[0] >> 4)));

		لكل (size_t i = 0; i < payload_معلومات.اندازه(); i++) {
			uint8_t current = (payload_معلومات[i] << 4) & 0xF0;

			إذا (i + 1 < payload_معلومات.اندازه()) {
				current |= (payload_معلومات[i + 1] >> 4) & 0x0F;
			}

			payload.push_back(current);
		}

		أرجع build_Raw_چوکاټ(payload);
	}

	// Helper to build a frame with raw payload bytes (no message type encoding)
	//
	// Keep this helper out-of-line to avoid a GCC false positive on the
	// inlined std::vector::push_back.
	// See https://github.com/PX4/PX4-Autopilot/issues/26875 for details.
	__attribute__((noinline)) std::ویکټور_2<uint8_t> build_Raw_چوکاټ(ثابت std::ویکټور_2<uint8_t> &payload)
	{
		std::ویکټور_2<uint8_t> چوکاټ;
		size_t payload_len = payload.اندازه();

		// Header: preamble + length
		چوکاټ.push_back(RTCM3_PREAMBLE);
		چوکاټ.push_back((payload_len >> 8) & 0x03);  // Upper 2 bits of length (reserved bits = 0)
		چوکاټ.push_back(payload_len & 0xFF);         // Lower 8 bits of length

		// Payload
		چوکاټ.ورننباسه(چوکاټ.پای_ټکی(), payload.begin(), payload.پای_ټکی());

		//دوراني_ازموینه
		uint32_t دوراني_ازموینه = rtcm3_crc24q(چوکاټ.معلومات(), چوکاټ.اندازه());
		چوکاټ.push_back((دوراني_ازموینه >> 16) & 0xFF);
		چوکاټ.push_back((دوراني_ازموینه >> 8) & 0xFF);
		چوکاټ.push_back(دوراني_ازموینه & 0xFF);

		أرجع چوکاټ;
	}

	// Helper to build a frame with bad CRC
	std::ویکټور_2<uint8_t> build_Bad_دوراني_ازموینه_چوکاټ(ثابت std::ویکټور_2<uint8_t> &payload)
	{
		خپلکاری چوکاټ = build_Raw_چوکاټ(payload);
		// Corrupt the CRC
		چوکاټ[چوکاټ.اندازه() - 1] ^= 0xFF;
		أرجع چوکاټ;
	}
};
