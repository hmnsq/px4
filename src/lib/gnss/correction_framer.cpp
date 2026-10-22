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
 * @file correction_framer.cpp
 *
 * GNSS correction stream framing implementation.
 */

#अन्तर्भाव "correction_framer.h"

#अन्तर्भाव <cstring>

नामाकाश gnss
{

तार्किक CorrectionFramer::isPreamble(uint8_t byte)
{
	यदि (byte == RTCM3_PREAMBLE) {
		प्रत्यागम सत्य;
	}

	यदि (byte == UBX_SYNC1) {
		प्रत्यागम सत्य;
	}

#यदि defined(CONFIG_GPS_SPARTN)

	यदि (byte == SPARTN_PREAMBLE) {
		प्रत्यागम सत्य;
	}

#यदि_अन्तः

	प्रत्यागम असत्य;
}

size_t CorrectionFramer::सङ्कलय_दत्त(ध्रुव uint8_t *दत्त, size_t len)
{
	ध्रुव size_t space_उपलब्धम् = बफर_आकार_2 - बफर_len;
	ध्रुव size_t गन्तव्यम्_copy = (len < space_उपलब्धम्) ? len : space_उपलब्धम्;

	यदि (गन्तव्यम्_copy > 0) {
		स्मृतिं_प्रतिलिख(&बफर[बफर_len], दत्त, गन्तव्यम्_copy);
		बफर_len += गन्तव्यम्_copy;
	}

	प्रत्यागम गन्तव्यम्_copy;
}

CorrectionFramer::Probe CorrectionFramer::probeRtcm3(size_t *frame_len) ध्रुव
{
	// Need at least the header to read the length
	यदि (बफर_len < RTCM3_शीर्षकम्_LEN) {
		प्रत्यागम Probe::Need_अधिकम्_दत्त;
	}

	// The 10-bit field cannot exceed RTCM3_MAX_PAYLOAD_LEN, so the length alone
	// never rejects a candidate: CRC-24Q is what validates the frame.
	ध्रुव size_t payload_len = rtcm3_payload_दीर्घता(बफर);
	ध्रुव size_t len = RTCM3_शीर्षकम्_LEN + payload_len + RTCM3_CRC_LEN;

	यदि (बफर_len < len) {
		प्रत्यागम Probe::Need_अधिकम्_दत्त;
	}

	ध्रुव uint32_t calculated_crc = rtcm3_crc24q(बफर, RTCM3_शीर्षकम्_LEN + payload_len);
	ध्रुव uint32_t received_crc = (static_cast<uint32_t>(बफर[len - 3]) << 16) |
				      (static_cast<uint32_t>(बफर[len - 2]) << 8) |
				      बफर[len - 1];

	यदि (calculated_crc != received_crc) {
		प्रत्यागम Probe::CrcError;
	}

	*frame_len = len;
	प्रत्यागम Probe::Complete;
}

CorrectionFramer::Probe CorrectionFramer::probeUbx(size_t *frame_len) ध्रुव
{
	// Need sync1 + sync2 at minimum to reject a false preamble quickly
	यदि (बफर_len < 2) {
		प्रत्यागम Probe::Need_अधिकम्_दत्त;
	}

	यदि (बफर[1] != UBX_SYNC2) {
		प्रत्यागम Probe::Invalid_शीर्षकम्;
	}

	यदि (बफर_len < UBX_शीर्षकम्_LEN) {
		प्रत्यागम Probe::Need_अधिकम्_दत्त;
	}

	ध्रुव size_t payload_len = static_cast<size_t>(बफर[4]) |
				   (static_cast<size_t>(बफर[5]) << 8);

	// Protocol allows larger payloads; reject beyond the injection cap so a
	// garbage length cannot pin the buffer waiting for thousands of bytes.
	यदि (payload_len > UBX_MAX_PAYLOAD_LEN) {
		प्रत्यागम Probe::Invalid_शीर्षकम्;
	}

	ध्रुव size_t len = UBX_शीर्षकम्_LEN + payload_len + UBX_CK_LEN;

	यदि (बफर_len < len) {
		प्रत्यागम Probe::Need_अधिकम्_दत्त;
	}

	// CK_A/CK_B cover Class..end of payload (not the sync bytes)
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(&बफर[2], UBX_शीर्षकम्_LEN - 2 + payload_len, &ck_a, &ck_b);

	यदि ((ck_a != बफर[len - 2]) || (ck_b != बफर[len - 1])) {
		प्रत्यागम Probe::CrcError;
	}

	*frame_len = len;
	प्रत्यागम Probe::Complete;
}

#यदि defined(CONFIG_GPS_SPARTN)
CorrectionFramer::Probe CorrectionFramer::probeSpartn(size_t *frame_len) ध्रुव
{
	// Need TF002..TF006 to read the length and CRC type
	यदि (बफर_len < SPARTN_FRAME_START_LEN) {
		प्रत्यागम Probe::Need_अधिकम्_दत्त;
	}

	ध्रुव uint8_t b1 = बफर[1];
	ध्रुव uint8_t b2 = बफर[2];
	ध्रुव uint8_t b3 = बफर[3];

	// TF002 is the only header field with a checkable range. The 10-bit length
	// spans its full range and TF006 is a 4-bit CRC over a non-byte-aligned
	// field, so this is the only cheap guard before the message CRC.
	यदि (!spartn_सन्देशः_type_defined(b1 >> 1)) {
		प्रत्यागम Probe::Invalid_शीर्षकम्;
	}

	ध्रुव uint16_t payload_len = static_cast<uint16_t>(((b1 & 0x01) << 9) | (b2 << 1) | (b3 >> 7));
	ध्रुव तार्किक eaf = ((b3 >> 6) & 0x01) != 0;
	ध्रुव uint8_t crc_type = (b3 >> 4) & 0x03;
	ध्रुव size_t crc_अष्टकानि = static_cast<size_t>(crc_type) + 1;

	// TF008 (time tag type) sets the description block length
	यदि (बफर_len < SPARTN_FRAME_START_LEN + 1) {
		प्रत्यागम Probe::Need_अधिकम्_दत्त;
	}

	ध्रुव तार्किक time_चिह्नकम्_32bit = ((बफर[4] >> 3) & 0x01) != 0;
	ध्रुव size_t desc_len = spartn_desc_दीर्घता_अष्टकानि(eaf, time_चिह्नकम्_32bit);

	यदि (बफर_len < SPARTN_FRAME_START_LEN + desc_len) {
		प्रत्यागम Probe::Need_अधिकम्_दत्त;
	}

	size_t auth_अष्टकानि = 0;

	यदि (eaf) {
		// TF014/TF015 occupy the low 6 bits of the last description byte
		ध्रुव uint8_t चरमः = बफर[SPARTN_FRAME_START_LEN + desc_len - 1];
		auth_अष्टकानि = spartn_auth_दीर्घता_अष्टकानि(सत्य, (चरमः >> 3) & 0x07, चरमः & 0x07);
	}

	ध्रुव size_t len = SPARTN_FRAME_START_LEN + desc_len + payload_len + auth_अष्टकानि + crc_अष्टकानि;

	यदि (बफर_len < len) {
		प्रत्यागम Probe::Need_अधिकम्_दत्त;
	}

	// Message CRC (TF018) covers TF002..TF017
	ध्रुव size_t core_len = len - 1 - crc_अष्टकानि;
	ध्रुव uint32_t calculated_crc = spartn_सन्देशः_crc(&बफर[1], core_len, crc_type);
	uint32_t received_crc = 0;

	प्रति (size_t i = 0; i < crc_अष्टकानि; i++) {
		received_crc = (received_crc << 8) | बफर[len - crc_अष्टकानि + i];
	}

	यदि (calculated_crc != received_crc) {
		प्रत्यागम Probe::CrcError;
	}

	*frame_len = len;
	प्रत्यागम Probe::Complete;
}
#यदि_अन्तः // CONFIG_GPS_SPARTN

ध्रुव uint8_t *CorrectionFramer::get_अग्र_सन्देशः(size_t *बहिर्गमनम्_len, CorrectionProtocol *बहिर्गमनम्_protocol)
{
	यावत् (बफर_len > 0) {
		// Drop everything that cannot start a frame of any supported protocol
		size_t गन्तव्यम्_drop = 0;

		यावत् ((गन्तव्यम्_drop < बफर_len) && !isPreamble(बफर[गन्तव्यम्_drop])) {
			गन्तव्यम्_drop++;
		}

		यदि (गन्तव्यम्_drop > 0) {
			अष्टकानि_discarded += गन्तव्यम्_drop;
			discard_अष्टकानि(गन्तव्यम्_drop);
		}

		यदि (बफर_len == 0) {
			प्रत्यागम शून्यसूचकः;
		}

		size_t frame_len = 0;
		CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
		Probe परिणाम;

		यदि (बफर[0] == UBX_SYNC1) {
			protocol = CorrectionProtocol::Ubx;
			परिणाम = probeUbx(&frame_len);

#यदि defined(CONFIG_GPS_SPARTN)

		} अन्यथा यदि (बफर[0] == SPARTN_PREAMBLE) {
			protocol = CorrectionProtocol::Spartn;
			परिणाम = probeSpartn(&frame_len);

#यदि_अन्तः

		} अन्यथा {
			// RTCM3 preamble (isPreamble already filtered other bytes)
			protocol = CorrectionProtocol::Rtcm3;
			परिणाम = probeRtcm3(&frame_len);
		}

		विकल्प (परिणाम) {
		पक्ष Probe::Need_अधिकम्_दत्त:
			प्रत्यागम शून्यसूचकः;

		पक्ष Probe::Invalid_शीर्षकम्:
			अष्टकानि_discarded++;
			discard_अष्टकानि(1);
			अनुवर्त;

		पक्ष Probe::CrcError:
			_crc_errors++;
			discard_अष्टकानि(1);
			अनुवर्त;

		पक्ष Probe::Complete:
			_pending_protocol = protocol;
			*बहिर्गमनम्_len = frame_len;

			यदि (बहिर्गमनम्_protocol != शून्यसूचकः) {
				*बहिर्गमनम्_protocol = protocol;
			}

			प्रत्यागम बफर;
		}
	}

	प्रत्यागम शून्यसूचकः;
}

शून्य CorrectionFramer::consume_सन्देशः(size_t len)
{
	discard_अष्टकानि(len);
	_messages_parsed++;
	योग_frame_अष्टकानि += len;

	विकल्प (_pending_protocol) {
	पक्ष CorrectionProtocol::Rtcm3:
		_rtcm3_messages++;
		भङ्ग;

	पक्ष CorrectionProtocol::Spartn:
		_spartn_messages++;
		भङ्ग;

	पक्ष CorrectionProtocol::Ubx:
		_ubx_messages++;
		भङ्ग;
	}
}

शून्य CorrectionFramer::discard_अष्टकानि(size_t गणनम्)
{
	यदि (गणनम् >= बफर_len) {
		बफर_len = 0;

	} अन्यथा {
		memmove(बफर, &बफर[गणनम्], बफर_len - गणनम्);
		बफर_len -= गणनम्;
	}
}

} // namespace gnss
