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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ "correction_framer.h"

#అన్తర్భావ <vector>
#అన్తర్భావ <cstring>
#అన్తర్భావ <random>

ఉపయోగించి నామాకాశ gnss;

తరగతి UbxTest : బహిరంగ ::testing::Test
{
రక్షిత:
	CorrectionFramer parser;

	// Build a valid UBX frame (class/id arbitrary; content not decoded).
	std::సదిశ_2<uint8_t> build_Ubx_చట్రం(uint8_t సందేశం_class, uint8_t సందేశం_id,
					   స్థిరరాశి std::సదిశ_2<uint8_t> &payload)
	{
		స్థిరరాశి size_t payload_len = payload.కొలత();
		std::సదిశ_2<uint8_t> చట్రం;
		చట్రం.push_back(UBX_SYNC1);
		చట్రం.push_back(UBX_SYNC2);
		చట్రం.push_back(సందేశం_class);
		చట్రం.push_back(సందేశం_id);
		చట్రం.push_back(static_cast<uint8_t>(payload_len & 0xFF));
		చట్రం.push_back(static_cast<uint8_t>((payload_len >> 8) & 0xFF));
		చట్రం.చొప్పించు(చట్రం.చివర(), payload.begin(), payload.చివర());

		uint8_t ck_a = 0;
		uint8_t ck_b = 0;
		ubx_checksum(&చట్రం[2], UBX_శీర్షిక_LEN - 2 + payload_len, &ck_a, &ck_b);
		చట్రం.push_back(ck_a);
		చట్రం.push_back(ck_b);
		తిరిగి చట్రం;
	}

	std::సదిశ_2<uint8_t> build_Rtcm_చట్రం(std::mt19937 &rng, size_t payload_len)
	{
		std::సదిశ_2<uint8_t> చట్రం;
		చట్రం.push_back(RTCM3_PREAMBLE);
		చట్రం.push_back(static_cast<uint8_t>((payload_len >> 8) & 0x03));
		చట్రం.push_back(static_cast<uint8_t>(payload_len & 0xFF));

		std::ఏకరీతి_int_distribution<పూర్ణాంకం> byte(0, 255);

		కొరకు (size_t i = 0; i < payload_len; i++) {
			చట్రం.push_back(static_cast<uint8_t>(byte(rng)));
		}

		స్థిరరాశి uint32_t చక్రీయ_తనిఖీ = rtcm3_crc24q(చట్రం.దత్తాంశం(), చట్రం.కొలత());
		చట్రం.push_back((చక్రీయ_తనిఖీ >> 16) & 0xFF);
		చట్రం.push_back((చక్రీయ_తనిఖీ >> 8) & 0xFF);
		చట్రం.push_back(చక్రీయ_తనిఖీ & 0xFF);
		తిరిగి చట్రం;
	}

#ఒకవేళ defined(CONFIG_GPS_SPARTN)
	std::సదిశ_2<uint8_t> build_Spartn_చట్రం(uint8_t సందేశం_type_2, స్థిరరాశి std::సదిశ_2<uint8_t> &payload,
					      uint8_t చక్రీయ_తనిఖీ_type = 0)
	{
		స్థిరరాశి size_t payload_len = payload.కొలత();
		std::సదిశ_2<uint8_t> చట్రం;
		చట్రం.push_back(SPARTN_PREAMBLE);

		uint8_t b1 = static_cast<uint8_t>(((సందేశం_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((చక్రీయ_తనిఖీ_type & 0x03) << 4));
		చట్రం.push_back(b1);
		చట్రం.push_back(b2);
		చట్రం.push_back(b3);

		// 4-byte description, 16-bit time tag
		చట్రం.push_back(0x00);
		చట్రం.push_back(0x00);
		చట్రం.push_back(0x00);
		చట్రం.push_back(0x00);

		చట్రం.చొప్పించు(చట్రం.చివర(), payload.begin(), payload.చివర());

		స్థిరరాశి uint32_t చక్రీయ_తనిఖీ = spartn_సందేశం_చక్రీయ_తనిఖీ(&చట్రం[1], చట్రం.కొలత() - 1, చక్రీయ_తనిఖీ_type);
		స్థిరరాశి size_t చక్రీయ_తనిఖీ_బైట్లు = static_cast<size_t>(చక్రీయ_తనిఖీ_type) + 1;

		కొరకు (పూర్ణాంకం i = static_cast<పూర్ణాంకం>(చక్రీయ_తనిఖీ_బైట్లు) - 1; i >= 0; i--) {
			చట్రం.push_back(static_cast<uint8_t>((చక్రీయ_తనిఖీ >>(8 * i)) & 0xFF));
		}

		తిరిగి చట్రం;
	}
#అయితే_ముగింపు
};

// =============================================================================
// Checksum
// =============================================================================

// Empty ACK-ACK (class=0x05, id=0x01, length=0): covered bytes 05 01 00 00.
// 8-bit Fletcher: a runs 5,6,6,6 and b runs 5,11,17,23 → CK_A=0x06, CK_B=0x17.
TEST_F(UbxTest, Checksum_తెలిసిన_సదిశ)
{
	స్థిరరాశి uint8_t covered[] = {0x05, 0x01, 0x00, 0x00};
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(covered, పరిమాణం(covered), &ck_a, &ck_b);
	EXPECT_EQ(ck_a, 0x06u);
	EXPECT_EQ(ck_b, 0x17u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(UbxTest, Single_కనీస_చట్రం)
{
	// UBX-MGA class is 0x13; any class/id is fine for framing
	స్వయంచాలక చట్రం = build_Ubx_చట్రం(0x13, 0x00, {0xAA, 0xBB});
	parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	స్థిరరాశి uint8_t *సందేశం_2 = parser.get_తదుపరి_సందేశం(&len, &protocol);
	ASSERT_NE(సందేశం_2, శూన్యసూచిక);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, చట్రం.కొలత());
	EXPECT_EQ(0, memcmp(సందేశం_2, చట్రం.దత్తాంశం(), చట్రం.కొలత()));
	parser.consume_సందేశం(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().చక్రీయ_తనిఖీ_errors_2, 0u);
}

TEST_F(UbxTest, ఖాళీలు_Payload_చట్రం)
{
	స్వయంచాలక చట్రం = build_Ubx_చట్రం(0x05, 0x01, {});
	parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

	size_t len = 0;
	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
	EXPECT_EQ(len, UBX_శీర్షిక_LEN + UBX_CK_LEN);
	parser.consume_సందేశం(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, గుణిజం_చట్రాలు)
{
	స్వయంచాలక f1 = build_Ubx_చట్రం(0x13, 0x00, {0x01});
	స్వయంచాలక f2 = build_Ubx_చట్రం(0x13, 0x02, {0x02, 0x03, 0x04});
	std::సదిశ_2<uint8_t> ప్రవాహం;
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), f1.begin(), f1.చివర());
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), f2.begin(), f2.చివర());
	parser.కూడు_దత్తాంశం(ప్రవాహం.దత్తాంశం(), ప్రవాహం.కొలత());

	పూర్ణాంకం లెక్క = 0;
	size_t len = 0;

	అయితే (parser.get_తదుపరి_సందేశం(&len) != శూన్యసూచిక) {
		parser.consume_సందేశం(len);
		లెక్క++;
	}

	EXPECT_EQ(లెక్క, 2);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
}

