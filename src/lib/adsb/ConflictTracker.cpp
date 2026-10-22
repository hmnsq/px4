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

#include "ConflictTracker.h"

#include <px4_platform_common/defines.h>
#include <px4_platform_common/log.h>

namespace
{

void append_change(conflict_tracker_changes_s &changes, const conflict_tracker_change_s &change)
{
	ถ้า (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

void record_ignored(conflict_tracker_changes_s &changes, const conflict_info_s &conflict, const IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

void record_removed(conflict_tracker_changes_s &changes, const conflict_info_s &conflict, const RemoveBufferCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

bool ConflictTracker::apply_conflict(const conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ถ้า (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		คืน false;
	}

	const จำนวนเต็ม conflict_idx = find_conflict_idx(conflict.encoded_id);

	ถ้า (conflict_idx < 0) {
		ถ้า (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			คืน false;
		}

		คืน add_conflict(conflict, changes);
	}

	คืน update_conflict(conflict, conflict_idx, changes);
}

bool ConflictTracker::add_conflict(const conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ถ้า (_buffer.size() < _buffer.max_size()) {
		_buffer.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		คืน true;
	}

	const จำนวนเต็ม least_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kLeastUrgent);

	ถ้า (!is_conflict_more_important(conflict, _buffer[least_urgent_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		record_ignored(changes, conflict, IgnoreTrafficCause::kBufferFull);
		คืน false;
	}

	const conflict_info_s removed_conflict = _buffer[least_urgent_conflict_idx];
	_buffer[least_urgent_conflict_idx] = conflict;
	record_removed(changes, removed_conflict, RemoveBufferCause::kBufferFull);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	คืน true;
}

bool ConflictTracker::update_conflict(const conflict_info_s &conflict, const จำนวนเต็ม conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	const uint8_t previous_conflict_level = _buffer[conflict_idx].conflict_level;

	ถ้า (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		_buffer.remove(conflict_idx);

	} มิฉะนั้น {
		_buffer[conflict_idx] = conflict;
	}

	ถ้า (conflict.conflict_level != previous_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictLevelChanged;
		change.conflict = conflict;
		change.previous_level = previous_conflict_level;

		const จำนวนเต็ม most_urgent_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);
		change.conflict_is_most_urgent = (_most_urgent.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_urgent_idx)
						     && _buffer[most_urgent_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	คืน true;
}

bool ConflictTracker::remove_stale_conflicts(const hrt_abstime now, const hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	จำนวนเต็ม nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	สำหรับ (จำนวนเต็ม idx = (static_cast<จำนวนเต็ม>(_buffer.size()) - 1); idx >= 0; --idx) {
		const hrt_abstime latest_update = _buffer[idx].latest_update_timestamp;

		ถ้า (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			const conflict_info_s removed_conflict = _buffer[idx];
			_buffer.remove(idx);
			record_removed(changes, removed_conflict, RemoveBufferCause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	ถ้า (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	คืน nb_conflicts_removed > 0;
}

void ConflictTracker::clear()
{
	_buffer.clear();
	reset_most_urgent();
}

void ConflictTracker::reset_most_urgent()
{
	static constexpr float kNoConflictDistance{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	_most_urgent = {};
	_most_urgent.aircraft_dist = kNoConflictDistance;
}

void ConflictTracker::refresh_most_urgent()
{
	ถ้า (_buffer.empty()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_urgent();
		คืน;
	}

	_most_urgent = find_most_urgent();
}

conflict_info_s ConflictTracker::find_most_urgent() const
{
	const จำนวนเต็ม most_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);

	ถ้า (!is_valid_idx(most_urgent_conflict_idx)) {
		คืน {};
	}

	คืน _buffer[most_urgent_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(const DaaEncodedId &encoded_id) const
{
	const จำนวนเต็ม conflict_idx = find_conflict_idx(encoded_id);

	ถ้า (!is_valid_idx(conflict_idx)) {
		คืน {};
	}

	คืน _buffer[conflict_idx];
}

จำนวนเต็ม ConflictTracker::find_conflict_idx_by_priority(const ConflictPriority priority) const
{
	const จำนวนเต็ม buff_size = static_cast<จำนวนเต็ม>(_buffer.size());

	ถ้า (buff_size < 1) {
		คืน -1;
	}

	จำนวนเต็ม best_idx = 0;

	สำหรับ (จำนวนเต็ม i = 1; i < buff_size; ++i) {
		const bool is_better = (priority == ConflictPriority::kMostUrgent)
				       ? is_conflict_more_important(_buffer[i], _buffer[best_idx])
				       : is_conflict_less_important(_buffer[i], _buffer[best_idx]);

		ถ้า (is_better) {
			best_idx = i;
		}
	}

	คืน best_idx;
}

จำนวนเต็ม ConflictTracker::find_conflict_idx(const DaaEncodedId &encoded_id) const
{
	สำหรับ (size_t i = 0; i < _buffer.size(); i++) {
		ถ้า (_buffer[i].encoded_id == encoded_id) {
			คืน static_cast<จำนวนเต็ม>(i);
		}
	}

	คืน -1;
}

bool ConflictTracker::is_conflict_less_important(const conflict_info_s &new_conflict,
		const conflict_info_s &base_conflict)
{
	ถ้า (new_conflict.conflict_level != base_conflict.conflict_level) {
		คืน new_conflict.conflict_level < base_conflict.conflict_level;
	}

	คืน new_conflict.aircraft_dist > base_conflict.aircraft_dist;
}

bool ConflictTracker::is_conflict_more_important(const conflict_info_s &new_conflict,
		const conflict_info_s &base_conflict)
{
	ถ้า (new_conflict.conflict_level != base_conflict.conflict_level) {
		คืน new_conflict.conflict_level > base_conflict.conflict_level;
	}

	คืน new_conflict.aircraft_dist < base_conflict.aircraft_dist;
}
