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

#సూచన once

#అన్తర్భావ <containers/Array.hpp>
#అన్తర్భావ <drivers/drv_hrt.h>
#అన్తర్భావ <uORB/topics/detect_and_avoid.h>
#అన్తర్భావ <uORB/topics/transponder_report.h>

#అన్తర్భావ "DaaEncodedId.h"

స్థిర constexpr uint8_t kDaaMaxTraffic{5};

నిర్మాణం conflict_info_s {
	DaaEncodedId encoded_id{};
	hrt_abstime latest_update_timestamp{0};
	uint8_t conflict_level{detect_and_avoid_s::DAA_CONFLICT_LVL_NONE};
	దశాంశం aircraft_dist{0.f}; // Distance to aircraft = sqrtf(dist_hor^2 + dist_vert^2)
};

గణన class Remove_బఫరు_Cause : uint8_t {
	kStaleConflict = 0,
	k_బఫరు_నిండిన = 1
};

గణన class IgnoreTrafficCause : uint8_t {
	k_బఫరు_నిండిన = 0
};

గణన class ConflictTrackerChangeType : uint8_t {
	kConflictAdded = 0,
	k_Conflict_Level_మారింది = 1,
	kConflictRemoved = 2,
	kReportIgnored = 3
};

నిర్మాణం conflict_tracker_change_s {
	ConflictTrackerChangeType type{ConflictTrackerChangeType::kConflictAdded};
	conflict_info_s conflict{};	// the affected entry (for kConflictRemoved: the removed entry)
	uint8_t మునుపటి_level{detect_and_avoid_s::DAA_CONFLICT_LVL_NONE};	// kConflictLevelChanged only
	Remove_బఫరు_Cause remove_cause{Remove_బఫరు_Cause::kStaleConflict};	// kConflictRemoved only
	IgnoreTrafficCause ignore_cause{IgnoreTrafficCause::k_బఫరు_నిండిన};	// kReportIgnored only
	// Required because the caller processes the full Uorb queue before emitting warnings
	bool conflict_is_most_అత్యవసర{false};
};

// A stale sweep can remove the full buffer and each queued report can replace one entry.
స్థిర constexpr uint16_t k_Max_Conflict_Changes_చొప్పున_చక్రం{kDaaMaxTraffic + 2 * transponder_report_s::ORB_వరుస_నిడివి};

using conflict_tracker_changes_s = px4::Array<conflict_tracker_change_s, k_Max_Conflict_Changes_చొప్పున_చక్రం>;

class ConflictTracker
{
బహిరంగ:
	ConflictTracker() { reset_most_అత్యవసర(); }
	~ConflictTracker() = అప్రమేయ;

	/**
	 * @brief Apply one evaluated report to the tracked set.
	 *
	 * Inserts new conflicts (evicting the least urgent entry when the buffer is full),
	 * overwrites known conflicts, and drops known conflicts that report
	 * DAA_CONFLICT_LVL_NONE.
	 *
	 * Emitted changes are appended to @p changes. Returns true if the tracked set changed.
	 */
	bool apply_conflict(స్థిరరాశి conflict_info_s &conflict, conflict_tracker_changes_s &changes);

	/**
	 * @brief Drop entries whose last update is older than @p timeout_us at time @p now.
	 *
	 * Emits one kConflictRemoved (kStaleConflict) change per dropped entry.
	 * Returns true if at least one entry was removed.
	 */
	bool remove_stale_conflicts(స్థిరరాశి hrt_abstime now, స్థిరరాశి hrt_abstime timeout_us, conflict_tracker_changes_s &changes);

	// Drop all conflicts and reset the most-urgent cache.
	శూన్య clear();

	// Recompute the cached most-urgent conflict from the buffer.
	శూన్య refresh_most_అత్యవసర();

	// Cached value; only refresh_most_urgent() and clear() update it.
	స్థిరరాశి conflict_info_s &most_అత్యవసర_2() స్థిరరాశి { తిరిగి most_అత్యవసర; }

	// Highest-priority conflict in the buffer, or id=0 when empty.
	conflict_info_s కనుగొను_most_అత్యవసర() స్థిరరాశి;

	bool కలిగియుంది(స్థిరరాశి DaaEncodedId &encoded_id) స్థిరరాశి { తిరిగి కనుగొను_conflict_idx(encoded_id) >= 0; }

	// Tracked entry for encoded_id, or id=0 when not tracked.
	conflict_info_s get_conflict(స్థిరరాశి DaaEncodedId &encoded_id) స్థిరరాశి;

	size_t కొలత() స్థిరరాశి { తిరిగి బఫరు.కొలత(); }
	bool ఖాళీలు() స్థిరరాశి { తిరిగి బఫరు.ఖాళీలు(); }

	// Priority rules: higher conflict level first, ties broken by aircraft distance.
	// Public so the priority policy can be unit tested directly.
	స్థిర bool is_conflict_మరిన్ని_important(స్థిరరాశి conflict_info_s &new_conflict, స్థిరరాశి conflict_info_s &పునాది_conflict);
	స్థిర bool is_conflict_less_important(స్థిరరాశి conflict_info_s &new_conflict, స్థిరరాశి conflict_info_s &పునాది_conflict);

అంతరంగ:
	గణన class Conflict_ప్రాధాన్యత : uint8_t {
		k_Most_అత్యవసర,
		k_కనిష్ఠ_అత్యవసర
	};

	/** @brief Insert a new conflict, replacing the least urgent entry when the buffer is full. */
	bool కూడు_conflict(స్థిరరాశి conflict_info_s &conflict, conflict_tracker_changes_s &changes);

	/**
	 * @brief Overwrite a tracked entry from a new report.
	 *
	 * If the level changed to DAA_CONFLICT_LVL_NONE, the conflict is removed form the buffer.
	 *
	 * Emits kConflictLevelChanged when the level differs from the stored entry.
	 */
	bool update_conflict(స్థిరరాశి conflict_info_s &conflict, స్థిరరాశి పూర్ణాంకం conflict_idx, conflict_tracker_changes_s &changes);

	/** @brief Return the index of the most or least urgent entry, or -1 if the buffer is empty. */
	పూర్ణాంకం కనుగొను_conflict_idx_by_ప్రాధాన్యత(స్థిరరాశి Conflict_ప్రాధాన్యత ప్రాధాన్యత) స్థిరరాశి;

	/** @brief Return the buffer index of @p encoded_id, or -1 if not tracked. */
	పూర్ణాంకం కనుగొను_conflict_idx(స్థిరరాశి DaaEncodedId &encoded_id) స్థిరరాశి;

	శూన్య reset_most_అత్యవసర();

	bool is_valid_idx(స్థిరరాశి పూర్ణాంకం idx) స్థిరరాశి { తిరిగి idx >= 0 && idx < static_cast<పూర్ణాంకం>(బఫరు.కొలత()); }

	px4::Array<conflict_info_s, kDaaMaxTraffic> బఫరు{};
	conflict_info_s most_అత్యవసర{};
};
