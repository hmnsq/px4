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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "correction_framer.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <vector>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>
#ਅਨ੍ਤਰ੍ਭਾਵ <random>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ gnss;

ਵਰਗ SpartnTest : ਜਨਤਕ ::testing::Test
{
ਸੁਰੱਖਿਅਤ:
	CorrectionFramer parser;

	// Build unencrypted frame with 16-bit time tag and given CRC type.
	std::ਸਦਿਸ਼_2<uint8_t> build_ਚੌਖਟਾ(uint8_t ਸੁਨੇਹਾ_type_2, uint8_t ਸੁਨੇਹਾ_subtype,
					ਅਚਲ std::ਸਦਿਸ਼_2<uint8_t> &payload,
					uint8_t ਚੱਕਰੀ_ਜਾਂਚ_type = 0)
	{
		ਅਚਲ size_t payload_len = payload.ਮਾਪ();
		std::ਸਦਿਸ਼_2<uint8_t> ਚੌਖਟਾ;
		ਚੌਖਟਾ.push_back(SPARTN_PREAMBLE);

		// TF002-TF006 (frame CRC nibble left 0; not checked by the framer)
		uint8_t b1 = static_cast<uint8_t>(((ਸੁਨੇਹਾ_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((ਚੱਕਰੀ_ਜਾਂਚ_type & 0x03) << 4));
		ਚੌਖਟਾ.push_back(b1);
		ਚੌਖਟਾ.push_back(b2);
		ਚੌਖਟਾ.push_back(b3);

		// 4-byte description, 16-bit time tag
		ਚੌਖਟਾ.push_back(static_cast<uint8_t>((ਸੁਨੇਹਾ_subtype & 0x0F) << 4));
		ਚੌਖਟਾ.push_back(0x00);
		ਚੌਖਟਾ.push_back(0x00);
		ਚੌਖਟਾ.push_back(0x00);

		ਚੌਖਟਾ.ਪਾਓ(ਚੌਖਟਾ.ਸਿਰਾ(), payload.begin(), payload.ਸਿਰਾ());

		// Message CRC over TF002..end of payload
		ਅਚਲ uint32_t ਚੱਕਰੀ_ਜਾਂਚ = spartn_ਸੁਨੇਹਾ_ਚੱਕਰੀ_ਜਾਂਚ(&ਚੌਖਟਾ[1], ਚੌਖਟਾ.ਮਾਪ() - 1, ਚੱਕਰੀ_ਜਾਂਚ_type);
		ਅਚਲ size_t ਚੱਕਰੀ_ਜਾਂਚ_ਬਾਈਟਾਂ = static_cast<size_t>(ਚੱਕਰੀ_ਜਾਂਚ_type) + 1;

		ਲਈ (ਪੂਰਨ i = static_cast<ਪੂਰਨ>(ਚੱਕਰੀ_ਜਾਂਚ_ਬਾਈਟਾਂ) - 1; i >= 0; i--) {
			ਚੌਖਟਾ.push_back(static_cast<uint8_t>((ਚੱਕਰੀ_ਜਾਂਚ >>(8 * i)) & 0xFF));
		}

		ਵਾਪਸ ਚੌਖਟਾ;
	}

	// Build a well-formed RTCM3 frame with a deterministic pseudo-random payload.
	std::ਸਦਿਸ਼_2<uint8_t> build_Rtcm_ਚੌਖਟਾ(std::mt19937 &rng, size_t payload_len)
	{
		std::ਸਦਿਸ਼_2<uint8_t> ਚੌਖਟਾ;
		ਚੌਖਟਾ.push_back(RTCM3_PREAMBLE);
		ਚੌਖਟਾ.push_back(static_cast<uint8_t>((payload_len >> 8) & 0x03));
		ਚੌਖਟਾ.push_back(static_cast<uint8_t>(payload_len & 0xFF));

		std::ਇਕਸਾਰ_int_distribution<ਪੂਰਨ> byte(0, 255);

		ਲਈ (size_t i = 0; i < payload_len; i++) {
			ਚੌਖਟਾ.push_back(static_cast<uint8_t>(byte(rng)));
		}

		ਅਚਲ uint32_t ਚੱਕਰੀ_ਜਾਂਚ = rtcm3_crc24q(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());
		ਚੌਖਟਾ.push_back((ਚੱਕਰੀ_ਜਾਂਚ >> 16) & 0xFF);
		ਚੌਖਟਾ.push_back((ਚੱਕਰੀ_ਜਾਂਚ >> 8) & 0xFF);
		ਚੌਖਟਾ.push_back(ਚੱਕਰੀ_ਜਾਂਚ & 0xFF);
		ਵਾਪਸ ਚੌਖਟਾ;
	}
};

// =============================================================================
// Message CRC (TF018)
// =============================================================================

// Vectors taken from the pyspartn reference implementation's test suite
// (semuconsulting/pyspartn, tests/test_static.py), which computes the same
// TF018 CRCs independently of this code.
TEST_F(SpartnTest, ਸੁਨੇਹਾ_ਚੱਕਰੀ_ਜਾਂਚ_Matches_Reference_Vectors)
{
	ਅਚਲ uint8_t ਸੁਨੇਹਾ_2[] = {'H', 'i', '!'};

	EXPECT_EQ(spartn_ਸੁਨੇਹਾ_ਚੱਕਰੀ_ਜਾਂਚ(ਸੁਨੇਹਾ_2, ਆਕਾਰ(ਸੁਨੇਹਾ_2), 0), 0x78u);
	EXPECT_EQ(spartn_ਸੁਨੇਹਾ_ਚੱਕਰੀ_ਜਾਂਚ(ਸੁਨੇਹਾ_2, ਆਕਾਰ(ਸੁਨੇਹਾ_2), 1), 0x31FDu);
	EXPECT_EQ(spartn_ਸੁਨੇਹਾ_ਚੱਕਰੀ_ਜਾਂਚ(ਸੁਨੇਹਾ_2, ਆਕਾਰ(ਸੁਨੇਹਾ_2), 2), 0x33220Fu);
	EXPECT_EQ(spartn_ਸੁਨੇਹਾ_ਚੱਕਰੀ_ਜਾਂਚ(ਸੁਨੇਹਾ_2, ਆਕਾਰ(ਸੁਨੇਹਾ_2), 3), 0x9523B4B4u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(SpartnTest, Single_ਨਿਊਨਤਮ_ਚੌਖਟਾ)
{
	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_ਚੌਖਟਾ(0, 0, {0xAA, 0xBB});
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ਅਚਲ uint8_t *ਸੁਨੇਹਾ_2 = parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len, &protocol);
	ASSERT_NE(ਸੁਨੇਹਾ_2, ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Spartn);
	EXPECT_EQ(len, ਚੌਖਟਾ.ਮਾਪ());
	EXPECT_EQ(0, memcmp(ਸੁਨੇਹਾ_2, ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ()));
	parser.consume_ਸੁਨੇਹਾ(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
	EXPECT_EQ(parser.getStats().ਚੱਕਰੀ_ਜਾਂਚ_errors_2, 0u);
}

TEST_F(SpartnTest, ਗੁਣਜ_ਚੌਖਟੇ)
{
	ਸਵੈਚਾਲਿਤ f1 = build_ਚੌਖਟਾ(0, 0, {0x01});
	ਸਵੈਚਾਲਿਤ f2 = build_ਚੌਖਟਾ(1, 0, {0x02, 0x03});
	std::ਸਦਿਸ਼_2<uint8_t> ਧਾਰਾ;
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), f1.begin(), f1.ਸਿਰਾ());
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), f2.begin(), f2.ਸਿਰਾ());
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਧਾਰਾ.ਅੰਕੜੇ(), ਧਾਰਾ.ਮਾਪ());

	ਪੂਰਨ ਗਿਣਤੀ_2 = 0;
	size_t len = 0;

	ਜਦੋਂ (parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len) != ਖਾਲੀ_ਸੂਚਕ) {
		parser.consume_ਸੁਨੇਹਾ(len);
		ਗਿਣਤੀ_2++;
	}

	EXPECT_EQ(ਗਿਣਤੀ_2, 2);
	EXPECT_EQ(parser.getStats().spartn_messages, 2u);
}

