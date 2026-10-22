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

#ਅਨ੍ਤਰ੍ਭਾਵ "ConflictTracker.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>

ਨਾਮਾਕਾਸ਼
{

ਖਾਲੀ append_change(conflict_tracker_changes_s &changes, ਅਚਲ conflict_tracker_change_s &change)
{
	ਜੇ (!changes.push_back(change)) {
		PX4_ERR("DAA: conflict changes overflow");
	}
}

ਖਾਲੀ ਰਿਕਾਰਡ_ignored(conflict_tracker_changes_s &changes, ਅਚਲ conflict_info_s &conflict, ਅਚਲ IgnoreTrafficCause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kReportIgnored;
	change.conflict = conflict;
	change.ignore_cause = cause;
	append_change(changes, change);
}

ਖਾਲੀ ਰਿਕਾਰਡ_removed(conflict_tracker_changes_s &changes, ਅਚਲ conflict_info_s &conflict, ਅਚਲ Remove_ਬਫਰ_Cause cause)
{
	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictRemoved;
	change.conflict = conflict;
	change.remove_cause = cause;
	append_change(changes, change);
}

} // namespace

ਤਾਰਕਿਕ ConflictTracker::apply_conflict(ਅਚਲ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ਜੇ (conflict.encoded_id.id == 0
	    || conflict.conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL
	    || !PX4_ISFINITE(conflict.aircraft_dist)
	    || conflict.aircraft_dist < 0.f) {
		PX4_ERR("DAA: invalid conflict");
		ਵਾਪਸ ਝੂਠ;
	}

	ਅਚਲ ਪੂਰਨ conflict_idx = ਲੱਭੋ_conflict_idx(conflict.encoded_id);

	ਜੇ (conflict_idx < 0) {
		ਜੇ (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
			ਵਾਪਸ ਝੂਠ;
		}

		ਵਾਪਸ ਜੋੜੋ_conflict(conflict, changes);
	}

	ਵਾਪਸ update_conflict(conflict, conflict_idx, changes);
}

