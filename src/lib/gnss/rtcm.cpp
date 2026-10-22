/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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
 * @file rtcm.cpp
 *
 * RTCM3 protocol parsing implementation.
 */

#అన్తర్భావ "rtcm.h"
#అన్తర్భావ <cstring>

నామాకాశ gnss
{

uint32_t rtcm3_crc24q(స్థిరరాశి uint8_t *దత్తాంశం, size_t len)
{
	uint32_t చక్రీయ_తనిఖీ = 0;

	కొరకు (size_t i = 0; i < len; i++) {
		చక్రీయ_తనిఖీ ^= static_cast<uint32_t>(దత్తాంశం[i]) << 16;

		కొరకు (పూర్ణాంకం j = 0; j < 8; j++) {
			చక్రీయ_తనిఖీ <<= 1;

			ఒకవేళ (చక్రీయ_తనిఖీ & 0x1000000) {
				చక్రీయ_తనిఖీ ^= RTCM3_CRC24Q_POLY;
			}
		}
	}

	తిరిగి చక్రీయ_తనిఖీ & 0xFFFFFF;
}

స్థిరరాశి uint8_t *Gps_Rtcm_సందేశం_Assembler::కూడు_Packet(uint8_t flags, స్థిరరాశి uint8_t *దత్తాంశం, size_t len, uint64_t timestamp,
		size_t &బహిర్గతం_len)
{
	బహిర్గతం_len = 0;

	ఒకవేళ (దత్తాంశం == శూన్యసూచిక) {
		reset();
		తిరిగి శూన్యసూచిక;
	}

	// Timeout, clear stale partial state.
	ఒకవేళ (చురుకైన_sequence.చురుకైన_2 && (చురుకైన_sequence.timestamp != 0)
	    && (timestamp > చురుకైన_sequence.timestamp + GPS_RTCM_FRAGMENT_TIMEOUT_US)) {
		reset_చురుకైన_State();
	}

	ఒకవేళ (!gps_rtcm_is_fragmented(flags)) {
		// Compatibility fallback for older QGroundControl builds
		// that omit the final zero-length fragment
		స్థిరరాశి size_t legacy_సందేశం_len = assemble_Legacy_Exact_గుణిజం_సందేశం(assembled_సందేశం);

		// New fragment is invalid, return previous message if valid
		ఒకవేళ (len > పరిమాణం(assembled_సందేశం)) {
			reset();
			బహిర్గతం_len = legacy_సందేశం_len;
			తిరిగి (legacy_సందేశం_len > 0) ? assembled_సందేశం : శూన్యసూచిక;
		}

		ఒకవేళ (legacy_సందేశం_len > 0) {
			// Return the older buffered message first. Queue this new
			// unfragmented packet as deferred output.
			జ్ఞాపకం_నకలు(deferred_సందేశం, దత్తాంశం, len);
			deferred_సందేశం_len = len;
			deferred_సందేశం_valid = నిజం;
			reset_చురుకైన_State();
			బహిర్గతం_len = legacy_సందేశం_len;
			తిరిగి assembled_సందేశం;
		}

		reset();
		జ్ఞాపకం_నకలు(assembled_సందేశం, దత్తాంశం, len);
		బహిర్గతం_len = len;

		తిరిగి assembled_సందేశం;
	}

	ఒకవేళ (len > GPS_RTCM_MAX_FRAGMENT_LEN) {
		reset_చురుకైన_State();
		తిరిగి శూన్యసూచిక;
	}

	స్థిరరాశి uint8_t sequence_id = gps_rtcm_sequence_id(flags);
	స్థిరరాశి uint8_t fragment_id = gps_rtcm_fragment_id(flags);
	స్థిరరాశి తార్కిక is_non_నిండిన_fragment = len < GPS_RTCM_MAX_FRAGMENT_LEN;
	స్థిరరాశి uint8_t *sequence_change_సందేశం = శూన్యసూచిక;
	size_t sequence_change_సందేశం_len = 0;

	ఒకవేళ (!చురుకైన_sequence.చురుకైన_2 || sequence_id != చురుకైన_sequence.sequence_id) {
		ఒకవేళ (చురుకైన_sequence.చురుకైన_2) {
			// Compatibility fallback for older QGroundControl builds
			// that omit the final zero-length fragment
			sequence_change_సందేశం_len = assemble_Legacy_Exact_గుణిజం_సందేశం(assembled_సందేశం);

			ఒకవేళ (sequence_change_సందేశం_len > 0) {
				// Do not return here: this same packet also starts the new
				// sequence, so it still needs to be validated and buffered.
				sequence_change_సందేశం = assembled_సందేశం;
			}
		}

		reset_చురుకైన_State();
		చురుకైన_sequence.చురుకైన_2 = నిజం;
		చురుకైన_sequence.sequence_id = sequence_id;
	}

	స్థిరరాశి తార్కిక చివరి_fragment_తెలిసిన = (చురుకైన_sequence.చివరి_fragment_id >= 0);

	// Once a non-full final fragment is known, any later fragment index is invalid.
	ఒకవేళ (చివరి_fragment_తెలిసిన && (fragment_id > చురుకైన_sequence.చివరి_fragment_id)) {
		reset_చురుకైన_State();
		బహిర్గతం_len = sequence_change_సందేశం_len;
		తిరిగి sequence_change_సందేశం;
	}

	// A non-full fragment before the known end would contradict the current
	// end-of-message boundary. That can only come from malformed sender state.
	ఒకవేళ (చివరి_fragment_తెలిసిన && (fragment_id < చురుకైన_sequence.చివరి_fragment_id) && is_non_నిండిన_fragment) {
		reset_చురుకైన_State();
		బహిర్గతం_len = sequence_change_సందేశం_len;
		తిరిగి sequence_change_సందేశం;
	}

	// The first fragment with a non-full payload defines the completion
	// boundary. A higher buffered fragment would contradict that boundary.
	ఒకవేళ (is_non_నిండిన_fragment && hasFragmentAfter(fragment_id)) {
		reset_చురుకైన_State();
		బహిర్గతం_len = sequence_change_సందేశం_len;
		తిరిగి sequence_change_సందేశం;
	}

	Fragment_స్థానం &fragment = _fragments[fragment_id];

	ఒకవేళ (fragment.present && ((fragment.len != len) || (memcmp(fragment.దత్తాంశం, దత్తాంశం, len) != 0))) {
		// Reusing a slot with different bytes means the current buffer no longer
		// matches the incoming fragments. Drop the old partial state and treat
		// this packet as the start of a new buffer for the same sequence ID.
		reset_చురుకైన_State();
		చురుకైన_sequence.చురుకైన_2 = నిజం;
		చురుకైన_sequence.sequence_id = sequence_id;
	}

	జ్ఞాపకం_నకలు(fragment.దత్తాంశం, దత్తాంశం, len);
	fragment.len = len;
	fragment.present = నిజం;
	చురుకైన_sequence.timestamp = timestamp;

	// The buffer is complete when either all 4 fragments are present, or when
	// the first fragment with a non-full payload has been received and every
	// lower fragment ID is already present. Recording fragment 3 as the
	// boundary covers the "all 4 fragments" case. A non-full fragment 0 also
	// completes immediately, although a compliant sender would normally send
	// that packet as unfragmented instead of setting the fragmented bit.
	ఒకవేళ (is_non_నిండిన_fragment || (fragment_id == GPS_RTCM_MAX_FRAGMENTS - 1)) {
		చురుకైన_sequence.చివరి_fragment_id = fragment_id;
	}

	ఒకవేళ (!isComplete()) {
		బహిర్గతం_len = sequence_change_సందేశం_len;
		తిరిగి sequence_change_సందేశం;
	}

	size_t assembled_len = 0;
	uint8_t *destination = (sequence_change_సందేశం != శూన్యసూచిక) ? deferred_సందేశం : assembled_సందేశం;

	కొరకు (size_t i = 0; i <= చివరి_Fragment_సూచీ(); i++) {
		స్థిరరాశి Fragment_స్థానం &స్థానం = _fragments[i];
		జ్ఞాపకం_నకలు(&destination[assembled_len], స్థానం.దత్తాంశం, స్థానం.len);
		assembled_len += స్థానం.len;
	}

	reset_చురుకైన_State();

	ఒకవేళ (sequence_change_సందేశం != శూన్యసూచిక) {
		deferred_సందేశం_len = assembled_len;
		deferred_సందేశం_valid = నిజం;
		బహిర్గతం_len = sequence_change_సందేశం_len;
		తిరిగి sequence_change_సందేశం;
	}

	బహిర్గతం_len = assembled_len;
	తిరిగి assembled_సందేశం;
}

స్థిరరాశి uint8_t *Gps_Rtcm_సందేశం_Assembler::take_Deferred_సందేశం(size_t &బహిర్గతం_len)
{
	బహిర్గతం_len = 0;

	ఒకవేళ (!deferred_సందేశం_valid) {
		తిరిగి శూన్యసూచిక;
	}

	deferred_సందేశం_valid = అబద్ధం;
	బహిర్గతం_len = deferred_సందేశం_len;
	తిరిగి deferred_సందేశం;
}

శూన్య Gps_Rtcm_సందేశం_Assembler::reset()
{
	reset_చురుకైన_State();
	deferred_సందేశం_len = 0;
	deferred_సందేశం_valid = అబద్ధం;
}

శూన్య Gps_Rtcm_సందేశం_Assembler::reset_చురుకైన_State()
{
	కొరకు (Fragment_స్థానం &fragment : _fragments) {
		fragment.present = అబద్ధం;
		fragment.len = 0;
	}

	చురుకైన_sequence = {};
}

తార్కిక Gps_Rtcm_సందేశం_Assembler::hasFragmentAfter(uint8_t fragment_id) స్థిరరాశి
{
	కొరకు (size_t i = fragment_id + 1; i < GPS_RTCM_MAX_FRAGMENTS; i++) {
		ఒకవేళ (_fragments[i].present) {
			తిరిగి నిజం;
		}
	}

	తిరిగి అబద్ధం;
}

size_t Gps_Rtcm_సందేశం_Assembler::assemble_Legacy_Exact_గుణిజం_సందేశం(uint8_t *destination) స్థిరరాశి
{
	size_t నిండిన_fragments = 0;

	// Extension to MAVLink rule: accept legacy senders that omit the required
	// zero-length terminator for exact multiples of 180 bytes, but only when
	// every buffered fragment forms a gap-free full-size prefix starting at
	// fragment 0.
	కొరకు (; నిండిన_fragments < GPS_RTCM_MAX_FRAGMENTS; నిండిన_fragments++) {
		ఒకవేళ (!_fragments[నిండిన_fragments].present) {
			ఆపు;
		}

		ఒకవేళ (_fragments[నిండిన_fragments].len != GPS_RTCM_MAX_FRAGMENT_LEN) {
			ఆపు;
		}
	}

	// Reject buffers like [180, _, 180]: only flush when every buffered
	// fragment belongs to one contiguous full-size run starting at fragment 0.
	ఒకవేళ ((నిండిన_fragments == 0) || hasFragmentAfter(నిండిన_fragments - 1)) {
		తిరిగి 0;
	}

	size_t assembled_len = 0;

	కొరకు (size_t i = 0; i < నిండిన_fragments; i++) {
		స్థిరరాశి Fragment_స్థానం &స్థానం = _fragments[i];
		జ్ఞాపకం_నకలు(&destination[assembled_len], స్థానం.దత్తాంశం, స్థానం.len);
		assembled_len += స్థానం.len;
	}

	తిరిగి assembled_len;
}

తార్కిక Gps_Rtcm_సందేశం_Assembler::isComplete() స్థిరరాశి
{
	కొరకు (size_t i = 0; i <= చివరి_Fragment_సూచీ(); i++) {
		ఒకవేళ (!_fragments[i].present) {
			తిరిగి అబద్ధం;
		}
	}

	తిరిగి నిజం;
}

size_t Gps_Rtcm_సందేశం_Assembler::చివరి_Fragment_సూచీ() స్థిరరాశి
{
	// Per the MAVLink GPS_RTCM_DATA rule, a buffer is complete once either all
	// 4 fragments are present, or the first fragment with a non-full payload
	// has been received and every lower fragment ID is present. Until such a
	// fragment arrives, the receiver must assume that all 4 fragment slots may
	// still be used. If the RTCM payload length is an exact multiple of 180
	// bytes and uses fewer than 4 fragments, the sender must still send a
	// final zero-length fragment to mark completion. Sequence rollover above
	// intentionally accepts older senders that omit that terminator, but only
	// for gap-free full-size fragment runs starting at fragment 0 while the old
	// sequence is still buffered.
	తిరిగి (చురుకైన_sequence.చివరి_fragment_id >= 0) ? static_cast<size_t>(చురుకైన_sequence.చివరి_fragment_id) :
	       (GPS_RTCM_MAX_FRAGMENTS - 1);
}

} // namespace gnss
