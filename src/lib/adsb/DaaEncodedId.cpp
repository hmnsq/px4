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

#ضمن "DaaEncodedId.h"

#ضمن <cinttypes>
#ضمن <cstdio>
#ضمن <cstring>

#ضمن <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(ثابت transponder_report_s &report)
{
	ܐܢ (report.icao_address > 0 && report.icao_address <= kMaxIcaoAddress) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		ܦܢܐ {
			static_cast<uint64_t>(report.icao_address),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	ܐܢ (report.icao_address > kMaxIcaoAddress) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	ܐܢ (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		ثابت uint64_t callsign_id = callsign_to_uint64(report.callsign);

		ܐܢ (callsign_id != 0) {
			ܦܢܐ {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	منطقي uas_id_valid = ܕܓܠܐ;

	لكل (صحيح i = 0; i < kUasIdByteLength; ++i) {
		ܐܢ (report.uas_id[i] != 0) {
			uas_id_valid = ܫܪܪܐ;
			اكسر;
		}
	}

	ܐܢ (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		ثابت uint64_t uas_id = last_uas_id_bytes_to_uint64(report.uas_id);

		ܐܢ (uas_id != 0) {
			ܦܢܐ {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	ܦܢܐ {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(ثابت transponder_report_s &report,
		ثابت daa_ownship_ids_s &ownship_ids)
{
	ثابت DaaEncodedId encoded_id = from_report(report);

	ܐܢ (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		ܦܢܐ {};
	}

	ܐܢ (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		ܦܢܐ {};
	}

	ܦܢܐ encoded_id;
}

منطقي DaaEncodedId::is_self_detection(ثابت DaaEncodedId &encoded_id, ثابت daa_ownship_ids_s &ownship_ids)
{
	بدل (encoded_id.encoding) {
	حالة detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			ܐܢ (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				ܦܢܐ ܫܪܪܐ;
			}

			ܐܢ (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				ܦܢܐ ܫܪܪܐ;
			}

			اكسر;
		}

	حالة detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			ܐܢ (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				ܦܢܐ ܫܪܪܐ;
			}

			اكسر;
		}

	حالة detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			ܐܢ (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				ܦܢܐ ܫܪܪܐ;
			}

			اكسر;
		}

	افتراضي:
		اكسر;
	}

	ܦܢܐ ܕܓܠܐ;
}

فراغ DaaEncodedId::to_string(حرف *buffer, size_t buffer_size) ثابت
{
	ܐܢ (buffer == nullptr || buffer_size == 0) { ܦܢܐ; }

	memset(buffer, 0, buffer_size);

	بدل (encoding) {
	حالة detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_to_hex_str(id, buffer, buffer_size);
			اكسر;
		}

	حالة detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			حرف callsign[kCallsignLength] {};
			convert_uint64_callsign_to_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			لكل (حرف &character : callsign) {
				ثابت uint8_t byte = static_cast<uint8_t>(character);

				ܐܢ (byte == 0) {
					اكسر;
				}

				ܐܢ (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(buffer, buffer_size, "%s", callsign);
			اكسر;
		}

	حالة detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			حرف uas_id[kUtmGuidMsgLength] {};
			convert_uas_id_uint64_to_str(id, uas_id);
			snprintf(buffer, buffer_size, "%s", uas_id);
			اكسر;
		}

	افتراضي:
		snprintf(buffer, buffer_size, "Unknown ID.");
		اكسر;
	}
}

uint64_t DaaEncodedId::callsign_to_uint64(ثابت حرف callsign[kCallsignLength])
{
	ܐܢ (callsign == nullptr) {
		ܦܢܐ 0;
	}

	منطقي null_terminated = ܕܓܠܐ;

	لكل (size_t i = 0; i < kCallsignLength; ++i) {
		ܐܢ (callsign[i] == '\0') {
			null_terminated = ܫܪܪܐ;
			اكسر;
		}
	}

	ܐܢ (!null_terminated) {
		ܦܢܐ 0;
	}

	uint64_t result = 0;

	لكل (صحيح i = 0; i < kIdEncodingNbBytes && callsign[i] != '\0'; ++i) {
		result |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	ܦܢܐ result;
}

uint64_t DaaEncodedId::callsign_params_to_uint64(ثابت int32_t callsign_part1, ثابت int32_t callsign_part2)
{
	ثابت uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	حرف callsign[kCallsignLength] {};

	لكل (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		لكل (size_t char_idx = 0; char_idx < حجم(uint32_t); ++char_idx) {
			ثابت unsigned shift = static_cast<unsigned>((حجم(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * حجم(uint32_t) + char_idx] = static_cast<حرف>((parts[part_idx] >> shift) & 0xffu);
		}
	}

	ܦܢܐ callsign_to_uint64(callsign);
}

فراغ DaaEncodedId::convert_uint64_callsign_to_str(uint64_t value, حرف callsign[kCallsignLength])
{
	ܐܢ (callsign == nullptr) {
		ܦܢܐ;
	}

	memset(callsign, 0, kCallsignLength);

	ثابت صحيح max_chars = kCallsignLength - 1;

	ܐܢ (value == 0) {
		memset(callsign, '0', max_chars);

	} ܐܚܪܢܐ {

		لكل (صحيح i = 0; i < max_chars; ++i) {
			حرف c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			ܐܢ (c == 0) {
				اكسر;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::last_uas_id_bytes_to_uint64(ثابت uint8_t uas_id[kUasIdByteLength])
{
	ܐܢ (uas_id == nullptr) {
		ܦܢܐ 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	لكل (صحيح i = 0; i < kIdEncodingNbBytes; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[kUasIdByteLength - kIdEncodingNbBytes + i]) << (i * 8);
	}

	ܦܢܐ uas_id_int;
}

فراغ DaaEncodedId::convert_uas_id_uint64_to_str(ثابت uint64_t uas_id_int, حرف uas_id_char_arr[kUtmGuidMsgLength])
{
	ܐܢ (uas_id_char_arr == nullptr) {
		ܦܢܐ;
	}

	ساكن constexpr حرف kHexDigits[] = "0123456789abcdef";

	ܐܢ (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', kUtmGuidMsgLength - 1);

	} ܐܚܪܢܐ {

		// Render the low reduced_uas_id_length bytes as hex.
		ثابت صحيح reduced_uas_id_length = (kUtmGuidMsgLength - 1) / 2;

		لكل (صحيح i = 0; i < reduced_uas_id_length; ++i) {
			ثابت uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[kUtmGuidMsgLength - 1] = '\0';
}

فراغ DaaEncodedId::convert_icao_uint32_to_hex_str(uint64_t value, حرف *buffer, size_t buffer_size)
{
	ܐܢ (buffer == nullptr || buffer_size < kIcaoLength) {
		ܦܢܐ;
	}

	ثابت uint32_t icao_address = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(buffer, buffer_size, "%06" PRIX32, icao_address);
}
