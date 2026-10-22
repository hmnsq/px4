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

#ضمن "rtcm.h"
#ضمن <cstring>

نطاق gnss
{

uint32_t rtcm3_crc24q(ثابت uint8_t *انکڑے, size_t len)
{
	uint32_t چکری_جانچ = 0;

	لكل (size_t i = 0; i < len; i++) {
		چکری_جانچ ^= static_cast<uint32_t>(انکڑے[i]) << 16;

		لكل (صحيح j = 0; j < 8; j++) {
			چکری_جانچ <<= 1;

			إذا (چکری_جانچ & 0x1000000) {
				چکری_جانچ ^= RTCM3_CRC24Q_POLY;
			}
		}
	}

	أرجع چکری_جانچ & 0xFFFFFF;
}

ثابت uint8_t *Gps_Rtcm_سنیہا_Assembler::جوڑو_Packet(uint8_t flags, ثابت uint8_t *انکڑے, size_t len, uint64_t timestamp,
		size_t &باہرلا_len)
{
	باہرلا_len = 0;

	إذا (انکڑے == خالی_سوچک) {
		reset();
		أرجع خالی_سوچک;
	}

	// Timeout, clear stale partial state.
	إذا (سرگرم_sequence.سرگرم_2 && (سرگرم_sequence.timestamp != 0)
	    && (timestamp > سرگرم_sequence.timestamp + GPS_RTCM_FRAGMENT_TIMEOUT_US)) {
		reset_سرگرم_State();
	}

	إذا (!gps_rtcm_is_fragmented(flags)) {
		// Compatibility fallback for older QGroundControl builds
		// that omit the final zero-length fragment
		ثابت size_t legacy_سنیہا_len = assemble_Legacy_Exact_مضرب_سنیہا(assembled_سنیہا);

		// New fragment is invalid, return previous message if valid
		إذا (len > حجم(assembled_سنیہا)) {
			reset();
			باہرلا_len = legacy_سنیہا_len;
			أرجع (legacy_سنیہا_len > 0) ? assembled_سنیہا : خالی_سوچک;
		}

		إذا (legacy_سنیہا_len > 0) {
			// Return the older buffered message first. Queue this new
			// unfragmented packet as deferred output.
			حافظہ_نقل(deferred_سنیہا, انکڑے, len);
			deferred_سنیہا_len = len;
			deferred_سنیہا_valid = سچ;
			reset_سرگرم_State();
			باہرلا_len = legacy_سنیہا_len;
			أرجع assembled_سنیہا;
		}

		reset();
		حافظہ_نقل(assembled_سنیہا, انکڑے, len);
		باہرلا_len = len;

		أرجع assembled_سنیہا;
	}

	إذا (len > GPS_RTCM_MAX_FRAGMENT_LEN) {
		reset_سرگرم_State();
		أرجع خالی_سوچک;
	}

	ثابت uint8_t sequence_id = gps_rtcm_sequence_id(flags);
	ثابت uint8_t fragment_id = gps_rtcm_fragment_id(flags);
	ثابت منطقي is_non_بھریا_fragment = len < GPS_RTCM_MAX_FRAGMENT_LEN;
	ثابت uint8_t *sequence_change_سنیہا = خالی_سوچک;
	size_t sequence_change_سنیہا_len = 0;

	إذا (!سرگرم_sequence.سرگرم_2 || sequence_id != سرگرم_sequence.sequence_id) {
		إذا (سرگرم_sequence.سرگرم_2) {
			// Compatibility fallback for older QGroundControl builds
			// that omit the final zero-length fragment
			sequence_change_سنیہا_len = assemble_Legacy_Exact_مضرب_سنیہا(assembled_سنیہا);

			إذا (sequence_change_سنیہا_len > 0) {
				// Do not return here: this same packet also starts the new
				// sequence, so it still needs to be validated and buffered.
				sequence_change_سنیہا = assembled_سنیہا;
			}
		}

		reset_سرگرم_State();
		سرگرم_sequence.سرگرم_2 = سچ;
		سرگرم_sequence.sequence_id = sequence_id;
	}

	ثابت منطقي آخری_fragment_معلوم = (سرگرم_sequence.آخری_fragment_id >= 0);

	// Once a non-full final fragment is known, any later fragment index is invalid.
	إذا (آخری_fragment_معلوم && (fragment_id > سرگرم_sequence.آخری_fragment_id)) {
		reset_سرگرم_State();
		باہرلا_len = sequence_change_سنیہا_len;
		أرجع sequence_change_سنیہا;
	}

	// A non-full fragment before the known end would contradict the current
	// end-of-message boundary. That can only come from malformed sender state.
	إذا (آخری_fragment_معلوم && (fragment_id < سرگرم_sequence.آخری_fragment_id) && is_non_بھریا_fragment) {
		reset_سرگرم_State();
		باہرلا_len = sequence_change_سنیہا_len;
		أرجع sequence_change_سنیہا;
	}

	// The first fragment with a non-full payload defines the completion
	// boundary. A higher buffered fragment would contradict that boundary.
	إذا (is_non_بھریا_fragment && hasFragmentAfter(fragment_id)) {
		reset_سرگرم_State();
		باہرلا_len = sequence_change_سنیہا_len;
		أرجع sequence_change_سنیہا;
	}

	Fragment_تھاں &fragment = _fragments[fragment_id];

	إذا (fragment.present && ((fragment.len != len) || (memcmp(fragment.انکڑے, انکڑے, len) != 0))) {
		// Reusing a slot with different bytes means the current buffer no longer
		// matches the incoming fragments. Drop the old partial state and treat
		// this packet as the start of a new buffer for the same sequence ID.
		reset_سرگرم_State();
		سرگرم_sequence.سرگرم_2 = سچ;
		سرگرم_sequence.sequence_id = sequence_id;
	}

	حافظہ_نقل(fragment.انکڑے, انکڑے, len);
	fragment.len = len;
	fragment.present = سچ;
	سرگرم_sequence.timestamp = timestamp;

	// The buffer is complete when either all 4 fragments are present, or when
	// the first fragment with a non-full payload has been received and every
	// lower fragment ID is already present. Recording fragment 3 as the
	// boundary covers the "all 4 fragments" case. A non-full fragment 0 also
	// completes immediately, although a compliant sender would normally send
	// that packet as unfragmented instead of setting the fragmented bit.
	إذا (is_non_بھریا_fragment || (fragment_id == GPS_RTCM_MAX_FRAGMENTS - 1)) {
		سرگرم_sequence.آخری_fragment_id = fragment_id;
	}

	إذا (!isComplete()) {
		باہرلا_len = sequence_change_سنیہا_len;
		أرجع sequence_change_سنیہا;
	}

	size_t assembled_len = 0;
	uint8_t *destination = (sequence_change_سنیہا != خالی_سوچک) ? deferred_سنیہا : assembled_سنیہا;

	لكل (size_t i = 0; i <= آخری_Fragment_اشاریہ(); i++) {
		ثابت Fragment_تھاں &تھاں = _fragments[i];
		حافظہ_نقل(&destination[assembled_len], تھاں.انکڑے, تھاں.len);
		assembled_len += تھاں.len;
	}

	reset_سرگرم_State();

	إذا (sequence_change_سنیہا != خالی_سوچک) {
		deferred_سنیہا_len = assembled_len;
		deferred_سنیہا_valid = سچ;
		باہرلا_len = sequence_change_سنیہا_len;
		أرجع sequence_change_سنیہا;
	}

	باہرلا_len = assembled_len;
	أرجع assembled_سنیہا;
}

ثابت uint8_t *Gps_Rtcm_سنیہا_Assembler::take_Deferred_سنیہا(size_t &باہرلا_len)
{
	باہرلا_len = 0;

	إذا (!deferred_سنیہا_valid) {
		أرجع خالی_سوچک;
	}

	deferred_سنیہا_valid = خطأ;
	باہرلا_len = deferred_سنیہا_len;
	أرجع deferred_سنیہا;
}

فراغ Gps_Rtcm_سنیہا_Assembler::reset()
{
	reset_سرگرم_State();
	deferred_سنیہا_len = 0;
	deferred_سنیہا_valid = خطأ;
}

فراغ Gps_Rtcm_سنیہا_Assembler::reset_سرگرم_State()
{
	لكل (Fragment_تھاں &fragment : _fragments) {
		fragment.present = خطأ;
		fragment.len = 0;
	}

	سرگرم_sequence = {};
}

منطقي Gps_Rtcm_سنیہا_Assembler::hasFragmentAfter(uint8_t fragment_id) ثابت
{
	لكل (size_t i = fragment_id + 1; i < GPS_RTCM_MAX_FRAGMENTS; i++) {
		إذا (_fragments[i].present) {
			أرجع سچ;
		}
	}

	أرجع خطأ;
}

size_t Gps_Rtcm_سنیہا_Assembler::assemble_Legacy_Exact_مضرب_سنیہا(uint8_t *destination) ثابت
{
	size_t بھریا_fragments = 0;

	// Extension to MAVLink rule: accept legacy senders that omit the required
	// zero-length terminator for exact multiples of 180 bytes, but only when
	// every buffered fragment forms a gap-free full-size prefix starting at
	// fragment 0.
	لكل (; بھریا_fragments < GPS_RTCM_MAX_FRAGMENTS; بھریا_fragments++) {
		إذا (!_fragments[بھریا_fragments].present) {
			اكسر;
		}

		إذا (_fragments[بھریا_fragments].len != GPS_RTCM_MAX_FRAGMENT_LEN) {
			اكسر;
		}
	}

	// Reject buffers like [180, _, 180]: only flush when every buffered
	// fragment belongs to one contiguous full-size run starting at fragment 0.
	إذا ((بھریا_fragments == 0) || hasFragmentAfter(بھریا_fragments - 1)) {
		أرجع 0;
	}

	size_t assembled_len = 0;

	لكل (size_t i = 0; i < بھریا_fragments; i++) {
		ثابت Fragment_تھاں &تھاں = _fragments[i];
		حافظہ_نقل(&destination[assembled_len], تھاں.انکڑے, تھاں.len);
		assembled_len += تھاں.len;
	}

	أرجع assembled_len;
}

منطقي Gps_Rtcm_سنیہا_Assembler::isComplete() ثابت
{
	لكل (size_t i = 0; i <= آخری_Fragment_اشاریہ(); i++) {
		إذا (!_fragments[i].present) {
			أرجع خطأ;
		}
	}

	أرجع سچ;
}

size_t Gps_Rtcm_سنیہا_Assembler::آخری_Fragment_اشاریہ() ثابت
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
	أرجع (سرگرم_sequence.آخری_fragment_id >= 0) ? static_cast<size_t>(سرگرم_sequence.آخری_fragment_id) :
	       (GPS_RTCM_MAX_FRAGMENTS - 1);
}

} // namespace gnss
