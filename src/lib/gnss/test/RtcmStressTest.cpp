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

#अन्तर्भाव "RtcmTestCommon.hpp"

TEST_F(RtcmTest, Stress_Many_Small_Valid_चौकटाः)
{
	// Simulate a good sender sending many small messages
	// Process as we go to avoid buffer overflow
	ध्रुव पूर्णाङ्क अङ्कः_चौकटाः = 1000;
	पूर्णाङ्क गणनम् = 0;

	प्रति (पूर्णाङ्क i = 0; i < अङ्कः_चौकटाः; i++) {
		स्वचालित frame = buildValidFrame(1005 + (i % 100), {static_cast<uint8_t>(i & 0xFF)});
		parser.सङ्कलय_दत्त(frame.दत्त(), frame.आकार());

		// Consume available messages after each add
		size_t len;

		यावत् (parser.get_अग्र_सन्देशः(&len) != शून्यसूचकः) {
			parser.consume_सन्देशः(len);
			गणनम्++;
		}
	}

	EXPECT_EQ(गणनम्, अङ्कः_चौकटाः);
	स्वचालित stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, static_cast<uint32_t>(अङ्कः_चौकटाः));
	EXPECT_EQ(stats.अष्टकानि_discarded_2, 0u);
	EXPECT_EQ(stats.crc_errors, 0u);
}

TEST_F(RtcmTest, Stress_Long_Garbage_प्रवाहः_Then_Valid)
{
	// This tests the pathological case Alex mentioned: lots of invalid data
	// causing many memmove operations during preamble search
	ध्रुव size_t garbage_आकार = 10000;
	std::सदिशम्_2<uint8_t> garbage(garbage_आकार);
	std::mt19937 rng(99);

	प्रति (स्वचालित &byte : garbage) {
		// Avoid every protocol preamble to ensure maximum discards: 0xD3 (RTCM3)
		// and 0x73 (SPARTN), which the framer also treats as a frame start when
		// CONFIG_GPS_SPARTN is enabled.
		byte = (rng() % 0x72) + 1;  // 0x01 to 0x72, never 0x73 or 0xD3
	}

	// Add in chunks to simulate streaming
	size_t विस्थापनम् = 0;

	यावत् (विस्थापनम् < garbage.आकार()) {
		size_t chunk = std::min<size_t>(256, garbage.आकार() - विस्थापनम्);
		parser.सङ्कलय_दत्त(&garbage[विस्थापनम्], chunk);

		// Try to parse (forces discarding)
		size_t len;
		parser.get_अग्र_सन्देशः(&len);
		विस्थापनम् += chunk;
	}

	// Now add a valid frame
	स्वचालित valid = buildValidFrame(1005, {0x01, 0x02, 0x03});
	parser.सङ्कलय_दत्त(valid.दत्त(), valid.आकार());

	size_t len;
	ध्रुव uint8_t *सन्देशः_2 = parser.get_अग्र_सन्देशः(&len);
	ASSERT_NE(सन्देशः_2, शून्यसूचकः);

	parser.consume_सन्देशः(len);
	स्वचालित stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 1u);
	EXPECT_EQ(stats.अष्टकानि_discarded_2, garbage_आकार);
}

TEST_F(RtcmTest, Stress_InterleavedGarbageAndValid)
{
	// Simulate a flaky sender that mixes garbage with valid frames
	ध्रुव पूर्णाङ्क अङ्कः_valid = 100;
	पूर्णाङ्क parsed = 0;

	प्रति (पूर्णाङ्क i = 0; i < अङ्कः_valid; i++) {
		// Add some garbage (variable amount)
		std::सदिशम्_2<uint8_t> garbage(10 + (i % 50), 0xAA);
		parser.सङ्कलय_दत्त(garbage.दत्त(), garbage.आकार());

		// Add valid frame
		स्वचालित frame = buildValidFrame(1005, {static_cast<uint8_t>(i)});
		parser.सङ्कलय_दत्त(frame.दत्त(), frame.आकार());

		// Parse what we can
		size_t len;

		यावत् (parser.get_अग्र_सन्देशः(&len) != शून्यसूचकः) {
			parser.consume_सन्देशः(len);
			parsed++;
		}
	}

	EXPECT_EQ(parsed, अङ्कः_valid);
}
