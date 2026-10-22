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

/**
 * @file RtcmParserTest.cpp
 *
 * Tests for RTCM3 parser with valid and partially valid input.
 * Consolidates: GoodSender, PartialSender, Helper, Buffer, Stats, EdgeCase tests.
 */

#అన్తర్భావ "RtcmTestCommon.hpp"

// =============================================================================
// Helper function tests
// =============================================================================

TEST_F(RtcmTest, Payload_నిడివి_Masks_Reserved_బిట్లు)
{
	// Reserved bits set to 1, length should still be extracted correctly
	uint8_t చట్రం[] = {RTCM3_PREAMBLE, 0xFF, 0xFF};  // Reserved=0x3F, Length=1023
	EXPECT_EQ(rtcm3_payload_నిడివి(చట్రం), 1023u);
}

TEST_F(RtcmTest, సందేశం_Type_Extraction)
{
	స్వయంచాలక చట్రం = build_Valid_చట్రం(1005, {});
	EXPECT_EQ(rtcm3_సందేశం_type(చట్రం.దత్తాంశం()), 1005u);
}

// =============================================================================
// Good sender tests - valid frames, happy path
// =============================================================================

TEST_F(RtcmTest, Single_కనీస_చట్రం)
{
	స్వయంచాలక చట్రం = build_Raw_చట్రం({0x00, 0x00});
	parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

	size_t len = 0;
	స్థిరరాశి uint8_t *సందేశం_2 = parser.get_తదుపరి_సందేశం(&len);

	ASSERT_NE(సందేశం_2, శూన్యసూచిక);
	EXPECT_EQ(len, చట్రం.కొలత());

	parser.consume_సందేశం(len);
	స్వయంచాలక stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 1u);
	EXPECT_EQ(stats.చక్రీయ_తనిఖీ_errors_2, 0u);
}

TEST_F(RtcmTest, Single_Maximal_చట్రం)
{
	std::సదిశ_2<uint8_t> payload(RTCM3_MAX_PAYLOAD_LEN, 0xAA);
	స్వయంచాలక చట్రం = build_Raw_చట్రం(payload);
	parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

	size_t len = 0;
	స్థిరరాశి uint8_t *సందేశం_2 = parser.get_తదుపరి_సందేశం(&len);

	ASSERT_NE(సందేశం_2, శూన్యసూచిక);
	EXPECT_EQ(len, RTCM3_MAX_చట్రం_LEN);
}

TEST_F(RtcmTest, గుణిజం_చట్రాలు_లోపల_Sequence)
{
	స్వయంచాలక frame1 = build_Valid_చట్రం(1005, {0x01, 0x02, 0x03});
	స్వయంచాలక frame2 = build_Valid_చట్రం(1077, {0x04, 0x05, 0x06, 0x07});
	స్వయంచాలక frame3 = build_Valid_చట్రం(1087, {0x08});

	parser.కూడు_దత్తాంశం(frame1.దత్తాంశం(), frame1.కొలత());
	parser.కూడు_దత్తాంశం(frame2.దత్తాంశం(), frame2.కొలత());
	parser.కూడు_దత్తాంశం(frame3.దత్తాంశం(), frame3.కొలత());

	size_t len;
	పూర్ణాంకం లెక్క = 0;

	అయితే (parser.get_తదుపరి_సందేశం(&len) != శూన్యసూచిక) {
		parser.consume_సందేశం(len);
		లెక్క++;
	}

	EXPECT_EQ(లెక్క, 3);
}

