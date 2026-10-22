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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <containers/Array.hpp>
#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/detect_and_avoid.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/transponder_report.h>

#ಅನ್ತರ್ಭಾವ "DaaEncodedId.h"

ಸ್ಥಿರ constexpr uint8_t kDaaMaxTraffic{5};

ರಚನೆ conflict_info_s {
	DaaEncodedId encoded_id{};
	hrt_abstime latest_update_timestamp{0};
	uint8_t conflict_level{detect_and_avoid_s::DAA_CONFLICT_LVL_NONE};
	ದಶಮಾಂಶ aircraft_dist{0.f}; // Distance to aircraft = sqrtf(dist_hor^2 + dist_vert^2)
};

ಎಣಿಕೆ class Remove_ಬಫರ್_Cause : uint8_t {
	kStaleConflict = 0,
	k_ಬಫರ್_ತುಂಬಿದ = 1
};

ಎಣಿಕೆ class IgnoreTrafficCause : uint8_t {
	k_ಬಫರ್_ತುಂಬಿದ = 0
};

ಎಣಿಕೆ class ConflictTrackerChangeType : uint8_t {
	kConflictAdded = 0,
	k_Conflict_Level_ಬದಲಾಯಿತು = 1,
	kConflictRemoved = 2,
	kReportIgnored = 3
};

ರಚನೆ conflict_tracker_change_s {
	ConflictTrackerChangeType type{ConflictTrackerChangeType::kConflictAdded};
	conflict_info_s conflict{};	// the affected entry (for kConflictRemoved: the removed entry)
	uint8_t ಹಿಂದಿನ_level{detect_and_avoid_s::DAA_CONFLICT_LVL_NONE};	// kConflictLevelChanged only
	Remove_ಬಫರ್_Cause remove_cause{Remove_ಬಫರ್_Cause::kStaleConflict};	// kConflictRemoved only
	IgnoreTrafficCause ignore_cause{IgnoreTrafficCause::k_ಬಫರ್_ತುಂಬಿದ};	// kReportIgnored only
	// Required because the caller processes the full Uorb queue before emitting warnings
	bool conflict_is_most_ತುರ್ತು{false};
};

// A stale sweep can remove the full buffer and each queued report can replace one entry.
ಸ್ಥಿರ constexpr uint16_t k_Max_Conflict_Changes_ಪ್ರತಿ_ಚಕ್ರ{kDaaMaxTraffic + 2 * transponder_report_s::ORB_ಸರತಿ_ಉದ್ದಳತೆ};

using conflict_tracker_changes_s = px4::Array<conflict_tracker_change_s, k_Max_Conflict_Changes_ಪ್ರತಿ_ಚಕ್ರ>;

class ConflictTracker
{
ಸಾರ್ವಜನಿಕ:
	ConflictTracker() { reset_most_ತುರ್ತು(); }
	~ConflictTracker() = ಪೂರ್ವನಿಯೋಜಿತ;

	/**
	 * @brief Apply one evaluated report to the tracked set.
	 *
	 * Inserts new conflicts (evicting the least urgent entry when the buffer is full),
	 * overwrites known conflicts, and drops known conflicts that report
	 * DAA_CONFLICT_LVL_NONE.
	 *
	 * Emitted changes are appended to @p changes. Returns true if the tracked set changed.
	 */
	bool apply_conflict(ಸ್ಥಿರಾಂಕ conflict_info_s &conflict, conflict_tracker_changes_s &changes);

	/**
	 * @brief Drop entries whose last update is older than @p timeout_us at time @p now.
	 *
	 * Emits one kConflictRemoved (kStaleConflict) change per dropped entry.
	 * Returns true if at least one entry was removed.
	 */
	bool remove_stale_conflicts(ಸ್ಥಿರಾಂಕ hrt_abstime now, ಸ್ಥಿರಾಂಕ hrt_abstime timeout_us, conflict_tracker_changes_s &changes);

	// Drop all conflicts and reset the most-urgent cache.
	ಶೂನ್ಯ clear();

	// Recompute the cached most-urgent conflict from the buffer.
	ಶೂನ್ಯ refresh_most_ತುರ್ತು();

	// Cached value; only refresh_most_urgent() and clear() update it.
	ಸ್ಥಿರಾಂಕ conflict_info_s &most_ತುರ್ತು_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ most_ತುರ್ತು; }

	// Highest-priority conflict in the buffer, or id=0 when empty.
	conflict_info_s ಕಂಡುಹಿಡಿ_most_ತುರ್ತು() ಸ್ಥಿರಾಂಕ;

	bool ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ DaaEncodedId &encoded_id) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_conflict_idx(encoded_id) >= 0; }

	// Tracked entry for encoded_id, or id=0 when not tracked.
	conflict_info_s get_conflict(ಸ್ಥಿರಾಂಕ DaaEncodedId &encoded_id) ಸ್ಥಿರಾಂಕ;

	size_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬಫರ್.ಗಾತ್ರ_2(); }
	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬಫರ್.ಖಾಲಿಗಳು(); }

	// Priority rules: higher conflict level first, ties broken by aircraft distance.
	// Public so the priority policy can be unit tested directly.
	ಸ್ಥಿರ bool is_conflict_ಇನ್ನಷ್ಟು_important(ಸ್ಥಿರಾಂಕ conflict_info_s &new_conflict, ಸ್ಥಿರಾಂಕ conflict_info_s &ತಳ_conflict);
	ಸ್ಥಿರ bool is_conflict_less_important(ಸ್ಥಿರಾಂಕ conflict_info_s &new_conflict, ಸ್ಥಿರಾಂಕ conflict_info_s &ತಳ_conflict);

ಖಾಸಗಿ:
	ಎಣಿಕೆ class Conflict_ಆದ್ಯತೆ : uint8_t {
		k_Most_ತುರ್ತು,
		k_ಕನಿಷ್ಠ_ತುರ್ತು
	};

	/** @brief Insert a new conflict, replacing the least urgent entry when the buffer is full. */
	bool ಕೂಡಿಸು_conflict(ಸ್ಥಿರಾಂಕ conflict_info_s &conflict, conflict_tracker_changes_s &changes);

	/**
	 * @brief Overwrite a tracked entry from a new report.
	 *
	 * If the level changed to DAA_CONFLICT_LVL_NONE, the conflict is removed form the buffer.
	 *
	 * Emits kConflictLevelChanged when the level differs from the stored entry.
	 */
	bool update_conflict(ಸ್ಥಿರಾಂಕ conflict_info_s &conflict, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ conflict_idx, conflict_tracker_changes_s &changes);

	/** @brief Return the index of the most or least urgent entry, or -1 if the buffer is empty. */
	ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_conflict_idx_by_ಆದ್ಯತೆ(ಸ್ಥಿರಾಂಕ Conflict_ಆದ್ಯತೆ ಆದ್ಯತೆ) ಸ್ಥಿರಾಂಕ;

	/** @brief Return the buffer index of @p encoded_id, or -1 if not tracked. */
	ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_conflict_idx(ಸ್ಥಿರಾಂಕ DaaEncodedId &encoded_id) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ reset_most_ತುರ್ತು();

	bool is_valid_idx(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ idx) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ idx >= 0 && idx < static_cast<ಪೂರ್ಣಾಂಕ>(ಬಫರ್.ಗಾತ್ರ_2()); }

	px4::Array<conflict_info_s, kDaaMaxTraffic> ಬಫರ್{};
	conflict_info_s most_ತುರ್ತು{};
};
