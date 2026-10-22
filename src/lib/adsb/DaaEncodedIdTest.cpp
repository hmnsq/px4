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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/adsb/DaaEncodedId.h>

ਨਾਮਾਕਾਸ਼
{

ਖਾਲੀ ਸੈੱਟ_report_callsign(transponder_report_s &report, ਅਚਲ ਅੱਖਰ *callsign)
{
	strncpy(report.callsign, callsign, ਆਕਾਰ(report.callsign) - 1);
	report.callsign[ਆਕਾਰ(report.callsign) - 1] = '\0';
}

} // namespace

// Callsign packs to a 64-bit key and back; the round-trip re-encodes identically, unterminated rejected.
TEST(DaaEncodedIdTest, Callsign_ਗੇੜਾ_Trip)
{
	ਅਚਲ ਅੱਖਰ *callsigns[] {
		"A",
		"PX4TEST1",
		"ABCD1234",
		"ZZ999999",
	};

	ਲਈ (ਅਚਲ ਅੱਖਰ *callsign : callsigns) {
		ਅਚਲ uint64_t ਕੁੰਜੀ = DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64(callsign);

		ਅੱਖਰ recovered[k_Callsign_ਲੰਮਾਈ];
		DaaEncodedId::convert_uint64_callsign_ਮੰਜ਼ਿਲ_str(ਕੁੰਜੀ, recovered);

		EXPECT_EQ(ਕੁੰਜੀ, DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64(recovered));
	}

	// unterminated input -> 0
	ਅੱਖਰ callsign_non_null[k_Callsign_ਲੰਮਾਈ];
	memset(callsign_non_null, 'A', ਆਕਾਰ(callsign_non_null));
	EXPECT_EQ(DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64(callsign_non_null), 0u);
}

// High-bit callsign bytes stay in their own byte slot; no sign extension across the key.
TEST(DaaEncodedIdTest, CallsignPackingDoesNotSignExtend)
{
	ਅਚਲ ਅੱਖਰ callsign[k_Callsign_ਲੰਮਾਈ] {static_cast<ਅੱਖਰ>(0x80), static_cast<ਅੱਖਰ>(0xFF), '\0'};

	EXPECT_EQ(DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64(callsign), static_cast<uint64_t>(0xFF80u));
}

TEST(DaaEncodedIdTest, Callsign_Params_Use_Documented_Character_ਕ੍ਰਮ)
{
	// Documented parameter values: "PX4 " = 0x50583420, "TEST" = 0x54455354.
	ਅਚਲ uint64_t encoded = DaaEncodedId::callsign_params_ਮੰਜ਼ਿਲ_uint64(0x50583420, 0x54455354);
	EXPECT_EQ(encoded, 0x5453455420345850ULL);

	ਅੱਖਰ decoded[k_Callsign_ਲੰਮਾਈ] {};
	DaaEncodedId::convert_uint64_callsign_ਮੰਜ਼ਿਲ_str(encoded, decoded);
	EXPECT_STREQ(decoded, "PX4 TEST");

	// An embedded terminator ends a shorter callsign just as it does in a traffic report.
	EXPECT_EQ(DaaEncodedId::callsign_params_ਮੰਜ਼ਿਲ_uint64(0x50583400, 0x54455354),
		  DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64("PX4"));
	EXPECT_EQ(DaaEncodedId::callsign_params_ਮੰਜ਼ਿਲ_uint64(0, 0), 0u);
}

// Decoding a short callsign clears every byte after the terminator (MAVLink copies the whole field).
TEST(DaaEncodedIdTest, Callsign_Decode_Clears_ਅਣਵਰਤਿਆ_ਬਾਈਟਾਂ)
{
	ਅਚਲ ਅੱਖਰ callsign[k_Callsign_ਲੰਮਾਈ] {'A', 'B', '\0'};
	ਅਚਲ uint64_t ਕੁੰਜੀ = DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64(callsign);

	ਅੱਖਰ recovered[k_Callsign_ਲੰਮਾਈ];
	memset(recovered, 0x7F, ਆਕਾਰ(recovered));
	DaaEncodedId::convert_uint64_callsign_ਮੰਜ਼ਿਲ_str(ਕੁੰਜੀ, recovered);

	EXPECT_STREQ(recovered, "AB");

	ਲਈ (ਪੂਰਨ i = 2; i < k_Callsign_ਲੰਮਾਈ; ++i) {
		EXPECT_EQ(recovered[i], '\0');
	}
}

