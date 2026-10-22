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

TEST_F(RtcmTest, Stress_Many_Small_Valid_چوکاټونه)
{
	// Simulate a good sender sending many small messages
	// Process as we go to avoid buffer overflow
	ثابت صحيح عدد_چوکاټونه = 1000;
	صحيح شمېر = 0;

	لكل (صحيح i = 0; i < عدد_چوکاټونه; i++) {
		خپلکاری چوکاټ = build_Valid_چوکاټ(1005 + (i % 100), {static_cast<uint8_t>(i & 0xFF)});
		parser.جمع_کړه_معلومات(چوکاټ.معلومات(), چوکاټ.اندازه());

		// Consume available messages after each add
		size_t len;

		طالما (parser.get_بل_پیغام(&len) != تش_اشاره) {
			parser.consume_پیغام(len);
			شمېر++;
		}
	}

	EXPECT_EQ(شمېر, عدد_چوکاټونه);
	خپلکاری stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, static_cast<uint32_t>(عدد_چوکاټونه));
	EXPECT_EQ(stats.بایټونه_discarded_2, 0u);
	EXPECT_EQ(stats.دوراني_ازموینه_errors_2, 0u);
}

TEST_F(RtcmTest, Stress_Long_Garbage_بهیر_Then_Valid)
{
	// This tests the pathological case Alex mentioned: lots of invalid data
	// causing many memmove operations during preamble search
	ثابت size_t garbage_اندازه = 10000;
	std::ویکټور_2<uint8_t> garbage(garbage_اندازه);
	std::mt19937 rng(99);

	لكل (خپلکاری &byte : garbage) {
		// Avoid every protocol preamble to ensure maximum discards: 0xD3 (RTCM3)
		// and 0x73 (SPARTN), which the framer also treats as a frame start when
		// CONFIG_GPS_SPARTN is enabled.
		byte = (rng() % 0x72) + 1;  // 0x01 to 0x72, never 0x73 or 0xD3
	}

	// Add in chunks to simulate streaming
	size_t دننه_واټن = 0;

	طالما (دننه_واټن < garbage.اندازه()) {
		size_t chunk = std::min<size_t>(256, garbage.اندازه() - دننه_واټن);
		parser.جمع_کړه_معلومات(&garbage[دننه_واټن], chunk);

		// Try to parse (forces discarding)
		size_t len;
		parser.get_بل_پیغام(&len);
		دننه_واټن += chunk;
	}

	// Now add a valid frame
	خپلکاری valid = build_Valid_چوکاټ(1005, {0x01, 0x02, 0x03});
	parser.جمع_کړه_معلومات(valid.معلومات(), valid.اندازه());

	size_t len;
	ثابت uint8_t *پیغام_2 = parser.get_بل_پیغام(&len);
	ASSERT_NE(پیغام_2, تش_اشاره);

	parser.consume_پیغام(len);
	خپلکاری stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 1u);
	EXPECT_EQ(stats.بایټونه_discarded_2, garbage_اندازه);
}

TEST_F(RtcmTest, Stress_InterleavedGarbageAndValid)
{
	// Simulate a flaky sender that mixes garbage with valid frames
	ثابت صحيح عدد_valid = 100;
	صحيح parsed = 0;

	لكل (صحيح i = 0; i < عدد_valid; i++) {
		// Add some garbage (variable amount)
		std::ویکټور_2<uint8_t> garbage(10 + (i % 50), 0xAA);
		parser.جمع_کړه_معلومات(garbage.معلومات(), garbage.اندازه());

		// Add valid frame
		خپلکاری چوکاټ = build_Valid_چوکاټ(1005, {static_cast<uint8_t>(i)});
		parser.جمع_کړه_معلومات(چوکاټ.معلومات(), چوکاټ.اندازه());

		// Parse what we can
		size_t len;

		طالما (parser.get_بل_پیغام(&len) != تش_اشاره) {
			parser.consume_پیغام(len);
			parsed++;
		}
	}

	EXPECT_EQ(parsed, عدد_valid);
}
