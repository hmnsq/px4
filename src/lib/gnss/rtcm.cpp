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

#ಅನ್ತರ್ಭಾವ "rtcm.h"
#ಅನ್ತರ್ಭಾವ <cstring>

ನಾಮಾಕಾಶ gnss
{

uint32_t rtcm3_crc24q(ಸ್ಥಿರಾಂಕ uint8_t *ದತ್ತಾಂಶ, size_t len)
{
	uint32_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = 0;

	ಗಾಗಿ (size_t i = 0; i < len; i++) {
		ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ ^= static_cast<uint32_t>(ದತ್ತಾಂಶ[i]) << 16;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < 8; j++) {
			ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ <<= 1;

			ಆದರೆ (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ & 0x1000000) {
				ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ ^= RTCM3_CRC24Q_POLY;
			}
		}
	}

	ಹಿಂತಿರುಗಿ ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ & 0xFFFFFF;
}

ಸ್ಥಿರಾಂಕ uint8_t *Gps_Rtcm_ಸಂದೇಶ_Assembler::ಕೂಡಿಸು_Packet(uint8_t flags, ಸ್ಥಿರಾಂಕ uint8_t *ದತ್ತಾಂಶ, size_t len, uint64_t timestamp,
		size_t &ಹೊರಹೋಗು_len)
{
	ಹೊರಹೋಗು_len = 0;

	ಆದರೆ (ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
		reset();
		ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
	}

	// Timeout, clear stale partial state.
	ಆದರೆ (ಸಕ್ರಿಯ_sequence.ಸಕ್ರಿಯ_2 && (ಸಕ್ರಿಯ_sequence.timestamp != 0)
	    && (timestamp > ಸಕ್ರಿಯ_sequence.timestamp + GPS_RTCM_FRAGMENT_TIMEOUT_US)) {
		reset_ಸಕ್ರಿಯ_State();
	}

	ಆದರೆ (!gps_rtcm_is_fragmented(flags)) {
		// Compatibility fallback for older QGroundControl builds
		// that omit the final zero-length fragment
		ಸ್ಥಿರಾಂಕ size_t legacy_ಸಂದೇಶ_len = assemble_Legacy_Exact_ಅಪವರ್ತ್ಯ_ಸಂದೇಶ(assembled_ಸಂದೇಶ);

		// New fragment is invalid, return previous message if valid
		ಆದರೆ (len > ಗಾತ್ರ(assembled_ಸಂದೇಶ)) {
			reset();
			ಹೊರಹೋಗು_len = legacy_ಸಂದೇಶ_len;
			ಹಿಂತಿರುಗಿ (legacy_ಸಂದೇಶ_len > 0) ? assembled_ಸಂದೇಶ : ಶೂನ್ಯಸೂಚಕ;
		}

		ಆದರೆ (legacy_ಸಂದೇಶ_len > 0) {
			// Return the older buffered message first. Queue this new
			// unfragmented packet as deferred output.
			ಸ್ಮರಣೆ_ನಕಲು(deferred_ಸಂದೇಶ, ದತ್ತಾಂಶ, len);
			deferred_ಸಂದೇಶ_len = len;
			deferred_ಸಂದೇಶ_valid = ನಿಜ;
			reset_ಸಕ್ರಿಯ_State();
			ಹೊರಹೋಗು_len = legacy_ಸಂದೇಶ_len;
			ಹಿಂತಿರುಗಿ assembled_ಸಂದೇಶ;
		}

		reset();
		ಸ್ಮರಣೆ_ನಕಲು(assembled_ಸಂದೇಶ, ದತ್ತಾಂಶ, len);
		ಹೊರಹೋಗು_len = len;

		ಹಿಂತಿರುಗಿ assembled_ಸಂದೇಶ;
	}

	ಆದರೆ (len > GPS_RTCM_MAX_FRAGMENT_LEN) {
		reset_ಸಕ್ರಿಯ_State();
		ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
	}

	ಸ್ಥಿರಾಂಕ uint8_t sequence_id = gps_rtcm_sequence_id(flags);
	ಸ್ಥಿರಾಂಕ uint8_t fragment_id = gps_rtcm_fragment_id(flags);
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ is_non_ತುಂಬಿದ_fragment = len < GPS_RTCM_MAX_FRAGMENT_LEN;
	ಸ್ಥಿರಾಂಕ uint8_t *sequence_change_ಸಂದೇಶ = ಶೂನ್ಯಸೂಚಕ;
	size_t sequence_change_ಸಂದೇಶ_len = 0;

	ಆದರೆ (!ಸಕ್ರಿಯ_sequence.ಸಕ್ರಿಯ_2 || sequence_id != ಸಕ್ರಿಯ_sequence.sequence_id) {
		ಆದರೆ (ಸಕ್ರಿಯ_sequence.ಸಕ್ರಿಯ_2) {
			// Compatibility fallback for older QGroundControl builds
			// that omit the final zero-length fragment
			sequence_change_ಸಂದೇಶ_len = assemble_Legacy_Exact_ಅಪವರ್ತ್ಯ_ಸಂದೇಶ(assembled_ಸಂದೇಶ);

			ಆದರೆ (sequence_change_ಸಂದೇಶ_len > 0) {
				// Do not return here: this same packet also starts the new
				// sequence, so it still needs to be validated and buffered.
				sequence_change_ಸಂದೇಶ = assembled_ಸಂದೇಶ;
			}
		}

		reset_ಸಕ್ರಿಯ_State();
		ಸಕ್ರಿಯ_sequence.ಸಕ್ರಿಯ_2 = ನಿಜ;
		ಸಕ್ರಿಯ_sequence.sequence_id = sequence_id;
	}

	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ ಕೊನೆಯ_fragment_ತಿಳಿದ = (ಸಕ್ರಿಯ_sequence.ಕೊನೆಯ_fragment_id >= 0);

	// Once a non-full final fragment is known, any later fragment index is invalid.
	ಆದರೆ (ಕೊನೆಯ_fragment_ತಿಳಿದ && (fragment_id > ಸಕ್ರಿಯ_sequence.ಕೊನೆಯ_fragment_id)) {
		reset_ಸಕ್ರಿಯ_State();
		ಹೊರಹೋಗು_len = sequence_change_ಸಂದೇಶ_len;
		ಹಿಂತಿರುಗಿ sequence_change_ಸಂದೇಶ;
	}

	// A non-full fragment before the known end would contradict the current
	// end-of-message boundary. That can only come from malformed sender state.
	ಆದರೆ (ಕೊನೆಯ_fragment_ತಿಳಿದ && (fragment_id < ಸಕ್ರಿಯ_sequence.ಕೊನೆಯ_fragment_id) && is_non_ತುಂಬಿದ_fragment) {
		reset_ಸಕ್ರಿಯ_State();
		ಹೊರಹೋಗು_len = sequence_change_ಸಂದೇಶ_len;
		ಹಿಂತಿರುಗಿ sequence_change_ಸಂದೇಶ;
	}

	// The first fragment with a non-full payload defines the completion
	// boundary. A higher buffered fragment would contradict that boundary.
	ಆದರೆ (is_non_ತುಂಬಿದ_fragment && hasFragmentAfter(fragment_id)) {
		reset_ಸಕ್ರಿಯ_State();
		ಹೊರಹೋಗು_len = sequence_change_ಸಂದೇಶ_len;
		ಹಿಂತಿರುಗಿ sequence_change_ಸಂದೇಶ;
	}

	Fragment_ಸ್ಥಾನ &fragment = _fragments[fragment_id];

	ಆದರೆ (fragment.present && ((fragment.len != len) || (memcmp(fragment.ದತ್ತಾಂಶ, ದತ್ತಾಂಶ, len) != 0))) {
		// Reusing a slot with different bytes means the current buffer no longer
		// matches the incoming fragments. Drop the old partial state and treat
		// this packet as the start of a new buffer for the same sequence ID.
		reset_ಸಕ್ರಿಯ_State();
		ಸಕ್ರಿಯ_sequence.ಸಕ್ರಿಯ_2 = ನಿಜ;
		ಸಕ್ರಿಯ_sequence.sequence_id = sequence_id;
	}

	ಸ್ಮರಣೆ_ನಕಲು(fragment.ದತ್ತಾಂಶ, ದತ್ತಾಂಶ, len);
	fragment.len = len;
	fragment.present = ನಿಜ;
	ಸಕ್ರಿಯ_sequence.timestamp = timestamp;

	// The buffer is complete when either all 4 fragments are present, or when
	// the first fragment with a non-full payload has been received and every
	// lower fragment ID is already present. Recording fragment 3 as the
	// boundary covers the "all 4 fragments" case. A non-full fragment 0 also
	// completes immediately, although a compliant sender would normally send
	// that packet as unfragmented instead of setting the fragmented bit.
	ಆದರೆ (is_non_ತುಂಬಿದ_fragment || (fragment_id == GPS_RTCM_MAX_FRAGMENTS - 1)) {
		ಸಕ್ರಿಯ_sequence.ಕೊನೆಯ_fragment_id = fragment_id;
	}

	ಆದರೆ (!isComplete()) {
		ಹೊರಹೋಗು_len = sequence_change_ಸಂದೇಶ_len;
		ಹಿಂತಿರುಗಿ sequence_change_ಸಂದೇಶ;
	}

	size_t assembled_len = 0;
	uint8_t *destination = (sequence_change_ಸಂದೇಶ != ಶೂನ್ಯಸೂಚಕ) ? deferred_ಸಂದೇಶ : assembled_ಸಂದೇಶ;

	ಗಾಗಿ (size_t i = 0; i <= ಕೊನೆಯ_Fragment_ಸೂಚ್ಯಂಕ(); i++) {
		ಸ್ಥಿರಾಂಕ Fragment_ಸ್ಥಾನ &ಸ್ಥಾನ = _fragments[i];
		ಸ್ಮರಣೆ_ನಕಲು(&destination[assembled_len], ಸ್ಥಾನ.ದತ್ತಾಂಶ, ಸ್ಥಾನ.len);
		assembled_len += ಸ್ಥಾನ.len;
	}

	reset_ಸಕ್ರಿಯ_State();

	ಆದರೆ (sequence_change_ಸಂದೇಶ != ಶೂನ್ಯಸೂಚಕ) {
		deferred_ಸಂದೇಶ_len = assembled_len;
		deferred_ಸಂದೇಶ_valid = ನಿಜ;
		ಹೊರಹೋಗು_len = sequence_change_ಸಂದೇಶ_len;
		ಹಿಂತಿರುಗಿ sequence_change_ಸಂದೇಶ;
	}

	ಹೊರಹೋಗು_len = assembled_len;
	ಹಿಂತಿರುಗಿ assembled_ಸಂದೇಶ;
}