// ICAO formats as fixed-width uppercase hex of the low 24 bits; a too-small buffer is left untouched.
TEST(DaaEncodedIdTest, Formats_Icao_As_Fixed_ਚੌੜਾਈ_Hex)
{
	ਅੱਖਰ icao_ਬਫਰ[k_Icao_ਲੰਮਾਈ] {};

	// zero-padded + uppercased
	DaaEncodedId::convert_icao_uint32_ਮੰਜ਼ਿਲ_hex_str(0xABCu, icao_ਬਫਰ, ਆਕਾਰ(icao_ਬਫਰ));
	EXPECT_STREQ(icao_ਬਫਰ, "000ABC");

	// only the low 24 bits
	DaaEncodedId::convert_icao_uint32_ਮੰਜ਼ਿਲ_hex_str(0x12ABCDEFu, icao_ਬਫਰ, ਆਕਾਰ(icao_ਬਫਰ));
	EXPECT_STREQ(icao_ਬਫਰ, "ABCDEF");

	// too-small buffer untouched
	ਅੱਖਰ small_ਬਫਰ[k_Icao_ਲੰਮਾਈ - 1];
	memset(small_ਬਫਰ, 'X', ਆਕਾਰ(small_ਬਫਰ));
	DaaEncodedId::convert_icao_uint32_ਮੰਜ਼ਿਲ_hex_str(0xABCu, small_ਬਫਰ, ਆਕਾਰ(small_ਬਫਰ));
	EXPECT_EQ(small_ਬਫਰ[0], 'X');
}

// Packed UAS-ID key preserves the exact GUID tail bytes (little-endian); all-zero packs to 0.
TEST(DaaEncodedIdTest, Uas_Id_Packing_Preserves_ਪੂਛ_ਬਾਈਟਾਂ)
{
	uint8_t uas_id[k_Uas_Id_Byte_ਲੰਮਾਈ];

	ਲਈ (ਪੂਰਨ i = 0; i < 4; ++i) {
		ਲਈ (ਪੂਰਨ b = 0; b < k_Uas_Id_Byte_ਲੰਮਾਈ; ++b) {
			uas_id[b] = static_cast<uint8_t>(0x10 * i + b);
		}

		ਅਚਲ uint64_t ਕੁੰਜੀ = DaaEncodedId::ਆਖਰੀ_uas_id_ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_uint64(uas_id);

		// key byte k == GUID tail byte k
		ਲਈ (ਪੂਰਨ k = 0; k < k_Id_Encoding_Nb_ਬਾਈਟਾਂ; ++k) {
			ਅਚਲ uint8_t ਕੁੰਜੀ_byte = static_cast<uint8_t>((ਕੁੰਜੀ >>(k * 8)) & 0xFF);
			EXPECT_EQ(uas_id[k_Uas_Id_Byte_ਲੰਮਾਈ - k_Id_Encoding_Nb_ਬਾਈਟਾਂ + k], ਕੁੰਜੀ_byte);
		}
	}

	uint8_t zero_uas_id[k_Uas_Id_Byte_ਲੰਮਾਈ] {};
	EXPECT_EQ(DaaEncodedId::ਆਖਰੀ_uas_id_ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_uint64(zero_uas_id), 0u);
}