TEST_F(RtcmTest, చట్రం_Arrives_లోపల_Chunks)
{
	స్వయంచాలక చట్రం = build_Valid_చట్రం(1005, {0x01, 0x02, 0x03, 0x04, 0x05});

	// Feed one byte at a time
	కొరకు (size_t i = 0; i < చట్రం.కొలత() - 1; i++) {
		parser.కూడు_దత్తాంశం(&చట్రం[i], 1);
		size_t len;
		EXPECT_EQ(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
	}

	parser.కూడు_దత్తాంశం(&చట్రం[చట్రం.కొలత() - 1], 1);

	size_t len;
	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
	EXPECT_EQ(len, చట్రం.కొలత());
}

// =============================================================================
// Partially good sender tests - mix of valid and invalid
// =============================================================================

TEST_F(RtcmTest, Garbage_Before_Valid_చట్రం)
{
	std::సదిశ_2<uint8_t> దత్తాంశం;

	// 50 bytes of garbage
	కొరకు (పూర్ణాంకం i = 0; i < 50; i++) {
		దత్తాంశం.push_back(0x00 + i);
	}

	స్వయంచాలక చట్రం = build_Valid_చట్రం(1005, {0x01, 0x02});
	దత్తాంశం.చొప్పించు(దత్తాంశం.చివర(), చట్రం.begin(), చట్రం.చివర());

	parser.కూడు_దత్తాంశం(దత్తాంశం.దత్తాంశం(), దత్తాంశం.కొలత());

	size_t len;
	స్థిరరాశి uint8_t *సందేశం_2 = parser.get_తదుపరి_సందేశం(&len);
	ASSERT_NE(సందేశం_2, శూన్యసూచిక);
	EXPECT_EQ(rtcm3_సందేశం_type(సందేశం_2), 1005u);

	parser.consume_సందేశం(len);
	EXPECT_EQ(parser.getStats().బైట్లు_discarded_2, 50u);
}

TEST_F(RtcmTest, Valid_చట్రం_Bad_చట్రం_Valid)
{
	స్వయంచాలక frame1 = build_Valid_చట్రం(1005, {0x01});
	స్వయంచాలక bad_చట్రం = build_Bad_చక్రీయ_తనిఖీ_చట్రం({0x02, 0x03});
	స్వయంచాలక frame2 = build_Valid_చట్రం(1077, {0x04});

	parser.కూడు_దత్తాంశం(frame1.దత్తాంశం(), frame1.కొలత());
	parser.కూడు_దత్తాంశం(bad_చట్రం.దత్తాంశం(), bad_చట్రం.కొలత());
	parser.కూడు_దత్తాంశం(frame2.దత్తాంశం(), frame2.కొలత());

	size_t len;
	పూర్ణాంకం valid_లెక్క = 0;

	అయితే (parser.get_తదుపరి_సందేశం(&len) != శూన్యసూచిక) {
		parser.consume_సందేశం(len);
		valid_లెక్క++;
	}

	EXPECT_EQ(valid_లెక్క, 2);
	EXPECT_EQ(parser.getStats().చక్రీయ_తనిఖీ_errors_2, 1u);
}

// =============================================================================
// Buffer and edge case tests
// =============================================================================

TEST_F(RtcmTest, బఫరు_అధికప్రవాహం_Protection)
{
	std::సదిశ_2<uint8_t> large_దత్తాంశం(CorrectionFramer::బఫరు_కొలత_2 + 1000, 0xAA);
	size_t added = parser.కూడు_దత్తాంశం(large_దత్తాంశం.దత్తాంశం(), large_దత్తాంశం.కొలత());

	EXPECT_EQ(added, CorrectionFramer::బఫరు_కొలత_2);
}

TEST_F(RtcmTest, ఖాళీలు_Payload_చట్రం)
{
	స్వయంచాలక చట్రం = build_Raw_చట్రం({});
	parser.కూడు_దత్తాంశం(చట్రం.దత్తాంశం(), చట్రం.కొలత());

	size_t len;
	ASSERT_NE(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
	EXPECT_EQ(len, RTCM3_శీర్షిక_LEN + RTCM3_చక్రీయ_తనిఖీ_LEN);
}

TEST_F(RtcmTest, Get_తదుపరి_On_ఖాళీలు_బఫరు)
{
	size_t len;
	EXPECT_EQ(parser.get_తదుపరి_సందేశం(&len), శూన్యసూచిక);
}

// =============================================================================
// GPS_RTCM_DATA fragmentation tests
// =============================================================================

// WHAT: Verify that a normal unfragmented payload is passed through unchanged.
// WHY: The assembler must ignore sequence bits when the fragmented flag is not set.
TEST_F(RtcmTest, GpsRtcmAssemblerReturnsUnfragmentedPayload)
{
	// GIVEN: A small payload that fits in one packet.
	స్థిరరాశి std::సదిశ_2<uint8_t> payload {0x01, 0x02, 0x03, 0x04};
	size_t len = 0;

	// WHEN: The payload is added without the fragmented flag.
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(అబద్ధం, 0, 9), payload.దత్తాంశం(), payload.కొలత(), 10,
				 len);

	// THEN: The assembler returns the full payload unchanged.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, payload.దత్తాంశం(), payload.కొలత()), 0);
}

// WHAT: Verify that the unfragmented path accepts the largest payload the helper stores.
// WHY: This keeps the direct helper's explicit size guard covered even though
// MAVLink GPS_RTCM_DATA packets top out at 180 payload bytes.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_స్వీకరించు_Max_కొలత_Unfragmented_Payload)
{
	// GIVEN: A payload that is exactly as large as the assembler's internal
	// message buffer. This exceeds what MAVLink can carry in one packet, but the
	// direct helper still needs a checked upper bound.
	std::సదిశ_2<uint8_t> payload(GPS_RTCM_MAX_సందేశం_LEN);
	std::iota(payload.begin(), payload.చివర(), 0);
	size_t len = 0;

	// WHEN: The payload is added as a single unfragmented packet.
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(అబద్ధం), payload.దత్తాంశం(), payload.కొలత(), 10, len);

	// THEN: The assembler accepts it and returns the original bytes unchanged.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, payload.దత్తాంశం(), payload.కొలత()), 0);
}

