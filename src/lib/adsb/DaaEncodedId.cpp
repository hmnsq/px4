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

#अन्तर्भाव "DaaEncodedId.h"

#अन्तर्भाव <cinttypes>
#अन्तर्भाव <cstdio>
#अन्तर्भाव <cstring>

#अन्तर्भाव <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(स्थिरांक transponder_report_s &report)
{
	जर (report.icao_address > 0 && report.icao_address <= kMaxIcaoAddress) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		परत {
			static_cast<uint64_t>(report.icao_address),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	जर (report.icao_address > kMaxIcaoAddress) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	जर (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		स्थिरांक uint64_t callsign_id = callsign_to_uint64(report.callsign);

		जर (callsign_id != 0) {
			परत {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	तार्किक uas_id_valid = असत्य;

	साठी (पूर्णांक i = 0; i < kUasIdByteLength; ++i) {
		जर (report.uas_id[i] != 0) {
			uas_id_valid = सत्य;
			थांबा;
		}
	}

	जर (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		स्थिरांक uint64_t uas_id = last_uas_id_bytes_to_uint64(report.uas_id);

		जर (uas_id != 0) {
			परत {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	परत {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(स्थिरांक transponder_report_s &report,
		स्थिरांक daa_ownship_ids_s &ownship_ids)
{
	स्थिरांक DaaEncodedId encoded_id = from_report(report);

	जर (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		परत {};
	}

	जर (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		परत {};
	}

	परत encoded_id;
}

तार्किक DaaEncodedId::is_self_detection(स्थिरांक DaaEncodedId &encoded_id, स्थिरांक daa_ownship_ids_s &ownship_ids)
{
	निवड (encoded_id.encoding) {
	स्थिती detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			जर (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				परत सत्य;
			}

			जर (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				परत सत्य;
			}

			थांबा;
		}

	स्थिती detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			जर (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				परत सत्य;
			}

			थांबा;
		}

	स्थिती detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			जर (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				परत सत्य;
			}

			थांबा;
		}

	पूर्वनिर्धारित:
		थांबा;
	}

	परत असत्य;
}

शून्य DaaEncodedId::to_string(अक्षर *buffer, माप_प्रकार buffer_size) स्थिरांक
{
	जर (buffer == nullptr || buffer_size == 0) { परत; }

	स_रखो(buffer, 0, buffer_size);

	निवड (encoding) {
	स्थिती detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_to_hex_str(id, buffer, buffer_size);
			थांबा;
		}

	स्थिती detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			अक्षर callsign[kCallsignLength] {};
			convert_uint64_callsign_to_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			साठी (अक्षर &character : callsign) {
				स्थिरांक uint8_t byte = static_cast<uint8_t>(character);

				जर (byte == 0) {
					थांबा;
				}

				जर (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(buffer, buffer_size, "%s", callsign);
			थांबा;
		}

	स्थिती detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			अक्षर uas_id[k_Utm_Guid_संदेश_Length] {};
			convert_uas_id_uint64_to_str(id, uas_id);
			snprintf(buffer, buffer_size, "%s", uas_id);
			थांबा;
		}

	पूर्वनिर्धारित:
		snprintf(buffer, buffer_size, "Unknown ID.");
		थांबा;
	}
}

uint64_t DaaEncodedId::callsign_to_uint64(स्थिरांक अक्षर callsign[kCallsignLength])
{
	जर (callsign == nullptr) {
		परत 0;
	}

	तार्किक null_terminated = असत्य;

	साठी (माप_प्रकार i = 0; i < kCallsignLength; ++i) {
		जर (callsign[i] == '\0') {
			null_terminated = सत्य;
			थांबा;
		}
	}

	जर (!null_terminated) {
		परत 0;
	}

	uint64_t result = 0;

	साठी (पूर्णांक i = 0; i < kIdEncodingNbBytes && callsign[i] != '\0'; ++i) {
		result |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	परत result;
}

uint64_t DaaEncodedId::callsign_params_to_uint64(स्थिरांक int32_t callsign_part1, स्थिरांक int32_t callsign_part2)
{
	स्थिरांक uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	अक्षर callsign[kCallsignLength] {};

	साठी (माप_प्रकार part_idx = 0; part_idx < 2; ++part_idx) {
		साठी (माप_प्रकार char_idx = 0; char_idx < आकार(uint32_t); ++char_idx) {
			स्थिरांक अचिन्हित shift = static_cast<अचिन्हित>((आकार(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * आकार(uint32_t) + char_idx] = static_cast<अक्षर>((parts[part_idx] >> shift) & 0xffu);
		}
	}

	परत callsign_to_uint64(callsign);
}

शून्य DaaEncodedId::convert_uint64_callsign_to_str(uint64_t value, अक्षर callsign[kCallsignLength])
{
	जर (callsign == nullptr) {
		परत;
	}

	स_रखो(callsign, 0, kCallsignLength);

	स्थिरांक पूर्णांक max_chars = kCallsignLength - 1;

	जर (value == 0) {
		स_रखो(callsign, '0', max_chars);

	} अन्यथा {

		साठी (पूर्णांक i = 0; i < max_chars; ++i) {
			अक्षर c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			जर (c == 0) {
				थांबा;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::last_uas_id_bytes_to_uint64(स्थिरांक uint8_t uas_id[kUasIdByteLength])
{
	जर (uas_id == nullptr) {
		परत 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	साठी (पूर्णांक i = 0; i < kIdEncodingNbBytes; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[kUasIdByteLength - kIdEncodingNbBytes + i]) << (i * 8);
	}

	परत uas_id_int;
}

शून्य DaaEncodedId::convert_uas_id_uint64_to_str(स्थिरांक uint64_t uas_id_int, अक्षर uas_id_char_arr[k_Utm_Guid_संदेश_Length])
{
	जर (uas_id_char_arr == nullptr) {
		परत;
	}

	स्थिर constexpr अक्षर kHexDigits[] = "0123456789abcdef";

	जर (uas_id_int == 0) {
		स_रखो(uas_id_char_arr, '0', k_Utm_Guid_संदेश_Length - 1);

	} अन्यथा {

		// Render the low reduced_uas_id_length bytes as hex.
		स्थिरांक पूर्णांक reduced_uas_id_length = (k_Utm_Guid_संदेश_Length - 1) / 2;

		साठी (पूर्णांक i = 0; i < reduced_uas_id_length; ++i) {
			स्थिरांक uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_संदेश_Length - 1] = '\0';
}

शून्य DaaEncodedId::convert_icao_uint32_to_hex_str(uint64_t value, अक्षर *buffer, माप_प्रकार buffer_size)
{
	जर (buffer == nullptr || buffer_size < kIcaoLength) {
		परत;
	}

	स्थिरांक uint32_t icao_address = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(buffer, buffer_size, "%06" PRIX32, icao_address);
}
