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

DaaEncodedId DaaEncodedId::from_report(ध्रुव transponder_report_s &report)
{
	यदि (report.icao_स्थानाङ्कः > 0 && report.icao_स्थानाङ्कः <= k_Max_Icao_स्थानाङ्कः) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		प्रत्यागम {
			static_cast<uint64_t>(report.icao_स्थानाङ्कः),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	यदि (report.icao_स्थानाङ्कः > k_Max_Icao_स्थानाङ्कः) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	यदि (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		ध्रुव uint64_t callsign_id = callsign_गन्तव्यम्_uint64(report.callsign);

		यदि (callsign_id != 0) {
			प्रत्यागम {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	तार्किक uas_id_valid = असत्य;

	प्रति (पूर्णाङ्क i = 0; i < k_Uas_Id_Byte_दीर्घता; ++i) {
		यदि (report.uas_id[i] != 0) {
			uas_id_valid = सत्य;
			भङ्ग;
		}
	}

	यदि (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		ध्रुव uint64_t uas_id = चरमः_uas_id_अष्टकानि_गन्तव्यम्_uint64(report.uas_id);

		यदि (uas_id != 0) {
			प्रत्यागम {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	प्रत्यागम {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(ध्रुव transponder_report_s &report,
		ध्रुव daa_ownship_ids_s &ownship_ids)
{
	ध्रुव DaaEncodedId encoded_id = from_report(report);

	यदि (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		प्रत्यागम {};
	}

	यदि (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		प्रत्यागम {};
	}

	प्रत्यागम encoded_id;
}

तार्किक DaaEncodedId::is_self_detection(ध्रुव DaaEncodedId &encoded_id, ध्रुव daa_ownship_ids_s &ownship_ids)
{
	विकल्प (encoded_id.encoding) {
	पक्ष detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			यदि (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				प्रत्यागम सत्य;
			}

			यदि (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				प्रत्यागम सत्य;
			}

			भङ्ग;
		}

	पक्ष detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			यदि (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				प्रत्यागम सत्य;
			}

			भङ्ग;
		}

	पक्ष detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			यदि (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				प्रत्यागम सत्य;
			}

			भङ्ग;
		}

	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम असत्य;
}

शून्य DaaEncodedId::गन्तव्यम्_string(वर्ण *बफर_2, size_t बफर_आकार) ध्रुव
{
	यदि (बफर_2 == शून्यसूचकः || बफर_आकार == 0) { प्रत्यागम; }

	स्मृतिं_पूरय(बफर_2, 0, बफर_आकार);

	विकल्प (encoding) {
	पक्ष detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_गन्तव्यम्_hex_str(id, बफर_2, बफर_आकार);
			भङ्ग;
		}

	पक्ष detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			वर्ण callsign[k_Callsign_दीर्घता] {};
			convert_uint64_callsign_गन्तव्यम्_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			प्रति (वर्ण &character : callsign) {
				ध्रुव uint8_t byte = static_cast<uint8_t>(character);

				यदि (byte == 0) {
					भङ्ग;
				}

				यदि (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(बफर_2, बफर_आकार, "%s", callsign);
			भङ्ग;
		}

	पक्ष detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			वर्ण uas_id[k_Utm_Guid_सन्देशः_दीर्घता] {};
			convert_uas_id_uint64_गन्तव्यम्_str(id, uas_id);
			snprintf(बफर_2, बफर_आकार, "%s", uas_id);
			भङ्ग;
		}

	स्वभाव:
		snprintf(बफर_2, बफर_आकार, "Unknown ID.");
		भङ्ग;
	}
}

uint64_t DaaEncodedId::callsign_गन्तव्यम्_uint64(ध्रुव वर्ण callsign[k_Callsign_दीर्घता])
{
	यदि (callsign == शून्यसूचकः) {
		प्रत्यागम 0;
	}

	तार्किक null_terminated = असत्य;

	प्रति (size_t i = 0; i < k_Callsign_दीर्घता; ++i) {
		यदि (callsign[i] == '\0') {
			null_terminated = सत्य;
			भङ्ग;
		}
	}

	यदि (!null_terminated) {
		प्रत्यागम 0;
	}

	uint64_t परिणाम = 0;

	प्रति (पूर्णाङ्क i = 0; i < k_Id_Encoding_Nb_अष्टकानि && callsign[i] != '\0'; ++i) {
		परिणाम |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	प्रत्यागम परिणाम;
}

uint64_t DaaEncodedId::callsign_params_गन्तव्यम्_uint64(ध्रुव int32_t callsign_part1, ध्रुव int32_t callsign_part2)
{
	ध्रुव uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	वर्ण callsign[k_Callsign_दीर्घता] {};

	प्रति (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		प्रति (size_t char_idx = 0; char_idx < परिमाण(uint32_t); ++char_idx) {
			ध्रुव अचिह्नित सरणम् = static_cast<अचिह्नित>((परिमाण(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * परिमाण(uint32_t) + char_idx] = static_cast<वर्ण>((parts[part_idx] >> सरणम्) & 0xffu);
		}
	}

	प्रत्यागम callsign_गन्तव्यम्_uint64(callsign);
}

शून्य DaaEncodedId::convert_uint64_callsign_गन्तव्यम्_str(uint64_t value, वर्ण callsign[k_Callsign_दीर्घता])
{
	यदि (callsign == शून्यसूचकः) {
		प्रत्यागम;
	}

	स्मृतिं_पूरय(callsign, 0, k_Callsign_दीर्घता);

	ध्रुव पूर्णाङ्क max_chars = k_Callsign_दीर्घता - 1;

	यदि (value == 0) {
		स्मृतिं_पूरय(callsign, '0', max_chars);

	} अन्यथा {

		प्रति (पूर्णाङ्क i = 0; i < max_chars; ++i) {
			वर्ण c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			यदि (c == 0) {
				भङ्ग;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::चरमः_uas_id_अष्टकानि_गन्तव्यम्_uint64(ध्रुव uint8_t uas_id[k_Uas_Id_Byte_दीर्घता])
{
	यदि (uas_id == शून्यसूचकः) {
		प्रत्यागम 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	प्रति (पूर्णाङ्क i = 0; i < k_Id_Encoding_Nb_अष्टकानि; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[k_Uas_Id_Byte_दीर्घता - k_Id_Encoding_Nb_अष्टकानि + i]) << (i * 8);
	}

	प्रत्यागम uas_id_int;
}

शून्य DaaEncodedId::convert_uas_id_uint64_गन्तव्यम्_str(ध्रुव uint64_t uas_id_int, वर्ण uas_id_char_arr[k_Utm_Guid_सन्देशः_दीर्घता])
{
	यदि (uas_id_char_arr == शून्यसूचकः) {
		प्रत्यागम;
	}

	स्थिर constexpr वर्ण kHexDigits[] = "0123456789abcdef";

	यदि (uas_id_int == 0) {
		स्मृतिं_पूरय(uas_id_char_arr, '0', k_Utm_Guid_सन्देशः_दीर्घता - 1);

	} अन्यथा {

		// Render the low reduced_uas_id_length bytes as hex.
		ध्रुव पूर्णाङ्क reduced_uas_id_दीर्घता = (k_Utm_Guid_सन्देशः_दीर्घता - 1) / 2;

		प्रति (पूर्णाङ्क i = 0; i < reduced_uas_id_दीर्घता; ++i) {
			ध्रुव uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_सन्देशः_दीर्घता - 1] = '\0';
}

शून्य DaaEncodedId::convert_icao_uint32_गन्तव्यम्_hex_str(uint64_t value, वर्ण *बफर_2, size_t बफर_आकार)
{
	यदि (बफर_2 == शून्यसूचकः || बफर_आकार < k_Icao_दीर्घता) {
		प्रत्यागम;
	}

	ध्रुव uint32_t icao_स्थानाङ्कः = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(बफर_2, बफर_आकार, "%06" PRIX32, icao_स्थानाङ्कः);
}
