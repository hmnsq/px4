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

#অন্তর্ভাব "correction_framer.h"

#অন্তর্ভাব <cstring>

নামাকাশ gnss
{

যৌক্তিক CorrectionFramer::isPreamble(uint8_t byte)
{
	যদি (byte == RTCM3_PREAMBLE) {
		ফেরত সত্য;
	}

	যদি (byte == UBX_SYNC1) {
		ফেরত সত্য;
	}

#যদি defined(CONFIG_GPS_SPARTN)

	যদি (byte == SPARTN_PREAMBLE) {
		ফেরত সত্য;
	}

#endif

	ফেরত মিথ্যা;
}

size_t CorrectionFramer::add_তথ্য(ধ্রুবক uint8_t *তথ্য, size_t len)
{
	ধ্রুবক size_t space_available = বাফার_আকার_2 - বাফার_len;
	ধ্রুবক size_t to_copy = (len < space_available) ? len : space_available;

	যদি (to_copy > 0) {
		memcpy(&বাফার[বাফার_len], তথ্য, to_copy);
		বাফার_len += to_copy;
	}

	ফেরত to_copy;
}

CorrectionFramer::Probe CorrectionFramer::probeRtcm3(size_t *frame_len) ধ্রুবক
{
	// Need at least the header to read the length
	যদি (বাফার_len < RTCM3_HEADER_LEN) {
		ফেরত Probe::Need_More_তথ্য;
	}

	// The 10-bit field cannot exceed RTCM3_MAX_PAYLOAD_LEN, so the length alone
	// never rejects a candidate: CRC-24Q is what validates the frame.
	ধ্রুবক size_t payload_len = rtcm3_payload_দৈর্ঘ্য(বাফার);
	ধ্রুবক size_t len = RTCM3_HEADER_LEN + payload_len + RTCM3_CRC_LEN;

	যদি (বাফার_len < len) {
		ফেরত Probe::Need_More_তথ্য;
	}

	ধ্রুবক uint32_t calculated_crc = rtcm3_crc24q(বাফার, RTCM3_HEADER_LEN + payload_len);
	ধ্রুবক uint32_t received_crc = (static_cast<uint32_t>(বাফার[len - 3]) << 16) |
				      (static_cast<uint32_t>(বাফার[len - 2]) << 8) |
				      বাফার[len - 1];

	যদি (calculated_crc != received_crc) {
		ফেরত Probe::CrcError;
	}

	*frame_len = len;
	ফেরত Probe::Complete;
}

CorrectionFramer::Probe CorrectionFramer::probeUbx(size_t *frame_len) ধ্রুবক
{
	// Need sync1 + sync2 at minimum to reject a false preamble quickly
	যদি (বাফার_len < 2) {
		ফেরত Probe::Need_More_তথ্য;
	}

	যদি (বাফার[1] != UBX_SYNC2) {
		ফেরত Probe::InvalidHeader;
	}

	যদি (বাফার_len < UBX_HEADER_LEN) {
		ফেরত Probe::Need_More_তথ্য;
	}

	ধ্রুবক size_t payload_len = static_cast<size_t>(বাফার[4]) |
				   (static_cast<size_t>(বাফার[5]) << 8);

	// Protocol allows larger payloads; reject beyond the injection cap so a
	// garbage length cannot pin the buffer waiting for thousands of bytes.
	যদি (payload_len > UBX_MAX_PAYLOAD_LEN) {
		ফেরত Probe::InvalidHeader;
	}

	ধ্রুবক size_t len = UBX_HEADER_LEN + payload_len + UBX_CK_LEN;

	যদি (বাফার_len < len) {
		ফেরত Probe::Need_More_তথ্য;
	}

	// CK_A/CK_B cover Class..end of payload (not the sync bytes)
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(&বাফার[2], UBX_HEADER_LEN - 2 + payload_len, &ck_a, &ck_b);

	যদি ((ck_a != বাফার[len - 2]) || (ck_b != বাফার[len - 1])) {
		ফেরত Probe::CrcError;
	}

	*frame_len = len;
	ফেরত Probe::Complete;
}

#যদি defined(CONFIG_GPS_SPARTN)
CorrectionFramer::Probe CorrectionFramer::probeSpartn(size_t *frame_len) ধ্রুবক
{
	// Need TF002..TF006 to read the length and CRC type
	যদি (বাফার_len < SPARTN_FRAME_START_LEN) {
		ফেরত Probe::Need_More_তথ্য;
	}

	ধ্রুবক uint8_t b1 = বাফার[1];
	ধ্রুবক uint8_t b2 = বাফার[2];
	ধ্রুবক uint8_t b3 = বাফার[3];

	// TF002 is the only header field with a checkable range. The 10-bit length
	// spans its full range and TF006 is a 4-bit CRC over a non-byte-aligned
	// field, so this is the only cheap guard before the message CRC.
	যদি (!spartn_message_type_defined(b1 >> 1)) {
		ফেরত Probe::InvalidHeader;
	}

	ধ্রুবক uint16_t payload_len = static_cast<uint16_t>(((b1 & 0x01) << 9) | (b2 << 1) | (b3 >> 7));
	ধ্রুবক যৌক্তিক eaf = ((b3 >> 6) & 0x01) != 0;
	ধ্রুবক uint8_t crc_type = (b3 >> 4) & 0x03;
	ধ্রুবক size_t crc_bytes = static_cast<size_t>(crc_type) + 1;

	// TF008 (time tag type) sets the description block length
	যদি (বাফার_len < SPARTN_FRAME_START_LEN + 1) {
		ফেরত Probe::Need_More_তথ্য;
	}

	ধ্রুবক যৌক্তিক time_tag_32bit = ((বাফার[4] >> 3) & 0x01) != 0;
	ধ্রুবক size_t desc_len = spartn_desc_দৈর্ঘ্য_bytes(eaf, time_tag_32bit);

	যদি (বাফার_len < SPARTN_FRAME_START_LEN + desc_len) {
		ফেরত Probe::Need_More_তথ্য;
	}

	size_t auth_bytes = 0;

	যদি (eaf) {
		// TF014/TF015 occupy the low 6 bits of the last description byte
		ধ্রুবক uint8_t last = বাফার[SPARTN_FRAME_START_LEN + desc_len - 1];
		auth_bytes = spartn_auth_দৈর্ঘ্য_bytes(সত্য, (last >> 3) & 0x07, last & 0x07);
	}

	ধ্রুবক size_t len = SPARTN_FRAME_START_LEN + desc_len + payload_len + auth_bytes + crc_bytes;

	যদি (বাফার_len < len) {
		ফেরত Probe::Need_More_তথ্য;
	}

	// Message CRC (TF018) covers TF002..TF017
	ধ্রুবক size_t core_len = len - 1 - crc_bytes;
	ধ্রুবক uint32_t calculated_crc = spartn_message_crc(&বাফার[1], core_len, crc_type);
	uint32_t received_crc = 0;

	জন্য (size_t i = 0; i < crc_bytes; i++) {
		received_crc = (received_crc << 8) | বাফার[len - crc_bytes + i];
	}

	যদি (calculated_crc != received_crc) {
		ফেরত Probe::CrcError;
	}

	*frame_len = len;
	ফেরত Probe::Complete;
}
#endif // CONFIG_GPS_SPARTN

ধ্রুবক uint8_t *CorrectionFramer::get_পরবর্তী_Message(size_t *out_len, CorrectionProtocol *out_protocol)
{
	যখন (বাফার_len > 0) {
		// Drop everything that cannot start a frame of any supported protocol
		size_t to_drop = 0;

		যখন ((to_drop < বাফার_len) && !isPreamble(বাফার[to_drop])) {
			to_drop++;
		}

		যদি (to_drop > 0) {
			_bytes_discarded += to_drop;
			discardBytes(to_drop);
		}

		যদি (বাফার_len == 0) {
			ফেরত nullptr;
		}

		size_t frame_len = 0;
		CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
		Probe ফলাফল;

		যদি (বাফার[0] == UBX_SYNC1) {
			protocol = CorrectionProtocol::Ubx;
			ফলাফল = probeUbx(&frame_len);

#যদি defined(CONFIG_GPS_SPARTN)

		} অন্যথা যদি (বাফার[0] == SPARTN_PREAMBLE) {
			protocol = CorrectionProtocol::Spartn;
			ফলাফল = probeSpartn(&frame_len);

#endif

		} অন্যথা {
			// RTCM3 preamble (isPreamble already filtered other bytes)
			protocol = CorrectionProtocol::Rtcm3;
			ফলাফল = probeRtcm3(&frame_len);
		}

		নির্বাচন (ফলাফল) {
		ক্ষেত্র Probe::Need_More_তথ্য:
			ফেরত nullptr;

		ক্ষেত্র Probe::InvalidHeader:
			_bytes_discarded++;
			discardBytes(1);
			চালু;

		ক্ষেত্র Probe::CrcError:
			_crc_errors++;
			discardBytes(1);
			চালু;

		ক্ষেত্র Probe::Complete:
			_pending_protocol = protocol;
			*out_len = frame_len;

			যদি (out_protocol != nullptr) {
				*out_protocol = protocol;
			}

			ফেরত বাফার;
		}
	}

	ফেরত nullptr;
}

শূন্য CorrectionFramer::consumeMessage(size_t len)
{
	discardBytes(len);
	_messages_parsed++;
	যোগ_frame_bytes += len;

	নির্বাচন (_pending_protocol) {
	ক্ষেত্র CorrectionProtocol::Rtcm3:
		_rtcm3_messages++;
		বিরতি;

	ক্ষেত্র CorrectionProtocol::Spartn:
		_spartn_messages++;
		বিরতি;

	ক্ষেত্র CorrectionProtocol::Ubx:
		_ubx_messages++;
		বিরতি;
	}
}

শূন্য CorrectionFramer::discardBytes(size_t গুনতি)
{
	যদি (গুনতি >= বাফার_len) {
		বাফার_len = 0;

	} অন্যথা {
		memmove(বাফার, &বাফার[গুনতি], বাফার_len - গুনতি);
		বাফার_len -= গুনতি;
	}
}

} // namespace gnss
