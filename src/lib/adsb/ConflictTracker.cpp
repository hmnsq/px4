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

#અન્તર્ભાવ "ConflictTracker.h"

#અન્તર્ભાવ <px4_platform_common/defines.h>
#અન્તર્ભાવ <px4_platform_common/log.h>

નામાકાશ
{

શૂન્ય append_change(conflict_tracker_changes_s &changes, અચલ conflict_tracker_change_s &change)
{
	જો (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

શૂન્ય record_ignored(conflict_tracker_changes_s &changes, અચલ conflict_info_s &conflict, અચલ IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

શૂન્ય record_removed(conflict_tracker_changes_s &changes, અચલ conflict_info_s &conflict, અચલ RemoveBufferCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

તાર્કિક ConflictTracker::apply_conflict(અચલ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	જો (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		પાછા ખોટું;
	}

	અચલ પૂર્ણાંક conflict_idx = find_conflict_idx(conflict.encoded_id);

	જો (conflict_idx < 0) {
		જો (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			પાછા ખોટું;
		}

		પાછા add_conflict(conflict, changes);
	}

	પાછા update_conflict(conflict, conflict_idx, changes);
}

તાર્કિક ConflictTracker::add_conflict(અચલ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	જો (_buffer.size() < _buffer.max_size()) {
		_buffer.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		પાછા સાચું;
	}

	અચલ પૂર્ણાંક least_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kLeastUrgent);

	જો (!is_conflict_more_important(conflict, _buffer[least_urgent_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		record_ignored(changes, conflict, IgnoreTrafficCause::kBufferFull);
		પાછા ખોટું;
	}

	અચલ conflict_info_s removed_conflict = _buffer[least_urgent_conflict_idx];
	_buffer[least_urgent_conflict_idx] = conflict;
	record_removed(changes, removed_conflict, RemoveBufferCause::kBufferFull);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	પાછા સાચું;
}

તાર્કિક ConflictTracker::update_conflict(અચલ conflict_info_s &conflict, અચલ પૂર્ણાંક conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	અચલ uint8_t previous_conflict_level = _buffer[conflict_idx].conflict_level;

	જો (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		_buffer.remove(conflict_idx);

	} નહીંતર {
		_buffer[conflict_idx] = conflict;
	}

	જો (conflict.conflict_level != previous_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictLevelChanged;
		change.conflict = conflict;
		change.previous_level = previous_conflict_level;

		અચલ પૂર્ણાંક most_urgent_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);
		change.conflict_is_most_urgent = (_most_urgent.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_urgent_idx)
						     && _buffer[most_urgent_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	પાછા સાચું;
}

તાર્કિક ConflictTracker::remove_stale_conflicts(અચલ hrt_abstime now, અચલ hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	પૂર્ણાંક nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	માટે (પૂર્ણાંક idx = (static_cast<પૂર્ણાંક>(_buffer.size()) - 1); idx >= 0; --idx) {
		અચલ hrt_abstime latest_update = _buffer[idx].latest_update_timestamp;

		જો (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			અચલ conflict_info_s removed_conflict = _buffer[idx];
			_buffer.remove(idx);
			record_removed(changes, removed_conflict, RemoveBufferCause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	જો (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	પાછા nb_conflicts_removed > 0;
}

શૂન્ય ConflictTracker::clear()
{
	_buffer.clear();
	reset_most_urgent();
}

શૂન્ય ConflictTracker::reset_most_urgent()
{
	સ્થિર constexpr દશાંશ kNoConflictDistance{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	_most_urgent = {};
	_most_urgent.aircraft_dist = kNoConflictDistance;
}

શૂન્ય ConflictTracker::refresh_most_urgent()
{
	જો (_buffer.empty()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_urgent();
		પાછા;
	}

	_most_urgent = find_most_urgent();
}

conflict_info_s ConflictTracker::find_most_urgent() અચલ
{
	અચલ પૂર્ણાંક most_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);

	જો (!is_valid_idx(most_urgent_conflict_idx)) {
		પાછા {};
	}

	પાછા _buffer[most_urgent_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(અચલ DaaEncodedId &encoded_id) અચલ
{
	અચલ પૂર્ણાંક conflict_idx = find_conflict_idx(encoded_id);

	જો (!is_valid_idx(conflict_idx)) {
		પાછા {};
	}

	પાછા _buffer[conflict_idx];
}

પૂર્ણાંક ConflictTracker::find_conflict_idx_by_priority(અચલ ConflictPriority priority) અચલ
{
	અચલ પૂર્ણાંક buff_size = static_cast<પૂર્ણાંક>(_buffer.size());

	જો (buff_size < 1) {
		પાછા -1;
	}

	પૂર્ણાંક best_idx = 0;

	માટે (પૂર્ણાંક i = 1; i < buff_size; ++i) {
		અચલ તાર્કિક is_better = (priority == ConflictPriority::kMostUrgent)
				       ? is_conflict_more_important(_buffer[i], _buffer[best_idx])
				       : is_conflict_less_important(_buffer[i], _buffer[best_idx]);

		જો (is_better) {
			best_idx = i;
		}
	}

	પાછા best_idx;
}

પૂર્ણાંક ConflictTracker::find_conflict_idx(અચલ DaaEncodedId &encoded_id) અચલ
{
	માટે (size_t i = 0; i < _buffer.size(); i++) {
		જો (_buffer[i].encoded_id == encoded_id) {
			પાછા static_cast<પૂર્ણાંક>(i);
		}
	}

	પાછા -1;
}

તાર્કિક ConflictTracker::is_conflict_less_important(અચલ conflict_info_s &new_conflict,
		અચલ conflict_info_s &base_conflict)
{
	જો (new_conflict.conflict_level != base_conflict.conflict_level) {
		પાછા new_conflict.conflict_level < base_conflict.conflict_level;
	}

	પાછા new_conflict.aircraft_dist > base_conflict.aircraft_dist;
}

તાર્કિક ConflictTracker::is_conflict_more_important(અચલ conflict_info_s &new_conflict,
		અચલ conflict_info_s &base_conflict)
{
	જો (new_conflict.conflict_level != base_conflict.conflict_level) {
		પાછા new_conflict.conflict_level > base_conflict.conflict_level;
	}

	પાછા new_conflict.aircraft_dist < base_conflict.aircraft_dist;
}
