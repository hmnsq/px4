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

#ضمن "correction_framer.h"

#ضمن <cstring>

نطاق gnss
{

منطقي CorrectionFramer::isPreamble(uint8_t byte)
{
	إذا (byte == RTCM3_PREAMBLE) {
		أرجع درست;
	}

	إذا (byte == UBX_SYNC1) {
		أرجع درست;
	}

#إذا defined(CONFIG_GPS_SPARTN)

	إذا (byte == SPARTN_PREAMBLE) {
		أرجع درست;
	}

#پایان_اگر

	أرجع خطأ;
}

size_t CorrectionFramer::جمع_کن_داده(ثابت uint8_t *داده, size_t len)
{
	ثابت size_t space_موجود = میانگیر_اندازه_2 - میانگیر_len;
	ثابت size_t مقصد_copy = (len < space_موجود) ? len : space_موجود;

	إذا (مقصد_copy > 0) {
		کپی_حافظه(&میانگیر[میانگیر_len], داده, مقصد_copy);
		میانگیر_len += مقصد_copy;
	}

	أرجع مقصد_copy;
}

CorrectionFramer::Probe CorrectionFramer::probeRtcm3(size_t *قاب_len) ثابت
{
	// Need at least the header to read the length
	إذا (میانگیر_len < RTCM3_سرآیند_LEN) {
		أرجع Probe::Need_بیشتر_داده;
	}

	// The 10-bit field cannot exceed RTCM3_MAX_PAYLOAD_LEN, so the length alone
	// never rejects a candidate: CRC-24Q is what validates the frame.
	ثابت size_t payload_len = rtcm3_payload_درازی(میانگیر);
	ثابت size_t len = RTCM3_سرآیند_LEN + payload_len + RTCM3_بررسی_چرخهای_LEN;

	إذا (میانگیر_len < len) {
		أرجع Probe::Need_بیشتر_داده;
	}

	ثابت uint32_t calculated_بررسی_چرخهای = rtcm3_crc24q(میانگیر, RTCM3_سرآیند_LEN + payload_len);
	ثابت uint32_t received_بررسی_چرخهای = (static_cast<uint32_t>(میانگیر[len - 3]) << 16) |
				      (static_cast<uint32_t>(میانگیر[len - 2]) << 8) |
				      میانگیر[len - 1];

	إذا (calculated_بررسی_چرخهای != received_بررسی_چرخهای) {
		أرجع Probe::بررسی_چرخهای_Error;
	}

	*قاب_len = len;
	أرجع Probe::Complete;
}

CorrectionFramer::Probe CorrectionFramer::probeUbx(size_t *قاب_len) ثابت
{
	// Need sync1 + sync2 at minimum to reject a false preamble quickly
	إذا (میانگیر_len < 2) {
		أرجع Probe::Need_بیشتر_داده;
	}

	إذا (میانگیر[1] != UBX_SYNC2) {
		أرجع Probe::Invalid_سرآیند;
	}

	إذا (میانگیر_len < UBX_سرآیند_LEN) {
		أرجع Probe::Need_بیشتر_داده;
	}

	ثابت size_t payload_len = static_cast<size_t>(میانگیر[4]) |
				   (static_cast<size_t>(میانگیر[5]) << 8);

	// Protocol allows larger payloads; reject beyond the injection cap so a
	// garbage length cannot pin the buffer waiting for thousands of bytes.
	إذا (payload_len > UBX_MAX_PAYLOAD_LEN) {
		أرجع Probe::Invalid_سرآیند;
	}

	ثابت size_t len = UBX_سرآیند_LEN + payload_len + UBX_CK_LEN;

	إذا (میانگیر_len < len) {
		أرجع Probe::Need_بیشتر_داده;
	}

	// CK_A/CK_B cover Class..end of payload (not the sync bytes)
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(&میانگیر[2], UBX_سرآیند_LEN - 2 + payload_len, &ck_a, &ck_b);

	إذا ((ck_a != میانگیر[len - 2]) || (ck_b != میانگیر[len - 1])) {
		أرجع Probe::بررسی_چرخهای_Error;
	}

	*قاب_len = len;
	أرجع Probe::Complete;
}

#إذا defined(CONFIG_GPS_SPARTN)
CorrectionFramer::Probe CorrectionFramer::probeSpartn(size_t *قاب_len) ثابت
{
	// Need TF002..TF006 to read the length and CRC type
	إذا (میانگیر_len < SPARTN_قاب_شروع_LEN) {
		أرجع Probe::Need_بیشتر_داده;
	}

	ثابت uint8_t b1 = میانگیر[1];
	ثابت uint8_t b2 = میانگیر[2];
	ثابت uint8_t b3 = میانگیر[3];

	// TF002 is the only header field with a checkable range. The 10-bit length
	// spans its full range and TF006 is a 4-bit CRC over a non-byte-aligned
	// field, so this is the only cheap guard before the message CRC.
	إذا (!spartn_پیام_type_defined(b1 >> 1)) {
		أرجع Probe::Invalid_سرآیند;
	}

	ثابت uint16_t payload_len = static_cast<uint16_t>(((b1 & 0x01) << 9) | (b2 << 1) | (b3 >> 7));
	ثابت منطقي eaf = ((b3 >> 6) & 0x01) != 0;
	ثابت uint8_t بررسی_چرخهای_type = (b3 >> 4) & 0x03;
	ثابت size_t بررسی_چرخهای_بایتها = static_cast<size_t>(بررسی_چرخهای_type) + 1;

	// TF008 (time tag type) sets the description block length
	إذا (میانگیر_len < SPARTN_قاب_شروع_LEN + 1) {
		أرجع Probe::Need_بیشتر_داده;
	}

	ثابت منطقي زمان_برچسب_32bit = ((میانگیر[4] >> 3) & 0x01) != 0;
	ثابت size_t desc_len = spartn_desc_درازی_بایتها(eaf, زمان_برچسب_32bit);

	إذا (میانگیر_len < SPARTN_قاب_شروع_LEN + desc_len) {
		أرجع Probe::Need_بیشتر_داده;
	}

	size_t auth_بایتها = 0;

	إذا (eaf) {
		// TF014/TF015 occupy the low 6 bits of the last description byte
		ثابت uint8_t آخرین = میانگیر[SPARTN_قاب_شروع_LEN + desc_len - 1];
		auth_بایتها = spartn_auth_درازی_بایتها(درست, (آخرین >> 3) & 0x07, آخرین & 0x07);
	}

	ثابت size_t len = SPARTN_قاب_شروع_LEN + desc_len + payload_len + auth_بایتها + بررسی_چرخهای_بایتها;

	إذا (میانگیر_len < len) {
		أرجع Probe::Need_بیشتر_داده;
	}

	// Message CRC (TF018) covers TF002..TF017
	ثابت size_t core_len = len - 1 - بررسی_چرخهای_بایتها;
	ثابت uint32_t calculated_بررسی_چرخهای = spartn_پیام_بررسی_چرخهای(&میانگیر[1], core_len, بررسی_چرخهای_type);
	uint32_t received_بررسی_چرخهای = 0;

	لكل (size_t i = 0; i < بررسی_چرخهای_بایتها; i++) {
		received_بررسی_چرخهای = (received_بررسی_چرخهای << 8) | میانگیر[len - بررسی_چرخهای_بایتها + i];
	}

	إذا (calculated_بررسی_چرخهای != received_بررسی_چرخهای) {
		أرجع Probe::بررسی_چرخهای_Error;
	}

	*قاب_len = len;
	أرجع Probe::Complete;
}
#پایان_اگر // CONFIG_GPS_SPARTN

ثابت uint8_t *CorrectionFramer::get_بعدی_پیام(size_t *بیرون_len, CorrectionProtocol *بیرون_protocol)
{
	طالما (میانگیر_len > 0) {
		// Drop everything that cannot start a frame of any supported protocol
		size_t مقصد_drop = 0;

		طالما ((مقصد_drop < میانگیر_len) && !isPreamble(میانگیر[مقصد_drop])) {
			مقصد_drop++;
		}

		إذا (مقصد_drop > 0) {
			بایتها_discarded += مقصد_drop;
			discard_بایتها(مقصد_drop);
		}

		إذا (میانگیر_len == 0) {
			أرجع اشاره_تهی;
		}

		size_t قاب_len = 0;
		CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
		Probe نتیجه;

		إذا (میانگیر[0] == UBX_SYNC1) {
			protocol = CorrectionProtocol::Ubx;
			نتیجه = probeUbx(&قاب_len);

#إذا defined(CONFIG_GPS_SPARTN)

		} وإلا إذا (میانگیر[0] == SPARTN_PREAMBLE) {
			protocol = CorrectionProtocol::Spartn;
			نتیجه = probeSpartn(&قاب_len);

#پایان_اگر

		} وإلا {
			// RTCM3 preamble (isPreamble already filtered other bytes)
			protocol = CorrectionProtocol::Rtcm3;
			نتیجه = probeRtcm3(&قاب_len);
		}

		بدل (نتیجه) {
		حالة Probe::Need_بیشتر_داده:
			أرجع اشاره_تهی;

		حالة Probe::Invalid_سرآیند:
			بایتها_discarded++;
			discard_بایتها(1);
			استمر;

		حالة Probe::بررسی_چرخهای_Error:
			بررسی_چرخهای_errors++;
			discard_بایتها(1);
			استمر;

		حالة Probe::Complete:
			_pending_protocol = protocol;
			*بیرون_len = قاب_len;

			إذا (بیرون_protocol != اشاره_تهی) {
				*بیرون_protocol = protocol;
			}

			أرجع میانگیر;
		}
	}

	أرجع اشاره_تهی;
}

فراغ CorrectionFramer::consume_پیام(size_t len)
{
	discard_بایتها(len);
	_messages_parsed++;
	مجموع_قاب_بایتها += len;

	بدل (_pending_protocol) {
	حالة CorrectionProtocol::Rtcm3:
		_rtcm3_messages++;
		اكسر;

	حالة CorrectionProtocol::Spartn:
		_spartn_messages++;
		اكسر;

	حالة CorrectionProtocol::Ubx:
		_ubx_messages++;
		اكسر;
	}
}

فراغ CorrectionFramer::discard_بایتها(size_t شمار)
{
	إذا (شمار >= میانگیر_len) {
		میانگیر_len = 0;

	} وإلا {
		memmove(میانگیر, &میانگیر[شمار], میانگیر_len - شمار);
		میانگیر_len -= شمار;
	}
}

} // namespace gnss
