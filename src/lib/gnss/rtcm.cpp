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

uint32_t rtcm3_crc24q(ध्रुव uint8_t *दत्त, size_t len)
{
	uint32_t crc = 0;

	प्रति (size_t i = 0; i < len; i++) {
		crc ^= static_cast<uint32_t>(दत्त[i]) << 16;

		प्रति (पूर्णाङ्क j = 0; j < 8; j++) {
			crc <<= 1;

			यदि (crc & 0x1000000) {
				crc ^= RTCM3_CRC24Q_POLY;
			}
		}
	}

	प्रत्यागम crc & 0xFFFFFF;
}

ध्रुव uint8_t *Gps_Rtcm_सन्देशः_Assembler::सङ्कलय_Packet(uint8_t flags, ध्रुव uint8_t *दत्त, size_t len, uint64_t timestamp,
		size_t &बहिर्गमनम्_len)
{
	बहिर्गमनम्_len = 0;

	यदि (दत्त == शून्यसूचकः) {
		reset();
		प्रत्यागम शून्यसूचकः;
	}

	// Timeout, clear stale partial state.
	यदि (सक्रिय_sequence.सक्रिय_2 && (सक्रिय_sequence.timestamp != 0)
	    && (timestamp > सक्रिय_sequence.timestamp + GPS_RTCM_FRAGMENT_TIMEOUT_US)) {
		reset_सक्रिय_State();
	}

	यदि (!gps_rtcm_is_fragmented(flags)) {
		// Compatibility fallback for older QGroundControl builds
		// that omit the final zero-length fragment
		ध्रुव size_t legacy_सन्देशः_len = assemble_Legacy_Exact_गुणजः_सन्देशः(assembled_सन्देशः);

		// New fragment is invalid, return previous message if valid
		यदि (len > परिमाण(assembled_सन्देशः)) {
			reset();
			बहिर्गमनम्_len = legacy_सन्देशः_len;
			प्रत्यागम (legacy_सन्देशः_len > 0) ? assembled_सन्देशः : शून्यसूचकः;
		}

		यदि (legacy_सन्देशः_len > 0) {
			// Return the older buffered message first. Queue this new
			// unfragmented packet as deferred output.
			स्मृतिं_प्रतिलिख(deferred_सन्देशः, दत्त, len);
			deferred_सन्देशः_len = len;
			deferred_सन्देशः_valid = सत्य;
			reset_सक्रिय_State();
			बहिर्गमनम्_len = legacy_सन्देशः_len;
			प्रत्यागम assembled_सन्देशः;
		}

		reset();
		स्मृतिं_प्रतिलिख(assembled_सन्देशः, दत्त, len);
		बहिर्गमनम्_len = len;

		प्रत्यागम assembled_सन्देशः;
	}

	यदि (len > GPS_RTCM_MAX_FRAGMENT_LEN) {
		reset_सक्रिय_State();
		प्रत्यागम शून्यसूचकः;
	}

	ध्रुव uint8_t sequence_id = gps_rtcm_sequence_id(flags);
	ध्रुव uint8_t fragment_id = gps_rtcm_fragment_id(flags);
	ध्रुव तार्किक is_non_पूर्णम्_fragment = len < GPS_RTCM_MAX_FRAGMENT_LEN;
	ध्रुव uint8_t *sequence_change_सन्देशः = शून्यसूचकः;
	size_t sequence_change_सन्देशः_len = 0;

	यदि (!सक्रिय_sequence.सक्रिय_2 || sequence_id != सक्रिय_sequence.sequence_id) {
		यदि (सक्रिय_sequence.सक्रिय_2) {
			// Compatibility fallback for older QGroundControl builds
			// that omit the final zero-length fragment
			sequence_change_सन्देशः_len = assemble_Legacy_Exact_गुणजः_सन्देशः(assembled_सन्देशः);

			यदि (sequence_change_सन्देशः_len > 0) {
				// Do not return here: this same packet also starts the new
				// sequence, so it still needs to be validated and buffered.
				sequence_change_सन्देशः = assembled_सन्देशः;
			}
		}

		reset_सक्रिय_State();
		सक्रिय_sequence.सक्रिय_2 = सत्य;
		सक्रिय_sequence.sequence_id = sequence_id;
	}

	ध्रुव तार्किक चरमः_fragment_ज्ञातम् = (सक्रिय_sequence.चरमः_fragment_id >= 0);

	// Once a non-full final fragment is known, any later fragment index is invalid.
	यदि (चरमः_fragment_ज्ञातम् && (fragment_id > सक्रिय_sequence.चरमः_fragment_id)) {
		reset_सक्रिय_State();
		बहिर्गमनम्_len = sequence_change_सन्देशः_len;
		प्रत्यागम sequence_change_सन्देशः;
	}

	// A non-full fragment before the known end would contradict the current
	// end-of-message boundary. That can only come from malformed sender state.
	यदि (चरमः_fragment_ज्ञातम् && (fragment_id < सक्रिय_sequence.चरमः_fragment_id) && is_non_पूर्णम्_fragment) {
		reset_सक्रिय_State();
		बहिर्गमनम्_len = sequence_change_सन्देशः_len;
		प्रत्यागम sequence_change_सन्देशः;
	}

	// The first fragment with a non-full payload defines the completion
	// boundary. A higher buffered fragment would contradict that boundary.
	यदि (is_non_पूर्णम्_fragment && hasFragmentAfter(fragment_id)) {
		reset_सक्रिय_State();
		बहिर्गमनम्_len = sequence_change_सन्देशः_len;
		प्रत्यागम sequence_change_सन्देशः;
	}

	FragmentSlot &fragment = _fragments[fragment_id];

	यदि (fragment.present && ((fragment.len != len) || (memcmp(fragment.दत्त, दत्त, len) != 0))) {
		// Reusing a slot with different bytes means the current buffer no longer
		// matches the incoming fragments. Drop the old partial state and treat
		// this packet as the start of a new buffer for the same sequence ID.
		reset_सक्रिय_State();
		सक्रिय_sequence.सक्रिय_2 = सत्य;
		सक्रिय_sequence.sequence_id = sequence_id;
	}

	स्मृतिं_प्रतिलिख(fragment.दत्त, दत्त, len);
	fragment.len = len;
	fragment.present = सत्य;
	सक्रिय_sequence.timestamp = timestamp;

	// The buffer is complete when either all 4 fragments are present, or when
	// the first fragment with a non-full payload has been received and every
	// lower fragment ID is already present. Recording fragment 3 as the
	// boundary covers the "all 4 fragments" case. A non-full fragment 0 also
	// completes immediately, although a compliant sender would normally send
	// that packet as unfragmented instead of setting the fragmented bit.
	यदि (is_non_पूर्णम्_fragment || (fragment_id == GPS_RTCM_MAX_FRAGMENTS - 1)) {
		सक्रिय_sequence.चरमः_fragment_id = fragment_id;
	}

	यदि (!isComplete()) {
		बहिर्गमनम्_len = sequence_change_सन्देशः_len;
		प्रत्यागम sequence_change_सन्देशः;
	}

	size_t assembled_len = 0;
	uint8_t *destination = (sequence_change_सन्देशः != शून्यसूचकः) ? deferred_सन्देशः : assembled_सन्देशः;

	प्रति (size_t i = 0; i <= चरमः_Fragment_सूचकांक(); i++) {
		ध्रुव FragmentSlot &slot = _fragments[i];
		स्मृतिं_प्रतिलिख(&destination[assembled_len], slot.दत्त, slot.len);
		assembled_len += slot.len;
	}

	reset_सक्रिय_State();

	यदि (sequence_change_सन्देशः != शून्यसूचकः) {
		deferred_सन्देशः_len = assembled_len;
		deferred_सन्देशः_valid = सत्य;
		बहिर्गमनम्_len = sequence_change_सन्देशः_len;
		प्रत्यागम sequence_change_सन्देशः;
	}

	बहिर्गमनम्_len = assembled_len;
	प्रत्यागम assembled_सन्देशः;
}

