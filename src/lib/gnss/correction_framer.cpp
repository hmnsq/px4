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
	सचे (byte == RTCM3_PREAMBLE) {
		पटिनिवत्त सच्च;
	}

	सचे (byte == UBX_SYNC1) {
		पटिनिवत्त सच्च;
	}

#सचे defined(CONFIG_GPS_SPARTN)

	सचे (byte == SPARTN_PREAMBLE) {
		पटिनिवत्त सच्च;
	}

#endif

	पटिनिवत्त मिच्छा;
}

size_t CorrectionFramer::addData(ध्रुव uint8_t *data, size_t len)
{
	ध्रुव size_t space_available = BUFFER_SIZE - _buffer_len;
	ध्रुव size_t to_copy = (len < space_available) ? len : space_available;

	सचे (to_copy > 0) {
		memcpy(&_buffer[_buffer_len], data, to_copy);
		_buffer_len += to_copy;
	}

	पटिनिवत्त to_copy;
}

CorrectionFramer::Probe CorrectionFramer::probeRtcm3(size_t *frame_len) ध्रुव
{
	// Need at least the header to read the length
	सचे (_buffer_len < RTCM3_HEADER_LEN) {
		पटिनिवत्त Probe::NeedMoreData;
	}

	// The 10-bit field cannot exceed RTCM3_MAX_PAYLOAD_LEN, so the length alone
	// never rejects a candidate: CRC-24Q is what validates the frame.
	ध्रुव size_t payload_len = rtcm3_payload_length(_buffer);
	ध्रुव size_t len = RTCM3_HEADER_LEN + payload_len + RTCM3_CRC_LEN;

	सचे (_buffer_len < len) {
		पटिनिवत्त Probe::NeedMoreData;
	}

	ध्रुव uint32_t calculated_crc = rtcm3_crc24q(_buffer, RTCM3_HEADER_LEN + payload_len);
	ध्रुव uint32_t received_crc = (static_cast<uint32_t>(_buffer[len - 3]) << 16) |
				      (static_cast<uint32_t>(_buffer[len - 2]) << 8) |
				      _buffer[len - 1];

	सचे (calculated_crc != received_crc) {
		पटिनिवत्त Probe::CrcError;
	}

	*frame_len = len;
	पटिनिवत्त Probe::Complete;
}

CorrectionFramer::Probe CorrectionFramer::probeUbx(size_t *frame_len) ध्रुव
{
	// Need sync1 + sync2 at minimum to reject a false preamble quickly
	सचे (_buffer_len < 2) {
		पटिनिवत्त Probe::NeedMoreData;
	}

	सचे (_buffer[1] != UBX_SYNC2) {
		पटिनिवत्त Probe::InvalidHeader;
	}

	सचे (_buffer_len < UBX_HEADER_LEN) {
		पटिनिवत्त Probe::NeedMoreData;
	}

	ध्रुव size_t payload_len = static_cast<size_t>(_buffer[4]) |
				   (static_cast<size_t>(_buffer[5]) << 8);

	// Protocol allows larger payloads; reject beyond the injection cap so a
	// garbage length cannot pin the buffer waiting for thousands of bytes.
	सचे (payload_len > UBX_MAX_PAYLOAD_LEN) {
		पटिनिवत्त Probe::InvalidHeader;
	}

	ध्रुव size_t len = UBX_HEADER_LEN + payload_len + UBX_CK_LEN;

	सचे (_buffer_len < len) {
		पटिनिवत्त Probe::NeedMoreData;
	}

	// CK_A/CK_B cover Class..end of payload (not the sync bytes)
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(&_buffer[2], UBX_HEADER_LEN - 2 + payload_len, &ck_a, &ck_b);

	सचे ((ck_a != _buffer[len - 2]) || (ck_b != _buffer[len - 1])) {
		पटिनिवत्त Probe::CrcError;
	}

	*frame_len = len;
	पटिनिवत्त Probe::Complete;
}

