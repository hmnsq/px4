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

#ضمن "RtcmTestCommon.hpp"

TEST_F(RtcmTest, Stress_Many_Small_Valid_چوکٺا)
{
	// Simulate a good sender sending many small messages
	// Process as we go to avoid buffer overflow
	ثابت صحيح انگ_چوکٺا = 1000;
	صحيح ڳڻپ = 0;

	لكل (صحيح i = 0; i < انگ_چوکٺا; i++) {
		خودڪار چوکٺو = build_Valid_چوکٺو(1005 + (i % 100), {static_cast<uint8_t>(i & 0xFF)});
		parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

		// Consume available messages after each add
		size_t len;

		طالما (parser.get_ايندڙ_پيغام(&len) != خالي_اشارو) {
			parser.consume_پيغام(len);
			ڳڻپ++;
		}
	}

	EXPECT_EQ(ڳڻپ, انگ_چوکٺا);
	خودڪار stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, static_cast<uint32_t>(انگ_چوکٺا));
	EXPECT_EQ(stats.بائيٽون_discarded_2, 0u);
	EXPECT_EQ(stats.چڪري_جاچ_errors_2, 0u);
}

TEST_F(RtcmTest, Stress_Long_Garbage_وهڪرو_Then_Valid)
{
	// This tests the pathological case Alex mentioned: lots of invalid data
	// causing many memmove operations during preamble search
	ثابت size_t garbage_سائيز = 10000;
	std::ويڪٽر_2<uint8_t> garbage(garbage_سائيز);
	std::mt19937 rng(99);

	لكل (خودڪار &byte : garbage) {
		// Avoid every protocol preamble to ensure maximum discards: 0xD3 (RTCM3)
		// and 0x73 (SPARTN), which the framer also treats as a frame start when
		// CONFIG_GPS_SPARTN is enabled.
		byte = (rng() % 0x72) + 1;  // 0x01 to 0x72, never 0x73 or 0xD3
	}

	// Add in chunks to simulate streaming
	size_t وٿي = 0;

	طالما (وٿي < garbage.سائيز()) {
		size_t chunk = std::min<size_t>(256, garbage.سائيز() - وٿي);
		parser.جوڙيو_معلومات(&garbage[وٿي], chunk);

		// Try to parse (forces discarding)
		size_t len;
		parser.get_ايندڙ_پيغام(&len);
		وٿي += chunk;
	}

	// Now add a valid frame
	خودڪار valid = build_Valid_چوکٺو(1005, {0x01, 0x02, 0x03});
	parser.جوڙيو_معلومات(valid.معلومات(), valid.سائيز());

	size_t len;
	ثابت uint8_t *نياپو = parser.get_ايندڙ_پيغام(&len);
	ASSERT_NE(نياپو, خالي_اشارو);

	parser.consume_پيغام(len);
	خودڪار stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 1u);
	EXPECT_EQ(stats.بائيٽون_discarded_2, garbage_سائيز);
}

TEST_F(RtcmTest, Stress_InterleavedGarbageAndValid)
{
	// Simulate a flaky sender that mixes garbage with valid frames
	ثابت صحيح انگ_valid = 100;
	صحيح parsed = 0;

	لكل (صحيح i = 0; i < انگ_valid; i++) {
		// Add some garbage (variable amount)
		std::ويڪٽر_2<uint8_t> garbage(10 + (i % 50), 0xAA);
		parser.جوڙيو_معلومات(garbage.معلومات(), garbage.سائيز());

		// Add valid frame
		خودڪار چوکٺو = build_Valid_چوکٺو(1005, {static_cast<uint8_t>(i)});
		parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

		// Parse what we can
		size_t len;

		طالما (parser.get_ايندڙ_پيغام(&len) != خالي_اشارو) {
			parser.consume_پيغام(len);
			parsed++;
		}
	}

	EXPECT_EQ(parsed, انگ_valid);
}
