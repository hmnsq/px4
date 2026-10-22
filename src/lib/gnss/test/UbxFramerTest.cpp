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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ "correction_framer.h"

#ಅನ್ತರ್ಭಾವ <vector>
#ಅನ್ತರ್ಭಾವ <cstring>
#ಅನ್ತರ್ಭಾವ <random>

ಬಳಸಿ ನಾಮಾಕಾಶ gnss;

ವರ್ಗ UbxTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಂರಕ್ಷಿತ:
	CorrectionFramer parser;

	// Build a valid UBX frame (class/id arbitrary; content not decoded).
	std::ಸದಿಶ_2<uint8_t> build_Ubx_ಚೌಕಟ್ಟು(uint8_t ಸಂದೇಶ_class, uint8_t ಸಂದೇಶ_id,
					   ಸ್ಥಿರಾಂಕ std::ಸದಿಶ_2<uint8_t> &payload)
	{
		ಸ್ಥಿರಾಂಕ size_t payload_len = payload.ಗಾತ್ರ_2();
		std::ಸದಿಶ_2<uint8_t> ಚೌಕಟ್ಟು;
		ಚೌಕಟ್ಟು.push_back(UBX_SYNC1);
		ಚೌಕಟ್ಟು.push_back(UBX_SYNC2);
		ಚೌಕಟ್ಟು.push_back(ಸಂದೇಶ_class);
		ಚೌಕಟ್ಟು.push_back(ಸಂದೇಶ_id);
		ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>(payload_len & 0xFF));
		ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>((payload_len >> 8) & 0xFF));
		ಚೌಕಟ್ಟು.ಸೇರಿಸು(ಚೌಕಟ್ಟು.ಕೊನೆ(), payload.begin(), payload.ಕೊನೆ());

		uint8_t ck_a = 0;
		uint8_t ck_b = 0;
		ubx_checksum(&ಚೌಕಟ್ಟು[2], UBX_ಶೀರ್ಷಿಕೆ_LEN - 2 + payload_len, &ck_a, &ck_b);
		ಚೌಕಟ್ಟು.push_back(ck_a);
		ಚೌಕಟ್ಟು.push_back(ck_b);
		ಹಿಂತಿರುಗಿ ಚೌಕಟ್ಟು;
	}

	std::ಸದಿಶ_2<uint8_t> build_Rtcm_ಚೌಕಟ್ಟು(std::mt19937 &rng, size_t payload_len)
	{
		std::ಸದಿಶ_2<uint8_t> ಚೌಕಟ್ಟು;
		ಚೌಕಟ್ಟು.push_back(RTCM3_PREAMBLE);
		ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>((payload_len >> 8) & 0x03));
		ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>(payload_len & 0xFF));

		std::ಏಕರೂಪ_int_distribution<ಪೂರ್ಣಾಂಕ> byte(0, 255);

		ಗಾಗಿ (size_t i = 0; i < payload_len; i++) {
			ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>(byte(rng)));
		}

		ಸ್ಥಿರಾಂಕ uint32_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = rtcm3_crc24q(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());
		ಚೌಕಟ್ಟು.push_back((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ >> 16) & 0xFF);
		ಚೌಕಟ್ಟು.push_back((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ >> 8) & 0xFF);
		ಚೌಕಟ್ಟು.push_back(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ & 0xFF);
		ಹಿಂತಿರುಗಿ ಚೌಕಟ್ಟು;
	}

