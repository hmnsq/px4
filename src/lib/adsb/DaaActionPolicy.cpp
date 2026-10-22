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

#ಅನ್ತರ್ಭಾವ "DaaActionPolicy.h"

#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/detect_and_avoid.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(ಸ್ಥಿರಾಂಕ uint8_t conflict_level, ಸ್ಥಿರಾಂಕ uint8_t prev_conflict_level,
		ಸ್ಥಿರಾಂಕ uint8_t nav_state, ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ landed, ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ armed,
		ಸ್ಥಿರಾಂಕ DaaAction ಹಿಂದಿನ_action, ಸ್ಥಿರಾಂಕ daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	ಸ್ಥಿರಾಂಕ DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	ಆದರೆ (landed) {
		ಆದರೆ (requested_action <= DaaAction::kWarnOnly) {
			ಹಿಂತಿರುಗಿ decision;
		}

		decision.warn_on_ground = ನಿಜ;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		ಹಿಂತಿರುಗಿ decision;
	}

	ಆದರೆ (conflict_level == prev_conflict_level) {
		ಹಿಂತಿರುಗಿ decision;
	}

	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ conflict_escalated = conflict_level > prev_conflict_level;

	ಆದರೆ (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = ಹಿಂದಿನ_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		ಹಿಂತಿರುಗಿ decision;
	}

	ಆದರೆ (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<ಪೂರ್ಣಾಂಕ>(ಹಿಂದಿನ_action), nav_state);
	}

	ಹಿಂತಿರುಗಿ decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(ಸ್ಥಿರಾಂಕ uint8_t conflict_level,
		ಸ್ಥಿರಾಂಕ daa_action_params_s &params)
{
	ಆದರೆ (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_ಕೆಳಗಿನ
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		ಹಿಂತಿರುಗಿ DaaAction::kDisabled;
	}

#ಆದರೆ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	ಆಯ್ಕೆ (conflict_level) {
	ಸ್ಥಿತಿ detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_ಗಮ್ಯ_daa_action(params.lvl_crit_act);

			ಆದರೆ (action != DaaAction::kDisabled) {
				ಹಿಂತಿರುಗಿ action;
			}

			action = action_param_ಗಮ್ಯ_daa_action(params.lvl_ಮೇಲಿನ_act);

			ಆದರೆ (action != DaaAction::kDisabled) {
				ಹಿಂತಿರುಗಿ action;
			}

			action = action_param_ಗಮ್ಯ_daa_action(params.lvl_med_act);

			ಆದರೆ (action != DaaAction::kDisabled) {
				ಹಿಂತಿರುಗಿ action;
			}

			ಹಿಂತಿರುಗಿ action_param_ಗಮ್ಯ_daa_action(params.lvl_ಕೆಳಗಿನ_act);
		}

	ಸ್ಥಿತಿ detect_and_avoid_s::DAA_CONFLICT_LVL_ಮೇಲಿನ: {
			ಸ್ಥಿರಾಂಕ DaaAction action = action_param_ಗಮ್ಯ_daa_action(params.lvl_ಮೇಲಿನ_act);
			ಹಿಂತಿರುಗಿ action != DaaAction::kDisabled ? action : action_param_ಗಮ್ಯ_daa_action(params.lvl_ಕೆಳಗಿನ_act);
		}

	ಸ್ಥಿತಿ detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			ಸ್ಥಿರಾಂಕ DaaAction action = action_param_ಗಮ್ಯ_daa_action(params.lvl_med_act);
			ಹಿಂತಿರುಗಿ action != DaaAction::kDisabled ? action : action_param_ಗಮ್ಯ_daa_action(params.lvl_ಕೆಳಗಿನ_act);
		}

	ಸ್ಥಿತಿ detect_and_avoid_s::DAA_CONFLICT_LVL_ಕೆಳಗಿನ: {
			ಹಿಂತಿರುಗಿ action_param_ಗಮ್ಯ_daa_action(params.lvl_ಕೆಳಗಿನ_act);
		}
	}

	ಹಿಂತಿರುಗಿ DaaAction::kDisabled;
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ action_param_ಗಮ್ಯ_daa_action(params.traff_avoid);
#ಆದರೆ_ಅಂತ್ಯ // CONFIG_NAVIGATOR_ADSB_F3442
}

