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

	यदि (landed) {
		यदि (requested_action <= DaaAction::kWarnOnly) {
			प्रत्यागम decision;
		}

		decision.warn_on_ground = सत्य;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		प्रत्यागम decision;
	}

	यदि (conflict_level == prev_conflict_level) {
		प्रत्यागम decision;
	}

	ध्रुव तार्किक conflict_escalated = conflict_level > prev_conflict_level;

	यदि (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		प्रत्यागम decision;
	}

	यदि (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<पूर्णाङ्क>(previous_action), nav_state);
	}

	प्रत्यागम decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(ध्रुव uint8_t conflict_level,
		ध्रुव daa_action_params_s &params)
{
	यदि (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_निचला
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		प्रत्यागम DaaAction::kDisabled;
	}

#यदि defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	विकल्प (conflict_level) {
	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			यदि (action != DaaAction::kDisabled) {
				प्रत्यागम action;
			}

			action = action_param_to_daa_action(params.lvl_ऊपरी_act);

			यदि (action != DaaAction::kDisabled) {
				प्रत्यागम action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			यदि (action != DaaAction::kDisabled) {
				प्रत्यागम action;
			}

			प्रत्यागम action_param_to_daa_action(params.lvl_निचला_act);
		}

	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_ऊपरी: {
			ध्रुव DaaAction action = action_param_to_daa_action(params.lvl_ऊपरी_act);
			प्रत्यागम action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_निचला_act);
		}

	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			ध्रुव DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			प्रत्यागम action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_निचला_act);
		}

	पक्ष detect_and_avoid_s::DAA_CONFLICT_LVL_निचला: {
			प्रत्यागम action_param_to_daa_action(params.lvl_निचला_act);
		}
	}

	प्रत्यागम DaaAction::kDisabled;
#अन्यथा
	प्रत्यागम action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

तार्किक DaaActionPolicy::action_escalates_above_nav_state(ध्रुव DaaAction requested_action, ध्रुव uint8_t nav_state)
{
	यदि (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		प्रत्यागम असत्य;
	}

	ध्रुव DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	यदि (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (पूर्णाङ्क)requested_action,
			  (पूर्णाङ्क)nav_state);
		प्रत्यागम असत्य;
	}

	प्रत्यागम सत्य;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(ध्रुव uint8_t nav_state)
{
	विकल्प (nav_state) {
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	पक्ष vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			प्रत्यागम DaaAction::kDisabled;
		}

	पक्ष vehicle_status_s::NAVIGATION_STATE_ORBIT:
	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			प्रत्यागम DaaAction::kPositionHoldMode;
		}

	पक्ष vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			प्रत्यागम DaaAction::kReturnMode;
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
			प्रत्यागम DaaAction::kLandMode;
		}

	पक्ष vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			प्रत्यागम DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	पक्ष vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			प्रत्यागम DaaAction::k_Max_Action_मूल्य;
		}

	स्वभाव:
		भङ्ग;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	प्रत्यागम DaaAction::k_Max_Action_मूल्य;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(ध्रुव int32_t action_param)
{
	विकल्प (action_param) {
	पक्ष 0:
		प्रत्यागम DaaAction::kDisabled;

	पक्ष 1:
		प्रत्यागम DaaAction::kWarnOnly;

	पक्ष 2:
		प्रत्यागम DaaAction::kReturnMode;

	पक्ष 3:
		प्रत्यागम DaaAction::kLandMode;

	पक्ष 4:
		प्रत्यागम DaaAction::kPositionHoldMode;

	पक्ष 5:
		प्रत्यागम DaaAction::kTerminate;

	स्वभाव:
		प्रत्यागम DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(ध्रुव DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	विकल्प (action) {
	पक्ष DaaAction::kDisabled:
		प्रत्यागम 0;

	पक्ष DaaAction::kWarnOnly:
		प्रत्यागम 1;

	पक्ष DaaAction::kReturnMode:
		प्रत्यागम 2;

	पक्ष DaaAction::kLandMode:
		प्रत्यागम 3;

	पक्ष DaaAction::kPositionHoldMode:
		प्रत्यागम 4;

	पक्ष DaaAction::kTerminate:
		प्रत्यागम 5;

	स्वभाव:
		प्रत्यागम 0;
	}
}
