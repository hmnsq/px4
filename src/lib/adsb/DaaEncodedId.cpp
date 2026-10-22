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

#அந்தர்பாவ "DaaEncodedId.h"

#அந்தர்பாவ <cinttypes>
#அந்தர்பாவ <cstdio>
#அந்தர்பாவ <cstring>

#அந்தர்பாவ <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(மாறிலி transponder_report_s &report)
{
	எனில் (report.icao_address > 0 && report.icao_address <= kMaxIcaoAddress) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		திரும்பு {
			static_cast<uint64_t>(report.icao_address),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	எனில் (report.icao_address > kMaxIcaoAddress) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	எனில் (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		மாறிலி uint64_t callsign_id = callsign_to_uint64(report.callsign);

		எனில் (callsign_id != 0) {
			திரும்பு {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	தருக்க uas_id_valid = பொய்;

	க்கு (முழு i = 0; i < kUasIdByteLength; ++i) {
		எனில் (report.uas_id[i] != 0) {
			uas_id_valid = உண்மை;
			நிறுத்து;
		}
	}

	எனில் (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		மாறிலி uint64_t uas_id = last_uas_id_bytes_to_uint64(report.uas_id);

		எனில் (uas_id != 0) {
			திரும்பு {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	திரும்பு {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(மாறிலி transponder_report_s &report,
		மாறிலி daa_ownship_ids_s &ownship_ids)
{
	மாறிலி DaaEncodedId encoded_id = from_report(report);

	எனில் (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		திரும்பு {};
	}

	எனில் (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		திரும்பு {};
	}

	திரும்பு encoded_id;
}

தருக்க DaaEncodedId::is_self_detection(மாறிலி DaaEncodedId &encoded_id, மாறிலி daa_ownship_ids_s &ownship_ids)
{
	தேர்வு (encoded_id.encoding) {
	நிலை detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			எனில் (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				திரும்பு உண்மை;
			}

			எனில் (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				திரும்பு உண்மை;
			}

			நிறுத்து;
		}

	நிலை detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			எனில் (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				திரும்பு உண்மை;
			}

			நிறுத்து;
		}

	நிலை detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			எனில் (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				திரும்பு உண்மை;
			}

			நிறுத்து;
		}

	இயல்பு:
		நிறுத்து;
	}

	திரும்பு பொய்;
}

வெறுமை DaaEncodedId::to_string(எழுத்து *buffer, size_t buffer_size) மாறிலி
{
	எனில் (buffer == nullptr || buffer_size == 0) { திரும்பு; }

	memset(buffer, 0, buffer_size);

	தேர்வு (encoding) {
	நிலை detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_to_hex_str(id, buffer, buffer_size);
			நிறுத்து;
		}

	நிலை detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			எழுத்து callsign[kCallsignLength] {};
			convert_uint64_callsign_to_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			க்கு (எழுத்து &character : callsign) {
				மாறிலி uint8_t byte = static_cast<uint8_t>(character);

				எனில் (byte == 0) {
					நிறுத்து;
				}

				எனில் (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(buffer, buffer_size, "%s", callsign);
			நிறுத்து;
		}

	நிலை detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			எழுத்து uas_id[k_Utm_Guid_செய்தி_Length] {};
			convert_uas_id_uint64_to_str(id, uas_id);
			snprintf(buffer, buffer_size, "%s", uas_id);
			நிறுத்து;
		}

	இயல்பு:
		snprintf(buffer, buffer_size, "Unknown ID.");
		நிறுத்து;
	}
}

uint64_t DaaEncodedId::callsign_to_uint64(மாறிலி எழுத்து callsign[kCallsignLength])
{
	எனில் (callsign == nullptr) {
		திரும்பு 0;
	}

	தருக்க null_terminated = பொய்;

	க்கு (size_t i = 0; i < kCallsignLength; ++i) {
		எனில் (callsign[i] == '\0') {
			null_terminated = உண்மை;
			நிறுத்து;
		}
	}

	எனில் (!null_terminated) {
		திரும்பு 0;
	}

	uint64_t result = 0;

	க்கு (முழு i = 0; i < kIdEncodingNbBytes && callsign[i] != '\0'; ++i) {
		result |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	திரும்பு result;
}

uint64_t DaaEncodedId::callsign_params_to_uint64(மாறிலி int32_t callsign_part1, மாறிலி int32_t callsign_part2)
{
	மாறிலி uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	எழுத்து callsign[kCallsignLength] {};

	க்கு (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		க்கு (size_t char_idx = 0; char_idx < அளவு(uint32_t); ++char_idx) {
			மாறிலி unsigned shift = static_cast<unsigned>((அளவு(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * அளவு(uint32_t) + char_idx] = static_cast<எழுத்து>((parts[part_idx] >> shift) & 0xffu);
		}
	}

	திரும்பு callsign_to_uint64(callsign);
}

வெறுமை DaaEncodedId::convert_uint64_callsign_to_str(uint64_t value, எழுத்து callsign[kCallsignLength])
{
	எனில் (callsign == nullptr) {
		திரும்பு;
	}

	memset(callsign, 0, kCallsignLength);

	மாறிலி முழு max_chars = kCallsignLength - 1;

	எனில் (value == 0) {
		memset(callsign, '0', max_chars);

	} இல்லையெனில் {

		க்கு (முழு i = 0; i < max_chars; ++i) {
			எழுத்து c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			எனில் (c == 0) {
				நிறுத்து;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::last_uas_id_bytes_to_uint64(மாறிலி uint8_t uas_id[kUasIdByteLength])
{
	எனில் (uas_id == nullptr) {
		திரும்பு 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	க்கு (முழு i = 0; i < kIdEncodingNbBytes; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[kUasIdByteLength - kIdEncodingNbBytes + i]) << (i * 8);
	}

	திரும்பு uas_id_int;
}

வெறுமை DaaEncodedId::convert_uas_id_uint64_to_str(மாறிலி uint64_t uas_id_int, எழுத்து uas_id_char_arr[k_Utm_Guid_செய்தி_Length])
{
	எனில் (uas_id_char_arr == nullptr) {
		திரும்பு;
	}

	நிலையான constexpr எழுத்து kHexDigits[] = "0123456789abcdef";

	எனில் (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_செய்தி_Length - 1);

	} இல்லையெனில் {

		// Render the low reduced_uas_id_length bytes as hex.
		மாறிலி முழு reduced_uas_id_length = (k_Utm_Guid_செய்தி_Length - 1) / 2;

		க்கு (முழு i = 0; i < reduced_uas_id_length; ++i) {
			மாறிலி uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_செய்தி_Length - 1] = '\0';
}

வெறுமை DaaEncodedId::convert_icao_uint32_to_hex_str(uint64_t value, எழுத்து *buffer, size_t buffer_size)
{
	எனில் (buffer == nullptr || buffer_size < kIcaoLength) {
		திரும்பு;
	}

	மாறிலி uint32_t icao_address = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(buffer, buffer_size, "%06" PRIX32, icao_address);
}
