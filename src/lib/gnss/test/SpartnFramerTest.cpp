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

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव "correction_framer.h"

#अन्तर्भाव <vector>
#अन्तर्भाव <cstring>
#अन्तर्भाव <random>

उपयुज्य नामाकाश gnss;

वर्ग SpartnTest : सार्वजनिक ::testing::Test
{
रक्षित:
	CorrectionFramer parser;

	// Build unencrypted frame with 16-bit time tag and given CRC type.
	std::सदिशम्_2<uint8_t> buildFrame(uint8_t सन्देशः_type_2, uint8_t सन्देशः_subtype,
					ध्रुव std::सदिशम्_2<uint8_t> &payload,
					uint8_t crc_type = 0)
	{
		ध्रुव size_t payload_len = payload.आकार();
		std::सदिशम्_2<uint8_t> frame;
		frame.push_back(SPARTN_PREAMBLE);

		// TF002-TF006 (frame CRC nibble left 0; not checked by the framer)
		uint8_t b1 = static_cast<uint8_t>(((सन्देशः_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((crc_type & 0x03) << 4));
		frame.push_back(b1);
		frame.push_back(b2);
		frame.push_back(b3);

		// 4-byte description, 16-bit time tag
		frame.push_back(static_cast<uint8_t>((सन्देशः_subtype & 0x0F) << 4));
		frame.push_back(0x00);
		frame.push_back(0x00);
		frame.push_back(0x00);

		frame.निवेशय(frame.अन्तः_2(), payload.begin(), payload.अन्तः_2());

		// Message CRC over TF002..end of payload
		ध्रुव uint32_t crc = spartn_सन्देशः_crc(&frame[1], frame.आकार() - 1, crc_type);
		ध्रुव size_t crc_अष्टकानि = static_cast<size_t>(crc_type) + 1;

		प्रति (पूर्णाङ्क i = static_cast<पूर्णाङ्क>(crc_अष्टकानि) - 1; i >= 0; i--) {
			frame.push_back(static_cast<uint8_t>((crc >>(8 * i)) & 0xFF));
		}

		प्रत्यागम frame;
	}

	// Build a well-formed RTCM3 frame with a deterministic pseudo-random payload.
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
};

// =============================================================================
// Message CRC (TF018)
// =============================================================================

// Vectors taken from the pyspartn reference implementation's test suite
// (semuconsulting/pyspartn, tests/test_static.py), which computes the same
// TF018 CRCs independently of this code.
TEST_F(SpartnTest, सन्देशः_Crc_Matches_Reference_Vectors)
{
	ध्रुव uint8_t सन्देशः_2[] = {'H', 'i', '!'};

	EXPECT_EQ(spartn_सन्देशः_crc(सन्देशः_2, परिमाण(सन्देशः_2), 0), 0x78u);
	EXPECT_EQ(spartn_सन्देशः_crc(सन्देशः_2, परिमाण(सन्देशः_2), 1), 0x31FDu);
	EXPECT_EQ(spartn_सन्देशः_crc(सन्देशः_2, परिमाण(सन्देशः_2), 2), 0x33220Fu);
	EXPECT_EQ(spartn_सन्देशः_crc(सन्देशः_2, परिमाण(सन्देशः_2), 3), 0x9523B4B4u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(SpartnTest, Single_अल्पिष्ठम्_Frame)
{
	स्वचालित frame = buildFrame(0, 0, {0xAA, 0xBB});
	parser.सङ्कलय_दत्त(frame.दत्त(), frame.आकार());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	ध्रुव uint8_t *सन्देशः_2 = parser.get_अग्र_सन्देशः(&len, &protocol);
	ASSERT_NE(सन्देशः_2, शून्यसूचकः);
	EXPECT_EQ(protocol, CorrectionProtocol::Spartn);
	EXPECT_EQ(len, frame.आकार());
	EXPECT_EQ(0, memcmp(सन्देशः_2, frame.दत्त(), frame.आकार()));
	parser.consume_सन्देशः(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
	EXPECT_EQ(parser.getStats().crc_errors, 0u);
}

TEST_F(SpartnTest, गुणजः_चौकटाः)
{
	स्वचालित f1 = buildFrame(0, 0, {0x01});
	स्वचालित f2 = buildFrame(1, 0, {0x02, 0x03});
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
	EXPECT_EQ(parser.getStats().spartn_messages, 2u);
}

TEST_F(SpartnTest, Chunked_आगमनम्)
{
	स्वचालित frame = buildFrame(0, 1, {0x10, 0x20, 0x30});

	प्रति (size_t i = 0; i < frame.आकार(); i++) {
		parser.सङ्कलय_दत्त(&frame[i], 1);
	}

	size_t len = 0;
	ASSERT_NE(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
	EXPECT_EQ(len, frame.आकार());
}

TEST_F(SpartnTest, NoiseBeforeFrame)
{
	स्वचालित frame = buildFrame(0, 0, {0x55});
	std::सदिशम्_2<uint8_t> प्रवाहः = {0x00, 0xFF, 0x01};
	प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), frame.begin(), frame.अन्तः_2());
	parser.सङ्कलय_दत्त(प्रवाहः.दत्त(), प्रवाहः.आकार());

	size_t len = 0;
	ASSERT_NE(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
	EXPECT_GT(parser.getStats().अष्टकानि_discarded_2, 0u);
}

TEST_F(SpartnTest, Bad_सन्देशः_Crc)
{
	स्वचालित frame = buildFrame(0, 0, {0x11, 0x22});
	frame.पृष्ठभागः() ^= 0xFF;
	parser.सङ्कलय_दत्त(frame.दत्त(), frame.आकार());

	size_t len = 0;
	EXPECT_EQ(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
	EXPECT_GE(parser.getStats().crc_errors, 1u);
}

TEST_F(SpartnTest, Crc16Frame)
{
	स्वचालित frame = buildFrame(0, 0, {0xDE, 0xAD}, 1);
	parser.सङ्कलय_दत्त(frame.दत्त(), frame.आकार());

	size_t len = 0;
	ASSERT_NE(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
	parser.consume_सन्देशः(len);
	EXPECT_EQ(parser.getStats().crc_errors, 0u);
}

// TF002 5-119 is reserved by SPARTN 2.x, so a frame claiming one cannot be real
// even if its message CRC checks out.
TEST_F(SpartnTest, Rejects_Reserved_सन्देशः_Type)
{
	EXPECT_TRUE(spartn_सन्देशः_type_defined(0));
	EXPECT_TRUE(spartn_सन्देशः_type_defined(4));
	EXPECT_FALSE(spartn_सन्देशः_type_defined(5));
	EXPECT_FALSE(spartn_सन्देशः_type_defined(119));
	EXPECT_TRUE(spartn_सन्देशः_type_defined(120));
	EXPECT_TRUE(spartn_सन्देशः_type_defined(127));

	स्वचालित frame = buildFrame(50, 0, {0x11, 0x22});
	parser.सङ्कलय_दत्त(frame.दत्त(), frame.आकार());

	size_t len = 0;
	EXPECT_EQ(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
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
TEST_F(SpartnTest, Rtcm_प्रवाहः_Is_Never_Framed_As_Spartn)
{
	std::mt19937 rng(1);
	std::एकरूपम्_int_distribution<पूर्णाङ्क> payload_len(60, 500);

	size_t चौकटाः_अन्तर् = 0;
	size_t चौकटाः_बहिर्गमनम् = 0;
	size_t generated = 0;
	std::सदिशम्_2<uint8_t> प्रवाहः;

	// ~1 MB of RTCM3, about 17 min of a 1 kB/s RTK correction stream
	यावत् (generated < 1000000) {
		स्वचालित frame = buildRtcmFrame(rng, payload_len(rng));
		generated += frame.आकार();
		चौकटाः_अन्तर्++;
		प्रवाहः.निवेशय(प्रवाहः.अन्तः_2(), frame.begin(), frame.अन्तः_2());

		// Feed in MAVLink GPS_RTCM_DATA sized chunks
		यावत् (प्रवाहः.आकार() >= 180) {
			parser.सङ्कलय_दत्त(प्रवाहः.दत्त(), 180);

			size_t len = 0;
			CorrectionProtocol protocol = CorrectionProtocol::Spartn;

			यावत् (parser.get_अग्र_सन्देशः(&len, &protocol) != शून्यसूचकः) {
				EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
				parser.consume_सन्देशः(len);
				चौकटाः_बहिर्गमनम्++;
			}

			प्रवाहः.erase(प्रवाहः.begin(), प्रवाहः.begin() + 180);
		}
	}

	// Every RTCM3 frame is recovered, and no part of the stream is framed as SPARTN
	EXPECT_EQ(parser.getStats().spartn_messages, 0u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, चौकटाः_बहिर्गमनम्);
	EXPECT_GE(चौकटाः_बहिर्गमनम्, चौकटाः_अन्तर् - 1);
	EXPECT_EQ(parser.getStats().crc_errors, 0u);
}
