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

#ಅನ್ತರ್ಭಾವ "DaaEncodedId.h"

#ಅನ್ತರ್ಭಾವ <cinttypes>
#ಅನ್ತರ್ಭಾವ <cstdio>
#ಅನ್ತರ್ಭಾವ <cstring>

#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(ಸ್ಥಿರಾಂಕ transponder_report_s &report)
{
	ಆದರೆ (report.icao_ವಿಳಾಸ > 0 && report.icao_ವಿಳಾಸ <= k_Max_Icao_ವಿಳಾಸ) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		ಹಿಂತಿರುಗಿ {
			static_cast<uint64_t>(report.icao_ವಿಳಾಸ),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	ಆದರೆ (report.icao_ವಿಳಾಸ > k_Max_Icao_ವಿಳಾಸ) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	ಆದರೆ (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		ಸ್ಥಿರಾಂಕ uint64_t callsign_id = callsign_ಗಮ್ಯ_uint64(report.callsign);

		ಆದರೆ (callsign_id != 0) {
			ಹಿಂತಿರುಗಿ {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	ತಾರ್ಕಿಕ uas_id_valid = ಸುಳ್ಳು;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < k_Uas_Id_Byte_ಉದ್ದಳತೆ; ++i) {
		ಆದರೆ (report.uas_id[i] != 0) {
			uas_id_valid = ನಿಜ;
			ನಿಲ್ಲಿಸು;
		}
	}

	ಆದರೆ (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		ಸ್ಥಿರಾಂಕ uint64_t uas_id = ಕೊನೆಯ_uas_id_ಬೈಟುಗಳು_ಗಮ್ಯ_uint64(report.uas_id);

		ಆದರೆ (uas_id != 0) {
			ಹಿಂತಿರುಗಿ {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	ಹಿಂತಿರುಗಿ {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(ಸ್ಥಿರಾಂಕ transponder_report_s &report,
		ಸ್ಥಿರಾಂಕ daa_ownship_ids_s &ownship_ids)
{
	ಸ್ಥಿರಾಂಕ DaaEncodedId encoded_id = from_report(report);

	ಆದರೆ (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		ಹಿಂತಿರುಗಿ {};
	}

	ಆದರೆ (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		ಹಿಂತಿರುಗಿ {};
	}

	ಹಿಂತಿರುಗಿ encoded_id;
}

ತಾರ್ಕಿಕ DaaEncodedId::is_self_detection(ಸ್ಥಿರಾಂಕ DaaEncodedId &encoded_id, ಸ್ಥಿರಾಂಕ daa_ownship_ids_s &ownship_ids)
{
	ಆಯ್ಕೆ (encoded_id.encoding) {
	ಸ್ಥಿತಿ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			ಆದರೆ (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				ಹಿಂತಿರುಗಿ ನಿಜ;
			}

			ಆದರೆ (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				ಹಿಂತಿರುಗಿ ನಿಜ;
			}

			ನಿಲ್ಲಿಸು;
		}

	ಸ್ಥಿತಿ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			ಆದರೆ (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				ಹಿಂತಿರುಗಿ ನಿಜ;
			}

			ನಿಲ್ಲಿಸು;
		}

	ಸ್ಥಿತಿ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			ಆದರೆ (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				ಹಿಂತಿರುಗಿ ನಿಜ;
			}

			ನಿಲ್ಲಿಸು;
		}

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ DaaEncodedId::ಗಮ್ಯ_string(ಅಕ್ಷರ *ಬಫರ್_2, size_t ಬಫರ್_ಗಾತ್ರ) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಬಫರ್_2 == ಶೂನ್ಯಸೂಚಕ || ಬಫರ್_ಗಾತ್ರ == 0) { ಹಿಂತಿರುಗಿ; }

	memset(ಬಫರ್_2, 0, ಬಫರ್_ಗಾತ್ರ);

	ಆಯ್ಕೆ (encoding) {
	ಸ್ಥಿತಿ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_ಗಮ್ಯ_hex_str(id, ಬಫರ್_2, ಬಫರ್_ಗಾತ್ರ);
			ನಿಲ್ಲಿಸು;
		}

	ಸ್ಥಿತಿ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			ಅಕ್ಷರ callsign[k_Callsign_ಉದ್ದಳತೆ] {};
			convert_uint64_callsign_ಗಮ್ಯ_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			ಗಾಗಿ (ಅಕ್ಷರ &character : callsign) {
				ಸ್ಥಿರಾಂಕ uint8_t byte = static_cast<uint8_t>(character);

				ಆದರೆ (byte == 0) {
					ನಿಲ್ಲಿಸು;
				}

				ಆದರೆ (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(ಬಫರ್_2, ಬಫರ್_ಗಾತ್ರ, "%s", callsign);
			ನಿಲ್ಲಿಸು;
		}

	ಸ್ಥಿತಿ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			ಅಕ್ಷರ uas_id[k_Utm_Guid_ಸಂದೇಶ_ಉದ್ದಳತೆ] {};
			convert_uas_id_uint64_ಗಮ್ಯ_str(id, uas_id);
			snprintf(ಬಫರ್_2, ಬಫರ್_ಗಾತ್ರ, "%s", uas_id);
			ನಿಲ್ಲಿಸು;
		}

	ಪೂರ್ವನಿಯೋಜಿತ:
		snprintf(ಬಫರ್_2, ಬಫರ್_ಗಾತ್ರ, "Unknown ID.");
		ನಿಲ್ಲಿಸು;
	}
}

uint64_t DaaEncodedId::callsign_ಗಮ್ಯ_uint64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ callsign[k_Callsign_ಉದ್ದಳತೆ])
{
	ಆದರೆ (callsign == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ 0;
	}

	ತಾರ್ಕಿಕ null_terminated = ಸುಳ್ಳು;

	ಗಾಗಿ (size_t i = 0; i < k_Callsign_ಉದ್ದಳತೆ; ++i) {
		ಆದರೆ (callsign[i] == '\0') {
			null_terminated = ನಿಜ;
			ನಿಲ್ಲಿಸು;
		}
	}

	ಆದರೆ (!null_terminated) {
		ಹಿಂತಿರುಗಿ 0;
	}

	uint64_t ಫಲಿತಾಂಶ = 0;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < k_Id_Encoding_Nb_ಬೈಟುಗಳು && callsign[i] != '\0'; ++i) {
		ಫಲಿತಾಂಶ |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

uint64_t DaaEncodedId::callsign_params_ಗಮ್ಯ_uint64(ಸ್ಥಿರಾಂಕ int32_t callsign_part1, ಸ್ಥಿರಾಂಕ int32_t callsign_part2)
{
	ಸ್ಥಿರಾಂಕ uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	ಅಕ್ಷರ callsign[k_Callsign_ಉದ್ದಳತೆ] {};

	ಗಾಗಿ (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		ಗಾಗಿ (size_t char_idx = 0; char_idx < ಗಾತ್ರ(uint32_t); ++char_idx) {
			ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಸರಿಸು = static_cast<ಚಿಹ್ನೆರಹಿತ>((ಗಾತ್ರ(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * ಗಾತ್ರ(uint32_t) + char_idx] = static_cast<ಅಕ್ಷರ>((parts[part_idx] >> ಸರಿಸು) & 0xffu);
		}
	}

	ಹಿಂತಿರುಗಿ callsign_ಗಮ್ಯ_uint64(callsign);
}

ಶೂನ್ಯ DaaEncodedId::convert_uint64_callsign_ಗಮ್ಯ_str(uint64_t value, ಅಕ್ಷರ callsign[k_Callsign_ಉದ್ದಳತೆ])
{
	ಆದರೆ (callsign == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	memset(callsign, 0, k_Callsign_ಉದ್ದಳತೆ);

	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ max_chars = k_Callsign_ಉದ್ದಳತೆ - 1;

	ಆದರೆ (value == 0) {
		memset(callsign, '0', max_chars);

	} ಇಲ್ಲದಿದ್ದರೆ {

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < max_chars; ++i) {
			ಅಕ್ಷರ c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			ಆದರೆ (c == 0) {
				ನಿಲ್ಲಿಸು;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::ಕೊನೆಯ_uas_id_ಬೈಟುಗಳು_ಗಮ್ಯ_uint64(ಸ್ಥಿರಾಂಕ uint8_t uas_id[k_Uas_Id_Byte_ಉದ್ದಳತೆ])
{
	ಆದರೆ (uas_id == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < k_Id_Encoding_Nb_ಬೈಟುಗಳು; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[k_Uas_Id_Byte_ಉದ್ದಳತೆ - k_Id_Encoding_Nb_ಬೈಟುಗಳು + i]) << (i * 8);
	}

	ಹಿಂತಿರುಗಿ uas_id_int;
}

ಶೂನ್ಯ DaaEncodedId::convert_uas_id_uint64_ಗಮ್ಯ_str(ಸ್ಥಿರಾಂಕ uint64_t uas_id_int, ಅಕ್ಷರ uas_id_char_arr[k_Utm_Guid_ಸಂದೇಶ_ಉದ್ದಳತೆ])
{
	ಆದರೆ (uas_id_char_arr == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಸ್ಥಿರ constexpr ಅಕ್ಷರ kHexDigits[] = "0123456789abcdef";

	ಆದರೆ (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_ಸಂದೇಶ_ಉದ್ದಳತೆ - 1);

	} ಇಲ್ಲದಿದ್ದರೆ {

		// Render the low reduced_uas_id_length bytes as hex.
		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ reduced_uas_id_ಉದ್ದಳತೆ = (k_Utm_Guid_ಸಂದೇಶ_ಉದ್ದಳತೆ - 1) / 2;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < reduced_uas_id_ಉದ್ದಳತೆ; ++i) {
			ಸ್ಥಿರಾಂಕ uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_ಸಂದೇಶ_ಉದ್ದಳತೆ - 1] = '\0';
}

ಶೂನ್ಯ DaaEncodedId::convert_icao_uint32_ಗಮ್ಯ_hex_str(uint64_t value, ಅಕ್ಷರ *ಬಫರ್_2, size_t ಬಫರ್_ಗಾತ್ರ)
{
	ಆದರೆ (ಬಫರ್_2 == ಶೂನ್ಯಸೂಚಕ || ಬಫರ್_ಗಾತ್ರ < k_Icao_ಉದ್ದಳತೆ) {
		ಹಿಂತಿರುಗಿ;
	}

	ಸ್ಥಿರಾಂಕ uint32_t icao_ವಿಳಾಸ = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(ಬಫರ್_2, ಬಫರ್_ಗಾತ್ರ, "%06" PRIX32, icao_ವಿಳಾಸ);
}
