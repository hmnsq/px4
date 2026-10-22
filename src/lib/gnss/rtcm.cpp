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

uint32_t rtcm3_crc24q(ثابت uint8_t *داده, size_t len)
{
	uint32_t بررسی_چرخهای = 0;

	لكل (size_t i = 0; i < len; i++) {
		بررسی_چرخهای ^= static_cast<uint32_t>(داده[i]) << 16;

		لكل (صحيح j = 0; j < 8; j++) {
			بررسی_چرخهای <<= 1;

			إذا (بررسی_چرخهای & 0x1000000) {
				بررسی_چرخهای ^= RTCM3_CRC24Q_POLY;
			}
		}
	}

	أرجع بررسی_چرخهای & 0xFFFFFF;
}

ثابت uint8_t *Gps_Rtcm_پیام_Assembler::جمع_کن_Packet(uint8_t flags, ثابت uint8_t *داده, size_t len, uint64_t timestamp,
		size_t &بیرون_len)
{
	بیرون_len = 0;

	إذا (داده == اشاره_تهی) {
		reset();
		أرجع اشاره_تهی;
	}

	// Timeout, clear stale partial state.
	إذا (فعال_sequence.فعال_2 && (فعال_sequence.timestamp != 0)
	    && (timestamp > فعال_sequence.timestamp + GPS_RTCM_FRAGMENT_TIMEOUT_US)) {
		reset_فعال_State();
	}

	إذا (!gps_rtcm_is_fragmented(flags)) {
		// Compatibility fallback for older QGroundControl builds
		// that omit the final zero-length fragment
		ثابت size_t legacy_پیام_len = assemble_Legacy_Exact_مضرب_پیام(assembled_پیام);

		// New fragment is invalid, return previous message if valid
		إذا (len > حجم(assembled_پیام)) {
			reset();
			بیرون_len = legacy_پیام_len;
			أرجع (legacy_پیام_len > 0) ? assembled_پیام : اشاره_تهی;
		}

		إذا (legacy_پیام_len > 0) {
			// Return the older buffered message first. Queue this new
			// unfragmented packet as deferred output.
			کپی_حافظه(deferred_پیام, داده, len);
			deferred_پیام_len = len;
			deferred_پیام_valid = درست;
			reset_فعال_State();
			بیرون_len = legacy_پیام_len;
			أرجع assembled_پیام;
		}

		reset();
		کپی_حافظه(assembled_پیام, داده, len);
		بیرون_len = len;

		أرجع assembled_پیام;
	}

	إذا (len > GPS_RTCM_MAX_FRAGMENT_LEN) {
		reset_فعال_State();
		أرجع اشاره_تهی;
	}

	ثابت uint8_t sequence_id = gps_rtcm_sequence_id(flags);
	ثابت uint8_t fragment_id = gps_rtcm_fragment_id(flags);
	ثابت منطقي is_non_پر_fragment = len < GPS_RTCM_MAX_FRAGMENT_LEN;
	ثابت uint8_t *sequence_change_پیام = اشاره_تهی;
	size_t sequence_change_پیام_len = 0;

	إذا (!فعال_sequence.فعال_2 || sequence_id != فعال_sequence.sequence_id) {
		إذا (فعال_sequence.فعال_2) {
			// Compatibility fallback for older QGroundControl builds
			// that omit the final zero-length fragment
			sequence_change_پیام_len = assemble_Legacy_Exact_مضرب_پیام(assembled_پیام);

			إذا (sequence_change_پیام_len > 0) {
				// Do not return here: this same packet also starts the new
				// sequence, so it still needs to be validated and buffered.
				sequence_change_پیام = assembled_پیام;
			}
		}

		reset_فعال_State();
		فعال_sequence.فعال_2 = درست;
		فعال_sequence.sequence_id = sequence_id;
	}

	ثابت منطقي آخرین_fragment_معلوم = (فعال_sequence.آخرین_fragment_id >= 0);

	// Once a non-full final fragment is known, any later fragment index is invalid.
	إذا (آخرین_fragment_معلوم && (fragment_id > فعال_sequence.آخرین_fragment_id)) {
		reset_فعال_State();
		بیرون_len = sequence_change_پیام_len;
		أرجع sequence_change_پیام;
	}

	// A non-full fragment before the known end would contradict the current
	// end-of-message boundary. That can only come from malformed sender state.
	إذا (آخرین_fragment_معلوم && (fragment_id < فعال_sequence.آخرین_fragment_id) && is_non_پر_fragment) {
		reset_فعال_State();
		بیرون_len = sequence_change_پیام_len;
		أرجع sequence_change_پیام;
	}

	// The first fragment with a non-full payload defines the completion
	// boundary. A higher buffered fragment would contradict that boundary.
	إذا (is_non_پر_fragment && hasFragmentAfter(fragment_id)) {
		reset_فعال_State();
		بیرون_len = sequence_change_پیام_len;
		أرجع sequence_change_پیام;
	}

	Fragment_جایگاه &fragment = _fragments[fragment_id];

	إذا (fragment.present && ((fragment.len != len) || (memcmp(fragment.داده, داده, len) != 0))) {
		// Reusing a slot with different bytes means the current buffer no longer
		// matches the incoming fragments. Drop the old partial state and treat
		// this packet as the start of a new buffer for the same sequence ID.
		reset_فعال_State();
		فعال_sequence.فعال_2 = درست;
		فعال_sequence.sequence_id = sequence_id;
	}

	کپی_حافظه(fragment.داده, داده, len);
	fragment.len = len;
	fragment.present = درست;
	فعال_sequence.timestamp = timestamp;

	// The buffer is complete when either all 4 fragments are present, or when
	// the first fragment with a non-full payload has been received and every
	// lower fragment ID is already present. Recording fragment 3 as the
	// boundary covers the "all 4 fragments" case. A non-full fragment 0 also
	// completes immediately, although a compliant sender would normally send
	// that packet as unfragmented instead of setting the fragmented bit.
	إذا (is_non_پر_fragment || (fragment_id == GPS_RTCM_MAX_FRAGMENTS - 1)) {
		فعال_sequence.آخرین_fragment_id = fragment_id;
	}

	إذا (!isComplete()) {
		بیرون_len = sequence_change_پیام_len;
		أرجع sequence_change_پیام;
	}

	size_t assembled_len = 0;
	uint8_t *destination = (sequence_change_پیام != اشاره_تهی) ? deferred_پیام : assembled_پیام;

	لكل (size_t i = 0; i <= آخرین_Fragment_شاخص(); i++) {
		ثابت Fragment_جایگاه &جایگاه = _fragments[i];
		کپی_حافظه(&destination[assembled_len], جایگاه.داده, جایگاه.len);
		assembled_len += جایگاه.len;
	}

	reset_فعال_State();

	إذا (sequence_change_پیام != اشاره_تهی) {
		deferred_پیام_len = assembled_len;
		deferred_پیام_valid = درست;
		بیرون_len = sequence_change_پیام_len;
		أرجع sequence_change_پیام;
	}

	بیرون_len = assembled_len;
	أرجع assembled_پیام;
}

