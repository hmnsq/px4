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

	ако (landed) {
		ако (requested_action <= DaaAction::kWarnOnly) {
			врати decision;
		}

		decision.warn_on_ground = true;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		врати decision;
	}

	ако (conflict_level == prev_conflict_level) {
		врати decision;
	}

	const bool conflict_escalated = conflict_level > prev_conflict_level;

	ако (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		врати decision;
	}

	ако (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<цео>(previous_action), nav_state);
	}

	врати decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(const uint8_t conflict_level,
		const daa_action_params_s &params)
{
	ако (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_LOW
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		врати DaaAction::kDisabled;
	}

#ако defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	switch (conflict_level) {
	case detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			ако (action != DaaAction::kDisabled) {
				врати action;
			}

			action = action_param_to_daa_action(params.lvl_high_act);

			ако (action != DaaAction::kDisabled) {
				врати action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			ако (action != DaaAction::kDisabled) {
				врати action;
			}

			врати action_param_to_daa_action(params.lvl_low_act);
		}

	case detect_and_avoid_s::DAA_CONFLICT_LVL_HIGH: {
			const DaaAction action = action_param_to_daa_action(params.lvl_high_act);
			врати action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_low_act);
		}

	case detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			const DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			врати action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_low_act);
		}

	case detect_and_avoid_s::DAA_CONFLICT_LVL_LOW: {
			врати action_param_to_daa_action(params.lvl_low_act);
		}
	}

	врати DaaAction::kDisabled;
#инако
	врати action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

bool DaaActionPolicy::action_escalates_above_nav_state(const DaaAction requested_action, const uint8_t nav_state)
{
	ако (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		врати false;
	}

	const DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	ако (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (цео)requested_action,
			  (цео)nav_state);
		врати false;
	}

	врати true;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(const uint8_t nav_state)
{
	switch (nav_state) {
	case vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	case vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	case vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	case vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	case vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			врати DaaAction::kDisabled;
		}

	case vehicle_status_s::NAVIGATION_STATE_ORBIT:
	case vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			врати DaaAction::kPositionHoldMode;
		}

	case vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			врати DaaAction::kReturnMode;
		}

	case vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	case vehicle_status_s::NAVIGATION_STATE_DESCEND:
	case vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	case vehicle_status_s::NAVIGATION_STATE_MANUAL:
	case vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	case vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	case vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	case vehicle_status_s::NAVIGATION_STATE_POSCTL:
	case vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	case vehicle_status_s::NAVIGATION_STATE_ACRO:
	case vehicle_status_s::NAVIGATION_STATE_STAB: {
			врати DaaAction::kLandMode;
		}

	case vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			врати DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	case vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			врати DaaAction::kMaxActionValue;
		}

	default:
		break;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	врати DaaAction::kMaxActionValue;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(const int32_t action_param)
{
	switch (action_param) {
	case 0:
		врати DaaAction::kDisabled;

	case 1:
		врати DaaAction::kWarnOnly;

	case 2:
		врати DaaAction::kReturnMode;

	case 3:
		врати DaaAction::kLandMode;

	case 4:
		врати DaaAction::kPositionHoldMode;

	case 5:
		врати DaaAction::kTerminate;

	default:
		врати DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(const DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	switch (action) {
	case DaaAction::kDisabled:
		врати 0;

	case DaaAction::kWarnOnly:
		врати 1;

	case DaaAction::kReturnMode:
		врати 2;

	case DaaAction::kLandMode:
		врати 3;

	case DaaAction::kPositionHoldMode:
		врати 4;

	case DaaAction::kTerminate:
		врати 5;

	default:
		врати 0;
	}
}
