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

शून्य लेखः_ignored(conflict_tracker_changes_s &changes, ध्रुव conflict_info_s &conflict, ध्रुव IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

शून्य लेखः_removed(conflict_tracker_changes_s &changes, ध्रुव conflict_info_s &conflict, ध्रुव Remove_बफर_Cause cause)
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

		प्रत्यागम सङ्कलय_conflict(conflict, changes);
	}

	प्रत्यागम update_conflict(conflict, conflict_idx, changes);
}

तार्किक ConflictTracker::सङ्कलय_conflict(ध्रुव conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	यदि (बफर.आकार() < बफर.max_आकार()) {
		बफर.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		प्रत्यागम सत्य;
	}

	ध्रुव पूर्णाङ्क न्यूनतमम्_आत्ययिकम्_conflict_idx = find_conflict_idx_by_प्राथमिकता(Conflict_प्राथमिकता::k_न्यूनतमम्_आत्ययिकम्);

	यदि (!is_conflict_अधिकम्_important(conflict, बफर[न्यूनतमम्_आत्ययिकम्_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		लेखः_ignored(changes, conflict, IgnoreTrafficCause::k_बफर_पूर्णम्);
		प्रत्यागम असत्य;
	}

	ध्रुव conflict_info_s removed_conflict = बफर[न्यूनतमम्_आत्ययिकम्_conflict_idx];
	बफर[न्यूनतमम्_आत्ययिकम्_conflict_idx] = conflict;
	लेखः_removed(changes, removed_conflict, Remove_बफर_Cause::k_बफर_पूर्णम्);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	प्रत्यागम सत्य;
}

तार्किक ConflictTracker::update_conflict(ध्रुव conflict_info_s &conflict, ध्रुव पूर्णाङ्क conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ध्रुव uint8_t previous_conflict_level = बफर[conflict_idx].conflict_level;

	यदि (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		बफर.remove(conflict_idx);

	} अन्यथा {
		बफर[conflict_idx] = conflict;
	}

	यदि (conflict.conflict_level != previous_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::k_Conflict_Level_परिवर्तितम्;
		change.conflict = conflict;
		change.previous_level = previous_conflict_level;

		ध्रुव पूर्णाङ्क most_आत्ययिकम्_idx = find_conflict_idx_by_प्राथमिकता(Conflict_प्राथमिकता::k_Most_आत्ययिकम्);
		change.conflict_is_most_आत्ययिकम् = (most_आत्ययिकम्.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_आत्ययिकम्_idx)
						     && बफर[most_आत्ययिकम्_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	प्रत्यागम सत्य;
}

तार्किक ConflictTracker::remove_stale_conflicts(ध्रुव hrt_abstime now, ध्रुव hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	पूर्णाङ्क nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	प्रति (पूर्णाङ्क idx = (static_cast<पूर्णाङ्क>(बफर.आकार()) - 1); idx >= 0; --idx) {
		ध्रुव hrt_abstime latest_update = बफर[idx].latest_update_timestamp;

		यदि (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ध्रुव conflict_info_s removed_conflict = बफर[idx];
			बफर.remove(idx);
			लेखः_removed(changes, removed_conflict, Remove_बफर_Cause::kStaleConflict);
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
	बफर.clear();
	reset_most_आत्ययिकम्();
}

शून्य ConflictTracker::reset_most_आत्ययिकम्()
{
	स्थिर constexpr भिन्न kNoConflictDistance{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	most_आत्ययिकम् = {};
	most_आत्ययिकम्.aircraft_dist = kNoConflictDistance;
}

शून्य ConflictTracker::refresh_most_आत्ययिकम्()
{
	यदि (बफर.रिक्तानि()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_आत्ययिकम्();
		प्रत्यागम;
	}

	most_आत्ययिकम् = find_most_आत्ययिकम्();
}

conflict_info_s ConflictTracker::find_most_आत्ययिकम्() ध्रुव
{
	ध्रुव पूर्णाङ्क most_आत्ययिकम्_conflict_idx = find_conflict_idx_by_प्राथमिकता(Conflict_प्राथमिकता::k_Most_आत्ययिकम्);

	यदि (!is_valid_idx(most_आत्ययिकम्_conflict_idx)) {
		प्रत्यागम {};
	}

	प्रत्यागम बफर[most_आत्ययिकम्_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ध्रुव DaaEncodedId &encoded_id) ध्रुव
{
	ध्रुव पूर्णाङ्क conflict_idx = find_conflict_idx(encoded_id);

	यदि (!is_valid_idx(conflict_idx)) {
		प्रत्यागम {};
	}

	प्रत्यागम बफर[conflict_idx];
}

पूर्णाङ्क ConflictTracker::find_conflict_idx_by_प्राथमिकता(ध्रुव Conflict_प्राथमिकता प्राथमिकता) ध्रुव
{
	ध्रुव पूर्णाङ्क buff_आकार = static_cast<पूर्णाङ्क>(बफर.आकार());

	यदि (buff_आकार < 1) {
		प्रत्यागम -1;
	}

	पूर्णाङ्क best_idx = 0;

	प्रति (पूर्णाङ्क i = 1; i < buff_आकार; ++i) {
		ध्रुव तार्किक is_better = (प्राथमिकता == Conflict_प्राथमिकता::k_Most_आत्ययिकम्)
				       ? is_conflict_अधिकम्_important(बफर[i], बफर[best_idx])
				       : is_conflict_less_important(बफर[i], बफर[best_idx]);

		यदि (is_better) {
			best_idx = i;
		}
	}

	प्रत्यागम best_idx;
}

पूर्णाङ्क ConflictTracker::find_conflict_idx(ध्रुव DaaEncodedId &encoded_id) ध्रुव
{
	प्रति (size_t i = 0; i < बफर.आकार(); i++) {
		यदि (बफर[i].encoded_id == encoded_id) {
			प्रत्यागम static_cast<पूर्णाङ्क>(i);
		}
	}

	प्रत्यागम -1;
}

तार्किक ConflictTracker::is_conflict_less_important(ध्रुव conflict_info_s &new_conflict,
		ध्रुव conflict_info_s &आधारः_conflict)
{
	यदि (new_conflict.conflict_level != आधारः_conflict.conflict_level) {
		प्रत्यागम new_conflict.conflict_level < आधारः_conflict.conflict_level;
	}

	प्रत्यागम new_conflict.aircraft_dist > आधारः_conflict.aircraft_dist;
}

तार्किक ConflictTracker::is_conflict_अधिकम्_important(ध्रुव conflict_info_s &new_conflict,
		ध्रुव conflict_info_s &आधारः_conflict)
{
	यदि (new_conflict.conflict_level != आधारः_conflict.conflict_level) {
		प्रत्यागम new_conflict.conflict_level > आधारः_conflict.conflict_level;
	}

	प्रत्यागम new_conflict.aircraft_dist < आधारः_conflict.aircraft_dist;
}
