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

#ضمن "RtcmTestCommon.hpp"

// =============================================================================
// Helper function tests
// =============================================================================

TEST_F(RtcmTest, Payload_ڊيگهائي_Masks_Reserved_بٽ)
{
	// Reserved bits set to 1, length should still be extracted correctly
	uint8_t چوکٺو[] = {RTCM3_PREAMBLE, 0xFF, 0xFF};  // Reserved=0x3F, Length=1023
	EXPECT_EQ(rtcm3_payload_ڊيگهائي(چوکٺو), 1023u);
}

TEST_F(RtcmTest, پيغام_Type_Extraction)
{
	خودڪار چوکٺو = build_Valid_چوکٺو(1005, {});
	EXPECT_EQ(rtcm3_پيغام_type(چوکٺو.معلومات()), 1005u);
}

// =============================================================================
// Good sender tests - valid frames, happy path
// =============================================================================

TEST_F(RtcmTest, Single_گهٽتر_چوکٺو)
{
	خودڪار چوکٺو = build_Raw_چوکٺو({0x00, 0x00});
	parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

	size_t len = 0;
	ثابت uint8_t *نياپو = parser.get_ايندڙ_پيغام(&len);

	ASSERT_NE(نياپو, خالي_اشارو);
	EXPECT_EQ(len, چوکٺو.سائيز());

	parser.consume_پيغام(len);
	خودڪار stats = parser.getStats();
	EXPECT_EQ(stats.messages_parsed, 1u);
	EXPECT_EQ(stats.چڪري_جاچ_errors_2, 0u);
}

TEST_F(RtcmTest, Single_Maximal_چوکٺو)
{
	std::ويڪٽر_2<uint8_t> payload(RTCM3_MAX_PAYLOAD_LEN, 0xAA);
	خودڪار چوکٺو = build_Raw_چوکٺو(payload);
	parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

	size_t len = 0;
	ثابت uint8_t *نياپو = parser.get_ايندڙ_پيغام(&len);

	ASSERT_NE(نياپو, خالي_اشارو);
	EXPECT_EQ(len, RTCM3_MAX_چوکٺو_LEN);
}

TEST_F(RtcmTest, گڻو_چوکٺا_اندريون_Sequence)
{
	خودڪار frame1 = build_Valid_چوکٺو(1005, {0x01, 0x02, 0x03});
	خودڪار frame2 = build_Valid_چوکٺو(1077, {0x04, 0x05, 0x06, 0x07});
	خودڪار frame3 = build_Valid_چوکٺو(1087, {0x08});

	parser.جوڙيو_معلومات(frame1.معلومات(), frame1.سائيز());
	parser.جوڙيو_معلومات(frame2.معلومات(), frame2.سائيز());
	parser.جوڙيو_معلومات(frame3.معلومات(), frame3.سائيز());

	size_t len;
	صحيح ڳڻپ = 0;

	طالما (parser.get_ايندڙ_پيغام(&len) != خالي_اشارو) {
		parser.consume_پيغام(len);
		ڳڻپ++;
	}

	EXPECT_EQ(ڳڻپ, 3);
}