TEST_F(UbxTest, Chunked_రాక)
{
	స్వయంచాలక చట్రం = build_Ubx_చట్రం(0x13, 0x01, {0x10, 0x20, 0x30, 0x40});

	కొరకు (size_t i = 0; i < చట్రం.కొలత(); i++) {
		parser.కూడు_దత్తాంశం(&చట్రం[i], 1);
		// No complete frame until the last byte
	}

	size_t len = 0;
	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
	EXPECT_EQ(len, చట్రం.కొలత());
}

TEST_F(UbxTest, Noise_Before_చట్రం)
{
	స్వయంచాలక చట్రం = build_Ubx_చట్రం(0x13, 0x00, {0x55});
	std::సదిశ_2<uint8_t> ప్రవాహం = {0x00, 0xFF, 0x01};
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), చట్రం.begin(), చట్రం.చివర());
	parser.కూడు_దత్తాంశం(ప్రవాహం.దత్తాంశం(), ప్రవాహం.కొలత());

	size_t len = 0;
	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
	EXPECT_GT(parser.getStats().బైట్లు_discarded_2, 0u);
	parser.consume_సందేశం(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, BadSync2)
{
	// 0xB5 followed by something other than 0x62 is not a frame start
	std::సదిశ_2<uint8_t> ప్రవాహం = {UBX_SYNC1, 0x00, 0x11, 0x22};
	స్వయంచాలక చట్రం = build_Ubx_చట్రం(0x13, 0x00, {0x01});
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), చట్రం.begin(), చట్రం.చివర());
	parser.కూడు_దత్తాంశం(ప్రవాహం.దత్తాంశం(), ప్రవాహం.కొలత());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len, &protocol), శూన్యసూచిక);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_సందేశం(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_GT(parser.getStats().బైట్లు_discarded_2, 0u);
}

