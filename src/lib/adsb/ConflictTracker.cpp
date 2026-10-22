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

#அந்தர்பாவ "ConflictTracker.h"

#அந்தர்பாவ <px4_platform_common/defines.h>
#அந்தர்பாவ <px4_platform_common/log.h>

நாமாகாஶ
{

வெறுமை append_change(conflict_tracker_changes_s &changes, மாறிலி conflict_tracker_change_s &change)
{
	எனில் (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

வெறுமை record_ignored(conflict_tracker_changes_s &changes, மாறிலி conflict_info_s &conflict, மாறிலி IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

வெறுமை record_removed(conflict_tracker_changes_s &changes, மாறிலி conflict_info_s &conflict, மாறிலி RemoveBufferCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

தருக்க ConflictTracker::apply_conflict(மாறிலி conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	எனில் (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		திரும்பு பொய்;
	}

	மாறிலி முழு conflict_idx = find_conflict_idx(conflict.encoded_id);

	எனில் (conflict_idx < 0) {
		எனில் (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			திரும்பு பொய்;
		}

		திரும்பு add_conflict(conflict, changes);
	}

	திரும்பு update_conflict(conflict, conflict_idx, changes);
}

தருக்க ConflictTracker::add_conflict(மாறிலி conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	எனில் (_buffer.size() < _buffer.max_size()) {
		_buffer.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		திரும்பு உண்மை;
	}

	மாறிலி முழு least_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kLeastUrgent);

	எனில் (!is_conflict_more_important(conflict, _buffer[least_urgent_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		record_ignored(changes, conflict, IgnoreTrafficCause::kBufferFull);
		திரும்பு பொய்;
	}

	மாறிலி conflict_info_s removed_conflict = _buffer[least_urgent_conflict_idx];
	_buffer[least_urgent_conflict_idx] = conflict;
	record_removed(changes, removed_conflict, RemoveBufferCause::kBufferFull);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	திரும்பு உண்மை;
}

தருக்க ConflictTracker::update_conflict(மாறிலி conflict_info_s &conflict, மாறிலி முழு conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	மாறிலி uint8_t previous_conflict_level = _buffer[conflict_idx].conflict_level;

	எனில் (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		_buffer.remove(conflict_idx);

	} இல்லையெனில் {
		_buffer[conflict_idx] = conflict;
	}

	எனில் (conflict.conflict_level != previous_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictLevelChanged;
		change.conflict = conflict;
		change.previous_level = previous_conflict_level;

		மாறிலி முழு most_urgent_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);
		change.conflict_is_most_urgent = (_most_urgent.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_urgent_idx)
						     && _buffer[most_urgent_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	திரும்பு உண்மை;
}

தருக்க ConflictTracker::remove_stale_conflicts(மாறிலி hrt_abstime now, மாறிலி hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	முழு nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	க்கு (முழு idx = (static_cast<முழு>(_buffer.size()) - 1); idx >= 0; --idx) {
		மாறிலி hrt_abstime latest_update = _buffer[idx].latest_update_timestamp;

		எனில் (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			மாறிலி conflict_info_s removed_conflict = _buffer[idx];
			_buffer.remove(idx);
			record_removed(changes, removed_conflict, RemoveBufferCause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	எனில் (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	திரும்பு nb_conflicts_removed > 0;
}

வெறுமை ConflictTracker::clear()
{
	_buffer.clear();
	reset_most_urgent();
}

வெறுமை ConflictTracker::reset_most_urgent()
{
	நிலையான constexpr தசம kNoConflictDistance{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	_most_urgent = {};
	_most_urgent.aircraft_dist = kNoConflictDistance;
}

வெறுமை ConflictTracker::refresh_most_urgent()
{
	எனில் (_buffer.empty()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_urgent();
		திரும்பு;
	}

	_most_urgent = find_most_urgent();
}

conflict_info_s ConflictTracker::find_most_urgent() மாறிலி
{
	மாறிலி முழு most_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);

	எனில் (!is_valid_idx(most_urgent_conflict_idx)) {
		திரும்பு {};
	}

	திரும்பு _buffer[most_urgent_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(மாறிலி DaaEncodedId &encoded_id) மாறிலி
{
	மாறிலி முழு conflict_idx = find_conflict_idx(encoded_id);

	எனில் (!is_valid_idx(conflict_idx)) {
		திரும்பு {};
	}

	திரும்பு _buffer[conflict_idx];
}

முழு ConflictTracker::find_conflict_idx_by_priority(மாறிலி ConflictPriority priority) மாறிலி
{
	மாறிலி முழு buff_size = static_cast<முழு>(_buffer.size());

	எனில் (buff_size < 1) {
		திரும்பு -1;
	}

	முழு best_idx = 0;

	க்கு (முழு i = 1; i < buff_size; ++i) {
		மாறிலி தருக்க is_better = (priority == ConflictPriority::kMostUrgent)
				       ? is_conflict_more_important(_buffer[i], _buffer[best_idx])
				       : is_conflict_less_important(_buffer[i], _buffer[best_idx]);

		எனில் (is_better) {
			best_idx = i;
		}
	}

	திரும்பு best_idx;
}

முழு ConflictTracker::find_conflict_idx(மாறிலி DaaEncodedId &encoded_id) மாறிலி
{
	க்கு (size_t i = 0; i < _buffer.size(); i++) {
		எனில் (_buffer[i].encoded_id == encoded_id) {
			திரும்பு static_cast<முழு>(i);
		}
	}

	திரும்பு -1;
}

தருக்க ConflictTracker::is_conflict_less_important(மாறிலி conflict_info_s &new_conflict,
		மாறிலி conflict_info_s &base_conflict)
{
	எனில் (new_conflict.conflict_level != base_conflict.conflict_level) {
		திரும்பு new_conflict.conflict_level < base_conflict.conflict_level;
	}

	திரும்பு new_conflict.aircraft_dist > base_conflict.aircraft_dist;
}

தருக்க ConflictTracker::is_conflict_more_important(மாறிலி conflict_info_s &new_conflict,
		மாறிலி conflict_info_s &base_conflict)
{
	எனில் (new_conflict.conflict_level != base_conflict.conflict_level) {
		திரும்பு new_conflict.conflict_level > base_conflict.conflict_level;
	}

	திரும்பு new_conflict.aircraft_dist < base_conflict.aircraft_dist;
}
