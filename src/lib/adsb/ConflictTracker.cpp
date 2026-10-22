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

فراغ رڪارڊ_ignored(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

فراغ رڪارڊ_removed(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت Remove_بفر_Cause cause)
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

	ثابت صحيح conflict_idx = ڳوليو_conflict_idx(conflict.encoded_id);

	إذا (conflict_idx < 0) {
		إذا (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			أرجع خطأ;
		}

		أرجع جوڙيو_conflict(conflict, changes);
	}

	أرجع update_conflict(conflict, conflict_idx, changes);
}

منطقي ConflictTracker::جوڙيو_conflict(ثابت conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	إذا (بفر.سائيز() < بفر.max_سائيز()) {
		بفر.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		أرجع سچ;
	}

	ثابت صحيح گهٽ_کان_گهٽ_تڪڙو_conflict_idx = ڳوليو_conflict_idx_by_ترجيح(Conflict_ترجيح::k_گهٽ_کان_گهٽ_تڪڙو);

	إذا (!is_conflict_وڌيڪ_important(conflict, بفر[گهٽ_کان_گهٽ_تڪڙو_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		رڪارڊ_ignored(changes, conflict, IgnoreTrafficCause::k_بفر_ڀريل);
		أرجع خطأ;
	}

	ثابت conflict_info_s removed_conflict = بفر[گهٽ_کان_گهٽ_تڪڙو_conflict_idx];
	بفر[گهٽ_کان_گهٽ_تڪڙو_conflict_idx] = conflict;
	رڪارڊ_removed(changes, removed_conflict, Remove_بفر_Cause::k_بفر_ڀريل);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	أرجع سچ;
}

منطقي ConflictTracker::update_conflict(ثابت conflict_info_s &conflict, ثابت صحيح conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ثابت uint8_t اڳيون_conflict_level = بفر[conflict_idx].conflict_level;

	إذا (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		بفر.remove(conflict_idx);

	} وإلا {
		بفر[conflict_idx] = conflict;
	}

	إذا (conflict.conflict_level != اڳيون_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::k_Conflict_Level_بدليو;
		change.conflict = conflict;
		change.اڳيون_level = اڳيون_conflict_level;

		ثابت صحيح most_تڪڙو_idx = ڳوليو_conflict_idx_by_ترجيح(Conflict_ترجيح::k_Most_تڪڙو);
		change.conflict_is_most_تڪڙو = (most_تڪڙو.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_تڪڙو_idx)
						     && بفر[most_تڪڙو_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	أرجع سچ;
}

منطقي ConflictTracker::remove_stale_conflicts(ثابت hrt_abstime now, ثابت hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	صحيح nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	لكل (صحيح idx = (static_cast<صحيح>(بفر.سائيز()) - 1); idx >= 0; --idx) {
		ثابت hrt_abstime latest_update = بفر[idx].latest_update_timestamp;

		إذا (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ثابت conflict_info_s removed_conflict = بفر[idx];
			بفر.remove(idx);
			رڪارڊ_removed(changes, removed_conflict, Remove_بفر_Cause::kStaleConflict);
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
	reset_most_تڪڙو();
}

فراغ ConflictTracker::reset_most_تڪڙو()
{
	ساكن constexpr عائم k_No_Conflict_مفاصلو{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	most_تڪڙو = {};
	most_تڪڙو.aircraft_dist = k_No_Conflict_مفاصلو;
}

فراغ ConflictTracker::refresh_most_تڪڙو()
{
	إذا (بفر.خالي_جايون()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_تڪڙو();
		أرجع;
	}

	most_تڪڙو = ڳوليو_most_تڪڙو();
}

conflict_info_s ConflictTracker::ڳوليو_most_تڪڙو() ثابت
{
	ثابت صحيح most_تڪڙو_conflict_idx = ڳوليو_conflict_idx_by_ترجيح(Conflict_ترجيح::k_Most_تڪڙو);

	إذا (!is_valid_idx(most_تڪڙو_conflict_idx)) {
		أرجع {};
	}

	أرجع بفر[most_تڪڙو_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ثابت DaaEncodedId &encoded_id) ثابت
{
	ثابت صحيح conflict_idx = ڳوليو_conflict_idx(encoded_id);

	إذا (!is_valid_idx(conflict_idx)) {
		أرجع {};
	}

	أرجع بفر[conflict_idx];
}

صحيح ConflictTracker::ڳوليو_conflict_idx_by_ترجيح(ثابت Conflict_ترجيح ترجيح) ثابت
{
	ثابت صحيح buff_سائيز = static_cast<صحيح>(بفر.سائيز());

	إذا (buff_سائيز < 1) {
		أرجع -1;
	}

	صحيح بهترين_idx = 0;

	لكل (صحيح i = 1; i < buff_سائيز; ++i) {
		ثابت منطقي is_better = (ترجيح == Conflict_ترجيح::k_Most_تڪڙو)
				       ? is_conflict_وڌيڪ_important(بفر[i], بفر[بهترين_idx])
				       : is_conflict_less_important(بفر[i], بفر[بهترين_idx]);

		إذا (is_better) {
			بهترين_idx = i;
		}
	}

	أرجع بهترين_idx;
}

صحيح ConflictTracker::ڳوليو_conflict_idx(ثابت DaaEncodedId &encoded_id) ثابت
{
	لكل (size_t i = 0; i < بفر.سائيز(); i++) {
		إذا (بفر[i].encoded_id == encoded_id) {
			أرجع static_cast<صحيح>(i);
		}
	}

	أرجع -1;
}

منطقي ConflictTracker::is_conflict_less_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &بنياد_conflict)
{
	إذا (new_conflict.conflict_level != بنياد_conflict.conflict_level) {
		أرجع new_conflict.conflict_level < بنياد_conflict.conflict_level;
	}

	أرجع new_conflict.aircraft_dist > بنياد_conflict.aircraft_dist;
}

منطقي ConflictTracker::is_conflict_وڌيڪ_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &بنياد_conflict)
{
	إذا (new_conflict.conflict_level != بنياد_conflict.conflict_level) {
		أرجع new_conflict.conflict_level > بنياد_conflict.conflict_level;
	}

	أرجع new_conflict.aircraft_dist < بنياد_conflict.aircraft_dist;
}
