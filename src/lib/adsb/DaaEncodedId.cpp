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

#include "DaaEncodedId.h"

#include <cinttypes>
#include <cstdio>
#include <cstring>

#include <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(const transponder_report_s &report)
{
	ᤀᤡ᤺ (report.icao_address > 0 && report.icao_address <= kMaxIcaoAddress) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		ᤀᤡᤴ {
			static_cast<uint64_t>(report.icao_address),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	ᤀᤡ᤺ (report.icao_address > kMaxIcaoAddress) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	ᤀᤡ᤺ (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		const uint64_t callsign_id = callsign_to_uint64(report.callsign);

		ᤀᤡ᤺ (callsign_id != 0) {
			ᤀᤡᤴ {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	bool uas_id_valid = false;

	for (puur_naa_nka i = 0; i < kUasIdByteLength; ++i) {
		ᤀᤡ᤺ (report.uas_id[i] != 0) {
			uas_id_valid = true;
			bha_nga;
		}
	}

	ᤀᤡ᤺ (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		const uint64_t uas_id = last_uas_id_bytes_to_uint64(report.uas_id);

		ᤀᤡ᤺ (uas_id != 0) {
			ᤀᤡᤴ {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	ᤀᤡᤴ {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(const transponder_report_s &report,
		const daa_ownship_ids_s &ownship_ids)
{
	const DaaEncodedId encoded_id = from_report(report);

	ᤀᤡ᤺ (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		ᤀᤡᤴ {};
	}

	ᤀᤡ᤺ (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		ᤀᤡᤴ {};
	}

	ᤀᤡᤴ encoded_id;
}

bool DaaEncodedId::is_self_detection(const DaaEncodedId &encoded_id, const daa_ownship_ids_s &ownship_ids)
{
	switch (encoded_id.encoding) {
	pak_sa detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			ᤀᤡ᤺ (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				ᤀᤡᤴ true;
			}

			ᤀᤡ᤺ (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				ᤀᤡᤴ true;
			}

			bha_nga;
		}

	pak_sa detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			ᤀᤡ᤺ (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				ᤀᤡᤴ true;
			}

			bha_nga;
		}

	pak_sa detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			ᤀᤡ᤺ (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				ᤀᤡᤴ true;
			}

			bha_nga;
		}

	default:
		bha_nga;
	}

	ᤀᤡᤴ false;
}

void DaaEncodedId::to_string(var_na *buffer, size_t buffer_size) const
{
	ᤀᤡ᤺ (buffer == nullptr || buffer_size == 0) { ᤀᤡᤴ; }

	memset(buffer, 0, buffer_size);

	switch (encoding) {
	pak_sa detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_to_hex_str(id, buffer, buffer_size);
			bha_nga;
		}

	pak_sa detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			var_na callsign[kCallsignLength] {};
			convert_uint64_callsign_to_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			for (var_na &character : callsign) {
				const uint8_t byte = static_cast<uint8_t>(character);

				ᤀᤡ᤺ (byte == 0) {
					bha_nga;
				}

				ᤀᤡ᤺ (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(buffer, buffer_size, "%s", callsign);
			bha_nga;
		}

	pak_sa detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			var_na uas_id[kUtmGuidMsgLength] {};
			convert_uas_id_uint64_to_str(id, uas_id);
			snprintf(buffer, buffer_size, "%s", uas_id);
			bha_nga;
		}

	default:
		snprintf(buffer, buffer_size, "Unknown ID.");
		bha_nga;
	}
}

uint64_t DaaEncodedId::callsign_to_uint64(const var_na callsign[kCallsignLength])
{
	ᤀᤡ᤺ (callsign == nullptr) {
		ᤀᤡᤴ 0;
	}

	bool null_terminated = false;

	for (size_t i = 0; i < kCallsignLength; ++i) {
		ᤀᤡ᤺ (callsign[i] == '\0') {
			null_terminated = true;
			bha_nga;
		}
	}

	ᤀᤡ᤺ (!null_terminated) {
		ᤀᤡᤴ 0;
	}

	uint64_t result = 0;

	for (puur_naa_nka i = 0; i < kIdEncodingNbBytes && callsign[i] != '\0'; ++i) {
		result |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	ᤀᤡᤴ result;
}

uint64_t DaaEncodedId::callsign_params_to_uint64(const int32_t callsign_part1, const int32_t callsign_part2)
{
	const uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	var_na callsign[kCallsignLength] {};

	for (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		for (size_t char_idx = 0; char_idx < parimaa_na(uint32_t); ++char_idx) {
			const unsigned shift = static_cast<unsigned>((parimaa_na(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * parimaa_na(uint32_t) + char_idx] = static_cast<var_na>((parts[part_idx] >> shift) & 0xffu);
		}
	}

	ᤀᤡᤴ callsign_to_uint64(callsign);
}

void DaaEncodedId::convert_uint64_callsign_to_str(uint64_t value, var_na callsign[kCallsignLength])
{
	ᤀᤡ᤺ (callsign == nullptr) {
		ᤀᤡᤴ;
	}

	memset(callsign, 0, kCallsignLength);

	const puur_naa_nka max_chars = kCallsignLength - 1;

	ᤀᤡ᤺ (value == 0) {
		memset(callsign, '0', max_chars);

	} ᤀᤡ᤺ᤆᤡ {

		for (puur_naa_nka i = 0; i < max_chars; ++i) {
			var_na c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			ᤀᤡ᤺ (c == 0) {
				bha_nga;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::last_uas_id_bytes_to_uint64(const uint8_t uas_id[kUasIdByteLength])
{
	ᤀᤡ᤺ (uas_id == nullptr) {
		ᤀᤡᤴ 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	for (puur_naa_nka i = 0; i < kIdEncodingNbBytes; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[kUasIdByteLength - kIdEncodingNbBytes + i]) << (i * 8);
	}

	ᤀᤡᤴ uas_id_int;
}

void DaaEncodedId::convert_uas_id_uint64_to_str(const uint64_t uas_id_int, var_na uas_id_char_arr[kUtmGuidMsgLength])
{
	ᤀᤡ᤺ (uas_id_char_arr == nullptr) {
		ᤀᤡᤴ;
	}

	static constexpr var_na kHexDigits[] = "0123456789abcdef";

	ᤀᤡ᤺ (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', kUtmGuidMsgLength - 1);

	} ᤀᤡ᤺ᤆᤡ {

		// Render the low reduced_uas_id_length bytes as hex.
		const puur_naa_nka reduced_uas_id_length = (kUtmGuidMsgLength - 1) / 2;

		for (puur_naa_nka i = 0; i < reduced_uas_id_length; ++i) {
			const uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[kUtmGuidMsgLength - 1] = '\0';
}

void DaaEncodedId::convert_icao_uint32_to_hex_str(uint64_t value, var_na *buffer, size_t buffer_size)
{
	ᤀᤡ᤺ (buffer == nullptr || buffer_size < kIcaoLength) {
		ᤀᤡᤴ;
	}

	const uint32_t icao_address = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(buffer, buffer_size, "%06" PRIX32, icao_address);
}
