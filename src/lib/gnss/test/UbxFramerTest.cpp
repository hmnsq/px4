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

استعمال نطاق gnss;

صنف UbxTest : عام ::testing::Test
{
محمي:
	CorrectionFramer parser;

	// Build a valid UBX frame (class/id arbitrary; content not decoded).
	std::ويڪٽر_2<uint8_t> build_Ubx_چوکٺو(uint8_t نياپو_class, uint8_t نياپو_id,
					   ثابت std::ويڪٽر_2<uint8_t> &payload)
	{
		ثابت size_t payload_len = payload.سائيز();
		std::ويڪٽر_2<uint8_t> چوکٺو;
		چوکٺو.push_back(UBX_SYNC1);
		چوکٺو.push_back(UBX_SYNC2);
		چوکٺو.push_back(نياپو_class);
		چوکٺو.push_back(نياپو_id);
		چوکٺو.push_back(static_cast<uint8_t>(payload_len & 0xFF));
		چوکٺو.push_back(static_cast<uint8_t>((payload_len >> 8) & 0xFF));
		چوکٺو.وجهو(چوکٺو.پڇاڙي(), payload.begin(), payload.پڇاڙي());

		uint8_t ck_a = 0;
		uint8_t ck_b = 0;
		ubx_checksum(&چوکٺو[2], UBX_سرو_LEN - 2 + payload_len, &ck_a, &ck_b);
		چوکٺو.push_back(ck_a);
		چوکٺو.push_back(ck_b);
		أرجع چوکٺو;
	}

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

#إذا defined(CONFIG_GPS_SPARTN)
	std::ويڪٽر_2<uint8_t> build_Spartn_چوکٺو(uint8_t نياپو_type, ثابت std::ويڪٽر_2<uint8_t> &payload,
					      uint8_t چڪري_جاچ_type = 0)
	{
		ثابت size_t payload_len = payload.سائيز();
		std::ويڪٽر_2<uint8_t> چوکٺو;
		چوکٺو.push_back(SPARTN_PREAMBLE);

		uint8_t b1 = static_cast<uint8_t>(((نياپو_type & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((چڪري_جاچ_type & 0x03) << 4));
		چوکٺو.push_back(b1);
		چوکٺو.push_back(b2);
		چوکٺو.push_back(b3);

		// 4-byte description, 16-bit time tag
		چوکٺو.push_back(0x00);
		چوکٺو.push_back(0x00);
		چوکٺو.push_back(0x00);
		چوکٺو.push_back(0x00);

		چوکٺو.وجهو(چوکٺو.پڇاڙي(), payload.begin(), payload.پڇاڙي());

		ثابت uint32_t چڪري_جاچ = spartn_پيغام_چڪري_جاچ(&چوکٺو[1], چوکٺو.سائيز() - 1, چڪري_جاچ_type);
		ثابت size_t چڪري_جاچ_بائيٽون = static_cast<size_t>(چڪري_جاچ_type) + 1;

		لكل (صحيح i = static_cast<صحيح>(چڪري_جاچ_بائيٽون) - 1; i >= 0; i--) {
			چوکٺو.push_back(static_cast<uint8_t>((چڪري_جاچ >>(8 * i)) & 0xFF));
		}

		أرجع چوکٺو;
	}
#جيڪڏهن_پورو
};

// =============================================================================
// Checksum
// =============================================================================

// Empty ACK-ACK (class=0x05, id=0x01, length=0): covered bytes 05 01 00 00.
// 8-bit Fletcher: a runs 5,6,6,6 and b runs 5,11,17,23 → CK_A=0x06, CK_B=0x17.
TEST_F(UbxTest, Checksum_معلوم_ويڪٽر)
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

TEST_F(UbxTest, Single_گهٽتر_چوکٺو)
{
	// UBX-MGA class is 0x13; any class/id is fine for framing
	خودڪار چوکٺو = build_Ubx_چوکٺو(0x13, 0x00, {0xAA, 0xBB});
	parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ثابت uint8_t *نياپو = parser.get_ايندڙ_پيغام(&len, &protocol);
	ASSERT_NE(نياپو, خالي_اشارو);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, چوکٺو.سائيز());
	EXPECT_EQ(0, memcmp(نياپو, چوکٺو.معلومات(), چوکٺو.سائيز()));
	parser.consume_پيغام(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().چڪري_جاچ_errors_2, 0u);
}

TEST_F(UbxTest, خالي_جايون_Payload_چوکٺو)
{
	خودڪار چوکٺو = build_Ubx_چوکٺو(0x05, 0x01, {});
	parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

	size_t len = 0;
	ASSERT_NE(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
	EXPECT_EQ(len, UBX_سرو_LEN + UBX_CK_LEN);
	parser.consume_پيغام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, گڻو_چوکٺا)
{
	خودڪار f1 = build_Ubx_چوکٺو(0x13, 0x00, {0x01});
	خودڪار f2 = build_Ubx_چوکٺو(0x13, 0x02, {0x02, 0x03, 0x04});
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
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
}

TEST_F(UbxTest, Chunked_آمد)
{
	خودڪار چوکٺو = build_Ubx_چوکٺو(0x13, 0x01, {0x10, 0x20, 0x30, 0x40});

	لكل (size_t i = 0; i < چوکٺو.سائيز(); i++) {
		parser.جوڙيو_معلومات(&چوکٺو[i], 1);
		// No complete frame until the last byte
	}

	size_t len = 0;
	ASSERT_NE(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
	EXPECT_EQ(len, چوکٺو.سائيز());
}

TEST_F(UbxTest, Noise_Before_چوکٺو)
{
	خودڪار چوکٺو = build_Ubx_چوکٺو(0x13, 0x00, {0x55});
	std::ويڪٽر_2<uint8_t> وهڪرو = {0x00, 0xFF, 0x01};
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), چوکٺو.begin(), چوکٺو.پڇاڙي());
	parser.جوڙيو_معلومات(وهڪرو.معلومات(), وهڪرو.سائيز());

	size_t len = 0;
	ASSERT_NE(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
	EXPECT_GT(parser.getStats().بائيٽون_discarded_2, 0u);
	parser.consume_پيغام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, BadSync2)
{
	// 0xB5 followed by something other than 0x62 is not a frame start
	std::ويڪٽر_2<uint8_t> وهڪرو = {UBX_SYNC1, 0x00, 0x11, 0x22};
	خودڪار چوکٺو = build_Ubx_چوکٺو(0x13, 0x00, {0x01});
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), چوکٺو.begin(), چوکٺو.پڇاڙي());
	parser.جوڙيو_معلومات(وهڪرو.معلومات(), وهڪرو.سائيز());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_ايندڙ_پيغام(&len, &protocol), خالي_اشارو);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_پيغام(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_GT(parser.getStats().بائيٽون_discarded_2, 0u);
}