TEST_F(RtcmTest, چوکٺو_Arrives_اندريون_Chunks)
{
	خودڪار چوکٺو = build_Valid_چوکٺو(1005, {0x01, 0x02, 0x03, 0x04, 0x05});

	// Feed one byte at a time
	لكل (size_t i = 0; i < چوکٺو.سائيز() - 1; i++) {
		parser.جوڙيو_معلومات(&چوکٺو[i], 1);
		size_t len;
		EXPECT_EQ(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
	}

	parser.جوڙيو_معلومات(&چوکٺو[چوکٺو.سائيز() - 1], 1);

	size_t len;
	ASSERT_NE(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
	EXPECT_EQ(len, چوکٺو.سائيز());
}

// =============================================================================
// Partially good sender tests - mix of valid and invalid
// =============================================================================

TEST_F(RtcmTest, Garbage_Before_Valid_چوکٺو)
{
	std::ويڪٽر_2<uint8_t> معلومات;

	// 50 bytes of garbage
	لكل (صحيح i = 0; i < 50; i++) {
		معلومات.push_back(0x00 + i);
	}

	خودڪار چوکٺو = build_Valid_چوکٺو(1005, {0x01, 0x02});
	معلومات.وجهو(معلومات.پڇاڙي(), چوکٺو.begin(), چوکٺو.پڇاڙي());

	parser.جوڙيو_معلومات(معلومات.معلومات(), معلومات.سائيز());

	size_t len;
	ثابت uint8_t *نياپو = parser.get_ايندڙ_پيغام(&len);
	ASSERT_NE(نياپو, خالي_اشارو);
	EXPECT_EQ(rtcm3_پيغام_type(نياپو), 1005u);

	parser.consume_پيغام(len);
	EXPECT_EQ(parser.getStats().بائيٽون_discarded_2, 50u);
}

TEST_F(RtcmTest, Valid_چوکٺو_Bad_چوکٺو_Valid)
{
	خودڪار frame1 = build_Valid_چوکٺو(1005, {0x01});
	خودڪار bad_چوکٺو = build_Bad_چڪري_جاچ_چوکٺو({0x02, 0x03});
	خودڪار frame2 = build_Valid_چوکٺو(1077, {0x04});

	parser.جوڙيو_معلومات(frame1.معلومات(), frame1.سائيز());
	parser.جوڙيو_معلومات(bad_چوکٺو.معلومات(), bad_چوکٺو.سائيز());
	parser.جوڙيو_معلومات(frame2.معلومات(), frame2.سائيز());

	size_t len;
	صحيح valid_ڳڻپ = 0;

	طالما (parser.get_ايندڙ_پيغام(&len) != خالي_اشارو) {
		parser.consume_پيغام(len);
		valid_ڳڻپ++;
	}

	EXPECT_EQ(valid_ڳڻپ, 2);
	EXPECT_EQ(parser.getStats().چڪري_جاچ_errors_2, 1u);
}

// =============================================================================
// Buffer and edge case tests
// =============================================================================

TEST_F(RtcmTest, بفر_حد_کان_مٿي_Protection)
{
	std::ويڪٽر_2<uint8_t> large_معلومات(CorrectionFramer::بفر_سائيز_2 + 1000, 0xAA);
	size_t added = parser.جوڙيو_معلومات(large_معلومات.معلومات(), large_معلومات.سائيز());

	EXPECT_EQ(added, CorrectionFramer::بفر_سائيز_2);
}

TEST_F(RtcmTest, خالي_جايون_Payload_چوکٺو)
{
	خودڪار چوکٺو = build_Raw_چوکٺو({});
	parser.جوڙيو_معلومات(چوکٺو.معلومات(), چوکٺو.سائيز());

	size_t len;
	ASSERT_NE(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
	EXPECT_EQ(len, RTCM3_سرو_LEN + RTCM3_چڪري_جاچ_LEN);
}

TEST_F(RtcmTest, Get_ايندڙ_On_خالي_جايون_بفر)
{
	size_t len;
	EXPECT_EQ(parser.get_ايندڙ_پيغام(&len), خالي_اشارو);
}

// =============================================================================
// GPS_RTCM_DATA fragmentation tests
// =============================================================================

// WHAT: Verify that a normal unfragmented payload is passed through unchanged.
// WHY: The assembler must ignore sequence bits when the fragmented flag is not set.
TEST_F(RtcmTest, GpsRtcmAssemblerReturnsUnfragmentedPayload)
{
	// GIVEN: A small payload that fits in one packet.
	ثابت std::ويڪٽر_2<uint8_t> payload {0x01, 0x02, 0x03, 0x04};
	size_t len = 0;

	// WHEN: The payload is added without the fragmented flag.
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(خطأ, 0, 9), payload.معلومات(), payload.سائيز(), 10,
				 len);

	// THEN: The assembler returns the full payload unchanged.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, payload.معلومات(), payload.سائيز()), 0);
}

// WHAT: Verify that the unfragmented path accepts the largest payload the helper stores.
// WHY: This keeps the direct helper's explicit size guard covered even though
// MAVLink GPS_RTCM_DATA packets top out at 180 payload bytes.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_قبولي_Max_سائيز_Unfragmented_Payload)
{
	// GIVEN: A payload that is exactly as large as the assembler's internal
	// message buffer. This exceeds what MAVLink can carry in one packet, but the
	// direct helper still needs a checked upper bound.
	std::ويڪٽر_2<uint8_t> payload(GPS_RTCM_MAX_پيغام_LEN);
	std::iota(payload.begin(), payload.پڇاڙي(), 0);
	size_t len = 0;

	// WHEN: The payload is added as a single unfragmented packet.
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(خطأ), payload.معلومات(), payload.سائيز(), 10, len);

	// THEN: The assembler accepts it and returns the original bytes unchanged.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, payload.معلومات(), payload.سائيز()), 0);
}

// WHAT: Verify that oversized fragmented packets are rejected.
// WHY: GPS_RTCM_DATA only allows up to 180 bytes per fragment, so larger packets must be dropped.
TEST_F(RtcmTest, GpsRtcmAssemblerRejectsOversizedFragment)
{
	// GIVEN: A fragmented packet that exceeds the protocol fragment size limit.
	std::ويڪٽر_2<uint8_t> oversized_fragment(GPS_RTCM_MAX_FRAGMENT_LEN + 1, 0x5a);
	size_t len = 123;

	// WHEN: The oversized fragment is passed to the assembler.
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 0, 6), oversized_fragment.معلومات(),
				 oversized_fragment.سائيز(), 10, len);

	// THEN: The fragment is rejected and no payload is returned.
	EXPECT_EQ(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, 0u);
}

