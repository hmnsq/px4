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

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव "correction_framer.h"

#अन्तर्भाव <vector>
#अन्तर्भाव <cstring>
#अन्तर्भाव <random>

उपयुज्य नामाकाश gnss;

वर्ग UbxTest : सार्वजनिक ::testing::Test
{
रक्षित:
	CorrectionFramer parser;

	// Build a valid UBX frame (class/id arbitrary; content not decoded).
	std::सदिशम्_2<uint8_t> buildUbxFrame(uint8_t सन्देशः_class, uint8_t सन्देशः_id,
					   ध्रुव std::सदिशम्_2<uint8_t> &payload)
	{
		ध्रुव size_t payload_len = payload.आकार();
		std::सदिशम्_2<uint8_t> frame;
		frame.push_back(UBX_SYNC1);
		frame.push_back(UBX_SYNC2);
		frame.push_back(सन्देशः_class);
		frame.push_back(सन्देशः_id);
		frame.push_back(static_cast<uint8_t>(payload_len & 0xFF));
		frame.push_back(static_cast<uint8_t>((payload_len >> 8) & 0xFF));
		frame.निवेशय(frame.अन्तः_2(), payload.begin(), payload.अन्तः_2());

		uint8_t ck_a = 0;
		uint8_t ck_b = 0;
		ubx_checksum(&frame[2], UBX_शीर्षकम्_LEN - 2 + payload_len, &ck_a, &ck_b);
		frame.push_back(ck_a);
		frame.push_back(ck_b);
		प्रत्यागम frame;
	}

	std::सदिशम्_2<uint8_t> buildRtcmFrame(std::mt19937 &rng, size_t payload_len)
	{
		std::सदिशम्_2<uint8_t> frame;
		frame.push_back(RTCM3_PREAMBLE);
		frame.push_back(static_cast<uint8_t>((payload_len >> 8) & 0x03));
		frame.push_back(static_cast<uint8_t>(payload_len & 0xFF));

		std::एकरूपम्_int_distribution<पूर्णाङ्क> byte(0, 255);

		प्रति (size_t i = 0; i < payload_len; i++) {
			frame.push_back(static_cast<uint8_t>(byte(rng)));
		}

		ध्रुव uint32_t crc = rtcm3_crc24q(frame.दत्त(), frame.आकार());
		frame.push_back((crc >> 16) & 0xFF);
		frame.push_back((crc >> 8) & 0xFF);
		frame.push_back(crc & 0xFF);
		प्रत्यागम frame;
	}

#यदि defined(CONFIG_GPS_SPARTN)
	std::सदिशम्_2<uint8_t> buildSpartnFrame(uint8_t सन्देशः_type_2, ध्रुव std::सदिशम्_2<uint8_t> &payload,
					      uint8_t crc_type = 0)
	{
		ध्रुव size_t payload_len = payload.आकार();
		std::सदिशम्_2<uint8_t> frame;
		frame.push_back(SPARTN_PREAMBLE);

		uint8_t b1 = static_cast<uint8_t>(((सन्देशः_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((crc_type & 0x03) << 4));
		frame.push_back(b1);
		frame.push_back(b2);
		frame.push_back(b3);

		// 4-byte description, 16-bit time tag
		frame.push_back(0x00);
		frame.push_back(0x00);
		frame.push_back(0x00);
		frame.push_back(0x00);

		frame.निवेशय(frame.अन्तः_2(), payload.begin(), payload.अन्तः_2());

		ध्रुव uint32_t crc = spartn_सन्देशः_crc(&frame[1], frame.आकार() - 1, crc_type);
		ध्रुव size_t crc_अष्टकानि = static_cast<size_t>(crc_type) + 1;

		प्रति (पूर्णाङ्क i = static_cast<पूर्णाङ्क>(crc_अष्टकानि) - 1; i >= 0; i--) {
			frame.push_back(static_cast<uint8_t>((crc >>(8 * i)) & 0xFF));
		}

		प्रत्यागम frame;
	}
#यदि_अन्तः
};

// =============================================================================
// Checksum
// =============================================================================

// Empty ACK-ACK (class=0x05, id=0x01, length=0): covered bytes 05 01 00 00.
// 8-bit Fletcher: a runs 5,6,6,6 and b runs 5,11,17,23 → CK_A=0x06, CK_B=0x17.
TEST_F(UbxTest, Checksum_ज्ञातम्_सदिशम्)
{
	ध्रुव uint8_t covered[] = {0x05, 0x01, 0x00, 0x00};
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(covered, परिमाण(covered), &ck_a, &ck_b);
	EXPECT_EQ(ck_a, 0x06u);
	EXPECT_EQ(ck_b, 0x17u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(UbxTest, Single_अल्पिष्ठम्_Frame)
{
	// UBX-MGA class is 0x13; any class/id is fine for framing
	स्वचालित frame = buildUbxFrame(0x13, 0x00, {0xAA, 0xBB});
	parser.सङ्कलय_दत्त(frame.दत्त(), frame.आकार());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ध्रुव uint8_t *सन्देशः_2 = parser.get_अग्र_सन्देशः(&len, &protocol);
	ASSERT_NE(सन्देशः_2, शून्यसूचकः);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, frame.आकार());
	EXPECT_EQ(0, memcmp(सन्देशः_2, frame.दत्त(), frame.आकार()));
	parser.consume_सन्देशः(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().crc_errors, 0u);
}

TEST_F(UbxTest, रिक्तानि_Payload_Frame)
{
	स्वचालित frame = buildUbxFrame(0x05, 0x01, {});
	parser.सङ्कलय_दत्त(frame.दत्त(), frame.आकार());

	size_t len = 0;
	ASSERT_NE(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
	EXPECT_EQ(len, UBX_शीर्षकम्_LEN + UBX_CK_LEN);
	parser.consume_सन्देशः(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, गुणजः_चौकटाः)
{
	स्वचालित f1 = buildUbxFrame(0x13, 0x00, {0x01});
	स्वचालित f2 = buildUbxFrame(0x13, 0x02, {0x02, 0x03, 0x04});
	std::सदिशम्_2<uint8_t> प्रवाहः;
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), f1.begin(), f1.अन्तः_2());
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), f2.begin(), f2.अन्तः_2());
	parser.सङ्कलय_दत्त(प्रवाहः.दत्त(), प्रवाहः.आकार());

	पूर्णाङ्क गणनम् = 0;
	size_t len = 0;

	यावत् (parser.get_अग्र_सन्देशः(&len) != शून्यसूचकः) {
		parser.consume_सन्देशः(len);
		गणनम्++;
	}

	EXPECT_EQ(गणनम्, 2);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
}

TEST_F(UbxTest, Chunked_आगमनम्)
{
	स्वचालित frame = buildUbxFrame(0x13, 0x01, {0x10, 0x20, 0x30, 0x40});

	प्रति (size_t i = 0; i < frame.आकार(); i++) {
		parser.सङ्कलय_दत्त(&frame[i], 1);
		// No complete frame until the last byte
	}

	size_t len = 0;
	ASSERT_NE(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
	EXPECT_EQ(len, frame.आकार());
}

TEST_F(UbxTest, NoiseBeforeFrame)
{
	स्वचालित frame = buildUbxFrame(0x13, 0x00, {0x55});
	std::सदिशम्_2<uint8_t> प्रवाहः = {0x00, 0xFF, 0x01};
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), frame.begin(), frame.अन्तः_2());
	parser.सङ्कलय_दत्त(प्रवाहः.दत्त(), प्रवाहः.आकार());

	size_t len = 0;
	ASSERT_NE(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
	EXPECT_GT(parser.getStats().अष्टकानि_discarded_2, 0u);
	parser.consume_सन्देशः(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
}

TEST_F(UbxTest, BadSync2)
{
	// 0xB5 followed by something other than 0x62 is not a frame start
	std::सदिशम्_2<uint8_t> प्रवाहः = {UBX_SYNC1, 0x00, 0x11, 0x22};
	स्वचालित frame = buildUbxFrame(0x13, 0x00, {0x01});
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), frame.begin(), frame.अन्तः_2());
	parser.सङ्कलय_दत्त(प्रवाहः.दत्त(), प्रवाहः.आकार());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_अग्र_सन्देशः(&len, &protocol), शून्यसूचकः);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_सन्देशः(len);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_GT(parser.getStats().अष्टकानि_discarded_2, 0u);
}

