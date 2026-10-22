/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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

/**
 * @file SpartnFramerTest.cpp
 *
 * SPARTN framing tests for CorrectionFramer, including the RTCM3/SPARTN
 * cross-protocol resync behaviour that motivates sharing one buffer.
 */

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ "correction_framer.h"

#ಅನ್ತರ್ಭಾವ <vector>
#ಅನ್ತರ್ಭಾವ <cstring>
#ಅನ್ತರ್ಭಾವ <random>

ಬಳಸಿ ನಾಮಾಕಾಶ gnss;

ವರ್ಗ SpartnTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಂರಕ್ಷಿತ:
	CorrectionFramer parser;

	// Build unencrypted frame with 16-bit time tag and given CRC type.
	std::ಸದಿಶ_2<uint8_t> build_ಚೌಕಟ್ಟು(uint8_t ಸಂದೇಶ_type_2, uint8_t ಸಂದೇಶ_subtype,
					ಸ್ಥಿರಾಂಕ std::ಸದಿಶ_2<uint8_t> &payload,
					uint8_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type = 0)
	{
		ಸ್ಥಿರಾಂಕ size_t payload_len = payload.ಗಾತ್ರ_2();
		std::ಸದಿಶ_2<uint8_t> ಚೌಕಟ್ಟು;
		ಚೌಕಟ್ಟು.push_back(SPARTN_PREAMBLE);

		// TF002-TF006 (frame CRC nibble left 0; not checked by the framer)
		uint8_t b1 = static_cast<uint8_t>(((ಸಂದೇಶ_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type & 0x03) << 4));
		ಚೌಕಟ್ಟು.push_back(b1);
		ಚೌಕಟ್ಟು.push_back(b2);
		ಚೌಕಟ್ಟು.push_back(b3);

		// 4-byte description, 16-bit time tag
		ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>((ಸಂದೇಶ_subtype & 0x0F) << 4));
		ಚೌಕಟ್ಟು.push_back(0x00);
		ಚೌಕಟ್ಟು.push_back(0x00);
		ಚೌಕಟ್ಟು.push_back(0x00);

		ಚೌಕಟ್ಟು.ಸೇರಿಸು(ಚೌಕಟ್ಟು.ಕೊನೆ(), payload.begin(), payload.ಕೊನೆ());

		// Message CRC over TF002..end of payload
		ಸ್ಥಿರಾಂಕ uint32_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = spartn_ಸಂದೇಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(&ಚೌಕಟ್ಟು[1], ಚೌಕಟ್ಟು.ಗಾತ್ರ_2() - 1, ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type);
		ಸ್ಥಿರಾಂಕ size_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಬೈಟುಗಳು = static_cast<size_t>(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type) + 1;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = static_cast<ಪೂರ್ಣಾಂಕ>(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಬೈಟುಗಳು) - 1; i >= 0; i--) {
			ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ >>(8 * i)) & 0xFF));
		}

		ಹಿಂತಿರುಗಿ ಚೌಕಟ್ಟು;
	}

	// Build a well-formed RTCM3 frame with a deterministic pseudo-random payload.
	std::ಸದಿಶ_2<uint8_t> build_Rtcm_ಚೌಕಟ್ಟು(std::mt19937 &rng, size_t payload_len)
	{
		std::ಸದಿಶ_2<uint8_t> ಚೌಕಟ್ಟು;
		ಚೌಕಟ್ಟು.push_back(RTCM3_PREAMBLE);
		ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>((payload_len >> 8) & 0x03));
		ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>(payload_len & 0xFF));

		std::ಏಕರೂಪ_int_distribution<ಪೂರ್ಣಾಂಕ> byte(0, 255);

		ಗಾಗಿ (size_t i = 0; i < payload_len; i++) {
			ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>(byte(rng)));
		}

		ಸ್ಥಿರಾಂಕ uint32_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = rtcm3_crc24q(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());
		ಚೌಕಟ್ಟು.push_back((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ >> 16) & 0xFF);
		ಚೌಕಟ್ಟು.push_back((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ >> 8) & 0xFF);
		ಚೌಕಟ್ಟು.push_back(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ & 0xFF);
		ಹಿಂತಿರುಗಿ ಚೌಕಟ್ಟು;
	}
};

// =============================================================================
// Message CRC (TF018)
// =============================================================================