// WHAT: Verify that fragments can arrive out of order and still be reassembled correctly.
// WHY: The sequence/fragment handling exists to allow for unreliable delivery order.
TEST_F(RtcmTest, GpsRtcmAssemblerReordersFragments)
{
	// GIVEN: A payload that spans three fragments with a non-full final fragment.
	std::ويڪٽر_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN * 2 + 25);
	std::iota(payload.begin(), payload.پڇاڙي(), 0);
	ثابت uint8_t flags0 = buildGpsRtcmFlags(سچ, 0, 7);
	ثابت uint8_t flags1 = buildGpsRtcmFlags(سچ, 1, 7);
	ثابت uint8_t flags2 = buildGpsRtcmFlags(سچ, 2, 7);

	size_t len = 0;

	// WHEN: The later fragments arrive before fragment 0.
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(flags1, &payload[GPS_RTCM_MAX_FRAGMENT_LEN], GPS_RTCM_MAX_FRAGMENT_LEN, 10, len),
		  خالي_اشارو);
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(flags2, &payload[GPS_RTCM_MAX_FRAGMENT_LEN * 2], 25, 20, len), خالي_اشارو);

	// WHEN: Fragment 0 finally arrives.
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(flags0, payload.معلومات(), GPS_RTCM_MAX_FRAGMENT_LEN, 30, len);

	// THEN: The assembler emits the full payload in the correct order.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, payload.معلومات(), payload.سائيز()), 0);
}

// WHAT: Verify that a duplicate fragment with identical content is handled.
// WHY: Retransmissions should not reset a healthy buffer or corrupt the assembled payload.
TEST_F(RtcmTest, GpsRtcmAssemblerIgnoresDuplicateMatchingFragment)
{
	// GIVEN: A payload that spans one full fragment and one non-full fragment.
	std::ويڪٽر_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN + 12);
	std::iota(payload.begin(), payload.پڇاڙي(), 0);
	ثابت uint8_t flags0 = buildGpsRtcmFlags(سچ, 0, 5);
	ثابت uint8_t flags1 = buildGpsRtcmFlags(سچ, 1, 5);
	size_t len = 0;

	// WHEN: Fragment 0 is received twice with the same content.
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(flags0, payload.معلومات(), GPS_RTCM_MAX_FRAGMENT_LEN, 10, len), خالي_اشارو);
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(flags0, payload.معلومات(), GPS_RTCM_MAX_FRAGMENT_LEN, 11, len), خالي_اشارو);

	// WHEN: The non-full final fragment arrives.
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(flags1, &payload[GPS_RTCM_MAX_FRAGMENT_LEN], 12, 20, len);

	// THEN: The assembler still returns the expected full payload once.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, payload.معلومات(), payload.سائيز()), 0);
}

بنية GpsRtcmSequenceChangeCase {
	ثابت حرف *name;
	std::ويڪٽر_2<uint8_t> old_fragment_ids;
	size_t expected_flushed_fragments;
};

صنف GpsRtcmSequenceChangeTest : عام RtcmTest, عام ::testing::WithParamInterface<GpsRtcmSequenceChangeCase>
{
};