TEST_F(UbxTest, BadChecksum)
{
	خودڪار چوکٺو = build_Ubx_چوکٺو(0x13, 0x00, {0x11, 0x22});
	چوکٺو.پٺ() ^= 0xFF;
	parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

	size_t len = 0;
	EXPECT_EQ(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
	EXPECT_GE(parser.getStats().چڪري_جاچ_errors_2, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 0u);
}

TEST_F(UbxTest, RejectsOversizedPayload)
{
	// Length field claims UBX_MAX_PAYLOAD_LEN + 1 — must not pin the buffer
	std::ويڪٽر_2<uint8_t> سرو = {
		UBX_SYNC1, UBX_SYNC2,
		0x13, 0x00,
		static_cast<uint8_t>((UBX_MAX_PAYLOAD_LEN + 1) & 0xFF),
		static_cast<uint8_t>(((UBX_MAX_PAYLOAD_LEN + 1) >> 8) & 0xFF),
	};
	خودڪار good = build_Ubx_چوکٺو(0x13, 0x00, {0xAB});
	سرو.وجهو(سرو.پڇاڙي(), good.begin(), good.پڇاڙي());
	parser.جوڙيو_معلومات(سرو.معلومات(), سرو.سائيز());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_ايندڙ_پيغام(&len, &protocol), خالي_اشارو);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, good.سائيز());
	parser.consume_پيغام(len);
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
	خودڪار mga1 = build_Ubx_چوکٺو(0x13, 0x00, {0x01, 0x02, 0x03});
	خودڪار mga2 = build_Ubx_چوکٺو(0x13, 0x02, {0x10, 0x20});
	خودڪار rtcm = build_Rtcm_چوکٺو(rng, 40);

	std::ويڪٽر_2<uint8_t> وهڪرو;
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), mga1.begin(), mga1.پڇاڙي());
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), mga2.begin(), mga2.پڇاڙي());
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), rtcm.begin(), rtcm.پڇاڙي());
	parser.جوڙيو_معلومات(وهڪرو.معلومات(), وهڪرو.سائيز());

	std::ويڪٽر_2<CorrectionProtocol> ڏٺل;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	طالما (parser.get_ايندڙ_پيغام(&len, &protocol) != خالي_اشارو) {
		ڏٺل.push_back(protocol);
		parser.consume_پيغام(len);
	}

	ASSERT_EQ(ڏٺل.سائيز(), 3u);
	EXPECT_EQ(ڏٺل[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(ڏٺل[1], CorrectionProtocol::Ubx);
	EXPECT_EQ(ڏٺل[2], CorrectionProtocol::Rtcm3);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().چڪري_جاچ_errors_2, 0u);
}