TEST_F(UbxTest, BadChecksum)
{
	స్వయంచాలక చట్రం = build_Ubx_చట్రం(0x13, 0x00, {0x11, 0x22});
	చట్రం.వెనుక() ^= 0xFF;
	parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

	size_t len = 0;
	EXPECT_EQ(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
	EXPECT_GE(parser.getStats().చక్రీయ_తనిఖీ_errors_2, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 0u);
}

TEST_F(UbxTest, RejectsOversizedPayload)
{
	// Length field claims UBX_MAX_PAYLOAD_LEN + 1 — must not pin the buffer
	std::సదిశ_2<uint8_t> శీర్షిక = {
		UBX_SYNC1, UBX_SYNC2,
		0x13, 0x00,
		static_cast<uint8_t>((UBX_MAX_PAYLOAD_LEN + 1) & 0xFF),
		static_cast<uint8_t>(((UBX_MAX_PAYLOAD_LEN + 1) >> 8) & 0xFF),
	};
	స్వయంచాలక good = build_Ubx_చట్రం(0x13, 0x00, {0xAB});
	శీర్షిక.చొప్పించు(శీర్షిక.చివర(), good.begin(), good.చివర());
	parser.కూడు_దత్తాంశం(శీర్షిక.దత్తాంశం(), శీర్షిక.కొలత());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len, &protocol), శూన్యసూచిక);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, good.కొలత());
	parser.consume_సందేశం(len);
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
	స్వయంచాలక mga1 = build_Ubx_చట్రం(0x13, 0x00, {0x01, 0x02, 0x03});
	స్వయంచాలక mga2 = build_Ubx_చట్రం(0x13, 0x02, {0x10, 0x20});
	స్వయంచాలక rtcm = build_Rtcm_చట్రం(rng, 40);

	std::సదిశ_2<uint8_t> ప్రవాహం;
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), mga1.begin(), mga1.చివర());
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), mga2.begin(), mga2.చివర());
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), rtcm.begin(), rtcm.చివర());
	parser.కూడు_దత్తాంశం(ప్రవాహం.దత్తాంశం(), ప్రవాహం.కొలత());

	std::సదిశ_2<CorrectionProtocol> చూసిన;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	అయితే (parser.get_తదుపరి_సందేశం(&len, &protocol) != శూన్యసూచిక) {
		చూసిన.push_back(protocol);
		parser.consume_సందేశం(len);
	}

	ASSERT_EQ(చూసిన.కొలత(), 3u);
	EXPECT_EQ(చూసిన[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(చూసిన[1], CorrectionProtocol::Ubx);
	EXPECT_EQ(చూసిన[2], CorrectionProtocol::Rtcm3);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().చక్రీయ_తనిఖీ_errors_2, 0u);
}

