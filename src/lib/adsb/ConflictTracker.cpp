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

#ଅନ୍ତର୍ଭାଵ "ConflictTracker.h"

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/defines.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/log.h>

ନାମାକାଶ
{

ଶୂନ୍ୟ append_change(conflict_tracker_changes_s &changes, ଅଚଳ conflict_tracker_change_s &change)
{
	ଯଦି (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

ଶୂନ୍ୟ record_ignored(conflict_tracker_changes_s &changes, ଅଚଳ conflict_info_s &conflict, ଅଚଳ IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

ଶୂନ୍ୟ record_removed(conflict_tracker_changes_s &changes, ଅଚଳ conflict_info_s &conflict, ଅଚଳ RemoveBufferCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

ତାର୍କିକ ConflictTracker::apply_conflict(ଅଚଳ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ଯଦି (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା conflict_idx = find_conflict_idx(conflict.encoded_id);

	ଯଦି (conflict_idx < 0) {
		ଯଦି (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}

		ଫେରନ୍ତୁ add_conflict(conflict, changes);
	}

	ଫେରନ୍ତୁ update_conflict(conflict, conflict_idx, changes);
}

ତାର୍କିକ ConflictTracker::add_conflict(ଅଚଳ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ଯଦି (_buffer.size() < _buffer.max_size()) {
		_buffer.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା least_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kLeastUrgent);

	ଯଦି (!is_conflict_more_important(conflict, _buffer[least_urgent_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		record_ignored(changes, conflict, IgnoreTrafficCause::kBufferFull);
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଅଚଳ conflict_info_s removed_conflict = _buffer[least_urgent_conflict_idx];
	_buffer[least_urgent_conflict_idx] = conflict;
	record_removed(changes, removed_conflict, RemoveBufferCause::kBufferFull);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ ConflictTracker::update_conflict(ଅଚଳ conflict_info_s &conflict, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ଅଚଳ uint8_t previous_conflict_level = _buffer[conflict_idx].conflict_level;

	ଯଦି (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		_buffer.remove(conflict_idx);

	} ଅନ୍ୟଥା {
		_buffer[conflict_idx] = conflict;
	}

	ଯଦି (conflict.conflict_level != previous_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictLevelChanged;
		change.conflict = conflict;
		change.previous_level = previous_conflict_level;

		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା most_urgent_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);
		change.conflict_is_most_urgent = (_most_urgent.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_urgent_idx)
						     && _buffer[most_urgent_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ ConflictTracker::remove_stale_conflicts(ଅଚଳ hrt_abstime now, ଅଚଳ hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = (static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(_buffer.size()) - 1); idx >= 0; --idx) {
		ଅଚଳ hrt_abstime latest_update = _buffer[idx].latest_update_timestamp;

		ଯଦି (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ଅଚଳ conflict_info_s removed_conflict = _buffer[idx];
			_buffer.remove(idx);
			record_removed(changes, removed_conflict, RemoveBufferCause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	ଯଦି (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	ଫେରନ୍ତୁ nb_conflicts_removed > 0;
}

ଶୂନ୍ୟ ConflictTracker::clear()
{
	_buffer.clear();
	reset_most_urgent();
}

ଶୂନ୍ୟ ConflictTracker::reset_most_urgent()
{
	ସ୍ଥିର constexpr ଦଶମିକ kNoConflictDistance{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	_most_urgent = {};
	_most_urgent.aircraft_dist = kNoConflictDistance;
}

ଶୂନ୍ୟ ConflictTracker::refresh_most_urgent()
{
	ଯଦି (_buffer.empty()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_urgent();
		ଫେରନ୍ତୁ;
	}

	_most_urgent = find_most_urgent();
}

conflict_info_s ConflictTracker::find_most_urgent() ଅଚଳ
{
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା most_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);

	ଯଦି (!is_valid_idx(most_urgent_conflict_idx)) {
		ଫେରନ୍ତୁ {};
	}

	ଫେରନ୍ତୁ _buffer[most_urgent_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ଅଚଳ DaaEncodedId &encoded_id) ଅଚଳ
{
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା conflict_idx = find_conflict_idx(encoded_id);

	ଯଦି (!is_valid_idx(conflict_idx)) {
		ଫେରନ୍ତୁ {};
	}

	ଫେରନ୍ତୁ _buffer[conflict_idx];
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ConflictTracker::find_conflict_idx_by_priority(ଅଚଳ ConflictPriority priority) ଅଚଳ
{
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା buff_size = static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(_buffer.size());

	ଯଦି (buff_size < 1) {
		ଫେରନ୍ତୁ -1;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା best_idx = 0;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < buff_size; ++i) {
		ଅଚଳ ତାର୍କିକ is_better = (priority == ConflictPriority::kMostUrgent)
				       ? is_conflict_more_important(_buffer[i], _buffer[best_idx])
				       : is_conflict_less_important(_buffer[i], _buffer[best_idx]);

		ଯଦି (is_better) {
			best_idx = i;
		}
	}

	ଫେରନ୍ତୁ best_idx;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ConflictTracker::find_conflict_idx(ଅଚଳ DaaEncodedId &encoded_id) ଅଚଳ
{
	ପାଇଁ (size_t i = 0; i < _buffer.size(); i++) {
		ଯଦି (_buffer[i].encoded_id == encoded_id) {
			ଫେରନ୍ତୁ static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(i);
		}
	}

	ଫେରନ୍ତୁ -1;
}

ତାର୍କିକ ConflictTracker::is_conflict_less_important(ଅଚଳ conflict_info_s &new_conflict,
		ଅଚଳ conflict_info_s &base_conflict)
{
	ଯଦି (new_conflict.conflict_level != base_conflict.conflict_level) {
		ଫେରନ୍ତୁ new_conflict.conflict_level < base_conflict.conflict_level;
	}

	ଫେରନ୍ତୁ new_conflict.aircraft_dist > base_conflict.aircraft_dist;
}

ତାର୍କିକ ConflictTracker::is_conflict_more_important(ଅଚଳ conflict_info_s &new_conflict,
		ଅଚଳ conflict_info_s &base_conflict)
{
	ଯଦି (new_conflict.conflict_level != base_conflict.conflict_level) {
		ଫେରନ୍ତୁ new_conflict.conflict_level > base_conflict.conflict_level;
	}

	ଫେରନ୍ତୁ new_conflict.aircraft_dist < base_conflict.aircraft_dist;
}
