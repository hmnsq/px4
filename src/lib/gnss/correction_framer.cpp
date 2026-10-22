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
		أرجع سچ;
	}

	إذا (byte == UBX_SYNC1) {
		أرجع سچ;
	}

#إذا defined(CONFIG_GPS_SPARTN)

	إذا (byte == SPARTN_PREAMBLE) {
		أرجع سچ;
	}

#جيڪڏهن_پورو

	أرجع خطأ;
}

size_t CorrectionFramer::جوڙيو_معلومات(ثابت uint8_t *معلومات, size_t len)
{
	ثابت size_t space_موجود = بفر_سائيز_2 - بفر_len;
	ثابت size_t منزل_copy = (len < space_موجود) ? len : space_موجود;

	إذا (منزل_copy > 0) {
		يادگيري_نقل(&بفر[بفر_len], معلومات, منزل_copy);
		بفر_len += منزل_copy;
	}

	أرجع منزل_copy;
}

CorrectionFramer::Probe CorrectionFramer::probeRtcm3(size_t *چوکٺو_len) ثابت
{
	// Need at least the header to read the length
	إذا (بفر_len < RTCM3_سرو_LEN) {
		أرجع Probe::Need_وڌيڪ_معلومات;
	}

	// The 10-bit field cannot exceed RTCM3_MAX_PAYLOAD_LEN, so the length alone
	// never rejects a candidate: CRC-24Q is what validates the frame.
	ثابت size_t payload_len = rtcm3_payload_ڊيگهائي(بفر);
	ثابت size_t len = RTCM3_سرو_LEN + payload_len + RTCM3_چڪري_جاچ_LEN;

	إذا (بفر_len < len) {
		أرجع Probe::Need_وڌيڪ_معلومات;
	}

	ثابت uint32_t calculated_چڪري_جاچ = rtcm3_crc24q(بفر, RTCM3_سرو_LEN + payload_len);
	ثابت uint32_t received_چڪري_جاچ = (static_cast<uint32_t>(بفر[len - 3]) << 16) |
				      (static_cast<uint32_t>(بفر[len - 2]) << 8) |
				      بفر[len - 1];

	إذا (calculated_چڪري_جاچ != received_چڪري_جاچ) {
		أرجع Probe::چڪري_جاچ_Error;
	}

	*چوکٺو_len = len;
	أرجع Probe::Complete;
}

CorrectionFramer::Probe CorrectionFramer::probeUbx(size_t *چوکٺو_len) ثابت
{
	// Need sync1 + sync2 at minimum to reject a false preamble quickly
	إذا (بفر_len < 2) {
		أرجع Probe::Need_وڌيڪ_معلومات;
	}

	إذا (بفر[1] != UBX_SYNC2) {
		أرجع Probe::Invalid_سرو;
	}

	إذا (بفر_len < UBX_سرو_LEN) {
		أرجع Probe::Need_وڌيڪ_معلومات;
	}

	ثابت size_t payload_len = static_cast<size_t>(بفر[4]) |
				   (static_cast<size_t>(بفر[5]) << 8);

	// Protocol allows larger payloads; reject beyond the injection cap so a
	// garbage length cannot pin the buffer waiting for thousands of bytes.
	إذا (payload_len > UBX_MAX_PAYLOAD_LEN) {
		أرجع Probe::Invalid_سرو;
	}

	ثابت size_t len = UBX_سرو_LEN + payload_len + UBX_CK_LEN;

	إذا (بفر_len < len) {
		أرجع Probe::Need_وڌيڪ_معلومات;
	}

	// CK_A/CK_B cover Class..end of payload (not the sync bytes)
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(&بفر[2], UBX_سرو_LEN - 2 + payload_len, &ck_a, &ck_b);

	إذا ((ck_a != بفر[len - 2]) || (ck_b != بفر[len - 1])) {
		أرجع Probe::چڪري_جاچ_Error;
	}

	*چوکٺو_len = len;
	أرجع Probe::Complete;
}