// WHAT: Verify that oversized fragmented packets are rejected.
// WHY: GPS_RTCM_DATA only allows up to 180 bytes per fragment, so larger packets must be dropped.
TEST_F(RtcmTest, GpsRtcmAssemblerRejectsOversizedFragment)
{
	// GIVEN: A fragmented packet that exceeds the protocol fragment size limit.
	std::సదిశ_2<uint8_t> oversized_fragment(GPS_RTCM_MAX_FRAGMENT_LEN + 1, 0x5a);
	size_t len = 123;

	// WHEN: The oversized fragment is passed to the assembler.
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 0, 6), oversized_fragment.దత్తాంశం(),
				 oversized_fragment.కొలత(), 10, len);

	// THEN: The fragment is rejected and no payload is returned.
	EXPECT_EQ(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, 0u);
}

// WHAT: Verify that fragments can arrive out of order and still be reassembled correctly.
// WHY: The sequence/fragment handling exists to allow for unreliable delivery order.
TEST_F(RtcmTest, GpsRtcmAssemblerReordersFragments)
{
	// GIVEN: A payload that spans three fragments with a non-full final fragment.
	std::సదిశ_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN * 2 + 25);
	std::iota(payload.begin(), payload.చివర(), 0);
	స్థిరరాశి uint8_t flags0 = buildGpsRtcmFlags(నిజం, 0, 7);
	స్థిరరాశి uint8_t flags1 = buildGpsRtcmFlags(నిజం, 1, 7);
	స్థిరరాశి uint8_t flags2 = buildGpsRtcmFlags(నిజం, 2, 7);

	size_t len = 0;

	// WHEN: The later fragments arrive before fragment 0.
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(flags1, &payload[GPS_RTCM_MAX_FRAGMENT_LEN], GPS_RTCM_MAX_FRAGMENT_LEN, 10, len),
		  శూన్యసూచిక);
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(flags2, &payload[GPS_RTCM_MAX_FRAGMENT_LEN * 2], 25, 20, len), శూన్యసూచిక);

	// WHEN: Fragment 0 finally arrives.
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(flags0, payload.దత్తాంశం(), GPS_RTCM_MAX_FRAGMENT_LEN, 30, len);

	// THEN: The assembler emits the full payload in the correct order.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, payload.దత్తాంశం(), payload.కొలత()), 0);
}

