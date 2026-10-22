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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "correction_framer.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <vector>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>
#ਅਨ੍ਤਰ੍ਭਾਵ <random>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ gnss;

ਵਰਗ UbxTest : ਜਨਤਕ ::testing::Test
{
ਸੁਰੱਖਿਅਤ:
	CorrectionFramer parser;

	// Build a valid UBX frame (class/id arbitrary; content not decoded).
	std::ਸਦਿਸ਼_2<uint8_t> build_Ubx_ਚੌਖਟਾ(uint8_t ਸੁਨੇਹਾ_class, uint8_t ਸੁਨੇਹਾ_id,
					   ਅਚਲ std::ਸਦਿਸ਼_2<uint8_t> &payload)
	{
		ਅਚਲ size_t payload_len = payload.ਮਾਪ();
		std::ਸਦਿਸ਼_2<uint8_t> ਚੌਖਟਾ;
		ਚੌਖਟਾ.push_back(UBX_SYNC1);
		ਚੌਖਟਾ.push_back(UBX_SYNC2);
		ਚੌਖਟਾ.push_back(ਸੁਨੇਹਾ_class);
		ਚੌਖਟਾ.push_back(ਸੁਨੇਹਾ_id);
		ਚੌਖਟਾ.push_back(static_cast<uint8_t>(payload_len & 0xFF));
		ਚੌਖਟਾ.push_back(static_cast<uint8_t>((payload_len >> 8) & 0xFF));
		ਚੌਖਟਾ.ਪਾਓ(ਚੌਖਟਾ.ਸਿਰਾ(), payload.begin(), payload.ਸਿਰਾ());

		uint8_t ck_a = 0;
		uint8_t ck_b = 0;
		ubx_checksum(&ਚੌਖਟਾ[2], UBX_ਸਿਰਲੇਖ_LEN - 2 + payload_len, &ck_a, &ck_b);
		ਚੌਖਟਾ.push_back(ck_a);
		ਚੌਖਟਾ.push_back(ck_b);
		ਵਾਪਸ ਚੌਖਟਾ;
	}

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

#ਜੇ defined(CONFIG_GPS_SPARTN)
	std::ਸਦਿਸ਼_2<uint8_t> build_Spartn_ਚੌਖਟਾ(uint8_t ਸੁਨੇਹਾ_type_2, ਅਚਲ std::ਸਦਿਸ਼_2<uint8_t> &payload,
					      uint8_t ਚੱਕਰੀ_ਜਾਂਚ_type = 0)
	{
		ਅਚਲ size_t payload_len = payload.ਮਾਪ();
		std::ਸਦਿਸ਼_2<uint8_t> ਚੌਖਟਾ;
		ਚੌਖਟਾ.push_back(SPARTN_PREAMBLE);

		uint8_t b1 = static_cast<uint8_t>(((ਸੁਨੇਹਾ_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((ਚੱਕਰੀ_ਜਾਂਚ_type & 0x03) << 4));
		ਚੌਖਟਾ.push_back(b1);
		ਚੌਖਟਾ.push_back(b2);
		ਚੌਖਟਾ.push_back(b3);

		// 4-byte description, 16-bit time tag
		ਚੌਖਟਾ.push_back(0x00);
		ਚੌਖਟਾ.push_back(0x00);
		ਚੌਖਟਾ.push_back(0x00);
		ਚੌਖਟਾ.push_back(0x00);

		ਚੌਖਟਾ.ਪਾਓ(ਚੌਖਟਾ.ਸਿਰਾ(), payload.begin(), payload.ਸਿਰਾ());

		ਅਚਲ uint32_t ਚੱਕਰੀ_ਜਾਂਚ = spartn_ਸੁਨੇਹਾ_ਚੱਕਰੀ_ਜਾਂਚ(&ਚੌਖਟਾ[1], ਚੌਖਟਾ.ਮਾਪ() - 1, ਚੱਕਰੀ_ਜਾਂਚ_type);
		ਅਚਲ size_t ਚੱਕਰੀ_ਜਾਂਚ_ਬਾਈਟਾਂ = static_cast<size_t>(ਚੱਕਰੀ_ਜਾਂਚ_type) + 1;

		ਲਈ (ਪੂਰਨ i = static_cast<ਪੂਰਨ>(ਚੱਕਰੀ_ਜਾਂਚ_ਬਾਈਟਾਂ) - 1; i >= 0; i--) {
			ਚੌਖਟਾ.push_back(static_cast<uint8_t>((ਚੱਕਰੀ_ਜਾਂਚ >>(8 * i)) & 0xFF));
		}

		ਵਾਪਸ ਚੌਖਟਾ;
	}
#ਜੇ_ਅੰਤ
};

// =============================================================================
// Checksum
// =============================================================================

// Empty ACK-ACK (class=0x05, id=0x01, length=0): covered bytes 05 01 00 00.
// 8-bit Fletcher: a runs 5,6,6,6 and b runs 5,11,17,23 → CK_A=0x06, CK_B=0x17.
TEST_F(UbxTest, Checksum_ਗਿਆਤ_ਸਦਿਸ਼)
{
	ਅਚਲ uint8_t covered[] = {0x05, 0x01, 0x00, 0x00};
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(covered, ਆਕਾਰ(covered), &ck_a, &ck_b);
	EXPECT_EQ(ck_a, 0x06u);
	EXPECT_EQ(ck_b, 0x17u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(UbxTest, Single_ਨਿਊਨਤਮ_ਚੌਖਟਾ)
{
	// UBX-MGA class is 0x13; any class/id is fine for framing
	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_Ubx_ਚੌਖਟਾ(0x13, 0x00, {0xAA, 0xBB});
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ਅਚਲ uint8_t *ਸੁਨੇਹਾ_2 = parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len, &protocol);
	ASSERT_NE(ਸੁਨੇਹਾ_2, ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, ਚੌਖਟਾ.ਮਾਪ());
	EXPECT_EQ(0, memcmp(ਸੁਨੇਹਾ_2, ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ()));
	parser.consume_ਸੁਨੇਹਾ(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().ਚੱਕਰੀ_ਜਾਂਚ_errors_2, 0u);
}

TEST_F(UbxTest, ਖਾਲੀ_ਥਾਵਾਂ_Payload_ਚੌਖਟਾ)
{
	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_Ubx_ਚੌਖਟਾ(0x05, 0x01, {});
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());

	size_t len = 0;
	ASSERT_NE(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len), ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_EQ(len, UBX_ਸਿਰਲੇਖ_LEN + UBX_CK_LEN);
	parser.consume_ਸੁਨੇਹਾ(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, ਗੁਣਜ_ਚੌਖਟੇ)
{
	ਸਵੈਚਾਲਿਤ f1 = build_Ubx_ਚੌਖਟਾ(0x13, 0x00, {0x01});
	ਸਵੈਚਾਲਿਤ f2 = build_Ubx_ਚੌਖਟਾ(0x13, 0x02, {0x02, 0x03, 0x04});
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
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
}

TEST_F(UbxTest, Chunked_ਆਮਦ)
{
	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_Ubx_ਚੌਖਟਾ(0x13, 0x01, {0x10, 0x20, 0x30, 0x40});

	ਲਈ (size_t i = 0; i < ਚੌਖਟਾ.ਮਾਪ(); i++) {
		parser.ਜੋੜੋ_ਅੰਕੜੇ(&ਚੌਖਟਾ[i], 1);
		// No complete frame until the last byte
	}

	size_t len = 0;
	ASSERT_NE(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len), ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_EQ(len, ਚੌਖਟਾ.ਮਾਪ());
}

TEST_F(UbxTest, Noise_Before_ਚੌਖਟਾ)
{
	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_Ubx_ਚੌਖਟਾ(0x13, 0x00, {0x55});
	std::ਸਦਿਸ਼_2<uint8_t> ਧਾਰਾ = {0x00, 0xFF, 0x01};
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), ਚੌਖਟਾ.begin(), ਚੌਖਟਾ.ਸਿਰਾ());
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਧਾਰਾ.ਅੰਕੜੇ(), ਧਾਰਾ.ਮਾਪ());

	size_t len = 0;
	ASSERT_NE(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len), ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_GT(parser.getStats().ਬਾਈਟਾਂ_discarded_2, 0u);
	parser.consume_ਸੁਨੇਹਾ(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, BadSync2)
{
	// 0xB5 followed by something other than 0x62 is not a frame start
	std::ਸਦਿਸ਼_2<uint8_t> ਧਾਰਾ = {UBX_SYNC1, 0x00, 0x11, 0x22};
	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_Ubx_ਚੌਖਟਾ(0x13, 0x00, {0x01});
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), ਚੌਖਟਾ.begin(), ਚੌਖਟਾ.ਸਿਰਾ());
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਧਾਰਾ.ਅੰਕੜੇ(), ਧਾਰਾ.ਮਾਪ());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len, &protocol), ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_ਸੁਨੇਹਾ(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_GT(parser.getStats().ਬਾਈਟਾਂ_discarded_2, 0u);
}