TEST_F(UbxTest, BadChecksum)
{
	स्वचालित frame = buildUbxFrame(0x13, 0x00, {0x11, 0x22});
	frame.पृष्ठभागः() ^= 0xFF;
	parser.सङ्कलय_दत्त(frame.दत्त(), frame.आकार());

	size_t len = 0;
	EXPECT_EQ(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
	EXPECT_GE(parser.getStats().crc_errors, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 0u);
}

TEST_F(UbxTest, RejectsOversizedPayload)
{
	// Length field claims UBX_MAX_PAYLOAD_LEN + 1 — must not pin the buffer
	std::सदिशम्_2<uint8_t> शीर्षकम् = {
		UBX_SYNC1, UBX_SYNC2,
		0x13, 0x00,
		static_cast<uint8_t>((UBX_MAX_PAYLOAD_LEN + 1) & 0xFF),
		static_cast<uint8_t>(((UBX_MAX_PAYLOAD_LEN + 1) >> 8) & 0xFF),
	};
	स्वचालित good = buildUbxFrame(0x13, 0x00, {0xAB});
	शीर्षकम्.निवेशय(शीर्षकम्.अन्तः_2(), good.begin(), good.अन्तः_2());
	parser.सङ्कलय_दत्त(शीर्षकम्.दत्त(), शीर्षकम्.आकार());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ASSERT_NE(parser.get_अग्र_सन्देशः(&len, &protocol), शून्यसूचकः);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	EXPECT_EQ(len, good.आकार());
	parser.consume_सन्देशः(len);
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
	स्वचालित mga1 = buildUbxFrame(0x13, 0x00, {0x01, 0x02, 0x03});
	स्वचालित mga2 = buildUbxFrame(0x13, 0x02, {0x10, 0x20});
	स्वचालित rtcm = buildRtcmFrame(rng, 40);

	std::सदिशम्_2<uint8_t> प्रवाहः;
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), mga1.begin(), mga1.अन्तः_2());
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), mga2.begin(), mga2.अन्तः_2());
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), rtcm.begin(), rtcm.अन्तः_2());
	parser.सङ्कलय_दत्त(प्रवाहः.दत्त(), प्रवाहः.आकार());

	std::सदिशम्_2<CorrectionProtocol> आलोकितम्;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	यावत् (parser.get_अग्र_सन्देशः(&len, &protocol) != शून्यसूचकः) {
		आलोकितम्.push_back(protocol);
		parser.consume_सन्देशः(len);
	}

	ASSERT_EQ(आलोकितम्.आकार(), 3u);
	EXPECT_EQ(आलोकितम्[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(आलोकितम्[1], CorrectionProtocol::Ubx);
	EXPECT_EQ(आलोकितम्[2], CorrectionProtocol::Rtcm3);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().crc_errors, 0u);
}

