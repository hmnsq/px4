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

#अन्तर्भाव "ConflictTracker.h"

#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <px4_platform_common/log.h>

नामाकाश
{

शून्य append_change(conflict_tracker_changes_s &changes, ध्रुव conflict_tracker_change_s &change)
{
	यदि (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

शून्य record_ignored(conflict_tracker_changes_s &changes, ध्रुव conflict_info_s &conflict, ध्रुव IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

शून्य record_removed(conflict_tracker_changes_s &changes, ध्रुव conflict_info_s &conflict, ध्रुव RemoveBufferCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

तार्किक ConflictTracker::apply_conflict(ध्रुव conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	यदि (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		प्रत्यागम असत्य;
	}

	ध्रुव पूर्णाङ्क conflict_idx = find_conflict_idx(conflict.encoded_id);

	यदि (conflict_idx < 0) {
		यदि (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			प्रत्यागम असत्य;
		}

		प्रत्यागम add_conflict(conflict, changes);
	}

	प्रत्यागम update_conflict(conflict, conflict_idx, changes);
}

तार्किक ConflictTracker::add_conflict(ध्रुव conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	यदि (_buffer.size() < _buffer.max_size()) {
		_buffer.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		प्रत्यागम सत्य;
	}

	ध्रुव पूर्णाङ्क least_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kLeastUrgent);

	यदि (!is_conflict_more_important(conflict, _buffer[least_urgent_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		record_ignored(changes, conflict, IgnoreTrafficCause::kBufferFull);
		प्रत्यागम असत्य;
	}

	ध्रुव conflict_info_s removed_conflict = _buffer[least_urgent_conflict_idx];
	_buffer[least_urgent_conflict_idx] = conflict;
	record_removed(changes, removed_conflict, RemoveBufferCause::kBufferFull);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	प्रत्यागम सत्य;
}

तार्किक ConflictTracker::update_conflict(ध्रुव conflict_info_s &conflict, ध्रुव पूर्णाङ्क conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ध्रुव uint8_t previous_conflict_level = _buffer[conflict_idx].conflict_level;

	यदि (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		_buffer.हटाओ(conflict_idx);

	} अन्यथा {
		_buffer[conflict_idx] = conflict;
	}

	यदि (conflict.conflict_level != previous_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictLevelChanged;
		change.conflict = conflict;
		change.previous_level = previous_conflict_level;

		ध्रुव पूर्णाङ्क most_urgent_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);
		change.conflict_is_most_urgent = (_most_urgent.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_urgent_idx)
						     && _buffer[most_urgent_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	प्रत्यागम सत्य;
}

तार्किक ConflictTracker::remove_stale_conflicts(ध्रुव hrt_abstime now, ध्रुव hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	पूर्णाङ्क nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	प्रति (पूर्णाङ्क idx = (static_cast<पूर्णाङ्क>(_buffer.size()) - 1); idx >= 0; --idx) {
		ध्रुव hrt_abstime latest_update = _buffer[idx].latest_update_timestamp;

		यदि (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ध्रुव conflict_info_s removed_conflict = _buffer[idx];
			_buffer.हटाओ(idx);
			record_removed(changes, removed_conflict, RemoveBufferCause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	यदि (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	प्रत्यागम nb_conflicts_removed > 0;
}

शून्य ConflictTracker::clear()
{
	_buffer.clear();
	reset_most_urgent();
}

शून्य ConflictTracker::reset_most_urgent()
{
	स्थिर constexpr भिन्न kNoConflictDistance{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	_most_urgent = {};
	_most_urgent.aircraft_dist = kNoConflictDistance;
}

शून्य ConflictTracker::refresh_most_urgent()
{
	यदि (_buffer.empty()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_urgent();
		प्रत्यागम;
	}

	_most_urgent = find_most_urgent();
}

conflict_info_s ConflictTracker::find_most_urgent() ध्रुव
{
	ध्रुव पूर्णाङ्क most_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);

	यदि (!is_valid_idx(most_urgent_conflict_idx)) {
		प्रत्यागम {};
	}

	प्रत्यागम _buffer[most_urgent_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ध्रुव DaaEncodedId &encoded_id) ध्रुव
{
	ध्रुव पूर्णाङ्क conflict_idx = find_conflict_idx(encoded_id);

	यदि (!is_valid_idx(conflict_idx)) {
		प्रत्यागम {};
	}

	प्रत्यागम _buffer[conflict_idx];
}

पूर्णाङ्क ConflictTracker::find_conflict_idx_by_priority(ध्रुव ConflictPriority priority) ध्रुव
{
	ध्रुव पूर्णाङ्क buff_size = static_cast<पूर्णाङ्क>(_buffer.size());

	यदि (buff_size < 1) {
		प्रत्यागम -1;
	}

	पूर्णाङ्क best_idx = 0;

	प्रति (पूर्णाङ्क i = 1; i < buff_size; ++i) {
		ध्रुव तार्किक is_better = (priority == ConflictPriority::kMostUrgent)
				       ? is_conflict_more_important(_buffer[i], _buffer[best_idx])
				       : is_conflict_less_important(_buffer[i], _buffer[best_idx]);

		यदि (is_better) {
			best_idx = i;
		}
	}

	प्रत्यागम best_idx;
}

पूर्णाङ्क ConflictTracker::find_conflict_idx(ध्रुव DaaEncodedId &encoded_id) ध्रुव
{
	प्रति (माप_प्रकार i = 0; i < _buffer.size(); i++) {
		यदि (_buffer[i].encoded_id == encoded_id) {
			प्रत्यागम static_cast<पूर्णाङ्क>(i);
		}
	}

	प्रत्यागम -1;
}

तार्किक ConflictTracker::is_conflict_less_important(ध्रुव conflict_info_s &new_conflict,
		ध्रुव conflict_info_s &base_conflict)
{
	यदि (new_conflict.conflict_level != base_conflict.conflict_level) {
		प्रत्यागम new_conflict.conflict_level < base_conflict.conflict_level;
	}

	प्रत्यागम new_conflict.aircraft_dist > base_conflict.aircraft_dist;
}

तार्किक ConflictTracker::is_conflict_more_important(ध्रुव conflict_info_s &new_conflict,
		ध्रुव conflict_info_s &base_conflict)
{
	यदि (new_conflict.conflict_level != base_conflict.conflict_level) {
		प्रत्यागम new_conflict.conflict_level > base_conflict.conflict_level;
	}

	प्रत्यागम new_conflict.aircraft_dist < base_conflict.aircraft_dist;
}
