/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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
 * @file ConflictTracker.cpp
 *
 * Fixed-size buffer of active DAA conflicts.
 *
 * @author Jonas Perolini <jonspero@me.com>
 */

#ಅನ್ತರ್ಭಾವ "ConflictTracker.h"

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>

ನಾಮಾಕಾಶ
{

ಶೂನ್ಯ append_change(conflict_tracker_changes_s &changes, ಧ್ರುವ conflict_tracker_change_s &change)
{
	ಯದಿ (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

ಶೂನ್ಯ record_ignored(conflict_tracker_changes_s &changes, ಧ್ರುವ conflict_info_s &conflict, ಧ್ರುವ IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

ಶೂನ್ಯ record_removed(conflict_tracker_changes_s &changes, ಧ್ರುವ conflict_info_s &conflict, ಧ್ರುವ RemoveBufferCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

ತಾರ್ಕಿಕ ConflictTracker::apply_conflict(ಧ್ರುವ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ಯದಿ (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ conflict_idx = find_conflict_idx(conflict.encoded_id);

	ಯದಿ (conflict_idx < 0) {
		ಯದಿ (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}

		ಪ್ರತ್ಯಾಗಮ add_conflict(conflict, changes);
	}

	ಪ್ರತ್ಯಾಗಮ update_conflict(conflict, conflict_idx, changes);
}

ತಾರ್ಕಿಕ ConflictTracker::add_conflict(ಧ್ರುವ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ಯದಿ (_buffer.size() < _buffer.max_size()) {
		_buffer.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ least_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kLeastUrgent);

	ಯದಿ (!is_conflict_more_important(conflict, _buffer[least_urgent_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		record_ignored(changes, conflict, IgnoreTrafficCause::kBufferFull);
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಧ್ರುವ conflict_info_s removed_conflict = _buffer[least_urgent_conflict_idx];
	_buffer[least_urgent_conflict_idx] = conflict;
	record_removed(changes, removed_conflict, RemoveBufferCause::kBufferFull);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ ConflictTracker::update_conflict(ಧ್ರುವ conflict_info_s &conflict, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ಧ್ರುವ uint8_t previous_conflict_level = _buffer[conflict_idx].conflict_level;

	ಯದಿ (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		_buffer.remove(conflict_idx);

	} ಅನ್ಯಥಾ {
		_buffer[conflict_idx] = conflict;
	}

	ಯದಿ (conflict.conflict_level != previous_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictLevelChanged;
		change.conflict = conflict;
		change.previous_level = previous_conflict_level;

		ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ most_urgent_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);
		change.conflict_is_most_urgent = (_most_urgent.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_urgent_idx)
						     && _buffer[most_urgent_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ ConflictTracker::remove_stale_conflicts(ಧ್ರುವ hrt_abstime now, ಧ್ರುವ hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	ಪೂರ್ಣಾಙ್ಕ nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = (static_cast<ಪೂರ್ಣಾಙ್ಕ>(_buffer.size()) - 1); idx >= 0; --idx) {
		ಧ್ರುವ hrt_abstime latest_update = _buffer[idx].latest_update_timestamp;

		ಯದಿ (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ಧ್ರುವ conflict_info_s removed_conflict = _buffer[idx];
			_buffer.remove(idx);
			record_removed(changes, removed_conflict, RemoveBufferCause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	ಯದಿ (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	ಪ್ರತ್ಯಾಗಮ nb_conflicts_removed > 0;
}

ಶೂನ್ಯ ConflictTracker::clear()
{
	_buffer.clear();
	reset_most_urgent();
}

ಶೂನ್ಯ ConflictTracker::reset_most_urgent()
{
	ಸ್ಥಿರ constexpr ಭಿನ್ನ kNoConflictDistance{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	_most_urgent = {};
	_most_urgent.aircraft_dist = kNoConflictDistance;
}

ಶೂನ್ಯ ConflictTracker::refresh_most_urgent()
{
	ಯದಿ (_buffer.empty()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_urgent();
		ಪ್ರತ್ಯಾಗಮ;
	}

	_most_urgent = find_most_urgent();
}

conflict_info_s ConflictTracker::find_most_urgent() ಧ್ರುವ
{
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ most_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);

	ಯದಿ (!is_valid_idx(most_urgent_conflict_idx)) {
		ಪ್ರತ್ಯಾಗಮ {};
	}

	ಪ್ರತ್ಯಾಗಮ _buffer[most_urgent_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ಧ್ರುವ DaaEncodedId &encoded_id) ಧ್ರುವ
{
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ conflict_idx = find_conflict_idx(encoded_id);

	ಯದಿ (!is_valid_idx(conflict_idx)) {
		ಪ್ರತ್ಯಾಗಮ {};
	}

	ಪ್ರತ್ಯಾಗಮ _buffer[conflict_idx];
}

ಪೂರ್ಣಾಙ್ಕ ConflictTracker::find_conflict_idx_by_priority(ಧ್ರುವ ConflictPriority priority) ಧ್ರುವ
{
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ buff_size = static_cast<ಪೂರ್ಣಾಙ್ಕ>(_buffer.size());

	ಯದಿ (buff_size < 1) {
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಪೂರ್ಣಾಙ್ಕ best_idx = 0;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < buff_size; ++i) {
		ಧ್ರುವ ತಾರ್ಕಿಕ is_better = (priority == ConflictPriority::kMostUrgent)
				       ? is_conflict_more_important(_buffer[i], _buffer[best_idx])
				       : is_conflict_less_important(_buffer[i], _buffer[best_idx]);

		ಯದಿ (is_better) {
			best_idx = i;
		}
	}

	ಪ್ರತ್ಯಾಗಮ best_idx;
}

ಪೂರ್ಣಾಙ್ಕ ConflictTracker::find_conflict_idx(ಧ್ರುವ DaaEncodedId &encoded_id) ಧ್ರುವ
{
	ಪ್ರತಿ (size_t i = 0; i < _buffer.size(); i++) {
		ಯದಿ (_buffer[i].encoded_id == encoded_id) {
			ಪ್ರತ್ಯಾಗಮ static_cast<ಪೂರ್ಣಾಙ್ಕ>(i);
		}
	}

	ಪ್ರತ್ಯಾಗಮ -1;
}

ತಾರ್ಕಿಕ ConflictTracker::is_conflict_less_important(ಧ್ರುವ conflict_info_s &new_conflict,
		ಧ್ರುವ conflict_info_s &base_conflict)
{
	ಯದಿ (new_conflict.conflict_level != base_conflict.conflict_level) {
		ಪ್ರತ್ಯಾಗಮ new_conflict.conflict_level < base_conflict.conflict_level;
	}

	ಪ್ರತ್ಯಾಗಮ new_conflict.aircraft_dist > base_conflict.aircraft_dist;
}

ತಾರ್ಕಿಕ ConflictTracker::is_conflict_more_important(ಧ್ರುವ conflict_info_s &new_conflict,
		ಧ್ರುವ conflict_info_s &base_conflict)
{
	ಯದಿ (new_conflict.conflict_level != base_conflict.conflict_level) {
		ಪ್ರತ್ಯಾಗಮ new_conflict.conflict_level > base_conflict.conflict_level;
	}

	ಪ್ರತ್ಯಾಗಮ new_conflict.aircraft_dist < base_conflict.aircraft_dist;
}
