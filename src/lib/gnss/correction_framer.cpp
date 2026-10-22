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

#ਅਨ੍ਤਰ੍ਭਾਵ "correction_framer.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>

ਨਾਮਾਕਾਸ਼ gnss
{

ਤਾਰਕਿਕ CorrectionFramer::isPreamble(uint8_t byte)
{
	ਜੇ (byte == RTCM3_PREAMBLE) {
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (byte == UBX_SYNC1) {
		ਵਾਪਸ ਸੱਚ;
	}

#ਜੇ defined(CONFIG_GPS_SPARTN)

	ਜੇ (byte == SPARTN_PREAMBLE) {
		ਵਾਪਸ ਸੱਚ;
	}

#ਜੇ_ਅੰਤ

	ਵਾਪਸ ਝੂਠ;
}

size_t CorrectionFramer::ਜੋੜੋ_ਅੰਕੜੇ(ਅਚਲ uint8_t *ਅੰਕੜੇ, size_t len)
{
	ਅਚਲ size_t space_ਉਪਲਬਧ = ਬਫਰ_ਮਾਪ_2 - ਬਫਰ_len;
	ਅਚਲ size_t ਮੰਜ਼ਿਲ_copy = (len < space_ਉਪਲਬਧ) ? len : space_ਉਪਲਬਧ;

	ਜੇ (ਮੰਜ਼ਿਲ_copy > 0) {
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ਬਫਰ[ਬਫਰ_len], ਅੰਕੜੇ, ਮੰਜ਼ਿਲ_copy);
		ਬਫਰ_len += ਮੰਜ਼ਿਲ_copy;
	}

	ਵਾਪਸ ਮੰਜ਼ਿਲ_copy;
}

CorrectionFramer::Probe CorrectionFramer::probeRtcm3(size_t *ਚੌਖਟਾ_len) ਅਚਲ
{
	// Need at least the header to read the length
	ਜੇ (ਬਫਰ_len < RTCM3_ਸਿਰਲੇਖ_LEN) {
		ਵਾਪਸ Probe::Need_ਹੋਰ_ਅੰਕੜੇ;
	}

	// The 10-bit field cannot exceed RTCM3_MAX_PAYLOAD_LEN, so the length alone
	// never rejects a candidate: CRC-24Q is what validates the frame.
	ਅਚਲ size_t payload_len = rtcm3_payload_ਲੰਮਾਈ(ਬਫਰ);
	ਅਚਲ size_t len = RTCM3_ਸਿਰਲੇਖ_LEN + payload_len + RTCM3_ਚੱਕਰੀ_ਜਾਂਚ_LEN;

	ਜੇ (ਬਫਰ_len < len) {
		ਵਾਪਸ Probe::Need_ਹੋਰ_ਅੰਕੜੇ;
	}

	ਅਚਲ uint32_t calculated_ਚੱਕਰੀ_ਜਾਂਚ = rtcm3_crc24q(ਬਫਰ, RTCM3_ਸਿਰਲੇਖ_LEN + payload_len);
	ਅਚਲ uint32_t received_ਚੱਕਰੀ_ਜਾਂਚ = (static_cast<uint32_t>(ਬਫਰ[len - 3]) << 16) |
				      (static_cast<uint32_t>(ਬਫਰ[len - 2]) << 8) |
				      ਬਫਰ[len - 1];

	ਜੇ (calculated_ਚੱਕਰੀ_ਜਾਂਚ != received_ਚੱਕਰੀ_ਜਾਂਚ) {
		ਵਾਪਸ Probe::ਚੱਕਰੀ_ਜਾਂਚ_Error;
	}

	*ਚੌਖਟਾ_len = len;
	ਵਾਪਸ Probe::Complete;
}

CorrectionFramer::Probe CorrectionFramer::probeUbx(size_t *ਚੌਖਟਾ_len) ਅਚਲ
{
	// Need sync1 + sync2 at minimum to reject a false preamble quickly
	ਜੇ (ਬਫਰ_len < 2) {
		ਵਾਪਸ Probe::Need_ਹੋਰ_ਅੰਕੜੇ;
	}

	ਜੇ (ਬਫਰ[1] != UBX_SYNC2) {
		ਵਾਪਸ Probe::Invalid_ਸਿਰਲੇਖ;
	}

	ਜੇ (ਬਫਰ_len < UBX_ਸਿਰਲੇਖ_LEN) {
		ਵਾਪਸ Probe::Need_ਹੋਰ_ਅੰਕੜੇ;
	}

	ਅਚਲ size_t payload_len = static_cast<size_t>(ਬਫਰ[4]) |
				   (static_cast<size_t>(ਬਫਰ[5]) << 8);

	// Protocol allows larger payloads; reject beyond the injection cap so a
	// garbage length cannot pin the buffer waiting for thousands of bytes.
	ਜੇ (payload_len > UBX_MAX_PAYLOAD_LEN) {
		ਵਾਪਸ Probe::Invalid_ਸਿਰਲੇਖ;
	}

	ਅਚਲ size_t len = UBX_ਸਿਰਲੇਖ_LEN + payload_len + UBX_CK_LEN;

	ਜੇ (ਬਫਰ_len < len) {
		ਵਾਪਸ Probe::Need_ਹੋਰ_ਅੰਕੜੇ;
	}

	// CK_A/CK_B cover Class..end of payload (not the sync bytes)
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(&ਬਫਰ[2], UBX_ਸਿਰਲੇਖ_LEN - 2 + payload_len, &ck_a, &ck_b);

	ਜੇ ((ck_a != ਬਫਰ[len - 2]) || (ck_b != ਬਫਰ[len - 1])) {
		ਵਾਪਸ Probe::ਚੱਕਰੀ_ਜਾਂਚ_Error;
	}

	*ਚੌਖਟਾ_len = len;
	ਵਾਪਸ Probe::Complete;
}

#ਜੇ defined(CONFIG_GPS_SPARTN)
CorrectionFramer::Probe CorrectionFramer::probeSpartn(size_t *ਚੌਖਟਾ_len) ਅਚਲ
{
	// Need TF002..TF006 to read the length and CRC type
	ਜੇ (ਬਫਰ_len < SPARTN_ਚੌਖਟਾ_ਸ਼ੁਰੂ_LEN) {
		ਵਾਪਸ Probe::Need_ਹੋਰ_ਅੰਕੜੇ;
	}

	ਅਚਲ uint8_t b1 = ਬਫਰ[1];
	ਅਚਲ uint8_t b2 = ਬਫਰ[2];
	ਅਚਲ uint8_t b3 = ਬਫਰ[3];

	// TF002 is the only header field with a checkable range. The 10-bit length
	// spans its full range and TF006 is a 4-bit CRC over a non-byte-aligned
	// field, so this is the only cheap guard before the message CRC.
	ਜੇ (!spartn_ਸੁਨੇਹਾ_type_defined(b1 >> 1)) {
		ਵਾਪਸ Probe::Invalid_ਸਿਰਲੇਖ;
	}

	ਅਚਲ uint16_t payload_len = static_cast<uint16_t>(((b1 & 0x01) << 9) | (b2 << 1) | (b3 >> 7));
	ਅਚਲ ਤਾਰਕਿਕ eaf = ((b3 >> 6) & 0x01) != 0;
	ਅਚਲ uint8_t ਚੱਕਰੀ_ਜਾਂਚ_type = (b3 >> 4) & 0x03;
	ਅਚਲ size_t ਚੱਕਰੀ_ਜਾਂਚ_ਬਾਈਟਾਂ = static_cast<size_t>(ਚੱਕਰੀ_ਜਾਂਚ_type) + 1;

	// TF008 (time tag type) sets the description block length
	ਜੇ (ਬਫਰ_len < SPARTN_ਚੌਖਟਾ_ਸ਼ੁਰੂ_LEN + 1) {
		ਵਾਪਸ Probe::Need_ਹੋਰ_ਅੰਕੜੇ;
	}

	ਅਚਲ ਤਾਰਕਿਕ ਸਮਾਂ_ਟੈਗ_32bit = ((ਬਫਰ[4] >> 3) & 0x01) != 0;
	ਅਚਲ size_t desc_len = spartn_desc_ਲੰਮਾਈ_ਬਾਈਟਾਂ(eaf, ਸਮਾਂ_ਟੈਗ_32bit);

	ਜੇ (ਬਫਰ_len < SPARTN_ਚੌਖਟਾ_ਸ਼ੁਰੂ_LEN + desc_len) {
		ਵਾਪਸ Probe::Need_ਹੋਰ_ਅੰਕੜੇ;
	}

	size_t auth_ਬਾਈਟਾਂ = 0;

	ਜੇ (eaf) {
		// TF014/TF015 occupy the low 6 bits of the last description byte
		ਅਚਲ uint8_t ਆਖਰੀ = ਬਫਰ[SPARTN_ਚੌਖਟਾ_ਸ਼ੁਰੂ_LEN + desc_len - 1];
		auth_ਬਾਈਟਾਂ = spartn_auth_ਲੰਮਾਈ_ਬਾਈਟਾਂ(ਸੱਚ, (ਆਖਰੀ >> 3) & 0x07, ਆਖਰੀ & 0x07);
	}

	ਅਚਲ size_t len = SPARTN_ਚੌਖਟਾ_ਸ਼ੁਰੂ_LEN + desc_len + payload_len + auth_ਬਾਈਟਾਂ + ਚੱਕਰੀ_ਜਾਂਚ_ਬਾਈਟਾਂ;

	ਜੇ (ਬਫਰ_len < len) {
		ਵਾਪਸ Probe::Need_ਹੋਰ_ਅੰਕੜੇ;
	}

	// Message CRC (TF018) covers TF002..TF017
	ਅਚਲ size_t core_len = len - 1 - ਚੱਕਰੀ_ਜਾਂਚ_ਬਾਈਟਾਂ;
	ਅਚਲ uint32_t calculated_ਚੱਕਰੀ_ਜਾਂਚ = spartn_ਸੁਨੇਹਾ_ਚੱਕਰੀ_ਜਾਂਚ(&ਬਫਰ[1], core_len, ਚੱਕਰੀ_ਜਾਂਚ_type);
	uint32_t received_ਚੱਕਰੀ_ਜਾਂਚ = 0;

	ਲਈ (size_t i = 0; i < ਚੱਕਰੀ_ਜਾਂਚ_ਬਾਈਟਾਂ; i++) {
		received_ਚੱਕਰੀ_ਜਾਂਚ = (received_ਚੱਕਰੀ_ਜਾਂਚ << 8) | ਬਫਰ[len - ਚੱਕਰੀ_ਜਾਂਚ_ਬਾਈਟਾਂ + i];
	}

	ਜੇ (calculated_ਚੱਕਰੀ_ਜਾਂਚ != received_ਚੱਕਰੀ_ਜਾਂਚ) {
		ਵਾਪਸ Probe::ਚੱਕਰੀ_ਜਾਂਚ_Error;
	}

	*ਚੌਖਟਾ_len = len;
	ਵਾਪਸ Probe::Complete;
}
#ਜੇ_ਅੰਤ // CONFIG_GPS_SPARTN

ਅਚਲ uint8_t *CorrectionFramer::get_ਅਗਲਾ_ਸੁਨੇਹਾ(size_t *ਬਾਹਰਲਾ_len, CorrectionProtocol *ਬਾਹਰਲਾ_protocol)
{
	ਜਦੋਂ (ਬਫਰ_len > 0) {
		// Drop everything that cannot start a frame of any supported protocol
		size_t ਮੰਜ਼ਿਲ_drop = 0;

		ਜਦੋਂ ((ਮੰਜ਼ਿਲ_drop < ਬਫਰ_len) && !isPreamble(ਬਫਰ[ਮੰਜ਼ਿਲ_drop])) {
			ਮੰਜ਼ਿਲ_drop++;
		}

		ਜੇ (ਮੰਜ਼ਿਲ_drop > 0) {
			ਬਾਈਟਾਂ_discarded += ਮੰਜ਼ਿਲ_drop;
			discard_ਬਾਈਟਾਂ(ਮੰਜ਼ਿਲ_drop);
		}

		ਜੇ (ਬਫਰ_len == 0) {
			ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
		}

		size_t ਚੌਖਟਾ_len = 0;
		CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
		Probe ਨਤੀਜਾ;

		ਜੇ (ਬਫਰ[0] == UBX_SYNC1) {
			protocol = CorrectionProtocol::Ubx;
			ਨਤੀਜਾ = probeUbx(&ਚੌਖਟਾ_len);

#ਜੇ defined(CONFIG_GPS_SPARTN)

		} ਨਹੀਂਤਾਂ ਜੇ (ਬਫਰ[0] == SPARTN_PREAMBLE) {
			protocol = CorrectionProtocol::Spartn;
			ਨਤੀਜਾ = probeSpartn(&ਚੌਖਟਾ_len);

#ਜੇ_ਅੰਤ

		} ਨਹੀਂਤਾਂ {
			// RTCM3 preamble (isPreamble already filtered other bytes)
			protocol = CorrectionProtocol::Rtcm3;
			ਨਤੀਜਾ = probeRtcm3(&ਚੌਖਟਾ_len);
		}

		ਚੋਣ (ਨਤੀਜਾ) {
		ਹਾਲਤ Probe::Need_ਹੋਰ_ਅੰਕੜੇ:
			ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

		ਹਾਲਤ Probe::Invalid_ਸਿਰਲੇਖ:
			ਬਾਈਟਾਂ_discarded++;
			discard_ਬਾਈਟਾਂ(1);
			ਜਾਰੀ;

		ਹਾਲਤ Probe::ਚੱਕਰੀ_ਜਾਂਚ_Error:
			ਚੱਕਰੀ_ਜਾਂਚ_errors++;
			discard_ਬਾਈਟਾਂ(1);
			ਜਾਰੀ;

		ਹਾਲਤ Probe::Complete:
			_pending_protocol = protocol;
			*ਬਾਹਰਲਾ_len = ਚੌਖਟਾ_len;

			ਜੇ (ਬਾਹਰਲਾ_protocol != ਖਾਲੀ_ਸੂਚਕ) {
				*ਬਾਹਰਲਾ_protocol = protocol;
			}

			ਵਾਪਸ ਬਫਰ;
		}
	}

	ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਖਾਲੀ CorrectionFramer::consume_ਸੁਨੇਹਾ(size_t len)
{
	discard_ਬਾਈਟਾਂ(len);
	_messages_parsed++;
	ਕੁੱਲ_ਚੌਖਟਾ_ਬਾਈਟਾਂ += len;

	ਚੋਣ (_pending_protocol) {
	ਹਾਲਤ CorrectionProtocol::Rtcm3:
		_rtcm3_messages++;
		ਰੋਕੋ;

	ਹਾਲਤ CorrectionProtocol::Spartn:
		_spartn_messages++;
		ਰੋਕੋ;

	ਹਾਲਤ CorrectionProtocol::Ubx:
		_ubx_messages++;
		ਰੋਕੋ;
	}
}

ਖਾਲੀ CorrectionFramer::discard_ਬਾਈਟਾਂ(size_t ਗਿਣਤੀ_2)
{
	ਜੇ (ਗਿਣਤੀ_2 >= ਬਫਰ_len) {
		ਬਫਰ_len = 0;

	} ਨਹੀਂਤਾਂ {
		memmove(ਬਫਰ, &ਬਫਰ[ਗਿਣਤੀ_2], ਬਫਰ_len - ਗਿਣਤੀ_2);
		ਬਫਰ_len -= ਗਿਣਤੀ_2;
	}
}

} // namespace gnss