// WHAT: Verify that legacy exact-multiple fragmented messages are sent
// when every buffered fragment is a full 180-byte prefix starting at fragment 0.
// WHY: This keeps the receiver compatible with older QGC senders while still
// rejecting partial buffers that have a missing fragment in the middle.
TEST_P(GpsRtcmSequenceChangeTest, Gps_Rtcm_Assembler_Flushes_Only_Gap_Free_ڀريل_اڳياڙي_On_Sequence_Change)
{
	ثابت GpsRtcmSequenceChangeCase &param = GetParam();
	ثابت uint8_t old_sequence_id = 9;
	ثابت uint8_t new_sequence_id = 10;
	size_t len = 0;
	uint64_t timestamp = 10;
	std::ويڪٽر_2<std::ويڪٽر_2<uint8_t>> old_fragments(GPS_RTCM_MAX_FRAGMENTS);

	// GIVEN: A buffered old sequence with full-sized fragments as described by
	// the parameter set, followed by a new sequence that starts with fragment 0.
	لكل (ثابت uint8_t fragment_id : param.old_fragment_ids) {
		old_fragments[fragment_id].resize(GPS_RTCM_MAX_FRAGMENT_LEN);
		std::iota(old_fragments[fragment_id].begin(), old_fragments[fragment_id].پڇاڙي(),
			  static_cast<uint8_t>(0x20 + fragment_id * 0x10));
		EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, fragment_id, old_sequence_id),
						       old_fragments[fragment_id].معلومات(), old_fragments[fragment_id].سائيز(), timestamp, len),
			  خالي_اشارو);
		EXPECT_EQ(len, 0u);
		timestamp += 10;
	}

	std::ويڪٽر_2<uint8_t> expected_old_پيغام;

	لكل (size_t fragment_id = 0; fragment_id < param.expected_flushed_fragments; fragment_id++) {
		expected_old_پيغام.وجهو(expected_old_پيغام.پڇاڙي(), old_fragments[fragment_id].begin(), old_fragments[fragment_id].پڇاڙي());
	}

	std::ويڪٽر_2<uint8_t> new_fragment0(GPS_RTCM_MAX_FRAGMENT_LEN);
	std::iota(new_fragment0.begin(), new_fragment0.پڇاڙي(), static_cast<uint8_t>(0x70));

	// WHEN: A packet for the next sequence arrives.
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 0, new_sequence_id), new_fragment0.معلومات(),
				 new_fragment0.سائيز(), timestamp, len);

	// THEN: Only a gap-free run of full 180-byte fragments from fragment 0 is
	// flushed. Any gap in the buffered old sequence emits nothing.
	إذا (expected_old_پيغام.خالي_جايون()) {
		EXPECT_EQ(پيغام_2, خالي_اشارو);
		EXPECT_EQ(len, 0u);

	} وإلا {
		ASSERT_NE(پيغام_2, خالي_اشارو);
		EXPECT_EQ(len, expected_old_پيغام.سائيز());
		EXPECT_EQ(memcmp(پيغام_2, expected_old_پيغام.معلومات(), expected_old_پيغام.سائيز()), 0);
	}

	std::ويڪٽر_2<uint8_t> new_fragment1(7);
	std::iota(new_fragment1.begin(), new_fragment1.پڇاڙي(), static_cast<uint8_t>(0x90));
	پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 1, new_sequence_id), new_fragment1.معلومات(), new_fragment1.سائيز(),
					       timestamp + 10, len);

	std::ويڪٽر_2<uint8_t> expected_new_پيغام = new_fragment0;
	expected_new_پيغام.وجهو(expected_new_پيغام.پڇاڙي(), new_fragment1.begin(), new_fragment1.پڇاڙي());

	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, expected_new_پيغام.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, expected_new_پيغام.معلومات(), expected_new_پيغام.سائيز()), 0);
}

