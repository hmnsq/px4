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

#निर्देशः once

#अन्तर्भाव <containers/Array.hpp>
#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <uORB/topics/detect_and_avoid.h>
#अन्तर्भाव <uORB/topics/transponder_report.h>

#अन्तर्भाव "DaaEncodedId.h"

स्थिर constexpr uint8_t kDaaMaxTraffic{5};

संरचना conflict_info_s {
	DaaEncodedId encoded_id{};
	hrt_abstime latest_update_timestamp{0};
	uint8_t conflict_level{detect_and_avoid_s::DAA_CONFLICT_LVL_NONE};
	भिन्न aircraft_dist{0.f}; // Distance to aircraft = sqrtf(dist_hor^2 + dist_vert^2)
};

गणना class Remove_बफर_Cause : uint8_t {
	kStaleConflict = 0,
	k_बफर_पूर्णम् = 1
};

गणना class IgnoreTrafficCause : uint8_t {
	k_बफर_पूर्णम् = 0
};

गणना class ConflictTrackerChangeType : uint8_t {
	kConflictAdded = 0,
	k_Conflict_Level_परिवर्तितम् = 1,
	kConflictRemoved = 2,
	kReportIgnored = 3
};

संरचना conflict_tracker_change_s {
	ConflictTrackerChangeType type{ConflictTrackerChangeType::kConflictAdded};
	conflict_info_s conflict{};	// the affected entry (for kConflictRemoved: the removed entry)
	uint8_t previous_level{detect_and_avoid_s::DAA_CONFLICT_LVL_NONE};	// kConflictLevelChanged only
	Remove_बफर_Cause remove_cause{Remove_बफर_Cause::kStaleConflict};	// kConflictRemoved only
	IgnoreTrafficCause ignore_cause{IgnoreTrafficCause::k_बफर_पूर्णम्};	// kReportIgnored only
	// Required because the caller processes the full Uorb queue before emitting warnings
	bool conflict_is_most_आत्ययिकम्{false};
};

// A stale sweep can remove the full buffer and each queued report can replace one entry.
स्थिर constexpr uint16_t k_Max_Conflict_Changes_प्रत्येकम्_आवृत्तिः{kDaaMaxTraffic + 2 * transponder_report_s::ORB_QUEUE_दीर्घता};

using conflict_tracker_changes_s = px4::Array<conflict_tracker_change_s, k_Max_Conflict_Changes_प्रत्येकम्_आवृत्तिः>;

class ConflictTracker
{
सार्वजनिक:
	ConflictTracker() { reset_most_आत्ययिकम्(); }
	~ConflictTracker() = स्वभाव;

	/**
	 * @brief Apply one evaluated report to the tracked set.
	 *
	 * Inserts new conflicts (evicting the least urgent entry when the buffer is full),
	 * overwrites known conflicts, and drops known conflicts that report
	 * DAA_CONFLICT_LVL_NONE.
	 *
	 * Emitted changes are appended to @p changes. Returns true if the tracked set changed.
	 */
	bool apply_conflict(ध्रुव conflict_info_s &conflict, conflict_tracker_changes_s &changes);

	/**
	 * @brief Drop entries whose last update is older than @p timeout_us at time @p now.
	 *
	 * Emits one kConflictRemoved (kStaleConflict) change per dropped entry.
	 * Returns true if at least one entry was removed.
	 */
	bool remove_stale_conflicts(ध्रुव hrt_abstime now, ध्रुव hrt_abstime timeout_us, conflict_tracker_changes_s &changes);

	// Drop all conflicts and reset the most-urgent cache.
	शून्य clear();

	// Recompute the cached most-urgent conflict from the buffer.
	शून्य refresh_most_आत्ययिकम्();

	// Cached value; only refresh_most_urgent() and clear() update it.
	ध्रुव conflict_info_s &most_आत्ययिकम्_2() ध्रुव { प्रत्यागम most_आत्ययिकम्; }

	// Highest-priority conflict in the buffer, or id=0 when empty.
	conflict_info_s find_most_आत्ययिकम्() ध्रुव;

	bool अन्तर्भवति(ध्रुव DaaEncodedId &encoded_id) ध्रुव { प्रत्यागम find_conflict_idx(encoded_id) >= 0; }

	// Tracked entry for encoded_id, or id=0 when not tracked.
	conflict_info_s get_conflict(ध्रुव DaaEncodedId &encoded_id) ध्रुव;

	size_t आकार() ध्रुव { प्रत्यागम बफर.आकार(); }
	bool रिक्तानि() ध्रुव { प्रत्यागम बफर.रिक्तानि(); }

	// Priority rules: higher conflict level first, ties broken by aircraft distance.
	// Public so the priority policy can be unit tested directly.
	स्थिर bool is_conflict_अधिकम्_important(ध्रुव conflict_info_s &new_conflict, ध्रुव conflict_info_s &आधारः_conflict);
	स्थिर bool is_conflict_less_important(ध्रुव conflict_info_s &new_conflict, ध्रुव conflict_info_s &आधारः_conflict);

गुप्त:
	गणना class Conflict_प्राथमिकता : uint8_t {
		k_Most_आत्ययिकम्,
		k_न्यूनतमम्_आत्ययिकम्
	};

	/** @brief Insert a new conflict, replacing the least urgent entry when the buffer is full. */
	bool सङ्कलय_conflict(ध्रुव conflict_info_s &conflict, conflict_tracker_changes_s &changes);

	/**
	 * @brief Overwrite a tracked entry from a new report.
	 *
	 * If the level changed to DAA_CONFLICT_LVL_NONE, the conflict is removed form the buffer.
	 *
	 * Emits kConflictLevelChanged when the level differs from the stored entry.
	 */
	bool update_conflict(ध्रुव conflict_info_s &conflict, ध्रुव पूर्णाङ्क conflict_idx, conflict_tracker_changes_s &changes);

	/** @brief Return the index of the most or least urgent entry, or -1 if the buffer is empty. */
	पूर्णाङ्क find_conflict_idx_by_प्राथमिकता(ध्रुव Conflict_प्राथमिकता प्राथमिकता) ध्रुव;

	/** @brief Return the buffer index of @p encoded_id, or -1 if not tracked. */
	पूर्णाङ्क find_conflict_idx(ध्रुव DaaEncodedId &encoded_id) ध्रुव;

	शून्य reset_most_आत्ययिकम्();

	bool is_valid_idx(ध्रुव पूर्णाङ्क idx) ध्रुव { प्रत्यागम idx >= 0 && idx < static_cast<पूर्णाङ्क>(बफर.आकार()); }

	px4::Array<conflict_info_s, kDaaMaxTraffic> बफर{};
	conflict_info_s most_आत्ययिकम्{};
};
