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

#ہدایت once

#شامل <gtest/gtest.h>
#شامل "correction_framer.h"
#شامل <vector>
#شامل <cstring>
#شامل <numeric>
#شامل <random>

استعمال نامخلا gnss;

جماعت RtcmTest : عوامی ::testing::Test
{
محفوظ:
	CorrectionFramer parser;
	Gps_Rtcm_پیغام_Assembler gps_rtcm_assembler;

	uint8_t buildGpsRtcmFlags(منطقی fragmented, uint8_t fragment_id = 0, uint8_t sequence_id = 0)
	{
		uint8_t flags = fragmented ? GPS_RTCM_FLAG_FRAGMENTED : 0;
		flags |= (fragment_id & GPS_RTCM_FLAG_FRAGMENT_ID_نقاب) << GPS_RTCM_FLAG_FRAGMENT_ID_سرکاؤ;
		flags |= (sequence_id & GPS_RTCM_FLAG_SEQUENCE_ID_نقاب) << GPS_RTCM_FLAG_SEQUENCE_ID_سرکاؤ;
		واپس flags;
	}

	// Helper to build a valid RTCM3 frame with proper CRC
	std::سمتیہ_2<uint8_t> build_Valid_چوکھٹا(uint16_t پیغام_type_2, ثابت std::سمتیہ_2<uint8_t> &payload_ڈیٹا)
	{
		// Payload = 12-bit message type + rest of payload_data
		// Message type is stored in first 12 bits of payload
		std::سمتیہ_2<uint8_t> payload;
		payload.push_back((پیغام_type_2 >> 4) & 0xFF);
		payload.push_back(((پیغام_type_2 & 0x0F) << 4) | (payload_ڈیٹا.خالی_خانے() ? 0 : (payload_ڈیٹا[0] >> 4)));

		برائے (size_t i = 0; i < payload_ڈیٹا.سائز(); i++) {
			uint8_t current = (payload_ڈیٹا[i] << 4) & 0xF0;

			اگر (i + 1 < payload_ڈیٹا.سائز()) {
				current |= (payload_ڈیٹا[i + 1] >> 4) & 0x0F;
			}

			payload.push_back(current);
		}

		واپس build_Raw_چوکھٹا(payload);
	}

	// Helper to build a frame with raw payload bytes (no message type encoding)
	//
	// Keep this helper out-of-line to avoid a GCC false positive on the
	// inlined std::vector::push_back.
	// See https://github.com/PX4/PX4-Autopilot/issues/26875 for details.
	__attribute__((noinline)) std::سمتیہ_2<uint8_t> build_Raw_چوکھٹا(ثابت std::سمتیہ_2<uint8_t> &payload)
	{
		std::سمتیہ_2<uint8_t> چوکھٹا;
		size_t payload_len = payload.سائز();

		// Header: preamble + length
		چوکھٹا.push_back(RTCM3_PREAMBLE);
		چوکھٹا.push_back((payload_len >> 8) & 0x03);  // Upper 2 bits of length (reserved bits = 0)
		چوکھٹا.push_back(payload_len & 0xFF);         // Lower 8 bits of length

		// Payload
		چوکھٹا.داخل_کرو(چوکھٹا.انتہا(), payload.begin(), payload.انتہا());

		//چکری_جانچ
		uint32_t چکری_جانچ = rtcm3_crc24q(چوکھٹا.ڈیٹا(), چوکھٹا.سائز());
		چوکھٹا.push_back((چکری_جانچ >> 16) & 0xFF);
		چوکھٹا.push_back((چکری_جانچ >> 8) & 0xFF);
		چوکھٹا.push_back(چکری_جانچ & 0xFF);

		واپس چوکھٹا;
	}

	// Helper to build a frame with bad CRC
	std::سمتیہ_2<uint8_t> build_Bad_چکری_جانچ_چوکھٹا(ثابت std::سمتیہ_2<uint8_t> &payload)
	{
		خودکار چوکھٹا = build_Raw_چوکھٹا(payload);
		// Corrupt the CRC
		چوکھٹا[چوکھٹا.سائز() - 1] ^= 0xFF;
		واپس چوکھٹا;
	}
};
