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

#అన్తర్భావ "RtcmTestCommon.hpp"

TEST_F(RtcmTest, Stress_Many_Small_Valid_చట్రాలు)
{
	// Simulate a good sender sending many small messages
	// Process as we go to avoid buffer overflow
	స్థిరరాశి పూర్ణాంకం అంకె_చట్రాలు = 1000;
	పూర్ణాంకం లెక్క = 0;

	కొరకు (పూర్ణాంకం i = 0; i < అంకె_చట్రాలు; i++) {
		స్వయంచాలక చట్రం = build_Valid_చట్రం(1005 + (i % 100), {static_cast<uint8_t>(i & 0xFF)});
		parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

		// Consume available messages after each add
		size_t len;

		అయితే (parser.get_తదుపరి_సందేశం(&len) != శూన్యసూచిక) {
			parser.consume_సందేశం(len);
			లెక్క++;
		}
	}

	EXPECT_EQ(లెక్క, అంకె_చట్రాలు);
	స్వయంచాలక stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, static_cast<uint32_t>(అంకె_చట్రాలు));
	EXPECT_EQ(stats.బైట్లు_discarded_2, 0u);
	EXPECT_EQ(stats.చక్రీయ_తనిఖీ_errors_2, 0u);
}

TEST_F(RtcmTest, Stress_Long_Garbage_ప్రవాహం_Then_Valid)
{
	// This tests the pathological case Alex mentioned: lots of invalid data
	// causing many memmove operations during preamble search
	స్థిరరాశి size_t garbage_కొలత = 10000;
	std::సదిశ_2<uint8_t> garbage(garbage_కొలత);
	std::mt19937 rng(99);

	కొరకు (స్వయంచాలక &byte : garbage) {
		// Avoid every protocol preamble to ensure maximum discards: 0xD3 (RTCM3)
		// and 0x73 (SPARTN), which the framer also treats as a frame start when
		// CONFIG_GPS_SPARTN is enabled.
		byte = (rng() % 0x72) + 1;  // 0x01 to 0x72, never 0x73 or 0xD3
	}

	// Add in chunks to simulate streaming
	size_t స్థానభ్రంశం = 0;

	అయితే (స్థానభ్రంశం < garbage.కొలత()) {
		size_t chunk = std::min<size_t>(256, garbage.కొలత() - స్థానభ్రంశం);
		parser.కూడు_దత్తాంశం(&garbage[స్థానభ్రంశం], chunk);

		// Try to parse (forces discarding)
		size_t len;
		parser.get_తదుపరి_సందేశం(&len);
		స్థానభ్రంశం += chunk;
	}

	// Now add a valid frame
	స్వయంచాలక valid = build_Valid_చట్రం(1005, {0x01, 0x02, 0x03});
	parser.కూడు_దత్తాంశం(valid.దత్తాంశం(), valid.కొలత());

	size_t len;
	స్థిరరాశి uint8_t *సందేశం_2 = parser.get_తదుపరి_సందేశం(&len);
	ASSERT_NE(సందేశం_2, శూన్యసూచిక);

	parser.consume_సందేశం(len);
	స్వయంచాలక stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 1u);
	EXPECT_EQ(stats.బైట్లు_discarded_2, garbage_కొలత);
}

TEST_F(RtcmTest, Stress_InterleavedGarbageAndValid)
{
	// Simulate a flaky sender that mixes garbage with valid frames
	స్థిరరాశి పూర్ణాంకం అంకె_valid = 100;
	పూర్ణాంకం parsed = 0;

	కొరకు (పూర్ణాంకం i = 0; i < అంకె_valid; i++) {
		// Add some garbage (variable amount)
		std::సదిశ_2<uint8_t> garbage(10 + (i % 50), 0xAA);
		parser.కూడు_దత్తాంశం(garbage.దత్తాంశం(), garbage.కొలత());

		// Add valid frame
		స్వయంచాలక చట్రం = build_Valid_చట్రం(1005, {static_cast<uint8_t>(i)});
		parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

		// Parse what we can
		size_t len;

		అయితే (parser.get_తదుపరి_సందేశం(&len) != శూన్యసూచిక) {
			parser.consume_సందేశం(len);
			parsed++;
		}
	}

	EXPECT_EQ(parsed, అంకె_valid);
}
