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

#অন্তর্ভাব "ConflictTracker.h"

#অন্তর্ভাব <px4_platform_common/defines.h>
#অন্তর্ভাব <px4_platform_common/log.h>

নামাকাশ
{

শূন্য append_change(conflict_tracker_changes_s &changes, ধ্রুবক conflict_tracker_change_s &change)
{
	যদি (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

শূন্য record_ignored(conflict_tracker_changes_s &changes, ধ্রুবক conflict_info_s &conflict, ধ্রুবক IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

শূন্য record_removed(conflict_tracker_changes_s &changes, ধ্রুবক conflict_info_s &conflict, ধ্রুবক Remove_বাফার_Cause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

যৌক্তিক ConflictTracker::apply_conflict(ধ্রুবক conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	যদি (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		ফেরত মিথ্যা;
	}

	ধ্রুবক পূর্ণসংখ্যা conflict_idx = find_conflict_idx(conflict.encoded_id);

	যদি (conflict_idx < 0) {
		যদি (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			ফেরত মিথ্যা;
		}

		ফেরত add_conflict(conflict, changes);
	}

	ফেরত update_conflict(conflict, conflict_idx, changes);
}

যৌক্তিক ConflictTracker::add_conflict(ধ্রুবক conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	যদি (বাফার.আকার_2() < বাফার.max_আকার()) {
		বাফার.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		ফেরত সত্য;
	}

	ধ্রুবক পূর্ণসংখ্যা least_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kLeastUrgent);

	যদি (!is_conflict_more_important(conflict, বাফার[least_urgent_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		record_ignored(changes, conflict, IgnoreTrafficCause::k_বাফার_Full);
		ফেরত মিথ্যা;
	}

	ধ্রুবক conflict_info_s removed_conflict = বাফার[least_urgent_conflict_idx];
	বাফার[least_urgent_conflict_idx] = conflict;
	record_removed(changes, removed_conflict, Remove_বাফার_Cause::k_বাফার_Full);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	ফেরত সত্য;
}

যৌক্তিক ConflictTracker::update_conflict(ধ্রুবক conflict_info_s &conflict, ধ্রুবক পূর্ণসংখ্যা conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ধ্রুবক uint8_t previous_conflict_level = বাফার[conflict_idx].conflict_level;

	যদি (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		বাফার.remove(conflict_idx);

	} অন্যথা {
		বাফার[conflict_idx] = conflict;
	}

	যদি (conflict.conflict_level != previous_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictLevelChanged;
		change.conflict = conflict;
		change.previous_level = previous_conflict_level;

		ধ্রুবক পূর্ণসংখ্যা most_urgent_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);
		change.conflict_is_most_urgent = (_most_urgent.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_urgent_idx)
						     && বাফার[most_urgent_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	ফেরত সত্য;
}

যৌক্তিক ConflictTracker::remove_stale_conflicts(ধ্রুবক hrt_abstime now, ধ্রুবক hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	পূর্ণসংখ্যা nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	জন্য (পূর্ণসংখ্যা idx = (static_cast<পূর্ণসংখ্যা>(বাফার.আকার_2()) - 1); idx >= 0; --idx) {
		ধ্রুবক hrt_abstime latest_update = বাফার[idx].latest_update_timestamp;

		যদি (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ধ্রুবক conflict_info_s removed_conflict = বাফার[idx];
			বাফার.remove(idx);
			record_removed(changes, removed_conflict, Remove_বাফার_Cause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	যদি (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	ফেরত nb_conflicts_removed > 0;
}

শূন্য ConflictTracker::clear()
{
	বাফার.clear();
	reset_most_urgent();
}

শূন্য ConflictTracker::reset_most_urgent()
{
	স্থির constexpr দশমিক kNoConflictDistance{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	_most_urgent = {};
	_most_urgent.aircraft_dist = kNoConflictDistance;
}

শূন্য ConflictTracker::refresh_most_urgent()
{
	যদি (বাফার.empty()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_urgent();
		ফেরত;
	}

	_most_urgent = find_most_urgent();
}

conflict_info_s ConflictTracker::find_most_urgent() ধ্রুবক
{
	ধ্রুবক পূর্ণসংখ্যা most_urgent_conflict_idx = find_conflict_idx_by_priority(ConflictPriority::kMostUrgent);

	যদি (!is_valid_idx(most_urgent_conflict_idx)) {
		ফেরত {};
	}

	ফেরত বাফার[most_urgent_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ধ্রুবক DaaEncodedId &encoded_id) ধ্রুবক
{
	ধ্রুবক পূর্ণসংখ্যা conflict_idx = find_conflict_idx(encoded_id);

	যদি (!is_valid_idx(conflict_idx)) {
		ফেরত {};
	}

	ফেরত বাফার[conflict_idx];
}

পূর্ণসংখ্যা ConflictTracker::find_conflict_idx_by_priority(ধ্রুবক ConflictPriority priority) ধ্রুবক
{
	ধ্রুবক পূর্ণসংখ্যা buff_আকার = static_cast<পূর্ণসংখ্যা>(বাফার.আকার_2());

	যদি (buff_আকার < 1) {
		ফেরত -1;
	}

	পূর্ণসংখ্যা best_idx = 0;

	জন্য (পূর্ণসংখ্যা i = 1; i < buff_আকার; ++i) {
		ধ্রুবক যৌক্তিক is_better = (priority == ConflictPriority::kMostUrgent)
				       ? is_conflict_more_important(বাফার[i], বাফার[best_idx])
				       : is_conflict_less_important(বাফার[i], বাফার[best_idx]);

		যদি (is_better) {
			best_idx = i;
		}
	}

	ফেরত best_idx;
}

পূর্ণসংখ্যা ConflictTracker::find_conflict_idx(ধ্রুবক DaaEncodedId &encoded_id) ধ্রুবক
{
	জন্য (size_t i = 0; i < বাফার.আকার_2(); i++) {
		যদি (বাফার[i].encoded_id == encoded_id) {
			ফেরত static_cast<পূর্ণসংখ্যা>(i);
		}
	}

	ফেরত -1;
}

যৌক্তিক ConflictTracker::is_conflict_less_important(ধ্রুবক conflict_info_s &new_conflict,
		ধ্রুবক conflict_info_s &base_conflict)
{
	যদি (new_conflict.conflict_level != base_conflict.conflict_level) {
		ফেরত new_conflict.conflict_level < base_conflict.conflict_level;
	}

	ফেরত new_conflict.aircraft_dist > base_conflict.aircraft_dist;
}

যৌক্তিক ConflictTracker::is_conflict_more_important(ধ্রুবক conflict_info_s &new_conflict,
		ধ্রুবক conflict_info_s &base_conflict)
{
	যদি (new_conflict.conflict_level != base_conflict.conflict_level) {
		ফেরত new_conflict.conflict_level > base_conflict.conflict_level;
	}

	ফেরত new_conflict.aircraft_dist < base_conflict.aircraft_dist;
}
