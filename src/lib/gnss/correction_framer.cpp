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

#అన్తర్భావ "correction_framer.h"

#అన్తర్భావ <cstring>

నామాకాశ gnss
{

తార్కిక CorrectionFramer::isPreamble(uint8_t byte)
{
	ఒకవేళ (byte == RTCM3_PREAMBLE) {
		తిరిగి నిజం;
	}

	ఒకవేళ (byte == UBX_SYNC1) {
		తిరిగి నిజం;
	}

#ఒకవేళ defined(CONFIG_GPS_SPARTN)

	ఒకవేళ (byte == SPARTN_PREAMBLE) {
		తిరిగి నిజం;
	}

#అయితే_ముగింపు

	తిరిగి అబద్ధం;
}

size_t CorrectionFramer::కూడు_దత్తాంశం(స్థిరరాశి uint8_t *దత్తాంశం, size_t len)
{
	స్థిరరాశి size_t space_అందుబాటు = బఫరు_కొలత_2 - బఫరు_len;
	స్థిరరాశి size_t గమ్యం_copy = (len < space_అందుబాటు) ? len : space_అందుబాటు;

	ఒకవేళ (గమ్యం_copy > 0) {
		జ్ఞాపకం_నకలు(&బఫరు[బఫరు_len], దత్తాంశం, గమ్యం_copy);
		బఫరు_len += గమ్యం_copy;
	}

	తిరిగి గమ్యం_copy;
}

CorrectionFramer::Probe CorrectionFramer::probeRtcm3(size_t *చట్రం_len) స్థిరరాశి
{
	// Need at least the header to read the length
	ఒకవేళ (బఫరు_len < RTCM3_శీర్షిక_LEN) {
		తిరిగి Probe::Need_మరిన్ని_దత్తాంశం;
	}

	// The 10-bit field cannot exceed RTCM3_MAX_PAYLOAD_LEN, so the length alone
	// never rejects a candidate: CRC-24Q is what validates the frame.
	స్థిరరాశి size_t payload_len = rtcm3_payload_నిడివి(బఫరు);
	స్థిరరాశి size_t len = RTCM3_శీర్షిక_LEN + payload_len + RTCM3_చక్రీయ_తనిఖీ_LEN;

	ఒకవేళ (బఫరు_len < len) {
		తిరిగి Probe::Need_మరిన్ని_దత్తాంశం;
	}

	స్థిరరాశి uint32_t calculated_చక్రీయ_తనిఖీ = rtcm3_crc24q(బఫరు, RTCM3_శీర్షిక_LEN + payload_len);
	స్థిరరాశి uint32_t received_చక్రీయ_తనిఖీ = (static_cast<uint32_t>(బఫరు[len - 3]) << 16) |
				      (static_cast<uint32_t>(బఫరు[len - 2]) << 8) |
				      బఫరు[len - 1];

	ఒకవేళ (calculated_చక్రీయ_తనిఖీ != received_చక్రీయ_తనిఖీ) {
		తిరిగి Probe::చక్రీయ_తనిఖీ_Error;
	}

	*చట్రం_len = len;
	తిరిగి Probe::Complete;
}

CorrectionFramer::Probe CorrectionFramer::probeUbx(size_t *చట్రం_len) స్థిరరాశి
{
	// Need sync1 + sync2 at minimum to reject a false preamble quickly
	ఒకవేళ (బఫరు_len < 2) {
		తిరిగి Probe::Need_మరిన్ని_దత్తాంశం;
	}

	ఒకవేళ (బఫరు[1] != UBX_SYNC2) {
		తిరిగి Probe::Invalid_శీర్షిక;
	}

	ఒకవేళ (బఫరు_len < UBX_శీర్షిక_LEN) {
		తిరిగి Probe::Need_మరిన్ని_దత్తాంశం;
	}

	స్థిరరాశి size_t payload_len = static_cast<size_t>(బఫరు[4]) |
				   (static_cast<size_t>(బఫరు[5]) << 8);

	// Protocol allows larger payloads; reject beyond the injection cap so a
	// garbage length cannot pin the buffer waiting for thousands of bytes.
	ఒకవేళ (payload_len > UBX_MAX_PAYLOAD_LEN) {
		తిరిగి Probe::Invalid_శీర్షిక;
	}

	స్థిరరాశి size_t len = UBX_శీర్షిక_LEN + payload_len + UBX_CK_LEN;

	ఒకవేళ (బఫరు_len < len) {
		తిరిగి Probe::Need_మరిన్ని_దత్తాంశం;
	}

	// CK_A/CK_B cover Class..end of payload (not the sync bytes)
	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	ubx_checksum(&బఫరు[2], UBX_శీర్షిక_LEN - 2 + payload_len, &ck_a, &ck_b);

	ఒకవేళ ((ck_a != బఫరు[len - 2]) || (ck_b != బఫరు[len - 1])) {
		తిరిగి Probe::చక్రీయ_తనిఖీ_Error;
	}

	*చట్రం_len = len;
	తిరిగి Probe::Complete;
}

#ఒకవేళ defined(CONFIG_GPS_SPARTN)
CorrectionFramer::Probe CorrectionFramer::probeSpartn(size_t *చట్రం_len) స్థిరరాశి
{
	// Need TF002..TF006 to read the length and CRC type
	ఒకవేళ (బఫరు_len < SPARTN_చట్రం_ప్రారంభం_LEN) {
		తిరిగి Probe::Need_మరిన్ని_దత్తాంశం;
	}

	స్థిరరాశి uint8_t b1 = బఫరు[1];
	స్థిరరాశి uint8_t b2 = బఫరు[2];
	స్థిరరాశి uint8_t b3 = బఫరు[3];

	// TF002 is the only header field with a checkable range. The 10-bit length
	// spans its full range and TF006 is a 4-bit CRC over a non-byte-aligned
	// field, so this is the only cheap guard before the message CRC.
	ఒకవేళ (!spartn_సందేశం_type_defined(b1 >> 1)) {
		తిరిగి Probe::Invalid_శీర్షిక;
	}

	స్థిరరాశి uint16_t payload_len = static_cast<uint16_t>(((b1 & 0x01) << 9) | (b2 << 1) | (b3 >> 7));
	స్థిరరాశి తార్కిక eaf = ((b3 >> 6) & 0x01) != 0;
	స్థిరరాశి uint8_t చక్రీయ_తనిఖీ_type = (b3 >> 4) & 0x03;
	స్థిరరాశి size_t చక్రీయ_తనిఖీ_బైట్లు = static_cast<size_t>(చక్రీయ_తనిఖీ_type) + 1;

	// TF008 (time tag type) sets the description block length
	ఒకవేళ (బఫరు_len < SPARTN_చట్రం_ప్రారంభం_LEN + 1) {
		తిరిగి Probe::Need_మరిన్ని_దత్తాంశం;
	}

	స్థిరరాశి తార్కిక సమయం_ట్యాగ్_32bit = ((బఫరు[4] >> 3) & 0x01) != 0;
	స్థిరరాశి size_t desc_len = spartn_desc_నిడివి_బైట్లు(eaf, సమయం_ట్యాగ్_32bit);

	ఒకవేళ (బఫరు_len < SPARTN_చట్రం_ప్రారంభం_LEN + desc_len) {
		తిరిగి Probe::Need_మరిన్ని_దత్తాంశం;
	}

	size_t auth_బైట్లు = 0;

	ఒకవేళ (eaf) {
		// TF014/TF015 occupy the low 6 bits of the last description byte
		స్థిరరాశి uint8_t చివరి = బఫరు[SPARTN_చట్రం_ప్రారంభం_LEN + desc_len - 1];
		auth_బైట్లు = spartn_auth_నిడివి_బైట్లు(నిజం, (చివరి >> 3) & 0x07, చివరి & 0x07);
	}

	స్థిరరాశి size_t len = SPARTN_చట్రం_ప్రారంభం_LEN + desc_len + payload_len + auth_బైట్లు + చక్రీయ_తనిఖీ_బైట్లు;

	ఒకవేళ (బఫరు_len < len) {
		తిరిగి Probe::Need_మరిన్ని_దత్తాంశం;
	}

	// Message CRC (TF018) covers TF002..TF017
	స్థిరరాశి size_t core_len = len - 1 - చక్రీయ_తనిఖీ_బైట్లు;
	స్థిరరాశి uint32_t calculated_చక్రీయ_తనిఖీ = spartn_సందేశం_చక్రీయ_తనిఖీ(&బఫరు[1], core_len, చక్రీయ_తనిఖీ_type);
	uint32_t received_చక్రీయ_తనిఖీ = 0;

	కొరకు (size_t i = 0; i < చక్రీయ_తనిఖీ_బైట్లు; i++) {
		received_చక్రీయ_తనిఖీ = (received_చక్రీయ_తనిఖీ << 8) | బఫరు[len - చక్రీయ_తనిఖీ_బైట్లు + i];
	}

	ఒకవేళ (calculated_చక్రీయ_తనిఖీ != received_చక్రీయ_తనిఖీ) {
		తిరిగి Probe::చక్రీయ_తనిఖీ_Error;
	}

	*చట్రం_len = len;
	తిరిగి Probe::Complete;
}
#అయితే_ముగింపు // CONFIG_GPS_SPARTN

స్థిరరాశి uint8_t *CorrectionFramer::get_తదుపరి_సందేశం(size_t *బహిర్గతం_len, CorrectionProtocol *బహిర్గతం_protocol)
{
	అయితే (బఫరు_len > 0) {
		// Drop everything that cannot start a frame of any supported protocol
		size_t గమ్యం_drop = 0;

		అయితే ((గమ్యం_drop < బఫరు_len) && !isPreamble(బఫరు[గమ్యం_drop])) {
			గమ్యం_drop++;
		}

		ఒకవేళ (గమ్యం_drop > 0) {
			బైట్లు_discarded += గమ్యం_drop;
			discard_బైట్లు(గమ్యం_drop);
		}

		ఒకవేళ (బఫరు_len == 0) {
			తిరిగి శూన్యసూచిక;
		}

		size_t చట్రం_len = 0;
		CorrectionProtocol protocol = CorrectionProtocol::Rtcm3;
		Probe ఫలితం;

		ఒకవేళ (బఫరు[0] == UBX_SYNC1) {
			protocol = CorrectionProtocol::Ubx;
			ఫలితం = probeUbx(&చట్రం_len);

#ఒకవేళ defined(CONFIG_GPS_SPARTN)

		} లేకపోతే ఒకవేళ (బఫరు[0] == SPARTN_PREAMBLE) {
			protocol = CorrectionProtocol::Spartn;
			ఫలితం = probeSpartn(&చట్రం_len);

#అయితే_ముగింపు

		} లేకపోతే {
			// RTCM3 preamble (isPreamble already filtered other bytes)
			protocol = CorrectionProtocol::Rtcm3;
			ఫలితం = probeRtcm3(&చట్రం_len);
		}

		ఎంపిక (ఫలితం) {
		స్థితి Probe::Need_మరిన్ని_దత్తాంశం:
			తిరిగి శూన్యసూచిక;

		స్థితి Probe::Invalid_శీర్షిక:
			బైట్లు_discarded++;
			discard_బైట్లు(1);
			కొనసాగు;

		స్థితి Probe::చక్రీయ_తనిఖీ_Error:
			చక్రీయ_తనిఖీ_errors++;
			discard_బైట్లు(1);
			కొనసాగు;

		స్థితి Probe::Complete:
			_pending_protocol = protocol;
			*బహిర్గతం_len = చట్రం_len;

			ఒకవేళ (బహిర్గతం_protocol != శూన్యసూచిక) {
				*బహిర్గతం_protocol = protocol;
			}

			తిరిగి బఫరు;
		}
	}

	తిరిగి శూన్యసూచిక;
}

శూన్య CorrectionFramer::consume_సందేశం(size_t len)
{
	discard_బైట్లు(len);
	_messages_parsed++;
	మొత్తం_చట్రం_బైట్లు += len;

	ఎంపిక (_pending_protocol) {
	స్థితి CorrectionProtocol::Rtcm3:
		_rtcm3_messages++;
		ఆపు;

	స్థితి CorrectionProtocol::Spartn:
		_spartn_messages++;
		ఆపు;

	స్థితి CorrectionProtocol::Ubx:
		_ubx_messages++;
		ఆపు;
	}
}

శూన్య CorrectionFramer::discard_బైట్లు(size_t లెక్క)
{
	ఒకవేళ (లెక్క >= బఫరు_len) {
		బఫరు_len = 0;

	} లేకపోతే {
		memmove(బఫరు, &బఫరు[లెక్క], బఫరు_len - లెక్క);
		బఫరు_len -= లెక్క;
	}
}

} // namespace gnss