TEST_F(SpartnTest, Chunked_ਆਮਦ)
{
	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_ਚੌਖਟਾ(0, 1, {0x10, 0x20, 0x30});

	ਲਈ (size_t i = 0; i < ਚੌਖਟਾ.ਮਾਪ(); i++) {
		parser.ਜੋੜੋ_ਅੰਕੜੇ(&ਚੌਖਟਾ[i], 1);
	}

	size_t len = 0;
	ASSERT_NE(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len), ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_EQ(len, ਚੌਖਟਾ.ਮਾਪ());
}

TEST_F(SpartnTest, Noise_Before_ਚੌਖਟਾ)
{
	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_ਚੌਖਟਾ(0, 0, {0x55});
	std::ਸਦਿਸ਼_2<uint8_t> ਧਾਰਾ = {0x00, 0xFF, 0x01};
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), ਚੌਖਟਾ.begin(), ਚੌਖਟਾ.ਸਿਰਾ());
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਧਾਰਾ.ਅੰਕੜੇ(), ਧਾਰਾ.ਮਾਪ());

	size_t len = 0;
	ASSERT_NE(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len), ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_GT(parser.getStats().ਬਾਈਟਾਂ_discarded_2, 0u);
}

TEST_F(SpartnTest, Bad_ਸੁਨੇਹਾ_ਚੱਕਰੀ_ਜਾਂਚ)
{
	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_ਚੌਖਟਾ(0, 0, {0x11, 0x22});
	ਚੌਖਟਾ.ਪਿੱਠ() ^= 0xFF;
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());

	size_t len = 0;
	EXPECT_EQ(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len), ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_GE(parser.getStats().ਚੱਕਰੀ_ਜਾਂਚ_errors_2, 1u);
}

