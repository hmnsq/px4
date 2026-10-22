/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

#अन्तर्भाव "RtcmTestCommon.hpp"

TEST_F(RtcmTest, BustedSender_AllZeros)
{
	std::सदिशम्_2<uint8_t> garbage(1000, 0x00);
	parser.सङ्कलय_दत्त(garbage.दत्त(), garbage.आकार());

	size_t len;
	EXPECT_EQ(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
	EXPECT_EQ(parser.getStats().अष्टकानि_discarded_2, 1000u);
	EXPECT_EQ(parser.buffered_अष्टकानि(), 0u);
}

TEST_F(RtcmTest, BustedSender_AllPreambles)
{
	// All preamble bytes (0xD3). The parser sees each 0xD3 as a potential frame start.
	// Length extracted from 0xD3,0xD3 is ((0x03)<<8)|0xD3 = 979 bytes.
	// Parser waits for full frame which never comes.
	std::सदिशम्_2<uint8_t> preambles(500, RTCM3_PREAMBLE);
	parser.सङ्कलय_दत्त(preambles.दत्त(), preambles.आकार());

	size_t len;
	// Parser should be stuck waiting for more data (incomplete frame)
	EXPECT_EQ(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
	// Buffer retains data waiting for frame completion
	EXPECT_GT(parser.buffered_अष्टकानि(), 0u);
}

TEST_F(RtcmTest, BustedSender_RandomNoise)
{
	std::mt19937 rng(12345);
	uint8_t noise[500];

	प्रति (size_t i = 0; i < परिमाण(noise); i++) {
		noise[i] = rng() & 0xFF;
	}

	parser.सङ्कलय_दत्त(noise, परिमाण(noise));

	size_t len;

	// Consume any messages that might accidentally form (unlikely but possible)
	यावत् (parser.get_अग्र_सन्देशः(&len) != शून्यसूचकः) {
		parser.consume_सन्देशः(len);
	}

	// Most data should be discarded
	स्वचालित stats = parser.getStats();
	EXPECT_GT(stats.अष्टकानि_discarded_2 + stats.crc_errors, 0u);
}

TEST_F(RtcmTest, Busted_Sender_Valid_शीर्षकम्_Bad_Crc)
{
	// Frame with valid header but bad CRC
	std::सदिशम्_2<uint8_t> frame = {
		RTCM3_PREAMBLE, 0x00, 0x04,  // Header: 4 byte payload
		0x01, 0x02, 0x03, 0x04,      // Payload
		0xDE, 0xAD, 0xBE             // Bad CRC
	};

	parser.सङ्कलय_दत्त(frame.दत्त(), frame.आकार());

	size_t len;
	EXPECT_EQ(parser.get_अग्र_सन्देशः(&len), शून्यसूचकः);
	EXPECT_EQ(parser.getStats().crc_errors, 1u);
}

TEST_F(RtcmTest, Busted_Sender_Repeated_Bad_Crc_चौकटाः)
{
	प्रति (पूर्णाङ्क i = 0; i < 100; i++) {
		स्वचालित bad = buildBadCrcFrame({static_cast<uint8_t>(i), 0x00});
		parser.सङ्कलय_दत्त(bad.दत्त(), bad.आकार());
	}

	// Re-scanning after a failure can land on a byte that claims a frame longer
	// than what is buffered, which parks the framer until the rest arrives (see
	// BustedSender_AllPreambles). Feed a frame's worth of non-preamble filler so
	// every pending candidate can be resolved, as a live stream would.
	ध्रुव std::सदिशम्_2<uint8_t> filler(CorrectionFramer::MAX_FRAME_LEN, 0x00);
	parser.सङ्कलय_दत्त(filler.दत्त(), filler.आकार());

	size_t len;

	यावत् (parser.get_अग्र_सन्देशः(&len) != शून्यसूचकः) {
		parser.consume_सन्देशः(len);
	}

	स्वचालित stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 0u);
	// At least 100 CRC errors (may be more due to re-scanning after failures)
	EXPECT_GE(stats.crc_errors, 100u);
}