#ಆದರೆ defined(CONFIG_GPS_SPARTN)
	std::ಸದಿಶ_2<uint8_t> build_Spartn_ಚೌಕಟ್ಟು(uint8_t ಸಂದೇಶ_type_2, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ_2<uint8_t> &payload,
					      uint8_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type = 0)
	{
		ಸ್ಥಿರಾಂಕ size_t payload_len = payload.ಗಾತ್ರ_2();
		std::ಸದಿಶ_2<uint8_t> ಚೌಕಟ್ಟು;
		ಚೌಕಟ್ಟು.push_back(SPARTN_PREAMBLE);

		uint8_t b1 = static_cast<uint8_t>(((ಸಂದೇಶ_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type & 0x03) << 4));
		ಚೌಕಟ್ಟು.push_back(b1);
		ಚೌಕಟ್ಟು.push_back(b2);
		ಚೌಕಟ್ಟು.push_back(b3);

		// 4-byte description, 16-bit time tag
		ಚೌಕಟ್ಟು.push_back(0x00);
		ಚೌಕಟ್ಟು.push_back(0x00);
		ಚೌಕಟ್ಟು.push_back(0x00);
		ಚೌಕಟ್ಟು.push_back(0x00);

		ಚೌಕಟ್ಟು.ಸೇರಿಸು(ಚೌಕಟ್ಟು.ಕೊನೆ(), payload.begin(), payload.ಕೊನೆ());

		ಸ್ಥಿರಾಂಕ uint32_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = spartn_ಸಂದೇಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(&ಚೌಕಟ್ಟು[1], ಚೌಕಟ್ಟು.ಗಾತ್ರ_2() - 1, ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type);
		ಸ್ಥಿರಾಂಕ size_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಬೈಟುಗಳು = static_cast<size_t>(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type) + 1;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = static_cast<ಪೂರ್ಣಾಂಕ>(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಬೈಟುಗಳು) - 1; i >= 0; i--) {
			ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ >>(8 * i)) & 0xFF));
		}

		ಹಿಂತಿರುಗಿ ಚೌಕಟ್ಟು;
	}
#ಆದರೆ_ಅಂತ್ಯ
};

// =============================================================================
// Checksum
// =============================================================================

// Empty ACK-ACK (class=0x05, id=0x01, length=0): covered bytes 05 01 00 00.
// 8-bit Fletcher: a runs 5,6,6,6 and b runs 5,11,17,23 → CK_A=0x06, CK_B=0x17.
TEST_F(UbxTest, Checksum_ತಿಳಿದ_ಸದಿಶ)
{
	ಸ್ಥಿರಾಂಕ uint8_t covered[] = {0x05, 0x01, 0x00, 0x00};
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(covered, ಗಾತ್ರ(covered), &ck_a, &ck_b);
	EXPECT_EQ(ck_a, 0x06u);
	EXPECT_EQ(ck_b, 0x17u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(UbxTest, Single_ಕನಿಷ್ಠತಮ_ಚೌಕಟ್ಟು)
{
	// UBX-MGA class is 0x13; any class/id is fine for framing
	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x00, {0xAA, 0xBB});
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ಸ್ಥಿರಾಂಕ uint8_t *ಸಂದೇಶ_2 = parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len, &protocol);
	ASSERT_NE(ಸಂದೇಶ_2, ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());
	EXPECT_EQ(0, memcmp(ಸಂದೇಶ_2, ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2()));
	parser.consume_ಸಂದೇಶ(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 0u);
}

TEST_F(UbxTest, ಖಾಲಿಗಳು_Payload_ಚೌಕಟ್ಟು)
{
	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_Ubx_ಚೌಕಟ್ಟು(0x05, 0x01, {});
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());

	size_t len = 0;
	ASSERT_NE(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(len, UBX_ಶೀರ್ಷಿಕೆ_LEN + UBX_CK_LEN);
	parser.consume_ಸಂದೇಶ(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, ಅಪವರ್ತ್ಯ_ಚೌಕಟ್ಟುಗಳು)
{
	ಸ್ವಯಂಚಾಲಿತ f1 = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x00, {0x01});
	ಸ್ವಯಂಚಾಲಿತ f2 = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x02, {0x02, 0x03, 0x04});
	std::ಸದಿಶ_2<uint8_t> ಹರಿವು;
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), f1.begin(), f1.ಕೊನೆ());
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), f2.begin(), f2.ಕೊನೆ());
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಹರಿವು.ದತ್ತಾಂಶ(), ಹರಿವು.ಗಾತ್ರ_2());

	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;
	size_t len = 0;

	ಆಗ (parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len) != ಶೂನ್ಯಸೂಚಕ) {
		parser.consume_ಸಂದೇಶ(len);
		ಎಣಿಕೆ_2++;
	}

	EXPECT_EQ(ಎಣಿಕೆ_2, 2);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
}

TEST_F(UbxTest, Chunked_ಆಗಮನ)
{
	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x01, {0x10, 0x20, 0x30, 0x40});

	ಗಾಗಿ (size_t i = 0; i < ಚೌಕಟ್ಟು.ಗಾತ್ರ_2(); i++) {
		parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(&ಚೌಕಟ್ಟು[i], 1);
		// No complete frame until the last byte
	}

	size_t len = 0;
	ASSERT_NE(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(len, ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());
}

