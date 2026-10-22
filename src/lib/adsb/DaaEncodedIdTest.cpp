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
 * @file DaaEncodedIdTest.cpp
 * @brief Unit tests for the DaaEncodedId.
 *
 * @author Jonas Perolini <jonspero@me.com>
 */

#অন্তর্ভাব <gtest/gtest.h>

#অন্তর্ভাব <cstring>

#অন্তর্ভাব <lib/adsb/DaaEncodedId.h>

নামাকাশ
{

শূন্য set_report_callsign(transponder_report_s &report, ধ্রুবক অক্ষর *callsign)
{
	strncpy(report.callsign, callsign, আকার(report.callsign) - 1);
	report.callsign[আকার(report.callsign) - 1] = '\0';
}

} // namespace

// Callsign packs to a 64-bit key and back; the round-trip re-encodes identically, unterminated rejected.
TEST(DaaEncodedIdTest, CallsignRoundTrip)
{
	ধ্রুবক অক্ষর *callsigns[] {
		"A",
		"PX4TEST1",
		"ABCD1234",
		"ZZ999999",
	};

	জন্য (ধ্রুবক অক্ষর *callsign : callsigns) {
		ধ্রুবক uint64_t চাবি = DaaEncodedId::callsign_to_uint64(callsign);

		অক্ষর recovered[k_Callsign_দৈর্ঘ্য];
		DaaEncodedId::convert_uint64_callsign_to_str(চাবি, recovered);

		EXPECT_EQ(চাবি, DaaEncodedId::callsign_to_uint64(recovered));
	}

	// unterminated input -> 0
	অক্ষর callsign_non_null[k_Callsign_দৈর্ঘ্য];
	memset(callsign_non_null, 'A', আকার(callsign_non_null));
	EXPECT_EQ(DaaEncodedId::callsign_to_uint64(callsign_non_null), 0u);
}

// High-bit callsign bytes stay in their own byte slot; no sign extension across the key.
TEST(DaaEncodedIdTest, CallsignPackingDoesNotSignExtend)
{
	ধ্রুবক অক্ষর callsign[k_Callsign_দৈর্ঘ্য] {static_cast<অক্ষর>(0x80), static_cast<অক্ষর>(0xFF), '\0'};

	EXPECT_EQ(DaaEncodedId::callsign_to_uint64(callsign), static_cast<uint64_t>(0xFF80u));
}

TEST(DaaEncodedIdTest, CallsignParamsUseDocumentedCharacterOrder)
{
	// Documented parameter values: "PX4 " = 0x50583420, "TEST" = 0x54455354.
	ধ্রুবক uint64_t encoded = DaaEncodedId::callsign_params_to_uint64(0x50583420, 0x54455354);
	EXPECT_EQ(encoded, 0x5453455420345850ULL);

	অক্ষর decoded[k_Callsign_দৈর্ঘ্য] {};
	DaaEncodedId::convert_uint64_callsign_to_str(encoded, decoded);
	EXPECT_STREQ(decoded, "PX4 TEST");

	// An embedded terminator ends a shorter callsign just as it does in a traffic report.
	EXPECT_EQ(DaaEncodedId::callsign_params_to_uint64(0x50583400, 0x54455354),
		  DaaEncodedId::callsign_to_uint64("PX4"));
	EXPECT_EQ(DaaEncodedId::callsign_params_to_uint64(0, 0), 0u);
}

// Decoding a short callsign clears every byte after the terminator (MAVLink copies the whole field).
TEST(DaaEncodedIdTest, CallsignDecodeClearsUnusedBytes)
{
	ধ্রুবক অক্ষর callsign[k_Callsign_দৈর্ঘ্য] {'A', 'B', '\0'};
	ধ্রুবক uint64_t চাবি = DaaEncodedId::callsign_to_uint64(callsign);

	অক্ষর recovered[k_Callsign_দৈর্ঘ্য];
	memset(recovered, 0x7F, আকার(recovered));
	DaaEncodedId::convert_uint64_callsign_to_str(চাবি, recovered);

	EXPECT_STREQ(recovered, "AB");

	জন্য (পূর্ণসংখ্যা i = 2; i < k_Callsign_দৈর্ঘ্য; ++i) {
		EXPECT_EQ(recovered[i], '\0');
	}
}

