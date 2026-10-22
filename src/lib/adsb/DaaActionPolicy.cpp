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
 * @file DaaActionPolicy.cpp
 *
 * DAA action policy.
 *
 * @author Jonas Perolini <jonspero@me.com>
 */

#include "DaaActionPolicy.h"

#include <px4_platform_common/log.h>
#include <uORB/topics/detect_and_avoid.h>
#include <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(const uint8_t conflict_level, const uint8_t prev_conflict_level,
		const uint8_t nav_state, const bool landed, const bool armed,
		const DaaAction previous_action, const daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	const DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	ᰀᰧ (landed) {
		ᰀᰧ (requested_action <= DaaAction::kWarnOnly) {
			ᰀᰧᰌ decision;
		}

		decision.warn_on_ground = true;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		ᰀᰧᰌ decision;
	}

	ᰀᰧ (conflict_level == prev_conflict_level) {
		ᰀᰧᰌ decision;
	}

	const bool conflict_escalated = conflict_level > prev_conflict_level;

	ᰀᰧ (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		ᰀᰧᰌ decision;
	}

	ᰀᰧ (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<puur_naa_nka>(previous_action), nav_state);
	}

	ᰀᰧᰌ decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(const uint8_t conflict_level,
		const daa_action_params_s &params)
{
	ᰀᰧ (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_LOW
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		ᰀᰧᰌ DaaAction::kDisabled;
	}

#ᰀᰧ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	switch (conflict_level) {
	pak_sa detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			ᰀᰧ (action != DaaAction::kDisabled) {
				ᰀᰧᰌ action;
			}

			action = action_param_to_daa_action(params.lvl_high_act);

			ᰀᰧ (action != DaaAction::kDisabled) {
				ᰀᰧᰌ action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			ᰀᰧ (action != DaaAction::kDisabled) {
				ᰀᰧᰌ action;
			}

			ᰀᰧᰌ action_param_to_daa_action(params.lvl_low_act);
		}

	pak_sa detect_and_avoid_s::DAA_CONFLICT_LVL_HIGH: {
			const DaaAction action = action_param_to_daa_action(params.lvl_high_act);
			ᰀᰧᰌ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_low_act);
		}

	pak_sa detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			const DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			ᰀᰧᰌ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_low_act);
		}

	pak_sa detect_and_avoid_s::DAA_CONFLICT_LVL_LOW: {
			ᰀᰧᰌ action_param_to_daa_action(params.lvl_low_act);
		}
	}

	ᰀᰧᰌ DaaAction::kDisabled;
#ᰀᰧᰀ
	ᰀᰧᰌ action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

bool DaaActionPolicy::action_escalates_above_nav_state(const DaaAction requested_action, const uint8_t nav_state)
{
	ᰀᰧ (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		ᰀᰧᰌ false;
	}

	const DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	ᰀᰧ (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (puur_naa_nka)requested_action,
			  (puur_naa_nka)nav_state);
		ᰀᰧᰌ false;
	}

	ᰀᰧᰌ true;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(const uint8_t nav_state)
{
	switch (nav_state) {
	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	pak_sa vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			ᰀᰧᰌ DaaAction::kDisabled;
		}

	pak_sa vehicle_status_s::NAVIGATION_STATE_ORBIT:
	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			ᰀᰧᰌ DaaAction::kPositionHoldMode;
		}

	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			ᰀᰧᰌ DaaAction::kReturnMode;
		}

	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	pak_sa vehicle_status_s::NAVIGATION_STATE_DESCEND:
	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	pak_sa vehicle_status_s::NAVIGATION_STATE_MANUAL:
	pak_sa vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	pak_sa vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	pak_sa vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	pak_sa vehicle_status_s::NAVIGATION_STATE_POSCTL:
	pak_sa vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	pak_sa vehicle_status_s::NAVIGATION_STATE_ACRO:
	pak_sa vehicle_status_s::NAVIGATION_STATE_STAB: {
			ᰀᰧᰌ DaaAction::kLandMode;
		}

	pak_sa vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			ᰀᰧᰌ DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	pak_sa vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			ᰀᰧᰌ DaaAction::kMaxActionValue;
		}

	default:
		bha_nga;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	ᰀᰧᰌ DaaAction::kMaxActionValue;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(const int32_t action_param)
{
	switch (action_param) {
	pak_sa 0:
		ᰀᰧᰌ DaaAction::kDisabled;

	pak_sa 1:
		ᰀᰧᰌ DaaAction::kWarnOnly;

	pak_sa 2:
		ᰀᰧᰌ DaaAction::kReturnMode;

	pak_sa 3:
		ᰀᰧᰌ DaaAction::kLandMode;

	pak_sa 4:
		ᰀᰧᰌ DaaAction::kPositionHoldMode;

	pak_sa 5:
		ᰀᰧᰌ DaaAction::kTerminate;

	default:
		ᰀᰧᰌ DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(const DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	switch (action) {
	pak_sa DaaAction::kDisabled:
		ᰀᰧᰌ 0;

	pak_sa DaaAction::kWarnOnly:
		ᰀᰧᰌ 1;

	pak_sa DaaAction::kReturnMode:
		ᰀᰧᰌ 2;

	pak_sa DaaAction::kLandMode:
		ᰀᰧᰌ 3;

	pak_sa DaaAction::kPositionHoldMode:
		ᰀᰧᰌ 4;

	pak_sa DaaAction::kTerminate:
		ᰀᰧᰌ 5;

	default:
		ᰀᰧᰌ 0;
	}
}