TEST_F(UbxTest, Noise_Before_ಚೌಕಟ್ಟು)
{
	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x00, {0x55});
	std::ಸದಿಶ_2<uint8_t> ಹರಿವು = {0x00, 0xFF, 0x01};
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), ಚೌಕಟ್ಟು.begin(), ಚೌಕಟ್ಟು.ಕೊನೆ());
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಹರಿವು.ದತ್ತಾಂಶ(), ಹರಿವು.ಗಾತ್ರ_2());

	size_t len = 0;
	ASSERT_NE(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_GT(parser.getStats().ಬೈಟುಗಳು_discarded_2, 0u);
	parser.consume_ಸಂದೇಶ(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, BadSync2)
{
	// 0xB5 followed by something other than 0x62 is not a frame start
	std::ಸದಿಶ_2<uint8_t> ಹರಿವು = {UBX_SYNC1, 0x00, 0x11, 0x22};
	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x00, {0x01});
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), ಚೌಕಟ್ಟು.begin(), ಚೌಕಟ್ಟು.ಕೊನೆ());
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಹರಿವು.ದತ್ತಾಂಶ(), ಹರಿವು.ಗಾತ್ರ_2());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len, &protocol), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_ಸಂದೇಶ(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_GT(parser.getStats().ಬೈಟುಗಳು_discarded_2, 0u);
}

TEST_F(UbxTest, BadChecksum)
{
	ಸ್ವಯಂಚಾಲಿತ ಚೌಕಟ್ಟು = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x00, {0x11, 0x22});
	ಚೌಕಟ್ಟು.ಹಿಂಭಾಗ() ^= 0xFF;
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());

	size_t len = 0;
	EXPECT_EQ(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_GE(parser.getStats().ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 0u);
}

TEST_F(UbxTest, RejectsOversizedPayload)
{
	// Length field claims UBX_MAX_PAYLOAD_LEN + 1 — must not pin the buffer
	std::ಸದಿಶ_2<uint8_t> ಶೀರ್ಷಿಕೆ = {
		UBX_SYNC1, UBX_SYNC2,
		0x13, 0x00,
		static_cast<uint8_t>((UBX_MAX_PAYLOAD_LEN + 1) & 0xFF),
		static_cast<uint8_t>(((UBX_MAX_PAYLOAD_LEN + 1) >> 8) & 0xFF),
	};
	ಸ್ವಯಂಚಾಲಿತ good = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x00, {0xAB});
	ಶೀರ್ಷಿಕೆ.ಸೇರಿಸು(ಶೀರ್ಷಿಕೆ.ಕೊನೆ(), good.begin(), good.ಕೊನೆ());
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಶೀರ್ಷಿಕೆ.ದತ್ತಾಂಶ(), ಶೀರ್ಷಿಕೆ.ಗಾತ್ರ_2());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len, &protocol), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, good.ಗಾತ್ರ_2());
	parser.consume_ಸಂದೇಶ(len);
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
	ಸ್ವಯಂಚಾಲಿತ mga1 = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x00, {0x01, 0x02, 0x03});
	ಸ್ವಯಂಚಾಲಿತ mga2 = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x02, {0x10, 0x20});
	ಸ್ವಯಂಚಾಲಿತ rtcm = build_Rtcm_ಚೌಕಟ್ಟು(rng, 40);

	std::ಸದಿಶ_2<uint8_t> ಹರಿವು;
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), mga1.begin(), mga1.ಕೊನೆ());
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), mga2.begin(), mga2.ಕೊನೆ());
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), rtcm.begin(), rtcm.ಕೊನೆ());
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಹರಿವು.ದತ್ತಾಂಶ(), ಹರಿವು.ಗಾತ್ರ_2());

	std::ಸದಿಶ_2<CorrectionProtocol> ನೋಡಿದ;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	ಆಗ (parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len, &protocol) != ಶೂನ್ಯಸೂಚಕ) {
		ನೋಡಿದ.push_back(protocol);
		parser.consume_ಸಂದೇಶ(len);
	}

	ASSERT_EQ(ನೋಡಿದ.ಗಾತ್ರ_2(), 3u);
	EXPECT_EQ(ನೋಡಿದ[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(ನೋಡಿದ[1], CorrectionProtocol::Ubx);
	EXPECT_EQ(ನೋಡಿದ[2], CorrectionProtocol::Rtcm3);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 0u);
}