// ICAO formats as fixed-width uppercase hex of the low 24 bits; a too-small buffer is left untouched.
TEST(DaaEncodedIdTest, FormatsIcaoAsFixedWidthHex)
{
	অক্ষর icao_বাফার[k_Icao_দৈর্ঘ্য] {};

	// zero-padded + uppercased
	DaaEncodedId::convert_icao_uint32_to_hex_str(0xABCu, icao_বাফার, আকার(icao_বাফার));
	EXPECT_STREQ(icao_বাফার, "000ABC");

	// only the low 24 bits
	DaaEncodedId::convert_icao_uint32_to_hex_str(0x12ABCDEFu, icao_বাফার, আকার(icao_বাফার));
	EXPECT_STREQ(icao_বাফার, "ABCDEF");

	// too-small buffer untouched
	অক্ষর small_বাফার[k_Icao_দৈর্ঘ্য - 1];
	memset(small_বাফার, 'X', আকার(small_বাফার));
	DaaEncodedId::convert_icao_uint32_to_hex_str(0xABCu, small_বাফার, আকার(small_বাফার));
	EXPECT_EQ(small_বাফার[0], 'X');
}

// Packed UAS-ID key preserves the exact GUID tail bytes (little-endian); all-zero packs to 0.
TEST(DaaEncodedIdTest, Uas_Id_Packing_Preserves_লেজ_Bytes)
{
	uint8_t uas_id[k_Uas_Id_Byte_দৈর্ঘ্য];

	জন্য (পূর্ণসংখ্যা i = 0; i < 4; ++i) {
		জন্য (পূর্ণসংখ্যা b = 0; b < k_Uas_Id_Byte_দৈর্ঘ্য; ++b) {
			uas_id[b] = static_cast<uint8_t>(0x10 * i + b);
		}

		ধ্রুবক uint64_t চাবি = DaaEncodedId::last_uas_id_bytes_to_uint64(uas_id);

		// key byte k == GUID tail byte k
		জন্য (পূর্ণসংখ্যা k = 0; k < kIdEncodingNbBytes; ++k) {
			ধ্রুবক uint8_t চাবি_byte = static_cast<uint8_t>((চাবি >>(k * 8)) & 0xFF);
			EXPECT_EQ(uas_id[k_Uas_Id_Byte_দৈর্ঘ্য - kIdEncodingNbBytes + k], চাবি_byte);
		}
	}

	uint8_t zero_uas_id[k_Uas_Id_Byte_দৈর্ঘ্য] {};
	EXPECT_EQ(DaaEncodedId::last_uas_id_bytes_to_uint64(zero_uas_id), 0u);
}