#إذا defined(CONFIG_GPS_SPARTN)
CorrectionFramer::Probe CorrectionFramer::probeSpartn(size_t *چوکٺو_len) ثابت
{
	// Need TF002..TF006 to read the length and CRC type
	إذا (بفر_len < SPARTN_چوکٺو_شروعات_LEN) {
		أرجع Probe::Need_وڌيڪ_معلومات;
	}

	ثابت uint8_t b1 = بفر[1];
	ثابت uint8_t b2 = بفر[2];
	ثابت uint8_t b3 = بفر[3];

	// TF002 is the only header field with a checkable range. The 10-bit length
	// spans its full range and TF006 is a 4-bit CRC over a non-byte-aligned
	// field, so this is the only cheap guard before the message CRC.
	إذا (!spartn_پيغام_type_defined(b1 >> 1)) {
		أرجع Probe::Invalid_سرو;
	}

	ثابت uint16_t payload_len = static_cast<uint16_t>(((b1 & 0x01) << 9) | (b2 << 1) | (b3 >> 7));
	ثابت منطقي eaf = ((b3 >> 6) & 0x01) != 0;
	ثابت uint8_t چڪري_جاچ_type = (b3 >> 4) & 0x03;
	ثابت size_t چڪري_جاچ_بائيٽون = static_cast<size_t>(چڪري_جاچ_type) + 1;

	// TF008 (time tag type) sets the description block length
	إذا (بفر_len < SPARTN_چوکٺو_شروعات_LEN + 1) {
		أرجع Probe::Need_وڌيڪ_معلومات;
	}

	ثابت منطقي وقت_ٽيگ_32bit = ((بفر[4] >> 3) & 0x01) != 0;
	ثابت size_t desc_len = spartn_desc_ڊيگهائي_بائيٽون(eaf, وقت_ٽيگ_32bit);

	إذا (بفر_len < SPARTN_چوکٺو_شروعات_LEN + desc_len) {
		أرجع Probe::Need_وڌيڪ_معلومات;
	}

	size_t auth_بائيٽون = 0;

	إذا (eaf) {
		// TF014/TF015 occupy the low 6 bits of the last description byte
		ثابت uint8_t آخري = بفر[SPARTN_چوکٺو_شروعات_LEN + desc_len - 1];
		auth_بائيٽون = spartn_auth_ڊيگهائي_بائيٽون(سچ, (آخري >> 3) & 0x07, آخري & 0x07);
	}

	ثابت size_t len = SPARTN_چوکٺو_شروعات_LEN + desc_len + payload_len + auth_بائيٽون + چڪري_جاچ_بائيٽون;

	إذا (بفر_len < len) {
		أرجع Probe::Need_وڌيڪ_معلومات;
	}

	// Message CRC (TF018) covers TF002..TF017
	ثابت size_t core_len = len - 1 - چڪري_جاچ_بائيٽون;
	ثابت uint32_t calculated_چڪري_جاچ = spartn_پيغام_چڪري_جاچ(&بفر[1], core_len, چڪري_جاچ_type);
	uint32_t received_چڪري_جاچ = 0;

	لكل (size_t i = 0; i < چڪري_جاچ_بائيٽون; i++) {
		received_چڪري_جاچ = (received_چڪري_جاچ << 8) | بفر[len - چڪري_جاچ_بائيٽون + i];
	}

	إذا (calculated_چڪري_جاچ != received_چڪري_جاچ) {
		أرجع Probe::چڪري_جاچ_Error;
	}

	*چوکٺو_len = len;
	أرجع Probe::Complete;
}
#جيڪڏهن_پورو // CONFIG_GPS_SPARTN

ثابت uint8_t *CorrectionFramer::get_ايندڙ_پيغام(size_t *ٻاهريون_len, CorrectionProtocol *ٻاهريون_protocol)
{
	طالما (بفر_len > 0) {
		// Drop everything that cannot start a frame of any supported protocol
		size_t منزل_drop = 0;

		طالما ((منزل_drop < بفر_len) && !isPreamble(بفر[منزل_drop])) {
			منزل_drop++;
		}

		إذا (منزل_drop > 0) {
			بائيٽون_discarded += منزل_drop;
			discard_بائيٽون(منزل_drop);
		}

		إذا (بفر_len == 0) {
			أرجع خالي_اشارو;
		}

		size_t چوکٺو_len = 0;
		CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
		Probe نتيجو;

		إذا (بفر[0] == UBX_SYNC1) {
			protocol = CorrectionProtocol::Ubx;
			نتيجو = probeUbx(&چوکٺو_len);

#إذا defined(CONFIG_GPS_SPARTN)

		} وإلا إذا (بفر[0] == SPARTN_PREAMBLE) {
			protocol = CorrectionProtocol::Spartn;
			نتيجو = probeSpartn(&چوکٺو_len);

#جيڪڏهن_پورو

		} وإلا {
			// RTCM3 preamble (isPreamble already filtered other bytes)
			protocol = CorrectionProtocol::Rtcm3;
			نتيجو = probeRtcm3(&چوکٺو_len);
		}

		بدل (نتيجو) {
		حالة Probe::Need_وڌيڪ_معلومات:
			أرجع خالي_اشارو;

		حالة Probe::Invalid_سرو:
			بائيٽون_discarded++;
			discard_بائيٽون(1);
			استمر;

		حالة Probe::چڪري_جاچ_Error:
			چڪري_جاچ_errors++;
			discard_بائيٽون(1);
			استمر;

		حالة Probe::Complete:
			_pending_protocol = protocol;
			*ٻاهريون_len = چوکٺو_len;

			إذا (ٻاهريون_protocol != خالي_اشارو) {
				*ٻاهريون_protocol = protocol;
			}

			أرجع بفر;
		}
	}

	أرجع خالي_اشارو;
}

فراغ CorrectionFramer::consume_پيغام(size_t len)
{
	discard_بائيٽون(len);
	_messages_parsed++;
	ڪل_چوکٺو_بائيٽون += len;

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

فراغ CorrectionFramer::discard_بائيٽون(size_t ڳڻپ)
{
	إذا (ڳڻپ >= بفر_len) {
		بفر_len = 0;

	} وإلا {
		memmove(بفر, &بفر[ڳڻپ], بفر_len - ڳڻپ);
		بفر_len -= ڳڻپ;
	}
}

} // namespace gnss
