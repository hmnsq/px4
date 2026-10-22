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

کارول نطاق gnss;

صنف UbxTest : عام ::testing::Test
{
محمي:
	CorrectionFramer parser;

	// Build a valid UBX frame (class/id arbitrary; content not decoded).
	std::ویکټور_2<uint8_t> build_Ubx_چوکاټ(uint8_t پیغام_class, uint8_t پیغام_id,
					   ثابت std::ویکټور_2<uint8_t> &payload)
	{
		ثابت size_t payload_len = payload.اندازه();
		std::ویکټور_2<uint8_t> چوکاټ;
		چوکاټ.push_back(UBX_SYNC1);
		چوکاټ.push_back(UBX_SYNC2);
		چوکاټ.push_back(پیغام_class);
		چوکاټ.push_back(پیغام_id);
		چوکاټ.push_back(static_cast<uint8_t>(payload_len & 0xFF));
		چوکاټ.push_back(static_cast<uint8_t>((payload_len >> 8) & 0xFF));
		چوکاټ.ورننباسه(چوکاټ.پای_ټکی(), payload.begin(), payload.پای_ټکی());

		uint8_t ck_a = 0;
		uint8_t ck_b = 0;
		ubx_checksum(&چوکاټ[2], UBX_سرلیک_LEN - 2 + payload_len, &ck_a, &ck_b);
		چوکاټ.push_back(ck_a);
		چوکاټ.push_back(ck_b);
		أرجع چوکاټ;
	}

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

#إذا defined(CONFIG_GPS_SPARTN)
	std::ویکټور_2<uint8_t> build_Spartn_چوکاټ(uint8_t پیغام_type_2, ثابت std::ویکټور_2<uint8_t> &payload,
					      uint8_t دوراني_ازموینه_type = 0)
	{
		ثابت size_t payload_len = payload.اندازه();
		std::ویکټور_2<uint8_t> چوکاټ;
		چوکاټ.push_back(SPARTN_PREAMBLE);

		uint8_t b1 = static_cast<uint8_t>(((پیغام_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((دوراني_ازموینه_type & 0x03) << 4));
		چوکاټ.push_back(b1);
		چوکاټ.push_back(b2);
		چوکاټ.push_back(b3);

		// 4-byte description, 16-bit time tag
		چوکاټ.push_back(0x00);
		چوکاټ.push_back(0x00);
		چوکاټ.push_back(0x00);
		چوکاټ.push_back(0x00);

		چوکاټ.ورننباسه(چوکاټ.پای_ټکی(), payload.begin(), payload.پای_ټکی());

		ثابت uint32_t دوراني_ازموینه = spartn_پیغام_دوراني_ازموینه(&چوکاټ[1], چوکاټ.اندازه() - 1, دوراني_ازموینه_type);
		ثابت size_t دوراني_ازموینه_بایټونه = static_cast<size_t>(دوراني_ازموینه_type) + 1;

		لكل (صحيح i = static_cast<صحيح>(دوراني_ازموینه_بایټونه) - 1; i >= 0; i--) {
			چوکاټ.push_back(static_cast<uint8_t>((دوراني_ازموینه >>(8 * i)) & 0xFF));
		}

		أرجع چوکاټ;
	}
#د_که_پای
};

// =============================================================================
// Checksum
// =============================================================================

// Empty ACK-ACK (class=0x05, id=0x01, length=0): covered bytes 05 01 00 00.
// 8-bit Fletcher: a runs 5,6,6,6 and b runs 5,11,17,23 → CK_A=0x06, CK_B=0x17.
TEST_F(UbxTest, Checksum_معلوم_ویکټور)
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

TEST_F(UbxTest, Single_اصغري_چوکاټ)
{
	// UBX-MGA class is 0x13; any class/id is fine for framing
	خپلکاری چوکاټ = build_Ubx_چوکاټ(0x13, 0x00, {0xAA, 0xBB});
	parser.جمع_کړه_معلومات(چوکاټ.معلومات(), چوکاټ.اندازه());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *پیغام_2 = parser.get_بل_پیغام(&len, &protocol);
	ASSERT_NE(پیغام_2, تش_اشاره);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, چوکاټ.اندازه());
	EXPECT_EQ(0, memcmp(پیغام_2, چوکاټ.معلومات(), چوکاټ.اندازه()));
	parser.consume_پیغام(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().دوراني_ازموینه_errors_2, 0u);
}

TEST_F(UbxTest, تشه_Payload_چوکاټ)
{
	خپلکاری چوکاټ = build_Ubx_چوکاټ(0x05, 0x01, {});
	parser.جمع_کړه_معلومات(چوکاټ.معلومات(), چوکاټ.اندازه());

	size_t len = 0;
	ASSERT_NE(parser.get_بل_پیغام(&len), تش_اشاره);
	EXPECT_EQ(len, UBX_سرلیک_LEN + UBX_CK_LEN);
	parser.consume_پیغام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, مضرب_چوکاټونه)
{
	خپلکاری f1 = build_Ubx_چوکاټ(0x13, 0x00, {0x01});
	خپلکاری f2 = build_Ubx_چوکاټ(0x13, 0x02, {0x02, 0x03, 0x04});
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
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
}

TEST_F(UbxTest, Chunked_رارسېدنه)
{
	خپلکاری چوکاټ = build_Ubx_چوکاټ(0x13, 0x01, {0x10, 0x20, 0x30, 0x40});

	لكل (size_t i = 0; i < چوکاټ.اندازه(); i++) {
		parser.جمع_کړه_معلومات(&چوکاټ[i], 1);
		// No complete frame until the last byte
	}

	size_t len = 0;
	ASSERT_NE(parser.get_بل_پیغام(&len), تش_اشاره);
	EXPECT_EQ(len, چوکاټ.اندازه());
}

TEST_F(UbxTest, Noise_Before_چوکاټ)
{
	خپلکاری چوکاټ = build_Ubx_چوکاټ(0x13, 0x00, {0x55});
	std::ویکټور_2<uint8_t> بهیر = {0x00, 0xFF, 0x01};
	بهیر.ورننباسه(بهیر.پای_ټکی(), چوکاټ.begin(), چوکاټ.پای_ټکی());
	parser.جمع_کړه_معلومات(بهیر.معلومات(), بهیر.اندازه());

	size_t len = 0;
	ASSERT_NE(parser.get_بل_پیغام(&len), تش_اشاره);
	EXPECT_GT(parser.getStats().بایټونه_discarded_2, 0u);
	parser.consume_پیغام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, BadSync2)
{
	// 0xB5 followed by something other than 0x62 is not a frame start
	std::ویکټور_2<uint8_t> بهیر = {UBX_SYNC1, 0x00, 0x11, 0x22};
	خپلکاری چوکاټ = build_Ubx_چوکاټ(0x13, 0x00, {0x01});
	بهیر.ورننباسه(بهیر.پای_ټکی(), چوکاټ.begin(), چوکاټ.پای_ټکی());
	parser.جمع_کړه_معلومات(بهیر.معلومات(), بهیر.اندازه());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_بل_پیغام(&len, &protocol), تش_اشاره);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_پیغام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_GT(parser.getStats().بایټونه_discarded_2, 0u);
}

