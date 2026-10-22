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

#شامل "DaaActionPolicy.h"

#شامل <px4_platform_common/log.h>
#شامل <uORB/topics/detect_and_avoid.h>
#شامل <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(ثابت uint8_t conflict_level, ثابت uint8_t prev_conflict_level,
		ثابت uint8_t nav_state, ثابت منطقی landed, ثابت منطقی armed,
		ثابت DaaAction previous_action, ثابت daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	ثابت DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	اگر (landed) {
		اگر (requested_action <= DaaAction::kWarnOnly) {
			بازگشت decision;
		}

		decision.warn_on_ground = درست;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		بازگشت decision;
	}

	اگر (conflict_level == prev_conflict_level) {
		بازگشت decision;
	}

	ثابت منطقی conflict_escalated = conflict_level > prev_conflict_level;

	اگر (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		بازگشت decision;
	}

	اگر (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<صحیح>(previous_action), nav_state);
	}

	بازگشت decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(ثابت uint8_t conflict_level,
		ثابت daa_action_params_s &params)
{
	اگر (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_کم
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		بازگشت DaaAction::kDisabled;
	}

#اگر defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	انتخاب (conflict_level) {
	حالت detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			اگر (action != DaaAction::kDisabled) {
				بازگشت action;
			}

			action = action_param_to_daa_action(params.lvl_بالا_act);

			اگر (action != DaaAction::kDisabled) {
				بازگشت action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			اگر (action != DaaAction::kDisabled) {
				بازگشت action;
			}

			بازگشت action_param_to_daa_action(params.lvl_کم_act);
		}

	حالت detect_and_avoid_s::DAA_CONFLICT_LVL_بالا: {
			ثابت DaaAction action = action_param_to_daa_action(params.lvl_بالا_act);
			بازگشت action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_کم_act);
		}

	حالت detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			ثابت DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			بازگشت action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_کم_act);
		}

	حالت detect_and_avoid_s::DAA_CONFLICT_LVL_کم: {
			بازگشت action_param_to_daa_action(params.lvl_کم_act);
		}
	}

	بازگشت DaaAction::kDisabled;
#وگرنه
	بازگشت action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

منطقی DaaActionPolicy::action_escalates_above_nav_state(ثابت DaaAction requested_action, ثابت uint8_t nav_state)
{
	اگر (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		بازگشت نادرست;
	}

	ثابت DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	اگر (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (صحیح)requested_action,
			  (صحیح)nav_state);
		بازگشت نادرست;
	}

	بازگشت درست;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(ثابت uint8_t nav_state)
{
	انتخاب (nav_state) {
	حالت vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	حالت vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	حالت vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	حالت vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	حالت vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			بازگشت DaaAction::kDisabled;
		}

	حالت vehicle_status_s::NAVIGATION_STATE_ORBIT:
	حالت vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			بازگشت DaaAction::kPositionHoldMode;
		}

	حالت vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			بازگشت DaaAction::kReturnMode;
		}

	حالت vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	حالت vehicle_status_s::NAVIGATION_STATE_DESCEND:
	حالت vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	حالت vehicle_status_s::NAVIGATION_STATE_MANUAL:
	حالت vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	حالت vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	حالت vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	حالت vehicle_status_s::NAVIGATION_STATE_POSCTL:
	حالت vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	حالت vehicle_status_s::NAVIGATION_STATE_ACRO:
	حالت vehicle_status_s::NAVIGATION_STATE_STAB: {
			بازگشت DaaAction::kLandMode;
		}

	حالت vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			بازگشت DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	حالت vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			بازگشت DaaAction::k_Max_Action_مقدار;
		}

	پیشفرض:
		بشکن;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	بازگشت DaaAction::k_Max_Action_مقدار;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(ثابت int32_t action_param)
{
	انتخاب (action_param) {
	حالت 0:
		بازگشت DaaAction::kDisabled;

	حالت 1:
		بازگشت DaaAction::kWarnOnly;

	حالت 2:
		بازگشت DaaAction::kReturnMode;

	حالت 3:
		بازگشت DaaAction::kLandMode;

	حالت 4:
		بازگشت DaaAction::kPositionHoldMode;

	حالت 5:
		بازگشت DaaAction::kTerminate;

	پیشفرض:
		بازگشت DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(ثابت DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	انتخاب (action) {
	حالت DaaAction::kDisabled:
		بازگشت 0;

	حالت DaaAction::kWarnOnly:
		بازگشت 1;

	حالت DaaAction::kReturnMode:
		بازگشت 2;

	حالت DaaAction::kLandMode:
		بازگشت 3;

	حالت DaaAction::kPositionHoldMode:
		بازگشت 4;

	حالت DaaAction::kTerminate:
		بازگشت 5;

	پیشفرض:
		بازگشت 0;
	}
}
