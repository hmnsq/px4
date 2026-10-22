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

#অন্তর্ভাৱ "ConflictTracker.h"

#অন্তর্ভাৱ <px4_platform_common/defines.h>
#অন্তর্ভাৱ <px4_platform_common/log.h>

নামাকাশ
{

শূন্য append_change(conflict_tracker_changes_s &changes, ধ্রুৱ conflict_tracker_change_s &change)
{
	যদি (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

শূন্য record_ignored(conflict_tracker_changes_s &changes, ধ্রুৱ conflict_info_s &conflict, ধ্রুৱ IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

শূন্য record_removed(conflict_tracker_changes_s &changes, ধ্রুৱ conflict_info_s &conflict, ধ্রুৱ RemoveBufferCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

তার্কিক ConflictTracker::apply_conflict(ধ্রুৱ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	যদি (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		ঘূৰাই মিছা;
	}

	ধ্রুৱ পূর্ণাঙ্ক conflict_idx = find_conflict_idx(conflict.encoded_id);

	যদি (conflict_idx < 0) {
		যদি (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			ঘূৰাই মিছা;
		}

		ঘূৰাই add_conflict(conflict, changes);
	}

	ঘূৰাই update_conflict(conflict, conflict_idx, changes);
}

তার্কিক ConflictTracker::add_conflict(ধ্রুৱ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	যদি (_buffer.size() < _buffer.max_size()) {
		_buffer.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		ঘূৰাই সঁচা;
	}

	ধ্রুৱ পূর্ণাঙ্ক least_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kLeastUrgent);

	যদি (!is_conflict_more_important(conflict, _buffer[least_urgent_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		record_ignored(changes, conflict, IgnoreTrafficCause::kBufferFull);
		ঘূৰাই মিছা;
	}

	ধ্রুৱ conflict_info_s removed_conflict = _buffer[least_urgent_conflict_idx];
	_buffer[least_urgent_conflict_idx] = conflict;
	record_removed(changes, removed_conflict, RemoveBufferCause::kBufferFull);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	ঘূৰাই সঁচা;
}

তার্কিক ConflictTracker::update_conflict(ধ্রুৱ conflict_info_s &conflict, ধ্রুৱ পূর্ণাঙ্ক conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ধ্রুৱ uint8_t previous_conflict_level = _buffer[conflict_idx].conflict_level;

	যদি (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		_buffer.remove(conflict_idx);

	} অন্যথা {
		_buffer[conflict_idx] = conflict;
	}

	যদি (conflict.conflict_level != previous_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictLevelChanged;
		change.conflict = conflict;
		change.previous_level = previous_conflict_level;

		ধ্রুৱ পূর্ণাঙ্ক most_urgent_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);
		change.conflict_is_most_urgent = (_most_urgent.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_urgent_idx)
						     && _buffer[most_urgent_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	ঘূৰাই সঁচা;
}

তার্কিক ConflictTracker::remove_stale_conflicts(ধ্রুৱ hrt_abstime now, ধ্রুৱ hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	পূর্ণাঙ্ক nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	বাবে (পূর্ণাঙ্ক idx = (static_cast<পূর্ণাঙ্ক>(_buffer.size()) - 1); idx >= 0; --idx) {
		ধ্রুৱ hrt_abstime latest_update = _buffer[idx].latest_update_timestamp;

		যদি (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ধ্রুৱ conflict_info_s removed_conflict = _buffer[idx];
			_buffer.remove(idx);
			record_removed(changes, removed_conflict, RemoveBufferCause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	যদি (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	ঘূৰাই nb_conflicts_removed > 0;
}

শূন্য ConflictTracker::clear()
{
	_buffer.clear();
	reset_most_urgent();
}

শূন্য ConflictTracker::reset_most_urgent()
{
	স্থির constexpr ভিন্ন kNoConflictDistance{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	_most_urgent = {};
	_most_urgent.aircraft_dist = kNoConflictDistance;
}

শূন্য ConflictTracker::refresh_most_urgent()
{
	যদি (_buffer.empty()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_urgent();
		ঘূৰাই;
	}

	_most_urgent = find_most_urgent();
}

conflict_info_s ConflictTracker::find_most_urgent() ধ্রুৱ
{
	ধ্রুৱ পূর্ণাঙ্ক most_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);

	যদি (!is_valid_idx(most_urgent_conflict_idx)) {
		ঘূৰাই {};
	}

	ঘূৰাই _buffer[most_urgent_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ধ্রুৱ DaaEncodedId &encoded_id) ধ্রুৱ
{
	ধ্রুৱ পূর্ণাঙ্ক conflict_idx = find_conflict_idx(encoded_id);

	যদি (!is_valid_idx(conflict_idx)) {
		ঘূৰাই {};
	}

	ঘূৰাই _buffer[conflict_idx];
}

পূর্ণাঙ্ক ConflictTracker::find_conflict_idx_by_priority(ধ্রুৱ ConflictPriority priority) ধ্রুৱ
{
	ধ্রুৱ পূর্ণাঙ্ক buff_size = static_cast<পূর্ণাঙ্ক>(_buffer.size());

	যদি (buff_size < 1) {
		ঘূৰাই -1;
	}

	পূর্ণাঙ্ক best_idx = 0;

	বাবে (পূর্ণাঙ্ক i = 1; i < buff_size; ++i) {
		ধ্রুৱ তার্কিক is_better = (priority == ConflictPriority::kMostUrgent)
				       ? is_conflict_more_important(_buffer[i], _buffer[best_idx])
				       : is_conflict_less_important(_buffer[i], _buffer[best_idx]);

		যদি (is_better) {
			best_idx = i;
		}
	}

	ঘূৰাই best_idx;
}

পূর্ণাঙ্ক ConflictTracker::find_conflict_idx(ধ্রুৱ DaaEncodedId &encoded_id) ধ্রুৱ
{
	বাবে (size_t i = 0; i < _buffer.size(); i++) {
		যদি (_buffer[i].encoded_id == encoded_id) {
			ঘূৰাই static_cast<পূর্ণাঙ্ক>(i);
		}
	}

	ঘূৰাই -1;
}

তার্কিক ConflictTracker::is_conflict_less_important(ধ্রুৱ conflict_info_s &new_conflict,
		ধ্রুৱ conflict_info_s &base_conflict)
{
	যদি (new_conflict.conflict_level != base_conflict.conflict_level) {
		ঘূৰাই new_conflict.conflict_level < base_conflict.conflict_level;
	}

	ঘূৰাই new_conflict.aircraft_dist > base_conflict.aircraft_dist;
}

তার্কিক ConflictTracker::is_conflict_more_important(ধ্রুৱ conflict_info_s &new_conflict,
		ধ্রুৱ conflict_info_s &base_conflict)
{
	যদি (new_conflict.conflict_level != base_conflict.conflict_level) {
		ঘূৰাই new_conflict.conflict_level > base_conflict.conflict_level;
	}

	ঘূৰাই new_conflict.aircraft_dist < base_conflict.aircraft_dist;
}