#यदि defined(CONFIG_GPS_SPARTN)
TEST_F(UbxTest, MgaThenSpartn)
{
	स्वचालित mga = buildUbxFrame(0x13, 0x00, {0xDE, 0xAD});
	स्वचालित spartn = buildSpartnFrame(0, {0xAA, 0xBB});

	std::सदिशम्_2<uint8_t> प्रवाहः;
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), mga.begin(), mga.अन्तः_2());
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), spartn.begin(), spartn.अन्तः_2());
	// Another MGA mid-stream should also work (reconnect-style)
	स्वचालित mga2 = buildUbxFrame(0x13, 0x01, {0x01});
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), mga2.begin(), mga2.अन्तः_2());
	parser.सङ्कलय_दत्त(प्रवाहः.दत्त(), प्रवाहः.आकार());

	std::सदिशम्_2<CorrectionProtocol> आलोकितम्;
	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;

	यावत् (parser.get_अग्र_सन्देशः(&len, &protocol) != शून्यसूचकः) {
		आलोकितम्.push_back(protocol);
		parser.consume_सन्देशः(len);
	}

	ASSERT_EQ(आलोकितम्.आकार(), 3u);
	EXPECT_EQ(आलोकितम्[0], CorrectionProtocol::Ubx);
	EXPECT_EQ(आलोकितम्[1], CorrectionProtocol::Spartn);
	EXPECT_EQ(आलोकितम्[2], CorrectionProtocol::Ubx);
	EXPECT_EQ(parser.getStats().ubx_messages, 2u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
}
#यदि_अन्तः

// A complete RTCM3 frame consumes its payload, so a 0xB5 that happens to sit
// inside that payload is never considered as a UBX start.
TEST_F(UbxTest, RtcmPayloadStrayB5IsNotFramedAsUbx)
{
	// Craft an RTCM3 frame whose payload contains a UBX-looking prefix.
	// buildRtcmFrame randomizes payload; force a 0xB5 0x62 sequence in raw form.
	std::सदिशम्_2<uint8_t> payload(32, 0x00);
	payload[4] = UBX_SYNC1;
	payload[5] = UBX_SYNC2;
	payload[6] = 0x13;
	payload[7] = 0x00;
	payload[8] = 0x02; // fake length
	payload[9] = 0x00;

	std::सदिशम्_2<uint8_t> frame;
	frame.push_back(RTCM3_PREAMBLE);
	frame.push_back(0x00);
	frame.push_back(static_cast<uint8_t>(payload.आकार()));
	frame.निवेशय(frame.अन्तः_2(), payload.begin(), payload.अन्तः_2());
	ध्रुव uint32_t crc = rtcm3_crc24q(frame.दत्त(), frame.आकार());
	frame.push_back((crc >> 16) & 0xFF);
	frame.push_back((crc >> 8) & 0xFF);
	frame.push_back(crc & 0xFF);

	// Follow with a real UBX so we know the framer still works after
	स्वचालित ubx = buildUbxFrame(0x13, 0x00, {0x99});
	std::सदिशम्_2<uint8_t> प्रवाहः = frame;
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), ubx.begin(), ubx.अन्तः_2());
	parser.सङ्कलय_दत्त(प्रवाहः.दत्त(), प्रवाहः.आकार());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Ubx;
	ASSERT_NE(parser.get_अग्र_सन्देशः(&len, &protocol), शून्यसूचकः);
	EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
	parser.consume_सन्देशः(len);

	ASSERT_NE(parser.get_अग्र_सन्देशः(&len, &protocol), शून्यसूचकः);
	EXPECT_EQ(protocol, CorrectionProtocol::Ubx);
	parser.consume_सन्देशः(len);

	EXPECT_EQ(parser.getStats().rtcm3_messages, 1u);
	EXPECT_EQ(parser.getStats().ubx_messages, 1u);
	EXPECT_EQ(parser.getStats().crc_errors, 0u);
}
