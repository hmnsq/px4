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

#అన్తర్భావ <gtest/gtest.h>

#అన్తర్భావ <cstring>

#అన్తర్భావ <lib/adsb/DaaEncodedId.h>

నామాకాశ
{

శూన్య సమితి_report_callsign(transponder_report_s &report, స్థిరరాశి అక్షరం *callsign)
{
	strncpy(report.callsign, callsign, పరిమాణం(report.callsign) - 1);
	report.callsign[పరిమాణం(report.callsign) - 1] = '\0';
}

} // namespace

// Callsign packs to a 64-bit key and back; the round-trip re-encodes identically, unterminated rejected.
TEST(DaaEncodedIdTest, Callsign_విడత_Trip)
{
	స్థిరరాశి అక్షరం *callsigns[] {
		"A",
		"PX4TEST1",
		"ABCD1234",
		"ZZ999999",
	};

	కొరకు (స్థిరరాశి అక్షరం *callsign : callsigns) {
		స్థిరరాశి uint64_t కీలకం = DaaEncodedId::callsign_గమ్యం_uint64(callsign);

		అక్షరం recovered[k_Callsign_నిడివి];
		DaaEncodedId::convert_uint64_callsign_గమ్యం_str(కీలకం, recovered);

		EXPECT_EQ(కీలకం, DaaEncodedId::callsign_గమ్యం_uint64(recovered));
	}

	// unterminated input -> 0
	అక్షరం callsign_non_null[k_Callsign_నిడివి];
	memset(callsign_non_null, 'A', పరిమాణం(callsign_non_null));
	EXPECT_EQ(DaaEncodedId::callsign_గమ్యం_uint64(callsign_non_null), 0u);
}

// High-bit callsign bytes stay in their own byte slot; no sign extension across the key.
TEST(DaaEncodedIdTest, CallsignPackingDoesNotSignExtend)
{
	స్థిరరాశి అక్షరం callsign[k_Callsign_నిడివి] {static_cast<అక్షరం>(0x80), static_cast<అక్షరం>(0xFF), '\0'};

	EXPECT_EQ(DaaEncodedId::callsign_గమ్యం_uint64(callsign), static_cast<uint64_t>(0xFF80u));
}

TEST(DaaEncodedIdTest, Callsign_Params_Use_Documented_Character_క్రమం)
{
	// Documented parameter values: "PX4 " = 0x50583420, "TEST" = 0x54455354.
	స్థిరరాశి uint64_t encoded = DaaEncodedId::callsign_params_గమ్యం_uint64(0x50583420, 0x54455354);
	EXPECT_EQ(encoded, 0x5453455420345850ULL);

	అక్షరం decoded[k_Callsign_నిడివి] {};
	DaaEncodedId::convert_uint64_callsign_గమ్యం_str(encoded, decoded);
	EXPECT_STREQ(decoded, "PX4 TEST");

	// An embedded terminator ends a shorter callsign just as it does in a traffic report.
	EXPECT_EQ(DaaEncodedId::callsign_params_గమ్యం_uint64(0x50583400, 0x54455354),
		  DaaEncodedId::callsign_గమ్యం_uint64("PX4"));
	EXPECT_EQ(DaaEncodedId::callsign_params_గమ్యం_uint64(0, 0), 0u);
}

// Decoding a short callsign clears every byte after the terminator (MAVLink copies the whole field).
TEST(DaaEncodedIdTest, Callsign_Decode_Clears_వాడని_బైట్లు)
{
	స్థిరరాశి అక్షరం callsign[k_Callsign_నిడివి] {'A', 'B', '\0'};
	స్థిరరాశి uint64_t కీలకం = DaaEncodedId::callsign_గమ్యం_uint64(callsign);

	అక్షరం recovered[k_Callsign_నిడివి];
	memset(recovered, 0x7F, పరిమాణం(recovered));
	DaaEncodedId::convert_uint64_callsign_గమ్యం_str(కీలకం, recovered);

	EXPECT_STREQ(recovered, "AB");

	కొరకు (పూర్ణాంకం i = 2; i < k_Callsign_నిడివి; ++i) {
		EXPECT_EQ(recovered[i], '\0');
	}
}

