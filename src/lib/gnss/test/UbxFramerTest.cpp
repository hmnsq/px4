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

#شامل <gtest/gtest.h>
#شامل "correction_framer.h"

#شامل <vector>
#شامل <cstring>
#شامل <random>

استعمال نامخلا gnss;

جماعت UbxTest : عوامی ::testing::Test
{
محفوظ:
	CorrectionFramer parser;

	// Build a valid UBX frame (class/id arbitrary; content not decoded).
	std::سمتیہ_2<uint8_t> build_Ubx_چوکھٹا(uint8_t پیغام_class, uint8_t پیغام_id,
					   ثابت std::سمتیہ_2<uint8_t> &payload)
	{
		ثابت size_t payload_len = payload.سائز();
		std::سمتیہ_2<uint8_t> چوکھٹا;
		چوکھٹا.push_back(UBX_SYNC1);
		چوکھٹا.push_back(UBX_SYNC2);
		چوکھٹا.push_back(پیغام_class);
		چوکھٹا.push_back(پیغام_id);
		چوکھٹا.push_back(static_cast<uint8_t>(payload_len & 0xFF));
		چوکھٹا.push_back(static_cast<uint8_t>((payload_len >> 8) & 0xFF));
		چوکھٹا.داخل_کرو(چوکھٹا.انتہا(), payload.begin(), payload.انتہا());

		uint8_t ck_a = 0;
		uint8_t ck_b = 0;
		ubx_checksum(&چوکھٹا[2], UBX_سرخی_LEN - 2 + payload_len, &ck_a, &ck_b);
		چوکھٹا.push_back(ck_a);
		چوکھٹا.push_back(ck_b);
		واپس چوکھٹا;
	}

	std::سمتیہ_2<uint8_t> build_Rtcm_چوکھٹا(std::mt19937 &rng, size_t payload_len)
	{
		std::سمتیہ_2<uint8_t> چوکھٹا;
		چوکھٹا.push_back(RTCM3_PREAMBLE);
		چوکھٹا.push_back(static_cast<uint8_t>((payload_len >> 8) & 0x03));
		چوکھٹا.push_back(static_cast<uint8_t>(payload_len & 0xFF));

		std::یکساں_int_distribution<صحیح> byte(0, 255);

		برائے (size_t i = 0; i < payload_len; i++) {
			چوکھٹا.push_back(static_cast<uint8_t>(byte(rng)));
		}

		ثابت uint32_t چکری_جانچ = rtcm3_crc24q(چوکھٹا.ڈیٹا(), چوکھٹا.سائز());
		چوکھٹا.push_back((چکری_جانچ >> 16) & 0xFF);
		چوکھٹا.push_back((چکری_جانچ >> 8) & 0xFF);
		چوکھٹا.push_back(چکری_جانچ & 0xFF);
		واپس چوکھٹا;
	}

#اگر defined(CONFIG_GPS_SPARTN)
	std::سمتیہ_2<uint8_t> build_Spartn_چوکھٹا(uint8_t پیغام_type_2, ثابت std::سمتیہ_2<uint8_t> &payload,
					      uint8_t چکری_جانچ_type = 0)
	{
		ثابت size_t payload_len = payload.سائز();
		std::سمتیہ_2<uint8_t> چوکھٹا;
		چوکھٹا.push_back(SPARTN_PREAMBLE);

		uint8_t b1 = static_cast<uint8_t>(((پیغام_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((چکری_جانچ_type & 0x03) << 4));
		چوکھٹا.push_back(b1);
		چوکھٹا.push_back(b2);
		چوکھٹا.push_back(b3);

		// 4-byte description, 16-bit time tag
		چوکھٹا.push_back(0x00);
		چوکھٹا.push_back(0x00);
		چوکھٹا.push_back(0x00);
		چوکھٹا.push_back(0x00);

		چوکھٹا.داخل_کرو(چوکھٹا.انتہا(), payload.begin(), payload.انتہا());

		ثابت uint32_t چکری_جانچ = spartn_پیغام_چکری_جانچ(&چوکھٹا[1], چوکھٹا.سائز() - 1, چکری_جانچ_type);
		ثابت size_t چکری_جانچ_بائٹیں = static_cast<size_t>(چکری_جانچ_type) + 1;

		برائے (صحیح i = static_cast<صحیح>(چکری_جانچ_بائٹیں) - 1; i >= 0; i--) {
			چوکھٹا.push_back(static_cast<uint8_t>((چکری_جانچ >>(8 * i)) & 0xFF));
		}

		واپس چوکھٹا;
	}
#اگر_ختم
};

// =============================================================================
// Checksum
// =============================================================================

// Empty ACK-ACK (class=0x05, id=0x01, length=0): covered bytes 05 01 00 00.
// 8-bit Fletcher: a runs 5,6,6,6 and b runs 5,11,17,23 → CK_A=0x06, CK_B=0x17.
TEST_F(UbxTest, Checksum_معلوم_سمتیہ)
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

TEST_F(UbxTest, Single_اقل_چوکھٹا)
{
	// UBX-MGA class is 0x13; any class/id is fine for framing
	خودکار چوکھٹا = build_Ubx_چوکھٹا(0x13, 0x00, {0xAA, 0xBB});
	parser.جوڑو_ڈیٹا(چوکھٹا.ڈیٹا(), چوکھٹا.سائز());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *پیغام_2 = parser.get_اگلا_پیغام(&len, &protocol);
	ASSERT_NE(پیغام_2, خالی_اشاریہ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, چوکھٹا.سائز());
	EXPECT_EQ(0, memcmp(پیغام_2, چوکھٹا.ڈیٹا(), چوکھٹا.سائز()));
	parser.consume_پیغام(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().چکری_جانچ_errors_2, 0u);
}

TEST_F(UbxTest, خالی_خانے_Payload_چوکھٹا)
{
	خودکار چوکھٹا = build_Ubx_چوکھٹا(0x05, 0x01, {});
	parser.جوڑو_ڈیٹا(چوکھٹا.ڈیٹا(), چوکھٹا.سائز());

	size_t len = 0;
	ASSERT_NE(parser.get_اگلا_پیغام(&len), خالی_اشاریہ);
	EXPECT_EQ(len, UBX_سرخی_LEN + UBX_CK_LEN);
	parser.consume_پیغام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, مضرب_چوکھٹے)
{
	خودکار f1 = build_Ubx_چوکھٹا(0x13, 0x00, {0x01});
	خودکار f2 = build_Ubx_چوکھٹا(0x13, 0x02, {0x02, 0x03, 0x04});
	std::سمتیہ_2<uint8_t> دھارا;
	دھارا.داخل_کرو(دھارا.انتہا(), f1.begin(), f1.انتہا());
	دھارا.داخل_کرو(دھارا.انتہا(), f2.begin(), f2.انتہا());
	parser.جوڑو_ڈیٹا(دھارا.ڈیٹا(), دھارا.سائز());

	صحیح گنتی = 0;
	size_t len = 0;

	جب (parser.get_اگلا_پیغام(&len) != خالی_اشاریہ) {
		parser.consume_پیغام(len);
		گنتی++;
	}

	EXPECT_EQ(گنتی, 2);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
}

TEST_F(UbxTest, Chunked_آمد)
{
	خودکار چوکھٹا = build_Ubx_چوکھٹا(0x13, 0x01, {0x10, 0x20, 0x30, 0x40});

	برائے (size_t i = 0; i < چوکھٹا.سائز(); i++) {
		parser.جوڑو_ڈیٹا(&چوکھٹا[i], 1);
		// No complete frame until the last byte
	}

	size_t len = 0;
	ASSERT_NE(parser.get_اگلا_پیغام(&len), خالی_اشاریہ);
	EXPECT_EQ(len, چوکھٹا.سائز());
}

TEST_F(UbxTest, Noise_Before_چوکھٹا)
{
	خودکار چوکھٹا = build_Ubx_چوکھٹا(0x13, 0x00, {0x55});
	std::سمتیہ_2<uint8_t> دھارا = {0x00, 0xFF, 0x01};
	دھارا.داخل_کرو(دھارا.انتہا(), چوکھٹا.begin(), چوکھٹا.انتہا());
	parser.جوڑو_ڈیٹا(دھارا.ڈیٹا(), دھارا.سائز());

	size_t len = 0;
	ASSERT_NE(parser.get_اگلا_پیغام(&len), خالی_اشاریہ);
	EXPECT_GT(parser.getStats().بائٹیں_discarded_2, 0u);
	parser.consume_پیغام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, BadSync2)
{
	// 0xB5 followed by something other than 0x62 is not a frame start
	std::سمتیہ_2<uint8_t> دھارا = {UBX_SYNC1, 0x00, 0x11, 0x22};
	خودکار چوکھٹا = build_Ubx_چوکھٹا(0x13, 0x00, {0x01});
	دھارا.داخل_کرو(دھارا.انتہا(), چوکھٹا.begin(), چوکھٹا.انتہا());
	parser.جوڑو_ڈیٹا(دھارا.ڈیٹا(), دھارا.سائز());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_اگلا_پیغام(&len, &protocol), خالی_اشاریہ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_پیغام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_GT(parser.getStats().بائٹیں_discarded_2, 0u);
}

TEST_F(UbxTest, BadChecksum)
{
	خودکار چوکھٹا = build_Ubx_چوکھٹا(0x13, 0x00, {0x11, 0x22});
	چوکھٹا.عقب() ^= 0xFF;
	parser.جوڑو_ڈیٹا(چوکھٹا.ڈیٹا(), چوکھٹا.سائز());

	size_t len = 0;
	EXPECT_EQ(parser.get_اگلا_پیغام(&len), خالی_اشاریہ);
	EXPECT_GE(parser.getStats().چکری_جانچ_errors_2, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 0u);
}

TEST_F(UbxTest, RejectsOversizedPayload)
{
	// Length field claims UBX_MAX_PAYLOAD_LEN + 1 — must not pin the buffer
	std::سمتیہ_2<uint8_t> سرخی = {
		UBX_SYNC1, UBX_SYNC2,
		0x13, 0x00,
		static_cast<uint8_t>((UBX_MAX_PAYLOAD_LEN + 1) & 0xFF),
		static_cast<uint8_t>(((UBX_MAX_PAYLOAD_LEN + 1) >> 8) & 0xFF),
	};
	خودکار good = build_Ubx_چوکھٹا(0x13, 0x00, {0xAB});
	سرخی.داخل_کرو(سرخی.انتہا(), good.begin(), good.انتہا());
	parser.جوڑو_ڈیٹا(سرخی.ڈیٹا(), سرخی.سائز());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_اگلا_پیغام(&len, &protocol), خالی_اشاریہ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, good.سائز());
	parser.consume_پیغام(len);
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
	خودکار mga1 = build_Ubx_چوکھٹا(0x13, 0x00, {0x01, 0x02, 0x03});
	خودکار mga2 = build_Ubx_چوکھٹا(0x13, 0x02, {0x10, 0x20});
	خودکار rtcm = build_Rtcm_چوکھٹا(rng, 40);

	std::سمتیہ_2<uint8_t> دھارا;
	دھارا.داخل_کرو(دھارا.انتہا(), mga1.begin(), mga1.انتہا());
	دھارا.داخل_کرو(دھارا.انتہا(), mga2.begin(), mga2.انتہا());
	دھارا.داخل_کرو(دھارا.انتہا(), rtcm.begin(), rtcm.انتہا());
	parser.جوڑو_ڈیٹا(دھارا.ڈیٹا(), دھارا.سائز());

	std::سمتیہ_2<CorrectionProtocol> نظر_آیا;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	جب (parser.get_اگلا_پیغام(&len, &protocol) != خالی_اشاریہ) {
		نظر_آیا.push_back(protocol);
		parser.consume_پیغام(len);
	}

	ASSERT_EQ(نظر_آیا.سائز(), 3u);
	EXPECT_EQ(نظر_آیا[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(نظر_آیا[1], CorrectionProtocol::Ubx);
	EXPECT_EQ(نظر_آیا[2], CorrectionProtocol::Rtcm3);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().چکری_جانچ_errors_2, 0u);
}

#اگر defined(CONFIG_GPS_SPARTN)
TEST_F(UbxTest, MgaThenSpartn)
{
	خودکار mga = build_Ubx_چوکھٹا(0x13, 0x00, {0xDE, 0xAD});
	خودکار spartn = build_Spartn_چوکھٹا(0, {0xAA, 0xBB});

	std::سمتیہ_2<uint8_t> دھارا;
	دھارا.داخل_کرو(دھارا.انتہا(), mga.begin(), mga.انتہا());
	دھارا.داخل_کرو(دھارا.انتہا(), spartn.begin(), spartn.انتہا());
	// Another MGA mid-stream should also work (reconnect-style)
	خودکار mga2 = build_Ubx_چوکھٹا(0x13, 0x01, {0x01});
	دھارا.داخل_کرو(دھارا.انتہا(), mga2.begin(), mga2.انتہا());
	parser.جوڑو_ڈیٹا(دھارا.ڈیٹا(), دھارا.سائز());

	std::سمتیہ_2<CorrectionProtocol> نظر_آیا;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	جب (parser.get_اگلا_پیغام(&len, &protocol) != خالی_اشاریہ) {
		نظر_آیا.push_back(protocol);
		parser.consume_پیغام(len);
	}

	ASSERT_EQ(نظر_آیا.سائز(), 3u);
	EXPECT_EQ(نظر_آیا[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(نظر_آیا[1], CorrectionProtocol::Spartn);
	EXPECT_EQ(نظر_آیا[2], CorrectionProtocol::Ubx);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
}
#اگر_ختم

// A complete RTCM3 frame consumes its payload, so a 0xB5 that happens to sit
// inside that payload is never considered as a UBX start.
TEST_F(UbxTest, RtcmPayloadStrayB5IsNotFramedAsUbx)
{
	// Craft an RTCM3 frame whose payload contains a UBX-looking prefix.
	// buildRtcmFrame randomizes payload; force a 0xB5 0x62 sequence in raw form.
	std::سمتیہ_2<uint8_t> payload(32, 0x00);
	payload[4] = UBX_SYNC1;
	payload[5] = UBX_SYNC2;
	payload[6] = 0x13;
	payload[7] = 0x00;
	payload[8] = 0x02; // fake length
	payload[9] = 0x00;

	std::سمتیہ_2<uint8_t> چوکھٹا;
	چوکھٹا.push_back(RTCM3_PREAMBLE);
	چوکھٹا.push_back(0x00);
	چوکھٹا.push_back(static_cast<uint8_t>(payload.سائز()));
	چوکھٹا.داخل_کرو(چوکھٹا.انتہا(), payload.begin(), payload.انتہا());
	ثابت uint32_t چکری_جانچ = rtcm3_crc24q(چوکھٹا.ڈیٹا(), چوکھٹا.سائز());
	چوکھٹا.push_back((چکری_جانچ >> 16) & 0xFF);
	چوکھٹا.push_back((چکری_جانچ >> 8) & 0xFF);
	چوکھٹا.push_back(چکری_جانچ & 0xFF);

	// Follow with a real UBX so we know the framer still works after
	خودکار ubx = build_Ubx_چوکھٹا(0x13, 0x00, {0x99});
	std::سمتیہ_2<uint8_t> دھارا = چوکھٹا;
	دھارا.داخل_کرو(دھارا.انتہا(), ubx.begin(), ubx.انتہا());
	parser.جوڑو_ڈیٹا(دھارا.ڈیٹا(), دھارا.سائز());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Ubx;
	ASSERT_NE(parser.get_اگلا_پیغام(&len, &protocol), خالی_اشاریہ);
	EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
	parser.consume_پیغام(len);

	ASSERT_NE(parser.get_اگلا_پیغام(&len, &protocol), خالی_اشاریہ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_پیغام(len);

	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().چکری_جانچ_errors_2, 0u);
}