ਤਾਰਕਿਕ ConflictTracker::ਜੋੜੋ_conflict(ਅਚਲ conflict_info_s &conflict, conflict_tracker_changes_s &changes)
{
	ਜੇ (ਬਫਰ.ਮਾਪ() < ਬਫਰ.max_ਮਾਪ()) {
		ਬਫਰ.push_back(conflict);

		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::kConflictAdded;
		change.conflict = conflict;
		append_change(changes, change);
		ਵਾਪਸ ਸੱਚ;
	}

	ਅਚਲ ਪੂਰਨ ਘੱਟ_ਤੋਂ_ਘੱਟ_ਜ਼ਰੂਰੀ_conflict_idx = ਲੱਭੋ_conflict_idx_by_ਤਰਜੀਹ(Conflict_ਤਰਜੀਹ::k_ਘੱਟ_ਤੋਂ_ਘੱਟ_ਜ਼ਰੂਰੀ);

	ਜੇ (!is_conflict_ਹੋਰ_important(conflict, ਬਫਰ[ਘੱਟ_ਤੋਂ_ਘੱਟ_ਜ਼ਰੂਰੀ_conflict_idx])) {
		PX4_DEBUG("DAA: new conflict does not outrank tracked conflicts, ignoring.");
		ਰਿਕਾਰਡ_ignored(changes, conflict, IgnoreTrafficCause::k_ਬਫਰ_ਭਰਿਆ);
		ਵਾਪਸ ਝੂਠ;
	}

	ਅਚਲ conflict_info_s removed_conflict = ਬਫਰ[ਘੱਟ_ਤੋਂ_ਘੱਟ_ਜ਼ਰੂਰੀ_conflict_idx];
	ਬਫਰ[ਘੱਟ_ਤੋਂ_ਘੱਟ_ਜ਼ਰੂਰੀ_conflict_idx] = conflict;
	ਰਿਕਾਰਡ_removed(changes, removed_conflict, Remove_ਬਫਰ_Cause::k_ਬਫਰ_ਭਰਿਆ);

	conflict_tracker_change_s change{};
	change.type = ConflictTrackerChangeType::kConflictAdded;
	change.conflict = conflict;
	append_change(changes, change);
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ConflictTracker::update_conflict(ਅਚਲ conflict_info_s &conflict, ਅਚਲ ਪੂਰਨ conflict_idx,
				      conflict_tracker_changes_s &changes)
{
	ਅਚਲ uint8_t ਪਿਛਲਾ_conflict_level = ਬਫਰ[conflict_idx].conflict_level;

	ਜੇ (conflict.conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE) {
		PX4_DEBUG("DAA: Conflict avoided");
		ਬਫਰ.remove(conflict_idx);

	} ਨਹੀਂਤਾਂ {
		ਬਫਰ[conflict_idx] = conflict;
	}

	ਜੇ (conflict.conflict_level != ਪਿਛਲਾ_conflict_level) {
		conflict_tracker_change_s change{};
		change.type = ConflictTrackerChangeType::k_Conflict_Level_ਬਦਲਿਆ;
		change.conflict = conflict;
		change.ਪਿਛਲਾ_level = ਪਿਛਲਾ_conflict_level;

		ਅਚਲ ਪੂਰਨ most_ਜ਼ਰੂਰੀ_idx = ਲੱਭੋ_conflict_idx_by_ਤਰਜੀਹ(Conflict_ਤਰਜੀਹ::k_Most_ਜ਼ਰੂਰੀ);
		change.conflict_is_most_ਜ਼ਰੂਰੀ = (most_ਜ਼ਰੂਰੀ.encoded_id == conflict.encoded_id)
						 || (is_valid_idx(most_ਜ਼ਰੂਰੀ_idx)
						     && ਬਫਰ[most_ਜ਼ਰੂਰੀ_idx].encoded_id == conflict.encoded_id);

		append_change(changes, change);
	}

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ ConflictTracker::remove_stale_conflicts(ਅਚਲ hrt_abstime now, ਅਚਲ hrt_abstime timeout_us,
		conflict_tracker_changes_s &changes)
{
	ਪੂਰਨ nb_conflicts_removed = 0;

	// Iterate backwards because Array::remove() shifts later entries left.
	ਲਈ (ਪੂਰਨ idx = (static_cast<ਪੂਰਨ>(ਬਫਰ.ਮਾਪ()) - 1); idx >= 0; --idx) {
		ਅਚਲ hrt_abstime latest_update = ਬਫਰ[idx].latest_update_timestamp;

		ਜੇ (latest_update == 0 || latest_update > now || (now - latest_update) > timeout_us) {
			ਅਚਲ conflict_info_s removed_conflict = ਬਫਰ[idx];
			ਬਫਰ.remove(idx);
			ਰਿਕਾਰਡ_removed(changes, removed_conflict, Remove_ਬਫਰ_Cause::kStaleConflict);
			nb_conflicts_removed++;
		}
	}

	ਜੇ (nb_conflicts_removed > 0) {
		PX4_DEBUG("DAA: removed %d stale conflicts from buffer", nb_conflicts_removed);
	}

	ਵਾਪਸ nb_conflicts_removed > 0;
}

ਖਾਲੀ ConflictTracker::clear()
{
	ਬਫਰ.clear();
	reset_most_ਜ਼ਰੂਰੀ();
}

ਖਾਲੀ ConflictTracker::reset_most_ਜ਼ਰੂਰੀ()
{
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ k_No_Conflict_ਦੂਰੀ{9999.f};
	PX4_DEBUG("DAA: reset most urgent buffer to null.");
	most_ਜ਼ਰੂਰੀ = {};
	most_ਜ਼ਰੂਰੀ.aircraft_dist = k_No_Conflict_ਦੂਰੀ;
}

ਖਾਲੀ ConflictTracker::refresh_most_ਜ਼ਰੂਰੀ()
{
	ਜੇ (ਬਫਰ.ਖਾਲੀ_ਥਾਵਾਂ()) {
		PX4_DEBUG("DAA: refresh_most_urgent empty buffer.");
		reset_most_ਜ਼ਰੂਰੀ();
		ਵਾਪਸ;
	}

	most_ਜ਼ਰੂਰੀ = ਲੱਭੋ_most_ਜ਼ਰੂਰੀ();
}

conflict_info_s ConflictTracker::ਲੱਭੋ_most_ਜ਼ਰੂਰੀ() ਅਚਲ
{
	ਅਚਲ ਪੂਰਨ most_ਜ਼ਰੂਰੀ_conflict_idx = ਲੱਭੋ_conflict_idx_by_ਤਰਜੀਹ(Conflict_ਤਰਜੀਹ::k_Most_ਜ਼ਰੂਰੀ);

	ਜੇ (!is_valid_idx(most_ਜ਼ਰੂਰੀ_conflict_idx)) {
		ਵਾਪਸ {};
	}

	ਵਾਪਸ ਬਫਰ[most_ਜ਼ਰੂਰੀ_conflict_idx];
}

conflict_info_s ConflictTracker::get_conflict(ਅਚਲ DaaEncodedId &encoded_id) ਅਚਲ
{
	ਅਚਲ ਪੂਰਨ conflict_idx = ਲੱਭੋ_conflict_idx(encoded_id);

	ਜੇ (!is_valid_idx(conflict_idx)) {
		ਵਾਪਸ {};
	}

	ਵਾਪਸ ਬਫਰ[conflict_idx];
}

ਪੂਰਨ ConflictTracker::ਲੱਭੋ_conflict_idx_by_ਤਰਜੀਹ(ਅਚਲ Conflict_ਤਰਜੀਹ ਤਰਜੀਹ) ਅਚਲ
{
	ਅਚਲ ਪੂਰਨ buff_ਮਾਪ = static_cast<ਪੂਰਨ>(ਬਫਰ.ਮਾਪ());

	ਜੇ (buff_ਮਾਪ < 1) {
		ਵਾਪਸ -1;
	}

	ਪੂਰਨ ਉੱਤਮ_idx = 0;

	ਲਈ (ਪੂਰਨ i = 1; i < buff_ਮਾਪ; ++i) {
		ਅਚਲ ਤਾਰਕਿਕ is_better = (ਤਰਜੀਹ == Conflict_ਤਰਜੀਹ::k_Most_ਜ਼ਰੂਰੀ)
				       ? is_conflict_ਹੋਰ_important(ਬਫਰ[i], ਬਫਰ[ਉੱਤਮ_idx])
				       : is_conflict_less_important(ਬਫਰ[i], ਬਫਰ[ਉੱਤਮ_idx]);

		ਜੇ (is_better) {
			ਉੱਤਮ_idx = i;
		}
	}

	ਵਾਪਸ ਉੱਤਮ_idx;
}

ਪੂਰਨ ConflictTracker::ਲੱਭੋ_conflict_idx(ਅਚਲ DaaEncodedId &encoded_id) ਅਚਲ
{
	ਲਈ (size_t i = 0; i < ਬਫਰ.ਮਾਪ(); i++) {
		ਜੇ (ਬਫਰ[i].encoded_id == encoded_id) {
			ਵਾਪਸ static_cast<ਪੂਰਨ>(i);
		}
	}

	ਵਾਪਸ -1;
}

ਤਾਰਕਿਕ ConflictTracker::is_conflict_less_important(ਅਚਲ conflict_info_s &new_conflict,
		ਅਚਲ conflict_info_s &ਅਧਾਰ_conflict)
{
	ਜੇ (new_conflict.conflict_level != ਅਧਾਰ_conflict.conflict_level) {
		ਵਾਪਸ new_conflict.conflict_level < ਅਧਾਰ_conflict.conflict_level;
	}

	ਵਾਪਸ new_conflict.aircraft_dist > ਅਧਾਰ_conflict.aircraft_dist;
}

ਤਾਰਕਿਕ ConflictTracker::is_conflict_ਹੋਰ_important(ਅਚਲ conflict_info_s &new_conflict,
		ਅਚਲ conflict_info_s &ਅਧਾਰ_conflict)
{
	ਜੇ (new_conflict.conflict_level != ਅਧਾਰ_conflict.conflict_level) {
		ਵਾਪਸ new_conflict.conflict_level > ਅਧਾਰ_conflict.conflict_level;
	}

	ਵਾਪਸ new_conflict.aircraft_dist < ਅਧਾਰ_conflict.aircraft_dist;
}
