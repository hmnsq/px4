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

#ಅನ್ತರ್ಭಾವ "RtcmTestCommon.hpp"

TEST_F(RtcmTest, Stress_Many_Small_Valid_ಚೌಕಟ್ಟುಗಳು)
{
	// Simulate a good sender sending many small messages
	// Process as we go to avoid buffer overflow
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಚೌಕಟ್ಟುಗಳು = 1000;
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_ಚೌಕಟ್ಟುಗಳು; i++) {
		ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_Valid_ಚೌಕಟ್ಟು(1005 + (i % 100), {static_cast<uint8_t>(i & 0xFF)});
		parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());

		// Consume available messages after each add
		size_t len;

		ಆಗ (parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len) != ಶೂನ್ಯಸೂಚಕ) {
			parser.consume_ಸಂದೇಶ(len);
			ಎಣಿಕೆ_2++;
		}
	}

	EXPECT_EQ(ಎಣಿಕೆ_2, ಅಂಕೆ_ಚೌಕಟ್ಟುಗಳು);
	ಸ್ವಯಂಚಾಲಿತ stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, static_cast<uint32_t>(ಅಂಕೆ_ಚೌಕಟ್ಟುಗಳು));
	EXPECT_EQ(stats.ಬೈಟುಗಳು_discarded_2, 0u);
	EXPECT_EQ(stats.ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 0u);
}

TEST_F(RtcmTest, Stress_Long_Garbage_ಹರಿವು_Then_Valid)
{
	// This tests the pathological case Alex mentioned: lots of invalid data
	// causing many memmove operations during preamble search
	ಸ್ಥಿರಾಂಕ size_t garbage_ಗಾತ್ರ = 10000;
	std::ಸದಿಶ_2<uint8_t> garbage(garbage_ಗಾತ್ರ);
	std::mt19937 rng(99);

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &byte : garbage) {
		// Avoid every protocol preamble to ensure maximum discards: 0xD3 (RTCM3)
		// and 0x73 (SPARTN), which the framer also treats as a frame start when
		// CONFIG_GPS_SPARTN is enabled.
		byte = (rng() % 0x72) + 1;  // 0x01 to 0x72, never 0x73 or 0xD3
	}

	// Add in chunks to simulate streaming
	size_t ಸ್ಥಾನಾಂತರ = 0;

	ಆಗ (ಸ್ಥಾನಾಂತರ < garbage.ಗಾತ್ರ_2()) {
		size_t chunk = std::min<size_t>(256, garbage.ಗಾತ್ರ_2() - ಸ್ಥಾನಾಂತರ);
		parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(&garbage[ಸ್ಥಾನಾಂತರ], chunk);

		// Try to parse (forces discarding)
		size_t len;
		parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len);
		ಸ್ಥಾನಾಂತರ += chunk;
	}

	// Now add a valid frame
	ಸ್ವಯಂಚಾಲಿತ valid = build_Valid_ಚೌಕಟ್ಟು(1005, {0x01, 0x02, 0x03});
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(valid.ದತ್ತಾಂಶ(), valid.ಗಾತ್ರ_2());

	size_t len;
	ಸ್ಥಿರಾಂಕ uint8_t *ಸಂದೇಶ_2 = parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len);
	ASSERT_NE(ಸಂದೇಶ_2, ಶೂನ್ಯಸೂಚಕ);

	parser.consume_ಸಂದೇಶ(len);
	ಸ್ವಯಂಚಾಲಿತ stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 1u);
	EXPECT_EQ(stats.ಬೈಟುಗಳು_discarded_2, garbage_ಗಾತ್ರ);
}

TEST_F(RtcmTest, Stress_InterleavedGarbageAndValid)
{
	// Simulate a flaky sender that mixes garbage with valid frames
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_valid = 100;
	ಪೂರ್ಣಾಂಕ parsed = 0;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_valid; i++) {
		// Add some garbage (variable amount)
		std::ಸದಿಶ_2<uint8_t> garbage(10 + (i % 50), 0xAA);
		parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(garbage.ದತ್ತಾಂಶ(), garbage.ಗಾತ್ರ_2());

		// Add valid frame
		ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_Valid_ಚೌಕಟ್ಟು(1005, {static_cast<uint8_t>(i)});
		parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());

		// Parse what we can
		size_t len;

		ಆಗ (parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len) != ಶೂನ್ಯಸೂಚಕ) {
			parser.consume_ಸಂದೇಶ(len);
			parsed++;
		}
	}

	EXPECT_EQ(parsed, ಅಂಕೆ_valid);
}