TEST_F(UbxTest, BadChecksum)
{
	خپلکاری چوکاټ = build_Ubx_چوکاټ(0x13, 0x00, {0x11, 0x22});
	چوکاټ.شا() ^= 0xFF;
	parser.جمع_کړه_معلومات(چوکاټ.معلومات(), چوکاټ.اندازه());

	size_t len = 0;
	EXPECT_EQ(parser.get_بل_پیغام(&len), تش_اشاره);
	EXPECT_GE(parser.getStats().دوراني_ازموینه_errors_2, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 0u);
}

TEST_F(UbxTest, RejectsOversizedPayload)
{
	// Length field claims UBX_MAX_PAYLOAD_LEN + 1 — must not pin the buffer
	std::ویکټور_2<uint8_t> سرلیک = {
		UBX_SYNC1, UBX_SYNC2,
		0x13, 0x00,
		static_cast<uint8_t>((UBX_MAX_PAYLOAD_LEN + 1) & 0xFF),
		static_cast<uint8_t>(((UBX_MAX_PAYLOAD_LEN + 1) >> 8) & 0xFF),
	};
	خپلکاری good = build_Ubx_چوکاټ(0x13, 0x00, {0xAB});
	سرلیک.ورننباسه(سرلیک.پای_ټکی(), good.begin(), good.پای_ټکی());
	parser.جمع_کړه_معلومات(سرلیک.معلومات(), سرلیک.اندازه());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_بل_پیغام(&len, &protocol), تش_اشاره);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, good.اندازه());
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
	خپلکاری mga1 = build_Ubx_چوکاټ(0x13, 0x00, {0x01, 0x02, 0x03});
	خپلکاری mga2 = build_Ubx_چوکاټ(0x13, 0x02, {0x10, 0x20});
	خپلکاری rtcm = build_Rtcm_چوکاټ(rng, 40);

	std::ویکټور_2<uint8_t> بهیر;
	بهیر.ورننباسه(بهیر.پای_ټکی(), mga1.begin(), mga1.پای_ټکی());
	بهیر.ورننباسه(بهیر.پای_ټکی(), mga2.begin(), mga2.پای_ټکی());
	بهیر.ورننباسه(بهیر.پای_ټکی(), rtcm.begin(), rtcm.پای_ټکی());
	parser.جمع_کړه_معلومات(بهیر.معلومات(), بهیر.اندازه());

	std::ویکټور_2<CorrectionProtocol> کتل_شوی;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	طالما (parser.get_بل_پیغام(&len, &protocol) != تش_اشاره) {
		کتل_شوی.push_back(protocol);
		parser.consume_پیغام(len);
	}

	ASSERT_EQ(کتل_شوی.اندازه(), 3u);
	EXPECT_EQ(کتل_شوی[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(کتل_شوی[1], CorrectionProtocol::Ubx);
	EXPECT_EQ(کتل_شوی[2], CorrectionProtocol::Rtcm3);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().دوراني_ازموینه_errors_2, 0u);
}

