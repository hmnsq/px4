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

#ଅନ୍ତର୍ଭାଵ "correction_framer.h"

#ଅନ୍ତର୍ଭାଵ <cstring>

ନାମାକାଶ gnss
{

ତାର୍କିକ CorrectionFramer::isPreamble(uint8_t byte)
{
	ଯଦି (byte == RTCM3_PREAMBLE) {
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (byte == UBX_SYNC1) {
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

#ଯଦି defined(CONFIG_GPS_SPARTN)

	ଯଦି (byte == SPARTN_PREAMBLE) {
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

#endif

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

size_t CorrectionFramer::addData(ଅଚଳ uint8_t *data, size_t len)
{
	ଅଚଳ size_t space_available = BUFFER_SIZE - _buffer_len;
	ଅଚଳ size_t to_copy = (len < space_available) ? len : space_available;

	ଯଦି (to_copy > 0) {
		memcpy(&_buffer[_buffer_len], data, to_copy);
		_buffer_len += to_copy;
	}

	ଫେରନ୍ତୁ to_copy;
}

CorrectionFramer::Probe CorrectionFramer::probeRtcm3(size_t *frame_len) ଅଚଳ
{
	// Need at least the header to read the length
	ଯଦି (_buffer_len < RTCM3_HEADER_LEN) {
		ଫେରନ୍ତୁ Probe::NeedMoreData;
	}

	// The 10-bit field cannot exceed RTCM3_MAX_PAYLOAD_LEN, so the length alone
	// never rejects a candidate: CRC-24Q is what validates the frame.
	ଅଚଳ size_t payload_len = rtcm3_payload_length(_buffer);
	ଅଚଳ size_t len = RTCM3_HEADER_LEN + payload_len + RTCM3_CRC_LEN;

	ଯଦି (_buffer_len < len) {
		ଫେରନ୍ତୁ Probe::NeedMoreData;
	}

	ଅଚଳ uint32_t calculated_crc = rtcm3_crc24q(_buffer, RTCM3_HEADER_LEN + payload_len);
	ଅଚଳ uint32_t received_crc = (static_cast<uint32_t>(_buffer[len - 3]) << 16) |
				      (static_cast<uint32_t>(_buffer[len - 2]) << 8) |
				      _buffer[len - 1];

	ଯଦି (calculated_crc != received_crc) {
		ଫେରନ୍ତୁ Probe::CrcError;
	}

	*frame_len = len;
	ଫେରନ୍ତୁ Probe::Complete;
}

CorrectionFramer::Probe CorrectionFramer::probeUbx(size_t *frame_len) ଅଚଳ
{
	// Need sync1 + sync2 at minimum to reject a false preamble quickly
	ଯଦି (_buffer_len < 2) {
		ଫେରନ୍ତୁ Probe::NeedMoreData;
	}

	ଯଦି (_buffer[1] != UBX_SYNC2) {
		ଫେରନ୍ତୁ Probe::InvalidHeader;
	}

	ଯଦି (_buffer_len < UBX_HEADER_LEN) {
		ଫେରନ୍ତୁ Probe::NeedMoreData;
	}

	ଅଚଳ size_t payload_len = static_cast<size_t>(_buffer[4]) |
				   (static_cast<size_t>(_buffer[5]) << 8);

	// Protocol allows larger payloads; reject beyond the injection cap so a
	// garbage length cannot pin the buffer waiting for thousands of bytes.
	ଯଦି (payload_len > UBX_MAX_PAYLOAD_LEN) {
		ଫେରନ୍ତୁ Probe::InvalidHeader;
	}

	ଅଚଳ size_t len = UBX_HEADER_LEN + payload_len + UBX_CK_LEN;

	ଯଦି (_buffer_len < len) {
		ଫେରନ୍ତୁ Probe::NeedMoreData;
	}

	// CK_A/CK_B cover Class..end of payload (not the sync bytes)
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(&_buffer[2], UBX_HEADER_LEN - 2 + payload_len, &ck_a, &ck_b);

	ଯଦି ((ck_a != _buffer[len - 2]) || (ck_b != _buffer[len - 1])) {
		ଫେରନ୍ତୁ Probe::CrcError;
	}

	*frame_len = len;
	ଫେରନ୍ତୁ Probe::Complete;
}

#ଯଦି defined(CONFIG_GPS_SPARTN)
CorrectionFramer::Probe CorrectionFramer::probeSpartn(size_t *frame_len) ଅଚଳ
{
	// Need TF002..TF006 to read the length and CRC type
	ଯଦି (_buffer_len < SPARTN_FRAME_START_LEN) {
		ଫେରନ୍ତୁ Probe::NeedMoreData;
	}

	ଅଚଳ uint8_t b1 = _buffer[1];
	ଅଚଳ uint8_t b2 = _buffer[2];
	ଅଚଳ uint8_t b3 = _buffer[3];

	// TF002 is the only header field with a checkable range. The 10-bit length
	// spans its full range and TF006 is a 4-bit CRC over a non-byte-aligned
	// field, so this is the only cheap guard before the message CRC.
	ଯଦି (!spartn_message_type_defined(b1 >> 1)) {
		ଫେରନ୍ତୁ Probe::InvalidHeader;
	}

	ଅଚଳ uint16_t payload_len = static_cast<uint16_t>(((b1 & 0x01) << 9) | (b2 << 1) | (b3 >> 7));
	ଅଚଳ ତାର୍କିକ eaf = ((b3 >> 6) & 0x01) != 0;
	ଅଚଳ uint8_t crc_type = (b3 >> 4) & 0x03;
	ଅଚଳ size_t crc_bytes = static_cast<size_t>(crc_type) + 1;

	// TF008 (time tag type) sets the description block length
	ଯଦି (_buffer_len < SPARTN_FRAME_START_LEN + 1) {
		ଫେରନ୍ତୁ Probe::NeedMoreData;
	}

	ଅଚଳ ତାର୍କିକ time_tag_32bit = ((_buffer[4] >> 3) & 0x01) != 0;
	ଅଚଳ size_t desc_len = spartn_desc_length_bytes(eaf, time_tag_32bit);

	ଯଦି (_buffer_len < SPARTN_FRAME_START_LEN + desc_len) {
		ଫେରନ୍ତୁ Probe::NeedMoreData;
	}

	size_t auth_bytes = 0;

	ଯଦି (eaf) {
		// TF014/TF015 occupy the low 6 bits of the last description byte
		ଅଚଳ uint8_t last = _buffer[SPARTN_FRAME_START_LEN + desc_len - 1];
		auth_bytes = spartn_auth_length_bytes(ସତ୍ୟ, (last >> 3) & 0x07, last & 0x07);
	}

	ଅଚଳ size_t len = SPARTN_FRAME_START_LEN + desc_len + payload_len + auth_bytes + crc_bytes;

	ଯଦି (_buffer_len < len) {
		ଫେରନ୍ତୁ Probe::NeedMoreData;
	}

	// Message CRC (TF018) covers TF002..TF017
	ଅଚଳ size_t core_len = len - 1 - crc_bytes;
	ଅଚଳ uint32_t calculated_crc = spartn_message_crc(&_buffer[1], core_len, crc_type);
	uint32_t received_crc = 0;

	ପାଇଁ (size_t i = 0; i < crc_bytes; i++) {
		received_crc = (received_crc << 8) | _buffer[len - crc_bytes + i];
	}

	ଯଦି (calculated_crc != received_crc) {
		ଫେରନ୍ତୁ Probe::CrcError;
	}

	*frame_len = len;
	ଫେରନ୍ତୁ Probe::Complete;
}
#endif // CONFIG_GPS_SPARTN

ଅଚଳ uint8_t *CorrectionFramer::getNextMessage(size_t *out_len, CorrectionProtocol *out_protocol)
{
	ଯେତେବେଳେ (_buffer_len > 0) {
		// Drop everything that cannot start a frame of any supported protocol
		size_t to_drop = 0;

		ଯେତେବେଳେ ((to_drop < _buffer_len) && !isPreamble(_buffer[to_drop])) {
			to_drop++;
		}

		ଯଦି (to_drop > 0) {
			_bytes_discarded += to_drop;
			discardBytes(to_drop);
		}

		ଯଦି (_buffer_len == 0) {
			ଫେରନ୍ତୁ nullptr;
		}

		size_t frame_len = 0;
		CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
		Probe result;

		ଯଦି (_buffer[0] == UBX_SYNC1) {
			protocol = CorrectionProtocol::Ubx;
			result = probeUbx(&frame_len);

#ଯଦି defined(CONFIG_GPS_SPARTN)

		} ଅନ୍ୟଥା ଯଦି (_buffer[0] == SPARTN_PREAMBLE) {
			protocol = CorrectionProtocol::Spartn;
			result = probeSpartn(&frame_len);

#endif

		} ଅନ୍ୟଥା {
			// RTCM3 preamble (isPreamble already filtered other bytes)
			protocol = CorrectionProtocol::Rtcm3;
			result = probeRtcm3(&frame_len);
		}

		ବାଛ (result) {
		ସ୍ଥିତି Probe::NeedMoreData:
			ଫେରନ୍ତୁ nullptr;

		ସ୍ଥିତି Probe::InvalidHeader:
			_bytes_discarded++;
			discardBytes(1);
			ଜାରି;

		ସ୍ଥିତି Probe::CrcError:
			_crc_errors++;
			discardBytes(1);
			ଜାରି;

		ସ୍ଥିତି Probe::Complete:
			_pending_protocol = protocol;
			*out_len = frame_len;

			ଯଦି (out_protocol != nullptr) {
				*out_protocol = protocol;
			}

			ଫେରନ୍ତୁ _buffer;
		}
	}

	ଫେରନ୍ତୁ nullptr;
}

ଶୂନ୍ୟ CorrectionFramer::consumeMessage(size_t len)
{
	discardBytes(len);
	_messages_parsed++;
	_total_frame_bytes += len;

	ବାଛ (_pending_protocol) {
	ସ୍ଥିତି CorrectionProtocol::Rtcm3:
		_rtcm3_messages++;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି CorrectionProtocol::Spartn:
		_spartn_messages++;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି CorrectionProtocol::Ubx:
		_ubx_messages++;
		ଭାଙ୍ଗ;
	}
}

ଶୂନ୍ୟ CorrectionFramer::discardBytes(size_t ଗଣନା_2)
{
	ଯଦି (ଗଣନା_2 >= _buffer_len) {
		_buffer_len = 0;

	} ଅନ୍ୟଥା {
		memmove(_buffer, &_buffer[ଗଣନା_2], _buffer_len - ଗଣନା_2);
		_buffer_len -= ଗଣନା_2;
	}
}

} // namespace gnss