INSTANTIATE_TEST_SUITE_P(RolloverFlushCases, GpsRtcmSequenceChangeTest,
			 ::testing::قدرون(
				 GpsRtcmSequenceChangeCase {"Single180ByteFragment", {0}, 1},
				 GpsRtcmSequenceChangeCase {"Two180ByteFragments", {0, 1}, 2},
				 GpsRtcmSequenceChangeCase {"Three180ByteFragments", {0, 1, 2}, 3},
				 GpsRtcmSequenceChangeCase {"MissingFragmentZero", {1, 2}, 0},
				 GpsRtcmSequenceChangeCase {"GapAfterFragmentZero", {0, 2}, 0},
				 GpsRtcmSequenceChangeCase {"GapBeforeFragmentThree", {0, 1, 3}, 0}
			 ),
			 [](ثابت ::testing::TestParamInfo<GpsRtcmSequenceChangeCase> &test_param_info)
{
	أرجع test_param_info.param.name;
});

// WHAT: Verify that the fragmented sequence-change path can emit both the
// older buffered message and a new single-fragment fragmented message.
// WHY: A non-full fragment 0 is already a complete fragmented message. This
// only happens if the sender sets the fragmented bit on a packet that would
// normally be sent unfragmented, but PX4 still needs to handle it safely.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Queues_Single_Fragment_پيغام_After_Sequence_Change_Flush)
{
	// GIVEN: An old exact-180-byte fragmented message and a new sequence whose
	// fragment 0 is non-full, so it is also the final fragment even though a
	// compliant sender would usually send it as unfragmented.
	std::ويڪٽر_2<uint8_t> old_payload(GPS_RTCM_MAX_FRAGMENT_LEN);
	std::iota(old_payload.begin(), old_payload.پڇاڙي(), 0);
	std::ويڪٽر_2<uint8_t> new_payload(23);
	std::iota(new_payload.begin(), new_payload.پڇاڙي(), static_cast<uint8_t>(0x80));
	size_t len = 0;

	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 0, 9), old_payload.معلومات(),
					       old_payload.سائيز(), 10, len), خالي_اشارو);

	// WHEN: A new sequence starts with a non-full fragment 0, which completes
	// the new message immediately while also flushing the old sequence.
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 0, 10), new_payload.معلومات(),
				 new_payload.سائيز(), 20, len);

	// THEN: addPacket() returns the older flushed message first.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, old_payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, old_payload.معلومات(), old_payload.سائيز()), 0);

	پيغام_2 = gps_rtcm_assembler.take_Deferred_پيغام(len);

	// THEN: The new short complete message is queued and can be drained
	// immediately afterwards.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, new_payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, new_payload.معلومات(), new_payload.سائيز()), 0);
	EXPECT_EQ(gps_rtcm_assembler.take_Deferred_پيغام(len), خالي_اشارو);
	EXPECT_EQ(len, 0u);
}

// WHAT: Verify that the unfragmented path can emit both the older buffered
// message and the new unfragmented packet from one call.
// WHY: This covers the separate unfragmented branch, which uses the same
// compatibility fallback but does not go through fragmented reassembly.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Queues_ٻيو_Unfragmented_پيغام_After_Legacy_Flush)
{
	// GIVEN: An old exact-180-byte fragmented message followed by a new
	// unfragmented packet.
	std::ويڪٽر_2<uint8_t> old_payload(GPS_RTCM_MAX_FRAGMENT_LEN);
	std::iota(old_payload.begin(), old_payload.پڇاڙي(), 0);
	std::ويڪٽر_2<uint8_t> new_payload(23);
	std::iota(new_payload.begin(), new_payload.پڇاڙي(), static_cast<uint8_t>(0xa0));
	size_t len = 0;

	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 0, 11), old_payload.معلومات(),
					       old_payload.سائيز(), 10, len), خالي_اشارو);

	// WHEN: The next packet arrives without the fragmented flag.
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(خطأ, 0, 12), new_payload.معلومات(),
				 new_payload.سائيز(), 20, len);

	// THEN: addPacket() returns the older flushed message first from
	// _assembled_message; the new unfragmented payload is deferred.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, old_payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, old_payload.معلومات(), old_payload.سائيز()), 0);

	پيغام_2 = gps_rtcm_assembler.take_Deferred_پيغام(len);

	// THEN: The new unfragmented payload is queued and can be drained
	// immediately afterwards.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, new_payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, new_payload.معلومات(), new_payload.سائيز()), 0);
	EXPECT_EQ(gps_rtcm_assembler.take_Deferred_پيغام(len), خالي_اشارو);
	EXPECT_EQ(len, 0u);
}