#ಆದರೆ defined(CONFIG_GPS_SPARTN)
TEST_F(UbxTest, MgaThenSpartn)
{
	ಸ್ವಯಂಚಾಲಿತ mga = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x00, {0xDE, 0xAD});
	ಸ್ವಯಂಚಾಲಿತ spartn = build_Spartn_ಚೌಕಟ್ಟು(0, {0xAA, 0xBB});

	std::ಸದಿಶ_2<uint8_t> ಹರಿವು;
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), mga.begin(), mga.ಕೊನೆ());
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), spartn.begin(), spartn.ಕೊನೆ());
	// Another MGA mid-stream should also work (reconnect-style)
	ಸ್ವಯಂಚಾಲಿತ mga2 = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x01, {0x01});
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), mga2.begin(), mga2.ಕೊನೆ());
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಹರಿವು.ದತ್ತಾಂಶ(), ಹರಿವು.ಗಾತ್ರ_2());

	std::ಸದಿಶ_2<CorrectionProtocol> ನೋಡಿದ;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	ಆಗ (parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len, &protocol) != ಶೂನ್ಯಸೂಚಕ) {
		ನೋಡಿದ.push_back(protocol);
		parser.consume_ಸಂದೇಶ(len);
	}

	ASSERT_EQ(ನೋಡಿದ.ಗಾತ್ರ_2(), 3u);
	EXPECT_EQ(ನೋಡಿದ[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(ನೋಡಿದ[1], CorrectionProtocol::Spartn);
	EXPECT_EQ(ನೋಡಿದ[2], CorrectionProtocol::Ubx);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
}
#ಆದರೆ_ಅಂತ್ಯ

// A complete RTCM3 frame consumes its payload, so a 0xB5 that happens to sit
// inside that payload is never considered as a UBX start.
TEST_F(UbxTest, RtcmPayloadStrayB5IsNotFramedAsUbx)
{
	// Craft an RTCM3 frame whose payload contains a UBX-looking prefix.
	// buildRtcmFrame randomizes payload; force a 0xB5 0x62 sequence in raw form.
	std::ಸದಿಶ_2<uint8_t> payload(32, 0x00);
	payload[4] = UBX_SYNC1;
	payload[5] = UBX_SYNC2;
	payload[6] = 0x13;
	payload[7] = 0x00;
	payload[8] = 0x02; // fake length
	payload[9] = 0x00;

	std::ಸದಿಶ_2<uint8_t> ಚೌಕಟ್ಟು;
	ಚೌಕಟ್ಟು.push_back(RTCM3_PREAMBLE);
	ಚೌಕಟ್ಟು.push_back(0x00);
	ಚೌಕಟ್ಟು.push_back(static_cast<uint8_t>(payload.ಗಾತ್ರ_2()));
	ಚೌಕಟ್ಟು.ಸೇರಿಸು(ಚೌಕಟ್ಟು.ಕೊನೆ(), payload.begin(), payload.ಕೊನೆ());
	ಸ್ಥಿರಾಂಕ uint32_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = rtcm3_crc24q(ಚೌಕಟ್ಟು.ದತ್ತಾಂಶ(), ಚೌಕಟ್ಟು.ಗಾತ್ರ_2());
	ಚೌಕಟ್ಟು.push_back((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ >> 16) & 0xFF);
	ಚೌಕಟ್ಟು.push_back((ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ >> 8) & 0xFF);
	ಚೌಕಟ್ಟು.push_back(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ & 0xFF);

	// Follow with a real UBX so we know the framer still works after
	ಸ್ವಯಂಚಾಲಿತ ubx = build_Ubx_ಚೌಕಟ್ಟು(0x13, 0x00, {0x99});
	std::ಸದಿಶ_2<uint8_t> ಹರಿವು = ಚೌಕಟ್ಟು;
	ಹರಿವು.ಸೇರಿಸು(ಹರಿವು.ಕೊನೆ(), ubx.begin(), ubx.ಕೊನೆ());
	parser.ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಹರಿವು.ದತ್ತಾಂಶ(), ಹರಿವು.ಗಾತ್ರ_2());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Ubx;
	ASSERT_NE(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len, &protocol), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
	parser.consume_ಸಂದೇಶ(len);

	ASSERT_NE(parser.get_ಮುಂದಿನ_ಸಂದೇಶ(&len, &protocol), ಶೂನ್ಯಸೂಚಕ);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_ಸಂದೇಶ(len);

	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors_2, 0u);
}
