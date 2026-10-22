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

#ଅନ୍ତର୍ଭାଵ "rtcm.h"
#ଅନ୍ତର୍ଭାଵ <cstring>

ନାମାକାଶ gnss
{

uint32_t rtcm3_crc24q(ଅଚଳ uint8_t *data, size_t len)
{
	uint32_t crc = 0;

	ପାଇଁ (size_t i = 0; i < len; i++) {
		crc ^= static_cast<uint32_t>(data[i]) << 16;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < 8; j++) {
			crc <<= 1;

			ଯଦି (crc & 0x1000000) {
				crc ^= RTCM3_CRC24Q_POLY;
			}
		}
	}

	ଫେରନ୍ତୁ crc & 0xFFFFFF;
}

ଅଚଳ uint8_t *GpsRtcmMessageAssembler::addPacket(uint8_t flags, ଅଚଳ uint8_t *data, size_t len, uint64_t timestamp,
		size_t &out_len)
{
	out_len = 0;

	ଯଦି (data == nullptr) {
		reset();
		ଫେରନ୍ତୁ nullptr;
	}

	// Timeout, clear stale partial state.
	ଯଦି (_active_sequence.active && (_active_sequence.timestamp != 0)
	    && (timestamp > _active_sequence.timestamp + GPS_RTCM_FRAGMENT_TIMEOUT_US)) {
		resetActiveState();
	}

	ଯଦି (!gps_rtcm_is_fragmented(flags)) {
		// Compatibility fallback for older QGroundControl builds
		// that omit the final zero-length fragment
		ଅଚଳ size_t legacy_message_len = assembleLegacyExactMultipleMessage(_assembled_message);

		// New fragment is invalid, return previous message if valid
		ଯଦି (len > ଆକାର(_assembled_message)) {
			reset();
			out_len = legacy_message_len;
			ଫେରନ୍ତୁ (legacy_message_len > 0) ? _assembled_message : nullptr;
		}

		ଯଦି (legacy_message_len > 0) {
			// Return the older buffered message first. Queue this new
			// unfragmented packet as deferred output.
			memcpy(_deferred_message, data, len);
			_deferred_message_len = len;
			_deferred_message_valid = ସତ୍ୟ;
			resetActiveState();
			out_len = legacy_message_len;
			ଫେରନ୍ତୁ _assembled_message;
		}

		reset();
		memcpy(_assembled_message, data, len);
		out_len = len;

		ଫେରନ୍ତୁ _assembled_message;
	}

	ଯଦି (len > GPS_RTCM_MAX_FRAGMENT_LEN) {
		resetActiveState();
		ଫେରନ୍ତୁ nullptr;
	}

	ଅଚଳ uint8_t sequence_id = gps_rtcm_sequence_id(flags);
	ଅଚଳ uint8_t fragment_id = gps_rtcm_fragment_id(flags);
	ଅଚଳ ତାର୍କିକ is_non_full_fragment = len < GPS_RTCM_MAX_FRAGMENT_LEN;
	ଅଚଳ uint8_t *sequence_change_message = nullptr;
	size_t sequence_change_message_len = 0;

	ଯଦି (!_active_sequence.active || sequence_id != _active_sequence.sequence_id) {
		ଯଦି (_active_sequence.active) {
			// Compatibility fallback for older QGroundControl builds
			// that omit the final zero-length fragment
			sequence_change_message_len = assembleLegacyExactMultipleMessage(_assembled_message);

			ଯଦି (sequence_change_message_len > 0) {
				// Do not return here: this same packet also starts the new
				// sequence, so it still needs to be validated and buffered.
				sequence_change_message = _assembled_message;
			}
		}

		resetActiveState();
		_active_sequence.active = ସତ୍ୟ;
		_active_sequence.sequence_id = sequence_id;
	}

	ଅଚଳ ତାର୍କିକ last_fragment_known = (_active_sequence.last_fragment_id >= 0);

	// Once a non-full final fragment is known, any later fragment index is invalid.
	ଯଦି (last_fragment_known && (fragment_id > _active_sequence.last_fragment_id)) {
		resetActiveState();
		out_len = sequence_change_message_len;
		ଫେରନ୍ତୁ sequence_change_message;
	}

	// A non-full fragment before the known end would contradict the current
	// end-of-message boundary. That can only come from malformed sender state.
	ଯଦି (last_fragment_known && (fragment_id < _active_sequence.last_fragment_id) && is_non_full_fragment) {
		resetActiveState();
		out_len = sequence_change_message_len;
		ଫେରନ୍ତୁ sequence_change_message;
	}

	// The first fragment with a non-full payload defines the completion
	// boundary. A higher buffered fragment would contradict that boundary.
	ଯଦି (is_non_full_fragment && hasFragmentAfter(fragment_id)) {
		resetActiveState();
		out_len = sequence_change_message_len;
		ଫେରନ୍ତୁ sequence_change_message;
	}

	FragmentSlot &fragment = _fragments[fragment_id];

	ଯଦି (fragment.present && ((fragment.len != len) || (memcmp(fragment.data, data, len) != 0))) {
		// Reusing a slot with different bytes means the current buffer no longer
		// matches the incoming fragments. Drop the old partial state and treat
		// this packet as the start of a new buffer for the same sequence ID.
		resetActiveState();
		_active_sequence.active = ସତ୍ୟ;
		_active_sequence.sequence_id = sequence_id;
	}

	memcpy(fragment.data, data, len);
	fragment.len = len;
	fragment.present = ସତ୍ୟ;
	_active_sequence.timestamp = timestamp;

	// The buffer is complete when either all 4 fragments are present, or when
	// the first fragment with a non-full payload has been received and every
	// lower fragment ID is already present. Recording fragment 3 as the
	// boundary covers the "all 4 fragments" case. A non-full fragment 0 also
	// completes immediately, although a compliant sender would normally send
	// that packet as unfragmented instead of setting the fragmented bit.
	ଯଦି (is_non_full_fragment || (fragment_id == GPS_RTCM_MAX_FRAGMENTS - 1)) {
		_active_sequence.last_fragment_id = fragment_id;
	}

	ଯଦି (!isComplete()) {
		out_len = sequence_change_message_len;
		ଫେରନ୍ତୁ sequence_change_message;
	}

	size_t assembled_len = 0;
	uint8_t *destination = (sequence_change_message != nullptr) ? _deferred_message : _assembled_message;

	ପାଇଁ (size_t i = 0; i <= lastFragmentIndex(); i++) {
		ଅଚଳ FragmentSlot &slot = _fragments[i];
		memcpy(&destination[assembled_len], slot.data, slot.len);
		assembled_len += slot.len;
	}

	resetActiveState();

	ଯଦି (sequence_change_message != nullptr) {
		_deferred_message_len = assembled_len;
		_deferred_message_valid = ସତ୍ୟ;
		out_len = sequence_change_message_len;
		ଫେରନ୍ତୁ sequence_change_message;
	}

	out_len = assembled_len;
	ଫେରନ୍ତୁ _assembled_message;
}