// WHAT: Verify that a duplicate fragment with identical content is handled.
// WHY: Retransmissions should not reset a healthy buffer or corrupt the assembled payload.
TEST_F(RtcmTest, GpsRtcmAssemblerIgnoresDuplicateMatchingFragment)
{
	// GIVEN: A payload that spans one full fragment and one non-full fragment.
	std::సదిశ_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN + 12);
	std::iota(payload.begin(), payload.చివర(), 0);
	స్థిరరాశి uint8_t flags0 = buildGpsRtcmFlags(నిజం, 0, 5);
	స్థిరరాశి uint8_t flags1 = buildGpsRtcmFlags(నిజం, 1, 5);
	size_t len = 0;

	// WHEN: Fragment 0 is received twice with the same content.
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(flags0, payload.దత్తాంశం(), GPS_RTCM_MAX_FRAGMENT_LEN, 10, len), శూన్యసూచిక);
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(flags0, payload.దత్తాంశం(), GPS_RTCM_MAX_FRAGMENT_LEN, 11, len), శూన్యసూచిక);

	// WHEN: The non-full final fragment arrives.
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(flags1, &payload[GPS_RTCM_MAX_FRAGMENT_LEN], 12, 20, len);

	// THEN: The assembler still returns the expected full payload once.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, payload.దత్తాంశం(), payload.కొలత()), 0);
}

నిర్మాణం GpsRtcmSequenceChangeCase {
	స్థిరరాశి అక్షరం *name;
	std::సదిశ_2<uint8_t> old_fragment_ids;
	size_t expected_flushed_fragments;
};

తరగతి GpsRtcmSequenceChangeTest : బహిరంగ RtcmTest, బహిరంగ ::testing::WithParamInterface<GpsRtcmSequenceChangeCase>
{
};

