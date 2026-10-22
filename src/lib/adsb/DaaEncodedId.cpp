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
	اگر (report.icao_پتہ > 0 && report.icao_پتہ <= k_Max_Icao_پتہ) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		واپس {
			static_cast<uint64_t>(report.icao_پتہ),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	اگر (report.icao_پتہ > k_Max_Icao_پتہ) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	اگر (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		ثابت uint64_t callsign_id = callsign_مقصد_uint64(report.callsign);

		اگر (callsign_id != 0) {
			واپس {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	منطقی uas_id_valid = جھوٹ;

	برائے (صحیح i = 0; i < k_Uas_Id_Byte_لمبائی; ++i) {
		اگر (report.uas_id[i] != 0) {
			uas_id_valid = سچ;
			توڑو;
		}
	}

	اگر (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		ثابت uint64_t uas_id = آخری_uas_id_بائٹیں_مقصد_uint64(report.uas_id);

		اگر (uas_id != 0) {
			واپس {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	واپس {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(ثابت transponder_report_s &report,
		ثابت daa_ownship_ids_s &ownship_ids)
{
	ثابت DaaEncodedId encoded_id = from_report(report);

	اگر (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		واپس {};
	}

	اگر (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		واپس {};
	}

	واپس encoded_id;
}

منطقی DaaEncodedId::is_self_detection(ثابت DaaEncodedId &encoded_id, ثابت daa_ownship_ids_s &ownship_ids)
{
	انتخاب (encoded_id.encoding) {
	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			اگر (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				واپس سچ;
			}

			اگر (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				واپس سچ;
			}

			توڑو;
		}

	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			اگر (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				واپس سچ;
			}

			توڑو;
		}

	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			اگر (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				واپس سچ;
			}

			توڑو;
		}

	طےشدہ:
		توڑو;
	}

	واپس جھوٹ;
}

خالی DaaEncodedId::مقصد_string(حرف *بفر_2, size_t بفر_سائز) ثابت
{
	اگر (بفر_2 == خالی_اشاریہ || بفر_سائز == 0) { واپس; }

	memset(بفر_2, 0, بفر_سائز);

	انتخاب (encoding) {
	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_مقصد_hex_str(id, بفر_2, بفر_سائز);
			توڑو;
		}

	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			حرف callsign[k_Callsign_لمبائی] {};
			convert_uint64_callsign_مقصد_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			برائے (حرف &character : callsign) {
				ثابت uint8_t byte = static_cast<uint8_t>(character);

				اگر (byte == 0) {
					توڑو;
				}

				اگر (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(بفر_2, بفر_سائز, "%s", callsign);
			توڑو;
		}

	حالت detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			حرف uas_id[k_Utm_Guid_پیغام_لمبائی] {};
			convert_uas_id_uint64_مقصد_str(id, uas_id);
			snprintf(بفر_2, بفر_سائز, "%s", uas_id);
			توڑو;
		}

	طےشدہ:
		snprintf(بفر_2, بفر_سائز, "Unknown ID.");
		توڑو;
	}
}

uint64_t DaaEncodedId::callsign_مقصد_uint64(ثابت حرف callsign[k_Callsign_لمبائی])
{
	اگر (callsign == خالی_اشاریہ) {
		واپس 0;
	}

	منطقی null_terminated = جھوٹ;

	برائے (size_t i = 0; i < k_Callsign_لمبائی; ++i) {
		اگر (callsign[i] == '\0') {
			null_terminated = سچ;
			توڑو;
		}
	}

	اگر (!null_terminated) {
		واپس 0;
	}

	uint64_t نتیجہ = 0;

	برائے (صحیح i = 0; i < k_Id_Encoding_Nb_بائٹیں && callsign[i] != '\0'; ++i) {
		نتیجہ |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	واپس نتیجہ;
}

uint64_t DaaEncodedId::callsign_params_مقصد_uint64(ثابت int32_t callsign_part1, ثابت int32_t callsign_part2)
{
	ثابت uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	حرف callsign[k_Callsign_لمبائی] {};

	برائے (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		برائے (size_t char_idx = 0; char_idx < حجم(uint32_t); ++char_idx) {
			ثابت غیرعلامتی سرکاؤ = static_cast<غیرعلامتی>((حجم(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * حجم(uint32_t) + char_idx] = static_cast<حرف>((parts[part_idx] >> سرکاؤ) & 0xffu);
		}
	}

	واپس callsign_مقصد_uint64(callsign);
}

خالی DaaEncodedId::convert_uint64_callsign_مقصد_str(uint64_t value, حرف callsign[k_Callsign_لمبائی])
{
	اگر (callsign == خالی_اشاریہ) {
		واپس;
	}

	memset(callsign, 0, k_Callsign_لمبائی);

	ثابت صحیح max_chars = k_Callsign_لمبائی - 1;

	اگر (value == 0) {
		memset(callsign, '0', max_chars);

	} ورنہ {

		برائے (صحیح i = 0; i < max_chars; ++i) {
			حرف c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			اگر (c == 0) {
				توڑو;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::آخری_uas_id_بائٹیں_مقصد_uint64(ثابت uint8_t uas_id[k_Uas_Id_Byte_لمبائی])
{
	اگر (uas_id == خالی_اشاریہ) {
		واپس 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	برائے (صحیح i = 0; i < k_Id_Encoding_Nb_بائٹیں; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[k_Uas_Id_Byte_لمبائی - k_Id_Encoding_Nb_بائٹیں + i]) << (i * 8);
	}

	واپس uas_id_int;
}

خالی DaaEncodedId::convert_uas_id_uint64_مقصد_str(ثابت uint64_t uas_id_int, حرف uas_id_char_arr[k_Utm_Guid_پیغام_لمبائی])
{
	اگر (uas_id_char_arr == خالی_اشاریہ) {
		واپس;
	}

	ساکن constexpr حرف kHexDigits[] = "0123456789abcdef";

	اگر (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_پیغام_لمبائی - 1);

	} ورنہ {

		// Render the low reduced_uas_id_length bytes as hex.
		ثابت صحیح reduced_uas_id_لمبائی = (k_Utm_Guid_پیغام_لمبائی - 1) / 2;

		برائے (صحیح i = 0; i < reduced_uas_id_لمبائی; ++i) {
			ثابت uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_پیغام_لمبائی - 1] = '\0';
}

خالی DaaEncodedId::convert_icao_uint32_مقصد_hex_str(uint64_t value, حرف *بفر_2, size_t بفر_سائز)
{
	اگر (بفر_2 == خالی_اشاریہ || بفر_سائز < k_Icao_لمبائی) {
		واپس;
	}

	ثابت uint32_t icao_پتہ = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(بفر_2, بفر_سائز, "%06" PRIX32, icao_پتہ);
}
