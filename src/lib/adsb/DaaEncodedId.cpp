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

#ଅନ୍ତର୍ଭାଵ "DaaEncodedId.h"

#ଅନ୍ତର୍ଭାଵ <cinttypes>
#ଅନ୍ତର୍ଭାଵ <cstdio>
#ଅନ୍ତର୍ଭାଵ <cstring>

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/log.h>

DaaEncodedId DaaEncodedId::from_report(ଅଚଳ transponder_report_s &report)
{
	ଯଦି (report.icao_address > 0 && report.icao_address <= kMaxIcaoAddress) {
		PX4_DEBUG("DAA: Unique ID encoding: ICAO.");
		ଫେରନ୍ତୁ {
			static_cast<uint64_t>(report.icao_address),
			detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO
		};
	}

	ଯଦି (report.icao_address > kMaxIcaoAddress) {
		PX4_DEBUG("DAA: invalid ICAO address");
	}

	ଯଦି (report.flags & transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN) {
		PX4_DEBUG("DAA: Unique ID encoding: Callsign.");
		ଅଚଳ uint64_t callsign_id = callsign_to_uint64(report.callsign);

		ଯଦି (callsign_id != 0) {
			ଫେରନ୍ତୁ {
				callsign_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN
			};
		}

		// Fall back to the UAS-ID encoding below.
		PX4_DEBUG("DAA: Failed to convert callsign to uint64.");
	}

	ତାର୍କିକ uas_id_valid = ମିଥ୍ୟା;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < kUasIdByteLength; ++i) {
		ଯଦି (report.uas_id[i] != 0) {
			uas_id_valid = ସତ୍ୟ;
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି (uas_id_valid) {
		PX4_DEBUG("DAA: Unique ID encoding: UAS id.");
		ଅଚଳ uint64_t uas_id = last_uas_id_bytes_to_uint64(report.uas_id);

		ଯଦି (uas_id != 0) {
			ଫେରନ୍ତୁ {
				uas_id,
				detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID
			};
		}

		PX4_DEBUG("DAA: Failed to convert uas_id to uint64.");
	}

	ଫେରନ୍ତୁ {};
}

DaaEncodedId DaaEncodedId::identify_traffic_report(ଅଚଳ transponder_report_s &report,
		ଅଚଳ daa_ownship_ids_s &ownship_ids)
{
	ଅଚଳ DaaEncodedId encoded_id = from_report(report);

	ଯଦି (encoded_id.id == 0) {
		PX4_DEBUG("DAA: No valid unique ID, skipping report");
		ଫେରନ୍ତୁ {};
	}

	ଯଦି (is_self_detection(encoded_id, ownship_ids)) {
		PX4_DEBUG("DAA: Self detection, skipping report.");
		ଫେରନ୍ତୁ {};
	}

	ଫେରନ୍ତୁ encoded_id;
}

ତାର୍କିକ DaaEncodedId::is_self_detection(ଅଚଳ DaaEncodedId &encoded_id, ଅଚଳ daa_ownship_ids_s &ownship_ids)
{
	ବାଛ (encoded_id.encoding) {
	ସ୍ଥିତି detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			ଯଦି (ownship_ids.icao >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao)) {
				PX4_DEBUG("DAA: Received own main ICAO.");
				ଫେରନ୍ତୁ ସତ୍ୟ;
			}

			ଯଦି (ownship_ids.icao_2 >= 0
			    && static_cast<uint32_t>(encoded_id.id) == static_cast<uint32_t>(ownship_ids.icao_2)) {
				PX4_DEBUG("DAA: Received own secondary ICAO.");
				ଫେରନ୍ତୁ ସତ୍ୟ;
			}

			ଭାଙ୍ଗ;
		}

	ସ୍ଥିତି detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			ଯଦି (encoded_id.id == ownship_ids.callsign) {
				PX4_DEBUG("DAA: Received own Callsign.");
				ଫେରନ୍ତୁ ସତ୍ୟ;
			}

			ଭାଙ୍ଗ;
		}

	ସ୍ଥିତି detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			ଯଦି (ownship_ids.uas_id_valid && encoded_id.id == ownship_ids.uas_id) {
				PX4_DEBUG("DAA: Received own UAS ID.");
				ଫେରନ୍ତୁ ସତ୍ୟ;
			}

			ଭାଙ୍ଗ;
		}

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ DaaEncodedId::to_string(ଅକ୍ଷର *buffer, size_t buffer_size) ଅଚଳ
{
	ଯଦି (buffer == nullptr || buffer_size == 0) { ଫେରନ୍ତୁ; }

	memset(buffer, 0, buffer_size);

	ବାଛ (encoding) {
	ସ୍ଥିତି detect_and_avoid_s::UNIQUE_ID_ENCODING_ICAO: {
			convert_icao_uint32_to_hex_str(id, buffer, buffer_size);
			ଭାଙ୍ଗ;
		}

	ସ୍ଥିତି detect_and_avoid_s::UNIQUE_ID_ENCODING_ADSB_CALLSIGN: {
			ଅକ୍ଷର callsign[kCallsignLength] {};
			convert_uint64_callsign_to_str(id, callsign);

			// Keep malformed/custom publishers from injecting control
			// characters into MAVLink status text.
			ପାଇଁ (ଅକ୍ଷର &character : callsign) {
				ଅଚଳ uint8_t byte = static_cast<uint8_t>(character);

				ଯଦି (byte == 0) {
					ଭାଙ୍ଗ;
				}

				ଯଦି (byte < 0x20 || byte > 0x7e) {
					character = '?';
				}
			}

			snprintf(buffer, buffer_size, "%s", callsign);
			ଭାଙ୍ଗ;
		}

	ସ୍ଥିତି detect_and_avoid_s::UNIQUE_ID_ENCODING_UAS_ID: {
			ଅକ୍ଷର uas_id[k_Utm_Guid_ସନ୍ଦେଶ_Length] {};
			convert_uas_id_uint64_to_str(id, uas_id);
			snprintf(buffer, buffer_size, "%s", uas_id);
			ଭାଙ୍ଗ;
		}

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		snprintf(buffer, buffer_size, "Unknown ID.");
		ଭାଙ୍ଗ;
	}
}