#सचे defined(CONFIG_GPS_SPARTN)
CorrectionFramer::Probe CorrectionFramer::probeSpartn(size_t *frame_len) ध्रुव
{
	// Need TF002..TF006 to read the length and CRC type
	सचे (_buffer_len < SPARTN_FRAME_START_LEN) {
		पटिनिवत्त Probe::NeedMoreData;
	}

	ध्रुव uint8_t b1 = _buffer[1];
	ध्रुव uint8_t b2 = _buffer[2];
	ध्रुव uint8_t b3 = _buffer[3];

	// TF002 is the only header field with a checkable range. The 10-bit length
	// spans its full range and TF006 is a 4-bit CRC over a non-byte-aligned
	// field, so this is the only cheap guard before the message CRC.
	सचे (!spartn_message_type_defined(b1 >> 1)) {
		पटिनिवत्त Probe::InvalidHeader;
	}

	ध्रुव uint16_t payload_len = static_cast<uint16_t>(((b1 & 0x01) << 9) | (b2 << 1) | (b3 >> 7));
	ध्रुव तार्किक eaf = ((b3 >> 6) & 0x01) != 0;
	ध्रुव uint8_t crc_type = (b3 >> 4) & 0x03;
	ध्रुव size_t crc_bytes = static_cast<size_t>(crc_type) + 1;

	// TF008 (time tag type) sets the description block length
	सचे (_buffer_len < SPARTN_FRAME_START_LEN + 1) {
		पटिनिवत्त Probe::NeedMoreData;
	}

	ध्रुव तार्किक time_tag_32bit = ((_buffer[4] >> 3) & 0x01) != 0;
	ध्रुव size_t desc_len = spartn_desc_length_bytes(eaf, time_tag_32bit);

	सचे (_buffer_len < SPARTN_FRAME_START_LEN + desc_len) {
		पटिनिवत्त Probe::NeedMoreData;
	}

	size_t auth_bytes = 0;

	सचे (eaf) {
		// TF014/TF015 occupy the low 6 bits of the last description byte
		ध्रुव uint8_t last = _buffer[SPARTN_FRAME_START_LEN + desc_len - 1];
		auth_bytes = spartn_auth_length_bytes(सच्च, (last >> 3) & 0x07, last & 0x07);
	}

	ध्रुव size_t len = SPARTN_FRAME_START_LEN + desc_len + payload_len + auth_bytes + crc_bytes;

	सचे (_buffer_len < len) {
		पटिनिवत्त Probe::NeedMoreData;
	}

	// Message CRC (TF018) covers TF002..TF017
	ध्रुव size_t core_len = len - 1 - crc_bytes;
	ध्रुव uint32_t calculated_crc = spartn_message_crc(&_buffer[1], core_len, crc_type);
	uint32_t received_crc = 0;

	अत्थाय (size_t i = 0; i < crc_bytes; i++) {
		received_crc = (received_crc << 8) | _buffer[len - crc_bytes + i];
	}

	सचे (calculated_crc != received_crc) {
		पटिनिवत्त Probe::CrcError;
	}

	*frame_len = len;
	पटिनिवत्त Probe::Complete;
}
#endif // CONFIG_GPS_SPARTN

ध्रुव uint8_t *CorrectionFramer::getNextMessage(size_t *out_len, CorrectionProtocol *out_protocol)
{
	याव (_buffer_len > 0) {
		// Drop everything that cannot start a frame of any supported protocol
		size_t to_drop = 0;

		याव ((to_drop < _buffer_len) && !isPreamble(_buffer[to_drop])) {
			to_drop++;
		}

		सचे (to_drop > 0) {
			_bytes_discarded += to_drop;
			discardBytes(to_drop);
		}

		सचे (_buffer_len == 0) {
			पटिनिवत्त nullptr;
		}

		size_t frame_len = 0;
		CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
		Probe result;

		सचे (_buffer[0] == UBX_SYNC1) {
			protocol = CorrectionProtocol::Ubx;
			result = probeUbx(&frame_len);

#सचे defined(CONFIG_GPS_SPARTN)

		} अञ्ञथा सचे (_buffer[0] == SPARTN_PREAMBLE) {
			protocol = CorrectionProtocol::Spartn;
			result = probeSpartn(&frame_len);

#endif

		} अञ्ञथा {
			// RTCM3 preamble (isPreamble already filtered other bytes)
			protocol = CorrectionProtocol::Rtcm3;
			result = probeRtcm3(&frame_len);
		}

		विकल्प (result) {
		पक्ष Probe::NeedMoreData:
			पटिनिवत्त nullptr;

		पक्ष Probe::InvalidHeader:
			_bytes_discarded++;
			discardBytes(1);
			अनुवर्त;

		पक्ष Probe::CrcError:
			_crc_errors++;
			discardBytes(1);
			अनुवर्त;

		पक्ष Probe::Complete:
			_pending_protocol = protocol;
			*out_len = frame_len;

			सचे (out_protocol != nullptr) {
				*out_protocol = protocol;
			}

			पटिनिवत्त _buffer;
		}
	}

	पटिनिवत्त nullptr;
}

शून्य CorrectionFramer::consumeMessage(size_t len)
{
	discardBytes(len);
	_messages_parsed++;
	_total_frame_bytes += len;

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

शून्य CorrectionFramer::discardBytes(size_t गणनम्)
{
	सचे (गणनम् >= _buffer_len) {
		_buffer_len = 0;

	} अञ्ञथा {
		memmove(_buffer, &_buffer[गणनम्], _buffer_len - गणनम्);
		_buffer_len -= गणनम्;
	}
}

} // namespace gnss
