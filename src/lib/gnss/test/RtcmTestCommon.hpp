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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ "correction_framer.h"
#ಅನ್ತರ್ಭಾವ <vector>
#ಅನ್ತರ್ಭಾವ <cstring>
#ಅನ್ತರ್ಭಾವ <numeric>
#ಅನ್ತರ್ಭಾವ <random>

ಬಳಸಿ ನಾಮಾಕಾಶ gnss;

ವರ್ಗ RtcmTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಂರಕ್ಷಿತ:
	CorrectionFramer parser;
	Gps_Rtcm_ಸಂದೇಶ_Assembler gps_rtcm_assembler;

	uint8_t buildGpsRtcmFlags(ತಾರ್ಕಿಕ fragmented, uint8_t fragment_id = 0, uint8_t sequence_id = 0)
	{
		uint8_t flags = fragmented ? GPS_RTCM_FLAG_FRAGMENTED : 0;
		flags |= (fragment_id & GPS_RTCM_FLAG_FRAGMENT_ID_ಮುಖವಾಡ) << GPS_RTCM_FLAG_FRAGMENT_ID_ಸರಿಸು;
		flags |= (sequence_id & GPS_RTCM_FLAG_SEQUENCE_ID_ಮುಖವಾಡ) << GPS_RTCM_FLAG_SEQUENCE_ID_ಸರಿಸು;
		ಹಿಂತಿರುಗಿ flags;
	}

	// Helper to build a valid RTCM3 frame with proper CRC
	std::ಸದಿಶ_2<uint8_t> build_Valid_ಚೌಕಟ್ಟು(uint16_t ಸಂದೇಶ_type_2, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ_2<uint8_t> &payload_ದತ್ತಾಂಶ)
	{
		// Payload = 12-bit message type + rest of payload_data
		// Message type is stored in first 12 bits of payload
		std::ಸದಿಶ_2<uint8_t> payload;
		payload.push_back((ಸಂದೇಶ_type_2 >> 4) & 0xFF);
		payload.push_back(((ಸಂದೇಶ_type_2 & 0x0F) << 4) | (payload_ದತ್ತಾಂಶ.ಖಾಲಿಗಳು() ? 0 : (payload_ದತ್ತಾಂಶ[0] >> 4)));

		ಗಾಗಿ (size_t i = 0; i < payload_ದತ್ತಾಂಶ.ಗಾತ್ರ_2(); i++) {
			uint8_t current = (payload_ದತ್ತಾಂಶ[i] << 4) & 0xF0;

			ಆದರೆ (i + 1 < payload_ದತ್ತಾಂಶ.ಗಾತ್ರ_2()) {
				current |= (payload_ದತ್ತಾಂಶ[i + 1] >> 4) & 0x0F;
			}

			payload.push_back(current);
		}

		ಹಿಂತಿರುಗಿ build_Raw_ಚೌಕಟ್ಟು(payload);
	}

	// Helper to build a frame with raw payload bytes (no message type encoding)
	//
	// Keep this helper out-of-line to avoid a GCC false positive on the
	// inlined std::vector::push_back.
	// See https://github.com/PX4/PX4-Autopilot/issues/26875 for details.
	__attribute__((noinline)) std::ಸದಿಶ_2<uint8_t> build_Raw_ಚೌಕಟ್ಟು(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ_2<uint8_t> &payload)
	{
		std::ಸದಿಶ_2<uint8_t> ಚೌಕಟ್ಟು;
		size_t payload_len = payload.ಗಾತ್ರ_2();

		// Header: preamble + length
		ಚೌಕಟ್ಟು.push_back(RTCM3_PREAMBLE);
		ಚೌಕಟ್ಟು.push_back((payload_len >> 8) & 0x03);  // Upper 2 bits of length (reserved bits = 0)
		ಚೌಕಟ್ಟು.push_back(payload_len & 0xFF);         // Lower 8 bits of length

		// Payload
		ಚೌಕಟ್ಟು.ಸೇರಿಸು(ಚೌಕಟ್ಟು.ಕೊನೆ(), payload.begin(), payload.ಕೊನೆ());

		//ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ
		uint32_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = rtcm3_crc24q(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());
		ಚೌಕಟ್ಟು.push_back((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ >> 16) & 0xFF);
		ಚೌಕಟ್ಟು.push_back((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ >> 8) & 0xFF);
		ಚೌಕಟ್ಟು.push_back(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ & 0xFF);

		ಹಿಂತಿರುಗಿ ಚೌಕಟ್ಟು;
	}

	// Helper to build a frame with bad CRC
	std::ಸದಿಶ_2<uint8_t> build_Bad_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಚೌಕಟ್ಟು(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ_2<uint8_t> &payload)
	{
		ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_Raw_ಚೌಕಟ್ಟು(payload);
		// Corrupt the CRC
		ಚೌಕಟ್ಟು[ಚೌಕಟ್ಟು.ಗಾತ್ರ_2() - 1] ^= 0xFF;
		ಹಿಂತಿರುಗಿ ಚೌಕಟ್ಟು;
	}
};