ಸ್ಥಿರಾಂಕ uint8_t *Gps_Rtcm_ಸಂದೇಶ_Assembler::take_Deferred_ಸಂದೇಶ(size_t &ಹೊರಹೋಗು_len)
{
	ಹೊರಹೋಗು_len = 0;

	ಆದರೆ (!deferred_ಸಂದೇಶ_valid) {
		ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
	}

	deferred_ಸಂದೇಶ_valid = ಸುಳ್ಳು;
	ಹೊರಹೋಗು_len = deferred_ಸಂದೇಶ_len;
	ಹಿಂತಿರುಗಿ deferred_ಸಂದೇಶ;
}

ಶೂನ್ಯ Gps_Rtcm_ಸಂದೇಶ_Assembler::reset()
{
	reset_ಸಕ್ರಿಯ_State();
	deferred_ಸಂದೇಶ_len = 0;
	deferred_ಸಂದೇಶ_valid = ಸುಳ್ಳು;
}

ಶೂನ್ಯ Gps_Rtcm_ಸಂದೇಶ_Assembler::reset_ಸಕ್ರಿಯ_State()
{
	ಗಾಗಿ (Fragment_ಸ್ಥಾನ &fragment : _fragments) {
		fragment.present = ಸುಳ್ಳು;
		fragment.len = 0;
	}

	ಸಕ್ರಿಯ_sequence = {};
}

