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

TEST_F(RtcmTest, BustedSender_AllZeros)
{
	std::ಸದಿಶ_2<uint8_t> garbage(1000, 0x00);
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(garbage.ದತ್ತಾಂಶ(), garbage.ಗಾತ್ರ_2());

	size_t len;
	EXPECT_EQ(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(parser.getStats().ಬೈಟುಗಳು_discarded_2, 1000u);
	EXPECT_EQ(parser.buffered_ಬೈಟುಗಳು(), 0u);
}

TEST_F(RtcmTest, BustedSender_AllPreambles)
{
	// All preamble bytes (0xD3). The parser sees each 0xD3 as a potential frame start.
	// Length extracted from 0xD3,0xD3 is ((0x03)<<8)|0xD3 = 979 bytes.
	// Parser waits for full frame which never comes.
	std::ಸದಿಶ_2<uint8_t> preambles(500, RTCM3_PREAMBLE);
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(preambles.ದತ್ತಾಂಶ(), preambles.ಗಾತ್ರ_2());

	size_t len;
	// Parser should be stuck waiting for more data (incomplete frame)
	EXPECT_EQ(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	// Buffer retains data waiting for frame completion
	EXPECT_GT(parser.buffered_ಬೈಟುಗಳು(), 0u);
}

TEST_F(RtcmTest, BustedSender_RandomNoise)
{
	std::mt19937 rng(12345);
	uint8_t noise[500];

	ಗಾಗಿ (size_t i = 0; i < ಗಾತ್ರ(noise); i++) {
		noise[i] = rng() & 0xFF;
	}

	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(noise, ಗಾತ್ರ(noise));

	size_t len;

	// Consume any messages that might accidentally form (unlikely but possible)
	ಆಗ (parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len) != ಶೂನ್ಯಸೂಚಕ) {
		parser.consume_ಸಂದೇಶ(len);
	}

	// Most data should be discarded
	ಸ್ವಯಂಚಾಲಿತ stats = parser.getStats();
	EXPECT_GT(stats.ಬೈಟುಗಳು_discarded_2 + stats.ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 0u);
}

TEST_F(RtcmTest, Busted_Sender_Valid_ಶೀರ್ಷಿಕೆ_Bad_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ)
{
	// Frame with valid header but bad CRC
	std::ಸದಿಶ_2<uint8_t> ಚೌಕಟ್ಟು = {
		RTCM3_PREAMBLE, 0x00, 0x04,  // Header: 4 byte payload
		0x01, 0x02, 0x03, 0x04,      // Payload
		0xDE, 0xAD, 0xBE             // Bad CRC
	};

	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());

	size_t len;
	EXPECT_EQ(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(parser.getStats().ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 1u);
}

TEST_F(RtcmTest, Busted_Sender_Repeated_Bad_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಚೌಕಟ್ಟುಗಳು)
{
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 100; i++) {
		ಸ್ವಯಂಚಾಲಿತ bad = build_Bad_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಚೌಕಟ್ಟು({static_cast<uint8_t>(i), 0x00});
		parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(bad.ದತ್ತಾಂಶ(), bad.ಗಾತ್ರ_2());
	}

	// Re-scanning after a failure can land on a byte that claims a frame longer
	// than what is buffered, which parks the framer until the rest arrives (see
	// BustedSender_AllPreambles). Feed a frame's worth of non-preamble filler so
	// every pending candidate can be resolved, as a live stream would.
	ಸ್ಥಿರಾಂಕ std::ಸದಿಶ_2<uint8_t> filler(CorrectionFramer::MAX_ಚೌಕಟ್ಟು_LEN, 0x00);
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(filler.ದತ್ತಾಂಶ(), filler.ಗಾತ್ರ_2());

	size_t len;

	ಆಗ (parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len) != ಶೂನ್ಯಸೂಚಕ) {
		parser.consume_ಸಂದೇಶ(len);
	}

	ಸ್ವಯಂಚಾಲಿತ stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 0u);
	// At least 100 CRC errors (may be more due to re-scanning after failures)
	EXPECT_GE(stats.ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 100u);
}