// WHAT: Verify that a spec-compliant sender still completes on the short
// terminal fragment before any sequence rollover logic is needed.
// WHY: The receiver-side sequence-change fallback must not change the normal
// completion path for a standard [180, short] fragmented payload.
TEST_F(RtcmTest, GpsRtcmAssemblerCompletesOnShortTerminalBeforeSequenceChange)
{
	// GIVEN: A payload with one full fragment and one short terminal fragment.
	std::ويڪٽر_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN + 23);
	std::iota(payload.begin(), payload.پڇاڙي(), 0);
	size_t len = 0;

	// WHEN: The fragmented payload arrives in order and completes on the short
	// final fragment.
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 0, 3), payload.معلومات(),
					       GPS_RTCM_MAX_FRAGMENT_LEN, 10, len), خالي_اشارو);
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 1, 3),
				 &payload[GPS_RTCM_MAX_FRAGMENT_LEN], 23, 20, len);

	// THEN: The message completes immediately, without waiting for a later
	// sequence change.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, payload.معلومات(), payload.سائيز()), 0);
}

// WHAT: Verify that four full-sized fragments complete a message explicitly.
// WHY: The exact 720-byte case must not rely on an accidental fallback path.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Completes_On_Fourth_ڀريل_Fragment)
{
	// GIVEN: A payload that uses all four 180-byte fragment slots.
	std::ويڪٽر_2<uint8_t> payload(GPS_RTCM_MAX_پيغام_LEN);
	std::iota(payload.begin(), payload.پڇاڙي(), 0);

	size_t len = 0;

	// WHEN: The first three full fragments arrive.
	لكل (uint8_t fragment = 0; fragment < GPS_RTCM_MAX_FRAGMENTS - 1; fragment++) {
		EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, fragment, 3),
						       &payload[fragment * GPS_RTCM_MAX_FRAGMENT_LEN], GPS_RTCM_MAX_FRAGMENT_LEN, 10 + fragment, len),
			  خالي_اشارو);
	}

	// WHEN: The fourth full fragment arrives.
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 3, 3),
				 &payload[3 * GPS_RTCM_MAX_FRAGMENT_LEN], GPS_RTCM_MAX_FRAGMENT_LEN, 20, len);

	// THEN: The assembler emits the completed 720-byte payload.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, payload.معلومات(), payload.سائيز()), 0);
}

// WHAT: Verify that an exact multiple of 180 bytes below 720 bytes can be
// completed with an empty terminating fragment.
// WHY: Per the MAVLink completion rule, the receiver only knows the message is
// shorter than 4 fragments after the first non-full fragment arrives.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Completes_On_Zero_ڊيگهائي_Terminating_Fragment)
{
	// GIVEN: A 360-byte payload split into two full fragments.
	std::ويڪٽر_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN * 2);
	std::iota(payload.begin(), payload.پڇاڙي(), 0);

	size_t len = 0;

	// WHEN: The two full fragments arrive.
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 0, 11), payload.معلومات(),
					       GPS_RTCM_MAX_FRAGMENT_LEN, 10, len), خالي_اشارو);
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 1, 11),
					       payload.معلومات() + GPS_RTCM_MAX_FRAGMENT_LEN, GPS_RTCM_MAX_FRAGMENT_LEN, 20, len), خالي_اشارو);

	// WHEN: An empty fragment marks the end of the MAVLink fragmented payload.
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 2, 11),
				 payload.معلومات() + payload.سائيز(), 0, 30, len);

	// THEN: The assembler returns the original 360-byte payload.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, payload.معلومات(), payload.سائيز()), 0);
}

// WHAT: Verify that a stale partial buffer is dropped after the timeout.
// WHY: This prevents an old fragment from being mixed into a later message with the same sequence ID.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Drops_Stale_Partial_پيغام)
{
	// GIVEN: A payload that spans one full fragment and one non-full fragment.
	std::ويڪٽر_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN + 15, 0x44);

	size_t len = 0;

	// WHEN: Fragment 0 is followed by a late fragment 1 after the timeout window.
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 0, 4), payload.معلومات(), GPS_RTCM_MAX_FRAGMENT_LEN, 10,
					       len), خالي_اشارو);
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 1, 4),
					       &payload[GPS_RTCM_MAX_FRAGMENT_LEN], 15, 10 + GPS_RTCM_FRAGMENT_TIMEOUT_US + 1, len),
		  خالي_اشارو);

	// WHEN: The matching fragment 0 arrives after the timeout reset.
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 0, 4), payload.معلومات(),
				 GPS_RTCM_MAX_FRAGMENT_LEN, 10 + GPS_RTCM_FRAGMENT_TIMEOUT_US + 2, len);

	// THEN: The fresh out-of-order pair is reconstructed without reusing the stale pre-timeout state.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, payload.معلومات(), payload.سائيز()), 0);
}

