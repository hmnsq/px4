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
	إذا (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

فراغ ریکارډ_ignored(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

فراغ ریکارډ_removed(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت Remove_بفر_Cause cause)
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
	إذا (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		أرجع خطأ;
	}

	ثابت صحيح conflict_idx = پیدا_کړه_conflict_idx(conflict.encoded_id);

	إذا (conflict_idx < 0) {
		إذا (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			أرجع خطأ;
		}

		أرجع جمع_کړه_conflict(conflict, changes);
	}

	أرجع update_conflict(conflict, conflict_idx, changes);
}

منطقي ConflictTracker::جمع_کړه_conflict(ثابت conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	إذا (بفر.اندازه() < بفر.max_اندازه()) {
		بفر.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		أرجع ریښتیا;
	}

	ثابت صحيح لږترلږه_بیړنی_conflict_idx = پیدا_کړه_conflict_idx_by_لومړیتوب(Conflict_لومړیتوب::k_لږترلږه_بیړنی);

	إذا (!is_conflict_نور_important(conflict, بفر[لږترلږه_بیړنی_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		ریکارډ_ignored(changes, conflict, IgnoreTrafficCause::k_بفر_ډک);
		أرجع خطأ;
	}

	ثابت conflict_info_s removed_conflict = بفر[لږترلږه_بیړنی_conflict_idx];
	بفر[لږترلږه_بیړنی_conflict_idx] = conflict;
	ریکارډ_removed(changes, removed_conflict, Remove_بفر_Cause::k_بفر_ډک);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	أرجع ریښتیا;
}

منطقي ConflictTracker::update_conflict(ثابت conflict_info_s &conflict, ثابت صحيح conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ثابت uint8_t پخوانی_conflict_level = بفر[conflict_idx].conflict_level;

	إذا (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		بفر.remove(conflict_idx);

	} وإلا {
		بفر[conflict_idx] = conflict;
	}

	إذا (conflict.conflict_level != پخوانی_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::k_Conflict_Level_بدل_شوی;
		change.conflict = conflict;
		change.پخوانی_level = پخوانی_conflict_level;

		ثابت صحيح most_بیړنی_idx = پیدا_کړه_conflict_idx_by_لومړیتوب(Conflict_لومړیتوب::k_Most_بیړنی);
		change.conflict_is_most_بیړنی = (most_بیړنی.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_بیړنی_idx)
						     && بفر[most_بیړنی_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	أرجع ریښتیا;
}

منطقي ConflictTracker::remove_stale_conflicts(ثابت hrt_abstime now, ثابت hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	صحيح nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	لكل (صحيح idx = (static_cast<صحيح>(بفر.اندازه()) - 1); idx >= 0; --idx) {
		ثابت hrt_abstime latest_update = بفر[idx].latest_update_timestamp;

		إذا (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ثابت conflict_info_s removed_conflict = بفر[idx];
			بفر.remove(idx);
			ریکارډ_removed(changes, removed_conflict, Remove_بفر_Cause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	إذا (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	أرجع nb_conflicts_removed > 0;
}

فراغ ConflictTracker::clear()
{
	بفر.clear();
	reset_most_بیړنی();
}

فراغ ConflictTracker::reset_most_بیړنی()
{
	ساكن constexpr عائم k_No_Conflict_واټن{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	most_بیړنی = {};
	most_بیړنی.aircraft_dist = k_No_Conflict_واټن;
}

فراغ ConflictTracker::refresh_most_بیړنی()
{
	إذا (بفر.تشه()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_بیړنی();
		أرجع;
	}

	most_بیړنی = پیدا_کړه_most_بیړنی();
}

conflict_info_s ConflictTracker::پیدا_کړه_most_بیړنی() ثابت
{
	ثابت صحيح most_بیړنی_conflict_idx = پیدا_کړه_conflict_idx_by_لومړیتوب(Conflict_لومړیتوب::k_Most_بیړنی);

	إذا (!is_valid_idx(most_بیړنی_conflict_idx)) {
		أرجع {};
	}

	أرجع بفر[most_بیړنی_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ثابت DaaEncodedId &encoded_id) ثابت
{
	ثابت صحيح conflict_idx = پیدا_کړه_conflict_idx(encoded_id);

	إذا (!is_valid_idx(conflict_idx)) {
		أرجع {};
	}

	أرجع بفر[conflict_idx];
}

صحيح ConflictTracker::پیدا_کړه_conflict_idx_by_لومړیتوب(ثابت Conflict_لومړیتوب لومړیتوب) ثابت
{
	ثابت صحيح buff_اندازه = static_cast<صحيح>(بفر.اندازه());

	إذا (buff_اندازه < 1) {
		أرجع -1;
	}

	صحيح غوره_idx = 0;

	لكل (صحيح i = 1; i < buff_اندازه; ++i) {
		ثابت منطقي is_better = (لومړیتوب == Conflict_لومړیتوب::k_Most_بیړنی)
				       ? is_conflict_نور_important(بفر[i], بفر[غوره_idx])
				       : is_conflict_less_important(بفر[i], بفر[غوره_idx]);

		إذا (is_better) {
			غوره_idx = i;
		}
	}

	أرجع غوره_idx;
}

صحيح ConflictTracker::پیدا_کړه_conflict_idx(ثابت DaaEncodedId &encoded_id) ثابت
{
	لكل (size_t i = 0; i < بفر.اندازه(); i++) {
		إذا (بفر[i].encoded_id == encoded_id) {
			أرجع static_cast<صحيح>(i);
		}
	}

	أرجع -1;
}

منطقي ConflictTracker::is_conflict_less_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &بنسټ_conflict)
{
	إذا (new_conflict.conflict_level != بنسټ_conflict.conflict_level) {
		أرجع new_conflict.conflict_level < بنسټ_conflict.conflict_level;
	}

	أرجع new_conflict.aircraft_dist > بنسټ_conflict.aircraft_dist;
}

منطقي ConflictTracker::is_conflict_نور_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &بنسټ_conflict)
{
	إذا (new_conflict.conflict_level != بنسټ_conflict.conflict_level) {
		أرجع new_conflict.conflict_level > بنسټ_conflict.conflict_level;
	}

	أرجع new_conflict.aircraft_dist < بنسټ_conflict.aircraft_dist;
}