// WHAT: Verify that legacy exact-multiple fragmented messages are sent
// when every buffered fragment is a full 180-byte prefix starting at fragment 0.
// WHY: This keeps the receiver compatible with older QGC senders while still
// rejecting partial buffers that have a missing fragment in the middle.
TEST_P(GpsRtcmSequenceChangeTest, Gps_Rtcm_Assembler_Flushes_Only_Gap_Free_నిండిన_ఉపసర్గ_On_Sequence_Change)
{
	స్థిరరాశి GpsRtcmSequenceChangeCase &param = GetParam();
	స్థిరరాశి uint8_t old_sequence_id = 9;
	స్థిరరాశి uint8_t new_sequence_id = 10;
	size_t len = 0;
	uint64_t timestamp = 10;
	std::సదిశ_2<std::సదిశ_2<uint8_t>> old_fragments(GPS_RTCM_MAX_FRAGMENTS);

	// GIVEN: A buffered old sequence with full-sized fragments as described by
	// the parameter set, followed by a new sequence that starts with fragment 0.
	కొరకు (స్థిరరాశి uint8_t fragment_id : param.old_fragment_ids) {
		old_fragments[fragment_id].resize(GPS_RTCM_MAX_FRAGMENT_LEN);
		std::iota(old_fragments[fragment_id].begin(), old_fragments[fragment_id].చివర(),
			  static_cast<uint8_t>(0x20 + fragment_id * 0x10));
		EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, fragment_id, old_sequence_id),
						       old_fragments[fragment_id].దత్తాంశం(), old_fragments[fragment_id].కొలత(), timestamp, len),
			  శూన్యసూచిక);
		EXPECT_EQ(len, 0u);
		timestamp += 10;
	}

	std::సదిశ_2<uint8_t> expected_old_సందేశం;

	కొరకు (size_t fragment_id = 0; fragment_id < param.expected_flushed_fragments; fragment_id++) {
		expected_old_సందేశం.చొప్పించు(expected_old_సందేశం.చివర(), old_fragments[fragment_id].begin(), old_fragments[fragment_id].చివర());
	}

	std::సదిశ_2<uint8_t> new_fragment0(GPS_RTCM_MAX_FRAGMENT_LEN);
	std::iota(new_fragment0.begin(), new_fragment0.చివర(), static_cast<uint8_t>(0x70));

	// WHEN: A packet for the next sequence arrives.
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 0, new_sequence_id), new_fragment0.దత్తాంశం(),
				 new_fragment0.కొలత(), timestamp, len);

	// THEN: Only a gap-free run of full 180-byte fragments from fragment 0 is
	// flushed. Any gap in the buffered old sequence emits nothing.
	ఒకవేళ (expected_old_సందేశం.ఖాళీలు()) {
		EXPECT_EQ(సందేశం_3, శూన్యసూచిక);
		EXPECT_EQ(len, 0u);

	} లేకపోతే {
		ASSERT_NE(సందేశం_3, శూన్యసూచిక);
		EXPECT_EQ(len, expected_old_సందేశం.కొలత());
		EXPECT_EQ(memcmp(సందేశం_3, expected_old_సందేశం.దత్తాంశం(), expected_old_సందేశం.కొలత()), 0);
	}

	std::సదిశ_2<uint8_t> new_fragment1(7);
	std::iota(new_fragment1.begin(), new_fragment1.చివర(), static_cast<uint8_t>(0x90));
	సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 1, new_sequence_id), new_fragment1.దత్తాంశం(), new_fragment1.కొలత(),
					       timestamp + 10, len);

	std::సదిశ_2<uint8_t> expected_new_సందేశం = new_fragment0;
	expected_new_సందేశం.చొప్పించు(expected_new_సందేశం.చివర(), new_fragment1.begin(), new_fragment1.చివర());

	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, expected_new_సందేశం.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, expected_new_సందేశం.దత్తాంశం(), expected_new_సందేశం.కొలత()), 0);
}

INSTANTIATE_TEST_SUITE_P(RolloverFlushCases, GpsRtcmSequenceChangeTest,
			 ::testing::విలువలు(
				 GpsRtcmSequenceChangeCase {"Single180ByteFragment", {0}, 1},
				 GpsRtcmSequenceChangeCase {"Two180ByteFragments", {0, 1}, 2},
				 GpsRtcmSequenceChangeCase {"Three180ByteFragments", {0, 1, 2}, 3},
				 GpsRtcmSequenceChangeCase {"MissingFragmentZero", {1, 2}, 0},
				 GpsRtcmSequenceChangeCase {"GapAfterFragmentZero", {0, 2}, 0},
				 GpsRtcmSequenceChangeCase {"GapBeforeFragmentThree", {0, 1, 3}, 0}
			 ),
			 [](స్థిరరాశి ::testing::TestParamInfo<GpsRtcmSequenceChangeCase> &test_param_info)
{
	తిరిగి test_param_info.param.name;
});

