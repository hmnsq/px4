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

#অন্তর্ভাব "DaaEncodedId.h"

#অন্তর্ভাব <cinttypes>
#অন্তর্ভাব <cstdio>
#অন্তর্ভাব <cstring>

#অন্তর্ভাব <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(ধ্রুব transponder_report_s &report)
{
	যদি (report.icao_address > 0 && report.icao_address <= kMaxIcaoAddress) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		প্রত্যাগম {
			static_cast<uint64_t>(report.icao_address),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	যদি (report.icao_address > kMaxIcaoAddress) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	যদি (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		ধ্রুব uint64_t callsign_id = callsign_to_uint64(report.callsign);

		যদি (callsign_id != 0) {
			প্রত্যাগম {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	তার্কিক uas_id_valid = অসত্য;

	প্রতি (পূর্ণাঙ্ক i = 0; i < kUasIdByteLength; ++i) {
		যদি (report.uas_id[i] != 0) {
			uas_id_valid = সত্য;
			ভঙ্গ;
		}
	}

	যদি (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		ধ্রুব uint64_t uas_id = last_uas_id_bytes_to_uint64(report.uas_id);

		যদি (uas_id != 0) {
			প্রত্যাগম {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	প্রত্যাগম {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(ধ্রুব transponder_report_s &report,
		ধ্রুব daa_ownship_ids_s &ownship_ids)
{
	ধ্রুব DaaEncodedId encoded_id = from_report(report);

	যদি (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		প্রত্যাগম {};
	}

	যদি (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		প্রত্যাগম {};
	}

	প্রত্যাগম encoded_id;
}

তার্কিক DaaEncodedId::is_self_detection(ধ্রুব DaaEncodedId &encoded_id, ধ্রুব daa_ownship_ids_s &ownship_ids)
{
	বিকল্প (encoded_id.encoding) {
	পক্ষ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			যদি (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				প্রত্যাগম সত্য;
			}

			যদি (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				প্রত্যাগম সত্য;
			}

			ভঙ্গ;
		}

	পক্ষ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			যদি (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				প্রত্যাগম সত্য;
			}

			ভঙ্গ;
		}

	পক্ষ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			যদি (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				প্রত্যাগম সত্য;
			}

			ভঙ্গ;
		}

	স্বভাব:
		ভঙ্গ;
	}

	প্রত্যাগম অসত্য;
}

শূন্য DaaEncodedId::to_string(বর্ণ *buffer, size_t buffer_size) ধ্রুব
{
	যদি (buffer == nullptr || buffer_size == 0) { প্রত্যাগম; }

	memset(buffer, 0, buffer_size);

	বিকল্প (encoding) {
	পক্ষ detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_to_hex_str(id, buffer, buffer_size);
			ভঙ্গ;
		}

	পক্ষ detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			বর্ণ callsign[kCallsignLength] {};
			convert_uint64_callsign_to_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			প্রতি (বর্ণ &character : callsign) {
				ধ্রুব uint8_t byte = static_cast<uint8_t>(character);

				যদি (byte == 0) {
					ভঙ্গ;
				}

				যদি (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(buffer, buffer_size, "%s", callsign);
			ভঙ্গ;
		}

	পক্ষ detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			বর্ণ uas_id[k_Utm_Guid_বার্তা_Length] {};
			convert_uas_id_uint64_to_str(id, uas_id);
			snprintf(buffer, buffer_size, "%s", uas_id);
			ভঙ্গ;
		}

	স্বভাব:
		snprintf(buffer, buffer_size, "Unknown ID.");
		ভঙ্গ;
	}
}

uint64_t DaaEncodedId::callsign_to_uint64(ধ্রুব বর্ণ callsign[kCallsignLength])
{
	যদি (callsign == nullptr) {
		প্রত্যাগম 0;
	}

	তার্কিক null_terminated = অসত্য;

	প্রতি (size_t i = 0; i < kCallsignLength; ++i) {
		যদি (callsign[i] == '\0') {
			null_terminated = সত্য;
			ভঙ্গ;
		}
	}

	যদি (!null_terminated) {
		প্রত্যাগম 0;
	}

	uint64_t result = 0;

	প্রতি (পূর্ণাঙ্ক i = 0; i < kIdEncodingNbBytes && callsign[i] != '\0'; ++i) {
		result |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	প্রত্যাগম result;
}

uint64_t DaaEncodedId::callsign_params_to_uint64(ধ্রুব int32_t callsign_part1, ধ্রুব int32_t callsign_part2)
{
	ধ্রুব uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	বর্ণ callsign[kCallsignLength] {};

	প্রতি (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		প্রতি (size_t char_idx = 0; char_idx < পরিমাণ(uint32_t); ++char_idx) {
			ধ্রুব unsigned shift = static_cast<unsigned>((পরিমাণ(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * পরিমাণ(uint32_t) + char_idx] = static_cast<বর্ণ>((parts[part_idx] >> shift) & 0xffu);
		}
	}

	প্রত্যাগম callsign_to_uint64(callsign);
}

শূন্য DaaEncodedId::convert_uint64_callsign_to_str(uint64_t value, বর্ণ callsign[kCallsignLength])
{
	যদি (callsign == nullptr) {
		প্রত্যাগম;
	}

	memset(callsign, 0, kCallsignLength);

	ধ্রুব পূর্ণাঙ্ক max_chars = kCallsignLength - 1;

	যদি (value == 0) {
		memset(callsign, '0', max_chars);

	} অন্যথা {

		প্রতি (পূর্ণাঙ্ক i = 0; i < max_chars; ++i) {
			বর্ণ c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			যদি (c == 0) {
				ভঙ্গ;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::last_uas_id_bytes_to_uint64(ধ্রুব uint8_t uas_id[kUasIdByteLength])
{
	যদি (uas_id == nullptr) {
		প্রত্যাগম 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	প্রতি (পূর্ণাঙ্ক i = 0; i < kIdEncodingNbBytes; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[kUasIdByteLength - kIdEncodingNbBytes + i]) << (i * 8);
	}

	প্রত্যাগম uas_id_int;
}

শূন্য DaaEncodedId::convert_uas_id_uint64_to_str(ধ্রুব uint64_t uas_id_int, বর্ণ uas_id_char_arr[k_Utm_Guid_বার্তা_Length])
{
	যদি (uas_id_char_arr == nullptr) {
		প্রত্যাগম;
	}

	স্থির constexpr বর্ণ kHexDigits[] = "0123456789abcdef";

	যদি (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_বার্তা_Length - 1);

	} অন্যথা {

		// Render the low reduced_uas_id_length bytes as hex.
		ধ্রুব পূর্ণাঙ্ক reduced_uas_id_length = (k_Utm_Guid_বার্তা_Length - 1) / 2;

		প্রতি (পূর্ণাঙ্ক i = 0; i < reduced_uas_id_length; ++i) {
			ধ্রুব uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_বার্তা_Length - 1] = '\0';
}

শূন্য DaaEncodedId::convert_icao_uint32_to_hex_str(uint64_t value, বর্ণ *buffer, size_t buffer_size)
{
	যদি (buffer == nullptr || buffer_size < kIcaoLength) {
		প্রত্যাগম;
	}

	ধ্রুব uint32_t icao_address = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(buffer, buffer_size, "%06" PRIX32, icao_address);
}