ध्रुव uint8_t *Gps_Rtcm_सन्देशः_Assembler::take_Deferred_सन्देशः(size_t &बहिर्गमनम्_len)
{
	बहिर्गमनम्_len = 0;

	यदि (!deferred_सन्देशः_valid) {
		प्रत्यागम शून्यसूचकः;
	}

	deferred_सन्देशः_valid = असत्य;
	बहिर्गमनम्_len = deferred_सन्देशः_len;
	प्रत्यागम deferred_सन्देशः;
}

शून्य Gps_Rtcm_सन्देशः_Assembler::reset()
{
	reset_सक्रिय_State();
	deferred_सन्देशः_len = 0;
	deferred_सन्देशः_valid = असत्य;
}

शून्य Gps_Rtcm_सन्देशः_Assembler::reset_सक्रिय_State()
{
	प्रति (FragmentSlot &fragment : _fragments) {
		fragment.present = असत्य;
		fragment.len = 0;
	}

	सक्रिय_sequence = {};
}

तार्किक Gps_Rtcm_सन्देशः_Assembler::hasFragmentAfter(uint8_t fragment_id) ध्रुव
{
	प्रति (size_t i = fragment_id + 1; i < GPS_RTCM_MAX_FRAGMENTS; i++) {
		यदि (_fragments[i].present) {
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

size_t Gps_Rtcm_सन्देशः_Assembler::assemble_Legacy_Exact_गुणजः_सन्देशः(uint8_t *destination) ध्रुव
{
	size_t पूर्णम्_fragments = 0;

	// Extension to MAVLink rule: accept legacy senders that omit the required
	// zero-length terminator for exact multiples of 180 bytes, but only when
	// every buffered fragment forms a gap-free full-size prefix starting at
	// fragment 0.
	प्रति (; पूर्णम्_fragments < GPS_RTCM_MAX_FRAGMENTS; पूर्णम्_fragments++) {
		यदि (!_fragments[पूर्णम्_fragments].present) {
			भङ्ग;
		}

		यदि (_fragments[पूर्णम्_fragments].len != GPS_RTCM_MAX_FRAGMENT_LEN) {
			भङ्ग;
		}
	}

	// Reject buffers like [180, _, 180]: only flush when every buffered
	// fragment belongs to one contiguous full-size run starting at fragment 0.
	यदि ((पूर्णम्_fragments == 0) || hasFragmentAfter(पूर्णम्_fragments - 1)) {
		प्रत्यागम 0;
	}

	size_t assembled_len = 0;

	प्रति (size_t i = 0; i < पूर्णम्_fragments; i++) {
		ध्रुव FragmentSlot &slot = _fragments[i];
		स्मृतिं_प्रतिलिख(&destination[assembled_len], slot.दत्त, slot.len);
		assembled_len += slot.len;
	}

	प्रत्यागम assembled_len;
}

तार्किक Gps_Rtcm_सन्देशः_Assembler::isComplete() ध्रुव
{
	प्रति (size_t i = 0; i <= चरमः_Fragment_सूचकांक(); i++) {
		यदि (!_fragments[i].present) {
			प्रत्यागम असत्य;
		}
	}

	प्रत्यागम सत्य;
}

size_t Gps_Rtcm_सन्देशः_Assembler::चरमः_Fragment_सूचकांक() ध्रुव
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
	प्रत्यागम (सक्रिय_sequence.चरमः_fragment_id >= 0) ? static_cast<size_t>(सक्रिय_sequence.चरमः_fragment_id) :
	       (GPS_RTCM_MAX_FRAGMENTS - 1);
}

} // namespace gnss
