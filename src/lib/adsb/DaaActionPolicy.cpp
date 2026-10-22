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

daa_action_decision_s DaaActionPolicy::decide(स्थिरांक uint8_t conflict_level, स्थिरांक uint8_t prev_conflict_level,
		स्थिरांक uint8_t nav_state, स्थिरांक तार्किक landed, स्थिरांक तार्किक armed,
		स्थिरांक DaaAction previous_action, स्थिरांक daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	स्थिरांक DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	जर (landed) {
		जर (requested_action <= DaaAction::kWarnOnly) {
			परत decision;
		}

		decision.warn_on_ground = सत्य;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		परत decision;
	}

	जर (conflict_level == prev_conflict_level) {
		परत decision;
	}

	स्थिरांक तार्किक conflict_escalated = conflict_level > prev_conflict_level;

	जर (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		परत decision;
	}

	जर (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<पूर्णांक>(previous_action), nav_state);
	}

	परत decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(स्थिरांक uint8_t conflict_level,
		स्थिरांक daa_action_params_s &params)
{
	जर (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_खालचा
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		परत DaaAction::kDisabled;
	}

#जर defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	निवड (conflict_level) {
	स्थिती detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			जर (action != DaaAction::kDisabled) {
				परत action;
			}

			action = action_param_to_daa_action(params.lvl_वरचा_act);

			जर (action != DaaAction::kDisabled) {
				परत action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			जर (action != DaaAction::kDisabled) {
				परत action;
			}

			परत action_param_to_daa_action(params.lvl_खालचा_act);
		}

	स्थिती detect_and_avoid_s::DAA_CONFLICT_LVL_वरचा: {
			स्थिरांक DaaAction action = action_param_to_daa_action(params.lvl_वरचा_act);
			परत action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_खालचा_act);
		}

	स्थिती detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			स्थिरांक DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			परत action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_खालचा_act);
		}

	स्थिती detect_and_avoid_s::DAA_CONFLICT_LVL_खालचा: {
			परत action_param_to_daa_action(params.lvl_खालचा_act);
		}
	}

	परत DaaAction::kDisabled;
#अन्यथा
	परत action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

तार्किक DaaActionPolicy::action_escalates_above_nav_state(स्थिरांक DaaAction requested_action, स्थिरांक uint8_t nav_state)
{
	जर (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		परत असत्य;
	}

	स्थिरांक DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	जर (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (पूर्णांक)requested_action,
			  (पूर्णांक)nav_state);
		परत असत्य;
	}

	परत सत्य;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(स्थिरांक uint8_t nav_state)
{
	निवड (nav_state) {
	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	स्थिती vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			परत DaaAction::kDisabled;
		}

	स्थिती vehicle_status_s::NAVIGATION_STATE_ORBIT:
	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			परत DaaAction::kPositionHoldMode;
		}

	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			परत DaaAction::kReturnMode;
		}

	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	स्थिती vehicle_status_s::NAVIGATION_STATE_DESCEND:
	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	स्थिती vehicle_status_s::NAVIGATION_STATE_MANUAL:
	स्थिती vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	स्थिती vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	स्थिती vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	स्थिती vehicle_status_s::NAVIGATION_STATE_POSCTL:
	स्थिती vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	स्थिती vehicle_status_s::NAVIGATION_STATE_ACRO:
	स्थिती vehicle_status_s::NAVIGATION_STATE_STAB: {
			परत DaaAction::kLandMode;
		}

	स्थिती vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			परत DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	स्थिती vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			परत DaaAction::k_Max_Action_मूल्य;
		}

	पूर्वनिर्धारित:
		थांबा;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	परत DaaAction::k_Max_Action_मूल्य;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(स्थिरांक int32_t action_param)
{
	निवड (action_param) {
	स्थिती 0:
		परत DaaAction::kDisabled;

	स्थिती 1:
		परत DaaAction::kWarnOnly;

	स्थिती 2:
		परत DaaAction::kReturnMode;

	स्थिती 3:
		परत DaaAction::kLandMode;

	स्थिती 4:
		परत DaaAction::kPositionHoldMode;

	स्थिती 5:
		परत DaaAction::kTerminate;

	पूर्वनिर्धारित:
		परत DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(स्थिरांक DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	निवड (action) {
	स्थिती DaaAction::kDisabled:
		परत 0;

	स्थिती DaaAction::kWarnOnly:
		परत 1;

	स्थिती DaaAction::kReturnMode:
		परत 2;

	स्थिती DaaAction::kLandMode:
		परत 3;

	स्थिती DaaAction::kPositionHoldMode:
		परत 4;

	स्थिती DaaAction::kTerminate:
		परत 5;

	पूर्वनिर्धारित:
		परत 0;
	}
}