uint64_t DaaEncodedId::callsign_to_uint64(ଅଚଳ ଅକ୍ଷର callsign[kCallsignLength])
{
	ଯଦି (callsign == nullptr) {
		ଫେରନ୍ତୁ 0;
	}

	ତାର୍କିକ null_terminated = ମିଥ୍ୟା;

	ପାଇଁ (size_t i = 0; i < kCallsignLength; ++i) {
		ଯଦି (callsign[i] == '\0') {
			null_terminated = ସତ୍ୟ;
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି (!null_terminated) {
		ଫେରନ୍ତୁ 0;
	}

	uint64_t result = 0;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < kIdEncodingNbBytes && callsign[i] != '\0'; ++i) {
		result |= (static_cast<uint64_t>(static_cast<uint8_t>(callsign[i])) << (i * 8));
	}

	ଫେରନ୍ତୁ result;
}

uint64_t DaaEncodedId::callsign_params_to_uint64(ଅଚଳ int32_t callsign_part1, ଅଚଳ int32_t callsign_part2)
{
	ଅଚଳ uint32_t parts[] {
		static_cast<uint32_t>(callsign_part1),
		static_cast<uint32_t>(callsign_part2)
	};
	ଅକ୍ଷର callsign[kCallsignLength] {};

	ପାଇଁ (size_t part_idx = 0; part_idx < 2; ++part_idx) {
		ପାଇଁ (size_t char_idx = 0; char_idx < ଆକାର(uint32_t); ++char_idx) {
			ଅଚଳ unsigned shift = static_cast<unsigned>((ଆକାର(uint32_t) - char_idx - 1) * 8);
			callsign[part_idx * ଆକାର(uint32_t) + char_idx] = static_cast<ଅକ୍ଷର>((parts[part_idx] >> shift) & 0xffu);
		}
	}

	ଫେରନ୍ତୁ callsign_to_uint64(callsign);
}

ଶୂନ୍ୟ DaaEncodedId::convert_uint64_callsign_to_str(uint64_t value, ଅକ୍ଷର callsign[kCallsignLength])
{
	ଯଦି (callsign == nullptr) {
		ଫେରନ୍ତୁ;
	}

	memset(callsign, 0, kCallsignLength);

	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_chars = kCallsignLength - 1;

	ଯଦି (value == 0) {
		memset(callsign, '0', max_chars);

	} ଅନ୍ୟଥା {

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < max_chars; ++i) {
			ଅକ୍ଷର c = (value >> (i * 8)) & 0xFF;
			callsign[i] = c;

			ଯଦି (c == 0) {
				ଭାଙ୍ଗ;
			}
		}
	}

	callsign[max_chars] = '\0';
}

uint64_t DaaEncodedId::last_uas_id_bytes_to_uint64(ଅଚଳ uint8_t uas_id[kUasIdByteLength])
{
	ଯଦି (uas_id == nullptr) {
		ଫେରନ୍ତୁ 0;
	}

	uint64_t uas_id_int = 0;

	// Pack the last kIdEncodingNbBytes bytes into the key.
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < kIdEncodingNbBytes; ++i) {
		uas_id_int |= static_cast<uint64_t>(uas_id[kUasIdByteLength - kIdEncodingNbBytes + i]) << (i * 8);
	}

	ଫେରନ୍ତୁ uas_id_int;
}

ଶୂନ୍ୟ DaaEncodedId::convert_uas_id_uint64_to_str(ଅଚଳ uint64_t uas_id_int, ଅକ୍ଷର uas_id_char_arr[k_Utm_Guid_ସନ୍ଦେଶ_Length])
{
	ଯଦି (uas_id_char_arr == nullptr) {
		ଫେରନ୍ତୁ;
	}

	ସ୍ଥିର constexpr ଅକ୍ଷର kHexDigits[] = "0123456789abcdef";

	ଯଦି (uas_id_int == 0) {
		memset(uas_id_char_arr, '0', k_Utm_Guid_ସନ୍ଦେଶ_Length - 1);

	} ଅନ୍ୟଥା {

		// Render the low reduced_uas_id_length bytes as hex.
		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା reduced_uas_id_length = (k_Utm_Guid_ସନ୍ଦେଶ_Length - 1) / 2;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < reduced_uas_id_length; ++i) {
			ଅଚଳ uint8_t byte = static_cast<uint8_t>((uas_id_int >>(i * 8)) & 0xFF);
			uas_id_char_arr[i * 2] = kHexDigits[byte >> 4];
			uas_id_char_arr[i * 2 + 1] = kHexDigits[byte & 0x0F];
		}
	}

	uas_id_char_arr[k_Utm_Guid_ସନ୍ଦେଶ_Length - 1] = '\0';
}

ଶୂନ୍ୟ DaaEncodedId::convert_icao_uint32_to_hex_str(uint64_t value, ଅକ୍ଷର *buffer, size_t buffer_size)
{
	ଯଦି (buffer == nullptr || buffer_size < kIcaoLength) {
		ଫେରନ୍ତୁ;
	}

	ଅଚଳ uint32_t icao_address = static_cast<uint32_t>(value & 0xFFFFFFu);
	snprintf(buffer, buffer_size, "%06" PRIX32, icao_address);
}
