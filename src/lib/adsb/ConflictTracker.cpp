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

#ضمن "ConflictTracker.h"

#ضمن <px4_platform_common/defines.h>
#ضمن <px4_platform_common/log.h>

نطاق
{

فراغ append_change(conflict_tracker_changes_s &changes, ثابت conflict_tracker_change_s &change)
{
	𐡄𐡍 (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

فراغ record_ignored(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

فراغ record_removed(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت RemoveBufferCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

منطقي ConflictTracker::apply_conflict(ثابت conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	𐡄𐡍 (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		أرجع خطأ;
	}

	ثابت صحيح conflict_idx = find_conflict_idx(conflict.encoded_id);

	𐡄𐡍 (conflict_idx < 0) {
		𐡄𐡍 (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			أرجع خطأ;
		}

		أرجع add_conflict(conflict, changes);
	}

	أرجع update_conflict(conflict, conflict_idx, changes);
}

منطقي ConflictTracker::add_conflict(ثابت conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	𐡄𐡍 (_buffer.size() < _buffer.max_size()) {
		_buffer.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		أرجع 𐡒𐡔𐡈;
	}

	ثابت صحيح least_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kLeastUrgent);

	𐡄𐡍 (!is_conflict_more_important(conflict, _buffer[least_urgent_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		record_ignored(changes, conflict, IgnoreTrafficCause::kBufferFull);
		أرجع خطأ;
	}

	ثابت conflict_info_s removed_conflict = _buffer[least_urgent_conflict_idx];
	_buffer[least_urgent_conflict_idx] = conflict;
	record_removed(changes, removed_conflict, RemoveBufferCause::kBufferFull);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	أرجع 𐡒𐡔𐡈;
}

منطقي ConflictTracker::update_conflict(ثابت conflict_info_s &conflict, ثابت صحيح conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ثابت uint8_t previous_conflict_level = _buffer[conflict_idx].conflict_level;

	𐡄𐡍 (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		_buffer.remove(conflict_idx);

	} وإلا {
		_buffer[conflict_idx] = conflict;
	}

	𐡄𐡍 (conflict.conflict_level != previous_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictLevelChanged;
		change.conflict = conflict;
		change.previous_level = previous_conflict_level;

		ثابت صحيح most_urgent_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);
		change.conflict_is_most_urgent = (_most_urgent.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_urgent_idx)
						     && _buffer[most_urgent_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	أرجع 𐡒𐡔𐡈;
}

منطقي ConflictTracker::remove_stale_conflicts(ثابت hrt_abstime now, ثابت hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	صحيح nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	لكل (صحيح idx = (static_cast<صحيح>(_buffer.size()) - 1); idx >= 0; --idx) {
		ثابت hrt_abstime latest_update = _buffer[idx].latest_update_timestamp;

		𐡄𐡍 (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ثابت conflict_info_s removed_conflict = _buffer[idx];
			_buffer.remove(idx);
			record_removed(changes, removed_conflict, RemoveBufferCause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	𐡄𐡍 (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	أرجع nb_conflicts_removed > 0;
}

فراغ ConflictTracker::clear()
{
	_buffer.clear();
	reset_most_urgent();
}

فراغ ConflictTracker::reset_most_urgent()
{
	ساكن constexpr عائم kNoConflictDistance{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	_most_urgent = {};
	_most_urgent.aircraft_dist = kNoConflictDistance;
}

فراغ ConflictTracker::refresh_most_urgent()
{
	𐡄𐡍 (_buffer.empty()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_urgent();
		أرجع;
	}

	_most_urgent = find_most_urgent();
}

conflict_info_s ConflictTracker::find_most_urgent() ثابت
{
	ثابت صحيح most_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);

	𐡄𐡍 (!is_valid_idx(most_urgent_conflict_idx)) {
		أرجع {};
	}

	أرجع _buffer[most_urgent_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ثابت DaaEncodedId &encoded_id) ثابت
{
	ثابت صحيح conflict_idx = find_conflict_idx(encoded_id);

	𐡄𐡍 (!is_valid_idx(conflict_idx)) {
		أرجع {};
	}

	أرجع _buffer[conflict_idx];
}

صحيح ConflictTracker::find_conflict_idx_by_priority(ثابت ConflictPriority priority) ثابت
{
	ثابت صحيح buff_size = static_cast<صحيح>(_buffer.size());

	𐡄𐡍 (buff_size < 1) {
		أرجع -1;
	}

	صحيح best_idx = 0;

	لكل (صحيح i = 1; i < buff_size; ++i) {
		ثابت منطقي is_better = (priority == ConflictPriority::kMostUrgent)
				       ? is_conflict_more_important(_buffer[i], _buffer[best_idx])
				       : is_conflict_less_important(_buffer[i], _buffer[best_idx]);

		𐡄𐡍 (is_better) {
			best_idx = i;
		}
	}

	أرجع best_idx;
}

صحيح ConflictTracker::find_conflict_idx(ثابت DaaEncodedId &encoded_id) ثابت
{
	لكل (size_t i = 0; i < _buffer.size(); i++) {
		𐡄𐡍 (_buffer[i].encoded_id == encoded_id) {
			أرجع static_cast<صحيح>(i);
		}
	}

	أرجع -1;
}

منطقي ConflictTracker::is_conflict_less_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &base_conflict)
{
	𐡄𐡍 (new_conflict.conflict_level != base_conflict.conflict_level) {
		أرجع new_conflict.conflict_level < base_conflict.conflict_level;
	}

	أرجع new_conflict.aircraft_dist > base_conflict.aircraft_dist;
}

منطقي ConflictTracker::is_conflict_more_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &base_conflict)
{
	𐡄𐡍 (new_conflict.conflict_level != base_conflict.conflict_level) {
		أرجع new_conflict.conflict_level > base_conflict.conflict_level;
	}

	أرجع new_conflict.aircraft_dist < base_conflict.aircraft_dist;
}
