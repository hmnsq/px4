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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ "correction_framer.h"

#అన్తర్భావ <vector>
#అన్తర్భావ <cstring>
#అన్తర్భావ <random>

ఉపయోగించి నామాకాశ gnss;

తరగతి SpartnTest : బహిరంగ ::testing::Test
{
రక్షిత:
	CorrectionFramer parser;

	// Build unencrypted frame with 16-bit time tag and given CRC type.
	std::సదిశ_2<uint8_t> build_చట్రం(uint8_t సందేశం_type_2, uint8_t సందేశం_subtype,
					స్థిరరాశి std::సదిశ_2<uint8_t> &payload,
					uint8_t చక్రీయ_తనిఖీ_type = 0)
	{
		స్థిరరాశి size_t payload_len = payload.కొలత();
		std::సదిశ_2<uint8_t> చట్రం;
		చట్రం.push_back(SPARTN_PREAMBLE);

		// TF002-TF006 (frame CRC nibble left 0; not checked by the framer)
		uint8_t b1 = static_cast<uint8_t>(((సందేశం_type_2 & 0x7F) << 1) | ((payload_len >> 9) & 0x01));
		uint8_t b2 = static_cast<uint8_t>((payload_len >> 1) & 0xFF);
		uint8_t b3 = static_cast<uint8_t>(((payload_len & 0x01) << 7) | ((చక్రీయ_తనిఖీ_type & 0x03) << 4));
		చట్రం.push_back(b1);
		చట్రం.push_back(b2);
		చట్రం.push_back(b3);

		// 4-byte description, 16-bit time tag
		చట్రం.push_back(static_cast<uint8_t>((సందేశం_subtype & 0x0F) << 4));
		చట్రం.push_back(0x00);
		చట్రం.push_back(0x00);
		చట్రం.push_back(0x00);

		చట్రం.చొప్పించు(చట్రం.చివర(), payload.begin(), payload.చివర());

		// Message CRC over TF002..end of payload
		స్థిరరాశి uint32_t చక్రీయ_తనిఖీ = spartn_సందేశం_చక్రీయ_తనిఖీ(&చట్రం[1], చట్రం.కొలత() - 1, చక్రీయ_తనిఖీ_type);
		స్థిరరాశి size_t చక్రీయ_తనిఖీ_బైట్లు = static_cast<size_t>(చక్రీయ_తనిఖీ_type) + 1;

		కొరకు (పూర్ణాంకం i = static_cast<పూర్ణాంకం>(చక్రీయ_తనిఖీ_బైట్లు) - 1; i >= 0; i--) {
			చట్రం.push_back(static_cast<uint8_t>((చక్రీయ_తనిఖీ >>(8 * i)) & 0xFF));
		}

		తిరిగి చట్రం;
	}

	// Build a well-formed RTCM3 frame with a deterministic pseudo-random payload.
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
};

// =============================================================================
// Message CRC (TF018)
// =============================================================================

// Vectors taken from the pyspartn reference implementation's test suite
// (semuconsulting/pyspartn, tests/test_static.py), which computes the same
// TF018 CRCs independently of this code.
TEST_F(SpartnTest, సందేశం_చక్రీయ_తనిఖీ_Matches_Reference_Vectors)
{
	స్థిరరాశి uint8_t సందేశం_2[] = {'H', 'i', '!'};

	EXPECT_EQ(spartn_సందేశం_చక్రీయ_తనిఖీ(సందేశం_2, పరిమాణం(సందేశం_2), 0), 0x78u);
	EXPECT_EQ(spartn_సందేశం_చక్రీయ_తనిఖీ(సందేశం_2, పరిమాణం(సందేశం_2), 1), 0x31FDu);
	EXPECT_EQ(spartn_సందేశం_చక్రీయ_తనిఖీ(సందేశం_2, పరిమాణం(సందేశం_2), 2), 0x33220Fu);
	EXPECT_EQ(spartn_సందేశం_చక్రీయ_తనిఖీ(సందేశం_2, పరిమాణం(సందేశం_2), 3), 0x9523B4B4u);
}

// =============================================================================
// Framing
// =============================================================================

TEST_F(SpartnTest, Single_కనీస_చట్రం)
{
	స్వయంచాలక చట్రం = build_చట్రం(0, 0, {0xAA, 0xBB});
	parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

	size_t len = 0;
	CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
	స్థిరరాశి uint8_t *సందేశం_2 = parser.get_తదుపరి_సందేశం(&len, &protocol);
	ASSERT_NE(సందేశం_2, శూన్యసూచిక);
	EXPECT_EQ(protocol, CorrectionProtocol::Spartn);
	EXPECT_EQ(len, చట్రం.కొలత());
	EXPECT_EQ(0, memcmp(సందేశం_2, చట్రం.దత్తాంశం(), చట్రం.కొలత()));
	parser.consume_సందేశం(len);
	EXPECT_EQ(parser.getStats().messages_parsed, 1u);
	EXPECT_EQ(parser.getStats().spartn_messages, 1u);
	EXPECT_EQ(parser.getStats().చక్రీయ_తనిఖీ_errors_2, 0u);
}

