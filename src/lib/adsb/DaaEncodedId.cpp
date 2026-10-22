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

DaaEncodedId DaaEncodedId::from_report(ধ্রুবক transponder_report_s &report)
{
	যদি (report.icao_address > 0 && report.icao_address <= kMaxIcaoAddress) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		ফেরত {
			static_cast<uint64_t>(report.icao_address),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	যদি (report.icao_address > kMaxIcaoAddress) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	যদি (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		ধ্রুবক uint64_t callsign_id = callsign_to_uint64(report.callsign);

		যদি (callsign_id != 0) {
			ফেরত {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	যৌক্তিক uas_id_valid = মিথ্যা;

	জন্য (পূর্ণসংখ্যা i = 0; i < k_Uas_Id_Byte_দৈর্ঘ্য; ++i) {
		যদি (report.uas_id[i] != 0) {
			uas_id_valid = সত্য;
			বিরতি;
		}
	}

	যদি (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		ধ্রুবক uint64_t uas_id = last_uas_id_bytes_to_uint64(report.uas_id);

		যদি (uas_id != 0) {
			ফেরত {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	ফেরত {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(ধ্রুবক transponder_report_s &report,
		ধ্রুবক daa_ownship_ids_s &ownship_ids)
{
	ধ্রুবক DaaEncodedId encoded_id = from_report(report);

	যদি (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		ফেরত {};
	}

	যদি (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		ফেরত {};
	}

	ফেরত encoded_id;
}

যৌক্তিক DaaEncodedId::is_self_detection(ধ্রুবক DaaEncodedId &encoded_id, ধ্রুবক daa_ownship_ids_s &ownship_ids)
{
	নির্বাচন (encoded_id.encoding) {
	ক্ষেত্র detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			যদি (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				ফেরত সত্য;
			}

			যদি (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				ফেরত সত্য;
			}

			বিরতি;
		}

	ক্ষেত্র detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			যদি (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				ফেরত সত্য;
			}

			বিরতি;
		}

	ক্ষেত্র detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			যদি (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				ফেরত সত্য;
			}

			বিরতি;
		}

	পূর্বনির্ধারিত:
		বিরতি;
	}

	ফেরত মিথ্যা;
}

শূন্য DaaEncodedId::to_string(অক্ষর *বাফার_2, size_t বাফার_আকার) ধ্রুবক
{
	যদি (বাফার_2 == nullptr || বাফার_আকার == 0) { ফেরত; }

	memset(বাফার_2, 0, বাফার_আকার);

	নির্বাচন (encoding) {
	ক্ষেত্র detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_to_hex_str(id, বাফার_2, বাফার_আকার);
			বিরতি;
		}

	ক্ষেত্র detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			অক্ষর callsign[k_Callsign_দৈর্ঘ্য] {};
			convert_uint64_callsign_to_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			জন্য (অক্ষর &character : callsign) {
				ধ্রুবক uint8_t byte = static_cast<uint8_t>(character);

				যদি (byte == 0) {
					বিরতি;
				}

				যদি (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(বাফার_2, বাফার_আকার, "%s", callsign);
			বিরতি;
		}

	ক্ষেত্র detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			অক্ষর uas_id[k_Utm_Guid_বার্তা_দৈর্ঘ্য] {};
			convert_uas_id_uint64_to_str(id, uas_id);
			snprintf(বাফার_2, বাফার_আকার, "%s", uas_id);
			বিরতি;
		}

	পূর্বনির্ধারিত:
		snprintf(বাফার_2, বাফার_আকার, "Unknown ID.");
		বিরতি;
	}
}

uint64_t DaaEncodedId::callsign_to_uint64(ধ্রুবক অক্ষর callsign[k_Callsign_দৈর্ঘ্য])
{
	যদি (callsign == nullptr) {
		ফেরত 0;
	}

	যৌক্তিক null_terminated = মিথ্যা;

	জন্য (size_t i = 0; i < k_Callsign_দৈর্ঘ্য; ++i) {
		যদি (callsign[i] == '\0') {
			null_terminated = সত্য;
			বিরতি;
		}
	}

	যদি (!null_terminated) {
		ফেরত 0;
	}

	uint64_t ফলাফল = 0;

	জন্য (পূর্ণসংখ্যা i = 0; i < kIdEncodingNbBytes && callsign[i] != '\0'; ++i) {
		ফলাফল |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	ফেরত ফলাফল;
}

uint64_t DaaEncodedId::callsign_params_to_uint64(ধ্রুবক int32_t callsign_part1, ধ্রুবক int32_t callsign_part2)
{
	ধ্রুবক uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	অক্ষর callsign[k_Callsign_দৈর্ঘ্য] {};

	জন্য (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		জন্য (size_t char_idx = 0; char_idx < আকার(uint32_t); ++char_idx) {
			ধ্রুবক unsigned shift = static_cast<unsigned>((আকার(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * আকার(uint32_t) + char_idx] = static_cast<অক্ষর>((parts[part_idx] >> shift) & 0xffu);
		}
	}

	ফেরত callsign_to_uint64(callsign);
}

শূন্য DaaEncodedId::convert_uint64_callsign_to_str(uint64_t value, অক্ষর callsign[k_Callsign_দৈর্ঘ্য])
{
	যদি (callsign == nullptr) {
		ফেরত;
	}

	memset(callsign, 0, k_Callsign_দৈর্ঘ্য);

	ধ্রুবক পূর্ণসংখ্যা max_chars = k_Callsign_দৈর্ঘ্য - 1;

	যদি (value == 0) {
		memset(callsign, '0', max_chars);

	} অন্যথা {

		জন্য (পূর্ণসংখ্যা i = 0; i < max_chars; ++i) {
			অক্ষর c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			যদি (c == 0) {
				বিরতি;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::last_uas_id_bytes_to_uint64(ধ্রুবক uint8_t uas_id[k_Uas_Id_Byte_দৈর্ঘ্য])
{
	যদি (uas_id == nullptr) {
		ফেরত 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	জন্য (পূর্ণসংখ্যা i = 0; i < kIdEncodingNbBytes; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[k_Uas_Id_Byte_দৈর্ঘ্য - kIdEncodingNbBytes + i]) << (i * 8);
	}

	ফেরত uas_id_int;
}

শূন্য DaaEncodedId::convert_uas_id_uint64_to_str(ধ্রুবক uint64_t uas_id_int, অক্ষর uas_id_char_arr[k_Utm_Guid_বার্তা_দৈর্ঘ্য])
{
	যদি (uas_id_char_arr == nullptr) {
		ফেরত;
	}

	স্থির constexpr অক্ষর kHexDigits[] = "0123456789abcdef";

	যদি (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_বার্তা_দৈর্ঘ্য - 1);

	} অন্যথা {

		// Render the low reduced_uas_id_length bytes as hex.
		ধ্রুবক পূর্ণসংখ্যা reduced_uas_id_দৈর্ঘ্য = (k_Utm_Guid_বার্তা_দৈর্ঘ্য - 1) / 2;

		জন্য (পূর্ণসংখ্যা i = 0; i < reduced_uas_id_দৈর্ঘ্য; ++i) {
			ধ্রুবক uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_বার্তা_দৈর্ঘ্য - 1] = '\0';
}

শূন্য DaaEncodedId::convert_icao_uint32_to_hex_str(uint64_t value, অক্ষর *বাফার_2, size_t বাফার_আকার)
{
	যদি (বাফার_2 == nullptr || বাফার_আকার < k_Icao_দৈর্ঘ্য) {
		ফেরত;
	}

	ধ্রুবক uint32_t icao_address = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(বাফার_2, বাফার_আকার, "%06" PRIX32, icao_address);
}
