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

#અન્તર્ભાવ "DaaActionPolicy.h"

#અન્તર્ભાવ <px4_platform_common/log.h>
#અન્તર્ભાવ <uORB/topics/detect_and_avoid.h>
#અન્તર્ભાવ <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(અચલ uint8_t conflict_level, અચલ uint8_t prev_conflict_level,
		અચલ uint8_t nav_state, અચલ તાર્કિક landed, અચલ તાર્કિક armed,
		અચલ DaaAction previous_action, અચલ daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	અચલ DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	જો (landed) {
		જો (requested_action <= DaaAction::kWarnOnly) {
			પાછા decision;
		}

		decision.warn_on_ground = સાચું;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		પાછા decision;
	}

	જો (conflict_level == prev_conflict_level) {
		પાછા decision;
	}

	અચલ તાર્કિક conflict_escalated = conflict_level > prev_conflict_level;

	જો (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		પાછા decision;
	}

	જો (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<પૂર્ણાંક>(previous_action), nav_state);
	}

	પાછા decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(અચલ uint8_t conflict_level,
		અચલ daa_action_params_s &params)
{
	જો (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_નીચલું
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		પાછા DaaAction::kDisabled;
	}

#જો defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	પસંદગી (conflict_level) {
	સ્થિતિ detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			જો (action != DaaAction::kDisabled) {
				પાછા action;
			}

			action = action_param_to_daa_action(params.lvl_ઉપલું_act);

			જો (action != DaaAction::kDisabled) {
				પાછા action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			જો (action != DaaAction::kDisabled) {
				પાછા action;
			}

			પાછા action_param_to_daa_action(params.lvl_નીચલું_act);
		}

	સ્થિતિ detect_and_avoid_s::DAA_CONFLICT_LVL_ઉપલું: {
			અચલ DaaAction action = action_param_to_daa_action(params.lvl_ઉપલું_act);
			પાછા action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_નીચલું_act);
		}

	સ્થિતિ detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			અચલ DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			પાછા action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_નીચલું_act);
		}

	સ્થિતિ detect_and_avoid_s::DAA_CONFLICT_LVL_નીચલું: {
			પાછા action_param_to_daa_action(params.lvl_નીચલું_act);
		}
	}

	પાછા DaaAction::kDisabled;
#નહીંતર
	પાછા action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

તાર્કિક DaaActionPolicy::action_escalates_above_nav_state(અચલ DaaAction requested_action, અચલ uint8_t nav_state)
{
	જો (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		પાછા ખોટું;
	}

	અચલ DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	જો (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (પૂર્ણાંક)requested_action,
			  (પૂર્ણાંક)nav_state);
		પાછા ખોટું;
	}

	પાછા સાચું;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(અચલ uint8_t nav_state)
{
	પસંદગી (nav_state) {
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			પાછા DaaAction::kDisabled;
		}

	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_ORBIT:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			પાછા DaaAction::kPositionHoldMode;
		}

	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			પાછા DaaAction::kReturnMode;
		}

	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_DESCEND:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_MANUAL:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_POSCTL:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_ACRO:
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_STAB: {
			પાછા DaaAction::kLandMode;
		}

	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			પાછા DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	સ્થિતિ vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			પાછા DaaAction::k_Max_Action_કિંમત;
		}

	મૂળભૂત:
		તોડો;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	પાછા DaaAction::k_Max_Action_કિંમત;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(અચલ int32_t action_param)
{
	પસંદગી (action_param) {
	સ્થિતિ 0:
		પાછા DaaAction::kDisabled;

	સ્થિતિ 1:
		પાછા DaaAction::kWarnOnly;

	સ્થિતિ 2:
		પાછા DaaAction::kReturnMode;

	સ્થિતિ 3:
		પાછા DaaAction::kLandMode;

	સ્થિતિ 4:
		પાછા DaaAction::kPositionHoldMode;

	સ્થિતિ 5:
		પાછા DaaAction::kTerminate;

	મૂળભૂત:
		પાછા DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(અચલ DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	પસંદગી (action) {
	સ્થિતિ DaaAction::kDisabled:
		પાછા 0;

	સ્થિતિ DaaAction::kWarnOnly:
		પાછા 1;

	સ્થિતિ DaaAction::kReturnMode:
		પાછા 2;

	સ્થિતિ DaaAction::kLandMode:
		પાછા 3;

	સ્થિતિ DaaAction::kPositionHoldMode:
		પાછા 4;

	સ્થિતિ DaaAction::kTerminate:
		પાછા 5;

	મૂળભૂત:
		પાછા 0;
	}
}
