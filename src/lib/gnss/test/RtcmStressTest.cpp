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

#ਅਨ੍ਤਰ੍ਭਾਵ "RtcmTestCommon.hpp"

TEST_F(RtcmTest, Stress_Many_Small_Valid_ਚੌਖਟੇ)
{
	// Simulate a good sender sending many small messages
	// Process as we go to avoid buffer overflow
	ਅਚਲ ਪੂਰਨ ਅੰਕ_ਚੌਖਟੇ = 1000;
	ਪੂਰਨ ਗਿਣਤੀ_2 = 0;

	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_ਚੌਖਟੇ; i++) {
		ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_Valid_ਚੌਖਟਾ(1005 + (i % 100), {static_cast<uint8_t>(i & 0xFF)});
		parser.ਜੋੜੋ_ਅੰਕੜੇ(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());

		// Consume available messages after each add
		size_t len;

		ਜਦੋਂ (parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len) != ਖਾਲੀ_ਸੂਚਕ) {
			parser.consume_ਸੁਨੇਹਾ(len);
			ਗਿਣਤੀ_2++;
		}
	}

	EXPECT_EQ(ਗਿਣਤੀ_2, ਅੰਕ_ਚੌਖਟੇ);
	ਸਵੈਚਾਲਿਤ stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, static_cast<uint32_t>(ਅੰਕ_ਚੌਖਟੇ));
	EXPECT_EQ(stats.ਬਾਈਟਾਂ_discarded_2, 0u);
	EXPECT_EQ(stats.ਚੱਕਰੀ_ਜਾਂਚ_errors_2, 0u);
}

TEST_F(RtcmTest, Stress_Long_Garbage_ਧਾਰਾ_Then_Valid)
{
	// This tests the pathological case Alex mentioned: lots of invalid data
	// causing many memmove operations during preamble search
	ਅਚਲ size_t garbage_ਮਾਪ = 10000;
	std::ਸਦਿਸ਼_2<uint8_t> garbage(garbage_ਮਾਪ);
	std::mt19937 rng(99);

	ਲਈ (ਸਵੈਚਾਲਿਤ &byte : garbage) {
		// Avoid every protocol preamble to ensure maximum discards: 0xD3 (RTCM3)
		// and 0x73 (SPARTN), which the framer also treats as a frame start when
		// CONFIG_GPS_SPARTN is enabled.
		byte = (rng() % 0x72) + 1;  // 0x01 to 0x72, never 0x73 or 0xD3
	}

	// Add in chunks to simulate streaming
	size_t ਵਿੱਥ = 0;

	ਜਦੋਂ (ਵਿੱਥ < garbage.ਮਾਪ()) {
		size_t chunk = std::min<size_t>(256, garbage.ਮਾਪ() - ਵਿੱਥ);
		parser.ਜੋੜੋ_ਅੰਕੜੇ(&garbage[ਵਿੱਥ], chunk);

		// Try to parse (forces discarding)
		size_t len;
		parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len);
		ਵਿੱਥ += chunk;
	}

	// Now add a valid frame
	ਸਵੈਚਾਲਿਤ valid = build_Valid_ਚੌਖਟਾ(1005, {0x01, 0x02, 0x03});
	parser.ਜੋੜੋ_ਅੰਕੜੇ(valid.ਅੰਕੜੇ(), valid.ਮਾਪ());

	size_t len;
	ਅਚਲ uint8_t *ਸੁਨੇਹਾ_2 = parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len);
	ASSERT_NE(ਸੁਨੇਹਾ_2, ਖਾਲੀ_ਸੂਚਕ);

	parser.consume_ਸੁਨੇਹਾ(len);
	ਸਵੈਚਾਲਿਤ stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 1u);
	EXPECT_EQ(stats.ਬਾਈਟਾਂ_discarded_2, garbage_ਮਾਪ);
}

TEST_F(RtcmTest, Stress_InterleavedGarbageAndValid)
{
	// Simulate a flaky sender that mixes garbage with valid frames
	ਅਚਲ ਪੂਰਨ ਅੰਕ_valid = 100;
	ਪੂਰਨ parsed = 0;

	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_valid; i++) {
		// Add some garbage (variable amount)
		std::ਸਦਿਸ਼_2<uint8_t> garbage(10 + (i % 50), 0xAA);
		parser.ਜੋੜੋ_ਅੰਕੜੇ(garbage.ਅੰਕੜੇ(), garbage.ਮਾਪ());

		// Add valid frame
		ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_Valid_ਚੌਖਟਾ(1005, {static_cast<uint8_t>(i)});
		parser.ਜੋੜੋ_ਅੰਕੜੇ(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());

		// Parse what we can
		size_t len;

		ਜਦੋਂ (parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len) != ਖਾਲੀ_ਸੂਚਕ) {
			parser.consume_ਸੁਨੇਹਾ(len);
			parsed++;
		}
	}

	EXPECT_EQ(parsed, ਅੰਕ_valid);
}
