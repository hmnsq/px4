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

فراغ رکورد_ignored(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

فراغ رکورد_removed(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت Remove_میانگیر_Cause cause)
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

	ثابت صحيح conflict_idx = پیدا_کن_conflict_idx(conflict.encoded_id);

	إذا (conflict_idx < 0) {
		إذا (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			أرجع خطأ;
		}

		أرجع جمع_کن_conflict(conflict, changes);
	}

	أرجع update_conflict(conflict, conflict_idx, changes);
}

منطقي ConflictTracker::جمع_کن_conflict(ثابت conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	إذا (میانگیر.اندازه() < میانگیر.max_اندازه()) {
		میانگیر.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		أرجع درست;
	}

	ثابت صحيح کمترین_اضطراری_conflict_idx = پیدا_کن_conflict_idx_by_اولویت(Conflict_اولویت::k_کمترین_اضطراری);

	إذا (!is_conflict_بیشتر_important(conflict, میانگیر[کمترین_اضطراری_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		رکورد_ignored(changes, conflict, IgnoreTrafficCause::k_میانگیر_پر);
		أرجع خطأ;
	}

	ثابت conflict_info_s removed_conflict = میانگیر[کمترین_اضطراری_conflict_idx];
	میانگیر[کمترین_اضطراری_conflict_idx] = conflict;
	رکورد_removed(changes, removed_conflict, Remove_میانگیر_Cause::k_میانگیر_پر);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	أرجع درست;
}

منطقي ConflictTracker::update_conflict(ثابت conflict_info_s &conflict, ثابت صحيح conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ثابت uint8_t قبلی_conflict_level = میانگیر[conflict_idx].conflict_level;

	إذا (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		میانگیر.remove(conflict_idx);

	} وإلا {
		میانگیر[conflict_idx] = conflict;
	}

	إذا (conflict.conflict_level != قبلی_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::k_Conflict_Level_تغییریافته;
		change.conflict = conflict;
		change.قبلی_level = قبلی_conflict_level;

		ثابت صحيح most_اضطراری_idx = پیدا_کن_conflict_idx_by_اولویت(Conflict_اولویت::k_Most_اضطراری);
		change.conflict_is_most_اضطراری = (most_اضطراری.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_اضطراری_idx)
						     && میانگیر[most_اضطراری_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	أرجع درست;
}

منطقي ConflictTracker::remove_stale_conflicts(ثابت hrt_abstime now, ثابت hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	صحيح nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	لكل (صحيح idx = (static_cast<صحيح>(میانگیر.اندازه()) - 1); idx >= 0; --idx) {
		ثابت hrt_abstime latest_update = میانگیر[idx].latest_update_timestamp;

		إذا (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ثابت conflict_info_s removed_conflict = میانگیر[idx];
			میانگیر.remove(idx);
			رکورد_removed(changes, removed_conflict, Remove_میانگیر_Cause::kStaleConflict);
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
	میانگیر.clear();
	reset_most_اضطراری();
}

فراغ ConflictTracker::reset_most_اضطراری()
{
	ساكن constexpr عائم k_No_Conflict_فاصله{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	most_اضطراری = {};
	most_اضطراری.aircraft_dist = k_No_Conflict_فاصله;
}

فراغ ConflictTracker::refresh_most_اضطراری()
{
	إذا (میانگیر.خالی()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_اضطراری();
		أرجع;
	}

	most_اضطراری = پیدا_کن_most_اضطراری();
}

conflict_info_s ConflictTracker::پیدا_کن_most_اضطراری() ثابت
{
	ثابت صحيح most_اضطراری_conflict_idx = پیدا_کن_conflict_idx_by_اولویت(Conflict_اولویت::k_Most_اضطراری);

	إذا (!is_valid_idx(most_اضطراری_conflict_idx)) {
		أرجع {};
	}

	أرجع میانگیر[most_اضطراری_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ثابت DaaEncodedId &encoded_id) ثابت
{
	ثابت صحيح conflict_idx = پیدا_کن_conflict_idx(encoded_id);

	إذا (!is_valid_idx(conflict_idx)) {
		أرجع {};
	}

	أرجع میانگیر[conflict_idx];
}

صحيح ConflictTracker::پیدا_کن_conflict_idx_by_اولویت(ثابت Conflict_اولویت اولویت) ثابت
{
	ثابت صحيح buff_اندازه = static_cast<صحيح>(میانگیر.اندازه());

	إذا (buff_اندازه < 1) {
		أرجع -1;
	}

	صحيح بهترین_idx = 0;

	لكل (صحيح i = 1; i < buff_اندازه; ++i) {
		ثابت منطقي is_better = (اولویت == Conflict_اولویت::k_Most_اضطراری)
				       ? is_conflict_بیشتر_important(میانگیر[i], میانگیر[بهترین_idx])
				       : is_conflict_less_important(میانگیر[i], میانگیر[بهترین_idx]);

		إذا (is_better) {
			بهترین_idx = i;
		}
	}

	أرجع بهترین_idx;
}

صحيح ConflictTracker::پیدا_کن_conflict_idx(ثابت DaaEncodedId &encoded_id) ثابت
{
	لكل (size_t i = 0; i < میانگیر.اندازه(); i++) {
		إذا (میانگیر[i].encoded_id == encoded_id) {
			أرجع static_cast<صحيح>(i);
		}
	}

	أرجع -1;
}

منطقي ConflictTracker::is_conflict_less_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &پایه_conflict)
{
	إذا (new_conflict.conflict_level != پایه_conflict.conflict_level) {
		أرجع new_conflict.conflict_level < پایه_conflict.conflict_level;
	}

	أرجع new_conflict.aircraft_dist > پایه_conflict.aircraft_dist;
}

منطقي ConflictTracker::is_conflict_بیشتر_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &پایه_conflict)
{
	إذا (new_conflict.conflict_level != پایه_conflict.conflict_level) {
		أرجع new_conflict.conflict_level > پایه_conflict.conflict_level;
	}

	أرجع new_conflict.aircraft_dist < پایه_conflict.aircraft_dist;
}
