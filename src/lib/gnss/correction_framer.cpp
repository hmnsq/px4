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

#ಅನ್ತರ್ಭಾವ "correction_framer.h"

#ಅನ್ತರ್ಭಾವ <cstring>

ನಾಮಾಕಾಶ gnss
{

ತಾರ್ಕಿಕ CorrectionFramer::isPreamble(uint8_t byte)
{
	ಯದಿ (byte == RTCM3_PREAMBLE) {
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (byte == UBX_SYNC1) {
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

#ಯದಿ defined(CONFIG_GPS_SPARTN)

	ಯದಿ (byte == SPARTN_PREAMBLE) {
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

#endif

	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

size_t CorrectionFramer::addData(ಧ್ರುವ uint8_t *data, size_t len)
{
	ಧ್ರುವ size_t space_available = BUFFER_SIZE - _buffer_len;
	ಧ್ರುವ size_t to_copy = (len < space_available) ? len : space_available;

	ಯದಿ (to_copy > 0) {
		memcpy(&_buffer[_buffer_len], data, to_copy);
		_buffer_len += to_copy;
	}

	ಪ್ರತ್ಯಾಗಮ to_copy;
}

CorrectionFramer::Probe CorrectionFramer::probeRtcm3(size_t *frame_len) ಧ್ರುವ
{
	// Need at least the header to read the length
	ಯದಿ (_buffer_len < RTCM3_HEADER_LEN) {
		ಪ್ರತ್ಯಾಗಮ Probe::NeedMoreData;
	}

	// The 10-bit field cannot exceed RTCM3_MAX_PAYLOAD_LEN, so the length alone
	// never rejects a candidate: CRC-24Q is what validates the frame.
	ಧ್ರುವ size_t payload_len = rtcm3_payload_length(_buffer);
	ಧ್ರುವ size_t len = RTCM3_HEADER_LEN + payload_len + RTCM3_CRC_LEN;

	ಯದಿ (_buffer_len < len) {
		ಪ್ರತ್ಯಾಗಮ Probe::NeedMoreData;
	}

	ಧ್ರುವ uint32_t calculated_crc = rtcm3_crc24q(_buffer, RTCM3_HEADER_LEN + payload_len);
	ಧ್ರುವ uint32_t received_crc = (static_cast<uint32_t>(_buffer[len - 3]) << 16) |
				      (static_cast<uint32_t>(_buffer[len - 2]) << 8) |
				      _buffer[len - 1];

	ಯದಿ (calculated_crc != received_crc) {
		ಪ್ರತ್ಯಾಗಮ Probe::CrcError;
	}

	*frame_len = len;
	ಪ್ರತ್ಯಾಗಮ Probe::Complete;
}

CorrectionFramer::Probe CorrectionFramer::probeUbx(size_t *frame_len) ಧ್ರುವ
{
	// Need sync1 + sync2 at minimum to reject a false preamble quickly
	ಯದಿ (_buffer_len < 2) {
		ಪ್ರತ್ಯಾಗಮ Probe::NeedMoreData;
	}

	ಯದಿ (_buffer[1] != UBX_SYNC2) {
		ಪ್ರತ್ಯಾಗಮ Probe::InvalidHeader;
	}

	ಯದಿ (_buffer_len < UBX_HEADER_LEN) {
		ಪ್ರತ್ಯಾಗಮ Probe::NeedMoreData;
	}

	ಧ್ರುವ size_t payload_len = static_cast<size_t>(_buffer[4]) |
				   (static_cast<size_t>(_buffer[5]) << 8);

	// Protocol allows larger payloads; reject beyond the injection cap so a
	// garbage length cannot pin the buffer waiting for thousands of bytes.
	ಯದಿ (payload_len > UBX_MAX_PAYLOAD_LEN) {
		ಪ್ರತ್ಯಾಗಮ Probe::InvalidHeader;
	}

	ಧ್ರುವ size_t len = UBX_HEADER_LEN + payload_len + UBX_CK_LEN;

	ಯದಿ (_buffer_len < len) {
		ಪ್ರತ್ಯಾಗಮ Probe::NeedMoreData;
	}

	// CK_A/CK_B cover Class..end of payload (not the sync bytes)
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(&_buffer[2], UBX_HEADER_LEN - 2 + payload_len, &ck_a, &ck_b);

	ಯದಿ ((ck_a != _buffer[len - 2]) || (ck_b != _buffer[len - 1])) {
		ಪ್ರತ್ಯಾಗಮ Probe::CrcError;
	}

	*frame_len = len;
	ಪ್ರತ್ಯಾಗಮ Probe::Complete;
}

#ಯದಿ defined(CONFIG_GPS_SPARTN)
CorrectionFramer::Probe CorrectionFramer::probeSpartn(size_t *frame_len) ಧ್ರುವ
{
	// Need TF002..TF006 to read the length and CRC type
	ಯದಿ (_buffer_len < SPARTN_FRAME_START_LEN) {
		ಪ್ರತ್ಯಾಗಮ Probe::NeedMoreData;
	}

	ಧ್ರುವ uint8_t b1 = _buffer[1];
	ಧ್ರುವ uint8_t b2 = _buffer[2];
	ಧ್ರುವ uint8_t b3 = _buffer[3];

	// TF002 is the only header field with a checkable range. The 10-bit length
	// spans its full range and TF006 is a 4-bit CRC over a non-byte-aligned
	// field, so this is the only cheap guard before the message CRC.
	ಯದಿ (!spartn_message_type_defined(b1 >> 1)) {
		ಪ್ರತ್ಯಾಗಮ Probe::InvalidHeader;
	}

	ಧ್ರುವ uint16_t payload_len = static_cast<uint16_t>(((b1 & 0x01) << 9) | (b2 << 1) | (b3 >> 7));
	ಧ್ರುವ ತಾರ್ಕಿಕ eaf = ((b3 >> 6) & 0x01) != 0;
	ಧ್ರುವ uint8_t crc_type = (b3 >> 4) & 0x03;
	ಧ್ರುವ size_t crc_bytes = static_cast<size_t>(crc_type) + 1;

	// TF008 (time tag type) sets the description block length
	ಯದಿ (_buffer_len < SPARTN_FRAME_START_LEN + 1) {
		ಪ್ರತ್ಯಾಗಮ Probe::NeedMoreData;
	}

	ಧ್ರುವ ತಾರ್ಕಿಕ time_tag_32bit = ((_buffer[4] >> 3) & 0x01) != 0;
	ಧ್ರುವ size_t desc_len = spartn_desc_length_bytes(eaf, time_tag_32bit);

	ಯದಿ (_buffer_len < SPARTN_FRAME_START_LEN + desc_len) {
		ಪ್ರತ್ಯಾಗಮ Probe::NeedMoreData;
	}

	size_t auth_bytes = 0;

	ಯದಿ (eaf) {
		// TF014/TF015 occupy the low 6 bits of the last description byte
		ಧ್ರುವ uint8_t last = _buffer[SPARTN_FRAME_START_LEN + desc_len - 1];
		auth_bytes = spartn_auth_length_bytes(ಸತ್ಯ, (last >> 3) & 0x07, last & 0x07);
	}

	ಧ್ರುವ size_t len = SPARTN_FRAME_START_LEN + desc_len + payload_len + auth_bytes + crc_bytes;

	ಯದಿ (_buffer_len < len) {
		ಪ್ರತ್ಯಾಗಮ Probe::NeedMoreData;
	}

	// Message CRC (TF018) covers TF002..TF017
	ಧ್ರುವ size_t core_len = len - 1 - crc_bytes;
	ಧ್ರುವ uint32_t calculated_crc = spartn_message_crc(&_buffer[1], core_len, crc_type);
	uint32_t received_crc = 0;

	ಪ್ರತಿ (size_t i = 0; i < crc_bytes; i++) {
		received_crc = (received_crc << 8) | _buffer[len - crc_bytes + i];
	}

	ಯದಿ (calculated_crc != received_crc) {
		ಪ್ರತ್ಯಾಗಮ Probe::CrcError;
	}

	*frame_len = len;
	ಪ್ರತ್ಯಾಗಮ Probe::Complete;
}
#endif // CONFIG_GPS_SPARTN

ಧ್ರುವ uint8_t *CorrectionFramer::getNextMessage(size_t *out_len, CorrectionProtocol *out_protocol)
{
	ಯಾವತ್ (_buffer_len > 0) {
		// Drop everything that cannot start a frame of any supported protocol
		size_t to_drop = 0;

		ಯಾವತ್ ((to_drop < _buffer_len) && !isPreamble(_buffer[to_drop])) {
			to_drop++;
		}

		ಯದಿ (to_drop > 0) {
			_bytes_discarded += to_drop;
			discardBytes(to_drop);
		}

		ಯದಿ (_buffer_len == 0) {
			ಪ್ರತ್ಯಾಗಮ nullptr;
		}

		size_t frame_len = 0;
		CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
		Probe result;

		ಯದಿ (_buffer[0] == UBX_SYNC1) {
			protocol = CorrectionProtocol::Ubx;
			result = probeUbx(&frame_len);

#ಯದಿ defined(CONFIG_GPS_SPARTN)

		} ಅನ್ಯಥಾ ಯದಿ (_buffer[0] == SPARTN_PREAMBLE) {
			protocol = CorrectionProtocol::Spartn;
			result = probeSpartn(&frame_len);

#endif

		} ಅನ್ಯಥಾ {
			// RTCM3 preamble (isPreamble already filtered other bytes)
			protocol = CorrectionProtocol::Rtcm3;
			result = probeRtcm3(&frame_len);
		}

		ವಿಕಲ್ಪ (result) {
		ಪಕ್ಷ Probe::NeedMoreData:
			ಪ್ರತ್ಯಾಗಮ nullptr;

		ಪಕ್ಷ Probe::InvalidHeader:
			_bytes_discarded++;
			discardBytes(1);
			ಅನುವರ್ತ;

		ಪಕ್ಷ Probe::CrcError:
			_crc_errors++;
			discardBytes(1);
			ಅನುವರ್ತ;

		ಪಕ್ಷ Probe::Complete:
			_pending_protocol = protocol;
			*out_len = frame_len;

			ಯದಿ (out_protocol != nullptr) {
				*out_protocol = protocol;
			}

			ಪ್ರತ್ಯಾಗಮ _buffer;
		}
	}

	ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಶೂನ್ಯ CorrectionFramer::consumeMessage(size_t len)
{
	discardBytes(len);
	_messages_parsed++;
	_total_frame_bytes += len;

	ವಿಕಲ್ಪ (_pending_protocol) {
	ಪಕ್ಷ CorrectionProtocol::Rtcm3:
		_rtcm3_messages++;
		ಭಙ್ಗ;

	ಪಕ್ಷ CorrectionProtocol::Spartn:
		_spartn_messages++;
		ಭಙ್ಗ;

	ಪಕ್ಷ CorrectionProtocol::Ubx:
		_ubx_messages++;
		ಭಙ್ಗ;
	}
}

ಶೂನ್ಯ CorrectionFramer::discardBytes(size_t ಎಣಿಕೆ)
{
	ಯದಿ (ಎಣಿಕೆ >= _buffer_len) {
		_buffer_len = 0;

	} ಅನ್ಯಥಾ {
		memmove(_buffer, &_buffer[ಎಣಿಕೆ], _buffer_len - ಎಣಿಕೆ);
		_buffer_len -= ಎಣಿಕೆ;
	}
}

} // namespace gnss