ತಾರ್ಕಿಕ DaaActionPolicy::action_escalates_above_nav_state(ಸ್ಥಿರಾಂಕ DaaAction requested_action, ಸ್ಥಿರಾಂಕ uint8_t nav_state)
{
	ಆದರೆ (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಸ್ಥಿರಾಂಕ DaaAction current_nav_state_action = nav_state_ಗಮ್ಯ_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	ಆದರೆ (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (ಪೂರ್ಣಾಂಕ)requested_action,
			  (ಪೂರ್ಣಾಂಕ)nav_state);
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

DaaAction DaaActionPolicy::nav_state_ಗಮ್ಯ_equivalent_daa_action(ಸ್ಥಿರಾಂಕ uint8_t nav_state)
{
	ಆಯ್ಕೆ (nav_state) {
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_MISSION:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_FOLLOW_ಗುರಿ:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_GUIDED_ಕೋರ್ಸು: {
			ಹಿಂತಿರುಗಿ DaaAction::kDisabled;
		}

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ORBIT:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_LOITER: {
			ಹಿಂತಿರುಗಿ DaaAction::k_Position_Hold_ಬಹುಲಕ;
		}

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_RTL: {
			ಹಿಂತಿರುಗಿ DaaAction::k_Return_ಬಹುಲಕ;
		}

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_LAND:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_DESCEND:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_PRECLAND:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_MANUAL:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ALTCTL:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_MANUAL_PARKING:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_POSCTL:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_POSITION_SLOW:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ACRO:
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_STAB: {
			ಹಿಂತಿರುಗಿ DaaAction::k_Land_ಬಹುಲಕ;
		}

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_TERMINATION: {
			ಹಿಂತಿರುಗಿ DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_OFFBOARD: {
			ಹಿಂತಿರುಗಿ DaaAction::k_Max_Action_ಮೌಲ್ಯ;
		}

	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	ಹಿಂತಿರುಗಿ DaaAction::k_Max_Action_ಮೌಲ್ಯ;
}

DaaAction DaaActionPolicy::action_param_ಗಮ್ಯ_daa_action(ಸ್ಥಿರಾಂಕ int32_t action_param)
{
	ಆಯ್ಕೆ (action_param) {
	ಸ್ಥಿತಿ 0:
		ಹಿಂತಿರುಗಿ DaaAction::kDisabled;

	ಸ್ಥಿತಿ 1:
		ಹಿಂತಿರುಗಿ DaaAction::kWarnOnly;

	ಸ್ಥಿತಿ 2:
		ಹಿಂತಿರುಗಿ DaaAction::k_Return_ಬಹುಲಕ;

	ಸ್ಥಿತಿ 3:
		ಹಿಂತಿರುಗಿ DaaAction::k_Land_ಬಹುಲಕ;

	ಸ್ಥಿತಿ 4:
		ಹಿಂತಿರುಗಿ DaaAction::k_Position_Hold_ಬಹುಲಕ;

	ಸ್ಥಿತಿ 5:
		ಹಿಂತಿರುಗಿ DaaAction::kTerminate;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_ಗಮ್ಯ_action_param(ಸ್ಥಿರಾಂಕ DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	ಆಯ್ಕೆ (action) {
	ಸ್ಥಿತಿ DaaAction::kDisabled:
		ಹಿಂತಿರುಗಿ 0;

	ಸ್ಥಿತಿ DaaAction::kWarnOnly:
		ಹಿಂತಿರುಗಿ 1;

	ಸ್ಥಿತಿ DaaAction::k_Return_ಬಹುಲಕ:
		ಹಿಂತಿರುಗಿ 2;

	ಸ್ಥಿತಿ DaaAction::k_Land_ಬಹುಲಕ:
		ಹಿಂತಿರುಗಿ 3;

	ಸ್ಥಿತಿ DaaAction::k_Position_Hold_ಬಹುಲಕ:
		ಹಿಂತಿರುಗಿ 4;

	ಸ್ಥಿತಿ DaaAction::kTerminate:
		ಹಿಂತಿರುಗಿ 5;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ 0;
	}
}
