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
 * @file ConflictTracker.h
 *
 * Fixed-size buffer of active DAA conflicts.
 *
 * Buffer changes that may require a user-facing message are reported as
 * conflict_tracker_change_s entries so the caller can apply its own
 * notification policy.
 *
 * @author Jonas Perolini <jonspero@me.com>
 */

#ہدایت once

#شامل <containers/Array.hpp>
#شامل <drivers/drv_hrt.h>
#شامل <uORB/topics/detect_and_avoid.h>
#شامل <uORB/topics/transponder_report.h>

#شامل "DaaEncodedId.h"

ساکن constexpr uint8_t kDaaMaxTraffic{5};

ساخت conflict_info_s {
	DaaEncodedId encoded_id{};
	hrt_abstime latest_update_timestamp{0};
	uint8_t conflict_level{detect_and_avoid_s::DAA_CONFLICT_LVL_NONE};
	اعشاری aircraft_dist{0.f}; // Distance to aircraft = sqrtf(dist_hor^2 + dist_vert^2)
};

شمار class Remove_بفر_Cause : uint8_t {
	kStaleConflict = 0,
	k_بفر_بھرا = 1
};

شمار class IgnoreTrafficCause : uint8_t {
	k_بفر_بھرا = 0
};

شمار class ConflictTrackerChangeType : uint8_t {
	kConflictAdded = 0,
	k_Conflict_Level_بدلا = 1,
	kConflictRemoved = 2,
	kReportIgnored = 3
};

ساخت conflict_tracker_change_s {
	ConflictTrackerChangeType type{ConflictTrackerChangeType::kConflictAdded};
	conflict_info_s conflict{};	// the affected entry (for kConflictRemoved: the removed entry)
	uint8_t پچھلا_level{detect_and_avoid_s::DAA_CONFLICT_LVL_NONE};	// kConflictLevelChanged only
	Remove_بفر_Cause remove_cause{Remove_بفر_Cause::kStaleConflict};	// kConflictRemoved only
	IgnoreTrafficCause ignore_cause{IgnoreTrafficCause::k_بفر_بھرا};	// kReportIgnored only
	// Required because the caller processes the full Uorb queue before emitting warnings
	bool conflict_is_most_ہنگامی{false};
};

// A stale sweep can remove the full buffer and each queued report can replace one entry.
ساکن constexpr uint16_t k_Max_Conflict_Changes_فی_چکر{kDaaMaxTraffic + 2 * transponder_report_s::ORB_قطار_لمبائی};

using conflict_tracker_changes_s = px4::Array<conflict_tracker_change_s, k_Max_Conflict_Changes_فی_چکر>;

class ConflictTracker
{
عوامی:
	ConflictTracker() { reset_most_ہنگامی(); }
	~ConflictTracker() = طےشدہ;

	/**
	 * @brief Apply one evaluated report to the tracked set.
	 *
	 * Inserts new conflicts (evicting the least urgent entry when the buffer is full),
	 * overwrites known conflicts, and drops known conflicts that report
	 * DAA_CONFLICT_LVL_NONE.
	 *
	 * Emitted changes are appended to @p changes. Returns true if the tracked set changed.
	 */
	bool apply_conflict(ثابت conflict_info_s &conflict, conflict_tracker_changes_s &changes);

	/**
	 * @brief Drop entries whose last update is older than @p timeout_us at time @p now.
	 *
	 * Emits one kConflictRemoved (kStaleConflict) change per dropped entry.
	 * Returns true if at least one entry was removed.
	 */
	bool remove_stale_conflicts(ثابت hrt_abstime now, ثابت hrt_abstime timeout_us, conflict_tracker_changes_s &changes);

	// Drop all conflicts and reset the most-urgent cache.
	خالی clear();

	// Recompute the cached most-urgent conflict from the buffer.
	خالی refresh_most_ہنگامی();

	// Cached value; only refresh_most_urgent() and clear() update it.
	ثابت conflict_info_s &most_ہنگامی_2() ثابت { واپس most_ہنگامی; }

	// Highest-priority conflict in the buffer, or id=0 when empty.
	conflict_info_s ڈھونڈو_most_ہنگامی() ثابت;

	bool شامل_ہے(ثابت DaaEncodedId &encoded_id) ثابت { واپس ڈھونڈو_conflict_idx(encoded_id) >= 0; }

	// Tracked entry for encoded_id, or id=0 when not tracked.
	conflict_info_s get_conflict(ثابت DaaEncodedId &encoded_id) ثابت;

	size_t سائز() ثابت { واپس بفر.سائز(); }
	bool خالی_خانے() ثابت { واپس بفر.خالی_خانے(); }

	// Priority rules: higher conflict level first, ties broken by aircraft distance.
	// Public so the priority policy can be unit tested directly.
	ساکن bool is_conflict_مزید_important(ثابت conflict_info_s &new_conflict, ثابت conflict_info_s &بنیاد_conflict);
	ساکن bool is_conflict_less_important(ثابت conflict_info_s &new_conflict, ثابت conflict_info_s &بنیاد_conflict);

نجی:
	شمار class Conflict_ترجیح : uint8_t {
		k_Most_ہنگامی,
		k_کم_سے_کم_ہنگامی
	};

	/** @brief Insert a new conflict, replacing the least urgent entry when the buffer is full. */
	bool جوڑو_conflict(ثابت conflict_info_s &conflict, conflict_tracker_changes_s &changes);

	/**
	 * @brief Overwrite a tracked entry from a new report.
	 *
	 * If the level changed to DAA_CONFLICT_LVL_NONE, the conflict is removed form the buffer.
	 *
	 * Emits kConflictLevelChanged when the level differs from the stored entry.
	 */
	bool update_conflict(ثابت conflict_info_s &conflict, ثابت صحیح conflict_idx, conflict_tracker_changes_s &changes);

	/** @brief Return the index of the most or least urgent entry, or -1 if the buffer is empty. */
	صحیح ڈھونڈو_conflict_idx_by_ترجیح(ثابت Conflict_ترجیح ترجیح) ثابت;

	/** @brief Return the buffer index of @p encoded_id, or -1 if not tracked. */
	صحیح ڈھونڈو_conflict_idx(ثابت DaaEncodedId &encoded_id) ثابت;

	خالی reset_most_ہنگامی();

	bool is_valid_idx(ثابت صحیح idx) ثابت { واپس idx >= 0 && idx < static_cast<صحیح>(بفر.سائز()); }

	px4::Array<conflict_info_s, kDaaMaxTraffic> بفر{};
	conflict_info_s most_ہنگامی{};
};