#إذا defined(CONFIG_GPS_SPARTN)
TEST_F(UbxTest, MgaThenSpartn)
{
	خودڪار mga = build_Ubx_چوکٺو(0x13, 0x00, {0xDE, 0xAD});
	خودڪار spartn = build_Spartn_چوکٺو(0, {0xAA, 0xBB});

	std::ويڪٽر_2<uint8_t> وهڪرو;
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), mga.begin(), mga.پڇاڙي());
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), spartn.begin(), spartn.پڇاڙي());
	// Another MGA mid-stream should also work (reconnect-style)
	خودڪار mga2 = build_Ubx_چوکٺو(0x13, 0x01, {0x01});
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), mga2.begin(), mga2.پڇاڙي());
	parser.جوڙيو_معلومات(وهڪرو.معلومات(), وهڪرو.سائيز());

	std::ويڪٽر_2<CorrectionProtocol> ڏٺل;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	طالما (parser.get_ايندڙ_پيغام(&len, &protocol) != خالي_اشارو) {
		ڏٺل.push_back(protocol);
		parser.consume_پيغام(len);
	}

	ASSERT_EQ(ڏٺل.سائيز(), 3u);
	EXPECT_EQ(ڏٺل[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(ڏٺل[1], CorrectionProtocol::Spartn);
	EXPECT_EQ(ڏٺل[2], CorrectionProtocol::Ubx);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
}
#جيڪڏهن_پورو

// A complete RTCM3 frame consumes its payload, so a 0xB5 that happens to sit
// inside that payload is never considered as a UBX start.
TEST_F(UbxTest, RtcmPayloadStrayB5IsNotFramedAsUbx)
{
	// Craft an RTCM3 frame whose payload contains a UBX-looking prefix.
	// buildRtcmFrame randomizes payload; force a 0xB5 0x62 sequence in raw form.
	std::ويڪٽر_2<uint8_t> payload(32, 0x00);
	payload[4] = UBX_SYNC1;
	payload[5] = UBX_SYNC2;
	payload[6] = 0x13;
	payload[7] = 0x00;
	payload[8] = 0x02; // fake length
	payload[9] = 0x00;

	std::ويڪٽر_2<uint8_t> چوکٺو;
	چوکٺو.push_back(RTCM3_PREAMBLE);
	چوکٺو.push_back(0x00);
	چوکٺو.push_back(static_cast<uint8_t>(payload.سائيز()));
	چوکٺو.وجهو(چوکٺو.پڇاڙي(), payload.begin(), payload.پڇاڙي());
	ثابت uint32_t چڪري_جاچ = rtcm3_crc24q(چوکٺو.معلومات(), چوکٺو.سائيز());
	چوکٺو.push_back((چڪري_جاچ >> 16) & 0xFF);
	چوکٺو.push_back((چڪري_جاچ >> 8) & 0xFF);
	چوکٺو.push_back(چڪري_جاچ & 0xFF);

	// Follow with a real UBX so we know the framer still works after
	خودڪار ubx = build_Ubx_چوکٺو(0x13, 0x00, {0x99});
	std::ويڪٽر_2<uint8_t> وهڪرو = چوکٺو;
	وهڪرو.وجهو(وهڪرو.پڇاڙي(), ubx.begin(), ubx.پڇاڙي());
	parser.جوڙيو_معلومات(وهڪرو.معلومات(), وهڪرو.سائيز());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Ubx;
	ASSERT_NE(parser.get_ايندڙ_پيغام(&len, &protocol), خالي_اشارو);
	EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
	parser.consume_پيغام(len);

	ASSERT_NE(parser.get_ايندڙ_پيغام(&len, &protocol), خالي_اشارو);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_پيغام(len);

	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().چڪري_جاچ_errors_2, 0u);
}
