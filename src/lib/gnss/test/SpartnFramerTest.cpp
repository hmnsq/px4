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

استفاده نطاق gnss;

صنف SpartnTest : عام ::testing::Test
{
محمي:
	CorrectionFramer parser;

	// Build unencrypted frame with 16-bit time tag and given CRC type.
	std::وکتور_2<uint8_t> build_قاب(uint8_t پیام_type_2, uint8_t پیام_subtype,
					ثابت std::وکتور_2<uint8_t> &payload,
					uint8_t بررسی_چرخهای_type = 0)
	{
		ثابت size_t payload_len = payload.اندازه();
		std::وکتور_2<uint8_t> قاب;
		قاب.push_back(SPARTN_PREAMBLE);

		// TF002-TF006 (frame CRC nibble left 0; not checked by the framer)
		uint8_t b1 = static_cast<uint8_t>(((پیام_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((بررسی_چرخهای_type & 0x03) << 4));
		قاب.push_back(b1);
		قاب.push_back(b2);
		قاب.push_back(b3);

		// 4-byte description, 16-bit time tag
		قاب.push_back(static_cast<uint8_t>((پیام_subtype & 0x0F) << 4));
		قاب.push_back(0x00);
		قاب.push_back(0x00);
		قاب.push_back(0x00);

		قاب.درج(قاب.انتها(), payload.begin(), payload.انتها());

		// Message CRC over TF002..end of payload
		ثابت uint32_t بررسی_چرخهای = spartn_پیام_بررسی_چرخهای(&قاب[1], قاب.اندازه() - 1, بررسی_چرخهای_type);
		ثابت size_t بررسی_چرخهای_بایتها = static_cast<size_t>(بررسی_چرخهای_type) + 1;

		لكل (صحيح i = static_cast<صحيح>(بررسی_چرخهای_بایتها) - 1; i >= 0; i--) {
			قاب.push_back(static_cast<uint8_t>((بررسی_چرخهای >>(8 * i)) & 0xFF));
		}

		أرجع قاب;
	}

	// Build a well-formed RTCM3 frame with a deterministic pseudo-random payload.
	std::وکتور_2<uint8_t> build_Rtcm_قاب(std::mt19937 &rng, size_t payload_len)
	{
		std::وکتور_2<uint8_t> قاب;
		قاب.push_back(RTCM3_PREAMBLE);
		قاب.push_back(static_cast<uint8_t>((payload_len >> 8) & 0x03));
		قاب.push_back(static_cast<uint8_t>(payload_len & 0xFF));

		std::یکنواخت_int_distribution<صحيح> byte(0, 255);

		لكل (size_t i = 0; i < payload_len; i++) {
			قاب.push_back(static_cast<uint8_t>(byte(rng)));
		}

		ثابت uint32_t بررسی_چرخهای = rtcm3_crc24q(قاب.داده(), قاب.اندازه());
		قاب.push_back((بررسی_چرخهای >> 16) & 0xFF);
		قاب.push_back((بررسی_چرخهای >> 8) & 0xFF);
		قاب.push_back(بررسی_چرخهای & 0xFF);
		أرجع قاب;
	}
};

// =============================================================================
// Message CRC (TF018)
// =============================================================================

// Vectors taken from the pyspartn reference implementation's test suite
// (semuconsulting/pyspartn, tests/test_static.py), which computes the same
// TF018 CRCs independently of this code.
TEST_F(SpartnTest, پیام_بررسی_چرخهای_Matches_Reference_Vectors)
{
	ثابت uint8_t پیام_2[] = {'H', 'i', '!'};

	EXPECT_EQ(spartn_پیام_بررسی_چرخهای(پیام_2, حجم(پیام_2), 0), 0x78u);
	EXPECT_EQ(spartn_پیام_بررسی_چرخهای(پیام_2, حجم(پیام_2), 1), 0x31FDu);
	EXPECT_EQ(spartn_پیام_بررسی_چرخهای(پیام_2, حجم(پیام_2), 2), 0x33220Fu);
	EXPECT_EQ(spartn_پیام_بررسی_چرخهای(پیام_2, حجم(پیام_2), 3), 0x9523B4B4u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(SpartnTest, Single_کمینه_قاب)
{
	خودکار قاب = build_قاب(0, 0, {0xAA, 0xBB});
	parser.جمع_کن_داده(قاب.داده(), قاب.اندازه());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *پیام_2 = parser.get_بعدی_پیام(&len, &protocol);
	ASSERT_NE(پیام_2, اشاره_تهی);
	EXPECT_EQ(protocol, CorrectionProtocol::Spartn);
	EXPECT_EQ(len, قاب.اندازه());
	EXPECT_EQ(0, memcmp(پیام_2, قاب.داده(), قاب.اندازه()));
	parser.consume_پیام(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
	EXPECT_EQ(parser.getStats().بررسی_چرخهای_errors_2, 0u);
}

TEST_F(SpartnTest, مضرب_قابها)
{
	خودکار f1 = build_قاب(0, 0, {0x01});
	خودکار f2 = build_قاب(1, 0, {0x02, 0x03});
	std::وکتور_2<uint8_t> جریان;
	جریان.درج(جریان.انتها(), f1.begin(), f1.انتها());
	جریان.درج(جریان.انتها(), f2.begin(), f2.انتها());
	parser.جمع_کن_داده(جریان.داده(), جریان.اندازه());

	صحيح شمار = 0;
	size_t len = 0;

	طالما (parser.get_بعدی_پیام(&len) != اشاره_تهی) {
		parser.consume_پیام(len);
		شمار++;
	}

	EXPECT_EQ(شمار, 2);
	EXPECT_EQ(parser.getStats().spartn_messages, 2u);
}

TEST_F(SpartnTest, Chunked_رسیدن)
{
	خودکار قاب = build_قاب(0, 1, {0x10, 0x20, 0x30});

	لكل (size_t i = 0; i < قاب.اندازه(); i++) {
		parser.جمع_کن_داده(&قاب[i], 1);
	}

	size_t len = 0;
	ASSERT_NE(parser.get_بعدی_پیام(&len), اشاره_تهی);
	EXPECT_EQ(len, قاب.اندازه());
}

TEST_F(SpartnTest, Noise_Before_قاب)
{
	خودکار قاب = build_قاب(0, 0, {0x55});
	std::وکتور_2<uint8_t> جریان = {0x00, 0xFF, 0x01};
	جریان.درج(جریان.انتها(), قاب.begin(), قاب.انتها());
	parser.جمع_کن_داده(جریان.داده(), جریان.اندازه());

	size_t len = 0;
	ASSERT_NE(parser.get_بعدی_پیام(&len), اشاره_تهی);
	EXPECT_GT(parser.getStats().بایتها_discarded_2, 0u);
}

TEST_F(SpartnTest, Bad_پیام_بررسی_چرخهای)
{
	خودکار قاب = build_قاب(0, 0, {0x11, 0x22});
	قاب.پشت() ^= 0xFF;
	parser.جمع_کن_داده(قاب.داده(), قاب.اندازه());

	size_t len = 0;
	EXPECT_EQ(parser.get_بعدی_پیام(&len), اشاره_تهی);
	EXPECT_GE(parser.getStats().بررسی_چرخهای_errors_2, 1u);
}

TEST_F(SpartnTest, Crc16_قاب)
{
	خودکار قاب = build_قاب(0, 0, {0xDE, 0xAD}, 1);
	parser.جمع_کن_داده(قاب.داده(), قاب.اندازه());

	size_t len = 0;
	ASSERT_NE(parser.get_بعدی_پیام(&len), اشاره_تهی);
	parser.consume_پیام(len);
	EXPECT_EQ(parser.getStats().بررسی_چرخهای_errors_2, 0u);
}

// TF002 5-119 is reserved by SPARTN 2.x, so a frame claiming one cannot be real
// even if its message CRC checks out.
TEST_F(SpartnTest, Rejects_Reserved_پیام_Type)
{
	EXPECT_TRUE(spartn_پیام_type_defined(0));
	EXPECT_TRUE(spartn_پیام_type_defined(4));
	EXPECT_FALSE(spartn_پیام_type_defined(5));
	EXPECT_FALSE(spartn_پیام_type_defined(119));
	EXPECT_TRUE(spartn_پیام_type_defined(120));
	EXPECT_TRUE(spartn_پیام_type_defined(127));

	خودکار قاب = build_قاب(50, 0, {0x11, 0x22});
	parser.جمع_کن_داده(قاب.داده(), قاب.اندازه());

	size_t len = 0;
	EXPECT_EQ(parser.get_بعدی_پیام(&len), اشاره_تهی);
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
TEST_F(SpartnTest, Rtcm_جریان_Is_Never_Framed_As_Spartn)
{
	std::mt19937 rng(1);
	std::یکنواخت_int_distribution<صحيح> payload_len(60, 500);

	size_t قابها_درون = 0;
	size_t قابها_بیرون = 0;
	size_t generated = 0;
	std::وکتور_2<uint8_t> جریان;

	// ~1 MB of RTCM3, about 17 min of a 1 kB/s RTK correction stream
	طالما (generated < 1000000) {
		خودکار قاب = build_Rtcm_قاب(rng, payload_len(rng));
		generated += قاب.اندازه();
		قابها_درون++;
		جریان.درج(جریان.انتها(), قاب.begin(), قاب.انتها());

		// Feed in MAVLink GPS_RTCM_DATA sized chunks
		طالما (جریان.اندازه() >= 180) {
			parser.جمع_کن_داده(جریان.داده(), 180);

			size_t len = 0;
			CorrectionProtocol protocol = CorrectionProtocol::Spartn;

			طالما (parser.get_بعدی_پیام(&len, &protocol) != اشاره_تهی) {
				EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
				parser.consume_پیام(len);
				قابها_بیرون++;
			}

			جریان.erase(جریان.begin(), جریان.begin() + 180);
		}
	}

	// Every RTCM3 frame is recovered, and no part of the stream is framed as SPARTN
	EXPECT_EQ(parser.getStats().spartn_messages, 0u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, قابها_بیرون);
	EXPECT_GE(قابها_بیرون, قابها_درون - 1);
	EXPECT_EQ(parser.getStats().بررسی_چرخهای_errors_2, 0u);
}