TEST_F(SpartnTest, గుణిజం_చట్రాలు)
{
	స్వయంచాలక f1 = build_చట్రం(0, 0, {0x01});
	స్వయంచాలక f2 = build_చట్రం(1, 0, {0x02, 0x03});
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
	EXPECT_EQ(parser.getStats().spartn_messages, 2u);
}

TEST_F(SpartnTest, Chunked_రాక)
{
	స్వయంచాలక చట్రం = build_చట్రం(0, 1, {0x10, 0x20, 0x30});

	కొరకు (size_t i = 0; i < చట్రం.కొలత(); i++) {
		parser.కూడు_దత్తాంశం(&చట్రం[i], 1);
	}

	size_t len = 0;
	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
	EXPECT_EQ(len, చట్రం.కొలత());
}

TEST_F(SpartnTest, Noise_Before_చట్రం)
{
	స్వయంచాలక చట్రం = build_చట్రం(0, 0, {0x55});
	std::సదిశ_2<uint8_t> ప్రవాహం = {0x00, 0xFF, 0x01};
	ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), చట్రం.begin(), చట్రం.చివర());
	parser.కూడు_దత్తాంశం(ప్రవాహం.దత్తాంశం(), ప్రవాహం.కొలత());

	size_t len = 0;
	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
	EXPECT_GT(parser.getStats().బైట్లు_discarded_2, 0u);
}

TEST_F(SpartnTest, Bad_సందేశం_చక్రీయ_తనిఖీ)
{
	స్వయంచాలక చట్రం = build_చట్రం(0, 0, {0x11, 0x22});
	చట్రం.వెనుక() ^= 0xFF;
	parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

	size_t len = 0;
	EXPECT_EQ(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
	EXPECT_GE(parser.getStats().చక్రీయ_తనిఖీ_errors_2, 1u);
}

TEST_F(SpartnTest, Crc16_చట్రం)
{
	స్వయంచాలక చట్రం = build_చట్రం(0, 0, {0xDE, 0xAD}, 1);
	parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

	size_t len = 0;
	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
	parser.consume_సందేశం(len);
	EXPECT_EQ(parser.getStats().చక్రీయ_తనిఖీ_errors_2, 0u);
}

// TF002 5-119 is reserved by SPARTN 2.x, so a frame claiming one cannot be real
// even if its message CRC checks out.
TEST_F(SpartnTest, Rejects_Reserved_సందేశం_Type)
{
	EXPECT_TRUE(spartn_సందేశం_type_defined(0));
	EXPECT_TRUE(spartn_సందేశం_type_defined(4));
	EXPECT_FALSE(spartn_సందేశం_type_defined(5));
	EXPECT_FALSE(spartn_సందేశం_type_defined(119));
	EXPECT_TRUE(spartn_సందేశం_type_defined(120));
	EXPECT_TRUE(spartn_సందేశం_type_defined(127));

	స్వయంచాలక చట్రం = build_చట్రం(50, 0, {0x11, 0x22});
	parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

	size_t len = 0;
	EXPECT_EQ(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
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
TEST_F(SpartnTest, Rtcm_ప్రవాహం_Is_Never_Framed_As_Spartn)
{
	std::mt19937 rng(1);
	std::ఏకరీతి_int_distribution<పూర్ణాంకం> payload_len(60, 500);

	size_t చట్రాలు_లోపల = 0;
	size_t చట్రాలు_బహిర్గతం = 0;
	size_t generated = 0;
	std::సదిశ_2<uint8_t> ప్రవాహం;

	// ~1 MB of RTCM3, about 17 min of a 1 kB/s RTK correction stream
	అయితే (generated < 1000000) {
		స్వయంచాలక చట్రం = build_Rtcm_చట్రం(rng, payload_len(rng));
		generated += చట్రం.కొలత();
		చట్రాలు_లోపల++;
		ప్రవాహం.చొప్పించు(ప్రవాహం.చివర(), చట్రం.begin(), చట్రం.చివర());

		// Feed in MAVLink GPS_RTCM_DATA sized chunks
		అయితే (ప్రవాహం.కొలత() >= 180) {
			parser.కూడు_దత్తాంశం(ప్రవాహం.దత్తాంశం(), 180);

			size_t len = 0;
			CorrectionProtocol protocol = CorrectionProtocol::Spartn;

			అయితే (parser.get_తదుపరి_సందేశం(&len, &protocol) != శూన్యసూచిక) {
				EXPECT_EQ(protocol, CorrectionProtocol::Rtcm3);
				parser.consume_సందేశం(len);
				చట్రాలు_బహిర్గతం++;
			}

			ప్రవాహం.erase(ప్రవాహం.begin(), ప్రవాహం.begin() + 180);
		}
	}

	// Every RTCM3 frame is recovered, and no part of the stream is framed as SPARTN
	EXPECT_EQ(parser.getStats().spartn_messages, 0u);
	EXPECT_EQ(parser.getStats().rtcm3_messages, చట్రాలు_బహిర్గతం);
	EXPECT_GE(చట్రాలు_బహిర్గతం, చట్రాలు_లోపల - 1);
	EXPECT_EQ(parser.getStats().చక్రీయ_తనిఖీ_errors_2, 0u);
}