// WHAT: Verify that the fragmented sequence-change path can emit both the
// older buffered message and a new single-fragment fragmented message.
// WHY: A non-full fragment 0 is already a complete fragmented message. This
// only happens if the sender sets the fragmented bit on a packet that would
// normally be sent unfragmented, but PX4 still needs to handle it safely.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Queues_Single_Fragment_సందేశం_After_Sequence_Change_Flush)
{
	// GIVEN: An old exact-180-byte fragmented message and a new sequence whose
	// fragment 0 is non-full, so it is also the final fragment even though a
	// compliant sender would usually send it as unfragmented.
	std::సదిశ_2<uint8_t> old_payload(GPS_RTCM_MAX_FRAGMENT_LEN);
	std::iota(old_payload.begin(), old_payload.చివర(), 0);
	std::సదిశ_2<uint8_t> new_payload(23);
	std::iota(new_payload.begin(), new_payload.చివర(), static_cast<uint8_t>(0x80));
	size_t len = 0;

	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 0, 9), old_payload.దత్తాంశం(),
					       old_payload.కొలత(), 10, len), శూన్యసూచిక);

	// WHEN: A new sequence starts with a non-full fragment 0, which completes
	// the new message immediately while also flushing the old sequence.
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 0, 10), new_payload.దత్తాంశం(),
				 new_payload.కొలత(), 20, len);

	// THEN: addPacket() returns the older flushed message first.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, old_payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, old_payload.దత్తాంశం(), old_payload.కొలత()), 0);

	సందేశం_3 = gps_rtcm_assembler.take_Deferred_సందేశం(len);

	// THEN: The new short complete message is queued and can be drained
	// immediately afterwards.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, new_payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, new_payload.దత్తాంశం(), new_payload.కొలత()), 0);
	EXPECT_EQ(gps_rtcm_assembler.take_Deferred_సందేశం(len), శూన్యసూచిక);
	EXPECT_EQ(len, 0u);
}

// WHAT: Verify that the unfragmented path can emit both the older buffered
// message and the new unfragmented packet from one call.
// WHY: This covers the separate unfragmented branch, which uses the same
// compatibility fallback but does not go through fragmented reassembly.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Queues_రెండవ_Unfragmented_సందేశం_After_Legacy_Flush)
{
	// GIVEN: An old exact-180-byte fragmented message followed by a new
	// unfragmented packet.
	std::సదిశ_2<uint8_t> old_payload(GPS_RTCM_MAX_FRAGMENT_LEN);
	std::iota(old_payload.begin(), old_payload.చివర(), 0);
	std::సదిశ_2<uint8_t> new_payload(23);
	std::iota(new_payload.begin(), new_payload.చివర(), static_cast<uint8_t>(0xa0));
	size_t len = 0;

	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 0, 11), old_payload.దత్తాంశం(),
					       old_payload.కొలత(), 10, len), శూన్యసూచిక);

	// WHEN: The next packet arrives without the fragmented flag.
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(అబద్ధం, 0, 12), new_payload.దత్తాంశం(),
				 new_payload.కొలత(), 20, len);

	// THEN: addPacket() returns the older flushed message first from
	// _assembled_message; the new unfragmented payload is deferred.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, old_payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, old_payload.దత్తాంశం(), old_payload.కొలత()), 0);

	సందేశం_3 = gps_rtcm_assembler.take_Deferred_సందేశం(len);

	// THEN: The new unfragmented payload is queued and can be drained
	// immediately afterwards.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, new_payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, new_payload.దత్తాంశం(), new_payload.కొలత()), 0);
	EXPECT_EQ(gps_rtcm_assembler.take_Deferred_సందేశం(len), శూన్యసూచిక);
	EXPECT_EQ(len, 0u);
}

// WHAT: Verify that a spec-compliant sender still completes on the short
// terminal fragment before any sequence rollover logic is needed.
// WHY: The receiver-side sequence-change fallback must not change the normal
// completion path for a standard [180, short] fragmented payload.
TEST_F(RtcmTest, GpsRtcmAssemblerCompletesOnShortTerminalBeforeSequenceChange)
{
	// GIVEN: A payload with one full fragment and one short terminal fragment.
	std::సదిశ_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN + 23);
	std::iota(payload.begin(), payload.చివర(), 0);
	size_t len = 0;

	// WHEN: The fragmented payload arrives in order and completes on the short
	// final fragment.
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 0, 3), payload.దత్తాంశం(),
					       GPS_RTCM_MAX_FRAGMENT_LEN, 10, len), శూన్యసూచిక);
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 1, 3),
				 &payload[GPS_RTCM_MAX_FRAGMENT_LEN], 23, 20, len);

	// THEN: The message completes immediately, without waiting for a later
	// sequence change.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, payload.దత్తాంశం(), payload.కొలత()), 0);
}

