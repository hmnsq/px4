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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "correction_framer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <vector>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>
#ਅਨ੍ਤਰ੍ਭਾਵ <numeric>
#ਅਨ੍ਤਰ੍ਭਾਵ <random>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ gnss;

ਵਰਗ RtcmTest : ਜਨਤਕ ::testing::Test
{
ਸੁਰੱਖਿਅਤ:
	CorrectionFramer parser;
	Gps_Rtcm_ਸੁਨੇਹਾ_Assembler gps_rtcm_assembler;

	uint8_t buildGpsRtcmFlags(ਤਾਰਕਿਕ fragmented, uint8_t fragment_id = 0, uint8_t sequence_id = 0)
	{
		uint8_t flags = fragmented ? GPS_RTCM_FLAG_FRAGMENTED : 0;
		flags |= (fragment_id & GPS_RTCM_FLAG_FRAGMENT_ID_ਨਕਾਬ) << GPS_RTCM_FLAG_FRAGMENT_ID_ਖਿਸਕਾਓ;
		flags |= (sequence_id & GPS_RTCM_FLAG_SEQUENCE_ID_ਨਕਾਬ) << GPS_RTCM_FLAG_SEQUENCE_ID_ਖਿਸਕਾਓ;
		ਵਾਪਸ flags;
	}

	// Helper to build a valid RTCM3 frame with proper CRC
	std::ਸਦਿਸ਼_2<uint8_t> build_Valid_ਚੌਖਟਾ(uint16_t ਸੁਨੇਹਾ_type_2, ਅਚਲ std::ਸਦਿਸ਼_2<uint8_t> &payload_ਅੰਕੜੇ)
	{
		// Payload = 12-bit message type + rest of payload_data
		// Message type is stored in first 12 bits of payload
		std::ਸਦਿਸ਼_2<uint8_t> payload;
		payload.push_back((ਸੁਨੇਹਾ_type_2 >> 4) & 0xFF);
		payload.push_back(((ਸੁਨੇਹਾ_type_2 & 0x0F) << 4) | (payload_ਅੰਕੜੇ.ਖਾਲੀ_ਥਾਵਾਂ() ? 0 : (payload_ਅੰਕੜੇ[0] >> 4)));

		ਲਈ (size_t i = 0; i < payload_ਅੰਕੜੇ.ਮਾਪ(); i++) {
			uint8_t current = (payload_ਅੰਕੜੇ[i] << 4) & 0xF0;

			ਜੇ (i + 1 < payload_ਅੰਕੜੇ.ਮਾਪ()) {
				current |= (payload_ਅੰਕੜੇ[i + 1] >> 4) & 0x0F;
			}

			payload.push_back(current);
		}

		ਵਾਪਸ build_Raw_ਚੌਖਟਾ(payload);
	}

	// Helper to build a frame with raw payload bytes (no message type encoding)
	//
	// Keep this helper out-of-line to avoid a GCC false positive on the
	// inlined std::vector::push_back.
	// See https://github.com/PX4/PX4-Autopilot/issues/26875 for details.
	__attribute__((noinline)) std::ਸਦਿਸ਼_2<uint8_t> build_Raw_ਚੌਖਟਾ(ਅਚਲ std::ਸਦਿਸ਼_2<uint8_t> &payload)
	{
		std::ਸਦਿਸ਼_2<uint8_t> ਚੌਖਟਾ;
		size_t payload_len = payload.ਮਾਪ();

		// Header: preamble + length
		ਚੌਖਟਾ.push_back(RTCM3_PREAMBLE);
		ਚੌਖਟਾ.push_back((payload_len >> 8) & 0x03);  // Upper 2 bits of length (reserved bits = 0)
		ਚੌਖਟਾ.push_back(payload_len & 0xFF);         // Lower 8 bits of length

		// Payload
		ਚੌਖਟਾ.ਪਾਓ(ਚੌਖਟਾ.ਸਿਰਾ(), payload.begin(), payload.ਸਿਰਾ());

		//ਚੱਕਰੀ_ਜਾਂਚ
		uint32_t ਚੱਕਰੀ_ਜਾਂਚ = rtcm3_crc24q(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());
		ਚੌਖਟਾ.push_back((ਚੱਕਰੀ_ਜਾਂਚ >> 16) & 0xFF);
		ਚੌਖਟਾ.push_back((ਚੱਕਰੀ_ਜਾਂਚ >> 8) & 0xFF);
		ਚੌਖਟਾ.push_back(ਚੱਕਰੀ_ਜਾਂਚ & 0xFF);

		ਵਾਪਸ ਚੌਖਟਾ;
	}

	// Helper to build a frame with bad CRC
	std::ਸਦਿਸ਼_2<uint8_t> build_Bad_ਚੱਕਰੀ_ਜਾਂਚ_ਚੌਖਟਾ(ਅਚਲ std::ਸਦਿਸ਼_2<uint8_t> &payload)
	{
		ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_Raw_ਚੌਖਟਾ(payload);
		// Corrupt the CRC
		ਚੌਖਟਾ[ਚੌਖਟਾ.ਮਾਪ() - 1] ^= 0xFF;
		ਵਾਪਸ ਚੌਖਟਾ;
	}
};