ತಾರ್ಕಿಕ Gps_Rtcm_ಸಂದೇಶ_Assembler::hasFragmentAfter(uint8_t fragment_id) ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (size_t i = fragment_id + 1; i < GPS_RTCM_MAX_FRAGMENTS; i++) {
		ಆದರೆ (_fragments[i].present) {
			ಹಿಂತಿರುಗಿ ನಿಜ;
		}
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

size_t Gps_Rtcm_ಸಂದೇಶ_Assembler::assemble_Legacy_Exact_ಅಪವರ್ತ್ಯ_ಸಂದೇಶ(uint8_t *destination) ಸ್ಥಿರಾಂಕ
{
	size_t ತುಂಬಿದ_fragments = 0;

	// Extension to MAVLink rule: accept legacy senders that omit the required
	// zero-length terminator for exact multiples of 180 bytes, but only when
	// every buffered fragment forms a gap-free full-size prefix starting at
	// fragment 0.
	ಗಾಗಿ (; ತುಂಬಿದ_fragments < GPS_RTCM_MAX_FRAGMENTS; ತುಂಬಿದ_fragments++) {
		ಆದರೆ (!_fragments[ತುಂಬಿದ_fragments].present) {
			ನಿಲ್ಲಿಸು;
		}

		ಆದರೆ (_fragments[ತುಂಬಿದ_fragments].len != GPS_RTCM_MAX_FRAGMENT_LEN) {
			ನಿಲ್ಲಿಸು;
		}
	}

	// Reject buffers like [180, _, 180]: only flush when every buffered
	// fragment belongs to one contiguous full-size run starting at fragment 0.
	ಆದರೆ ((ತುಂಬಿದ_fragments == 0) || hasFragmentAfter(ತುಂಬಿದ_fragments - 1)) {
		ಹಿಂತಿರುಗಿ 0;
	}

	size_t assembled_len = 0;

	ಗಾಗಿ (size_t i = 0; i < ತುಂಬಿದ_fragments; i++) {
		ಸ್ಥಿರಾಂಕ Fragment_ಸ್ಥಾನ &ಸ್ಥಾನ = _fragments[i];
		ಸ್ಮರಣೆ_ನಕಲು(&destination[assembled_len], ಸ್ಥಾನ.ದತ್ತಾಂಶ, ಸ್ಥಾನ.len);
		assembled_len += ಸ್ಥಾನ.len;
	}

	ಹಿಂತಿರುಗಿ assembled_len;
}

ತಾರ್ಕಿಕ Gps_Rtcm_ಸಂದೇಶ_Assembler::isComplete() ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (size_t i = 0; i <= ಕೊನೆಯ_Fragment_ಸೂಚ್ಯಂಕ(); i++) {
		ಆದರೆ (!_fragments[i].present) {
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

size_t Gps_Rtcm_ಸಂದೇಶ_Assembler::ಕೊನೆಯ_Fragment_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ
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
	ಹಿಂತಿರುಗಿ (ಸಕ್ರಿಯ_sequence.ಕೊನೆಯ_fragment_id >= 0) ? static_cast<size_t>(ಸಕ್ರಿಯ_sequence.ಕೊನೆಯ_fragment_id) :
	       (GPS_RTCM_MAX_FRAGMENTS - 1);
}

} // namespace gnss