// ICAO formats as fixed-width uppercase hex of the low 24 bits; a too-small buffer is left untouched.
TEST(DaaEncodedIdTest, Formats_Icao_As_Fixed_వెడల్పు_Hex)
{
	అక్షరం icao_బఫరు[k_Icao_నిడివి] {};

	// zero-padded + uppercased
	DaaEncodedId::convert_icao_uint32_గమ్యం_hex_str(0xABCu, icao_బఫరు, పరిమాణం(icao_బఫరు));
	EXPECT_STREQ(icao_బఫరు, "000ABC");

	// only the low 24 bits
	DaaEncodedId::convert_icao_uint32_గమ్యం_hex_str(0x12ABCDEFu, icao_బఫరు, పరిమాణం(icao_బఫరు));
	EXPECT_STREQ(icao_బఫరు, "ABCDEF");

	// too-small buffer untouched
	అక్షరం small_బఫరు[k_Icao_నిడివి - 1];
	memset(small_బఫరు, 'X', పరిమాణం(small_బఫరు));
	DaaEncodedId::convert_icao_uint32_గమ్యం_hex_str(0xABCu, small_బఫరు, పరిమాణం(small_బఫరు));
	EXPECT_EQ(small_బఫరు[0], 'X');
}

// Packed UAS-ID key preserves the exact GUID tail bytes (little-endian); all-zero packs to 0.
TEST(DaaEncodedIdTest, Uas_Id_Packing_Preserves_తోక_బైట్లు)
{
	uint8_t uas_id[k_Uas_Id_Byte_నిడివి];

	కొరకు (పూర్ణాంకం i = 0; i < 4; ++i) {
		కొరకు (పూర్ణాంకం b = 0; b < k_Uas_Id_Byte_నిడివి; ++b) {
			uas_id[b] = static_cast<uint8_t>(0x10 * i + b);
		}

		స్థిరరాశి uint64_t కీలకం = DaaEncodedId::చివరి_uas_id_బైట్లు_గమ్యం_uint64(uas_id);

		// key byte k == GUID tail byte k
		కొరకు (పూర్ణాంకం k = 0; k < k_Id_Encoding_Nb_బైట్లు; ++k) {
			స్థిరరాశి uint8_t కీలకం_byte = static_cast<uint8_t>((కీలకం >>(k * 8)) & 0xFF);
			EXPECT_EQ(uas_id[k_Uas_Id_Byte_నిడివి - k_Id_Encoding_Nb_బైట్లు + k], కీలకం_byte);
		}
	}

	uint8_t zero_uas_id[k_Uas_Id_Byte_నిడివి] {};
	EXPECT_EQ(DaaEncodedId::చివరి_uas_id_బైట్లు_గమ్యం_uint64(zero_uas_id), 0u);
}

