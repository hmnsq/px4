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
	ಆದರೆ (byte == RTCM3_PREAMBLE) {
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (byte == UBX_SYNC1) {
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

#ಆದರೆ defined(CONFIG_GPS_SPARTN)

	ಆದರೆ (byte == SPARTN_PREAMBLE) {
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

size_t CorrectionFramer::ಕೂಡಿಸು_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ uint8_t *ದತ್ತಾಂಶ, size_t len)
{
	ಸ್ಥಿರಾಂಕ size_t space_ಲಭ್ಯ = ಬಫರ್_ಗಾತ್ರ_2 - ಬಫರ್_len;
	ಸ್ಥಿರಾಂಕ size_t ಗಮ್ಯ_copy = (len < space_ಲಭ್ಯ) ? len : space_ಲಭ್ಯ;

	ಆದರೆ (ಗಮ್ಯ_copy > 0) {
		ಸ್ಮರಣೆ_ನಕಲು(&ಬಫರ್[ಬಫರ್_len], ದತ್ತಾಂಶ, ಗಮ್ಯ_copy);
		ಬಫರ್_len += ಗಮ್ಯ_copy;
	}

	ಹಿಂತಿರುಗಿ ಗಮ್ಯ_copy;
}

CorrectionFramer::Probe CorrectionFramer::probeRtcm3(size_t *ಚೌಕಟ್ಟು_len) ಸ್ಥಿರಾಂಕ
{
	// Need at least the header to read the length
	ಆದರೆ (ಬಫರ್_len < RTCM3_ಶೀರ್ಷಿಕೆ_LEN) {
		ಹಿಂತಿರುಗಿ Probe::Need_ಇನ್ನಷ್ಟು_ದತ್ತಾಂಶ;
	}

	// The 10-bit field cannot exceed RTCM3_MAX_PAYLOAD_LEN, so the length alone
	// never rejects a candidate: CRC-24Q is what validates the frame.
	ಸ್ಥಿರಾಂಕ size_t payload_len = rtcm3_payload_ಉದ್ದಳತೆ(ಬಫರ್);
	ಸ್ಥಿರಾಂಕ size_t len = RTCM3_ಶೀರ್ಷಿಕೆ_LEN + payload_len + RTCM3_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_LEN;

	ಆದರೆ (ಬಫರ್_len < len) {
		ಹಿಂತಿರುಗಿ Probe::Need_ಇನ್ನಷ್ಟು_ದತ್ತಾಂಶ;
	}

	ಸ್ಥಿರಾಂಕ uint32_t calculated_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = rtcm3_crc24q(ಬಫರ್, RTCM3_ಶೀರ್ಷಿಕೆ_LEN + payload_len);
	ಸ್ಥಿರಾಂಕ uint32_t received_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = (static_cast<uint32_t>(ಬಫರ್[len - 3]) << 16) |
				      (static_cast<uint32_t>(ಬಫರ್[len - 2]) << 8) |
				      ಬಫರ್[len - 1];

	ಆದರೆ (calculated_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ != received_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ) {
		ಹಿಂತಿರುಗಿ Probe::ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_Error;
	}

	*ಚೌಕಟ್ಟು_len = len;
	ಹಿಂತಿರುಗಿ Probe::Complete;
}

CorrectionFramer::Probe CorrectionFramer::probeUbx(size_t *ಚೌಕಟ್ಟು_len) ಸ್ಥಿರಾಂಕ
{
	// Need sync1 + sync2 at minimum to reject a false preamble quickly
	ಆದರೆ (ಬಫರ್_len < 2) {
		ಹಿಂತಿರುಗಿ Probe::Need_ಇನ್ನಷ್ಟು_ದತ್ತಾಂಶ;
	}

	ಆದರೆ (ಬಫರ್[1] != UBX_SYNC2) {
		ಹಿಂತಿರುಗಿ Probe::Invalid_ಶೀರ್ಷಿಕೆ;
	}

	ಆದರೆ (ಬಫರ್_len < UBX_ಶೀರ್ಷಿಕೆ_LEN) {
		ಹಿಂತಿರುಗಿ Probe::Need_ಇನ್ನಷ್ಟು_ದತ್ತಾಂಶ;
	}

	ಸ್ಥಿರಾಂಕ size_t payload_len = static_cast<size_t>(ಬಫರ್[4]) |
				   (static_cast<size_t>(ಬಫರ್[5]) << 8);

	// Protocol allows larger payloads; reject beyond the injection cap so a
	// garbage length cannot pin the buffer waiting for thousands of bytes.
	ಆದರೆ (payload_len > UBX_MAX_PAYLOAD_LEN) {
		ಹಿಂತಿರುಗಿ Probe::Invalid_ಶೀರ್ಷಿಕೆ;
	}

	ಸ್ಥಿರಾಂಕ size_t len = UBX_ಶೀರ್ಷಿಕೆ_LEN + payload_len + UBX_CK_LEN;

	ಆದರೆ (ಬಫರ್_len < len) {
		ಹಿಂತಿರುಗಿ Probe::Need_ಇನ್ನಷ್ಟು_ದತ್ತಾಂಶ;
	}

	// CK_A/CK_B cover Class..end of payload (not the sync bytes)
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(&ಬಫರ್[2], UBX_ಶೀರ್ಷಿಕೆ_LEN - 2 + payload_len, &ck_a, &ck_b);

	ಆದರೆ ((ck_a != ಬಫರ್[len - 2]) || (ck_b != ಬಫರ್[len - 1])) {
		ಹಿಂತಿರುಗಿ Probe::ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_Error;
	}

	*ಚೌಕಟ್ಟು_len = len;
	ಹಿಂತಿರುಗಿ Probe::Complete;
}

#ಆದರೆ defined(CONFIG_GPS_SPARTN)
CorrectionFramer::Probe CorrectionFramer::probeSpartn(size_t *ಚೌಕಟ್ಟು_len) ಸ್ಥಿರಾಂಕ
{
	// Need TF002..TF006 to read the length and CRC type
	ಆದರೆ (ಬಫರ್_len < SPARTN_ಚೌಕಟ್ಟು_ಆರಂಭ_LEN) {
		ಹಿಂತಿರುಗಿ Probe::Need_ಇನ್ನಷ್ಟು_ದತ್ತಾಂಶ;
	}

	ಸ್ಥಿರಾಂಕ uint8_t b1 = ಬಫರ್[1];
	ಸ್ಥಿರಾಂಕ uint8_t b2 = ಬಫರ್[2];
	ಸ್ಥಿರಾಂಕ uint8_t b3 = ಬಫರ್[3];

	// TF002 is the only header field with a checkable range. The 10-bit length
	// spans its full range and TF006 is a 4-bit CRC over a non-byte-aligned
	// field, so this is the only cheap guard before the message CRC.
	ಆದರೆ (!spartn_ಸಂದೇಶ_type_defined(b1 >> 1)) {
		ಹಿಂತಿರುಗಿ Probe::Invalid_ಶೀರ್ಷಿಕೆ;
	}

	ಸ್ಥಿರಾಂಕ uint16_t payload_len = static_cast<uint16_t>(((b1 & 0x01) << 9) | (b2 << 1) | (b3 >> 7));
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ eaf = ((b3 >> 6) & 0x01) != 0;
	ಸ್ಥಿರಾಂಕ uint8_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type = (b3 >> 4) & 0x03;
	ಸ್ಥಿರಾಂಕ size_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಬೈಟುಗಳು = static_cast<size_t>(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type) + 1;

	// TF008 (time tag type) sets the description block length
	ಆದರೆ (ಬಫರ್_len < SPARTN_ಚೌಕಟ್ಟು_ಆರಂಭ_LEN + 1) {
		ಹಿಂತಿರುಗಿ Probe::Need_ಇನ್ನಷ್ಟು_ದತ್ತಾಂಶ;
	}

	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ ಸಮಯ_ಟ್ಯಾಗ್_32bit = ((ಬಫರ್[4] >> 3) & 0x01) != 0;
	ಸ್ಥಿರಾಂಕ size_t desc_len = spartn_desc_ಉದ್ದಳತೆ_ಬೈಟುಗಳು(eaf, ಸಮಯ_ಟ್ಯಾಗ್_32bit);

	ಆದರೆ (ಬಫರ್_len < SPARTN_ಚೌಕಟ್ಟು_ಆರಂಭ_LEN + desc_len) {
		ಹಿಂತಿರುಗಿ Probe::Need_ಇನ್ನಷ್ಟು_ದತ್ತಾಂಶ;
	}

	size_t auth_ಬೈಟುಗಳು = 0;

	ಆದರೆ (eaf) {
		// TF014/TF015 occupy the low 6 bits of the last description byte
		ಸ್ಥಿರಾಂಕ uint8_t ಕೊನೆಯ = ಬಫರ್[SPARTN_ಚೌಕಟ್ಟು_ಆರಂಭ_LEN + desc_len - 1];
		auth_ಬೈಟುಗಳು = spartn_auth_ಉದ್ದಳತೆ_ಬೈಟುಗಳು(ನಿಜ, (ಕೊನೆಯ >> 3) & 0x07, ಕೊನೆಯ & 0x07);
	}

	ಸ್ಥಿರಾಂಕ size_t len = SPARTN_ಚೌಕಟ್ಟು_ಆರಂಭ_LEN + desc_len + payload_len + auth_ಬೈಟುಗಳು + ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಬೈಟುಗಳು;

	ಆದರೆ (ಬಫರ್_len < len) {
		ಹಿಂತಿರುಗಿ Probe::Need_ಇನ್ನಷ್ಟು_ದತ್ತಾಂಶ;
	}

	// Message CRC (TF018) covers TF002..TF017
	ಸ್ಥಿರಾಂಕ size_t core_len = len - 1 - ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಬೈಟುಗಳು;
	ಸ್ಥಿರಾಂಕ uint32_t calculated_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = spartn_ಸಂದೇಶ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(&ಬಫರ್[1], core_len, ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_type);
	uint32_t received_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = 0;

	ಗಾಗಿ (size_t i = 0; i < ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಬೈಟುಗಳು; i++) {
		received_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = (received_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ << 8) | ಬಫರ್[len - ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಬೈಟುಗಳು + i];
	}

	ಆದರೆ (calculated_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ != received_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ) {
		ಹಿಂತಿರುಗಿ Probe::ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_Error;
	}

	*ಚೌಕಟ್ಟು_len = len;
	ಹಿಂತಿರುಗಿ Probe::Complete;
}
#ಆದರೆ_ಅಂತ್ಯ // CONFIG_GPS_SPARTN

ಸ್ಥಿರಾಂಕ uint8_t *CorrectionFramer::get_ಮುಂದಿನ_ಸಂದೇಶ(size_t *ಹೊರಹೋಗು_len, CorrectionProtocol *ಹೊರಹೋಗು_protocol)
{
	ಆಗ (ಬಫರ್_len > 0) {
		// Drop everything that cannot start a frame of any supported protocol
		size_t ಗಮ್ಯ_drop = 0;

		ಆಗ ((ಗಮ್ಯ_drop < ಬಫರ್_len) && !isPreamble(ಬಫರ್[ಗಮ್ಯ_drop])) {
			ಗಮ್ಯ_drop++;
		}

		ಆದರೆ (ಗಮ್ಯ_drop > 0) {
			ಬೈಟುಗಳು_discarded += ಗಮ್ಯ_drop;
			discard_ಬೈಟುಗಳು(ಗಮ್ಯ_drop);
		}

		ಆದರೆ (ಬಫರ್_len == 0) {
			ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
		}

		size_t ಚೌಕಟ್ಟು_len = 0;
		CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
		Probe ಫಲಿತಾಂಶ;

		ಆದರೆ (ಬಫರ್[0] == UBX_SYNC1) {
			protocol = CorrectionProtocol::Ubx;
			ಫಲಿತಾಂಶ = probeUbx(&ಚೌಕಟ್ಟು_len);

#ಆದರೆ defined(CONFIG_GPS_SPARTN)

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಫರ್[0] == SPARTN_PREAMBLE) {
			protocol = CorrectionProtocol::Spartn;
			ಫಲಿತಾಂಶ = probeSpartn(&ಚೌಕಟ್ಟು_len);

#ಆದರೆ_ಅಂತ್ಯ

		} ಇಲ್ಲದಿದ್ದರೆ {
			// RTCM3 preamble (isPreamble already filtered other bytes)
			protocol = CorrectionProtocol::Rtcm3;
			ಫಲಿತಾಂಶ = probeRtcm3(&ಚೌಕಟ್ಟು_len);
		}

		ಆಯ್ಕೆ (ಫಲಿತಾಂಶ) {
		ಸ್ಥಿತಿ Probe::Need_ಇನ್ನಷ್ಟು_ದತ್ತಾಂಶ:
			ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

		ಸ್ಥಿತಿ Probe::Invalid_ಶೀರ್ಷಿಕೆ:
			ಬೈಟುಗಳು_discarded++;
			discard_ಬೈಟುಗಳು(1);
			ಮುಂದುವರಿ;

		ಸ್ಥಿತಿ Probe::ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_Error:
			ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_errors++;
			discard_ಬೈಟುಗಳು(1);
			ಮುಂದುವರಿ;

		ಸ್ಥಿತಿ Probe::Complete:
			_pending_protocol = protocol;
			*ಹೊರಹೋಗು_len = ಚೌಕಟ್ಟು_len;

			ಆದರೆ (ಹೊರಹೋಗು_protocol != ಶೂನ್ಯಸೂಚಕ) {
				*ಹೊರಹೋಗು_protocol = protocol;
			}

			ಹಿಂತಿರುಗಿ ಬಫರ್;
		}
	}

	ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಶೂನ್ಯ CorrectionFramer::consume_ಸಂದೇಶ(size_t len)
{
	discard_ಬೈಟುಗಳು(len);
	_messages_parsed++;
	ಒಟ್ಟು_ಚೌಕಟ್ಟು_ಬೈಟುಗಳು += len;

	ಆಯ್ಕೆ (_pending_protocol) {
	ಸ್ಥಿತಿ CorrectionProtocol::Rtcm3:
		_rtcm3_messages++;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CorrectionProtocol::Spartn:
		_spartn_messages++;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CorrectionProtocol::Ubx:
		_ubx_messages++;
		ನಿಲ್ಲಿಸು;
	}
}

ಶೂನ್ಯ CorrectionFramer::discard_ಬೈಟುಗಳು(size_t ಎಣಿಕೆ_2)
{
	ಆದರೆ (ಎಣಿಕೆ_2 >= ಬಫರ್_len) {
		ಬಫರ್_len = 0;

	} ಇಲ್ಲದಿದ್ದರೆ {
		memmove(ಬಫರ್, &ಬಫರ್[ಎಣಿಕೆ_2], ಬಫರ್_len - ಎಣಿಕೆ_2);
		ಬಫರ್_len -= ಎಣಿಕೆ_2;
	}
}

} // namespace gnss