// to_string() renders each encoding, plus an explicit placeholder for an unknown one.
TEST(DaaEncodedIdTest, ਮੰਜ਼ਿਲ_String_Renders_Each_Encoding)
{
	ਅੱਖਰ ਬਫਰ_2[k_Utm_Guid_ਸੁਨੇਹਾ_ਲੰਮਾਈ];

	// ICAO -> fixed-width uppercase hex of the low 24 bits.
	DaaEncodedId{0xABCu, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO}.ਮੰਜ਼ਿਲ_string(ਬਫਰ_2, ਆਕਾਰ(ਬਫਰ_2));
	EXPECT_STREQ(ਬਫਰ_2, "000ABC");

	// Callsign -> the decoded callsign string.
	ਅਚਲ uint64_t callsign_ਕੁੰਜੀ = DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64("AB");
	DaaEncodedId{callsign_ਕੁੰਜੀ, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.ਮੰਜ਼ਿਲ_string(ਬਫਰ_2, ਆਕਾਰ(ਬਫਰ_2));
	EXPECT_STREQ(ਬਫਰ_2, "AB");

	ਅਚਲ ਅੱਖਰ callsign_with_controls[k_Callsign_ਲੰਮਾਈ] {'A', '\n', static_cast<ਅੱਖਰ>(0x7f), 'B', '\0'};
	DaaEncodedId{DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64(callsign_with_controls),
		     detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.ਮੰਜ਼ਿਲ_string(ਬਫਰ_2, ਆਕਾਰ(ਬਫਰ_2));
	EXPECT_STREQ(ਬਫਰ_2, "A??B");

	// UAS ID -> reduced lowercase hex of the packed tail bytes (little-endian).
	DaaEncodedId{0x0102030405u, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID}.ਮੰਜ਼ਿਲ_string(ਬਫਰ_2, ਆਕਾਰ(ਬਫਰ_2));
	EXPECT_STREQ(ਬਫਰ_2, "0504030201");

	// Unknown encoding -> explicit placeholder.
	ਅੱਖਰ unknown_ਬਫਰ[32];
	DaaEncodedId{42u, 0xFF}.ਮੰਜ਼ਿਲ_string(unknown_ਬਫਰ, ਆਕਾਰ(unknown_ਬਫਰ));
	EXPECT_STREQ(unknown_ਬਫਰ, "Unknown ID.");
}

TEST(DaaEncodedIdTest, ਮੰਜ਼ਿਲ_String_Respects_ਬਫਰ_ਮਾਪ)
{
	ਬਣਤਰ guarded_ਬਫਰ_s {
		ਅੱਖਰ output[4];
		ਅੱਖਰ guard;
	};

	guarded_ਬਫਰ_s callsign_ਬਫਰ{{}, 'X'};
	ਅਚਲ uint64_t callsign_ਕੁੰਜੀ = DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64("ABCDEFGH");
	DaaEncodedId{callsign_ਕੁੰਜੀ, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.ਮੰਜ਼ਿਲ_string(
		callsign_ਬਫਰ.output, ਆਕਾਰ(callsign_ਬਫਰ.output));
	EXPECT_STREQ(callsign_ਬਫਰ.output, "ABC");
	EXPECT_EQ(callsign_ਬਫਰ.guard, 'X');

	guarded_ਬਫਰ_s uas_id_ਬਫਰ{{}, 'X'};
	DaaEncodedId{0x0102030405u, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID}.ਮੰਜ਼ਿਲ_string(
		uas_id_ਬਫਰ.output, ਆਕਾਰ(uas_id_ਬਫਰ.output));
	EXPECT_STREQ(uas_id_ਬਫਰ.output, "050");
	EXPECT_EQ(uas_id_ਬਫਰ.guard, 'X');

	ਅੱਖਰ one_byte_ਬਫਰ{'X'};
	DaaEncodedId{callsign_ਕੁੰਜੀ, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN}.ਮੰਜ਼ਿਲ_string(&one_byte_ਬਫਰ, 1);
	EXPECT_EQ(one_byte_ਬਫਰ, '\0');
}

// Equality keys on both id and encoding.
TEST(DaaEncodedIdTest, EqualityComparesIdAndEncoding)
{
	ਅਚਲ DaaEncodedId a{42u, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO};

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
TEST(DaaEncodedIdTest, From_Report_Selects_Encoding_By_ਤਰਜੀਹ)
{
	// ICAO present: selected, and it wins even when a valid callsign is also present.
	transponder_report_s report{};
	report.icao_ਪਤਾ = 0x4F74EE;
	report.flags = transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN;
	ਸੈੱਟ_report_callsign(report, "ABC");

	DaaEncodedId id = DaaEncodedId::from_report(report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO);
	EXPECT_EQ(id.id, 0x4F74EEu);

	// No ICAO, valid callsign flag: callsign selected.
	report.icao_ਪਤਾ = 0;
	id = DaaEncodedId::from_report(report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN);
	EXPECT_EQ(id.id, DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64("ABC"));

	// An out-of-range ICAO address is ignored in favor of the next valid identifier.
	report.icao_ਪਤਾ = k_Max_Icao_ਪਤਾ + 1u;
	id = DaaEncodedId::from_report(report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN);
	EXPECT_EQ(id.id, DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64("ABC"));

	// No ICAO, no callsign flag, but a non-zero UAS ID: UAS-ID selected.
	transponder_report_s uas_report{};
	uas_report.uas_id[k_Uas_Id_Byte_ਲੰਮਾਈ - 1] = 0xAB;
	id = DaaEncodedId::from_report(uas_report);
	EXPECT_EQ(id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID);
	EXPECT_EQ(id.id, DaaEncodedId::ਆਖਰੀ_uas_id_ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_uint64(uas_report.uas_id));

	// Nothing usable: default id.
	transponder_report_s ਖਾਲੀ_ਥਾਵਾਂ_report{};
	EXPECT_EQ(DaaEncodedId::from_report(ਖਾਲੀ_ਥਾਵਾਂ_report).id, 0u);
}

// ICAO self-detection matches the primary or secondary ownship ICAO; unset (negative) never matches.
TEST(DaaEncodedIdTest, SelfDetectionIcao)
{
	ਅਚਲ DaaEncodedId traffic_id{0x123456, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO};

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
	ਅਚਲ ਅੱਖਰ callsign[k_Callsign_ਲੰਮਾਈ] = "TST1234";
	ਅਚਲ uint64_t packed_callsign = DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64(callsign);
	ASSERT_NE(packed_callsign, 0u);

	ਅਚਲ DaaEncodedId traffic_id{packed_callsign, detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN};

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
	uint8_t uas_id_ਬਾਈਟਾਂ[k_Uas_Id_Byte_ਲੰਮਾਈ];

	ਲਈ (ਪੂਰਨ i = 0; i < k_Uas_Id_Byte_ਲੰਮਾਈ; ++i) {
		uas_id_ਬਾਈਟਾਂ[i] = 0xE0 + i;
	}

	ਅਚਲ uint64_t packed_uas_id = DaaEncodedId::ਆਖਰੀ_uas_id_ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_uint64(uas_id_ਬਾਈਟਾਂ);
	ASSERT_NE(packed_uas_id, 0u);

	ਅਚਲ DaaEncodedId traffic_id{packed_uas_id, detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID};

	daa_ownship_ids_s ownship_ids{};
	ownship_ids.uas_id = packed_uas_id;
	EXPECT_FALSE(DaaEncodedId::is_self_detection(traffic_id, ownship_ids));

	ownship_ids.uas_id_valid = ਸੱਚ;
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

	report.icao_ਪਤਾ = 0xABCDEF;
	ਅਚਲ DaaEncodedId encoded_id = DaaEncodedId::identify_traffic_report(report, ownship_ids);
	EXPECT_EQ(encoded_id.id, 0xABCDEFu);
	EXPECT_EQ(encoded_id.encoding, detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO);

	ownship_ids.icao = 0xABCDEF;
	EXPECT_EQ(DaaEncodedId::identify_traffic_report(report, ownship_ids).id, 0u);
}
