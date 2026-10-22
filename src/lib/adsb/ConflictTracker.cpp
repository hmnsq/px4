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

#അന്തര്ഭാവ "ConflictTracker.h"

#അന്തര്ഭാവ <px4_platform_common/defines.h>
#അന്തര്ഭാവ <px4_platform_common/log.h>

നാമാകാശ
{

ശൂന്യം append_change(conflict_tracker_changes_s &changes, സ്ഥിരം conflict_tracker_change_s &change)
{
	എങ്കിൽ (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

ശൂന്യം record_ignored(conflict_tracker_changes_s &changes, സ്ഥിരം conflict_info_s &conflict, സ്ഥിരം IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

ശൂന്യം record_removed(conflict_tracker_changes_s &changes, സ്ഥിരം conflict_info_s &conflict, സ്ഥിരം RemoveBufferCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

തർക്കം ConflictTracker::apply_conflict(സ്ഥിരം conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	എങ്കിൽ (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		തിരികെ അസത്യം;
	}

	സ്ഥിരം പൂർണ്ണസംഖ്യ conflict_idx = find_conflict_idx(conflict.encoded_id);

	എങ്കിൽ (conflict_idx < 0) {
		എങ്കിൽ (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			തിരികെ അസത്യം;
		}

		തിരികെ add_conflict(conflict, changes);
	}

	തിരികെ update_conflict(conflict, conflict_idx, changes);
}

തർക്കം ConflictTracker::add_conflict(സ്ഥിരം conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	എങ്കിൽ (_buffer.size() < _buffer.max_size()) {
		_buffer.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		തിരികെ സത്യം;
	}

	സ്ഥിരം പൂർണ്ണസംഖ്യ least_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kLeastUrgent);

	എങ്കിൽ (!is_conflict_more_important(conflict, _buffer[least_urgent_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		record_ignored(changes, conflict, IgnoreTrafficCause::kBufferFull);
		തിരികെ അസത്യം;
	}

	സ്ഥിരം conflict_info_s removed_conflict = _buffer[least_urgent_conflict_idx];
	_buffer[least_urgent_conflict_idx] = conflict;
	record_removed(changes, removed_conflict, RemoveBufferCause::kBufferFull);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	തിരികെ സത്യം;
}

തർക്കം ConflictTracker::update_conflict(സ്ഥിരം conflict_info_s &conflict, സ്ഥിരം പൂർണ്ണസംഖ്യ conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	സ്ഥിരം uint8_t previous_conflict_level = _buffer[conflict_idx].conflict_level;

	എങ്കിൽ (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		_buffer.remove(conflict_idx);

	} അല്ലെങ്കിൽ {
		_buffer[conflict_idx] = conflict;
	}

	എങ്കിൽ (conflict.conflict_level != previous_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictLevelChanged;
		change.conflict = conflict;
		change.previous_level = previous_conflict_level;

		സ്ഥിരം പൂർണ്ണസംഖ്യ most_urgent_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);
		change.conflict_is_most_urgent = (_most_urgent.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_urgent_idx)
						     && _buffer[most_urgent_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	തിരികെ സത്യം;
}

തർക്കം ConflictTracker::remove_stale_conflicts(സ്ഥിരം hrt_abstime now, സ്ഥിരം hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	പൂർണ്ണസംഖ്യ nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	വേണ്ടി (പൂർണ്ണസംഖ്യ idx = (static_cast<പൂർണ്ണസംഖ്യ>(_buffer.size()) - 1); idx >= 0; --idx) {
		സ്ഥിരം hrt_abstime latest_update = _buffer[idx].latest_update_timestamp;

		എങ്കിൽ (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			സ്ഥിരം conflict_info_s removed_conflict = _buffer[idx];
			_buffer.remove(idx);
			record_removed(changes, removed_conflict, RemoveBufferCause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	എങ്കിൽ (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	തിരികെ nb_conflicts_removed > 0;
}

ശൂന്യം ConflictTracker::clear()
{
	_buffer.clear();
	reset_most_urgent();
}

ശൂന്യം ConflictTracker::reset_most_urgent()
{
	സ്ഥിതം constexpr ദശാംശം kNoConflictDistance{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	_most_urgent = {};
	_most_urgent.aircraft_dist = kNoConflictDistance;
}

ശൂന്യം ConflictTracker::refresh_most_urgent()
{
	എങ്കിൽ (_buffer.empty()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_urgent();
		തിരികെ;
	}

	_most_urgent = find_most_urgent();
}

conflict_info_s ConflictTracker::find_most_urgent() സ്ഥിരം
{
	സ്ഥിരം പൂർണ്ണസംഖ്യ most_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);

	എങ്കിൽ (!is_valid_idx(most_urgent_conflict_idx)) {
		തിരികെ {};
	}

	തിരികെ _buffer[most_urgent_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(സ്ഥിരം DaaEncodedId &encoded_id) സ്ഥിരം
{
	സ്ഥിരം പൂർണ്ണസംഖ്യ conflict_idx = find_conflict_idx(encoded_id);

	എങ്കിൽ (!is_valid_idx(conflict_idx)) {
		തിരികെ {};
	}

	തിരികെ _buffer[conflict_idx];
}

പൂർണ്ണസംഖ്യ ConflictTracker::find_conflict_idx_by_priority(സ്ഥിരം ConflictPriority priority) സ്ഥിരം
{
	സ്ഥിരം പൂർണ്ണസംഖ്യ buff_size = static_cast<പൂർണ്ണസംഖ്യ>(_buffer.size());

	എങ്കിൽ (buff_size < 1) {
		തിരികെ -1;
	}

	പൂർണ്ണസംഖ്യ best_idx = 0;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < buff_size; ++i) {
		സ്ഥിരം തർക്കം is_better = (priority == ConflictPriority::kMostUrgent)
				       ? is_conflict_more_important(_buffer[i], _buffer[best_idx])
				       : is_conflict_less_important(_buffer[i], _buffer[best_idx]);

		എങ്കിൽ (is_better) {
			best_idx = i;
		}
	}

	തിരികെ best_idx;
}

പൂർണ്ണസംഖ്യ ConflictTracker::find_conflict_idx(സ്ഥിരം DaaEncodedId &encoded_id) സ്ഥിരം
{
	വേണ്ടി (size_t i = 0; i < _buffer.size(); i++) {
		എങ്കിൽ (_buffer[i].encoded_id == encoded_id) {
			തിരികെ static_cast<പൂർണ്ണസംഖ്യ>(i);
		}
	}

	തിരികെ -1;
}

തർക്കം ConflictTracker::is_conflict_less_important(സ്ഥിരം conflict_info_s &new_conflict,
		സ്ഥിരം conflict_info_s &base_conflict)
{
	എങ്കിൽ (new_conflict.conflict_level != base_conflict.conflict_level) {
		തിരികെ new_conflict.conflict_level < base_conflict.conflict_level;
	}

	തിരികെ new_conflict.aircraft_dist > base_conflict.aircraft_dist;
}

തർക്കം ConflictTracker::is_conflict_more_important(സ്ഥിരം conflict_info_s &new_conflict,
		സ്ഥിരം conflict_info_s &base_conflict)
{
	എങ്കിൽ (new_conflict.conflict_level != base_conflict.conflict_level) {
		തിരികെ new_conflict.conflict_level > base_conflict.conflict_level;
	}

	തിരികെ new_conflict.aircraft_dist < base_conflict.aircraft_dist;
}