ଅଚଳ uint8_t *GpsRtcmMessageAssembler::takeDeferredMessage(size_t &out_len)
{
	out_len = 0;

	ଯଦି (!_deferred_message_valid) {
		ଫେରନ୍ତୁ nullptr;
	}

	_deferred_message_valid = ମିଥ୍ୟା;
	out_len = _deferred_message_len;
	ଫେରନ୍ତୁ _deferred_message;
}

ଶୂନ୍ୟ GpsRtcmMessageAssembler::reset()
{
	resetActiveState();
	_deferred_message_len = 0;
	_deferred_message_valid = ମିଥ୍ୟା;
}

ଶୂନ୍ୟ GpsRtcmMessageAssembler::resetActiveState()
{
	ପାଇଁ (FragmentSlot &fragment : _fragments) {
		fragment.present = ମିଥ୍ୟା;
		fragment.len = 0;
	}

	_active_sequence = {};
}

ତାର୍କିକ GpsRtcmMessageAssembler::hasFragmentAfter(uint8_t fragment_id) ଅଚଳ
{
	ପାଇଁ (size_t i = fragment_id + 1; i < GPS_RTCM_MAX_FRAGMENTS; i++) {
		ଯଦି (_fragments[i].present) {
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

size_t GpsRtcmMessageAssembler::assembleLegacyExactMultipleMessage(uint8_t *destination) ଅଚଳ
{
	size_t full_fragments = 0;

	// Extension to MAVLink rule: accept legacy senders that omit the required
	// zero-length terminator for exact multiples of 180 bytes, but only when
	// every buffered fragment forms a gap-free full-size prefix starting at
	// fragment 0.
	ପାଇଁ (; full_fragments < GPS_RTCM_MAX_FRAGMENTS; full_fragments++) {
		ଯଦି (!_fragments[full_fragments].present) {
			ଭାଙ୍ଗ;
		}

		ଯଦି (_fragments[full_fragments].len != GPS_RTCM_MAX_FRAGMENT_LEN) {
			ଭାଙ୍ଗ;
		}
	}

	// Reject buffers like [180, _, 180]: only flush when every buffered
	// fragment belongs to one contiguous full-size run starting at fragment 0.
	ଯଦି ((full_fragments == 0) || hasFragmentAfter(full_fragments - 1)) {
		ଫେରନ୍ତୁ 0;
	}

	size_t assembled_len = 0;

	ପାଇଁ (size_t i = 0; i < full_fragments; i++) {
		ଅଚଳ FragmentSlot &slot = _fragments[i];
		memcpy(&destination[assembled_len], slot.data, slot.len);
		assembled_len += slot.len;
	}

	ଫେରନ୍ତୁ assembled_len;
}

ତାର୍କିକ GpsRtcmMessageAssembler::isComplete() ଅଚଳ
{
	ପାଇଁ (size_t i = 0; i <= lastFragmentIndex(); i++) {
		ଯଦି (!_fragments[i].present) {
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

size_t GpsRtcmMessageAssembler::lastFragmentIndex() ଅଚଳ
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
	ଫେରନ୍ତୁ (_active_sequence.last_fragment_id >= 0) ? static_cast<size_t>(_active_sequence.last_fragment_id) :
	       (GPS_RTCM_MAX_FRAGMENTS - 1);
}

} // namespace gnss
