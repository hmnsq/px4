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

#ਅਨ੍ਤਰ੍ਭਾਵ "rtcm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>

ਨਾਮਾਕਾਸ਼ gnss
{

uint32_t rtcm3_crc24q(ਅਚਲ uint8_t *ਅੰਕੜੇ, size_t len)
{
	uint32_t ਚੱਕਰੀ_ਜਾਂਚ = 0;

	ਲਈ (size_t i = 0; i < len; i++) {
		ਚੱਕਰੀ_ਜਾਂਚ ^= static_cast<uint32_t>(ਅੰਕੜੇ[i]) << 16;

		ਲਈ (ਪੂਰਨ j = 0; j < 8; j++) {
			ਚੱਕਰੀ_ਜਾਂਚ <<= 1;

			ਜੇ (ਚੱਕਰੀ_ਜਾਂਚ & 0x1000000) {
				ਚੱਕਰੀ_ਜਾਂਚ ^= RTCM3_CRC24Q_POLY;
			}
		}
	}

	ਵਾਪਸ ਚੱਕਰੀ_ਜਾਂਚ & 0xFFFFFF;
}

ਅਚਲ uint8_t *Gps_Rtcm_ਸੁਨੇਹਾ_Assembler::ਜੋੜੋ_Packet(uint8_t flags, ਅਚਲ uint8_t *ਅੰਕੜੇ, size_t len, uint64_t timestamp,
		size_t &ਬਾਹਰਲਾ_len)
{
	ਬਾਹਰਲਾ_len = 0;

	ਜੇ (ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
		reset();
		ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
	}

	// Timeout, clear stale partial state.
	ਜੇ (ਸਰਗਰਮ_sequence.ਸਰਗਰਮ_2 && (ਸਰਗਰਮ_sequence.timestamp != 0)
	    && (timestamp > ਸਰਗਰਮ_sequence.timestamp + GPS_RTCM_FRAGMENT_TIMEOUT_US)) {
		reset_ਸਰਗਰਮ_State();
	}

	ਜੇ (!gps_rtcm_is_fragmented(flags)) {
		// Compatibility fallback for older QGroundControl builds
		// that omit the final zero-length fragment
		ਅਚਲ size_t legacy_ਸੁਨੇਹਾ_len = assemble_Legacy_Exact_ਗੁਣਜ_ਸੁਨੇਹਾ(assembled_ਸੁਨੇਹਾ);

		// New fragment is invalid, return previous message if valid
		ਜੇ (len > ਆਕਾਰ(assembled_ਸੁਨੇਹਾ)) {
			reset();
			ਬਾਹਰਲਾ_len = legacy_ਸੁਨੇਹਾ_len;
			ਵਾਪਸ (legacy_ਸੁਨੇਹਾ_len > 0) ? assembled_ਸੁਨੇਹਾ : ਖਾਲੀ_ਸੂਚਕ;
		}

		ਜੇ (legacy_ਸੁਨੇਹਾ_len > 0) {
			// Return the older buffered message first. Queue this new
			// unfragmented packet as deferred output.
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(deferred_ਸੁਨੇਹਾ, ਅੰਕੜੇ, len);
			deferred_ਸੁਨੇਹਾ_len = len;
			deferred_ਸੁਨੇਹਾ_valid = ਸੱਚ;
			reset_ਸਰਗਰਮ_State();
			ਬਾਹਰਲਾ_len = legacy_ਸੁਨੇਹਾ_len;
			ਵਾਪਸ assembled_ਸੁਨੇਹਾ;
		}

		reset();
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(assembled_ਸੁਨੇਹਾ, ਅੰਕੜੇ, len);
		ਬਾਹਰਲਾ_len = len;

		ਵਾਪਸ assembled_ਸੁਨੇਹਾ;
	}

	ਜੇ (len > GPS_RTCM_MAX_FRAGMENT_LEN) {
		reset_ਸਰਗਰਮ_State();
		ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
	}

	ਅਚਲ uint8_t sequence_id = gps_rtcm_sequence_id(flags);
	ਅਚਲ uint8_t fragment_id = gps_rtcm_fragment_id(flags);
	ਅਚਲ ਤਾਰਕਿਕ is_non_ਭਰਿਆ_fragment = len < GPS_RTCM_MAX_FRAGMENT_LEN;
	ਅਚਲ uint8_t *sequence_change_ਸੁਨੇਹਾ = ਖਾਲੀ_ਸੂਚਕ;
	size_t sequence_change_ਸੁਨੇਹਾ_len = 0;

	ਜੇ (!ਸਰਗਰਮ_sequence.ਸਰਗਰਮ_2 || sequence_id != ਸਰਗਰਮ_sequence.sequence_id) {
		ਜੇ (ਸਰਗਰਮ_sequence.ਸਰਗਰਮ_2) {
			// Compatibility fallback for older QGroundControl builds
			// that omit the final zero-length fragment
			sequence_change_ਸੁਨੇਹਾ_len = assemble_Legacy_Exact_ਗੁਣਜ_ਸੁਨੇਹਾ(assembled_ਸੁਨੇਹਾ);

			ਜੇ (sequence_change_ਸੁਨੇਹਾ_len > 0) {
				// Do not return here: this same packet also starts the new
				// sequence, so it still needs to be validated and buffered.
				sequence_change_ਸੁਨੇਹਾ = assembled_ਸੁਨੇਹਾ;
			}
		}

		reset_ਸਰਗਰਮ_State();
		ਸਰਗਰਮ_sequence.ਸਰਗਰਮ_2 = ਸੱਚ;
		ਸਰਗਰਮ_sequence.sequence_id = sequence_id;
	}

	ਅਚਲ ਤਾਰਕਿਕ ਆਖਰੀ_fragment_ਗਿਆਤ = (ਸਰਗਰਮ_sequence.ਆਖਰੀ_fragment_id >= 0);

	// Once a non-full final fragment is known, any later fragment index is invalid.
	ਜੇ (ਆਖਰੀ_fragment_ਗਿਆਤ && (fragment_id > ਸਰਗਰਮ_sequence.ਆਖਰੀ_fragment_id)) {
		reset_ਸਰਗਰਮ_State();
		ਬਾਹਰਲਾ_len = sequence_change_ਸੁਨੇਹਾ_len;
		ਵਾਪਸ sequence_change_ਸੁਨੇਹਾ;
	}

	// A non-full fragment before the known end would contradict the current
	// end-of-message boundary. That can only come from malformed sender state.
	ਜੇ (ਆਖਰੀ_fragment_ਗਿਆਤ && (fragment_id < ਸਰਗਰਮ_sequence.ਆਖਰੀ_fragment_id) && is_non_ਭਰਿਆ_fragment) {
		reset_ਸਰਗਰਮ_State();
		ਬਾਹਰਲਾ_len = sequence_change_ਸੁਨੇਹਾ_len;
		ਵਾਪਸ sequence_change_ਸੁਨੇਹਾ;
	}

	// The first fragment with a non-full payload defines the completion
	// boundary. A higher buffered fragment would contradict that boundary.
	ਜੇ (is_non_ਭਰਿਆ_fragment && hasFragmentAfter(fragment_id)) {
		reset_ਸਰਗਰਮ_State();
		ਬਾਹਰਲਾ_len = sequence_change_ਸੁਨੇਹਾ_len;
		ਵਾਪਸ sequence_change_ਸੁਨੇਹਾ;
	}

	Fragment_ਥਾਂ &fragment = _fragments[fragment_id];

	ਜੇ (fragment.present && ((fragment.len != len) || (memcmp(fragment.ਅੰਕੜੇ, ਅੰਕੜੇ, len) != 0))) {
		// Reusing a slot with different bytes means the current buffer no longer
		// matches the incoming fragments. Drop the old partial state and treat
		// this packet as the start of a new buffer for the same sequence ID.
		reset_ਸਰਗਰਮ_State();
		ਸਰਗਰਮ_sequence.ਸਰਗਰਮ_2 = ਸੱਚ;
		ਸਰਗਰਮ_sequence.sequence_id = sequence_id;
	}

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(fragment.ਅੰਕੜੇ, ਅੰਕੜੇ, len);
	fragment.len = len;
	fragment.present = ਸੱਚ;
	ਸਰਗਰਮ_sequence.timestamp = timestamp;

	// The buffer is complete when either all 4 fragments are present, or when
	// the first fragment with a non-full payload has been received and every
	// lower fragment ID is already present. Recording fragment 3 as the
	// boundary covers the "all 4 fragments" case. A non-full fragment 0 also
	// completes immediately, although a compliant sender would normally send
	// that packet as unfragmented instead of setting the fragmented bit.
	ਜੇ (is_non_ਭਰਿਆ_fragment || (fragment_id == GPS_RTCM_MAX_FRAGMENTS - 1)) {
		ਸਰਗਰਮ_sequence.ਆਖਰੀ_fragment_id = fragment_id;
	}

	ਜੇ (!isComplete()) {
		ਬਾਹਰਲਾ_len = sequence_change_ਸੁਨੇਹਾ_len;
		ਵਾਪਸ sequence_change_ਸੁਨੇਹਾ;
	}

	size_t assembled_len = 0;
	uint8_t *destination = (sequence_change_ਸੁਨੇਹਾ != ਖਾਲੀ_ਸੂਚਕ) ? deferred_ਸੁਨੇਹਾ : assembled_ਸੁਨੇਹਾ;

	ਲਈ (size_t i = 0; i <= ਆਖਰੀ_Fragment_ਸੂਚਕਾਂਕ(); i++) {
		ਅਚਲ Fragment_ਥਾਂ &ਥਾਂ = _fragments[i];
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(&destination[assembled_len], ਥਾਂ.ਅੰਕੜੇ, ਥਾਂ.len);
		assembled_len += ਥਾਂ.len;
	}

	reset_ਸਰਗਰਮ_State();

	ਜੇ (sequence_change_ਸੁਨੇਹਾ != ਖਾਲੀ_ਸੂਚਕ) {
		deferred_ਸੁਨੇਹਾ_len = assembled_len;
		deferred_ਸੁਨੇਹਾ_valid = ਸੱਚ;
		ਬਾਹਰਲਾ_len = sequence_change_ਸੁਨੇਹਾ_len;
		ਵਾਪਸ sequence_change_ਸੁਨੇਹਾ;
	}

	ਬਾਹਰਲਾ_len = assembled_len;
	ਵਾਪਸ assembled_ਸੁਨੇਹਾ;
}

ਅਚਲ uint8_t *Gps_Rtcm_ਸੁਨੇਹਾ_Assembler::take_Deferred_ਸੁਨੇਹਾ(size_t &ਬਾਹਰਲਾ_len)
{
	ਬਾਹਰਲਾ_len = 0;

	ਜੇ (!deferred_ਸੁਨੇਹਾ_valid) {
		ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
	}

	deferred_ਸੁਨੇਹਾ_valid = ਝੂਠ;
	ਬਾਹਰਲਾ_len = deferred_ਸੁਨੇਹਾ_len;
	ਵਾਪਸ deferred_ਸੁਨੇਹਾ;
}

ਖਾਲੀ Gps_Rtcm_ਸੁਨੇਹਾ_Assembler::reset()
{
	reset_ਸਰਗਰਮ_State();
	deferred_ਸੁਨੇਹਾ_len = 0;
	deferred_ਸੁਨੇਹਾ_valid = ਝੂਠ;
}

ਖਾਲੀ Gps_Rtcm_ਸੁਨੇਹਾ_Assembler::reset_ਸਰਗਰਮ_State()
{
	ਲਈ (Fragment_ਥਾਂ &fragment : _fragments) {
		fragment.present = ਝੂਠ;
		fragment.len = 0;
	}

	ਸਰਗਰਮ_sequence = {};
}

ਤਾਰਕਿਕ Gps_Rtcm_ਸੁਨੇਹਾ_Assembler::hasFragmentAfter(uint8_t fragment_id) ਅਚਲ
{
	ਲਈ (size_t i = fragment_id + 1; i < GPS_RTCM_MAX_FRAGMENTS; i++) {
		ਜੇ (_fragments[i].present) {
			ਵਾਪਸ ਸੱਚ;
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

size_t Gps_Rtcm_ਸੁਨੇਹਾ_Assembler::assemble_Legacy_Exact_ਗੁਣਜ_ਸੁਨੇਹਾ(uint8_t *destination) ਅਚਲ
{
	size_t ਭਰਿਆ_fragments = 0;

	// Extension to MAVLink rule: accept legacy senders that omit the required
	// zero-length terminator for exact multiples of 180 bytes, but only when
	// every buffered fragment forms a gap-free full-size prefix starting at
	// fragment 0.
	ਲਈ (; ਭਰਿਆ_fragments < GPS_RTCM_MAX_FRAGMENTS; ਭਰਿਆ_fragments++) {
		ਜੇ (!_fragments[ਭਰਿਆ_fragments].present) {
			ਰੋਕੋ;
		}

		ਜੇ (_fragments[ਭਰਿਆ_fragments].len != GPS_RTCM_MAX_FRAGMENT_LEN) {
			ਰੋਕੋ;
		}
	}

	// Reject buffers like [180, _, 180]: only flush when every buffered
	// fragment belongs to one contiguous full-size run starting at fragment 0.
	ਜੇ ((ਭਰਿਆ_fragments == 0) || hasFragmentAfter(ਭਰਿਆ_fragments - 1)) {
		ਵਾਪਸ 0;
	}

	size_t assembled_len = 0;

	ਲਈ (size_t i = 0; i < ਭਰਿਆ_fragments; i++) {
		ਅਚਲ Fragment_ਥਾਂ &ਥਾਂ = _fragments[i];
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(&destination[assembled_len], ਥਾਂ.ਅੰਕੜੇ, ਥਾਂ.len);
		assembled_len += ਥਾਂ.len;
	}

	ਵਾਪਸ assembled_len;
}

ਤਾਰਕਿਕ Gps_Rtcm_ਸੁਨੇਹਾ_Assembler::isComplete() ਅਚਲ
{
	ਲਈ (size_t i = 0; i <= ਆਖਰੀ_Fragment_ਸੂਚਕਾਂਕ(); i++) {
		ਜੇ (!_fragments[i].present) {
			ਵਾਪਸ ਝੂਠ;
		}
	}

	ਵਾਪਸ ਸੱਚ;
}

size_t Gps_Rtcm_ਸੁਨੇਹਾ_Assembler::ਆਖਰੀ_Fragment_ਸੂਚਕਾਂਕ() ਅਚਲ
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
	ਵਾਪਸ (ਸਰਗਰਮ_sequence.ਆਖਰੀ_fragment_id >= 0) ? static_cast<size_t>(ਸਰਗਰਮ_sequence.ਆਖਰੀ_fragment_id) :
	       (GPS_RTCM_MAX_FRAGMENTS - 1);
}

} // namespace gnss
