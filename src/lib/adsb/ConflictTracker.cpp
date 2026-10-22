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

#అన్తర్భావ "ConflictTracker.h"

#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <px4_platform_common/log.h>

నామాకాశ
{

శూన్య append_change(conflict_tracker_changes_s &changes, స్థిరరాశి conflict_tracker_change_s &change)
{
	ఒకవేళ (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

శూన్య రికార్డు_ignored(conflict_tracker_changes_s &changes, స్థిరరాశి conflict_info_s &conflict, స్థిరరాశి IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

శూన్య రికార్డు_removed(conflict_tracker_changes_s &changes, స్థిరరాశి conflict_info_s &conflict, స్థిరరాశి Remove_బఫరు_Cause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

తార్కిక ConflictTracker::apply_conflict(స్థిరరాశి conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ఒకవేళ (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		తిరిగి అబద్ధం;
	}

	స్థిరరాశి పూర్ణాంకం conflict_idx = కనుగొను_conflict_idx(conflict.encoded_id);

	ఒకవేళ (conflict_idx < 0) {
		ఒకవేళ (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			తిరిగి అబద్ధం;
		}

		తిరిగి కూడు_conflict(conflict, changes);
	}

	తిరిగి update_conflict(conflict, conflict_idx, changes);
}

తార్కిక ConflictTracker::కూడు_conflict(స్థిరరాశి conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ఒకవేళ (బఫరు.కొలత() < బఫరు.max_కొలత()) {
		బఫరు.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		తిరిగి నిజం;
	}

	స్థిరరాశి పూర్ణాంకం కనిష్ఠ_అత్యవసర_conflict_idx = కనుగొను_conflict_idx_by_ప్రాధాన్యత(Conflict_ప్రాధాన్యత::k_కనిష్ఠ_అత్యవసర);

	ఒకవేళ (!is_conflict_మరిన్ని_important(conflict, బఫరు[కనిష్ఠ_అత్యవసర_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		రికార్డు_ignored(changes, conflict, IgnoreTrafficCause::k_బఫరు_నిండిన);
		తిరిగి అబద్ధం;
	}

	స్థిరరాశి conflict_info_s removed_conflict = బఫరు[కనిష్ఠ_అత్యవసర_conflict_idx];
	బఫరు[కనిష్ఠ_అత్యవసర_conflict_idx] = conflict;
	రికార్డు_removed(changes, removed_conflict, Remove_బఫరు_Cause::k_బఫరు_నిండిన);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	తిరిగి నిజం;
}

తార్కిక ConflictTracker::update_conflict(స్థిరరాశి conflict_info_s &conflict, స్థిరరాశి పూర్ణాంకం conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	స్థిరరాశి uint8_t మునుపటి_conflict_level = బఫరు[conflict_idx].conflict_level;

	ఒకవేళ (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		బఫరు.remove(conflict_idx);

	} లేకపోతే {
		బఫరు[conflict_idx] = conflict;
	}

	ఒకవేళ (conflict.conflict_level != మునుపటి_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::k_Conflict_Level_మారింది;
		change.conflict = conflict;
		change.మునుపటి_level = మునుపటి_conflict_level;

		స్థిరరాశి పూర్ణాంకం most_అత్యవసర_idx = కనుగొను_conflict_idx_by_ప్రాధాన్యత(Conflict_ప్రాధాన్యత::k_Most_అత్యవసర);
		change.conflict_is_most_అత్యవసర = (most_అత్యవసర.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_అత్యవసర_idx)
						     && బఫరు[most_అత్యవసర_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	తిరిగి నిజం;
}

తార్కిక ConflictTracker::remove_stale_conflicts(స్థిరరాశి hrt_abstime now, స్థిరరాశి hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	పూర్ణాంకం nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	కొరకు (పూర్ణాంకం idx = (static_cast<పూర్ణాంకం>(బఫరు.కొలత()) - 1); idx >= 0; --idx) {
		స్థిరరాశి hrt_abstime latest_update = బఫరు[idx].latest_update_timestamp;

		ఒకవేళ (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			స్థిరరాశి conflict_info_s removed_conflict = బఫరు[idx];
			బఫరు.remove(idx);
			రికార్డు_removed(changes, removed_conflict, Remove_బఫరు_Cause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	ఒకవేళ (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	తిరిగి nb_conflicts_removed > 0;
}

శూన్య ConflictTracker::clear()
{
	బఫరు.clear();
	reset_most_అత్యవసర();
}

శూన్య ConflictTracker::reset_most_అత్యవసర()
{
	స్థిర constexpr దశాంశం k_No_Conflict_దూరం{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	most_అత్యవసర = {};
	most_అత్యవసర.aircraft_dist = k_No_Conflict_దూరం;
}

శూన్య ConflictTracker::refresh_most_అత్యవసర()
{
	ఒకవేళ (బఫరు.ఖాళీలు()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_అత్యవసర();
		తిరిగి;
	}

	most_అత్యవసర = కనుగొను_most_అత్యవసర();
}

conflict_info_s ConflictTracker::కనుగొను_most_అత్యవసర() స్థిరరాశి
{
	స్థిరరాశి పూర్ణాంకం most_అత్యవసర_conflict_idx = కనుగొను_conflict_idx_by_ప్రాధాన్యత(Conflict_ప్రాధాన్యత::k_Most_అత్యవసర);

	ఒకవేళ (!is_valid_idx(most_అత్యవసర_conflict_idx)) {
		తిరిగి {};
	}

	తిరిగి బఫరు[most_అత్యవసర_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(స్థిరరాశి DaaEncodedId &encoded_id) స్థిరరాశి
{
	స్థిరరాశి పూర్ణాంకం conflict_idx = కనుగొను_conflict_idx(encoded_id);

	ఒకవేళ (!is_valid_idx(conflict_idx)) {
		తిరిగి {};
	}

	తిరిగి బఫరు[conflict_idx];
}

పూర్ణాంకం ConflictTracker::కనుగొను_conflict_idx_by_ప్రాధాన్యత(స్థిరరాశి Conflict_ప్రాధాన్యత ప్రాధాన్యత) స్థిరరాశి
{
	స్థిరరాశి పూర్ణాంకం buff_కొలత = static_cast<పూర్ణాంకం>(బఫరు.కొలత());

	ఒకవేళ (buff_కొలత < 1) {
		తిరిగి -1;
	}

	పూర్ణాంకం ఉత్తమ_idx = 0;

	కొరకు (పూర్ణాంకం i = 1; i < buff_కొలత; ++i) {
		స్థిరరాశి తార్కిక is_better = (ప్రాధాన్యత == Conflict_ప్రాధాన్యత::k_Most_అత్యవసర)
				       ? is_conflict_మరిన్ని_important(బఫరు[i], బఫరు[ఉత్తమ_idx])
				       : is_conflict_less_important(బఫరు[i], బఫరు[ఉత్తమ_idx]);

		ఒకవేళ (is_better) {
			ఉత్తమ_idx = i;
		}
	}

	తిరిగి ఉత్తమ_idx;
}

పూర్ణాంకం ConflictTracker::కనుగొను_conflict_idx(స్థిరరాశి DaaEncodedId &encoded_id) స్థిరరాశి
{
	కొరకు (size_t i = 0; i < బఫరు.కొలత(); i++) {
		ఒకవేళ (బఫరు[i].encoded_id == encoded_id) {
			తిరిగి static_cast<పూర్ణాంకం>(i);
		}
	}

	తిరిగి -1;
}

తార్కిక ConflictTracker::is_conflict_less_important(స్థిరరాశి conflict_info_s &new_conflict,
		స్థిరరాశి conflict_info_s &పునాది_conflict)
{
	ఒకవేళ (new_conflict.conflict_level != పునాది_conflict.conflict_level) {
		తిరిగి new_conflict.conflict_level < పునాది_conflict.conflict_level;
	}

	తిరిగి new_conflict.aircraft_dist > పునాది_conflict.aircraft_dist;
}

తార్కిక ConflictTracker::is_conflict_మరిన్ని_important(స్థిరరాశి conflict_info_s &new_conflict,
		స్థిరరాశి conflict_info_s &పునాది_conflict)
{
	ఒకవేళ (new_conflict.conflict_level != పునాది_conflict.conflict_level) {
		తిరిగి new_conflict.conflict_level > పునాది_conflict.conflict_level;
	}

	తిరిగి new_conflict.aircraft_dist < పునాది_conflict.aircraft_dist;
}