TEST_F(UbxTest, BadChecksum)
{
	ਸਵੈਚਾਲਿਤ ਚੌਖਟਾ = build_Ubx_ਚੌਖਟਾ(0x13, 0x00, {0x11, 0x22});
	ਚੌਖਟਾ.ਪਿੱਠ() ^= 0xFF;
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());

	size_t len = 0;
	EXPECT_EQ(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len), ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_GE(parser.getStats().ਚੱਕਰੀ_ਜਾਂਚ_errors_2, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 0u);
}

TEST_F(UbxTest, RejectsOversizedPayload)
{
	// Length field claims UBX_MAX_PAYLOAD_LEN + 1 — must not pin the buffer
	std::ਸਦਿਸ਼_2<uint8_t> ਸਿਰਲੇਖ = {
		UBX_SYNC1, UBX_SYNC2,
		0x13, 0x00,
		static_cast<uint8_t>((UBX_MAX_PAYLOAD_LEN + 1) & 0xFF),
		static_cast<uint8_t>(((UBX_MAX_PAYLOAD_LEN + 1) >> 8) & 0xFF),
	};
	ਸਵੈਚਾਲਿਤ good = build_Ubx_ਚੌਖਟਾ(0x13, 0x00, {0xAB});
	ਸਿਰਲੇਖ.ਪਾਓ(ਸਿਰਲੇਖ.ਸਿਰਾ(), good.begin(), good.ਸਿਰਾ());
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਸਿਰਲੇਖ.ਅੰਕੜੇ(), ਸਿਰਲੇਖ.ਮਾਪ());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len, &protocol), ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, good.ਮਾਪ());
	parser.consume_ਸੁਨੇਹਾ(len);
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
	ਸਵੈਚਾਲਿਤ mga1 = build_Ubx_ਚੌਖਟਾ(0x13, 0x00, {0x01, 0x02, 0x03});
	ਸਵੈਚਾਲਿਤ mga2 = build_Ubx_ਚੌਖਟਾ(0x13, 0x02, {0x10, 0x20});
	ਸਵੈਚਾਲਿਤ rtcm = build_Rtcm_ਚੌਖਟਾ(rng, 40);

	std::ਸਦਿਸ਼_2<uint8_t> ਧਾਰਾ;
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), mga1.begin(), mga1.ਸਿਰਾ());
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), mga2.begin(), mga2.ਸਿਰਾ());
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), rtcm.begin(), rtcm.ਸਿਰਾ());
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਧਾਰਾ.ਅੰਕੜੇ(), ਧਾਰਾ.ਮਾਪ());

	std::ਸਦਿਸ਼_2<CorrectionProtocol> ਵੇਖਿਆ;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	ਜਦੋਂ (parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len, &protocol) != ਖਾਲੀ_ਸੂਚਕ) {
		ਵੇਖਿਆ.push_back(protocol);
		parser.consume_ਸੁਨੇਹਾ(len);
	}

	ASSERT_EQ(ਵੇਖਿਆ.ਮਾਪ(), 3u);
	EXPECT_EQ(ਵੇਖਿਆ[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(ਵੇਖਿਆ[1], CorrectionProtocol::Ubx);
	EXPECT_EQ(ਵੇਖਿਆ[2], CorrectionProtocol::Rtcm3);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().ਚੱਕਰੀ_ਜਾਂਚ_errors_2, 0u);
}

