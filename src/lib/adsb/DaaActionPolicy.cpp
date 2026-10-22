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

#ضمن "DaaActionPolicy.h"

#ضمن <px4_platform_common/log.h>
#ضمن <uORB/topics/detect_and_avoid.h>
#ضمن <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(ثابت uint8_t conflict_level, ثابت uint8_t prev_conflict_level,
		ثابت uint8_t nav_state, ثابت منطقي landed, ثابت منطقي armed,
		ثابت DaaAction previous_action, ثابت daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	ثابت DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	ܐܢ (landed) {
		ܐܢ (requested_action <= DaaAction::kWarnOnly) {
			ܦܢܐ decision;
		}

		decision.warn_on_ground = ܫܪܪܐ;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		ܦܢܐ decision;
	}

	ܐܢ (conflict_level == prev_conflict_level) {
		ܦܢܐ decision;
	}

	ثابت منطقي conflict_escalated = conflict_level > prev_conflict_level;

	ܐܢ (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		ܦܢܐ decision;
	}

	ܐܢ (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<صحيح>(previous_action), nav_state);
	}

	ܦܢܐ decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(ثابت uint8_t conflict_level,
		ثابت daa_action_params_s &params)
{
	ܐܢ (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_LOW
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		ܦܢܐ DaaAction::kDisabled;
	}

#ܐܢ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	بدل (conflict_level) {
	حالة detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			ܐܢ (action != DaaAction::kDisabled) {
				ܦܢܐ action;
			}

			action = action_param_to_daa_action(params.lvl_high_act);

			ܐܢ (action != DaaAction::kDisabled) {
				ܦܢܐ action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			ܐܢ (action != DaaAction::kDisabled) {
				ܦܢܐ action;
			}

			ܦܢܐ action_param_to_daa_action(params.lvl_low_act);
		}

	حالة detect_and_avoid_s::DAA_CONFLICT_LVL_HIGH: {
			ثابت DaaAction action = action_param_to_daa_action(params.lvl_high_act);
			ܦܢܐ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_low_act);
		}

	حالة detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			ثابت DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			ܦܢܐ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_low_act);
		}

	حالة detect_and_avoid_s::DAA_CONFLICT_LVL_LOW: {
			ܦܢܐ action_param_to_daa_action(params.lvl_low_act);
		}
	}

	ܦܢܐ DaaAction::kDisabled;
#ܐܚܪܢܐ
	ܦܢܐ action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

منطقي DaaActionPolicy::action_escalates_above_nav_state(ثابت DaaAction requested_action, ثابت uint8_t nav_state)
{
	ܐܢ (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		ܦܢܐ ܕܓܠܐ;
	}

	ثابت DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	ܐܢ (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (صحيح)requested_action,
			  (صحيح)nav_state);
		ܦܢܐ ܕܓܠܐ;
	}

	ܦܢܐ ܫܪܪܐ;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(ثابت uint8_t nav_state)
{
	بدل (nav_state) {
	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	حالة vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			ܦܢܐ DaaAction::kDisabled;
		}

	حالة vehicle_status_s::NAVIGATION_STATE_ORBIT:
	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			ܦܢܐ DaaAction::kPositionHoldMode;
		}

	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			ܦܢܐ DaaAction::kReturnMode;
		}

	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	حالة vehicle_status_s::NAVIGATION_STATE_DESCEND:
	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	حالة vehicle_status_s::NAVIGATION_STATE_MANUAL:
	حالة vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	حالة vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	حالة vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	حالة vehicle_status_s::NAVIGATION_STATE_POSCTL:
	حالة vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	حالة vehicle_status_s::NAVIGATION_STATE_ACRO:
	حالة vehicle_status_s::NAVIGATION_STATE_STAB: {
			ܦܢܐ DaaAction::kLandMode;
		}

	حالة vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			ܦܢܐ DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	حالة vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			ܦܢܐ DaaAction::kMaxActionValue;
		}

	افتراضي:
		اكسر;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	ܦܢܐ DaaAction::kMaxActionValue;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(ثابت int32_t action_param)
{
	بدل (action_param) {
	حالة 0:
		ܦܢܐ DaaAction::kDisabled;

	حالة 1:
		ܦܢܐ DaaAction::kWarnOnly;

	حالة 2:
		ܦܢܐ DaaAction::kReturnMode;

	حالة 3:
		ܦܢܐ DaaAction::kLandMode;

	حالة 4:
		ܦܢܐ DaaAction::kPositionHoldMode;

	حالة 5:
		ܦܢܐ DaaAction::kTerminate;

	افتراضي:
		ܦܢܐ DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(ثابت DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	بدل (action) {
	حالة DaaAction::kDisabled:
		ܦܢܐ 0;

	حالة DaaAction::kWarnOnly:
		ܦܢܐ 1;

	حالة DaaAction::kReturnMode:
		ܦܢܐ 2;

	حالة DaaAction::kLandMode:
		ܦܢܐ 3;

	حالة DaaAction::kPositionHoldMode:
		ܦܢܐ 4;

	حالة DaaAction::kTerminate:
		ܦܢܐ 5;

	افتراضي:
		ܦܢܐ 0;
	}
}