// to_string() renders each encoding, plus an explicit placeholder for an unknown one.
TEST(DaaEncodedIdTest, గమ్యం_String_Renders_Each_Encoding)
{
	అక్షరం బఫరు_2[k_Utm_Guid_సందేశం_నిడివి];

	// ICAO -> fixed-width uppercase hex of the low 24 bits.
	DaaEncodedId{0xABCu, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO}.గమ్యం_string(బఫరు_2, పరిమాణం(బఫరు_2));
	EXPECT_STREQ(బఫరు_2, "000ABC");

	// Callsign -> the decoded callsign string.
	స్థిరరాశి uint64_t callsign_కీలకం = DaaEncodedId::callsign_గమ్యం_uint64("AB");
	DaaEncodedId{callsign_కీలకం, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.గమ్యం_string(బఫరు_2, పరిమాణం(బఫరు_2));
	EXPECT_STREQ(బఫరు_2, "AB");

	స్థిరరాశి అక్షరం callsign_with_controls[k_Callsign_నిడివి] {'A', '\n', static_cast<అక్షరం>(0x7f), 'B', '\0'};
	DaaEncodedId{DaaEncodedId::callsign_గమ్యం_uint64(callsign_with_controls),
		     detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.గమ్యం_string(బఫరు_2, పరిమాణం(బఫరు_2));
	EXPECT_STREQ(బఫరు_2, "A??B");

	// UAS ID -> reduced lowercase hex of the packed tail bytes (little-endian).
	DaaEncodedId{0x0102030405u, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID}.గమ్యం_string(బఫరు_2, పరిమాణం(బఫరు_2));
	EXPECT_STREQ(బఫరు_2, "0504030201");

	// Unknown encoding -> explicit placeholder.
	అక్షరం unknown_బఫరు[32];
	DaaEncodedId{42u, 0xFF}.గమ్యం_string(unknown_బఫరు, పరిమాణం(unknown_బఫరు));
	EXPECT_STREQ(unknown_బఫరు, "Unknown ID.");
}

TEST(DaaEncodedIdTest, గమ్యం_String_Respects_బఫరు_కొలత)
{
	నిర్మాణం guarded_బఫరు_s {
		అక్షరం output[4];
		అక్షరం guard;
	};

	guarded_బఫరు_s callsign_బఫరు{{}, 'X'};
	స్థిరరాశి uint64_t callsign_కీలకం = DaaEncodedId::callsign_గమ్యం_uint64("ABCDEFGH");
	DaaEncodedId{callsign_కీలకం, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.గమ్యం_string(
		callsign_బఫరు.output, పరిమాణం(callsign_బఫరు.output));
	EXPECT_STREQ(callsign_బఫరు.output, "ABC");
	EXPECT_EQ(callsign_బఫరు.guard, 'X');

	guarded_బఫరు_s uas_id_బఫరు{{}, 'X'};
	DaaEncodedId{0x0102030405u, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID}.గమ్యం_string(
		uas_id_బఫరు.output, పరిమాణం(uas_id_బఫరు.output));
	EXPECT_STREQ(uas_id_బఫరు.output, "050");
	EXPECT_EQ(uas_id_బఫరు.guard, 'X');

	అక్షరం one_byte_బఫరు{'X'};
	DaaEncodedId{callsign_కీలకం, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.గమ్యం_string(&one_byte_బఫరు, 1);
	EXPECT_EQ(one_byte_బఫరు, '\0');
}

// Equality keys on both id and encoding.
TEST(DaaEncodedIdTest, EqualityComparesIdAndEncoding)
{
	స్థిరరాశి DaaEncodedId a{42u, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO};

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
TEST(DaaEncodedIdTest, From_Report_Selects_Encoding_By_ప్రాధాన్యత)
{
	// ICAO present: selected, and it wins even when a valid callsign is also present.
	transponder_report_s report{};
	report.icao_చిరునామా = 0x4F74EE;
	report.flags = transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN;
	సమితి_report_callsign(report, "ABC");

	DaaEncodedId id = DaaEncodedId::from_report(report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO);
	EXPECT_EQ(id.id, 0x4F74EEu);

	// No ICAO, valid callsign flag: callsign selected.
	report.icao_చిరునామా = 0;
	id = DaaEncodedId::from_report(report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN);
	EXPECT_EQ(id.id, DaaEncodedId::callsign_గమ్యం_uint64("ABC"));

	// An out-of-range ICAO address is ignored in favor of the next valid identifier.
	report.icao_చిరునామా = k_Max_Icao_చిరునామా + 1u;
	id = DaaEncodedId::from_report(report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN);
	EXPECT_EQ(id.id, DaaEncodedId::callsign_గమ్యం_uint64("ABC"));

	// No ICAO, no callsign flag, but a non-zero UAS ID: UAS-ID selected.
	transponder_report_s uas_report{};
	uas_report.uas_id[k_Uas_Id_Byte_నిడివి - 1] = 0xAB;
	id = DaaEncodedId::from_report(uas_report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID);
	EXPECT_EQ(id.id, DaaEncodedId::చివరి_uas_id_బైట్లు_గమ్యం_uint64(uas_report.uas_id));

	// Nothing usable: default id.
	transponder_report_s ఖాళీలు_report{};
	EXPECT_EQ(DaaEncodedId::from_report(ఖాళీలు_report).id, 0u);
}

// ICAO self-detection matches the primary or secondary ownship ICAO; unset (negative) never matches.
TEST(DaaEncodedIdTest, SelfDetectionIcao)
{
	స్థిరరాశి DaaEncodedId traffic_id{0x123456, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO};

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
	స్థిరరాశి అక్షరం callsign[k_Callsign_నిడివి] = "TST1234";
	స్థిరరాశి uint64_t packed_callsign = DaaEncodedId::callsign_గమ్యం_uint64(callsign);
	ASSERT_NE(packed_callsign, 0u);

	స్థిరరాశి DaaEncodedId traffic_id{packed_callsign, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN};

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
	uint8_t uas_id_బైట్లు[k_Uas_Id_Byte_నిడివి];

	కొరకు (పూర్ణాంకం i = 0; i < k_Uas_Id_Byte_నిడివి; ++i) {
		uas_id_బైట్లు[i] = 0xE0 + i;
	}

	స్థిరరాశి uint64_t packed_uas_id = DaaEncodedId::చివరి_uas_id_బైట్లు_గమ్యం_uint64(uas_id_బైట్లు);
	ASSERT_NE(packed_uas_id, 0u);

	స్థిరరాశి DaaEncodedId traffic_id{packed_uas_id, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID};

	daa_ownship_ids_s ownship_ids{};
	ownship_ids.uas_id = packed_uas_id;
	EXPECT_FALSE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));

	ownship_ids.uas_id_valid = నిజం;
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

	report.icao_చిరునామా = 0xABCDEF;
	స్థిరరాశి DaaEncodedId encoded_id = DaaEncodedId::identify_traffic_report(report, ownship_ids);
	EXPECT_EQ(encoded_id.id, 0xABCDEFu);
	EXPECT_EQ(encoded_id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO);

	ownship_ids.icao = 0xABCDEF;
	EXPECT_EQ(DaaEncodedId::identify_traffic_report(report, ownship_ids).id, 0u);
}