#ਜੇ defined(CONFIG_GPS_SPARTN)
TEST_F(UbxTest, MgaThenSpartn)
{
	ਸਵੈਚਾਲਿਤ mga = build_Ubx_ਚੌਖਟਾ(0x13, 0x00, {0xDE, 0xAD});
	ਸਵੈਚਾਲਿਤ spartn = build_Spartn_ਚੌਖਟਾ(0, {0xAA, 0xBB});

	std::ਸਦਿਸ਼_2<uint8_t> ਧਾਰਾ;
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), mga.begin(), mga.ਸਿਰਾ());
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), spartn.begin(), spartn.ਸਿਰਾ());
	// Another MGA mid-stream should also work (reconnect-style)
	ਸਵੈਚਾਲਿਤ mga2 = build_Ubx_ਚੌਖਟਾ(0x13, 0x01, {0x01});
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), mga2.begin(), mga2.ਸਿਰਾ());
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਧਾਰਾ.ਅੰਕੜੇ(), ਧਾਰਾ.ਮਾਪ());

	std::ਸਦਿਸ਼_2<CorrectionProtocol> ਵੇਖਿਆ;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	ਜਦੋਂ (parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len, &protocol) != ਖਾਲੀ_ਸੂਚਕ) {
		ਵੇਖਿਆ.push_back(protocol);
		parser.consume_ਸੁਨੇਹਾ(len);
	}

	ASSERT_EQ(ਵੇਖਿਆ.ਮਾਪ(), 3u);
	EXPECT_EQ(ਵੇਖਿਆ[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(ਵੇਖਿਆ[1], CorrectionProtocol::Spartn);
	EXPECT_EQ(ਵੇਖਿਆ[2], CorrectionProtocol::Ubx);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
}
#ਜੇ_ਅੰਤ

// A complete RTCM3 frame consumes its payload, so a 0xB5 that happens to sit
// inside that payload is never considered as a UBX start.
TEST_F(UbxTest, RtcmPayloadStrayB5IsNotFramedAsUbx)
{
	// Craft an RTCM3 frame whose payload contains a UBX-looking prefix.
	// buildRtcmFrame randomizes payload; force a 0xB5 0x62 sequence in raw form.
	std::ਸਦਿਸ਼_2<uint8_t> payload(32, 0x00);
	payload[4] = UBX_SYNC1;
	payload[5] = UBX_SYNC2;
	payload[6] = 0x13;
	payload[7] = 0x00;
	payload[8] = 0x02; // fake length
	payload[9] = 0x00;

	std::ਸਦਿਸ਼_2<uint8_t> ਚੌਖਟਾ;
	ਚੌਖਟਾ.push_back(RTCM3_PREAMBLE);
	ਚੌਖਟਾ.push_back(0x00);
	ਚੌਖਟਾ.push_back(static_cast<uint8_t>(payload.ਮਾਪ()));
	ਚੌਖਟਾ.ਪਾਓ(ਚੌਖਟਾ.ਸਿਰਾ(), payload.begin(), payload.ਸਿਰਾ());
	ਅਚਲ uint32_t ਚੱਕਰੀ_ਜਾਂਚ = rtcm3_crc24q(ਚੌਖਟਾ.ਅੰਕੜੇ(), ਚੌਖਟਾ.ਮਾਪ());
	ਚੌਖਟਾ.push_back((ਚੱਕਰੀ_ਜਾਂਚ >> 16) & 0xFF);
	ਚੌਖਟਾ.push_back((ਚੱਕਰੀ_ਜਾਂਚ >> 8) & 0xFF);
	ਚੌਖਟਾ.push_back(ਚੱਕਰੀ_ਜਾਂਚ & 0xFF);

	// Follow with a real UBX so we know the framer still works after
	ਸਵੈਚਾਲਿਤ ubx = build_Ubx_ਚੌਖਟਾ(0x13, 0x00, {0x99});
	std::ਸਦਿਸ਼_2<uint8_t> ਧਾਰਾ = ਚੌਖਟਾ;
	ਧਾਰਾ.ਪਾਓ(ਧਾਰਾ.ਸਿਰਾ(), ubx.begin(), ubx.ਸਿਰਾ());
	parser.ਜੋੜੋ_ਅੰਕੜੇ(ਧਾਰਾ.ਅੰਕੜੇ(), ਧਾਰਾ.ਮਾਪ());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Ubx;
	ASSERT_NE(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len, &protocol), ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
	parser.consume_ਸੁਨੇਹਾ(len);

	ASSERT_NE(parser.get_ਅਗਲਾ_ਸੁਨੇਹਾ(&len, &protocol), ਖਾਲੀ_ਸੂਚਕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_ਸੁਨੇਹਾ(len);

	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().ਚੱਕਰੀ_ਜਾਂਚ_errors_2, 0u);
}
