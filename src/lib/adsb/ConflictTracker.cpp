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

فراغ ریکارڈ_ignored(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

فراغ ریکارڈ_removed(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت Remove_بفر_Cause cause)
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

	ثابت صحيح conflict_idx = لبھو_conflict_idx(conflict.encoded_id);

	إذا (conflict_idx < 0) {
		إذا (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			أرجع خطأ;
		}

		أرجع جوڑو_conflict(conflict, changes);
	}

	أرجع update_conflict(conflict, conflict_idx, changes);
}

منطقي ConflictTracker::جوڑو_conflict(ثابت conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	إذا (بفر.ناپ() < بفر.max_ناپ()) {
		بفر.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		أرجع سچ;
	}

	ثابت صحيح گھٹ_توں_گھٹ_ضروری_conflict_idx = لبھو_conflict_idx_by_ترجیح(Conflict_ترجیح::k_گھٹ_توں_گھٹ_ضروری);

	إذا (!is_conflict_ہور_important(conflict, بفر[گھٹ_توں_گھٹ_ضروری_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		ریکارڈ_ignored(changes, conflict, IgnoreTrafficCause::k_بفر_بھریا);
		أرجع خطأ;
	}

	ثابت conflict_info_s removed_conflict = بفر[گھٹ_توں_گھٹ_ضروری_conflict_idx];
	بفر[گھٹ_توں_گھٹ_ضروری_conflict_idx] = conflict;
	ریکارڈ_removed(changes, removed_conflict, Remove_بفر_Cause::k_بفر_بھریا);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	أرجع سچ;
}

منطقي ConflictTracker::update_conflict(ثابت conflict_info_s &conflict, ثابت صحيح conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ثابت uint8_t پچھلا_conflict_level = بفر[conflict_idx].conflict_level;

	إذا (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		بفر.remove(conflict_idx);

	} وإلا {
		بفر[conflict_idx] = conflict;
	}

	إذا (conflict.conflict_level != پچھلا_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::k_Conflict_Level_بدلیا;
		change.conflict = conflict;
		change.پچھلا_level = پچھلا_conflict_level;

		ثابت صحيح most_ضروری_idx = لبھو_conflict_idx_by_ترجیح(Conflict_ترجیح::k_Most_ضروری);
		change.conflict_is_most_ضروری = (most_ضروری.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_ضروری_idx)
						     && بفر[most_ضروری_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	أرجع سچ;
}

منطقي ConflictTracker::remove_stale_conflicts(ثابت hrt_abstime now, ثابت hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	صحيح nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	لكل (صحيح idx = (static_cast<صحيح>(بفر.ناپ()) - 1); idx >= 0; --idx) {
		ثابت hrt_abstime latest_update = بفر[idx].latest_update_timestamp;

		إذا (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ثابت conflict_info_s removed_conflict = بفر[idx];
			بفر.remove(idx);
			ریکارڈ_removed(changes, removed_conflict, Remove_بفر_Cause::kStaleConflict);
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
	reset_most_ضروری();
}

فراغ ConflictTracker::reset_most_ضروری()
{
	ساكن constexpr عائم k_No_Conflict_فاصلہ{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	most_ضروری = {};
	most_ضروری.aircraft_dist = k_No_Conflict_فاصلہ;
}

فراغ ConflictTracker::refresh_most_ضروری()
{
	إذا (بفر.خالی_تھاواں()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_ضروری();
		أرجع;
	}

	most_ضروری = لبھو_most_ضروری();
}

conflict_info_s ConflictTracker::لبھو_most_ضروری() ثابت
{
	ثابت صحيح most_ضروری_conflict_idx = لبھو_conflict_idx_by_ترجیح(Conflict_ترجیح::k_Most_ضروری);

	إذا (!is_valid_idx(most_ضروری_conflict_idx)) {
		أرجع {};
	}

	أرجع بفر[most_ضروری_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ثابت DaaEncodedId &encoded_id) ثابت
{
	ثابت صحيح conflict_idx = لبھو_conflict_idx(encoded_id);

	إذا (!is_valid_idx(conflict_idx)) {
		أرجع {};
	}

	أرجع بفر[conflict_idx];
}

صحيح ConflictTracker::لبھو_conflict_idx_by_ترجیح(ثابت Conflict_ترجیح ترجیح) ثابت
{
	ثابت صحيح buff_ناپ = static_cast<صحيح>(بفر.ناپ());

	إذا (buff_ناپ < 1) {
		أرجع -1;
	}

	صحيح بہترین_idx = 0;

	لكل (صحيح i = 1; i < buff_ناپ; ++i) {
		ثابت منطقي is_better = (ترجیح == Conflict_ترجیح::k_Most_ضروری)
				       ? is_conflict_ہور_important(بفر[i], بفر[بہترین_idx])
				       : is_conflict_less_important(بفر[i], بفر[بہترین_idx]);

		إذا (is_better) {
			بہترین_idx = i;
		}
	}

	أرجع بہترین_idx;
}

صحيح ConflictTracker::لبھو_conflict_idx(ثابت DaaEncodedId &encoded_id) ثابت
{
	لكل (size_t i = 0; i < بفر.ناپ(); i++) {
		إذا (بفر[i].encoded_id == encoded_id) {
			أرجع static_cast<صحيح>(i);
		}
	}

	أرجع -1;
}

منطقي ConflictTracker::is_conflict_less_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &ادھار_conflict)
{
	إذا (new_conflict.conflict_level != ادھار_conflict.conflict_level) {
		أرجع new_conflict.conflict_level < ادھار_conflict.conflict_level;
	}

	أرجع new_conflict.aircraft_dist > ادھار_conflict.aircraft_dist;
}

منطقي ConflictTracker::is_conflict_ہور_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &ادھار_conflict)
{
	إذا (new_conflict.conflict_level != ادھار_conflict.conflict_level) {
		أرجع new_conflict.conflict_level > ادھار_conflict.conflict_level;
	}

	أرجع new_conflict.aircraft_dist < ادھار_conflict.aircraft_dist;
}