// to_string() renders each encoding, plus an explicit placeholder for an unknown one.
TEST(DaaEncodedIdTest, ToStringRendersEachEncoding)
{
	অক্ষর বাফার_2[k_Utm_Guid_বার্তা_দৈর্ঘ্য];

	// ICAO -> fixed-width uppercase hex of the low 24 bits.
	DaaEncodedId{0xABCu, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO}.to_string(বাফার_2, আকার(বাফার_2));
	EXPECT_STREQ(বাফার_2, "000ABC");

	// Callsign -> the decoded callsign string.
	ধ্রুবক uint64_t callsign_চাবি = DaaEncodedId::callsign_to_uint64("AB");
	DaaEncodedId{callsign_চাবি, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.to_string(বাফার_2, আকার(বাফার_2));
	EXPECT_STREQ(বাফার_2, "AB");

	ধ্রুবক অক্ষর callsign_with_controls[k_Callsign_দৈর্ঘ্য] {'A', '\n', static_cast<অক্ষর>(0x7f), 'B', '\0'};
	DaaEncodedId{DaaEncodedId::callsign_to_uint64(callsign_with_controls),
		     detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.to_string(বাফার_2, আকার(বাফার_2));
	EXPECT_STREQ(বাফার_2, "A??B");

	// UAS ID -> reduced lowercase hex of the packed tail bytes (little-endian).
	DaaEncodedId{0x0102030405u, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID}.to_string(বাফার_2, আকার(বাফার_2));
	EXPECT_STREQ(বাফার_2, "0504030201");

	// Unknown encoding -> explicit placeholder.
	অক্ষর unknown_বাফার[32];
	DaaEncodedId{42u, 0xFF}.to_string(unknown_বাফার, আকার(unknown_বাফার));
	EXPECT_STREQ(unknown_বাফার, "Unknown ID.");
}

TEST(DaaEncodedIdTest, To_String_Respects_বাফার_আকার)
{
	কাঠামো guarded_বাফার_s {
		অক্ষর output[4];
		অক্ষর guard;
	};

	guarded_বাফার_s callsign_বাফার{{}, 'X'};
	ধ্রুবক uint64_t callsign_চাবি = DaaEncodedId::callsign_to_uint64("ABCDEFGH");
	DaaEncodedId{callsign_চাবি, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.to_string(
		callsign_বাফার.output, আকার(callsign_বাফার.output));
	EXPECT_STREQ(callsign_বাফার.output, "ABC");
	EXPECT_EQ(callsign_বাফার.guard, 'X');

	guarded_বাফার_s uas_id_বাফার{{}, 'X'};
	DaaEncodedId{0x0102030405u, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID}.to_string(
		uas_id_বাফার.output, আকার(uas_id_বাফার.output));
	EXPECT_STREQ(uas_id_বাফার.output, "050");
	EXPECT_EQ(uas_id_বাফার.guard, 'X');

	অক্ষর one_byte_বাফার{'X'};
	DaaEncodedId{callsign_চাবি, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.to_string(&one_byte_বাফার, 1);
	EXPECT_EQ(one_byte_বাফার, '\0');
}

// Equality keys on both id and encoding.
TEST(DaaEncodedIdTest, EqualityComparesIdAndEncoding)
{
	ধ্রুবক DaaEncodedId a{42u, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO};

	// Same id and encoding: equal.
	EXPECT_TRUE(a == (DaaEncodedId{42u, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO}));
	EXPECT_FALSE(a != (DaaEncodedId{42u, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO}));

	// Different id: not equal.
	EXPECT_FALSE(a == (DaaEncodedId{43u, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO}));

	// Same numeric id, different encoding: not equal (keeps colliding values in separate buffer slots).
	EXPECT_FALSE(a == (DaaEncodedId{42u, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}));
	EXPECT_TRUE(a != (DaaEncodedId{42u, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}));
}

// from_report() selects ICAO > callsign > UAS-ID, or id=0 when nothing usable is present.
TEST(DaaEncodedIdTest, FromReportSelectsEncodingByPriority)
{
	// ICAO present: selected, and it wins even when a valid callsign is also present.
	transponder_report_s report{};
	report.icao_address = 0x4F74EE;
	report.flags = transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN;
	set_report_callsign(report, "ABC");

	DaaEncodedId id = DaaEncodedId::from_report(report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO);
	EXPECT_EQ(id.id, 0x4F74EEu);

	// No ICAO, valid callsign flag: callsign selected.
	report.icao_address = 0;
	id = DaaEncodedId::from_report(report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN);
	EXPECT_EQ(id.id, DaaEncodedId::callsign_to_uint64("ABC"));

	// An out-of-range ICAO address is ignored in favor of the next valid identifier.
	report.icao_address = kMaxIcaoAddress + 1u;
	id = DaaEncodedId::from_report(report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN);
	EXPECT_EQ(id.id, DaaEncodedId::callsign_to_uint64("ABC"));

	// No ICAO, no callsign flag, but a non-zero UAS ID: UAS-ID selected.
	transponder_report_s uas_report{};
	uas_report.uas_id[k_Uas_Id_Byte_দৈর্ঘ্য - 1] = 0xAB;
	id = DaaEncodedId::from_report(uas_report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID);
	EXPECT_EQ(id.id, DaaEncodedId::last_uas_id_bytes_to_uint64(uas_report.uas_id));

	// Nothing usable: default id.
	transponder_report_s empty_report{};
	EXPECT_EQ(DaaEncodedId::from_report(empty_report).id, 0u);
}

// ICAO self-detection matches the primary or secondary ownship ICAO; unset (negative) never matches.
TEST(DaaEncodedIdTest, SelfDetectionIcao)
{
	ধ্রুবক DaaEncodedId traffic_id{0x123456, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO};

	daa_ownship_ids_s ownship_ids{};
	EXPECT_FALSE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));

	ownship_ids.icao = 0x123456;
	EXPECT_TRUE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));

	ownship_ids = {};
	ownship_ids.icao_2 = 0x123456;
	EXPECT_TRUE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));

	ownship_ids.icao = 0x654321;
	ownship_ids.icao_2 = 0x654322;
	EXPECT_FALSE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));
}