// Vectors taken from the pyspartn reference implementation's test suite
// (semuconsulting/pyspartn, tests/test_static.py), which computes the same
// TF018 CRCs independently of this code.
TEST_F(SpartnTest, ಸಂದೇಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_Matches_Reference_Vectors)
{
	ಸ್ಥಿರಾಂಕ uint8_t ಸಂದೇಶ_2[] = {'H', 'i', '!'};

	EXPECT_EQ(spartn_ಸಂದೇಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ಸಂದೇಶ_2, ಗಾತ್ರ(ಸಂದೇಶ_2), 0), 0x78u);
	EXPECT_EQ(spartn_ಸಂದೇಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ಸಂದೇಶ_2, ಗಾತ್ರ(ಸಂದೇಶ_2), 1), 0x31FDu);
	EXPECT_EQ(spartn_ಸಂದೇಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ಸಂದೇಶ_2, ಗಾತ್ರ(ಸಂದೇಶ_2), 2), 0x33220Fu);
	EXPECT_EQ(spartn_ಸಂದೇಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ಸಂದೇಶ_2, ಗಾತ್ರ(ಸಂದೇಶ_2), 3), 0x9523B4B4u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(SpartnTest, Single_ಕನಿಷ್ಠತಮ_ಚೌಕಟ್ಟು)
{
	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_ಚೌಕಟ್ಟು(0, 0, {0xAA, 0xBB});
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ಸ್ಥಿರಾಂಕ uint8_t *ಸಂದೇಶ_2 = parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len, &protocol);
	ASSERT_NE(ಸಂದೇಶ_2, ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Spartn);
	EXPECT_EQ(len, ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());
	EXPECT_EQ(0, memcmp(ಸಂದೇಶ_2, ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2()));
	parser.consume_ಸಂದೇಶ(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
	EXPECT_EQ(parser.getStats().ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 0u);
}

TEST_F(SpartnTest, ಅಪವರ್ತ್ಯ_ಚೌಕಟ್ಟುಗಳು)
{
	ಸ್ವಯಂಚಾಲಿತ f1 = build_ಚೌಕಟ್ಟು(0, 0, {0x01});
	ಸ್ವಯಂಚಾಲಿತ f2 = build_ಚೌಕಟ್ಟು(1, 0, {0x02, 0x03});
	std::ಸದಿಶ_2<uint8_t> ಹರಿವು;
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), f1.begin(), f1.ಕೊನೆ());
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), f2.begin(), f2.ಕೊನೆ());
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಹರಿವು.ದತ್ತಾಂಶ(), ಹರಿವು.ಗಾತ್ರ_2());

	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;
	size_t len = 0;

	ಆಗ (parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len) != ಶೂನ್ಯಸೂಚಕ) {
		parser.consume_ಸಂದೇಶ(len);
		ಎಣಿಕೆ_2++;
	}

	EXPECT_EQ(ಎಣಿಕೆ_2, 2);
	EXPECT_EQ(parser.getStats().spartn_messages, 2u);
}

TEST_F(SpartnTest, Chunked_ಆಗಮನ)
{
	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_ಚೌಕಟ್ಟು(0, 1, {0x10, 0x20, 0x30});

	ಗಾಗಿ (size_t i = 0; i < ಚೌಕಟ್ಟು.ಗಾತ್ರ_2(); i++) {
		parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(&ಚೌಕಟ್ಟು[i], 1);
	}

	size_t len = 0;
	ASSERT_NE(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(len, ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());
}

TEST_F(SpartnTest, Noise_Before_ಚೌಕಟ್ಟು)
{
	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_ಚೌಕಟ್ಟು(0, 0, {0x55});
	std::ಸದಿಶ_2<uint8_t> ಹರಿವು = {0x00, 0xFF, 0x01};
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), ಚೌಕಟ್ಟು.begin(), ಚೌಕಟ್ಟು.ಕೊನೆ());
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಹರಿವು.ದತ್ತಾಂಶ(), ಹರಿವು.ಗಾತ್ರ_2());

	size_t len = 0;
	ASSERT_NE(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_GT(parser.getStats().ಬೈಟುಗಳು_discarded_2, 0u);
}

TEST_F(SpartnTest, Bad_ಸಂದೇಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ)
{
	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_ಚೌಕಟ್ಟು(0, 0, {0x11, 0x22});
	ಚೌಕಟ್ಟು.ಹಿಂಭಾಗ() ^= 0xFF;
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());

	size_t len = 0;
	EXPECT_EQ(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_GE(parser.getStats().ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 1u);
}