#ఒకవేళ defined(CONFIG_GPS_SPARTN)
TEST_F(UbxTest, MgaThenSpartn)
{
	స్వయంచాలక mga = build_Ubx_చట్రం(0x13, 0x00, {0xDE, 0xAD});
	స్వయంచాలక spartn = build_Spartn_చట్రం(0, {0xAA, 0xBB});

	std::సదిశ_2<uint8_t> ప్రవాహం;
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), mga.begin(), mga.చివర());
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), spartn.begin(), spartn.చివర());
	// Another MGA mid-stream should also work (reconnect-style)
	స్వయంచాలక mga2 = build_Ubx_చట్రం(0x13, 0x01, {0x01});
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), mga2.begin(), mga2.చివర());
	parser.కూడు_దత్తాంశం(ప్రవాహం.దత్తాంశం(), ప్రవాహం.కొలత());

	std::సదిశ_2<CorrectionProtocol> చూసిన;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	అయితే (parser.get_తదుపరి_సందేశం(&len, &protocol) != శూన్యసూచిక) {
		చూసిన.push_back(protocol);
		parser.consume_సందేశం(len);
	}

	ASSERT_EQ(చూసిన.కొలత(), 3u);
	EXPECT_EQ(చూసిన[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(చూసిన[1], CorrectionProtocol::Spartn);
	EXPECT_EQ(చూసిన[2], CorrectionProtocol::Ubx);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
}
#అయితే_ముగింపు

// A complete RTCM3 frame consumes its payload, so a 0xB5 that happens to sit
// inside that payload is never considered as a UBX start.
TEST_F(UbxTest, RtcmPayloadStrayB5IsNotFramedAsUbx)
{
	// Craft an RTCM3 frame whose payload contains a UBX-looking prefix.
	// buildRtcmFrame randomizes payload; force a 0xB5 0x62 sequence in raw form.
	std::సదిశ_2<uint8_t> payload(32, 0x00);
	payload[4] = UBX_SYNC1;
	payload[5] = UBX_SYNC2;
	payload[6] = 0x13;
	payload[7] = 0x00;
	payload[8] = 0x02; // fake length
	payload[9] = 0x00;

	std::సదిశ_2<uint8_t> చట్రం;
	చట్రం.push_back(RTCM3_PREAMBLE);
	చట్రం.push_back(0x00);
	చట్రం.push_back(static_cast<uint8_t>(payload.కొలత()));
	చట్రం.చొప్పించు(చట్రం.చివర(), payload.begin(), payload.చివర());
	స్థిరరాశి uint32_t చక్రీయ_తనిఖీ = rtcm3_crc24q(చట్రం.దత్తాంశం(), చట్రం.కొలత());
	చట్రం.push_back((చక్రీయ_తనిఖీ >> 16) & 0xFF);
	చట్రం.push_back((చక్రీయ_తనిఖీ >> 8) & 0xFF);
	చట్రం.push_back(చక్రీయ_తనిఖీ & 0xFF);

	// Follow with a real UBX so we know the framer still works after
	స్వయంచాలక ubx = build_Ubx_చట్రం(0x13, 0x00, {0x99});
	std::సదిశ_2<uint8_t> ప్రవాహం = చట్రం;
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), ubx.begin(), ubx.చివర());
	parser.కూడు_దత్తాంశం(ప్రవాహం.దత్తాంశం(), ప్రవాహం.కొలత());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Ubx;
	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len, &protocol), శూన్యసూచిక);
	EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
	parser.consume_సందేశం(len);

	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len, &protocol), శూన్యసూచిక);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_సందేశం(len);

	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().చక్రీయ_తనిఖీ_errors_2, 0u);
}
