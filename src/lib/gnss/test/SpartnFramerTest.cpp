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

ورتوں نطاق gnss;

صنف SpartnTest : عام ::testing::Test
{
محمي:
	CorrectionFramer parser;

	// Build unencrypted frame with 16-bit time tag and given CRC type.
	std::سمتیہ_2<uint8_t> build_چوکھٹا(uint8_t پیغام_type, uint8_t پیغام_subtype,
					ثابت std::سمتیہ_2<uint8_t> &payload,
					uint8_t چکری_جانچ_type = 0)
	{
		ثابت size_t payload_len = payload.ناپ();
		std::سمتیہ_2<uint8_t> چوکھٹا;
		چوکھٹا.push_back(SPARTN_PREAMBLE);

		// TF002-TF006 (frame CRC nibble left 0; not checked by the framer)
		uint8_t b1 = static_cast<uint8_t>(((پیغام_type & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((چکری_جانچ_type & 0x03) << 4));
		چوکھٹا.push_back(b1);
		چوکھٹا.push_back(b2);
		چوکھٹا.push_back(b3);

		// 4-byte description, 16-bit time tag
		چوکھٹا.push_back(static_cast<uint8_t>((پیغام_subtype & 0x0F) << 4));
		چوکھٹا.push_back(0x00);
		چوکھٹا.push_back(0x00);
		چوکھٹا.push_back(0x00);

		چوکھٹا.داخل_کرو(چوکھٹا.سرا(), payload.begin(), payload.سرا());

		// Message CRC over TF002..end of payload
		ثابت uint32_t چکری_جانچ = spartn_سنیہا_چکری_جانچ(&چوکھٹا[1], چوکھٹا.ناپ() - 1, چکری_جانچ_type);
		ثابت size_t چکری_جانچ_بائٹاں = static_cast<size_t>(چکری_جانچ_type) + 1;

		لكل (صحيح i = static_cast<صحيح>(چکری_جانچ_بائٹاں) - 1; i >= 0; i--) {
			چوکھٹا.push_back(static_cast<uint8_t>((چکری_جانچ >>(8 * i)) & 0xFF));
		}

		أرجع چوکھٹا;
	}

	// Build a well-formed RTCM3 frame with a deterministic pseudo-random payload.
	std::سمتیہ_2<uint8_t> build_Rtcm_چوکھٹا(std::mt19937 &rng, size_t payload_len)
	{
		std::سمتیہ_2<uint8_t> چوکھٹا;
		چوکھٹا.push_back(RTCM3_PREAMBLE);
		چوکھٹا.push_back(static_cast<uint8_t>((payload_len >> 8) & 0x03));
		چوکھٹا.push_back(static_cast<uint8_t>(payload_len & 0xFF));

		std::اکسار_int_distribution<صحيح> byte(0, 255);

		لكل (size_t i = 0; i < payload_len; i++) {
			چوکھٹا.push_back(static_cast<uint8_t>(byte(rng)));
		}

		ثابت uint32_t چکری_جانچ = rtcm3_crc24q(چوکھٹا.انکڑے(), چوکھٹا.ناپ());
		چوکھٹا.push_back((چکری_جانچ >> 16) & 0xFF);
		چوکھٹا.push_back((چکری_جانچ >> 8) & 0xFF);
		چوکھٹا.push_back(چکری_جانچ & 0xFF);
		أرجع چوکھٹا;
	}
};

// =============================================================================
// Message CRC (TF018)
// =============================================================================

// Vectors taken from the pyspartn reference implementation's test suite
// (semuconsulting/pyspartn, tests/test_static.py), which computes the same
// TF018 CRCs independently of this code.
TEST_F(SpartnTest, سنیہا_چکری_جانچ_Matches_Reference_Vectors)
{
	ثابت uint8_t پیغام[] = {'H', 'i', '!'};

	EXPECT_EQ(spartn_سنیہا_چکری_جانچ(پیغام, حجم(پیغام), 0), 0x78u);
	EXPECT_EQ(spartn_سنیہا_چکری_جانچ(پیغام, حجم(پیغام), 1), 0x31FDu);
	EXPECT_EQ(spartn_سنیہا_چکری_جانچ(پیغام, حجم(پیغام), 2), 0x33220Fu);
	EXPECT_EQ(spartn_سنیہا_چکری_جانچ(پیغام, حجم(پیغام), 3), 0x9523B4B4u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(SpartnTest, Single_اقل_چوکھٹا)
{
	خودکار چوکھٹا = build_چوکھٹا(0, 0, {0xAA, 0xBB});
	parser.جوڑو_انکڑے(چوکھٹا.انکڑے(), چوکھٹا.ناپ());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *پیغام = parser.get_اگلا_سنیہا(&len, &protocol);
	ASSERT_NE(پیغام, خالی_سوچک);
	EXPECT_EQ(protocol, CorrectionProtocol::Spartn);
	EXPECT_EQ(len, چوکھٹا.ناپ());
	EXPECT_EQ(0, memcmp(پیغام, چوکھٹا.انکڑے(), چوکھٹا.ناپ()));
	parser.consume_سنیہا(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
	EXPECT_EQ(parser.getStats().چکری_جانچ_errors_2, 0u);
}

TEST_F(SpartnTest, مضرب_چوکھٹے)
{
	خودکار f1 = build_چوکھٹا(0, 0, {0x01});
	خودکار f2 = build_چوکھٹا(1, 0, {0x02, 0x03});
	std::سمتیہ_2<uint8_t> دھارا;
	دھارا.داخل_کرو(دھارا.سرا(), f1.begin(), f1.سرا());
	دھارا.داخل_کرو(دھارا.سرا(), f2.begin(), f2.سرا());
	parser.جوڑو_انکڑے(دھارا.انکڑے(), دھارا.ناپ());

	صحيح گنتی = 0;
	size_t len = 0;

	طالما (parser.get_اگلا_سنیہا(&len) != خالی_سوچک) {
		parser.consume_سنیہا(len);
		گنتی++;
	}

	EXPECT_EQ(گنتی, 2);
	EXPECT_EQ(parser.getStats().spartn_messages, 2u);
}

TEST_F(SpartnTest, Chunked_آمد)
{
	خودکار چوکھٹا = build_چوکھٹا(0, 1, {0x10, 0x20, 0x30});

	لكل (size_t i = 0; i < چوکھٹا.ناپ(); i++) {
		parser.جوڑو_انکڑے(&چوکھٹا[i], 1);
	}

	size_t len = 0;
	ASSERT_NE(parser.get_اگلا_سنیہا(&len), خالی_سوچک);
	EXPECT_EQ(len, چوکھٹا.ناپ());
}

TEST_F(SpartnTest, Noise_Before_چوکھٹا)
{
	خودکار چوکھٹا = build_چوکھٹا(0, 0, {0x55});
	std::سمتیہ_2<uint8_t> دھارا = {0x00, 0xFF, 0x01};
	دھارا.داخل_کرو(دھارا.سرا(), چوکھٹا.begin(), چوکھٹا.سرا());
	parser.جوڑو_انکڑے(دھارا.انکڑے(), دھارا.ناپ());

	size_t len = 0;
	ASSERT_NE(parser.get_اگلا_سنیہا(&len), خالی_سوچک);
	EXPECT_GT(parser.getStats().بائٹاں_discarded_2, 0u);
}

TEST_F(SpartnTest, Bad_سنیہا_چکری_جانچ)
{
	خودکار چوکھٹا = build_چوکھٹا(0, 0, {0x11, 0x22});
	چوکھٹا.پٹھ() ^= 0xFF;
	parser.جوڑو_انکڑے(چوکھٹا.انکڑے(), چوکھٹا.ناپ());

	size_t len = 0;
	EXPECT_EQ(parser.get_اگلا_سنیہا(&len), خالی_سوچک);
	EXPECT_GE(parser.getStats().چکری_جانچ_errors_2, 1u);
}

TEST_F(SpartnTest, Crc16_چوکھٹا)
{
	خودکار چوکھٹا = build_چوکھٹا(0, 0, {0xDE, 0xAD}, 1);
	parser.جوڑو_انکڑے(چوکھٹا.انکڑے(), چوکھٹا.ناپ());

	size_t len = 0;
	ASSERT_NE(parser.get_اگلا_سنیہا(&len), خالی_سوچک);
	parser.consume_سنیہا(len);
	EXPECT_EQ(parser.getStats().چکری_جانچ_errors_2, 0u);
}

// TF002 5-119 is reserved by SPARTN 2.x, so a frame claiming one cannot be real
// even if its message CRC checks out.
TEST_F(SpartnTest, Rejects_Reserved_سنیہا_Type)
{
	EXPECT_TRUE(spartn_سنیہا_type_defined(0));
	EXPECT_TRUE(spartn_سنیہا_type_defined(4));
	EXPECT_FALSE(spartn_سنیہا_type_defined(5));
	EXPECT_FALSE(spartn_سنیہا_type_defined(119));
	EXPECT_TRUE(spartn_سنیہا_type_defined(120));
	EXPECT_TRUE(spartn_سنیہا_type_defined(127));

	خودکار چوکھٹا = build_چوکھٹا(50, 0, {0x11, 0x22});
	parser.جوڑو_انکڑے(چوکھٹا.انکڑے(), چوکھٹا.ناپ());

	size_t len = 0;
	EXPECT_EQ(parser.get_اگلا_سنیہا(&len), خالی_سوچک);
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
TEST_F(SpartnTest, Rtcm_دھارا_Is_Never_Framed_As_Spartn)
{
	std::mt19937 rng(1);
	std::اکسار_int_distribution<صحيح> payload_len(60, 500);

	size_t چوکھٹے_وچ = 0;
	size_t چوکھٹے_باہرلا = 0;
	size_t generated = 0;
	std::سمتیہ_2<uint8_t> دھارا;

	// ~1 MB of RTCM3, about 17 min of a 1 kB/s RTK correction stream
	طالما (generated < 1000000) {
		خودکار چوکھٹا = build_Rtcm_چوکھٹا(rng, payload_len(rng));
		generated += چوکھٹا.ناپ();
		چوکھٹے_وچ++;
		دھارا.داخل_کرو(دھارا.سرا(), چوکھٹا.begin(), چوکھٹا.سرا());

		// Feed in MAVLink GPS_RTCM_DATA sized chunks
		طالما (دھارا.ناپ() >= 180) {
			parser.جوڑو_انکڑے(دھارا.انکڑے(), 180);

			size_t len = 0;
			CorrectionProtocol protocol = CorrectionProtocol::Spartn;

			طالما (parser.get_اگلا_سنیہا(&len, &protocol) != خالی_سوچک) {
				EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
				parser.consume_سنیہا(len);
				چوکھٹے_باہرلا++;
			}

			دھارا.erase(دھارا.begin(), دھارا.begin() + 180);
		}
	}

	// Every RTCM3 frame is recovered, and no part of the stream is framed as SPARTN
	EXPECT_EQ(parser.getStats().spartn_messages, 0u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, چوکھٹے_باہرلا);
	EXPECT_GE(چوکھٹے_باہرلا, چوکھٹے_وچ - 1);
	EXPECT_EQ(parser.getStats().چکری_جانچ_errors_2, 0u);
}
