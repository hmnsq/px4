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

daa_action_decision_s DaaActionPolicy::decide(अचर uint8_t conflict_level, अचर uint8_t prev_conflict_level,
		अचर uint8_t nav_state, अचर तार्किक landed, अचर तार्किक armed,
		अचर DaaAction previous_action, अचर daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	अचर DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	यदि (landed) {
		यदि (requested_action <= DaaAction::kWarnOnly) {
			फर्काऊ decision;
		}

		decision.warn_on_ground = सत्य;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		फर्काऊ decision;
	}

	यदि (conflict_level == prev_conflict_level) {
		फर्काऊ decision;
	}

	अचर तार्किक conflict_escalated = conflict_level > prev_conflict_level;

	यदि (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		फर्काऊ decision;
	}

	यदि (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<पूर्णाङ्क>(previous_action), nav_state);
	}

	फर्काऊ decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(अचर uint8_t conflict_level,
		अचर daa_action_params_s &params)
{
	यदि (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_तल्लो
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		फर्काऊ DaaAction::kDisabled;
	}

#यदि defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	विकल्प (conflict_level) {
	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			यदि (action != DaaAction::kDisabled) {
				फर्काऊ action;
			}

			action = action_param_to_daa_action(params.lvl_माथिल्लो_act);

			यदि (action != DaaAction::kDisabled) {
				फर्काऊ action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			यदि (action != DaaAction::kDisabled) {
				फर्काऊ action;
			}

			फर्काऊ action_param_to_daa_action(params.lvl_तल्लो_act);
		}

	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_माथिल्लो: {
			अचर DaaAction action = action_param_to_daa_action(params.lvl_माथिल्लो_act);
			फर्काऊ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_तल्लो_act);
		}

	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			अचर DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			फर्काऊ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_तल्लो_act);
		}

	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_तल्लो: {
			फर्काऊ action_param_to_daa_action(params.lvl_तल्लो_act);
		}
	}

	फर्काऊ DaaAction::kDisabled;
#अन्यथा
	फर्काऊ action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

तार्किक DaaActionPolicy::action_escalates_above_nav_state(अचर DaaAction requested_action, अचर uint8_t nav_state)
{
	यदि (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		फर्काऊ असत्य;
	}

	अचर DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	यदि (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (पूर्णाङ्क)requested_action,
			  (पूर्णाङ्क)nav_state);
		फर्काऊ असत्य;
	}

	फर्काऊ सत्य;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(अचर uint8_t nav_state)
{
	विकल्प (nav_state) {
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	पक्ष vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			फर्काऊ DaaAction::kDisabled;
		}

	पक्ष vehicle_status_s::NAVIGATION_STATE_ORBIT:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			फर्काऊ DaaAction::kPositionHoldMode;
		}

	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			फर्काऊ DaaAction::kReturnMode;
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
			फर्काऊ DaaAction::kLandMode;
		}

	पक्ष vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			फर्काऊ DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	पक्ष vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			फर्काऊ DaaAction::k_Max_Action_मान;
		}

	स्वभाव:
		रोक;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	फर्काऊ DaaAction::k_Max_Action_मान;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(अचर int32_t action_param)
{
	विकल्प (action_param) {
	पक्ष 0:
		फर्काऊ DaaAction::kDisabled;

	पक्ष 1:
		फर्काऊ DaaAction::kWarnOnly;

	पक्ष 2:
		फर्काऊ DaaAction::kReturnMode;

	पक्ष 3:
		फर्काऊ DaaAction::kLandMode;

	पक्ष 4:
		फर्काऊ DaaAction::kPositionHoldMode;

	पक्ष 5:
		फर्काऊ DaaAction::kTerminate;

	स्वभाव:
		फर्काऊ DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(अचर DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	विकल्प (action) {
	पक्ष DaaAction::kDisabled:
		फर्काऊ 0;

	पक्ष DaaAction::kWarnOnly:
		फर्काऊ 1;

	पक्ष DaaAction::kReturnMode:
		फर्काऊ 2;

	पक्ष DaaAction::kLandMode:
		फर्काऊ 3;

	पक्ष DaaAction::kPositionHoldMode:
		फर्काऊ 4;

	पक्ष DaaAction::kTerminate:
		फर्काऊ 5;

	स्वभाव:
		फर्काऊ 0;
	}
}
