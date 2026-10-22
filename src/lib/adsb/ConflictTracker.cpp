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

#ಅನ್ತರ್ಭಾವ "ConflictTracker.h"

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>

ನಾಮಾಕಾಶ
{

ಶೂನ್ಯ append_change(conflict_tracker_changes_s &changes, ಸ್ಥಿರಾಂಕ conflict_tracker_change_s &change)
{
	ಆದರೆ (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

ಶೂನ್ಯ ದಾಖಲೆ_ignored(conflict_tracker_changes_s &changes, ಸ್ಥಿರಾಂಕ conflict_info_s &conflict, ಸ್ಥಿರಾಂಕ IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

ಶೂನ್ಯ ದಾಖಲೆ_removed(conflict_tracker_changes_s &changes, ಸ್ಥಿರಾಂಕ conflict_info_s &conflict, ಸ್ಥಿರಾಂಕ Remove_ಬಫರ್_Cause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

ತಾರ್ಕಿಕ ConflictTracker::apply_conflict(ಸ್ಥಿರಾಂಕ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ಆದರೆ (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ conflict_idx = ಕಂಡುಹಿಡಿ_conflict_idx(conflict.encoded_id);

	ಆದರೆ (conflict_idx < 0) {
		ಆದರೆ (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}

		ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_conflict(conflict, changes);
	}

	ಹಿಂತಿರುಗಿ update_conflict(conflict, conflict_idx, changes);
}

ತಾರ್ಕಿಕ ConflictTracker::ಕೂಡಿಸು_conflict(ಸ್ಥಿರಾಂಕ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ಆದರೆ (ಬಫರ್.ಗಾತ್ರ_2() < ಬಫರ್.max_ಗಾತ್ರ()) {
		ಬಫರ್.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಕನಿಷ್ಠ_ತುರ್ತು_conflict_idx = ಕಂಡುಹಿಡಿ_conflict_idx_by_ಆದ್ಯತೆ(Conflict_ಆದ್ಯತೆ::k_ಕನಿಷ್ಠ_ತುರ್ತು);

	ಆದರೆ (!is_conflict_ಇನ್ನಷ್ಟು_important(conflict, ಬಫರ್[ಕನಿಷ್ಠ_ತುರ್ತು_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		ದಾಖಲೆ_ignored(changes, conflict, IgnoreTrafficCause::k_ಬಫರ್_ತುಂಬಿದ);
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಸ್ಥಿರಾಂಕ conflict_info_s removed_conflict = ಬಫರ್[ಕನಿಷ್ಠ_ತುರ್ತು_conflict_idx];
	ಬಫರ್[ಕನಿಷ್ಠ_ತುರ್ತು_conflict_idx] = conflict;
	ದಾಖಲೆ_removed(changes, removed_conflict, Remove_ಬಫರ್_Cause::k_ಬಫರ್_ತುಂಬಿದ);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ConflictTracker::update_conflict(ಸ್ಥಿರಾಂಕ conflict_info_s &conflict, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ಸ್ಥಿರಾಂಕ uint8_t ಹಿಂದಿನ_conflict_level = ಬಫರ್[conflict_idx].conflict_level;

	ಆದರೆ (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		ಬಫರ್.remove(conflict_idx);

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಬಫರ್[conflict_idx] = conflict;
	}

	ಆದರೆ (conflict.conflict_level != ಹಿಂದಿನ_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::k_Conflict_Level_ಬದಲಾಯಿತು;
		change.conflict = conflict;
		change.ಹಿಂದಿನ_level = ಹಿಂದಿನ_conflict_level;

		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ most_ತುರ್ತು_idx = ಕಂಡುಹಿಡಿ_conflict_idx_by_ಆದ್ಯತೆ(Conflict_ಆದ್ಯತೆ::k_Most_ತುರ್ತು);
		change.conflict_is_most_ತುರ್ತು = (most_ತುರ್ತು.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_ತುರ್ತು_idx)
						     && ಬಫರ್[most_ತುರ್ತು_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ ConflictTracker::remove_stale_conflicts(ಸ್ಥಿರಾಂಕ hrt_abstime now, ಸ್ಥಿರಾಂಕ hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	ಪೂರ್ಣಾಂಕ nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = (static_cast<ಪೂರ್ಣಾಂಕ>(ಬಫರ್.ಗಾತ್ರ_2()) - 1); idx >= 0; --idx) {
		ಸ್ಥಿರಾಂಕ hrt_abstime latest_update = ಬಫರ್[idx].latest_update_timestamp;

		ಆದರೆ (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ಸ್ಥಿರಾಂಕ conflict_info_s removed_conflict = ಬಫರ್[idx];
			ಬಫರ್.remove(idx);
			ದಾಖಲೆ_removed(changes, removed_conflict, Remove_ಬಫರ್_Cause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	ಆದರೆ (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	ಹಿಂತಿರುಗಿ nb_conflicts_removed > 0;
}

ಶೂನ್ಯ ConflictTracker::clear()
{
	ಬಫರ್.clear();
	reset_most_ತುರ್ತು();
}

ಶೂನ್ಯ ConflictTracker::reset_most_ತುರ್ತು()
{
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ k_No_Conflict_ದೂರ{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	most_ತುರ್ತು = {};
	most_ತುರ್ತು.aircraft_dist = k_No_Conflict_ದೂರ;
}

ಶೂನ್ಯ ConflictTracker::refresh_most_ತುರ್ತು()
{
	ಆದರೆ (ಬಫರ್.ಖಾಲಿಗಳು()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_ತುರ್ತು();
		ಹಿಂತಿರುಗಿ;
	}

	most_ತುರ್ತು = ಕಂಡುಹಿಡಿ_most_ತುರ್ತು();
}

conflict_info_s ConflictTracker::ಕಂಡುಹಿಡಿ_most_ತುರ್ತು() ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ most_ತುರ್ತು_conflict_idx = ಕಂಡುಹಿಡಿ_conflict_idx_by_ಆದ್ಯತೆ(Conflict_ಆದ್ಯತೆ::k_Most_ತುರ್ತು);

	ಆದರೆ (!is_valid_idx(most_ತುರ್ತು_conflict_idx)) {
		ಹಿಂತಿರುಗಿ {};
	}

	ಹಿಂತಿರುಗಿ ಬಫರ್[most_ತುರ್ತು_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ಸ್ಥಿರಾಂಕ DaaEncodedId &encoded_id) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ conflict_idx = ಕಂಡುಹಿಡಿ_conflict_idx(encoded_id);

	ಆದರೆ (!is_valid_idx(conflict_idx)) {
		ಹಿಂತಿರುಗಿ {};
	}

	ಹಿಂತಿರುಗಿ ಬಫರ್[conflict_idx];
}

ಪೂರ್ಣಾಂಕ ConflictTracker::ಕಂಡುಹಿಡಿ_conflict_idx_by_ಆದ್ಯತೆ(ಸ್ಥಿರಾಂಕ Conflict_ಆದ್ಯತೆ ಆದ್ಯತೆ) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ buff_ಗಾತ್ರ = static_cast<ಪೂರ್ಣಾಂಕ>(ಬಫರ್.ಗಾತ್ರ_2());

	ಆದರೆ (buff_ಗಾತ್ರ < 1) {
		ಹಿಂತಿರುಗಿ -1;
	}

	ಪೂರ್ಣಾಂಕ ಉತ್ತಮ_idx = 0;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < buff_ಗಾತ್ರ; ++i) {
		ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ is_better = (ಆದ್ಯತೆ == Conflict_ಆದ್ಯತೆ::k_Most_ತುರ್ತು)
				       ? is_conflict_ಇನ್ನಷ್ಟು_important(ಬಫರ್[i], ಬಫರ್[ಉತ್ತಮ_idx])
				       : is_conflict_less_important(ಬಫರ್[i], ಬಫರ್[ಉತ್ತಮ_idx]);

		ಆದರೆ (is_better) {
			ಉತ್ತಮ_idx = i;
		}
	}

	ಹಿಂತಿರುಗಿ ಉತ್ತಮ_idx;
}

ಪೂರ್ಣಾಂಕ ConflictTracker::ಕಂಡುಹಿಡಿ_conflict_idx(ಸ್ಥಿರಾಂಕ DaaEncodedId &encoded_id) ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (size_t i = 0; i < ಬಫರ್.ಗಾತ್ರ_2(); i++) {
		ಆದರೆ (ಬಫರ್[i].encoded_id == encoded_id) {
			ಹಿಂತಿರುಗಿ static_cast<ಪೂರ್ಣಾಂಕ>(i);
		}
	}

	ಹಿಂತಿರುಗಿ -1;
}

ತಾರ್ಕಿಕ ConflictTracker::is_conflict_less_important(ಸ್ಥಿರಾಂಕ conflict_info_s &new_conflict,
		ಸ್ಥಿರಾಂಕ conflict_info_s &ತಳ_conflict)
{
	ಆದರೆ (new_conflict.conflict_level != ತಳ_conflict.conflict_level) {
		ಹಿಂತಿರುಗಿ new_conflict.conflict_level < ತಳ_conflict.conflict_level;
	}

	ಹಿಂತಿರುಗಿ new_conflict.aircraft_dist > ತಳ_conflict.aircraft_dist;
}

ತಾರ್ಕಿಕ ConflictTracker::is_conflict_ಇನ್ನಷ್ಟು_important(ಸ್ಥಿರಾಂಕ conflict_info_s &new_conflict,
		ಸ್ಥಿರಾಂಕ conflict_info_s &ತಳ_conflict)
{
	ಆದರೆ (new_conflict.conflict_level != ತಳ_conflict.conflict_level) {
		ಹಿಂತಿರುಗಿ new_conflict.conflict_level > ತಳ_conflict.conflict_level;
	}

	ಹಿಂತಿರುಗಿ new_conflict.aircraft_dist < ತಳ_conflict.aircraft_dist;
}
