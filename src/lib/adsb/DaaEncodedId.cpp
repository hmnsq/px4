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

#അന്തര്ഭാവ "DaaEncodedId.h"

#അന്തര്ഭാവ <cinttypes>
#അന്തര്ഭാവ <cstdio>
#അന്തര്ഭാവ <cstring>

#അന്തര്ഭാവ <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(സ്ഥിരം transponder_report_s &report)
{
	എങ്കിൽ (report.icao_address > 0 && report.icao_address <= kMaxIcaoAddress) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		തിരികെ {
			static_cast<uint64_t>(report.icao_address),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	എങ്കിൽ (report.icao_address > kMaxIcaoAddress) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	എങ്കിൽ (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		സ്ഥിരം uint64_t callsign_id = callsign_to_uint64(report.callsign);

		എങ്കിൽ (callsign_id != 0) {
			തിരികെ {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	തർക്കം uas_id_valid = അസത്യം;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < kUasIdByteLength; ++i) {
		എങ്കിൽ (report.uas_id[i] != 0) {
			uas_id_valid = സത്യം;
			നിർത്തുക;
		}
	}

	എങ്കിൽ (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		സ്ഥിരം uint64_t uas_id = last_uas_id_bytes_to_uint64(report.uas_id);

		എങ്കിൽ (uas_id != 0) {
			തിരികെ {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	തിരികെ {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(സ്ഥിരം transponder_report_s &report,
		സ്ഥിരം daa_ownship_ids_s &ownship_ids)
{
	സ്ഥിരം DaaEncodedId encoded_id = from_report(report);

	എങ്കിൽ (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		തിരികെ {};
	}

	എങ്കിൽ (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		തിരികെ {};
	}

	തിരികെ encoded_id;
}

തർക്കം DaaEncodedId::is_self_detection(സ്ഥിരം DaaEncodedId &encoded_id, സ്ഥിരം daa_ownship_ids_s &ownship_ids)
{
	തിരഞ്ഞെടുപ്പ് (encoded_id.encoding) {
	അവസ്ഥ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			എങ്കിൽ (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				തിരികെ സത്യം;
			}

			എങ്കിൽ (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				തിരികെ സത്യം;
			}

			നിർത്തുക;
		}

	അവസ്ഥ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			എങ്കിൽ (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				തിരികെ സത്യം;
			}

			നിർത്തുക;
		}

	അവസ്ഥ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			എങ്കിൽ (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				തിരികെ സത്യം;
			}

			നിർത്തുക;
		}

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	തിരികെ അസത്യം;
}

ശൂന്യം DaaEncodedId::to_string(അക്ഷരം *buffer, size_t buffer_size) സ്ഥിരം
{
	എങ്കിൽ (buffer == nullptr || buffer_size == 0) { തിരികെ; }

	memset(buffer, 0, buffer_size);

	തിരഞ്ഞെടുപ്പ് (encoding) {
	അവസ്ഥ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_to_hex_str(id, buffer, buffer_size);
			നിർത്തുക;
		}

	അവസ്ഥ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			അക്ഷരം callsign[kCallsignLength] {};
			convert_uint64_callsign_to_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			വേണ്ടി (അക്ഷരം &character : callsign) {
				സ്ഥിരം uint8_t byte = static_cast<uint8_t>(character);

				എങ്കിൽ (byte == 0) {
					നിർത്തുക;
				}

				എങ്കിൽ (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(buffer, buffer_size, "%s", callsign);
			നിർത്തുക;
		}

	അവസ്ഥ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			അക്ഷരം uas_id[k_Utm_Guid_സന്ദേശം_Length] {};
			convert_uas_id_uint64_to_str(id, uas_id);
			snprintf(buffer, buffer_size, "%s", uas_id);
			നിർത്തുക;
		}

	സ്ഥിരസ്ഥിതി:
		snprintf(buffer, buffer_size, "Unknown ID.");
		നിർത്തുക;
	}
}

uint64_t DaaEncodedId::callsign_to_uint64(സ്ഥിരം അക്ഷരം callsign[kCallsignLength])
{
	എങ്കിൽ (callsign == nullptr) {
		തിരികെ 0;
	}

	തർക്കം null_terminated = അസത്യം;

	വേണ്ടി (size_t i = 0; i < kCallsignLength; ++i) {
		എങ്കിൽ (callsign[i] == '\0') {
			null_terminated = സത്യം;
			നിർത്തുക;
		}
	}

	എങ്കിൽ (!null_terminated) {
		തിരികെ 0;
	}

	uint64_t result = 0;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < kIdEncodingNbBytes && callsign[i] != '\0'; ++i) {
		result |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	തിരികെ result;
}

uint64_t DaaEncodedId::callsign_params_to_uint64(സ്ഥിരം int32_t callsign_part1, സ്ഥിരം int32_t callsign_part2)
{
	സ്ഥിരം uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	അക്ഷരം callsign[kCallsignLength] {};

	വേണ്ടി (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		വേണ്ടി (size_t char_idx = 0; char_idx < വലുപ്പം(uint32_t); ++char_idx) {
			സ്ഥിരം unsigned shift = static_cast<unsigned>((വലുപ്പം(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * വലുപ്പം(uint32_t) + char_idx] = static_cast<അക്ഷരം>((parts[part_idx] >> shift) & 0xffu);
		}
	}

	തിരികെ callsign_to_uint64(callsign);
}

ശൂന്യം DaaEncodedId::convert_uint64_callsign_to_str(uint64_t value, അക്ഷരം callsign[kCallsignLength])
{
	എങ്കിൽ (callsign == nullptr) {
		തിരികെ;
	}

	memset(callsign, 0, kCallsignLength);

	സ്ഥിരം പൂർണ്ണസംഖ്യ max_chars = kCallsignLength - 1;

	എങ്കിൽ (value == 0) {
		memset(callsign, '0', max_chars);

	} അല്ലെങ്കിൽ {

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < max_chars; ++i) {
			അക്ഷരം c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			എങ്കിൽ (c == 0) {
				നിർത്തുക;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::last_uas_id_bytes_to_uint64(സ്ഥിരം uint8_t uas_id[kUasIdByteLength])
{
	എങ്കിൽ (uas_id == nullptr) {
		തിരികെ 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < kIdEncodingNbBytes; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[kUasIdByteLength - kIdEncodingNbBytes + i]) << (i * 8);
	}

	തിരികെ uas_id_int;
}

ശൂന്യം DaaEncodedId::convert_uas_id_uint64_to_str(സ്ഥിരം uint64_t uas_id_int, അക്ഷരം uas_id_char_arr[k_Utm_Guid_സന്ദേശം_Length])
{
	എങ്കിൽ (uas_id_char_arr == nullptr) {
		തിരികെ;
	}

	സ്ഥിതം constexpr അക്ഷരം kHexDigits[] = "0123456789abcdef";

	എങ്കിൽ (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_സന്ദേശം_Length - 1);

	} അല്ലെങ്കിൽ {

		// Render the low reduced_uas_id_length bytes as hex.
		സ്ഥിരം പൂർണ്ണസംഖ്യ reduced_uas_id_length = (k_Utm_Guid_സന്ദേശം_Length - 1) / 2;

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < reduced_uas_id_length; ++i) {
			സ്ഥിരം uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_സന്ദേശം_Length - 1] = '\0';
}

ശൂന്യം DaaEncodedId::convert_icao_uint32_to_hex_str(uint64_t value, അക്ഷരം *buffer, size_t buffer_size)
{
	എങ്കിൽ (buffer == nullptr || buffer_size < kIcaoLength) {
		തിരികെ;
	}

	സ്ഥിരം uint32_t icao_address = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(buffer, buffer_size, "%06" PRIX32, icao_address);
}
