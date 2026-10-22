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
		ثابت DaaAction پخوانی_action, ثابت daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	ثابت DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	إذا (landed) {
		إذا (requested_action <= DaaAction::kWarnOnly) {
			أرجع decision;
		}

		decision.warn_on_ground = ریښتیا;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		أرجع decision;
	}

	إذا (conflict_level == prev_conflict_level) {
		أرجع decision;
	}

	ثابت منطقي conflict_escalated = conflict_level > prev_conflict_level;

	إذا (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = پخوانی_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		أرجع decision;
	}

	إذا (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<صحيح>(پخوانی_action), nav_state);
	}

	أرجع decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(ثابت uint8_t conflict_level,
		ثابت daa_action_params_s &params)
{
	إذا (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_ټیټ
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		أرجع DaaAction::kDisabled;
	}

#إذا defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	بدل (conflict_level) {
	حالة detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_منزل_daa_action(params.lvl_crit_act);

			إذا (action != DaaAction::kDisabled) {
				أرجع action;
			}

			action = action_param_منزل_daa_action(params.lvl_لوړ_act);

			إذا (action != DaaAction::kDisabled) {
				أرجع action;
			}

			action = action_param_منزل_daa_action(params.lvl_med_act);

			إذا (action != DaaAction::kDisabled) {
				أرجع action;
			}

			أرجع action_param_منزل_daa_action(params.lvl_ټیټ_act);
		}

	حالة detect_and_avoid_s::DAA_CONFLICT_LVL_لوړ: {
			ثابت DaaAction action = action_param_منزل_daa_action(params.lvl_لوړ_act);
			أرجع action != DaaAction::kDisabled ? action : action_param_منزل_daa_action(params.lvl_ټیټ_act);
		}

	حالة detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			ثابت DaaAction action = action_param_منزل_daa_action(params.lvl_med_act);
			أرجع action != DaaAction::kDisabled ? action : action_param_منزل_daa_action(params.lvl_ټیټ_act);
		}

	حالة detect_and_avoid_s::DAA_CONFLICT_LVL_ټیټ: {
			أرجع action_param_منزل_daa_action(params.lvl_ټیټ_act);
		}
	}

	أرجع DaaAction::kDisabled;
#وإلا
	أرجع action_param_منزل_daa_action(params.traff_avoid);
#د_که_پای // CONFIG_NAVIGATOR_ADSB_F3442
}

منطقي DaaActionPolicy::action_escalates_above_nav_state(ثابت DaaAction requested_action, ثابت uint8_t nav_state)
{
	إذا (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		أرجع خطأ;
	}

	ثابت DaaAction current_nav_state_action = nav_state_منزل_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	إذا (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (صحيح)requested_action,
			  (صحيح)nav_state);
		أرجع خطأ;
	}

	أرجع ریښتیا;
}

DaaAction DaaActionPolicy::nav_state_منزل_equivalent_daa_action(ثابت uint8_t nav_state)
{
	بدل (nav_state) {
	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION:
	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_FOLLOW_موخه:
	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	حالة vehicle_حالت_s::NAVIGATION_STATE_GUIDED_کورس: {
			أرجع DaaAction::kDisabled;
		}

	حالة vehicle_حالت_s::NAVIGATION_STATE_ORBIT:
	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_LOITER: {
			أرجع DaaAction::k_Position_Hold_ډېرپېښ;
		}

	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_RTL: {
			أرجع DaaAction::k_Return_ډېرپېښ;
		}

	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_LAND:
	حالة vehicle_حالت_s::NAVIGATION_STATE_DESCEND:
	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_PRECLAND:
	حالة vehicle_حالت_s::NAVIGATION_STATE_MANUAL:
	حالة vehicle_حالت_s::NAVIGATION_STATE_ALTCTL:
	حالة vehicle_حالت_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	حالة vehicle_حالت_s::NAVIGATION_STATE_MANUAL_PARKING:
	حالة vehicle_حالت_s::NAVIGATION_STATE_POSCTL:
	حالة vehicle_حالت_s::NAVIGATION_STATE_POSITION_SLOW:
	حالة vehicle_حالت_s::NAVIGATION_STATE_ACRO:
	حالة vehicle_حالت_s::NAVIGATION_STATE_STAB: {
			أرجع DaaAction::k_Land_ډېرپېښ;
		}

	حالة vehicle_حالت_s::NAVIGATION_STATE_TERMINATION: {
			أرجع DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	حالة vehicle_حالت_s::NAVIGATION_STATE_OFFBOARD: {
			أرجع DaaAction::k_Max_Action_ارزښت;
		}

	افتراضي:
		اكسر;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	أرجع DaaAction::k_Max_Action_ارزښت;
}

DaaAction DaaActionPolicy::action_param_منزل_daa_action(ثابت int32_t action_param)
{
	بدل (action_param) {
	حالة 0:
		أرجع DaaAction::kDisabled;

	حالة 1:
		أرجع DaaAction::kWarnOnly;

	حالة 2:
		أرجع DaaAction::k_Return_ډېرپېښ;

	حالة 3:
		أرجع DaaAction::k_Land_ډېرپېښ;

	حالة 4:
		أرجع DaaAction::k_Position_Hold_ډېرپېښ;

	حالة 5:
		أرجع DaaAction::kTerminate;

	افتراضي:
		أرجع DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_منزل_action_param(ثابت DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	بدل (action) {
	حالة DaaAction::kDisabled:
		أرجع 0;

	حالة DaaAction::kWarnOnly:
		أرجع 1;

	حالة DaaAction::k_Return_ډېرپېښ:
		أرجع 2;

	حالة DaaAction::k_Land_ډېرپېښ:
		أرجع 3;

	حالة DaaAction::k_Position_Hold_ډېرپېښ:
		أرجع 4;

	حالة DaaAction::kTerminate:
		أرجع 5;

	افتراضي:
		أرجع 0;
	}
}
