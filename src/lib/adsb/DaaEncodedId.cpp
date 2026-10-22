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

#అన్తర్భావ "DaaEncodedId.h"

#అన్తర్భావ <cinttypes>
#అన్తర్భావ <cstdio>
#అన్తర్భావ <cstring>

#అన్తర్భావ <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(స్థిరరాశి transponder_report_s &report)
{
	ఒకవేళ (report.icao_చిరునామా > 0 && report.icao_చిరునామా <= k_Max_Icao_చిరునామా) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		తిరిగి {
			static_cast<uint64_t>(report.icao_చిరునామా),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	ఒకవేళ (report.icao_చిరునామా > k_Max_Icao_చిరునామా) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	ఒకవేళ (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		స్థిరరాశి uint64_t callsign_id = callsign_గమ్యం_uint64(report.callsign);

		ఒకవేళ (callsign_id != 0) {
			తిరిగి {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	తార్కిక uas_id_valid = అబద్ధం;

	కొరకు (పూర్ణాంకం i = 0; i < k_Uas_Id_Byte_నిడివి; ++i) {
		ఒకవేళ (report.uas_id[i] != 0) {
			uas_id_valid = నిజం;
			ఆపు;
		}
	}

	ఒకవేళ (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		స్థిరరాశి uint64_t uas_id = చివరి_uas_id_బైట్లు_గమ్యం_uint64(report.uas_id);

		ఒకవేళ (uas_id != 0) {
			తిరిగి {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	తిరిగి {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(స్థిరరాశి transponder_report_s &report,
		స్థిరరాశి daa_ownship_ids_s &ownship_ids)
{
	స్థిరరాశి DaaEncodedId encoded_id = from_report(report);

	ఒకవేళ (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		తిరిగి {};
	}

	ఒకవేళ (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		తిరిగి {};
	}

	తిరిగి encoded_id;
}

తార్కిక DaaEncodedId::is_self_detection(స్థిరరాశి DaaEncodedId &encoded_id, స్థిరరాశి daa_ownship_ids_s &ownship_ids)
{
	ఎంపిక (encoded_id.encoding) {
	స్థితి detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			ఒకవేళ (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				తిరిగి నిజం;
			}

			ఒకవేళ (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				తిరిగి నిజం;
			}

			ఆపు;
		}

	స్థితి detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			ఒకవేళ (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				తిరిగి నిజం;
			}

			ఆపు;
		}

	స్థితి detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			ఒకవేళ (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				తిరిగి నిజం;
			}

			ఆపు;
		}

	అప్రమేయ:
		ఆపు;
	}

	తిరిగి అబద్ధం;
}

శూన్య DaaEncodedId::గమ్యం_string(అక్షరం *బఫరు_2, size_t బఫరు_కొలత) స్థిరరాశి
{
	ఒకవేళ (బఫరు_2 == శూన్యసూచిక || బఫరు_కొలత == 0) { తిరిగి; }

	memset(బఫరు_2, 0, బఫరు_కొలత);

	ఎంపిక (encoding) {
	స్థితి detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_గమ్యం_hex_str(id, బఫరు_2, బఫరు_కొలత);
			ఆపు;
		}

	స్థితి detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			అక్షరం callsign[k_Callsign_నిడివి] {};
			convert_uint64_callsign_గమ్యం_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			కొరకు (అక్షరం &character : callsign) {
				స్థిరరాశి uint8_t byte = static_cast<uint8_t>(character);

				ఒకవేళ (byte == 0) {
					ఆపు;
				}

				ఒకవేళ (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(బఫరు_2, బఫరు_కొలత, "%s", callsign);
			ఆపు;
		}

	స్థితి detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			అక్షరం uas_id[k_Utm_Guid_సందేశం_నిడివి] {};
			convert_uas_id_uint64_గమ్యం_str(id, uas_id);
			snprintf(బఫరు_2, బఫరు_కొలత, "%s", uas_id);
			ఆపు;
		}

	అప్రమేయ:
		snprintf(బఫరు_2, బఫరు_కొలత, "Unknown ID.");
		ఆపు;
	}
}

uint64_t DaaEncodedId::callsign_గమ్యం_uint64(స్థిరరాశి అక్షరం callsign[k_Callsign_నిడివి])
{
	ఒకవేళ (callsign == శూన్యసూచిక) {
		తిరిగి 0;
	}

	తార్కిక null_terminated = అబద్ధం;

	కొరకు (size_t i = 0; i < k_Callsign_నిడివి; ++i) {
		ఒకవేళ (callsign[i] == '\0') {
			null_terminated = నిజం;
			ఆపు;
		}
	}

	ఒకవేళ (!null_terminated) {
		తిరిగి 0;
	}

	uint64_t ఫలితం = 0;

	కొరకు (పూర్ణాంకం i = 0; i < k_Id_Encoding_Nb_బైట్లు && callsign[i] != '\0'; ++i) {
		ఫలితం |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	తిరిగి ఫలితం;
}

uint64_t DaaEncodedId::callsign_params_గమ్యం_uint64(స్థిరరాశి int32_t callsign_part1, స్థిరరాశి int32_t callsign_part2)
{
	స్థిరరాశి uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	అక్షరం callsign[k_Callsign_నిడివి] {};

	కొరకు (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		కొరకు (size_t char_idx = 0; char_idx < పరిమాణం(uint32_t); ++char_idx) {
			స్థిరరాశి గుర్తులేని జరుపు = static_cast<గుర్తులేని>((పరిమాణం(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * పరిమాణం(uint32_t) + char_idx] = static_cast<అక్షరం>((parts[part_idx] >> జరుపు) & 0xffu);
		}
	}

	తిరిగి callsign_గమ్యం_uint64(callsign);
}

శూన్య DaaEncodedId::convert_uint64_callsign_గమ్యం_str(uint64_t value, అక్షరం callsign[k_Callsign_నిడివి])
{
	ఒకవేళ (callsign == శూన్యసూచిక) {
		తిరిగి;
	}

	memset(callsign, 0, k_Callsign_నిడివి);

	స్థిరరాశి పూర్ణాంకం max_chars = k_Callsign_నిడివి - 1;

	ఒకవేళ (value == 0) {
		memset(callsign, '0', max_chars);

	} లేకపోతే {

		కొరకు (పూర్ణాంకం i = 0; i < max_chars; ++i) {
			అక్షరం c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			ఒకవేళ (c == 0) {
				ఆపు;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::చివరి_uas_id_బైట్లు_గమ్యం_uint64(స్థిరరాశి uint8_t uas_id[k_Uas_Id_Byte_నిడివి])
{
	ఒకవేళ (uas_id == శూన్యసూచిక) {
		తిరిగి 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	కొరకు (పూర్ణాంకం i = 0; i < k_Id_Encoding_Nb_బైట్లు; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[k_Uas_Id_Byte_నిడివి - k_Id_Encoding_Nb_బైట్లు + i]) << (i * 8);
	}

	తిరిగి uas_id_int;
}

శూన్య DaaEncodedId::convert_uas_id_uint64_గమ్యం_str(స్థిరరాశి uint64_t uas_id_int, అక్షరం uas_id_char_arr[k_Utm_Guid_సందేశం_నిడివి])
{
	ఒకవేళ (uas_id_char_arr == శూన్యసూచిక) {
		తిరిగి;
	}

	స్థిర constexpr అక్షరం kHexDigits[] = "0123456789abcdef";

	ఒకవేళ (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_సందేశం_నిడివి - 1);

	} లేకపోతే {

		// Render the low reduced_uas_id_length bytes as hex.
		స్థిరరాశి పూర్ణాంకం reduced_uas_id_నిడివి = (k_Utm_Guid_సందేశం_నిడివి - 1) / 2;

		కొరకు (పూర్ణాంకం i = 0; i < reduced_uas_id_నిడివి; ++i) {
			స్థిరరాశి uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_సందేశం_నిడివి - 1] = '\0';
}

శూన్య DaaEncodedId::convert_icao_uint32_గమ్యం_hex_str(uint64_t value, అక్షరం *బఫరు_2, size_t బఫరు_కొలత)
{
	ఒకవేళ (బఫరు_2 == శూన్యసూచిక || బఫరు_కొలత < k_Icao_నిడివి) {
		తిరిగి;
	}

	స్థిరరాశి uint32_t icao_చిరునామా = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(బఫరు_2, బఫరు_కొలత, "%06" PRIX32, icao_చిరునామా);
}