// WHAT: Verify that a conflicting fragment slot restarts assembly from the
// newest packet.
// WHY: The sequence ID only identifies one active buffer. If a slot is reused
// with different bytes, the old partial state is no longer trustworthy.
TEST_F(RtcmTest, GpsRtcmAssemblerRestartsOnFragmentContentMismatch)
{
	// GIVEN: Two different payloads that reuse the same fragment slot under the
	// same sequence ID.
	std::ويڪٽر_2<uint8_t> old_fragment(GPS_RTCM_MAX_FRAGMENT_LEN, 0x55);
	std::ويڪٽر_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN * 2 + 12);
	std::iota(payload.begin(), payload.پڇاڙي(), 0);
	size_t len = 0;

	// WHEN: Fragment 1 is first buffered with old content, then replaced with
	// new content for the same sequence ID.
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 1, 9), old_fragment.معلومات(),
					       old_fragment.سائيز(), 10, len), خالي_اشارو);
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 1, 9),
					       &payload[GPS_RTCM_MAX_FRAGMENT_LEN], GPS_RTCM_MAX_FRAGMENT_LEN, 20, len), خالي_اشارو);

	// WHEN: The remaining fragments for the new buffer arrive.
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 0, 9), payload.معلومات(),
					       GPS_RTCM_MAX_FRAGMENT_LEN, 30, len), خالي_اشارو);
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 2, 9),
				 &payload[GPS_RTCM_MAX_FRAGMENT_LEN * 2], 12, 40, len);

	// THEN: The assembler returns the newer payload, not the abandoned partial buffer.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, payload.معلومات(), payload.سائيز()), 0);
}

// WHAT: Verify that a non-full fragment cannot appear before an already buffered higher fragment.
// WHY: Once a fragment claims to be the first non-full fragment, any stored
// higher fragment makes the buffered message internally inconsistent and the
// buffer must be dropped.
TEST_F(RtcmTest, Gps_Rtcm_Assembler_Rejects_Non_ڀريل_Fragment_Before_Buffered_Higher_Fragment)
{
	// GIVEN: An out-of-order higher fragment and a later valid two-fragment message with the same sequence ID.
	std::ويڪٽر_2<uint8_t> garbage_fragment(GPS_RTCM_MAX_FRAGMENT_LEN, 0x77);
	std::ويڪٽر_2<uint8_t> payload(GPS_RTCM_MAX_FRAGMENT_LEN + 16, 0x88);
	size_t len = 0;

	// WHEN: A higher fragment is buffered first.
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 2, 10), garbage_fragment.معلومات(),
					       garbage_fragment.سائيز(), 10, len), خالي_اشارو);

	// WHEN: A non-full lower fragment arrives and would incorrectly claim the message already ended.
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 1, 10), &payload[GPS_RTCM_MAX_FRAGMENT_LEN], 16, 20,
					       len), خالي_اشارو);

	// WHEN: A clean message is sent afterwards with the same sequence ID.
	EXPECT_EQ(gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 0, 10), payload.معلومات(),
					       GPS_RTCM_MAX_FRAGMENT_LEN, 30, len), خالي_اشارو);
	ثابت uint8_t *پيغام_2 = gps_rtcm_assembler.جوڙيو_Packet(buildGpsRtcmFlags(سچ, 1, 10),
				 &payload[GPS_RTCM_MAX_FRAGMENT_LEN], 16, 40, len);

	// THEN: The inconsistent partial buffer was discarded and the clean message reconstructs normally.
	ASSERT_NE(پيغام_2, خالي_اشارو);
	EXPECT_EQ(len, payload.سائيز());
	EXPECT_EQ(memcmp(پيغام_2, payload.معلومات(), payload.سائيز()), 0);
}
