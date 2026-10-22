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
 * @file UbxFramerTest.cpp
 *
 * UBX framing tests for CorrectionFramer (AssistNow MGA pass-through), including
 * interleaved UBX then RTCM3/SPARTN streams that mirror PointPerfect -MGA
 * mountpoints.
 */

#ضمن <gtest/gtest.h>
#ضمن "correction_framer.h"

#ضمن <vector>
#ضمن <cstring>
#ضمن <random>

استفاده نطاق gnss;

صنف UbxTest : عام ::testing::Test
{
محمي:
	CorrectionFramer parser;

	// Build a valid UBX frame (class/id arbitrary; content not decoded).
	std::وکتور_2<uint8_t> build_Ubx_قاب(uint8_t پیام_class, uint8_t پیام_id,
					   ثابت std::وکتور_2<uint8_t> &payload)
	{
		ثابت size_t payload_len = payload.اندازه();
		std::وکتور_2<uint8_t> قاب;
		قاب.push_back(UBX_SYNC1);
		قاب.push_back(UBX_SYNC2);
		قاب.push_back(پیام_class);
		قاب.push_back(پیام_id);
		قاب.push_back(static_cast<uint8_t>(payload_len & 0xFF));
		قاب.push_back(static_cast<uint8_t>((payload_len >> 8) & 0xFF));
		قاب.درج(قاب.انتها(), payload.begin(), payload.انتها());

		uint8_t ck_a = 0;
		uint8_t ck_b = 0;
		ubx_checksum(&قاب[2], UBX_سرآیند_LEN - 2 + payload_len, &ck_a, &ck_b);
		قاب.push_back(ck_a);
		قاب.push_back(ck_b);
		أرجع قاب;
	}

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

#إذا defined(CONFIG_GPS_SPARTN)
	std::وکتور_2<uint8_t> build_Spartn_قاب(uint8_t پیام_type_2, ثابت std::وکتور_2<uint8_t> &payload,
					      uint8_t بررسی_چرخهای_type = 0)
	{
		ثابت size_t payload_len = payload.اندازه();
		std::وکتور_2<uint8_t> قاب;
		قاب.push_back(SPARTN_PREAMBLE);

		uint8_t b1 = static_cast<uint8_t>(((پیام_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((بررسی_چرخهای_type & 0x03) << 4));
		قاب.push_back(b1);
		قاب.push_back(b2);
		قاب.push_back(b3);

		// 4-byte description, 16-bit time tag
		قاب.push_back(0x00);
		قاب.push_back(0x00);
		قاب.push_back(0x00);
		قاب.push_back(0x00);

		قاب.درج(قاب.انتها(), payload.begin(), payload.انتها());

		ثابت uint32_t بررسی_چرخهای = spartn_پیام_بررسی_چرخهای(&قاب[1], قاب.اندازه() - 1, بررسی_چرخهای_type);
		ثابت size_t بررسی_چرخهای_بایتها = static_cast<size_t>(بررسی_چرخهای_type) + 1;

		لكل (صحيح i = static_cast<صحيح>(بررسی_چرخهای_بایتها) - 1; i >= 0; i--) {
			قاب.push_back(static_cast<uint8_t>((بررسی_چرخهای >>(8 * i)) & 0xFF));
		}

		أرجع قاب;
	}
#پایان_اگر
};

// =============================================================================
// Checksum
// =============================================================================

// Empty ACK-ACK (class=0x05, id=0x01, length=0): covered bytes 05 01 00 00.
// 8-bit Fletcher: a runs 5,6,6,6 and b runs 5,11,17,23 → CK_A=0x06, CK_B=0x17.
TEST_F(UbxTest, Checksum_معلوم_وکتور)
{
	ثابت uint8_t covered[] = {0x05, 0x01, 0x00, 0x00};
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(covered, حجم(covered), &ck_a, &ck_b);
	EXPECT_EQ(ck_a, 0x06u);
	EXPECT_EQ(ck_b, 0x17u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(UbxTest, Single_کمینه_قاب)
{
	// UBX-MGA class is 0x13; any class/id is fine for framing
	خودکار قاب = build_Ubx_قاب(0x13, 0x00, {0xAA, 0xBB});
	parser.جمع_کن_داده(قاب.داده(), قاب.اندازه());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *پیام_2 = parser.get_بعدی_پیام(&len, &protocol);
	ASSERT_NE(پیام_2, اشاره_تهی);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, قاب.اندازه());
	EXPECT_EQ(0, memcmp(پیام_2, قاب.داده(), قاب.اندازه()));
	parser.consume_پیام(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().بررسی_چرخهای_errors_2, 0u);
}

TEST_F(UbxTest, خالی_Payload_قاب)
{
	خودکار قاب = build_Ubx_قاب(0x05, 0x01, {});
	parser.جمع_کن_داده(قاب.داده(), قاب.اندازه());

	size_t len = 0;
	ASSERT_NE(parser.get_بعدی_پیام(&len), اشاره_تهی);
	EXPECT_EQ(len, UBX_سرآیند_LEN + UBX_CK_LEN);
	parser.consume_پیام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, مضرب_قابها)
{
	خودکار f1 = build_Ubx_قاب(0x13, 0x00, {0x01});
	خودکار f2 = build_Ubx_قاب(0x13, 0x02, {0x02, 0x03, 0x04});
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
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
}

TEST_F(UbxTest, Chunked_رسیدن)
{
	خودکار قاب = build_Ubx_قاب(0x13, 0x01, {0x10, 0x20, 0x30, 0x40});

	لكل (size_t i = 0; i < قاب.اندازه(); i++) {
		parser.جمع_کن_داده(&قاب[i], 1);
		// No complete frame until the last byte
	}

	size_t len = 0;
	ASSERT_NE(parser.get_بعدی_پیام(&len), اشاره_تهی);
	EXPECT_EQ(len, قاب.اندازه());
}

TEST_F(UbxTest, Noise_Before_قاب)
{
	خودکار قاب = build_Ubx_قاب(0x13, 0x00, {0x55});
	std::وکتور_2<uint8_t> جریان = {0x00, 0xFF, 0x01};
	جریان.درج(جریان.انتها(), قاب.begin(), قاب.انتها());
	parser.جمع_کن_داده(جریان.داده(), جریان.اندازه());

	size_t len = 0;
	ASSERT_NE(parser.get_بعدی_پیام(&len), اشاره_تهی);
	EXPECT_GT(parser.getStats().بایتها_discarded_2, 0u);
	parser.consume_پیام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, BadSync2)
{
	// 0xB5 followed by something other than 0x62 is not a frame start
	std::وکتور_2<uint8_t> جریان = {UBX_SYNC1, 0x00, 0x11, 0x22};
	خودکار قاب = build_Ubx_قاب(0x13, 0x00, {0x01});
	جریان.درج(جریان.انتها(), قاب.begin(), قاب.انتها());
	parser.جمع_کن_داده(جریان.داده(), جریان.اندازه());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_بعدی_پیام(&len, &protocol), اشاره_تهی);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_پیام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_GT(parser.getStats().بایتها_discarded_2, 0u);
}

TEST_F(UbxTest, BadChecksum)
{
	خودکار قاب = build_Ubx_قاب(0x13, 0x00, {0x11, 0x22});
	قاب.پشت() ^= 0xFF;
	parser.جمع_کن_داده(قاب.داده(), قاب.اندازه());

	size_t len = 0;
	EXPECT_EQ(parser.get_بعدی_پیام(&len), اشاره_تهی);
	EXPECT_GE(parser.getStats().بررسی_چرخهای_errors_2, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 0u);
}

TEST_F(UbxTest, RejectsOversizedPayload)
{
	// Length field claims UBX_MAX_PAYLOAD_LEN + 1 — must not pin the buffer
	std::وکتور_2<uint8_t> سرآیند = {
		UBX_SYNC1, UBX_SYNC2,
		0x13, 0x00,
		static_cast<uint8_t>((UBX_MAX_PAYLOAD_LEN + 1) & 0xFF),
		static_cast<uint8_t>(((UBX_MAX_PAYLOAD_LEN + 1) >> 8) & 0xFF),
	};
	خودکار good = build_Ubx_قاب(0x13, 0x00, {0xAB});
	سرآیند.درج(سرآیند.انتها(), good.begin(), good.انتها());
	parser.جمع_کن_داده(سرآیند.داده(), سرآیند.اندازه());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_بعدی_پیام(&len, &protocol), اشاره_تهی);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, good.اندازه());
	parser.consume_پیام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

// =============================================================================
// Cross-protocol: MGA then corrections (the PointPerfect -MGA case)
// =============================================================================

// PointPerfect -MGA mountpoints deliver a UBX-MGA burst once on connect, then
// the normal correction stream (RTCM3 or SPARTN). The framer must not lock onto
// UBX and reject everything after — each complete frame is consumed and the next
// preamble (of any protocol) starts the next frame.
TEST_F(UbxTest, MgaThenRtcm3)
{
	std::mt19937 rng(42);
	خودکار mga1 = build_Ubx_قاب(0x13, 0x00, {0x01, 0x02, 0x03});
	خودکار mga2 = build_Ubx_قاب(0x13, 0x02, {0x10, 0x20});
	خودکار rtcm = build_Rtcm_قاب(rng, 40);

	std::وکتور_2<uint8_t> جریان;
	جریان.درج(جریان.انتها(), mga1.begin(), mga1.انتها());
	جریان.درج(جریان.انتها(), mga2.begin(), mga2.انتها());
	جریان.درج(جریان.انتها(), rtcm.begin(), rtcm.انتها());
	parser.جمع_کن_داده(جریان.داده(), جریان.اندازه());

	std::وکتور_2<CorrectionProtocol> دیده;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	طالما (parser.get_بعدی_پیام(&len, &protocol) != اشاره_تهی) {
		دیده.push_back(protocol);
		parser.consume_پیام(len);
	}

	ASSERT_EQ(دیده.اندازه(), 3u);
	EXPECT_EQ(دیده[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(دیده[1], CorrectionProtocol::Ubx);
	EXPECT_EQ(دیده[2], CorrectionProtocol::Rtcm3);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().بررسی_چرخهای_errors_2, 0u);
}

#إذا defined(CONFIG_GPS_SPARTN)
TEST_F(UbxTest, MgaThenSpartn)
{
	خودکار mga = build_Ubx_قاب(0x13, 0x00, {0xDE, 0xAD});
	خودکار spartn = build_Spartn_قاب(0, {0xAA, 0xBB});

	std::وکتور_2<uint8_t> جریان;
	جریان.درج(جریان.انتها(), mga.begin(), mga.انتها());
	جریان.درج(جریان.انتها(), spartn.begin(), spartn.انتها());
	// Another MGA mid-stream should also work (reconnect-style)
	خودکار mga2 = build_Ubx_قاب(0x13, 0x01, {0x01});
	جریان.درج(جریان.انتها(), mga2.begin(), mga2.انتها());
	parser.جمع_کن_داده(جریان.داده(), جریان.اندازه());

	std::وکتور_2<CorrectionProtocol> دیده;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	طالما (parser.get_بعدی_پیام(&len, &protocol) != اشاره_تهی) {
		دیده.push_back(protocol);
		parser.consume_پیام(len);
	}

	ASSERT_EQ(دیده.اندازه(), 3u);
	EXPECT_EQ(دیده[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(دیده[1], CorrectionProtocol::Spartn);
	EXPECT_EQ(دیده[2], CorrectionProtocol::Ubx);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
}
#پایان_اگر

// A complete RTCM3 frame consumes its payload, so a 0xB5 that happens to sit
// inside that payload is never considered as a UBX start.
TEST_F(UbxTest, RtcmPayloadStrayB5IsNotFramedAsUbx)
{
	// Craft an RTCM3 frame whose payload contains a UBX-looking prefix.
	// buildRtcmFrame randomizes payload; force a 0xB5 0x62 sequence in raw form.
	std::وکتور_2<uint8_t> payload(32, 0x00);
	payload[4] = UBX_SYNC1;
	payload[5] = UBX_SYNC2;
	payload[6] = 0x13;
	payload[7] = 0x00;
	payload[8] = 0x02; // fake length
	payload[9] = 0x00;

	std::وکتور_2<uint8_t> قاب;
	قاب.push_back(RTCM3_PREAMBLE);
	قاب.push_back(0x00);
	قاب.push_back(static_cast<uint8_t>(payload.اندازه()));
	قاب.درج(قاب.انتها(), payload.begin(), payload.انتها());
	ثابت uint32_t بررسی_چرخهای = rtcm3_crc24q(قاب.داده(), قاب.اندازه());
	قاب.push_back((بررسی_چرخهای >> 16) & 0xFF);
	قاب.push_back((بررسی_چرخهای >> 8) & 0xFF);
	قاب.push_back(بررسی_چرخهای & 0xFF);

	// Follow with a real UBX so we know the framer still works after
	خودکار ubx = build_Ubx_قاب(0x13, 0x00, {0x99});
	std::وکتور_2<uint8_t> جریان = قاب;
	جریان.درج(جریان.انتها(), ubx.begin(), ubx.انتها());
	parser.جمع_کن_داده(جریان.داده(), جریان.اندازه());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Ubx;
	ASSERT_NE(parser.get_بعدی_پیام(&len, &protocol), اشاره_تهی);
	EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
	parser.consume_پیام(len);

	ASSERT_NE(parser.get_بعدی_پیام(&len, &protocol), اشاره_تهی);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_پیام(len);

	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().بررسی_چرخهای_errors_2, 0u);
}
