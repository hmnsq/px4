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

کارول نطاق gnss;

صنف SpartnTest : عام ::testing::Test
{
محمي:
	CorrectionFramer parser;

	// Build unencrypted frame with 16-bit time tag and given CRC type.
	std::ویکټور_2<uint8_t> build_چوکاټ(uint8_t پیغام_type_2, uint8_t پیغام_subtype,
					ثابت std::ویکټور_2<uint8_t> &payload,
					uint8_t دوراني_ازموینه_type = 0)
	{
		ثابت size_t payload_len = payload.اندازه();
		std::ویکټور_2<uint8_t> چوکاټ;
		چوکاټ.push_back(SPARTN_PREAMBLE);

		// TF002-TF006 (frame CRC nibble left 0; not checked by the framer)
		uint8_t b1 = static_cast<uint8_t>(((پیغام_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((دوراني_ازموینه_type & 0x03) << 4));
		چوکاټ.push_back(b1);
		چوکاټ.push_back(b2);
		چوکاټ.push_back(b3);

		// 4-byte description, 16-bit time tag
		چوکاټ.push_back(static_cast<uint8_t>((پیغام_subtype & 0x0F) << 4));
		چوکاټ.push_back(0x00);
		چوکاټ.push_back(0x00);
		چوکاټ.push_back(0x00);

		چوکاټ.ورننباسه(چوکاټ.پای_ټکی(), payload.begin(), payload.پای_ټکی());

		// Message CRC over TF002..end of payload
		ثابت uint32_t دوراني_ازموینه = spartn_پیغام_دوراني_ازموینه(&چوکاټ[1], چوکاټ.اندازه() - 1, دوراني_ازموینه_type);
		ثابت size_t دوراني_ازموینه_بایټونه = static_cast<size_t>(دوراني_ازموینه_type) + 1;

		لكل (صحيح i = static_cast<صحيح>(دوراني_ازموینه_بایټونه) - 1; i >= 0; i--) {
			چوکاټ.push_back(static_cast<uint8_t>((دوراني_ازموینه >>(8 * i)) & 0xFF));
		}

		أرجع چوکاټ;
	}

	// Build a well-formed RTCM3 frame with a deterministic pseudo-random payload.
	std::ویکټور_2<uint8_t> build_Rtcm_چوکاټ(std::mt19937 &rng, size_t payload_len)
	{
		std::ویکټور_2<uint8_t> چوکاټ;
		چوکاټ.push_back(RTCM3_PREAMBLE);
		چوکاټ.push_back(static_cast<uint8_t>((payload_len >> 8) & 0x03));
		چوکاټ.push_back(static_cast<uint8_t>(payload_len & 0xFF));

		std::یوشان_int_distribution<صحيح> byte(0, 255);

		لكل (size_t i = 0; i < payload_len; i++) {
			چوکاټ.push_back(static_cast<uint8_t>(byte(rng)));
		}

		ثابت uint32_t دوراني_ازموینه = rtcm3_crc24q(چوکاټ.معلومات(), چوکاټ.اندازه());
		چوکاټ.push_back((دوراني_ازموینه >> 16) & 0xFF);
		چوکاټ.push_back((دوراني_ازموینه >> 8) & 0xFF);
		چوکاټ.push_back(دوراني_ازموینه & 0xFF);
		أرجع چوکاټ;
	}
};

// =============================================================================
// Message CRC (TF018)
// =============================================================================

// Vectors taken from the pyspartn reference implementation's test suite
// (semuconsulting/pyspartn, tests/test_static.py), which computes the same
// TF018 CRCs independently of this code.
TEST_F(SpartnTest, پیغام_دوراني_ازموینه_Matches_Reference_Vectors)
{
	ثابت uint8_t پیغام_2[] = {'H', 'i', '!'};

	EXPECT_EQ(spartn_پیغام_دوراني_ازموینه(پیغام_2, حجم(پیغام_2), 0), 0x78u);
	EXPECT_EQ(spartn_پیغام_دوراني_ازموینه(پیغام_2, حجم(پیغام_2), 1), 0x31FDu);
	EXPECT_EQ(spartn_پیغام_دوراني_ازموینه(پیغام_2, حجم(پیغام_2), 2), 0x33220Fu);
	EXPECT_EQ(spartn_پیغام_دوراني_ازموینه(پیغام_2, حجم(پیغام_2), 3), 0x9523B4B4u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(SpartnTest, Single_اصغري_چوکاټ)
{
	خپلکاری چوکاټ = build_چوکاټ(0, 0, {0xAA, 0xBB});
	parser.جمع_کړه_معلومات(چوکاټ.معلومات(), چوکاټ.اندازه());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *پیغام_2 = parser.get_بل_پیغام(&len, &protocol);
	ASSERT_NE(پیغام_2, تش_اشاره);
	EXPECT_EQ(protocol, CorrectionProtocol::Spartn);
	EXPECT_EQ(len, چوکاټ.اندازه());
	EXPECT_EQ(0, memcmp(پیغام_2, چوکاټ.معلومات(), چوکاټ.اندازه()));
	parser.consume_پیغام(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
	EXPECT_EQ(parser.getStats().دوراني_ازموینه_errors_2, 0u);
}

TEST_F(SpartnTest, مضرب_چوکاټونه)
{
	خپلکاری f1 = build_چوکاټ(0, 0, {0x01});
	خپلکاری f2 = build_چوکاټ(1, 0, {0x02, 0x03});
	std::ویکټور_2<uint8_t> بهیر;
	بهیر.ورننباسه(بهیر.پای_ټکی(), f1.begin(), f1.پای_ټکی());
	بهیر.ورننباسه(بهیر.پای_ټکی(), f2.begin(), f2.پای_ټکی());
	parser.جمع_کړه_معلومات(بهیر.معلومات(), بهیر.اندازه());

	صحيح شمېر = 0;
	size_t len = 0;

	طالما (parser.get_بل_پیغام(&len) != تش_اشاره) {
		parser.consume_پیغام(len);
		شمېر++;
	}

	EXPECT_EQ(شمېر, 2);
	EXPECT_EQ(parser.getStats().spartn_messages, 2u);
}

TEST_F(SpartnTest, Chunked_رارسېدنه)
{
	خپلکاری چوکاټ = build_چوکاټ(0, 1, {0x10, 0x20, 0x30});

	لكل (size_t i = 0; i < چوکاټ.اندازه(); i++) {
		parser.جمع_کړه_معلومات(&چوکاټ[i], 1);
	}

	size_t len = 0;
	ASSERT_NE(parser.get_بل_پیغام(&len), تش_اشاره);
	EXPECT_EQ(len, چوکاټ.اندازه());
}

TEST_F(SpartnTest, Noise_Before_چوکاټ)
{
	خپلکاری چوکاټ = build_چوکاټ(0, 0, {0x55});
	std::ویکټور_2<uint8_t> بهیر = {0x00, 0xFF, 0x01};
	بهیر.ورننباسه(بهیر.پای_ټکی(), چوکاټ.begin(), چوکاټ.پای_ټکی());
	parser.جمع_کړه_معلومات(بهیر.معلومات(), بهیر.اندازه());

	size_t len = 0;
	ASSERT_NE(parser.get_بل_پیغام(&len), تش_اشاره);
	EXPECT_GT(parser.getStats().بایټونه_discarded_2, 0u);
}

TEST_F(SpartnTest, Bad_پیغام_دوراني_ازموینه)
{
	خپلکاری چوکاټ = build_چوکاټ(0, 0, {0x11, 0x22});
	چوکاټ.شا() ^= 0xFF;
	parser.جمع_کړه_معلومات(چوکاټ.معلومات(), چوکاټ.اندازه());

	size_t len = 0;
	EXPECT_EQ(parser.get_بل_پیغام(&len), تش_اشاره);
	EXPECT_GE(parser.getStats().دوراني_ازموینه_errors_2, 1u);
}

TEST_F(SpartnTest, Crc16_چوکاټ)
{
	خپلکاری چوکاټ = build_چوکاټ(0, 0, {0xDE, 0xAD}, 1);
	parser.جمع_کړه_معلومات(چوکاټ.معلومات(), چوکاټ.اندازه());

	size_t len = 0;
	ASSERT_NE(parser.get_بل_پیغام(&len), تش_اشاره);
	parser.consume_پیغام(len);
	EXPECT_EQ(parser.getStats().دوراني_ازموینه_errors_2, 0u);
}

// TF002 5-119 is reserved by SPARTN 2.x, so a frame claiming one cannot be real
// even if its message CRC checks out.
TEST_F(SpartnTest, Rejects_Reserved_پیغام_Type)
{
	EXPECT_TRUE(spartn_پیغام_type_defined(0));
	EXPECT_TRUE(spartn_پیغام_type_defined(4));
	EXPECT_FALSE(spartn_پیغام_type_defined(5));
	EXPECT_FALSE(spartn_پیغام_type_defined(119));
	EXPECT_TRUE(spartn_پیغام_type_defined(120));
	EXPECT_TRUE(spartn_پیغام_type_defined(127));

	خپلکاری چوکاټ = build_چوکاټ(50, 0, {0x11, 0x22});
	parser.جمع_کړه_معلومات(چوکاټ.معلومات(), چوکاټ.اندازه());

	size_t len = 0;
	EXPECT_EQ(parser.get_بل_پیغام(&len), تش_اشاره);
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
TEST_F(SpartnTest, Rtcm_بهیر_Is_Never_Framed_As_Spartn)
{
	std::mt19937 rng(1);
	std::یوشان_int_distribution<صحيح> payload_len(60, 500);

	size_t چوکاټونه_دننه = 0;
	size_t چوکاټونه_بهر = 0;
	size_t generated = 0;
	std::ویکټور_2<uint8_t> بهیر;

	// ~1 MB of RTCM3, about 17 min of a 1 kB/s RTK correction stream
	طالما (generated < 1000000) {
		خپلکاری چوکاټ = build_Rtcm_چوکاټ(rng, payload_len(rng));
		generated += چوکاټ.اندازه();
		چوکاټونه_دننه++;
		بهیر.ورننباسه(بهیر.پای_ټکی(), چوکاټ.begin(), چوکاټ.پای_ټکی());

		// Feed in MAVLink GPS_RTCM_DATA sized chunks
		طالما (بهیر.اندازه() >= 180) {
			parser.جمع_کړه_معلومات(بهیر.معلومات(), 180);

			size_t len = 0;
			CorrectionProtocol protocol = CorrectionProtocol::Spartn;

			طالما (parser.get_بل_پیغام(&len, &protocol) != تش_اشاره) {
				EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
				parser.consume_پیغام(len);
				چوکاټونه_بهر++;
			}

			بهیر.erase(بهیر.begin(), بهیر.begin() + 180);
		}
	}

	// Every RTCM3 frame is recovered, and no part of the stream is framed as SPARTN
	EXPECT_EQ(parser.getStats().spartn_messages, 0u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, چوکاټونه_بهر);
	EXPECT_GE(چوکاټونه_بهر, چوکاټونه_دننه - 1);
	EXPECT_EQ(parser.getStats().دوراني_ازموینه_errors_2, 0u);
}