// Callsign self-detection requires an exact packed match.
TEST(DaaEncodedIdTest, SelfDetectionCallsign)
{
	ধ্রুবক অক্ষর callsign[k_Callsign_দৈর্ঘ্য] = "TST1234";
	ধ্রুবক uint64_t packed_callsign = DaaEncodedId::callsign_to_uint64(callsign);
	ASSERT_NE(packed_callsign, 0u);

	ধ্রুবক DaaEncodedId traffic_id{packed_callsign, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN};

	daa_ownship_ids_s ownship_ids{};
	EXPECT_FALSE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));

	ownship_ids.callsign = packed_callsign;
	EXPECT_TRUE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));

	ownship_ids.callsign = packed_callsign ^ 1u;
	EXPECT_FALSE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));
}

// UAS-ID self-detection only when the board UUID is valid and the packed key matches.
TEST(DaaEncodedIdTest, SelfDetectionUasId)
{
	uint8_t uas_id_bytes[k_Uas_Id_Byte_দৈর্ঘ্য];

	জন্য (পূর্ণসংখ্যা i = 0; i < k_Uas_Id_Byte_দৈর্ঘ্য; ++i) {
		uas_id_bytes[i] = 0xE0 + i;
	}

	ধ্রুবক uint64_t packed_uas_id = DaaEncodedId::last_uas_id_bytes_to_uint64(uas_id_bytes);
	ASSERT_NE(packed_uas_id, 0u);

	ধ্রুবক DaaEncodedId traffic_id{packed_uas_id, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID};

	daa_ownship_ids_s ownship_ids{};
	ownship_ids.uas_id = packed_uas_id;
	EXPECT_FALSE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));

	ownship_ids.uas_id_valid = সত্য;
	EXPECT_TRUE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));

	ownship_ids.uas_id = packed_uas_id ^ 1u;
	EXPECT_FALSE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));
}

// identify_traffic_report() rejects reports with no identity and ownship reports, decodes the rest.
TEST(DaaEncodedIdTest, IdentifyTrafficReport)
{
	daa_ownship_ids_s ownship_ids{};

	transponder_report_s report{};
	EXPECT_EQ(DaaEncodedId::identify_traffic_report(report, ownship_ids).id, 0u);

	report.icao_address = 0xABCDEF;
	ধ্রুবক DaaEncodedId encoded_id = DaaEncodedId::identify_traffic_report(report, ownship_ids);
	EXPECT_EQ(encoded_id.id, 0xABCDEFu);
	EXPECT_EQ(encoded_id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO);

	ownship_ids.icao = 0xABCDEF;
	EXPECT_EQ(DaaEncodedId::identify_traffic_report(report, ownship_ids).id, 0u);
}
