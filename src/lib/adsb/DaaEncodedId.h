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
 * @file DaaEncodedId.h
 * @brief Traffic report identification helpers and encoded traffic IDs.
 *
 * A transponder report can identify an aircraft three different ways: an ICAO
 * address, an ADS-B callsign, or a UAS / UTM GUID. The DAA traffic buffer keys
 * conflicts on a single 64-bit value, so each of those representations is packed
 * into a uint64_t and the encoding that produced it.
 *
 * @author Jonas Perolini <jonspero@me.com>
 */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <cstddef>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdint>

#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/detect_and_avoid.h>	// UNIQUE_ID_ENCODING_*
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/transponder_report.h>	// from_report()

// Number of trailing identifier bytes packed into the 64-bit key. Max 8 (fits a uint64_t).
ਸਥਿਰ constexpr uint8_t k_Id_Encoding_Nb_ਬਾਈਟਾਂ{8};
// Chars used to display a UAS id: 10 hex + null terminator. Max: 17 = 16 + null.
ਸਥਿਰ constexpr uint8_t k_Utm_Guid_ਸੁਨੇਹਾ_ਲੰਮਾਈ{11};
// ADS-B callsign string length: 8 chars + null terminator.
ਸਥਿਰ constexpr uint8_t k_Callsign_ਲੰਮਾਈ{9};
// ICAO address string length: 6 hex chars + null terminator.
ਸਥਿਰ constexpr uint8_t k_Icao_ਲੰਮਾਈ{7};
ਸਥਿਰ constexpr uint32_t k_Max_Icao_ਪਤਾ{0xFFFFFFu};
// Length of a UAS ID (UTM GUID) byte array.
// Similar to PX4_GUID_BYTE_LENGTH and transponder_report_s::uas_id;
ਸਥਿਰ constexpr uint8_t k_Uas_Id_Byte_ਲੰਮਾਈ{18};

// The ownship identifiers used to reject self-detections.
ਬਣਤਰ daa_ownship_ids_s {
	int32_t icao{-1};         // ADSB_ICAO_ID convention: negative = unset
	int32_t icao_2{-1};       // ADSB_ICAO_ID_2 convention: negative = unset
	uint64_t callsign{0};     // packed with DaaEncodedId::callsign_to_uint64
	uint64_t uas_id{0};       // packed with DaaEncodedId::last_uas_id_bytes_to_uint64
	bool uas_id_valid{false}; // false when the board UUID is unavailable
};

/**
 * @brief A traffic identifier reduced to a 64-bit key plus its source encoding.
 *
 * @c encoding is one of detect_and_avoid_s::UNIQUE_ID_ENCODING_* and tells the
 * codecs and to_string() how to interpret @c id.
 */
ਬਣਤਰ DaaEncodedId {
	uint64_t id{0};
	uint8_t encoding{detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO};

	bool ਸੰਚਾਲਕ==(ਅਚਲ DaaEncodedId &other) ਅਚਲ { ਵਾਪਸ encoding == other.encoding && id == other.id; }
	bool ਸੰਚਾਲਕ!=(ਅਚਲ DaaEncodedId &other) ਅਚਲ { ਵਾਪਸ !(*this == other); }

	/**
	 * @brief Pick the best available identifier from a transponder report.
	 *
	 * Priority is a valid 24-bit ICAO address, ADS-B callsign, then UAS-ID.
	 * Returns the default value when the report has no usable identifier.
	 */
	ਸਥਿਰ DaaEncodedId from_report(ਅਚਲ transponder_report_s &report);

	/**
	 * @brief Identify a transponder traffic report.
	 *
	 * Returns the extracted identifier, or id=0 (the default-constructed value) when the report
	 * carries no usable identifier or matches ownship (self-detection).
	 */
	ਸਥਿਰ DaaEncodedId identify_traffic_report(ਅਚਲ transponder_report_s &report, ਅਚਲ daa_ownship_ids_s &ownship_ids);

	// True if the report's identifier matches ownship (ICAO, callsign or UAS-ID).
	ਸਥਿਰ bool is_self_detection(ਅਚਲ DaaEncodedId &encoded_id, ਅਚਲ daa_ownship_ids_s &ownship_ids);

	// Render to a null-terminated string per its encoding.
	ਖਾਲੀ ਮੰਜ਼ਿਲ_string(ਅੱਖਰ *ਬਫਰ_2, size_t ਬਫਰ_ਮਾਪ) ਅਚਲ;

	// Pack a callsign into a 64-bit key. Returns 0 if the input is not null-terminated.
	ਸਥਿਰ uint64_t callsign_ਮੰਜ਼ਿਲ_uint64(ਅਚਲ ਅੱਖਰ callsign[k_Callsign_ਲੰਮਾਈ]);

	/**
	 * @brief Convert ADSB_CALLSIGN_1/2 parameter values to the packed callsign key.
	 *
	 * Each parameter stores four characters in display order (most-significant byte
	 * first), while callsign_to_uint64() stores the first character in the least-significant
	 * byte. This conversion keeps ownship parameter IDs in the same representation as
	 * callsigns received in transponder_report.
	 */
	ਸਥਿਰ uint64_t callsign_params_ਮੰਜ਼ਿਲ_uint64(int32_t callsign_part1, int32_t callsign_part2);

	// Inverse of callsign_to_uint64.
	ਸਥਿਰ ਖਾਲੀ convert_uint64_callsign_ਮੰਜ਼ਿਲ_str(uint64_t ਮੁੱਲ, ਅੱਖਰ callsign[k_Callsign_ਲੰਮਾਈ]);

	// Pack the trailing bytes of a UAS-ID (UTM GUID) into a 64-bit key.
	ਸਥਿਰ uint64_t ਆਖਰੀ_uas_id_ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_uint64(ਅਚਲ uint8_t uas_id[k_Uas_Id_Byte_ਲੰਮਾਈ]);

	// Render a packed UAS-ID key as a reduced lowercase hex string.
	ਸਥਿਰ ਖਾਲੀ convert_uas_id_uint64_ਮੰਜ਼ਿਲ_str(uint64_t uas_id_int, ਅੱਖਰ uas_id_char_arr[k_Utm_Guid_ਸੁਨੇਹਾ_ਲੰਮਾਈ]);

	// Print the low 24 bits of an ICAO address as a 6-digit uppercase hex string.
	ਸਥਿਰ ਖਾਲੀ convert_icao_uint32_ਮੰਜ਼ਿਲ_hex_str(uint64_t ਮੁੱਲ, ਅੱਖਰ *ਬਫਰ_2, size_t ਬਫਰ_ਮਾਪ);
};
