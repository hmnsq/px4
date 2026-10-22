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

DaaEncodedId DaaEncodedId::from_report(ಧ್ರುವ transponder_report_s &report)
{
	ಯದಿ (report.icao_address > 0 && report.icao_address <= kMaxIcaoAddress) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		ಪ್ರತ್ಯಾಗಮ {
			static_cast<uint64_t>(report.icao_address),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	ಯದಿ (report.icao_address > kMaxIcaoAddress) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	ಯದಿ (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		ಧ್ರುವ uint64_t callsign_id = callsign_to_uint64(report.callsign);

		ಯದಿ (callsign_id != 0) {
			ಪ್ರತ್ಯಾಗಮ {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	ತಾರ್ಕಿಕ uas_id_valid = ಅಸತ್ಯ;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < kUasIdByteLength; ++i) {
		ಯದಿ (report.uas_id[i] != 0) {
			uas_id_valid = ಸತ್ಯ;
			ಭಙ್ಗ;
		}
	}

	ಯದಿ (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		ಧ್ರುವ uint64_t uas_id = last_uas_id_bytes_to_uint64(report.uas_id);

		ಯದಿ (uas_id != 0) {
			ಪ್ರತ್ಯಾಗಮ {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	ಪ್ರತ್ಯಾಗಮ {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(ಧ್ರುವ transponder_report_s &report,
		ಧ್ರುವ daa_ownship_ids_s &ownship_ids)
{
	ಧ್ರುವ DaaEncodedId encoded_id = from_report(report);

	ಯದಿ (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		ಪ್ರತ್ಯಾಗಮ {};
	}

	ಯದಿ (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		ಪ್ರತ್ಯಾಗಮ {};
	}

	ಪ್ರತ್ಯಾಗಮ encoded_id;
}

ತಾರ್ಕಿಕ DaaEncodedId::is_self_detection(ಧ್ರುವ DaaEncodedId &encoded_id, ಧ್ರುವ daa_ownship_ids_s &ownship_ids)
{
	ವಿಕಲ್ಪ (encoded_id.encoding) {
	ಪಕ್ಷ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			ಯದಿ (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
			}

			ಯದಿ (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
			}

			ಭಙ್ಗ;
		}

	ಪಕ್ಷ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			ಯದಿ (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
			}

			ಭಙ್ಗ;
		}

	ಪಕ್ಷ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			ಯದಿ (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
			}

			ಭಙ್ಗ;
		}

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ DaaEncodedId::to_string(ವರ್ಣ *buffer, size_t buffer_size) ಧ್ರುವ
{
	ಯದಿ (buffer == nullptr || buffer_size == 0) { ಪ್ರತ್ಯಾಗಮ; }

	memset(buffer, 0, buffer_size);

	ವಿಕಲ್ಪ (encoding) {
	ಪಕ್ಷ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_to_hex_str(id, buffer, buffer_size);
			ಭಙ್ಗ;
		}

	ಪಕ್ಷ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			ವರ್ಣ callsign[kCallsignLength] {};
			convert_uint64_callsign_to_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			ಪ್ರತಿ (ವರ್ಣ &character : callsign) {
				ಧ್ರುವ uint8_t byte = static_cast<uint8_t>(character);

				ಯದಿ (byte == 0) {
					ಭಙ್ಗ;
				}

				ಯದಿ (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(buffer, buffer_size, "%s", callsign);
			ಭಙ್ಗ;
		}

	ಪಕ್ಷ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			ವರ್ಣ uas_id[k_Utm_Guid_ಸಂದೇಶ_Length] {};
			convert_uas_id_uint64_to_str(id, uas_id);
			snprintf(buffer, buffer_size, "%s", uas_id);
			ಭಙ್ಗ;
		}

	ಸ್ವಭಾವ:
		snprintf(buffer, buffer_size, "Unknown ID.");
		ಭಙ್ಗ;
	}
}

uint64_t DaaEncodedId::callsign_to_uint64(ಧ್ರುವ ವರ್ಣ callsign[kCallsignLength])
{
	ಯದಿ (callsign == nullptr) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ತಾರ್ಕಿಕ null_terminated = ಅಸತ್ಯ;

	ಪ್ರತಿ (size_t i = 0; i < kCallsignLength; ++i) {
		ಯದಿ (callsign[i] == '\0') {
			null_terminated = ಸತ್ಯ;
			ಭಙ್ಗ;
		}
	}

	ಯದಿ (!null_terminated) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	uint64_t result = 0;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < kIdEncodingNbBytes && callsign[i] != '\0'; ++i) {
		result |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

uint64_t DaaEncodedId::callsign_params_to_uint64(ಧ್ರುವ int32_t callsign_part1, ಧ್ರುವ int32_t callsign_part2)
{
	ಧ್ರುವ uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	ವರ್ಣ callsign[kCallsignLength] {};

	ಪ್ರತಿ (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		ಪ್ರತಿ (size_t char_idx = 0; char_idx < ಪರಿಮಾಣ(uint32_t); ++char_idx) {
			ಧ್ರುವ unsigned shift = static_cast<unsigned>((ಪರಿಮಾಣ(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * ಪರಿಮಾಣ(uint32_t) + char_idx] = static_cast<ವರ್ಣ>((parts[part_idx] >> shift) & 0xffu);
		}
	}

	ಪ್ರತ್ಯಾಗಮ callsign_to_uint64(callsign);
}

ಶೂನ್ಯ DaaEncodedId::convert_uint64_callsign_to_str(uint64_t value, ವರ್ಣ callsign[kCallsignLength])
{
	ಯದಿ (callsign == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	memset(callsign, 0, kCallsignLength);

	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ max_chars = kCallsignLength - 1;

	ಯದಿ (value == 0) {
		memset(callsign, '0', max_chars);

	} ಅನ್ಯಥಾ {

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < max_chars; ++i) {
			ವರ್ಣ c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			ಯದಿ (c == 0) {
				ಭಙ್ಗ;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::last_uas_id_bytes_to_uint64(ಧ್ರುವ uint8_t uas_id[kUasIdByteLength])
{
	ಯದಿ (uas_id == nullptr) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < kIdEncodingNbBytes; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[kUasIdByteLength - kIdEncodingNbBytes + i]) << (i * 8);
	}

	ಪ್ರತ್ಯಾಗಮ uas_id_int;
}

ಶೂನ್ಯ DaaEncodedId::convert_uas_id_uint64_to_str(ಧ್ರುವ uint64_t uas_id_int, ವರ್ಣ uas_id_char_arr[k_Utm_Guid_ಸಂದೇಶ_Length])
{
	ಯದಿ (uas_id_char_arr == nullptr) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಸ್ಥಿರ constexpr ವರ್ಣ kHexDigits[] = "0123456789abcdef";

	ಯದಿ (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_ಸಂದೇಶ_Length - 1);

	} ಅನ್ಯಥಾ {

		// Render the low reduced_uas_id_length bytes as hex.
		ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ reduced_uas_id_length = (k_Utm_Guid_ಸಂದೇಶ_Length - 1) / 2;

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < reduced_uas_id_length; ++i) {
			ಧ್ರುವ uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_ಸಂದೇಶ_Length - 1] = '\0';
}

ಶೂನ್ಯ DaaEncodedId::convert_icao_uint32_to_hex_str(uint64_t value, ವರ್ಣ *buffer, size_t buffer_size)
{
	ಯದಿ (buffer == nullptr || buffer_size < kIcaoLength) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಧ್ರುವ uint32_t icao_address = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(buffer, buffer_size, "%06" PRIX32, icao_address);
}
