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

daa_action_decision_s DaaActionPolicy::decide(ಧ್ರುವ uint8_t conflict_level, ಧ್ರುವ uint8_t prev_conflict_level,
		ಧ್ರುವ uint8_t nav_state, ಧ್ರುವ ತಾರ್ಕಿಕ landed, ಧ್ರುವ ತಾರ್ಕಿಕ armed,
		ಧ್ರುವ DaaAction previous_action, ಧ್ರುವ daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	ಧ್ರುವ DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	ಯದಿ (landed) {
		ಯದಿ (requested_action <= DaaAction::kWarnOnly) {
			ಪ್ರತ್ಯಾಗಮ decision;
		}

		decision.warn_on_ground = ಸತ್ಯ;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		ಪ್ರತ್ಯಾಗಮ decision;
	}

	ಯದಿ (conflict_level == prev_conflict_level) {
		ಪ್ರತ್ಯಾಗಮ decision;
	}

	ಧ್ರುವ ತಾರ್ಕಿಕ conflict_escalated = conflict_level > prev_conflict_level;

	ಯದಿ (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		ಪ್ರತ್ಯಾಗಮ decision;
	}

	ಯದಿ (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<ಪೂರ್ಣಾಙ್ಕ>(previous_action), nav_state);
	}

	ಪ್ರತ್ಯಾಗಮ decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(ಧ್ರುವ uint8_t conflict_level,
		ಧ್ರುವ daa_action_params_s &params)
{
	ಯದಿ (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_ಕೆಳಗಿನ
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		ಪ್ರತ್ಯಾಗಮ DaaAction::kDisabled;
	}

#ಯದಿ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	ವಿಕಲ್ಪ (conflict_level) {
	ಪಕ್ಷ detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			ಯದಿ (action != DaaAction::kDisabled) {
				ಪ್ರತ್ಯಾಗಮ action;
			}

			action = action_param_to_daa_action(params.lvl_ಮೇಲಿನ_act);

			ಯದಿ (action != DaaAction::kDisabled) {
				ಪ್ರತ್ಯಾಗಮ action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			ಯದಿ (action != DaaAction::kDisabled) {
				ಪ್ರತ್ಯಾಗಮ action;
			}

			ಪ್ರತ್ಯಾಗಮ action_param_to_daa_action(params.lvl_ಕೆಳಗಿನ_act);
		}

	ಪಕ್ಷ detect_and_avoid_s::DAA_CONFLICT_LVL_ಮೇಲಿನ: {
			ಧ್ರುವ DaaAction action = action_param_to_daa_action(params.lvl_ಮೇಲಿನ_act);
			ಪ್ರತ್ಯಾಗಮ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_ಕೆಳಗಿನ_act);
		}

	ಪಕ್ಷ detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			ಧ್ರುವ DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			ಪ್ರತ್ಯಾಗಮ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_ಕೆಳಗಿನ_act);
		}

	ಪಕ್ಷ detect_and_avoid_s::DAA_CONFLICT_LVL_ಕೆಳಗಿನ: {
			ಪ್ರತ್ಯಾಗಮ action_param_to_daa_action(params.lvl_ಕೆಳಗಿನ_act);
		}
	}

	ಪ್ರತ್ಯಾಗಮ DaaAction::kDisabled;
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

ತಾರ್ಕಿಕ DaaActionPolicy::action_escalates_above_nav_state(ಧ್ರುವ DaaAction requested_action, ಧ್ರುವ uint8_t nav_state)
{
	ಯದಿ (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಧ್ರುವ DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	ಯದಿ (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (ಪೂರ್ಣಾಙ್ಕ)requested_action,
			  (ಪೂರ್ಣಾಙ್ಕ)nav_state);
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(ಧ್ರುವ uint8_t nav_state)
{
	ವಿಕಲ್ಪ (nav_state) {
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			ಪ್ರತ್ಯಾಗಮ DaaAction::kDisabled;
		}

	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_ORBIT:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			ಪ್ರತ್ಯಾಗಮ DaaAction::kPositionHoldMode;
		}

	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			ಪ್ರತ್ಯಾಗಮ DaaAction::kReturnMode;
		}

	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_DESCEND:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_MANUAL:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_POSCTL:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_ACRO:
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_STAB: {
			ಪ್ರತ್ಯಾಗಮ DaaAction::kLandMode;
		}

	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			ಪ್ರತ್ಯಾಗಮ DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			ಪ್ರತ್ಯಾಗಮ DaaAction::k_Max_Action_ಮೌಲ್ಯ;
		}

	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	ಪ್ರತ್ಯಾಗಮ DaaAction::k_Max_Action_ಮೌಲ್ಯ;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(ಧ್ರುವ int32_t action_param)
{
	ವಿಕಲ್ಪ (action_param) {
	ಪಕ್ಷ 0:
		ಪ್ರತ್ಯಾಗಮ DaaAction::kDisabled;

	ಪಕ್ಷ 1:
		ಪ್ರತ್ಯಾಗಮ DaaAction::kWarnOnly;

	ಪಕ್ಷ 2:
		ಪ್ರತ್ಯಾಗಮ DaaAction::kReturnMode;

	ಪಕ್ಷ 3:
		ಪ್ರತ್ಯಾಗಮ DaaAction::kLandMode;

	ಪಕ್ಷ 4:
		ಪ್ರತ್ಯಾಗಮ DaaAction::kPositionHoldMode;

	ಪಕ್ಷ 5:
		ಪ್ರತ್ಯಾಗಮ DaaAction::kTerminate;

	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(ಧ್ರುವ DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	ವಿಕಲ್ಪ (action) {
	ಪಕ್ಷ DaaAction::kDisabled:
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪಕ್ಷ DaaAction::kWarnOnly:
		ಪ್ರತ್ಯಾಗಮ 1;

	ಪಕ್ಷ DaaAction::kReturnMode:
		ಪ್ರತ್ಯಾಗಮ 2;

	ಪಕ್ಷ DaaAction::kLandMode:
		ಪ್ರತ್ಯಾಗಮ 3;

	ಪಕ್ಷ DaaAction::kPositionHoldMode:
		ಪ್ರತ್ಯಾಗಮ 4;

	ಪಕ್ಷ DaaAction::kTerminate:
		ಪ್ರತ್ಯಾಗಮ 5;

	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ 0;
	}
}