ثابت uint8_t *Gps_Rtcm_پیام_Assembler::take_Deferred_پیام(size_t &بیرون_len)
{
	بیرون_len = 0;

	إذا (!deferred_پیام_valid) {
		أرجع اشاره_تهی;
	}

	deferred_پیام_valid = خطأ;
	بیرون_len = deferred_پیام_len;
	أرجع deferred_پیام;
}

فراغ Gps_Rtcm_پیام_Assembler::reset()
{
	reset_فعال_State();
	deferred_پیام_len = 0;
	deferred_پیام_valid = خطأ;
}

فراغ Gps_Rtcm_پیام_Assembler::reset_فعال_State()
{
	لكل (Fragment_جایگاه &fragment : _fragments) {
		fragment.present = خطأ;
		fragment.len = 0;
	}

	فعال_sequence = {};
}

منطقي Gps_Rtcm_پیام_Assembler::hasFragmentAfter(uint8_t fragment_id) ثابت
{
	لكل (size_t i = fragment_id + 1; i < GPS_RTCM_MAX_FRAGMENTS; i++) {
		إذا (_fragments[i].present) {
			أرجع درست;
		}
	}

	أرجع خطأ;
}

size_t Gps_Rtcm_پیام_Assembler::assemble_Legacy_Exact_مضرب_پیام(uint8_t *destination) ثابت
{
	size_t پر_fragments = 0;

	// Extension to MAVLink rule: accept legacy senders that omit the required
	// zero-length terminator for exact multiples of 180 bytes, but only when
	// every buffered fragment forms a gap-free full-size prefix starting at
	// fragment 0.
	لكل (; پر_fragments < GPS_RTCM_MAX_FRAGMENTS; پر_fragments++) {
		إذا (!_fragments[پر_fragments].present) {
			اكسر;
		}

		إذا (_fragments[پر_fragments].len != GPS_RTCM_MAX_FRAGMENT_LEN) {
			اكسر;
		}
	}

	// Reject buffers like [180, _, 180]: only flush when every buffered
	// fragment belongs to one contiguous full-size run starting at fragment 0.
	إذا ((پر_fragments == 0) || hasFragmentAfter(پر_fragments - 1)) {
		أرجع 0;
	}

	size_t assembled_len = 0;

	لكل (size_t i = 0; i < پر_fragments; i++) {
		ثابت Fragment_جایگاه &جایگاه = _fragments[i];
		کپی_حافظه(&destination[assembled_len], جایگاه.داده, جایگاه.len);
		assembled_len += جایگاه.len;
	}

	أرجع assembled_len;
}

منطقي Gps_Rtcm_پیام_Assembler::isComplete() ثابت
{
	لكل (size_t i = 0; i <= آخرین_Fragment_شاخص(); i++) {
		إذا (!_fragments[i].present) {
			أرجع خطأ;
		}
	}

	أرجع درست;
}

size_t Gps_Rtcm_پیام_Assembler::آخرین_Fragment_شاخص() ثابت
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
	أرجع (فعال_sequence.آخرین_fragment_id >= 0) ? static_cast<size_t>(فعال_sequence.آخرین_fragment_id) :
	       (GPS_RTCM_MAX_FRAGMENTS - 1);
}

} // namespace gnss