// WHAT: Verify that four full-sized fragments complete a message explicitly.
// WHY: The exact 720-byte case must not rely on an accidental fallback path.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Completes_On_Fourth_నిండిన_Fragment)
{
	// GIVEN: A payload that uses all four 180-byte fragment slots.
	std::సదిశ_2<uint8_t> payload(GPS_RTCM_MAX_సందేశం_LEN);
	std::iota(payload.begin(), payload.చివర(), 0);

	size_t len = 0;

	// WHEN: The first three full fragments arrive.
	కొరకు (uint8_t fragment = 0; fragment < GPS_RTCM_MAX_FRAGMENTS - 1; fragment++) {
		EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, fragment, 3),
						       &payload[fragment * GPS_RTCM_MAX_FRAGMENT_LEN], GPS_RTCM_MAX_FRAGMENT_LEN, 10 + fragment, len),
			  శూన్యసూచిక);
	}

	// WHEN: The fourth full fragment arrives.
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 3, 3),
				 &payload[3 * GPS_RTCM_MAX_FRAGMENT_LEN], GPS_RTCM_MAX_FRAGMENT_LEN, 20, len);

	// THEN: The assembler emits the completed 720-byte payload.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, payload.దత్తాంశం(), payload.కొలత()), 0);
}

// WHAT: Verify that an exact multiple of 180 bytes below 720 bytes can be
// completed with an empty terminating fragment.
// WHY: Per the MAVLink completion rule, the receiver only knows the message is
// shorter than 4 fragments after the first non-full fragment arrives.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Completes_On_Zero_నిడివి_Terminating_Fragment)
{
	// GIVEN: A 360-byte payload split into two full fragments.
	std::సదిశ_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN * 2);
	std::iota(payload.begin(), payload.చివర(), 0);

	size_t len = 0;

	// WHEN: The two full fragments arrive.
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 0, 11), payload.దత్తాంశం(),
					       GPS_RTCM_MAX_FRAGMENT_LEN, 10, len), శూన్యసూచిక);
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 1, 11),
					       payload.దత్తాంశం() + GPS_RTCM_MAX_FRAGMENT_LEN, GPS_RTCM_MAX_FRAGMENT_LEN, 20, len), శూన్యసూచిక);

	// WHEN: An empty fragment marks the end of the MAVLink fragmented payload.
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 2, 11),
				 payload.దత్తాంశం() + payload.కొలత(), 0, 30, len);

	// THEN: The assembler returns the original 360-byte payload.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, payload.దత్తాంశం(), payload.కొలత()), 0);
}

// WHAT: Verify that a stale partial buffer is dropped after the timeout.
// WHY: This prevents an old fragment from being mixed into a later message with the same sequence ID.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Drops_Stale_Partial_సందేశం)
{
	// GIVEN: A payload that spans one full fragment and one non-full fragment.
	std::సదిశ_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN + 15, 0x44);

	size_t len = 0;

	// WHEN: Fragment 0 is followed by a late fragment 1 after the timeout window.
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 0, 4), payload.దత్తాంశం(), GPS_RTCM_MAX_FRAGMENT_LEN, 10,
					       len), శూన్యసూచిక);
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 1, 4),
					       &payload[GPS_RTCM_MAX_FRAGMENT_LEN], 15, 10 + GPS_RTCM_FRAGMENT_TIMEOUT_US + 1, len),
		  శూన్యసూచిక);

	// WHEN: The matching fragment 0 arrives after the timeout reset.
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 0, 4), payload.దత్తాంశం(),
				 GPS_RTCM_MAX_FRAGMENT_LEN, 10 + GPS_RTCM_FRAGMENT_TIMEOUT_US + 2, len);

	// THEN: The fresh out-of-order pair is reconstructed without reusing the stale pre-timeout state.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, payload.దత్తాంశం(), payload.కొలత()), 0);
}

