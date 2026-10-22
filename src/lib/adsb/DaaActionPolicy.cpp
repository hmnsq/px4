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

#अन्तर्भाव "DaaActionPolicy.h"

#अन्तर्भाव <px4_platform_common/log.h>
#अन्तर्भाव <uORB/topics/detect_and_avoid.h>
#अन्तर्भाव <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(ध्रुव uint8_t conflict_level, ध्रुव uint8_t prev_conflict_level,
		ध्रुव uint8_t nav_state, ध्रुव तार्किक landed, ध्रुव तार्किक armed,
		ध्रुव DaaAction previous_action, ध्रुव daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	ध्रुव DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	सचे (landed) {
		सचे (requested_action <= DaaAction::kWarnOnly) {
			पटिनिवत्त decision;
		}

		decision.warn_on_ground = सच्च;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		पटिनिवत्त decision;
	}

	सचे (conflict_level == prev_conflict_level) {
		पटिनिवत्त decision;
	}

	ध्रुव तार्किक conflict_escalated = conflict_level > prev_conflict_level;

	सचे (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		पटिनिवत्त decision;
	}

	सचे (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<पूर्णाङ्क>(previous_action), nav_state);
	}

	पटिनिवत्त decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(ध्रुव uint8_t conflict_level,
		ध्रुव daa_action_params_s &params)
{
	सचे (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_नीचः
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		पटिनिवत्त DaaAction::kDisabled;
	}

#सचे defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	विकल्प (conflict_level) {
	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			सचे (action != DaaAction::kDisabled) {
				पटिनिवत्त action;
			}

			action = action_param_to_daa_action(params.lvl_उच्चः_act);

			सचे (action != DaaAction::kDisabled) {
				पटिनिवत्त action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			सचे (action != DaaAction::kDisabled) {
				पटिनिवत्त action;
			}

			पटिनिवत्त action_param_to_daa_action(params.lvl_नीचः_act);
		}

	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_उच्चः: {
			ध्रुव DaaAction action = action_param_to_daa_action(params.lvl_उच्चः_act);
			पटिनिवत्त action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_नीचः_act);
		}

	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			ध्रुव DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			पटिनिवत्त action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_नीचः_act);
		}

	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_नीचः: {
			पटिनिवत्त action_param_to_daa_action(params.lvl_नीचः_act);
		}
	}

	पटिनिवत्त DaaAction::kDisabled;
#अञ्ञथा
	पटिनिवत्त action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

तार्किक DaaActionPolicy::action_escalates_above_nav_state(ध्रुव DaaAction requested_action, ध्रुव uint8_t nav_state)
{
	सचे (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		पटिनिवत्त मिच्छा;
	}

	ध्रुव DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	सचे (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (पूर्णाङ्क)requested_action,
			  (पूर्णाङ्क)nav_state);
		पटिनिवत्त मिच्छा;
	}

	पटिनिवत्त सच्च;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(ध्रुव uint8_t nav_state)
{
	विकल्प (nav_state) {
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	पक्ष vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			पटिनिवत्त DaaAction::kDisabled;
		}

	पक्ष vehicle_status_s::NAVIGATION_STATE_ORBIT:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			पटिनिवत्त DaaAction::kPositionHoldMode;
		}

	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			पटिनिवत्त DaaAction::kReturnMode;
		}

	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	पक्ष vehicle_status_s::NAVIGATION_STATE_DESCEND:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	पक्ष vehicle_status_s::NAVIGATION_STATE_MANUAL:
	पक्ष vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	पक्ष vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	पक्ष vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	पक्ष vehicle_status_s::NAVIGATION_STATE_POSCTL:
	पक्ष vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	पक्ष vehicle_status_s::NAVIGATION_STATE_ACRO:
	पक्ष vehicle_status_s::NAVIGATION_STATE_STAB: {
			पटिनिवत्त DaaAction::kLandMode;
		}

	पक्ष vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			पटिनिवत्त DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	पक्ष vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			पटिनिवत्त DaaAction::k_Max_Action_मानम्;
		}

	स्वभाव:
		भङ्ग;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	पटिनिवत्त DaaAction::k_Max_Action_मानम्;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(ध्रुव int32_t action_param)
{
	विकल्प (action_param) {
	पक्ष 0:
		पटिनिवत्त DaaAction::kDisabled;

	पक्ष 1:
		पटिनिवत्त DaaAction::kWarnOnly;

	पक्ष 2:
		पटिनिवत्त DaaAction::kReturnMode;

	पक्ष 3:
		पटिनिवत्त DaaAction::kLandMode;

	पक्ष 4:
		पटिनिवत्त DaaAction::kPositionHoldMode;

	पक्ष 5:
		पटिनिवत्त DaaAction::kTerminate;

	स्वभाव:
		पटिनिवत्त DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(ध्रुव DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	विकल्प (action) {
	पक्ष DaaAction::kDisabled:
		पटिनिवत्त 0;

	पक्ष DaaAction::kWarnOnly:
		पटिनिवत्त 1;

	पक्ष DaaAction::kReturnMode:
		पटिनिवत्त 2;

	पक्ष DaaAction::kLandMode:
		पटिनिवत्त 3;

	पक्ष DaaAction::kPositionHoldMode:
		पटिनिवत्त 4;

	पक्ष DaaAction::kTerminate:
		पटिनिवत्त 5;

	स्वभाव:
		पटिनिवत्त 0;
	}
}