TEST_F(SpartnTest, Crc16_ਚੌਖਟਾ)
{
	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_ਚੌਖਟਾ(0, 0, {0xDE, 0xAD}, 1);
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());

	size_t len = 0;
	ASSERT_NE(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len), ਖਾਲੀ_ਸੂਚਕ);
	parser.consume_ਸੁਨੇਹਾ(len);
	EXPECT_EQ(parser.getStats().ਚੱਕਰੀ_ਜਾਂਚ_errors_2, 0u);
}

// TF002 5-119 is reserved by SPARTN 2.x, so a frame claiming one cannot be real
// even if its message CRC checks out.
TEST_F(SpartnTest, Rejects_Reserved_ਸੁਨੇਹਾ_Type)
{
	EXPECT_TRUE(spartn_ਸੁਨੇਹਾ_type_defined(0));
	EXPECT_TRUE(spartn_ਸੁਨੇਹਾ_type_defined(4));
	EXPECT_FALSE(spartn_ਸੁਨੇਹਾ_type_defined(5));
	EXPECT_FALSE(spartn_ਸੁਨੇਹਾ_type_defined(119));
	EXPECT_TRUE(spartn_ਸੁਨੇਹਾ_type_defined(120));
	EXPECT_TRUE(spartn_ਸੁਨੇਹਾ_type_defined(127));

	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_ਚੌਖਟਾ(50, 0, {0x11, 0x22});
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());

	size_t len = 0;
	EXPECT_EQ(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len), ਖਾਲੀ_ਸੂਚਕ);
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
TEST_F(SpartnTest, Rtcm_ਧਾਰਾ_Is_Never_Framed_As_Spartn)
{
	std::mt19937 rng(1);
	std::ਇਕਸਾਰ_int_distribution<ਪੂਰਨ> payload_len(60, 500);

	size_t ਚੌਖਟੇ_ਵਿੱਚ = 0;
	size_t ਚੌਖਟੇ_ਬਾਹਰਲਾ = 0;
	size_t generated = 0;
	std::ਸਦਿਸ਼_2<uint8_t> ਧਾਰਾ;

	// ~1 MB of RTCM3, about 17 min of a 1 kB/s RTK correction stream
	ਜਦੋਂ (generated < 1000000) {
		ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_Rtcm_ਚੌਖਟਾ(rng, payload_len(rng));
		generated += ਚੌਖਟਾ.ਮਾਪ();
		ਚੌਖਟੇ_ਵਿੱਚ++;
		ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), ਚੌਖਟਾ.begin(), ਚੌਖਟਾ.ਸਿਰਾ());

		// Feed in MAVLink GPS_RTCM_DATA sized chunks
		ਜਦੋਂ (ਧਾਰਾ.ਮਾਪ() >= 180) {
			parser.ਜੋੜੋ_ਅੰਕੜੇ(ਧਾਰਾ.ਅੰਕੜੇ(), 180);

			size_t len = 0;
			CorrectionProtocol protocol = CorrectionProtocol::Spartn;

			ਜਦੋਂ (parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len, &protocol) != ਖਾਲੀ_ਸੂਚਕ) {
				EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
				parser.consume_ਸੁਨੇਹਾ(len);
				ਚੌਖਟੇ_ਬਾਹਰਲਾ++;
			}

			ਧਾਰਾ.erase(ਧਾਰਾ.begin(), ਧਾਰਾ.begin() + 180);
		}
	}

	// Every RTCM3 frame is recovered, and no part of the stream is framed as SPARTN
	EXPECT_EQ(parser.getStats().spartn_messages, 0u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, ਚੌਖਟੇ_ਬਾਹਰਲਾ);
	EXPECT_GE(ਚੌਖਟੇ_ਬਾਹਰਲਾ, ਚੌਖਟੇ_ਵਿੱਚ - 1);
	EXPECT_EQ(parser.getStats().ਚੱਕਰੀ_ਜਾਂਚ_errors_2, 0u);
}