// WHAT: Verify that a conflicting fragment slot restarts assembly from the
// newest packet.
// WHY: The sequence ID only identifies one active buffer. If a slot is reused
// with different bytes, the old partial state is no longer trustworthy.
TEST_F(RtcmTest, GpsRtcmAssemblerRestartsOnFragmentContentMismatch)
{
	// GIVEN: Two different payloads that reuse the same fragment slot under the
	// same sequence ID.
	std::సదిశ_2<uint8_t> old_fragment(GPS_RTCM_MAX_FRAGMENT_LEN, 0x55);
	std::సదిశ_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN * 2 + 12);
	std::iota(payload.begin(), payload.చివర(), 0);
	size_t len = 0;

	// WHEN: Fragment 1 is first buffered with old content, then replaced with
	// new content for the same sequence ID.
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 1, 9), old_fragment.దత్తాంశం(),
					       old_fragment.కొలత(), 10, len), శూన్యసూచిక);
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 1, 9),
					       &payload[GPS_RTCM_MAX_FRAGMENT_LEN], GPS_RTCM_MAX_FRAGMENT_LEN, 20, len), శూన్యసూచిక);

	// WHEN: The remaining fragments for the new buffer arrive.
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 0, 9), payload.దత్తాంశం(),
					       GPS_RTCM_MAX_FRAGMENT_LEN, 30, len), శూన్యసూచిక);
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 2, 9),
				 &payload[GPS_RTCM_MAX_FRAGMENT_LEN * 2], 12, 40, len);

	// THEN: The assembler returns the newer payload, not the abandoned partial buffer.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, payload.దత్తాంశం(), payload.కొలత()), 0);
}

// WHAT: Verify that a non-full fragment cannot appear before an already buffered higher fragment.
// WHY: Once a fragment claims to be the first non-full fragment, any stored
// higher fragment makes the buffered message internally inconsistent and the
// buffer must be dropped.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Rejects_Non_నిండిన_Fragment_Before_Buffered_Higher_Fragment)
{
	// GIVEN: An out-of-order higher fragment and a later valid two-fragment message with the same sequence ID.
	std::సదిశ_2<uint8_t> garbage_fragment(GPS_RTCM_MAX_FRAGMENT_LEN, 0x77);
	std::సదిశ_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN + 16, 0x88);
	size_t len = 0;

	// WHEN: A higher fragment is buffered first.
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 2, 10), garbage_fragment.దత్తాంశం(),
					       garbage_fragment.కొలత(), 10, len), శూన్యసూచిక);

	// WHEN: A non-full lower fragment arrives and would incorrectly claim the message already ended.
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 1, 10), &payload[GPS_RTCM_MAX_FRAGMENT_LEN], 16, 20,
					       len), శూన్యసూచిక);

	// WHEN: A clean message is sent afterwards with the same sequence ID.
	EXPECT_EQ(gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 0, 10), payload.దత్తాంశం(),
					       GPS_RTCM_MAX_FRAGMENT_LEN, 30, len), శూన్యసూచిక);
	స్థిరరాశి uint8_t *సందేశం_3 = gps_rtcm_assembler.కూడు_Packet(buildGpsRtcmFlags(నిజం, 1, 10),
				 &payload[GPS_RTCM_MAX_FRAGMENT_LEN], 16, 40, len);

	// THEN: The inconsistent partial buffer was discarded and the clean message reconstructs normally.
	ASSERT_NE(సందేశం_3, శూన్యసూచిక);
	EXPECT_EQ(len, payload.కొలత());
	EXPECT_EQ(memcmp(సందేశం_3, payload.దత్తాంశం(), payload.కొలత()), 0);
}
