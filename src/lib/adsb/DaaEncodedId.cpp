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

#شامل "DaaEncodedId.h"

#شامل <cinttypes>
#شامل <cstdio>
#شامل <cstring>

#شامل <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(ثابت transponder_report_s &report)
{
	اگر (report.icao_address > 0 && report.icao_address <= kMaxIcaoAddress) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		بازگشت {
			static_cast<uint64_t>(report.icao_address),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	اگر (report.icao_address > kMaxIcaoAddress) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	اگر (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		ثابت uint64_t callsign_id = callsign_to_uint64(report.callsign);

		اگر (callsign_id != 0) {
			بازگشت {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	منطقی uas_id_valid = نادرست;

	برای (صحیح i = 0; i < kUasIdByteLength; ++i) {
		اگر (report.uas_id[i] != 0) {
			uas_id_valid = درست;
			بشکن;
		}
	}

	اگر (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		ثابت uint64_t uas_id = last_uas_id_bytes_to_uint64(report.uas_id);

		اگر (uas_id != 0) {
			بازگشت {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	بازگشت {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(ثابت transponder_report_s &report,
		ثابت daa_ownship_ids_s &ownship_ids)
{
	ثابت DaaEncodedId encoded_id = from_report(report);

	اگر (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		بازگشت {};
	}

	اگر (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		بازگشت {};
	}

	بازگشت encoded_id;
}

منطقی DaaEncodedId::is_self_detection(ثابت DaaEncodedId &encoded_id, ثابت daa_ownship_ids_s &ownship_ids)
{
	انتخاب (encoded_id.encoding) {
	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			اگر (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				بازگشت درست;
			}

			اگر (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				بازگشت درست;
			}

			بشکن;
		}

	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			اگر (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				بازگشت درست;
			}

			بشکن;
		}

	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			اگر (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				بازگشت درست;
			}

			بشکن;
		}

	پیشفرض:
		بشکن;
	}

	بازگشت نادرست;
}

تهی DaaEncodedId::to_string(نویسه *buffer, size_t buffer_size) ثابت
{
	اگر (buffer == nullptr || buffer_size == 0) { بازگشت; }

	memset(buffer, 0, buffer_size);

	انتخاب (encoding) {
	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_to_hex_str(id, buffer, buffer_size);
			بشکن;
		}

	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			نویسه callsign[kCallsignLength] {};
			convert_uint64_callsign_to_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			برای (نویسه &character : callsign) {
				ثابت uint8_t byte = static_cast<uint8_t>(character);

				اگر (byte == 0) {
					بشکن;
				}

				اگر (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(buffer, buffer_size, "%s", callsign);
			بشکن;
		}

	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			نویسه uas_id[k_Utm_Guid_پیام_Length] {};
			convert_uas_id_uint64_to_str(id, uas_id);
			snprintf(buffer, buffer_size, "%s", uas_id);
			بشکن;
		}

	پیشفرض:
		snprintf(buffer, buffer_size, "Unknown ID.");
		بشکن;
	}
}

uint64_t DaaEncodedId::callsign_to_uint64(ثابت نویسه callsign[kCallsignLength])
{
	اگر (callsign == nullptr) {
		بازگشت 0;
	}

	منطقی null_terminated = نادرست;

	برای (size_t i = 0; i < kCallsignLength; ++i) {
		اگر (callsign[i] == '\0') {
			null_terminated = درست;
			بشکن;
		}
	}

	اگر (!null_terminated) {
		بازگشت 0;
	}

	uint64_t result = 0;

	برای (صحیح i = 0; i < kIdEncodingNbBytes && callsign[i] != '\0'; ++i) {
		result |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	بازگشت result;
}

uint64_t DaaEncodedId::callsign_params_to_uint64(ثابت int32_t callsign_part1, ثابت int32_t callsign_part2)
{
	ثابت uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	نویسه callsign[kCallsignLength] {};

	برای (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		برای (size_t char_idx = 0; char_idx < اندازه(uint32_t); ++char_idx) {
			ثابت unsigned shift = static_cast<unsigned>((اندازه(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * اندازه(uint32_t) + char_idx] = static_cast<نویسه>((parts[part_idx] >> shift) & 0xffu);
		}
	}

	بازگشت callsign_to_uint64(callsign);
}

تهی DaaEncodedId::convert_uint64_callsign_to_str(uint64_t value, نویسه callsign[kCallsignLength])
{
	اگر (callsign == nullptr) {
		بازگشت;
	}

	memset(callsign, 0, kCallsignLength);

	ثابت صحیح max_chars = kCallsignLength - 1;

	اگر (value == 0) {
		memset(callsign, '0', max_chars);

	} وگرنه {

		برای (صحیح i = 0; i < max_chars; ++i) {
			نویسه c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			اگر (c == 0) {
				بشکن;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::last_uas_id_bytes_to_uint64(ثابت uint8_t uas_id[kUasIdByteLength])
{
	اگر (uas_id == nullptr) {
		بازگشت 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	برای (صحیح i = 0; i < kIdEncodingNbBytes; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[kUasIdByteLength - kIdEncodingNbBytes + i]) << (i * 8);
	}

	بازگشت uas_id_int;
}

تهی DaaEncodedId::convert_uas_id_uint64_to_str(ثابت uint64_t uas_id_int, نویسه uas_id_char_arr[k_Utm_Guid_پیام_Length])
{
	اگر (uas_id_char_arr == nullptr) {
		بازگشت;
	}

	ایستا constexpr نویسه kHexDigits[] = "0123456789abcdef";

	اگر (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_پیام_Length - 1);

	} وگرنه {

		// Render the low reduced_uas_id_length bytes as hex.
		ثابت صحیح reduced_uas_id_length = (k_Utm_Guid_پیام_Length - 1) / 2;

		برای (صحیح i = 0; i < reduced_uas_id_length; ++i) {
			ثابت uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_پیام_Length - 1] = '\0';
}

تهی DaaEncodedId::convert_icao_uint32_to_hex_str(uint64_t value, نویسه *buffer, size_t buffer_size)
{
	اگر (buffer == nullptr || buffer_size < kIcaoLength) {
		بازگشت;
	}

	ثابت uint32_t icao_address = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(buffer, buffer_size, "%06" PRIX32, icao_address);
}
