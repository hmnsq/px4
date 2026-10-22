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

#અન્તર્ભાવ "DaaEncodedId.h"

#અન્તર્ભાવ <cinttypes>
#અન્તર્ભાવ <cstdio>
#અન્તર્ભાવ <cstring>

#અન્તર્ભાવ <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(અચલ transponder_report_s &report)
{
	જો (report.icao_address > 0 && report.icao_address <= kMaxIcaoAddress) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		પાછા {
			static_cast<uint64_t>(report.icao_address),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	જો (report.icao_address > kMaxIcaoAddress) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	જો (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		અચલ uint64_t callsign_id = callsign_to_uint64(report.callsign);

		જો (callsign_id != 0) {
			પાછા {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	તાર્કિક uas_id_valid = ખોટું;

	માટે (પૂર્ણાંક i = 0; i < kUasIdByteLength; ++i) {
		જો (report.uas_id[i] != 0) {
			uas_id_valid = સાચું;
			તોડો;
		}
	}

	જો (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		અચલ uint64_t uas_id = last_uas_id_bytes_to_uint64(report.uas_id);

		જો (uas_id != 0) {
			પાછા {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	પાછા {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(અચલ transponder_report_s &report,
		અચલ daa_ownship_ids_s &ownship_ids)
{
	અચલ DaaEncodedId encoded_id = from_report(report);

	જો (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		પાછા {};
	}

	જો (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		પાછા {};
	}

	પાછા encoded_id;
}

તાર્કિક DaaEncodedId::is_self_detection(અચલ DaaEncodedId &encoded_id, અચલ daa_ownship_ids_s &ownship_ids)
{
	પસંદગી (encoded_id.encoding) {
	સ્થિતિ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			જો (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				પાછા સાચું;
			}

			જો (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				પાછા સાચું;
			}

			તોડો;
		}

	સ્થિતિ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			જો (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				પાછા સાચું;
			}

			તોડો;
		}

	સ્થિતિ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			જો (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				પાછા સાચું;
			}

			તોડો;
		}

	મૂળભૂત:
		તોડો;
	}

	પાછા ખોટું;
}

શૂન્ય DaaEncodedId::to_string(અક્ષર *buffer, size_t buffer_size) અચલ
{
	જો (buffer == nullptr || buffer_size == 0) { પાછા; }

	memset(buffer, 0, buffer_size);

	પસંદગી (encoding) {
	સ્થિતિ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_to_hex_str(id, buffer, buffer_size);
			તોડો;
		}

	સ્થિતિ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			અક્ષર callsign[kCallsignLength] {};
			convert_uint64_callsign_to_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			માટે (અક્ષર &character : callsign) {
				અચલ uint8_t byte = static_cast<uint8_t>(character);

				જો (byte == 0) {
					તોડો;
				}

				જો (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(buffer, buffer_size, "%s", callsign);
			તોડો;
		}

	સ્થિતિ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			અક્ષર uas_id[k_Utm_Guid_સંદેશ_Length] {};
			convert_uas_id_uint64_to_str(id, uas_id);
			snprintf(buffer, buffer_size, "%s", uas_id);
			તોડો;
		}

	મૂળભૂત:
		snprintf(buffer, buffer_size, "Unknown ID.");
		તોડો;
	}
}

uint64_t DaaEncodedId::callsign_to_uint64(અચલ અક્ષર callsign[kCallsignLength])
{
	જો (callsign == nullptr) {
		પાછા 0;
	}

	તાર્કિક null_terminated = ખોટું;

	માટે (size_t i = 0; i < kCallsignLength; ++i) {
		જો (callsign[i] == '\0') {
			null_terminated = સાચું;
			તોડો;
		}
	}

	જો (!null_terminated) {
		પાછા 0;
	}

	uint64_t result = 0;

	માટે (પૂર્ણાંક i = 0; i < kIdEncodingNbBytes && callsign[i] != '\0'; ++i) {
		result |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	પાછા result;
}

uint64_t DaaEncodedId::callsign_params_to_uint64(અચલ int32_t callsign_part1, અચલ int32_t callsign_part2)
{
	અચલ uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	અક્ષર callsign[kCallsignLength] {};

	માટે (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		માટે (size_t char_idx = 0; char_idx < કદ(uint32_t); ++char_idx) {
			અચલ unsigned shift = static_cast<unsigned>((કદ(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * કદ(uint32_t) + char_idx] = static_cast<અક્ષર>((parts[part_idx] >> shift) & 0xffu);
		}
	}

	પાછા callsign_to_uint64(callsign);
}

શૂન્ય DaaEncodedId::convert_uint64_callsign_to_str(uint64_t value, અક્ષર callsign[kCallsignLength])
{
	જો (callsign == nullptr) {
		પાછા;
	}

	memset(callsign, 0, kCallsignLength);

	અચલ પૂર્ણાંક max_chars = kCallsignLength - 1;

	જો (value == 0) {
		memset(callsign, '0', max_chars);

	} નહીંતર {

		માટે (પૂર્ણાંક i = 0; i < max_chars; ++i) {
			અક્ષર c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			જો (c == 0) {
				તોડો;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::last_uas_id_bytes_to_uint64(અચલ uint8_t uas_id[kUasIdByteLength])
{
	જો (uas_id == nullptr) {
		પાછા 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	માટે (પૂર્ણાંક i = 0; i < kIdEncodingNbBytes; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[kUasIdByteLength - kIdEncodingNbBytes + i]) << (i * 8);
	}

	પાછા uas_id_int;
}

શૂન્ય DaaEncodedId::convert_uas_id_uint64_to_str(અચલ uint64_t uas_id_int, અક્ષર uas_id_char_arr[k_Utm_Guid_સંદેશ_Length])
{
	જો (uas_id_char_arr == nullptr) {
		પાછા;
	}

	સ્થિર constexpr અક્ષર kHexDigits[] = "0123456789abcdef";

	જો (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_સંદેશ_Length - 1);

	} નહીંતર {

		// Render the low reduced_uas_id_length bytes as hex.
		અચલ પૂર્ણાંક reduced_uas_id_length = (k_Utm_Guid_સંદેશ_Length - 1) / 2;

		માટે (પૂર્ણાંક i = 0; i < reduced_uas_id_length; ++i) {
			અચલ uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_સંદેશ_Length - 1] = '\0';
}

શૂન્ય DaaEncodedId::convert_icao_uint32_to_hex_str(uint64_t value, અક્ષર *buffer, size_t buffer_size)
{
	જો (buffer == nullptr || buffer_size < kIcaoLength) {
		પાછા;
	}

	અચલ uint32_t icao_address = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(buffer, buffer_size, "%06" PRIX32, icao_address);
}
