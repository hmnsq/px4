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
 * @file DaaEncodedId.cpp
 *
 * @author Jonas Perolini <jonspero@me.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "DaaEncodedId.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <cinttypes>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdio>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(ਅਚਲ transponder_report_s &report)
{
	ਜੇ (report.icao_ਪਤਾ > 0 && report.icao_ਪਤਾ <= k_Max_Icao_ਪਤਾ) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		ਵਾਪਸ {
			static_cast<uint64_t>(report.icao_ਪਤਾ),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	ਜੇ (report.icao_ਪਤਾ > k_Max_Icao_ਪਤਾ) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	ਜੇ (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		ਅਚਲ uint64_t callsign_id = callsign_ਮੰਜ਼ਿਲ_uint64(report.callsign);

		ਜੇ (callsign_id != 0) {
			ਵਾਪਸ {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	ਤਾਰਕਿਕ uas_id_valid = ਝੂਠ;

	ਲਈ (ਪੂਰਨ i = 0; i < k_Uas_Id_Byte_ਲੰਮਾਈ; ++i) {
		ਜੇ (report.uas_id[i] != 0) {
			uas_id_valid = ਸੱਚ;
			ਰੋਕੋ;
		}
	}

	ਜੇ (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		ਅਚਲ uint64_t uas_id = ਆਖਰੀ_uas_id_ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_uint64(report.uas_id);

		ਜੇ (uas_id != 0) {
			ਵਾਪਸ {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	ਵਾਪਸ {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(ਅਚਲ transponder_report_s &report,
		ਅਚਲ daa_ownship_ids_s &ownship_ids)
{
	ਅਚਲ DaaEncodedId encoded_id = from_report(report);

	ਜੇ (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		ਵਾਪਸ {};
	}

	ਜੇ (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		ਵਾਪਸ {};
	}

	ਵਾਪਸ encoded_id;
}

ਤਾਰਕਿਕ DaaEncodedId::is_self_detection(ਅਚਲ DaaEncodedId &encoded_id, ਅਚਲ daa_ownship_ids_s &ownship_ids)
{
	ਚੋਣ (encoded_id.encoding) {
	ਹਾਲਤ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			ਜੇ (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				ਵਾਪਸ ਸੱਚ;
			}

			ਜੇ (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				ਵਾਪਸ ਸੱਚ;
			}

			ਰੋਕੋ;
		}

	ਹਾਲਤ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			ਜੇ (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				ਵਾਪਸ ਸੱਚ;
			}

			ਰੋਕੋ;
		}

	ਹਾਲਤ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			ਜੇ (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				ਵਾਪਸ ਸੱਚ;
			}

			ਰੋਕੋ;
		}

	ਮੂਲ:
		ਰੋਕੋ;
	}

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ DaaEncodedId::ਮੰਜ਼ਿਲ_string(ਅੱਖਰ *ਬਫਰ_2, size_t ਬਫਰ_ਮਾਪ) ਅਚਲ
{
	ਜੇ (ਬਫਰ_2 == ਖਾਲੀ_ਸੂਚਕ || ਬਫਰ_ਮਾਪ == 0) { ਵਾਪਸ; }

	memset(ਬਫਰ_2, 0, ਬਫਰ_ਮਾਪ);

	ਚੋਣ (encoding) {
	ਹਾਲਤ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_ਮੰਜ਼ਿਲ_hex_str(id, ਬਫਰ_2, ਬਫਰ_ਮਾਪ);
			ਰੋਕੋ;
		}

	ਹਾਲਤ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			ਅੱਖਰ callsign[k_Callsign_ਲੰਮਾਈ] {};
			convert_uint64_callsign_ਮੰਜ਼ਿਲ_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			ਲਈ (ਅੱਖਰ &character : callsign) {
				ਅਚਲ uint8_t byte = static_cast<uint8_t>(character);

				ਜੇ (byte == 0) {
					ਰੋਕੋ;
				}

				ਜੇ (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(ਬਫਰ_2, ਬਫਰ_ਮਾਪ, "%s", callsign);
			ਰੋਕੋ;
		}

	ਹਾਲਤ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			ਅੱਖਰ uas_id[k_Utm_Guid_ਸੁਨੇਹਾ_ਲੰਮਾਈ] {};
			convert_uas_id_uint64_ਮੰਜ਼ਿਲ_str(id, uas_id);
			snprintf(ਬਫਰ_2, ਬਫਰ_ਮਾਪ, "%s", uas_id);
			ਰੋਕੋ;
		}

	ਮੂਲ:
		snprintf(ਬਫਰ_2, ਬਫਰ_ਮਾਪ, "Unknown ID.");
		ਰੋਕੋ;
	}
}

uint64_t DaaEncodedId::callsign_ਮੰਜ਼ਿਲ_uint64(ਅਚਲ ਅੱਖਰ callsign[k_Callsign_ਲੰਮਾਈ])
{
	ਜੇ (callsign == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ 0;
	}

	ਤਾਰਕਿਕ null_terminated = ਝੂਠ;

	ਲਈ (size_t i = 0; i < k_Callsign_ਲੰਮਾਈ; ++i) {
		ਜੇ (callsign[i] == '\0') {
			null_terminated = ਸੱਚ;
			ਰੋਕੋ;
		}
	}

	ਜੇ (!null_terminated) {
		ਵਾਪਸ 0;
	}

	uint64_t ਨਤੀਜਾ = 0;

	ਲਈ (ਪੂਰਨ i = 0; i < k_Id_Encoding_Nb_ਬਾਈਟਾਂ && callsign[i] != '\0'; ++i) {
		ਨਤੀਜਾ |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

uint64_t DaaEncodedId::callsign_params_ਮੰਜ਼ਿਲ_uint64(ਅਚਲ int32_t callsign_part1, ਅਚਲ int32_t callsign_part2)
{
	ਅਚਲ uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	ਅੱਖਰ callsign[k_Callsign_ਲੰਮਾਈ] {};

	ਲਈ (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		ਲਈ (size_t char_idx = 0; char_idx < ਆਕਾਰ(uint32_t); ++char_idx) {
			ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਖਿਸਕਾਓ = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ>((ਆਕਾਰ(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * ਆਕਾਰ(uint32_t) + char_idx] = static_cast<ਅੱਖਰ>((parts[part_idx] >> ਖਿਸਕਾਓ) & 0xffu);
		}
	}

	ਵਾਪਸ callsign_ਮੰਜ਼ਿਲ_uint64(callsign);
}

ਖਾਲੀ DaaEncodedId::convert_uint64_callsign_ਮੰਜ਼ਿਲ_str(uint64_t value, ਅੱਖਰ callsign[k_Callsign_ਲੰਮਾਈ])
{
	ਜੇ (callsign == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	memset(callsign, 0, k_Callsign_ਲੰਮਾਈ);

	ਅਚਲ ਪੂਰਨ max_chars = k_Callsign_ਲੰਮਾਈ - 1;

	ਜੇ (value == 0) {
		memset(callsign, '0', max_chars);

	} ਨਹੀਂਤਾਂ {

		ਲਈ (ਪੂਰਨ i = 0; i < max_chars; ++i) {
			ਅੱਖਰ c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			ਜੇ (c == 0) {
				ਰੋਕੋ;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::ਆਖਰੀ_uas_id_ਬਾਈਟਾਂ_ਮੰਜ਼ਿਲ_uint64(ਅਚਲ uint8_t uas_id[k_Uas_Id_Byte_ਲੰਮਾਈ])
{
	ਜੇ (uas_id == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	ਲਈ (ਪੂਰਨ i = 0; i < k_Id_Encoding_Nb_ਬਾਈਟਾਂ; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[k_Uas_Id_Byte_ਲੰਮਾਈ - k_Id_Encoding_Nb_ਬਾਈਟਾਂ + i]) << (i * 8);
	}

	ਵਾਪਸ uas_id_int;
}

ਖਾਲੀ DaaEncodedId::convert_uas_id_uint64_ਮੰਜ਼ਿਲ_str(ਅਚਲ uint64_t uas_id_int, ਅੱਖਰ uas_id_char_arr[k_Utm_Guid_ਸੁਨੇਹਾ_ਲੰਮਾਈ])
{
	ਜੇ (uas_id_char_arr == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ;
	}

	ਸਥਿਰ constexpr ਅੱਖਰ kHexDigits[] = "0123456789abcdef";

	ਜੇ (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_ਸੁਨੇਹਾ_ਲੰਮਾਈ - 1);

	} ਨਹੀਂਤਾਂ {

		// Render the low reduced_uas_id_length bytes as hex.
		ਅਚਲ ਪੂਰਨ reduced_uas_id_ਲੰਮਾਈ = (k_Utm_Guid_ਸੁਨੇਹਾ_ਲੰਮਾਈ - 1) / 2;

		ਲਈ (ਪੂਰਨ i = 0; i < reduced_uas_id_ਲੰਮਾਈ; ++i) {
			ਅਚਲ uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_ਸੁਨੇਹਾ_ਲੰਮਾਈ - 1] = '\0';
}

ਖਾਲੀ DaaEncodedId::convert_icao_uint32_ਮੰਜ਼ਿਲ_hex_str(uint64_t value, ਅੱਖਰ *ਬਫਰ_2, size_t ਬਫਰ_ਮਾਪ)
{
	ਜੇ (ਬਫਰ_2 == ਖਾਲੀ_ਸੂਚਕ || ਬਫਰ_ਮਾਪ < k_Icao_ਲੰਮਾਈ) {
		ਵਾਪਸ;
	}

	ਅਚਲ uint32_t icao_ਪਤਾ = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(ਬਫਰ_2, ਬਫਰ_ਮਾਪ, "%06" PRIX32, icao_ਪਤਾ);
}
