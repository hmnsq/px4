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

#அந்தர்பாவ "rtcm.h"
#அந்தர்பாவ <cstring>

நாமாகாஶ gnss
{

uint32_t rtcm3_crc24q(மாறிலி uint8_t *data, size_t len)
{
	uint32_t crc = 0;

	க்கு (size_t i = 0; i < len; i++) {
		crc ^= static_cast<uint32_t>(data[i]) << 16;

		க்கு (முழு j = 0; j < 8; j++) {
			crc <<= 1;

			எனில் (crc & 0x1000000) {
				crc ^= RTCM3_CRC24Q_POLY;
			}
		}
	}

	திரும்பு crc & 0xFFFFFF;
}

மாறிலி uint8_t *GpsRtcmMessageAssembler::addPacket(uint8_t flags, மாறிலி uint8_t *data, size_t len, uint64_t timestamp,
		size_t &out_len)
{
	out_len = 0;

	எனில் (data == nullptr) {
		reset();
		திரும்பு nullptr;
	}

	// Timeout, clear stale partial state.
	எனில் (_active_sequence.active && (_active_sequence.timestamp != 0)
	    && (timestamp > _active_sequence.timestamp + GPS_RTCM_FRAGMENT_TIMEOUT_US)) {
		resetActiveState();
	}

	எனில் (!gps_rtcm_is_fragmented(flags)) {
		// Compatibility fallback for older QGroundControl builds
		// that omit the final zero-length fragment
		மாறிலி size_t legacy_message_len = assembleLegacyExactMultipleMessage(_assembled_message);

		// New fragment is invalid, return previous message if valid
		எனில் (len > அளவு(_assembled_message)) {
			reset();
			out_len = legacy_message_len;
			திரும்பு (legacy_message_len > 0) ? _assembled_message : nullptr;
		}

		எனில் (legacy_message_len > 0) {
			// Return the older buffered message first. Queue this new
			// unfragmented packet as deferred output.
			memcpy(_deferred_message, data, len);
			_deferred_message_len = len;
			_deferred_message_valid = உண்மை;
			resetActiveState();
			out_len = legacy_message_len;
			திரும்பு _assembled_message;
		}

		reset();
		memcpy(_assembled_message, data, len);
		out_len = len;

		திரும்பு _assembled_message;
	}

	எனில் (len > GPS_RTCM_MAX_FRAGMENT_LEN) {
		resetActiveState();
		திரும்பு nullptr;
	}

	மாறிலி uint8_t sequence_id = gps_rtcm_sequence_id(flags);
	மாறிலி uint8_t fragment_id = gps_rtcm_fragment_id(flags);
	மாறிலி தருக்க is_non_full_fragment = len < GPS_RTCM_MAX_FRAGMENT_LEN;
	மாறிலி uint8_t *sequence_change_message = nullptr;
	size_t sequence_change_message_len = 0;

	எனில் (!_active_sequence.active || sequence_id != _active_sequence.sequence_id) {
		எனில் (_active_sequence.active) {
			// Compatibility fallback for older QGroundControl builds
			// that omit the final zero-length fragment
			sequence_change_message_len = assembleLegacyExactMultipleMessage(_assembled_message);

			எனில் (sequence_change_message_len > 0) {
				// Do not return here: this same packet also starts the new
				// sequence, so it still needs to be validated and buffered.
				sequence_change_message = _assembled_message;
			}
		}

		resetActiveState();
		_active_sequence.active = உண்மை;
		_active_sequence.sequence_id = sequence_id;
	}

	மாறிலி தருக்க last_fragment_known = (_active_sequence.last_fragment_id >= 0);

	// Once a non-full final fragment is known, any later fragment index is invalid.
	எனில் (last_fragment_known && (fragment_id > _active_sequence.last_fragment_id)) {
		resetActiveState();
		out_len = sequence_change_message_len;
		திரும்பு sequence_change_message;
	}

	// A non-full fragment before the known end would contradict the current
	// end-of-message boundary. That can only come from malformed sender state.
	எனில் (last_fragment_known && (fragment_id < _active_sequence.last_fragment_id) && is_non_full_fragment) {
		resetActiveState();
		out_len = sequence_change_message_len;
		திரும்பு sequence_change_message;
	}

	// The first fragment with a non-full payload defines the completion
	// boundary. A higher buffered fragment would contradict that boundary.
	எனில் (is_non_full_fragment && hasFragmentAfter(fragment_id)) {
		resetActiveState();
		out_len = sequence_change_message_len;
		திரும்பு sequence_change_message;
	}

	FragmentSlot &fragment = _fragments[fragment_id];

	எனில் (fragment.present && ((fragment.len != len) || (memcmp(fragment.data, data, len) != 0))) {
		// Reusing a slot with different bytes means the current buffer no longer
		// matches the incoming fragments. Drop the old partial state and treat
		// this packet as the start of a new buffer for the same sequence ID.
		resetActiveState();
		_active_sequence.active = உண்மை;
		_active_sequence.sequence_id = sequence_id;
	}

	memcpy(fragment.data, data, len);
	fragment.len = len;
	fragment.present = உண்மை;
	_active_sequence.timestamp = timestamp;

	// The buffer is complete when either all 4 fragments are present, or when
	// the first fragment with a non-full payload has been received and every
	// lower fragment ID is already present. Recording fragment 3 as the
	// boundary covers the "all 4 fragments" case. A non-full fragment 0 also
	// completes immediately, although a compliant sender would normally send
	// that packet as unfragmented instead of setting the fragmented bit.
	எனில் (is_non_full_fragment || (fragment_id == GPS_RTCM_MAX_FRAGMENTS - 1)) {
		_active_sequence.last_fragment_id = fragment_id;
	}

	எனில் (!isComplete()) {
		out_len = sequence_change_message_len;
		திரும்பு sequence_change_message;
	}

	size_t assembled_len = 0;
	uint8_t *destination = (sequence_change_message != nullptr) ? _deferred_message : _assembled_message;

	க்கு (size_t i = 0; i <= lastFragmentIndex(); i++) {
		மாறிலி FragmentSlot &slot = _fragments[i];
		memcpy(&destination[assembled_len], slot.data, slot.len);
		assembled_len += slot.len;
	}

	resetActiveState();

	எனில் (sequence_change_message != nullptr) {
		_deferred_message_len = assembled_len;
		_deferred_message_valid = உண்மை;
		out_len = sequence_change_message_len;
		திரும்பு sequence_change_message;
	}

	out_len = assembled_len;
	திரும்பு _assembled_message;
}

மாறிலி uint8_t *GpsRtcmMessageAssembler::takeDeferredMessage(size_t &out_len)
{
	out_len = 0;

	எனில் (!_deferred_message_valid) {
		திரும்பு nullptr;
	}

	_deferred_message_valid = பொய்;
	out_len = _deferred_message_len;
	திரும்பு _deferred_message;
}

வெறுமை GpsRtcmMessageAssembler::reset()
{
	resetActiveState();
	_deferred_message_len = 0;
	_deferred_message_valid = பொய்;
}

வெறுமை GpsRtcmMessageAssembler::resetActiveState()
{
	க்கு (FragmentSlot &fragment : _fragments) {
		fragment.present = பொய்;
		fragment.len = 0;
	}

	_active_sequence = {};
}

தருக்க GpsRtcmMessageAssembler::hasFragmentAfter(uint8_t fragment_id) மாறிலி
{
	க்கு (size_t i = fragment_id + 1; i < GPS_RTCM_MAX_FRAGMENTS; i++) {
		எனில் (_fragments[i].present) {
			திரும்பு உண்மை;
		}
	}

	திரும்பு பொய்;
}

size_t GpsRtcmMessageAssembler::assembleLegacyExactMultipleMessage(uint8_t *destination) மாறிலி
{
	size_t full_fragments = 0;

	// Extension to MAVLink rule: accept legacy senders that omit the required
	// zero-length terminator for exact multiples of 180 bytes, but only when
	// every buffered fragment forms a gap-free full-size prefix starting at
	// fragment 0.
	க்கு (; full_fragments < GPS_RTCM_MAX_FRAGMENTS; full_fragments++) {
		எனில் (!_fragments[full_fragments].present) {
			நிறுத்து;
		}

		எனில் (_fragments[full_fragments].len != GPS_RTCM_MAX_FRAGMENT_LEN) {
			நிறுத்து;
		}
	}

	// Reject buffers like [180, _, 180]: only flush when every buffered
	// fragment belongs to one contiguous full-size run starting at fragment 0.
	எனில் ((full_fragments == 0) || hasFragmentAfter(full_fragments - 1)) {
		திரும்பு 0;
	}

	size_t assembled_len = 0;

	க்கு (size_t i = 0; i < full_fragments; i++) {
		மாறிலி FragmentSlot &slot = _fragments[i];
		memcpy(&destination[assembled_len], slot.data, slot.len);
		assembled_len += slot.len;
	}

	திரும்பு assembled_len;
}

தருக்க GpsRtcmMessageAssembler::isComplete() மாறிலி
{
	க்கு (size_t i = 0; i <= lastFragmentIndex(); i++) {
		எனில் (!_fragments[i].present) {
			திரும்பு பொய்;
		}
	}

	திரும்பு உண்மை;
}

size_t GpsRtcmMessageAssembler::lastFragmentIndex() மாறிலி
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
	திரும்பு (_active_sequence.last_fragment_id >= 0) ? static_cast<size_t>(_active_sequence.last_fragment_id) :
	       (GPS_RTCM_MAX_FRAGMENTS - 1);
}

} // namespace gnss
