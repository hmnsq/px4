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

#ضمن <gtest/gtest.h>
#ضمن "correction_framer.h"

#ضمن <vector>
#ضمن <cstring>
#ضمن <random>

استعمال نطاق gnss;

صنف SpartnTest : عام ::testing::Test
{
محمي:
	CorrectionFramer parser;

	// Build unencrypted frame with 16-bit time tag and given CRC type.
	std::ويڪٽر_2<uint8_t> build_چوکٺو(uint8_t نياپو_type, uint8_t نياپو_subtype,
					ثابت std::ويڪٽر_2<uint8_t> &payload,
					uint8_t چڪري_جاچ_type = 0)
	{
		ثابت size_t payload_len = payload.سائيز();
		std::ويڪٽر_2<uint8_t> چوکٺو;
		چوکٺو.push_back(SPARTN_PREAMBLE);

		// TF002-TF006 (frame CRC nibble left 0; not checked by the framer)
		uint8_t b1 = static_cast<uint8_t>(((نياپو_type & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((چڪري_جاچ_type & 0x03) << 4));
		چوکٺو.push_back(b1);
		چوکٺو.push_back(b2);
		چوکٺو.push_back(b3);

		// 4-byte description, 16-bit time tag
		چوکٺو.push_back(static_cast<uint8_t>((نياپو_subtype & 0x0F) << 4));
		چوکٺو.push_back(0x00);
		چوکٺو.push_back(0x00);
		چوکٺو.push_back(0x00);

		چوکٺو.وجهو(چوکٺو.پڇاڙي(), payload.begin(), payload.پڇاڙي());

		// Message CRC over TF002..end of payload
		ثابت uint32_t چڪري_جاچ = spartn_پيغام_چڪري_جاچ(&چوکٺو[1], چوکٺو.سائيز() - 1, چڪري_جاچ_type);
		ثابت size_t چڪري_جاچ_بائيٽون = static_cast<size_t>(چڪري_جاچ_type) + 1;

		لكل (صحيح i = static_cast<صحيح>(چڪري_جاچ_بائيٽون) - 1; i >= 0; i--) {
			چوکٺو.push_back(static_cast<uint8_t>((چڪري_جاچ >>(8 * i)) & 0xFF));
		}

		أرجع چوکٺو;
	}

	// Build a well-formed RTCM3 frame with a deterministic pseudo-random payload.
	std::ويڪٽر_2<uint8_t> build_Rtcm_چوکٺو(std::mt19937 &rng, size_t payload_len)
	{
		std::ويڪٽر_2<uint8_t> چوکٺو;
		چوکٺو.push_back(RTCM3_PREAMBLE);
		چوکٺو.push_back(static_cast<uint8_t>((payload_len >> 8) & 0x03));
		چوکٺو.push_back(static_cast<uint8_t>(payload_len & 0xFF));

		std::هڪجهڙو_int_distribution<صحيح> byte(0, 255);

		لكل (size_t i = 0; i < payload_len; i++) {
			چوکٺو.push_back(static_cast<uint8_t>(byte(rng)));
		}

		ثابت uint32_t چڪري_جاچ = rtcm3_crc24q(چوکٺو.معلومات(), چوکٺو.سائيز());
		چوکٺو.push_back((چڪري_جاچ >> 16) & 0xFF);
		چوکٺو.push_back((چڪري_جاچ >> 8) & 0xFF);
		چوکٺو.push_back(چڪري_جاچ & 0xFF);
		أرجع چوکٺو;
	}
};

// =============================================================================
// Message CRC (TF018)
// =============================================================================

// Vectors taken from the pyspartn reference implementation's test suite
// (semuconsulting/pyspartn, tests/test_static.py), which computes the same
// TF018 CRCs independently of this code.
TEST_F(SpartnTest, پيغام_چڪري_جاچ_Matches_Reference_Vectors)
{
	ثابت uint8_t نياپو[] = {'H', 'i', '!'};

	EXPECT_EQ(spartn_پيغام_چڪري_جاچ(نياپو, حجم(نياپو), 0), 0x78u);
	EXPECT_EQ(spartn_پيغام_چڪري_جاچ(نياپو, حجم(نياپو), 1), 0x31FDu);
	EXPECT_EQ(spartn_پيغام_چڪري_جاچ(نياپو, حجم(نياپو), 2), 0x33220Fu);
	EXPECT_EQ(spartn_پيغام_چڪري_جاچ(نياپو, حجم(نياپو), 3), 0x9523B4B4u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(SpartnTest, Single_گهٽتر_چوکٺو)
{
	خودڪار چوکٺو = build_چوکٺو(0, 0, {0xAA, 0xBB});
	parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *نياپو = parser.get_ايندڙ_پيغام(&len, &protocol);
	ASSERT_NE(نياپو, خالي_اشارو);
	EXPECT_EQ(protocol, CorrectionProtocol::Spartn);
	EXPECT_EQ(len, چوکٺو.سائيز());
	EXPECT_EQ(0, memcmp(نياپو, چوکٺو.معلومات(), چوکٺو.سائيز()));
	parser.consume_پيغام(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
	EXPECT_EQ(parser.getStats().چڪري_جاچ_errors_2, 0u);
}

TEST_F(SpartnTest, گڻو_چوکٺا)
{
	خودڪار f1 = build_چوکٺو(0, 0, {0x01});
	خودڪار f2 = build_چوکٺو(1, 0, {0x02, 0x03});
	std::ويڪٽر_2<uint8_t> وهڪرو;
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), f1.begin(), f1.پڇاڙي());
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), f2.begin(), f2.پڇاڙي());
	parser.جوڙيو_معلومات(وهڪرو.معلومات(), وهڪرو.سائيز());

	صحيح ڳڻپ = 0;
	size_t len = 0;

	طالما (parser.get_ايندڙ_پيغام(&len) != خالي_اشارو) {
		parser.consume_پيغام(len);
		ڳڻپ++;
	}

	EXPECT_EQ(ڳڻپ, 2);
	EXPECT_EQ(parser.getStats().spartn_messages, 2u);
}

TEST_F(SpartnTest, Chunked_آمد)
{
	خودڪار چوکٺو = build_چوکٺو(0, 1, {0x10, 0x20, 0x30});

	لكل (size_t i = 0; i < چوکٺو.سائيز(); i++) {
		parser.جوڙيو_معلومات(&چوکٺو[i], 1);
	}

	size_t len = 0;
	ASSERT_NE(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
	EXPECT_EQ(len, چوکٺو.سائيز());
}

TEST_F(SpartnTest, Noise_Before_چوکٺو)
{
	خودڪار چوکٺو = build_چوکٺو(0, 0, {0x55});
	std::ويڪٽر_2<uint8_t> وهڪرو = {0x00, 0xFF, 0x01};
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), چوکٺو.begin(), چوکٺو.پڇاڙي());
	parser.جوڙيو_معلومات(وهڪرو.معلومات(), وهڪرو.سائيز());

	size_t len = 0;
	ASSERT_NE(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
	EXPECT_GT(parser.getStats().بائيٽون_discarded_2, 0u);
}

TEST_F(SpartnTest, Bad_پيغام_چڪري_جاچ)
{
	خودڪار چوکٺو = build_چوکٺو(0, 0, {0x11, 0x22});
	چوکٺو.پٺ() ^= 0xFF;
	parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

	size_t len = 0;
	EXPECT_EQ(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
	EXPECT_GE(parser.getStats().چڪري_جاچ_errors_2, 1u);
}

TEST_F(SpartnTest, Crc16_چوکٺو)
{
	خودڪار چوکٺو = build_چوکٺو(0, 0, {0xDE, 0xAD}, 1);
	parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

	size_t len = 0;
	ASSERT_NE(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
	parser.consume_پيغام(len);
	EXPECT_EQ(parser.getStats().چڪري_جاچ_errors_2, 0u);
}

// TF002 5-119 is reserved by SPARTN 2.x, so a frame claiming one cannot be real
// even if its message CRC checks out.
TEST_F(SpartnTest, Rejects_Reserved_پيغام_Type)
{
	EXPECT_TRUE(spartn_پيغام_type_defined(0));
	EXPECT_TRUE(spartn_پيغام_type_defined(4));
	EXPECT_FALSE(spartn_پيغام_type_defined(5));
	EXPECT_FALSE(spartn_پيغام_type_defined(119));
	EXPECT_TRUE(spartn_پيغام_type_defined(120));
	EXPECT_TRUE(spartn_پيغام_type_defined(127));

	خودڪار چوکٺو = build_چوکٺو(50, 0, {0x11, 0x22});
	parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

	size_t len = 0;
	EXPECT_EQ(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
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
TEST_F(SpartnTest, Rtcm_وهڪرو_Is_Never_Framed_As_Spartn)
{
	std::mt19937 rng(1);
	std::هڪجهڙو_int_distribution<صحيح> payload_len(60, 500);

	size_t چوکٺا_اندريون = 0;
	size_t چوکٺا_ٻاهريون = 0;
	size_t generated = 0;
	std::ويڪٽر_2<uint8_t> وهڪرو;

	// ~1 MB of RTCM3, about 17 min of a 1 kB/s RTK correction stream
	طالما (generated < 1000000) {
		خودڪار چوکٺو = build_Rtcm_چوکٺو(rng, payload_len(rng));
		generated += چوکٺو.سائيز();
		چوکٺا_اندريون++;
		وهڪرو.وجهو(وهڪرو.پڇاڙي(), چوکٺو.begin(), چوکٺو.پڇاڙي());

		// Feed in MAVLink GPS_RTCM_DATA sized chunks
		طالما (وهڪرو.سائيز() >= 180) {
			parser.جوڙيو_معلومات(وهڪرو.معلومات(), 180);

			size_t len = 0;
			CorrectionProtocol protocol = CorrectionProtocol::Spartn;

			طالما (parser.get_ايندڙ_پيغام(&len, &protocol) != خالي_اشارو) {
				EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
				parser.consume_پيغام(len);
				چوکٺا_ٻاهريون++;
			}

			وهڪرو.erase(وهڪرو.begin(), وهڪرو.begin() + 180);
		}
	}

	// Every RTCM3 frame is recovered, and no part of the stream is framed as SPARTN
	EXPECT_EQ(parser.getStats().spartn_messages, 0u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, چوکٺا_ٻاهريون);
	EXPECT_GE(چوکٺا_ٻاهريون, چوکٺا_اندريون - 1);
	EXPECT_EQ(parser.getStats().چڪري_جاچ_errors_2, 0u);
}
