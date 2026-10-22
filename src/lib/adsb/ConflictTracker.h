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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <containers/Array.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/detect_and_avoid.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/transponder_report.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "DaaEncodedId.h"

ਸਥਿਰ constexpr uint8_t kDaaMaxTraffic{5};

ਬਣਤਰ conflict_info_s {
	DaaEncodedId encoded_id{};
	hrt_abstime latest_update_timestamp{0};
	uint8_t conflict_level{detect_and_avoid_s::DAA_CONFLICT_LVL_NONE};
	ਦਸ਼ਮਲਵ aircraft_dist{0.f}; // Distance to aircraft = sqrtf(dist_hor^2 + dist_vert^2)
};

ਗਿਣਤੀ class Remove_ਬਫਰ_Cause : uint8_t {
	kStaleConflict = 0,
	k_ਬਫਰ_ਭਰਿਆ = 1
};

ਗਿਣਤੀ class IgnoreTrafficCause : uint8_t {
	k_ਬਫਰ_ਭਰਿਆ = 0
};

ਗਿਣਤੀ class ConflictTrackerChangeType : uint8_t {
	kConflictAdded = 0,
	k_Conflict_Level_ਬਦਲਿਆ = 1,
	kConflictRemoved = 2,
	kReportIgnored = 3
};

ਬਣਤਰ conflict_tracker_change_s {
	ConflictTrackerChangeType type{ConflictTrackerChangeType::kConflictAdded};
	conflict_info_s conflict{};	// the affected entry (for kConflictRemoved: the removed entry)
	uint8_t ਪਿਛਲਾ_level{detect_and_avoid_s::DAA_CONFLICT_LVL_NONE};	// kConflictLevelChanged only
	Remove_ਬਫਰ_Cause remove_cause{Remove_ਬਫਰ_Cause::kStaleConflict};	// kConflictRemoved only
	IgnoreTrafficCause ignore_cause{IgnoreTrafficCause::k_ਬਫਰ_ਭਰਿਆ};	// kReportIgnored only
	// Required because the caller processes the full Uorb queue before emitting warnings
	bool conflict_is_most_ਜ਼ਰੂਰੀ{false};
};

// A stale sweep can remove the full buffer and each queued report can replace one entry.
ਸਥਿਰ constexpr uint16_t k_Max_Conflict_Changes_ਪ੍ਰਤੀ_ਚੱਕਰ{kDaaMaxTraffic + 2 * transponder_report_s::ORB_ਕਤਾਰ_ਲੰਮਾਈ};

using conflict_tracker_changes_s = px4::Array<conflict_tracker_change_s, k_Max_Conflict_Changes_ਪ੍ਰਤੀ_ਚੱਕਰ>;

class ConflictTracker
{
ਜਨਤਕ:
	ConflictTracker() { reset_most_ਜ਼ਰੂਰੀ(); }
	~ConflictTracker() = ਮੂਲ;

	/**
	 * @brief Apply one evaluated report to the tracked set.
	 *
	 * Inserts new conflicts (evicting the least urgent entry when the buffer is full),
	 * overwrites known conflicts, and drops known conflicts that report
	 * DAA_CONFLICT_LVL_NONE.
	 *
	 * Emitted changes are appended to @p changes. Returns true if the tracked set changed.
	 */
	bool apply_conflict(ਅਚਲ conflict_info_s &conflict, conflict_tracker_changes_s &changes);

	/**
	 * @brief Drop entries whose last update is older than @p timeout_us at time @p now.
	 *
	 * Emits one kConflictRemoved (kStaleConflict) change per dropped entry.
	 * Returns true if at least one entry was removed.
	 */
	bool remove_stale_conflicts(ਅਚਲ hrt_abstime now, ਅਚਲ hrt_abstime timeout_us, conflict_tracker_changes_s &changes);

	// Drop all conflicts and reset the most-urgent cache.
	ਖਾਲੀ clear();

	// Recompute the cached most-urgent conflict from the buffer.
	ਖਾਲੀ refresh_most_ਜ਼ਰੂਰੀ();

	// Cached value; only refresh_most_urgent() and clear() update it.
	ਅਚਲ conflict_info_s &most_ਜ਼ਰੂਰੀ_2() ਅਚਲ { ਵਾਪਸ most_ਜ਼ਰੂਰੀ; }

	// Highest-priority conflict in the buffer, or id=0 when empty.
	conflict_info_s ਲੱਭੋ_most_ਜ਼ਰੂਰੀ() ਅਚਲ;

	bool ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ DaaEncodedId &encoded_id) ਅਚਲ { ਵਾਪਸ ਲੱਭੋ_conflict_idx(encoded_id) >= 0; }

	// Tracked entry for encoded_id, or id=0 when not tracked.
	conflict_info_s get_conflict(ਅਚਲ DaaEncodedId &encoded_id) ਅਚਲ;

	size_t ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਬਫਰ.ਮਾਪ(); }
	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ ਬਫਰ.ਖਾਲੀ_ਥਾਵਾਂ(); }

	// Priority rules: higher conflict level first, ties broken by aircraft distance.
	// Public so the priority policy can be unit tested directly.
	ਸਥਿਰ bool is_conflict_ਹੋਰ_important(ਅਚਲ conflict_info_s &new_conflict, ਅਚਲ conflict_info_s &ਅਧਾਰ_conflict);
	ਸਥਿਰ bool is_conflict_less_important(ਅਚਲ conflict_info_s &new_conflict, ਅਚਲ conflict_info_s &ਅਧਾਰ_conflict);

ਨਿਜੀ:
	ਗਿਣਤੀ class Conflict_ਤਰਜੀਹ : uint8_t {
		k_Most_ਜ਼ਰੂਰੀ,
		k_ਘੱਟ_ਤੋਂ_ਘੱਟ_ਜ਼ਰੂਰੀ
	};

	/** @brief Insert a new conflict, replacing the least urgent entry when the buffer is full. */
	bool ਜੋੜੋ_conflict(ਅਚਲ conflict_info_s &conflict, conflict_tracker_changes_s &changes);

	/**
	 * @brief Overwrite a tracked entry from a new report.
	 *
	 * If the level changed to DAA_CONFLICT_LVL_NONE, the conflict is removed form the buffer.
	 *
	 * Emits kConflictLevelChanged when the level differs from the stored entry.
	 */
	bool update_conflict(ਅਚਲ conflict_info_s &conflict, ਅਚਲ ਪੂਰਨ conflict_idx, conflict_tracker_changes_s &changes);

	/** @brief Return the index of the most or least urgent entry, or -1 if the buffer is empty. */
	ਪੂਰਨ ਲੱਭੋ_conflict_idx_by_ਤਰਜੀਹ(ਅਚਲ Conflict_ਤਰਜੀਹ ਤਰਜੀਹ) ਅਚਲ;

	/** @brief Return the buffer index of @p encoded_id, or -1 if not tracked. */
	ਪੂਰਨ ਲੱਭੋ_conflict_idx(ਅਚਲ DaaEncodedId &encoded_id) ਅਚਲ;

	ਖਾਲੀ reset_most_ਜ਼ਰੂਰੀ();

	bool is_valid_idx(ਅਚਲ ਪੂਰਨ idx) ਅਚਲ { ਵਾਪਸ idx >= 0 && idx < static_cast<ਪੂਰਨ>(ਬਫਰ.ਮਾਪ()); }

	px4::Array<conflict_info_s, kDaaMaxTraffic> ਬਫਰ{};
	conflict_info_s most_ਜ਼ਰੂਰੀ{};
};