#إذا defined(CONFIG_GPS_SPARTN)
TEST_F(UbxTest, MgaThenSpartn)
{
	خپلکاری mga = build_Ubx_چوکاټ(0x13, 0x00, {0xDE, 0xAD});
	خپلکاری spartn = build_Spartn_چوکاټ(0, {0xAA, 0xBB});

	std::ویکټور_2<uint8_t> بهیر;
	بهیر.ورننباسه(بهیر.پای_ټکی(), mga.begin(), mga.پای_ټکی());
	بهیر.ورننباسه(بهیر.پای_ټکی(), spartn.begin(), spartn.پای_ټکی());
	// Another MGA mid-stream should also work (reconnect-style)
	خپلکاری mga2 = build_Ubx_چوکاټ(0x13, 0x01, {0x01});
	بهیر.ورننباسه(بهیر.پای_ټکی(), mga2.begin(), mga2.پای_ټکی());
	parser.جمع_کړه_معلومات(بهیر.معلومات(), بهیر.اندازه());

	std::ویکټور_2<CorrectionProtocol> کتل_شوی;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	طالما (parser.get_بل_پیغام(&len, &protocol) != تش_اشاره) {
		کتل_شوی.push_back(protocol);
		parser.consume_پیغام(len);
	}

	ASSERT_EQ(کتل_شوی.اندازه(), 3u);
	EXPECT_EQ(کتل_شوی[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(کتل_شوی[1], CorrectionProtocol::Spartn);
	EXPECT_EQ(کتل_شوی[2], CorrectionProtocol::Ubx);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
}
#د_که_پای

// A complete RTCM3 frame consumes its payload, so a 0xB5 that happens to sit
// inside that payload is never considered as a UBX start.
TEST_F(UbxTest, RtcmPayloadStrayB5IsNotFramedAsUbx)
{
	// Craft an RTCM3 frame whose payload contains a UBX-looking prefix.
	// buildRtcmFrame randomizes payload; force a 0xB5 0x62 sequence in raw form.
	std::ویکټور_2<uint8_t> payload(32, 0x00);
	payload[4] = UBX_SYNC1;
	payload[5] = UBX_SYNC2;
	payload[6] = 0x13;
	payload[7] = 0x00;
	payload[8] = 0x02; // fake length
	payload[9] = 0x00;

	std::ویکټور_2<uint8_t> چوکاټ;
	چوکاټ.push_back(RTCM3_PREAMBLE);
	چوکاټ.push_back(0x00);
	چوکاټ.push_back(static_cast<uint8_t>(payload.اندازه()));
	چوکاټ.ورننباسه(چوکاټ.پای_ټکی(), payload.begin(), payload.پای_ټکی());
	ثابت uint32_t دوراني_ازموینه = rtcm3_crc24q(چوکاټ.معلومات(), چوکاټ.اندازه());
	چوکاټ.push_back((دوراني_ازموینه >> 16) & 0xFF);
	چوکاټ.push_back((دوراني_ازموینه >> 8) & 0xFF);
	چوکاټ.push_back(دوراني_ازموینه & 0xFF);

	// Follow with a real UBX so we know the framer still works after
	خپلکاری ubx = build_Ubx_چوکاټ(0x13, 0x00, {0x99});
	std::ویکټور_2<uint8_t> بهیر = چوکاټ;
	بهیر.ورننباسه(بهیر.پای_ټکی(), ubx.begin(), ubx.پای_ټکی());
	parser.جمع_کړه_معلومات(بهیر.معلومات(), بهیر.اندازه());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Ubx;
	ASSERT_NE(parser.get_بل_پیغام(&len, &protocol), تش_اشاره);
	EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
	parser.consume_پیغام(len);

	ASSERT_NE(parser.get_بل_پیغام(&len, &protocol), تش_اشاره);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_پیغام(len);

	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().دوراني_ازموینه_errors_2, 0u);
}
