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

#অন্তর্ভাব "RtcmTestCommon.hpp"

TEST_F(RtcmTest, Stress_ManySmallValidFrames)
{
	// Simulate a good sender sending many small messages
	// Process as we go to avoid buffer overflow
	ধ্রুবক পূর্ণসংখ্যা অঙ্ক_frames = 1000;
	পূর্ণসংখ্যা গুনতি = 0;

	জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_frames; i++) {
		auto frame = buildValidFrame(1005 + (i % 100), {static_cast<uint8_t>(i & 0xFF)});
		parser.add_তথ্য(frame.তথ্য(), frame.আকার_2());

		// Consume available messages after each add
		size_t len;

		যখন (parser.get_পরবর্তী_Message(&len) != nullptr) {
			parser.consumeMessage(len);
			গুনতি++;
		}
	}

	EXPECT_EQ(গুনতি, অঙ্ক_frames);
	auto stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, static_cast<uint32_t>(অঙ্ক_frames));
	EXPECT_EQ(stats.bytes_discarded, 0u);
	EXPECT_EQ(stats.crc_errors, 0u);
}

TEST_F(RtcmTest, Stress_LongGarbageStreamThenValid)
{
	// This tests the pathological case Alex mentioned: lots of invalid data
	// causing many memmove operations during preamble search
	ধ্রুবক size_t garbage_আকার = 10000;
	std::vector<uint8_t> garbage(garbage_আকার);
	std::mt19937 rng(99);

	জন্য (auto &byte : garbage) {
		// Avoid every protocol preamble to ensure maximum discards: 0xD3 (RTCM3)
		// and 0x73 (SPARTN), which the framer also treats as a frame start when
		// CONFIG_GPS_SPARTN is enabled.
		byte = (rng() % 0x72) + 1;  // 0x01 to 0x72, never 0x73 or 0xD3
	}

	// Add in chunks to simulate streaming
	size_t offset = 0;

	যখন (offset < garbage.আকার_2()) {
		size_t chunk = std::min<size_t>(256, garbage.আকার_2() - offset);
		parser.add_তথ্য(&garbage[offset], chunk);

		// Try to parse (forces discarding)
		size_t len;
		parser.get_পরবর্তী_Message(&len);
		offset += chunk;
	}

	// Now add a valid frame
	auto valid = buildValidFrame(1005, {0x01, 0x02, 0x03});
	parser.add_তথ্য(valid.তথ্য(), valid.আকার_2());

	size_t len;
	ধ্রুবক uint8_t *বার্তা = parser.get_পরবর্তী_Message(&len);
	ASSERT_NE(বার্তা, nullptr);

	parser.consumeMessage(len);
	auto stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 1u);
	EXPECT_EQ(stats.bytes_discarded, garbage_আকার);
}

TEST_F(RtcmTest, Stress_InterleavedGarbageAndValid)
{
	// Simulate a flaky sender that mixes garbage with valid frames
	ধ্রুবক পূর্ণসংখ্যা অঙ্ক_valid = 100;
	পূর্ণসংখ্যা parsed = 0;

	জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_valid; i++) {
		// Add some garbage (variable amount)
		std::vector<uint8_t> garbage(10 + (i % 50), 0xAA);
		parser.add_তথ্য(garbage.তথ্য(), garbage.আকার_2());

		// Add valid frame
		auto frame = buildValidFrame(1005, {static_cast<uint8_t>(i)});
		parser.add_তথ্য(frame.তথ্য(), frame.আকার_2());

		// Parse what we can
		size_t len;

		যখন (parser.get_পরবর্তী_Message(&len) != nullptr) {
			parser.consumeMessage(len);
			parsed++;
		}
	}

	EXPECT_EQ(parsed, অঙ্ক_valid);
}
