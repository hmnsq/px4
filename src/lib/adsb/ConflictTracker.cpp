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

#شامل "ConflictTracker.h"

#شامل <px4_platform_common/defines.h>
#شامل <px4_platform_common/log.h>

نامخلا
{

خالی append_change(conflict_tracker_changes_s &changes, ثابت conflict_tracker_change_s &change)
{
	اگر (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

خالی ریکارڈ_ignored(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

خالی ریکارڈ_removed(conflict_tracker_changes_s &changes, ثابت conflict_info_s &conflict, ثابت Remove_بفر_Cause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

منطقی ConflictTracker::apply_conflict(ثابت conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	اگر (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		واپس جھوٹ;
	}

	ثابت صحیح conflict_idx = ڈھونڈو_conflict_idx(conflict.encoded_id);

	اگر (conflict_idx < 0) {
		اگر (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			واپس جھوٹ;
		}

		واپس جوڑو_conflict(conflict, changes);
	}

	واپس update_conflict(conflict, conflict_idx, changes);
}

منطقی ConflictTracker::جوڑو_conflict(ثابت conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	اگر (بفر.سائز() < بفر.max_سائز()) {
		بفر.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		واپس سچ;
	}

	ثابت صحیح کم_سے_کم_ہنگامی_conflict_idx = ڈھونڈو_conflict_idx_by_ترجیح(Conflict_ترجیح::k_کم_سے_کم_ہنگامی);

	اگر (!is_conflict_مزید_important(conflict, بفر[کم_سے_کم_ہنگامی_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		ریکارڈ_ignored(changes, conflict, IgnoreTrafficCause::k_بفر_بھرا);
		واپس جھوٹ;
	}

	ثابت conflict_info_s removed_conflict = بفر[کم_سے_کم_ہنگامی_conflict_idx];
	بفر[کم_سے_کم_ہنگامی_conflict_idx] = conflict;
	ریکارڈ_removed(changes, removed_conflict, Remove_بفر_Cause::k_بفر_بھرا);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	واپس سچ;
}

منطقی ConflictTracker::update_conflict(ثابت conflict_info_s &conflict, ثابت صحیح conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ثابت uint8_t پچھلا_conflict_level = بفر[conflict_idx].conflict_level;

	اگر (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		بفر.remove(conflict_idx);

	} ورنہ {
		بفر[conflict_idx] = conflict;
	}

	اگر (conflict.conflict_level != پچھلا_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::k_Conflict_Level_بدلا;
		change.conflict = conflict;
		change.پچھلا_level = پچھلا_conflict_level;

		ثابت صحیح most_ہنگامی_idx = ڈھونڈو_conflict_idx_by_ترجیح(Conflict_ترجیح::k_Most_ہنگامی);
		change.conflict_is_most_ہنگامی = (most_ہنگامی.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_ہنگامی_idx)
						     && بفر[most_ہنگامی_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	واپس سچ;
}

منطقی ConflictTracker::remove_stale_conflicts(ثابت hrt_abstime now, ثابت hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	صحیح nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	برائے (صحیح idx = (static_cast<صحیح>(بفر.سائز()) - 1); idx >= 0; --idx) {
		ثابت hrt_abstime latest_update = بفر[idx].latest_update_timestamp;

		اگر (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ثابت conflict_info_s removed_conflict = بفر[idx];
			بفر.remove(idx);
			ریکارڈ_removed(changes, removed_conflict, Remove_بفر_Cause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	اگر (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	واپس nb_conflicts_removed > 0;
}

خالی ConflictTracker::clear()
{
	بفر.clear();
	reset_most_ہنگامی();
}

خالی ConflictTracker::reset_most_ہنگامی()
{
	ساکن constexpr اعشاری k_No_Conflict_فاصلہ{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	most_ہنگامی = {};
	most_ہنگامی.aircraft_dist = k_No_Conflict_فاصلہ;
}

خالی ConflictTracker::refresh_most_ہنگامی()
{
	اگر (بفر.خالی_خانے()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_ہنگامی();
		واپس;
	}

	most_ہنگامی = ڈھونڈو_most_ہنگامی();
}

conflict_info_s ConflictTracker::ڈھونڈو_most_ہنگامی() ثابت
{
	ثابت صحیح most_ہنگامی_conflict_idx = ڈھونڈو_conflict_idx_by_ترجیح(Conflict_ترجیح::k_Most_ہنگامی);

	اگر (!is_valid_idx(most_ہنگامی_conflict_idx)) {
		واپس {};
	}

	واپس بفر[most_ہنگامی_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ثابت DaaEncodedId &encoded_id) ثابت
{
	ثابت صحیح conflict_idx = ڈھونڈو_conflict_idx(encoded_id);

	اگر (!is_valid_idx(conflict_idx)) {
		واپس {};
	}

	واپس بفر[conflict_idx];
}

صحیح ConflictTracker::ڈھونڈو_conflict_idx_by_ترجیح(ثابت Conflict_ترجیح ترجیح) ثابت
{
	ثابت صحیح buff_سائز = static_cast<صحیح>(بفر.سائز());

	اگر (buff_سائز < 1) {
		واپس -1;
	}

	صحیح بہترین_idx = 0;

	برائے (صحیح i = 1; i < buff_سائز; ++i) {
		ثابت منطقی is_better = (ترجیح == Conflict_ترجیح::k_Most_ہنگامی)
				       ? is_conflict_مزید_important(بفر[i], بفر[بہترین_idx])
				       : is_conflict_less_important(بفر[i], بفر[بہترین_idx]);

		اگر (is_better) {
			بہترین_idx = i;
		}
	}

	واپس بہترین_idx;
}

صحیح ConflictTracker::ڈھونڈو_conflict_idx(ثابت DaaEncodedId &encoded_id) ثابت
{
	برائے (size_t i = 0; i < بفر.سائز(); i++) {
		اگر (بفر[i].encoded_id == encoded_id) {
			واپس static_cast<صحیح>(i);
		}
	}

	واپس -1;
}

منطقی ConflictTracker::is_conflict_less_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &بنیاد_conflict)
{
	اگر (new_conflict.conflict_level != بنیاد_conflict.conflict_level) {
		واپس new_conflict.conflict_level < بنیاد_conflict.conflict_level;
	}

	واپس new_conflict.aircraft_dist > بنیاد_conflict.aircraft_dist;
}

منطقی ConflictTracker::is_conflict_مزید_important(ثابت conflict_info_s &new_conflict,
		ثابت conflict_info_s &بنیاد_conflict)
{
	اگر (new_conflict.conflict_level != بنیاد_conflict.conflict_level) {
		واپس new_conflict.conflict_level > بنیاد_conflict.conflict_level;
	}

	واپس new_conflict.aircraft_dist < بنیاد_conflict.aircraft_dist;
}
