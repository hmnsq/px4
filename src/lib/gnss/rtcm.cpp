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

#अन्तर्भाव "rtcm.h"
#अन्तर्भाव <cstring>

नामाकाश gnss
{

uint32_t rtcm3_crc24q(स्थिरांक uint8_t *data, माप_प्रकार len)
{
	uint32_t crc = 0;

	साठी (माप_प्रकार i = 0; i < len; i++) {
		crc ^= static_cast<uint32_t>(data[i]) << 16;

		साठी (पूर्णांक j = 0; j < 8; j++) {
			crc <<= 1;

			जर (crc & 0x1000000) {
				crc ^= RTCM3_CRC24Q_POLY;
			}
		}
	}

	परत crc & 0xFFFFFF;
}

स्थिरांक uint8_t *GpsRtcmMessageAssembler::addPacket(uint8_t flags, स्थिरांक uint8_t *data, माप_प्रकार len, uint64_t timestamp,
		माप_प्रकार &out_len)
{
	out_len = 0;

	जर (data == nullptr) {
		reset();
		परत nullptr;
	}

	// Timeout, clear stale partial state.
	जर (_active_sequence.active && (_active_sequence.timestamp != 0)
	    && (timestamp > _active_sequence.timestamp + GPS_RTCM_FRAGMENT_TIMEOUT_US)) {
		resetActiveState();
	}

	जर (!gps_rtcm_is_fragmented(flags)) {
		// Compatibility fallback for older QGroundControl builds
		// that omit the final zero-length fragment
		स्थिरांक माप_प्रकार legacy_message_len = assembleLegacyExactMultipleMessage(_assembled_message);

		// New fragment is invalid, return previous message if valid
		जर (len > आकार(_assembled_message)) {
			reset();
			out_len = legacy_message_len;
			परत (legacy_message_len > 0) ? _assembled_message : nullptr;
		}

		जर (legacy_message_len > 0) {
			// Return the older buffered message first. Queue this new
			// unfragmented packet as deferred output.
			स_नकल(_deferred_message, data, len);
			_deferred_message_len = len;
			_deferred_message_valid = सत्य;
			resetActiveState();
			out_len = legacy_message_len;
			परत _assembled_message;
		}

		reset();
		स_नकल(_assembled_message, data, len);
		out_len = len;

		परत _assembled_message;
	}

	जर (len > GPS_RTCM_MAX_FRAGMENT_LEN) {
		resetActiveState();
		परत nullptr;
	}

	स्थिरांक uint8_t sequence_id = gps_rtcm_sequence_id(flags);
	स्थिरांक uint8_t fragment_id = gps_rtcm_fragment_id(flags);
	स्थिरांक तार्किक is_non_full_fragment = len < GPS_RTCM_MAX_FRAGMENT_LEN;
	स्थिरांक uint8_t *sequence_change_message = nullptr;
	माप_प्रकार sequence_change_message_len = 0;

	जर (!_active_sequence.active || sequence_id != _active_sequence.sequence_id) {
		जर (_active_sequence.active) {
			// Compatibility fallback for older QGroundControl builds
			// that omit the final zero-length fragment
			sequence_change_message_len = assembleLegacyExactMultipleMessage(_assembled_message);

			जर (sequence_change_message_len > 0) {
				// Do not return here: this same packet also starts the new
				// sequence, so it still needs to be validated and buffered.
				sequence_change_message = _assembled_message;
			}
		}

		resetActiveState();
		_active_sequence.active = सत्य;
		_active_sequence.sequence_id = sequence_id;
	}

	स्थिरांक तार्किक last_fragment_known = (_active_sequence.last_fragment_id >= 0);

	// Once a non-full final fragment is known, any later fragment index is invalid.
	जर (last_fragment_known && (fragment_id > _active_sequence.last_fragment_id)) {
		resetActiveState();
		out_len = sequence_change_message_len;
		परत sequence_change_message;
	}

	// A non-full fragment before the known end would contradict the current
	// end-of-message boundary. That can only come from malformed sender state.
	जर (last_fragment_known && (fragment_id < _active_sequence.last_fragment_id) && is_non_full_fragment) {
		resetActiveState();
		out_len = sequence_change_message_len;
		परत sequence_change_message;
	}

	// The first fragment with a non-full payload defines the completion
	// boundary. A higher buffered fragment would contradict that boundary.
	जर (is_non_full_fragment && hasFragmentAfter(fragment_id)) {
		resetActiveState();
		out_len = sequence_change_message_len;
		परत sequence_change_message;
	}

	FragmentSlot &fragment = _fragments[fragment_id];

	जर (fragment.present && ((fragment.len != len) || (स_भेद(fragment.data, data, len) != 0))) {
		// Reusing a slot with different bytes means the current buffer no longer
		// matches the incoming fragments. Drop the old partial state and treat
		// this packet as the start of a new buffer for the same sequence ID.
		resetActiveState();
		_active_sequence.active = सत्य;
		_active_sequence.sequence_id = sequence_id;
	}

	स_नकल(fragment.data, data, len);
	fragment.len = len;
	fragment.present = सत्य;
	_active_sequence.timestamp = timestamp;

	// The buffer is complete when either all 4 fragments are present, or when
	// the first fragment with a non-full payload has been received and every
	// lower fragment ID is already present. Recording fragment 3 as the
	// boundary covers the "all 4 fragments" case. A non-full fragment 0 also
	// completes immediately, although a compliant sender would normally send
	// that packet as unfragmented instead of setting the fragmented bit.
	जर (is_non_full_fragment || (fragment_id == GPS_RTCM_MAX_FRAGMENTS - 1)) {
		_active_sequence.last_fragment_id = fragment_id;
	}

	जर (!isComplete()) {
		out_len = sequence_change_message_len;
		परत sequence_change_message;
	}

	माप_प्रकार assembled_len = 0;
	uint8_t *destination = (sequence_change_message != nullptr) ? _deferred_message : _assembled_message;

	साठी (माप_प्रकार i = 0; i <= lastFragmentIndex(); i++) {
		स्थिरांक FragmentSlot &slot = _fragments[i];
		स_नकल(&destination[assembled_len], slot.data, slot.len);
		assembled_len += slot.len;
	}

	resetActiveState();

	जर (sequence_change_message != nullptr) {
		_deferred_message_len = assembled_len;
		_deferred_message_valid = सत्य;
		out_len = sequence_change_message_len;
		परत sequence_change_message;
	}

	out_len = assembled_len;
	परत _assembled_message;
}

स्थिरांक uint8_t *GpsRtcmMessageAssembler::takeDeferredMessage(माप_प्रकार &out_len)
{
	out_len = 0;

	जर (!_deferred_message_valid) {
		परत nullptr;
	}

	_deferred_message_valid = असत्य;
	out_len = _deferred_message_len;
	परत _deferred_message;
}

शून्य GpsRtcmMessageAssembler::reset()
{
	resetActiveState();
	_deferred_message_len = 0;
	_deferred_message_valid = असत्य;
}

शून्य GpsRtcmMessageAssembler::resetActiveState()
{
	साठी (FragmentSlot &fragment : _fragments) {
		fragment.present = असत्य;
		fragment.len = 0;
	}

	_active_sequence = {};
}

तार्किक GpsRtcmMessageAssembler::hasFragmentAfter(uint8_t fragment_id) स्थिरांक
{
	साठी (माप_प्रकार i = fragment_id + 1; i < GPS_RTCM_MAX_FRAGMENTS; i++) {
		जर (_fragments[i].present) {
			परत सत्य;
		}
	}

	परत असत्य;
}

माप_प्रकार GpsRtcmMessageAssembler::assembleLegacyExactMultipleMessage(uint8_t *destination) स्थिरांक
{
	माप_प्रकार full_fragments = 0;

	// Extension to MAVLink rule: accept legacy senders that omit the required
	// zero-length terminator for exact multiples of 180 bytes, but only when
	// every buffered fragment forms a gap-free full-size prefix starting at
	// fragment 0.
	साठी (; full_fragments < GPS_RTCM_MAX_FRAGMENTS; full_fragments++) {
		जर (!_fragments[full_fragments].present) {
			थांबा;
		}

		जर (_fragments[full_fragments].len != GPS_RTCM_MAX_FRAGMENT_LEN) {
			थांबा;
		}
	}

	// Reject buffers like [180, _, 180]: only flush when every buffered
	// fragment belongs to one contiguous full-size run starting at fragment 0.
	जर ((full_fragments == 0) || hasFragmentAfter(full_fragments - 1)) {
		परत 0;
	}

	माप_प्रकार assembled_len = 0;

	साठी (माप_प्रकार i = 0; i < full_fragments; i++) {
		स्थिरांक FragmentSlot &slot = _fragments[i];
		स_नकल(&destination[assembled_len], slot.data, slot.len);
		assembled_len += slot.len;
	}

	परत assembled_len;
}

तार्किक GpsRtcmMessageAssembler::isComplete() स्थिरांक
{
	साठी (माप_प्रकार i = 0; i <= lastFragmentIndex(); i++) {
		जर (!_fragments[i].present) {
			परत असत्य;
		}
	}

	परत सत्य;
}

माप_प्रकार GpsRtcmMessageAssembler::lastFragmentIndex() स्थिरांक
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
	परत (_active_sequence.last_fragment_id >= 0) ? static_cast<माप_प्रकार>(_active_sequence.last_fragment_id) :
	       (GPS_RTCM_MAX_FRAGMENTS - 1);
}

} // namespace gnss