TEST_F(SpartnTest, Crc16_ಚೌಕಟ್ಟು)
{
	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_ಚೌಕಟ್ಟು(0, 0, {0xDE, 0xAD}, 1);
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());

	size_t len = 0;
	ASSERT_NE(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	parser.consume_ಸಂದೇಶ(len);
	EXPECT_EQ(parser.getStats().ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 0u);
}

// TF002 5-119 is reserved by SPARTN 2.x, so a frame claiming one cannot be real
// even if its message CRC checks out.
TEST_F(SpartnTest, Rejects_Reserved_ಸಂದೇಶ_Type)
{
	EXPECT_TRUE(spartn_ಸಂದೇಶ_type_defined(0));
	EXPECT_TRUE(spartn_ಸಂದೇಶ_type_defined(4));
	EXPECT_FALSE(spartn_ಸಂದೇಶ_type_defined(5));
	EXPECT_FALSE(spartn_ಸಂದೇಶ_type_defined(119));
	EXPECT_TRUE(spartn_ಸಂದೇಶ_type_defined(120));
	EXPECT_TRUE(spartn_ಸಂದೇಶ_type_defined(127));

	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_ಚೌಕಟ್ಟು(50, 0, {0x11, 0x22});
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());

	size_t len = 0;
	EXPECT_EQ(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(parser.getStats().spartn_messages, 0u);
}

// =============================================================================
// Cross-protocol resync
// =============================================================================

// An RTCM3 payload is dense binary, so it carries stray 0x73 SPARTN preambles at
// roughly 1 byte in 256. Framing both protocols from one buffer means a valid
// RTCM3 frame consumes its own payload, so those bytes never start a frame. An
// independent SPARTN framer over the same bytes would instead scan inside RTCM3
// payloads, where the only remaining check is the message CRC that TF005 permits
// to be 8 bits wide.
TEST_F(SpartnTest, Rtcm_ಹರಿವು_Is_Never_Framed_As_Spartn)
{
	std::mt19937 rng(1);
	std::ಏಕರೂಪ_int_distribution<ಪೂರ್ಣಾಂಕ> payload_len(60, 500);

	size_t ಚೌಕಟ್ಟುಗಳು_ಒಳ = 0;
	size_t ಚೌಕಟ್ಟುಗಳು_ಹೊರಹೋಗು = 0;
	size_t generated = 0;
	std::ಸದಿಶ_2<uint8_t> ಹರಿವು;

	// ~1 MB of RTCM3, about 17 min of a 1 kB/s RTK correction stream
	ಆಗ (generated < 1000000) {
		ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_Rtcm_ಚೌಕಟ್ಟು(rng, payload_len(rng));
		generated += ಚೌಕಟ್ಟು.ಗಾತ್ರ_2();
		ಚೌಕಟ್ಟುಗಳು_ಒಳ++;
		ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), ಚೌಕಟ್ಟು.begin(), ಚೌಕಟ್ಟು.ಕೊನೆ());

		// Feed in MAVLink GPS_RTCM_DATA sized chunks
		ಆಗ (ಹರಿವು.ಗಾತ್ರ_2() >= 180) {
			parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಹರಿವು.ದತ್ತಾಂಶ(), 180);

			size_t len = 0;
			CorrectionProtocol protocol = CorrectionProtocol::Spartn;

			ಆಗ (parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len, &protocol) != ಶೂನ್ಯಸೂಚಕ) {
				EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
				parser.consume_ಸಂದೇಶ(len);
				ಚೌಕಟ್ಟುಗಳು_ಹೊರಹೋಗು++;
			}

			ಹರಿವು.erase(ಹರಿವು.begin(), ಹರಿವು.begin() + 180);
		}
	}

	// Every RTCM3 frame is recovered, and no part of the stream is framed as SPARTN
	EXPECT_EQ(parser.getStats().spartn_messages, 0u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, ಚೌಕಟ್ಟುಗಳು_ಹೊರಹೋಗು);
	EXPECT_GE(ಚೌಕಟ್ಟುಗಳು_ಹೊರಹೋಗು, ಚೌಕಟ್ಟುಗಳು_ಒಳ - 1);
	EXPECT_EQ(parser.getStats().ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 0u);
}
