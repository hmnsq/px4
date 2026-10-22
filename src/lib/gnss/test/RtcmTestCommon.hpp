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

#సూచన once

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ "correction_framer.h"
#అన్తర్భావ <vector>
#అన్తర్భావ <cstring>
#అన్తర్భావ <numeric>
#అన్తర్భావ <random>

ఉపయోగించి నామాకాశ gnss;

తరగతి RtcmTest : బహిరంగ ::testing::Test
{
రక్షిత:
	CorrectionFramer parser;
	Gps_Rtcm_సందేశం_Assembler gps_rtcm_assembler;

	uint8_t buildGpsRtcmFlags(తార్కిక fragmented, uint8_t fragment_id = 0, uint8_t sequence_id = 0)
	{
		uint8_t flags = fragmented ? GPS_RTCM_FLAG_FRAGMENTED : 0;
		flags |= (fragment_id & GPS_RTCM_FLAG_FRAGMENT_ID_ముసుగు) << GPS_RTCM_FLAG_FRAGMENT_ID_జరుపు;
		flags |= (sequence_id & GPS_RTCM_FLAG_SEQUENCE_ID_ముసుగు) << GPS_RTCM_FLAG_SEQUENCE_ID_జరుపు;
		తిరిగి flags;
	}

	// Helper to build a valid RTCM3 frame with proper CRC
	std::సదిశ_2<uint8_t> build_Valid_చట్రం(uint16_t సందేశం_type_2, స్థిరరాశి std::సదిశ_2<uint8_t> &payload_దత్తాంశం)
	{
		// Payload = 12-bit message type + rest of payload_data
		// Message type is stored in first 12 bits of payload
		std::సదిశ_2<uint8_t> payload;
		payload.push_back((సందేశం_type_2 >> 4) & 0xFF);
		payload.push_back(((సందేశం_type_2 & 0x0F) << 4) | (payload_దత్తాంశం.ఖాళీలు() ? 0 : (payload_దత్తాంశం[0] >> 4)));

		కొరకు (size_t i = 0; i < payload_దత్తాంశం.కొలత(); i++) {
			uint8_t current = (payload_దత్తాంశం[i] << 4) & 0xF0;

			ఒకవేళ (i + 1 < payload_దత్తాంశం.కొలత()) {
				current |= (payload_దత్తాంశం[i + 1] >> 4) & 0x0F;
			}

			payload.push_back(current);
		}

		తిరిగి build_Raw_చట్రం(payload);
	}

	// Helper to build a frame with raw payload bytes (no message type encoding)
	//
	// Keep this helper out-of-line to avoid a GCC false positive on the
	// inlined std::vector::push_back.
	// See https://github.com/PX4/PX4-Autopilot/issues/26875 for details.
	__attribute__((noinline)) std::సదిశ_2<uint8_t> build_Raw_చట్రం(స్థిరరాశి std::సదిశ_2<uint8_t> &payload)
	{
		std::సదిశ_2<uint8_t> చట్రం;
		size_t payload_len = payload.కొలత();

		// Header: preamble + length
		చట్రం.push_back(RTCM3_PREAMBLE);
		చట్రం.push_back((payload_len >> 8) & 0x03);  // Upper 2 bits of length (reserved bits = 0)
		చట్రం.push_back(payload_len & 0xFF);         // Lower 8 bits of length

		// Payload
		చట్రం.చొప్పించు(చట్రం.చివర(), payload.begin(), payload.చివర());

		//చక్రీయ_తనిఖీ
		uint32_t చక్రీయ_తనిఖీ = rtcm3_crc24q(చట్రం.దత్తాంశం(), చట్రం.కొలత());
		చట్రం.push_back((చక్రీయ_తనిఖీ >> 16) & 0xFF);
		చట్రం.push_back((చక్రీయ_తనిఖీ >> 8) & 0xFF);
		చట్రం.push_back(చక్రీయ_తనిఖీ & 0xFF);

		తిరిగి చట్రం;
	}

	// Helper to build a frame with bad CRC
	std::సదిశ_2<uint8_t> build_Bad_చక్రీయ_తనిఖీ_చట్రం(స్థిరరాశి std::సదిశ_2<uint8_t> &payload)
	{
		స్వయంచాలక చట్రం = build_Raw_చట్రం(payload);
		// Corrupt the CRC
		చట్రం[చట్రం.కొలత() - 1] ^= 0xFF;
		తిరిగి చట్రం;
	}
};
