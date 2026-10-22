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

#അന്തര്ഭാവ "DaaActionPolicy.h"

#അന്തര്ഭാവ <px4_platform_common/log.h>
#അന്തര്ഭാവ <uORB/topics/detect_and_avoid.h>
#അന്തര്ഭാവ <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(സ്ഥിരം uint8_t conflict_level, സ്ഥിരം uint8_t prev_conflict_level,
		സ്ഥിരം uint8_t nav_state, സ്ഥിരം തർക്കം landed, സ്ഥിരം തർക്കം armed,
		സ്ഥിരം DaaAction previous_action, സ്ഥിരം daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	സ്ഥിരം DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	എങ്കിൽ (landed) {
		എങ്കിൽ (requested_action <= DaaAction::kWarnOnly) {
			തിരികെ decision;
		}

		decision.warn_on_ground = സത്യം;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		തിരികെ decision;
	}

	എങ്കിൽ (conflict_level == prev_conflict_level) {
		തിരികെ decision;
	}

	സ്ഥിരം തർക്കം conflict_escalated = conflict_level > prev_conflict_level;

	എങ്കിൽ (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		തിരികെ decision;
	}

	എങ്കിൽ (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<പൂർണ്ണസംഖ്യ>(previous_action), nav_state);
	}

	തിരികെ decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(സ്ഥിരം uint8_t conflict_level,
		സ്ഥിരം daa_action_params_s &params)
{
	എങ്കിൽ (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_താഴെയുള്ള
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		തിരികെ DaaAction::kDisabled;
	}

#എങ്കിൽ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	തിരഞ്ഞെടുപ്പ് (conflict_level) {
	അവസ്ഥ detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			എങ്കിൽ (action != DaaAction::kDisabled) {
				തിരികെ action;
			}

			action = action_param_to_daa_action(params.lvl_മുകളിലെ_act);

			എങ്കിൽ (action != DaaAction::kDisabled) {
				തിരികെ action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			എങ്കിൽ (action != DaaAction::kDisabled) {
				തിരികെ action;
			}

			തിരികെ action_param_to_daa_action(params.lvl_താഴെയുള്ള_act);
		}

	അവസ്ഥ detect_and_avoid_s::DAA_CONFLICT_LVL_മുകളിലെ: {
			സ്ഥിരം DaaAction action = action_param_to_daa_action(params.lvl_മുകളിലെ_act);
			തിരികെ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_താഴെയുള്ള_act);
		}

	അവസ്ഥ detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			സ്ഥിരം DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			തിരികെ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_താഴെയുള്ള_act);
		}

	അവസ്ഥ detect_and_avoid_s::DAA_CONFLICT_LVL_താഴെയുള്ള: {
			തിരികെ action_param_to_daa_action(params.lvl_താഴെയുള്ള_act);
		}
	}

	തിരികെ DaaAction::kDisabled;
#അല്ലെങ്കിൽ
	തിരികെ action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

തർക്കം DaaActionPolicy::action_escalates_above_nav_state(സ്ഥിരം DaaAction requested_action, സ്ഥിരം uint8_t nav_state)
{
	എങ്കിൽ (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		തിരികെ അസത്യം;
	}

	സ്ഥിരം DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	എങ്കിൽ (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (പൂർണ്ണസംഖ്യ)requested_action,
			  (പൂർണ്ണസംഖ്യ)nav_state);
		തിരികെ അസത്യം;
	}

	തിരികെ സത്യം;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(സ്ഥിരം uint8_t nav_state)
{
	തിരഞ്ഞെടുപ്പ് (nav_state) {
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			തിരികെ DaaAction::kDisabled;
		}

	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_ORBIT:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			തിരികെ DaaAction::kPositionHoldMode;
		}

	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			തിരികെ DaaAction::kReturnMode;
		}

	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_DESCEND:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_MANUAL:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_POSCTL:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_ACRO:
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_STAB: {
			തിരികെ DaaAction::kLandMode;
		}

	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			തിരികെ DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			തിരികെ DaaAction::k_Max_Action_മൂല്യം;
		}

	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	തിരികെ DaaAction::k_Max_Action_മൂല്യം;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(സ്ഥിരം int32_t action_param)
{
	തിരഞ്ഞെടുപ്പ് (action_param) {
	അവസ്ഥ 0:
		തിരികെ DaaAction::kDisabled;

	അവസ്ഥ 1:
		തിരികെ DaaAction::kWarnOnly;

	അവസ്ഥ 2:
		തിരികെ DaaAction::kReturnMode;

	അവസ്ഥ 3:
		തിരികെ DaaAction::kLandMode;

	അവസ്ഥ 4:
		തിരികെ DaaAction::kPositionHoldMode;

	അവസ്ഥ 5:
		തിരികെ DaaAction::kTerminate;

	സ്ഥിരസ്ഥിതി:
		തിരികെ DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(സ്ഥിരം DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	തിരഞ്ഞെടുപ്പ് (action) {
	അവസ്ഥ DaaAction::kDisabled:
		തിരികെ 0;

	അവസ്ഥ DaaAction::kWarnOnly:
		തിരികെ 1;

	അവസ്ഥ DaaAction::kReturnMode:
		തിരികെ 2;

	അവസ്ഥ DaaAction::kLandMode:
		തിരികെ 3;

	അവസ്ഥ DaaAction::kPositionHoldMode:
		തിരികെ 4;

	അവസ്ഥ DaaAction::kTerminate:
		തിരികെ 5;

	സ്ഥിരസ്ഥിതി:
		തിരികെ 0;
	}
}
