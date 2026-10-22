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

#شامل "rtcm.h"
#شامل <cstring>

نامخلا gnss
{

uint32_t rtcm3_crc24q(ثابت uint8_t *ڈیٹا, size_t len)
{
	uint32_t چکری_جانچ = 0;

	برائے (size_t i = 0; i < len; i++) {
		چکری_جانچ ^= static_cast<uint32_t>(ڈیٹا[i]) << 16;

		برائے (صحیح j = 0; j < 8; j++) {
			چکری_جانچ <<= 1;

			اگر (چکری_جانچ & 0x1000000) {
				چکری_جانچ ^= RTCM3_CRC24Q_POLY;
			}
		}
	}

	واپس چکری_جانچ & 0xFFFFFF;
}

ثابت uint8_t *Gps_Rtcm_پیغام_Assembler::جوڑو_Packet(uint8_t flags, ثابت uint8_t *ڈیٹا, size_t len, uint64_t timestamp,
		size_t &برآمد_len)
{
	برآمد_len = 0;

	اگر (ڈیٹا == خالی_اشاریہ) {
		reset();
		واپس خالی_اشاریہ;
	}

	// Timeout, clear stale partial state.
	اگر (فعال_sequence.فعال_2 && (فعال_sequence.timestamp != 0)
	    && (timestamp > فعال_sequence.timestamp + GPS_RTCM_FRAGMENT_TIMEOUT_US)) {
		reset_فعال_State();
	}

	اگر (!gps_rtcm_is_fragmented(flags)) {
		// Compatibility fallback for older QGroundControl builds
		// that omit the final zero-length fragment
		ثابت size_t legacy_پیغام_len = assemble_Legacy_Exact_مضرب_پیغام(assembled_پیغام);

		// New fragment is invalid, return previous message if valid
		اگر (len > حجم(assembled_پیغام)) {
			reset();
			برآمد_len = legacy_پیغام_len;
			واپس (legacy_پیغام_len > 0) ? assembled_پیغام : خالی_اشاریہ;
		}

		اگر (legacy_پیغام_len > 0) {
			// Return the older buffered message first. Queue this new
			// unfragmented packet as deferred output.
			حافظہ_نقل(deferred_پیغام, ڈیٹا, len);
			deferred_پیغام_len = len;
			deferred_پیغام_valid = سچ;
			reset_فعال_State();
			برآمد_len = legacy_پیغام_len;
			واپس assembled_پیغام;
		}

		reset();
		حافظہ_نقل(assembled_پیغام, ڈیٹا, len);
		برآمد_len = len;

		واپس assembled_پیغام;
	}

	اگر (len > GPS_RTCM_MAX_FRAGMENT_LEN) {
		reset_فعال_State();
		واپس خالی_اشاریہ;
	}

	ثابت uint8_t sequence_id = gps_rtcm_sequence_id(flags);
	ثابت uint8_t fragment_id = gps_rtcm_fragment_id(flags);
	ثابت منطقی is_non_بھرا_fragment = len < GPS_RTCM_MAX_FRAGMENT_LEN;
	ثابت uint8_t *sequence_change_پیغام = خالی_اشاریہ;
	size_t sequence_change_پیغام_len = 0;

	اگر (!فعال_sequence.فعال_2 || sequence_id != فعال_sequence.sequence_id) {
		اگر (فعال_sequence.فعال_2) {
			// Compatibility fallback for older QGroundControl builds
			// that omit the final zero-length fragment
			sequence_change_پیغام_len = assemble_Legacy_Exact_مضرب_پیغام(assembled_پیغام);

			اگر (sequence_change_پیغام_len > 0) {
				// Do not return here: this same packet also starts the new
				// sequence, so it still needs to be validated and buffered.
				sequence_change_پیغام = assembled_پیغام;
			}
		}

		reset_فعال_State();
		فعال_sequence.فعال_2 = سچ;
		فعال_sequence.sequence_id = sequence_id;
	}

	ثابت منطقی آخری_fragment_معلوم = (فعال_sequence.آخری_fragment_id >= 0);

	// Once a non-full final fragment is known, any later fragment index is invalid.
	اگر (آخری_fragment_معلوم && (fragment_id > فعال_sequence.آخری_fragment_id)) {
		reset_فعال_State();
		برآمد_len = sequence_change_پیغام_len;
		واپس sequence_change_پیغام;
	}

	// A non-full fragment before the known end would contradict the current
	// end-of-message boundary. That can only come from malformed sender state.
	اگر (آخری_fragment_معلوم && (fragment_id < فعال_sequence.آخری_fragment_id) && is_non_بھرا_fragment) {
		reset_فعال_State();
		برآمد_len = sequence_change_پیغام_len;
		واپس sequence_change_پیغام;
	}

	// The first fragment with a non-full payload defines the completion
	// boundary. A higher buffered fragment would contradict that boundary.
	اگر (is_non_بھرا_fragment && hasFragmentAfter(fragment_id)) {
		reset_فعال_State();
		برآمد_len = sequence_change_پیغام_len;
		واپس sequence_change_پیغام;
	}

	Fragment_خانہ &fragment = _fragments[fragment_id];

	اگر (fragment.present && ((fragment.len != len) || (memcmp(fragment.ڈیٹا, ڈیٹا, len) != 0))) {
		// Reusing a slot with different bytes means the current buffer no longer
		// matches the incoming fragments. Drop the old partial state and treat
		// this packet as the start of a new buffer for the same sequence ID.
		reset_فعال_State();
		فعال_sequence.فعال_2 = سچ;
		فعال_sequence.sequence_id = sequence_id;
	}

	حافظہ_نقل(fragment.ڈیٹا, ڈیٹا, len);
	fragment.len = len;
	fragment.present = سچ;
	فعال_sequence.timestamp = timestamp;

	// The buffer is complete when either all 4 fragments are present, or when
	// the first fragment with a non-full payload has been received and every
	// lower fragment ID is already present. Recording fragment 3 as the
	// boundary covers the "all 4 fragments" case. A non-full fragment 0 also
	// completes immediately, although a compliant sender would normally send
	// that packet as unfragmented instead of setting the fragmented bit.
	اگر (is_non_بھرا_fragment || (fragment_id == GPS_RTCM_MAX_FRAGMENTS - 1)) {
		فعال_sequence.آخری_fragment_id = fragment_id;
	}

	اگر (!isComplete()) {
		برآمد_len = sequence_change_پیغام_len;
		واپس sequence_change_پیغام;
	}

	size_t assembled_len = 0;
	uint8_t *destination = (sequence_change_پیغام != خالی_اشاریہ) ? deferred_پیغام : assembled_پیغام;

	برائے (size_t i = 0; i <= آخری_Fragment_اشاریہ(); i++) {
		ثابت Fragment_خانہ &خانہ = _fragments[i];
		حافظہ_نقل(&destination[assembled_len], خانہ.ڈیٹا, خانہ.len);
		assembled_len += خانہ.len;
	}

	reset_فعال_State();

	اگر (sequence_change_پیغام != خالی_اشاریہ) {
		deferred_پیغام_len = assembled_len;
		deferred_پیغام_valid = سچ;
		برآمد_len = sequence_change_پیغام_len;
		واپس sequence_change_پیغام;
	}

	برآمد_len = assembled_len;
	واپس assembled_پیغام;
}

ثابت uint8_t *Gps_Rtcm_پیغام_Assembler::take_Deferred_پیغام(size_t &برآمد_len)
{
	برآمد_len = 0;

	اگر (!deferred_پیغام_valid) {
		واپس خالی_اشاریہ;
	}

	deferred_پیغام_valid = جھوٹ;
	برآمد_len = deferred_پیغام_len;
	واپس deferred_پیغام;
}

خالی Gps_Rtcm_پیغام_Assembler::reset()
{
	reset_فعال_State();
	deferred_پیغام_len = 0;
	deferred_پیغام_valid = جھوٹ;
}

خالی Gps_Rtcm_پیغام_Assembler::reset_فعال_State()
{
	برائے (Fragment_خانہ &fragment : _fragments) {
		fragment.present = جھوٹ;
		fragment.len = 0;
	}

	فعال_sequence = {};
}

منطقی Gps_Rtcm_پیغام_Assembler::hasFragmentAfter(uint8_t fragment_id) ثابت
{
	برائے (size_t i = fragment_id + 1; i < GPS_RTCM_MAX_FRAGMENTS; i++) {
		اگر (_fragments[i].present) {
			واپس سچ;
		}
	}

	واپس جھوٹ;
}

size_t Gps_Rtcm_پیغام_Assembler::assemble_Legacy_Exact_مضرب_پیغام(uint8_t *destination) ثابت
{
	size_t بھرا_fragments = 0;

	// Extension to MAVLink rule: accept legacy senders that omit the required
	// zero-length terminator for exact multiples of 180 bytes, but only when
	// every buffered fragment forms a gap-free full-size prefix starting at
	// fragment 0.
	برائے (; بھرا_fragments < GPS_RTCM_MAX_FRAGMENTS; بھرا_fragments++) {
		اگر (!_fragments[بھرا_fragments].present) {
			توڑو;
		}

		اگر (_fragments[بھرا_fragments].len != GPS_RTCM_MAX_FRAGMENT_LEN) {
			توڑو;
		}
	}

	// Reject buffers like [180, _, 180]: only flush when every buffered
	// fragment belongs to one contiguous full-size run starting at fragment 0.
	اگر ((بھرا_fragments == 0) || hasFragmentAfter(بھرا_fragments - 1)) {
		واپس 0;
	}

	size_t assembled_len = 0;

	برائے (size_t i = 0; i < بھرا_fragments; i++) {
		ثابت Fragment_خانہ &خانہ = _fragments[i];
		حافظہ_نقل(&destination[assembled_len], خانہ.ڈیٹا, خانہ.len);
		assembled_len += خانہ.len;
	}

	واپس assembled_len;
}

منطقی Gps_Rtcm_پیغام_Assembler::isComplete() ثابت
{
	برائے (size_t i = 0; i <= آخری_Fragment_اشاریہ(); i++) {
		اگر (!_fragments[i].present) {
			واپس جھوٹ;
		}
	}

	واپس سچ;
}

size_t Gps_Rtcm_پیغام_Assembler::آخری_Fragment_اشاریہ() ثابت
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
	واپس (فعال_sequence.آخری_fragment_id >= 0) ? static_cast<size_t>(فعال_sequence.آخری_fragment_id) :
	       (GPS_RTCM_MAX_FRAGMENTS - 1);
}

} // namespace gnss
