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

#অন্তর্ভাব "DaaActionPolicy.h"

#অন্তর্ভাব <px4_platform_common/log.h>
#অন্তর্ভাব <uORB/topics/detect_and_avoid.h>
#অন্তর্ভাব <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(ধ্রুবক uint8_t conflict_level, ধ্রুবক uint8_t পূর্ব_conflict_level,
		ধ্রুবক uint8_t nav_state, ধ্রুবক যৌক্তিক landed, ধ্রুবক যৌক্তিক armed,
		ধ্রুবক DaaAction previous_action, ধ্রুবক daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	ধ্রুবক DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	যদি (landed) {
		যদি (requested_action <= DaaAction::kWarnOnly) {
			ফেরত decision;
		}

		decision.warn_on_ground = সত্য;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		ফেরত decision;
	}

	যদি (conflict_level == পূর্ব_conflict_level) {
		ফেরত decision;
	}

	ধ্রুবক যৌক্তিক conflict_escalated = conflict_level > পূর্ব_conflict_level;

	যদি (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || পূর্ব_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		ফেরত decision;
	}

	যদি (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<পূর্ণসংখ্যা>(previous_action), nav_state);
	}

	ফেরত decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(ধ্রুবক uint8_t conflict_level,
		ধ্রুবক daa_action_params_s &params)
{
	যদি (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_নিচের
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		ফেরত DaaAction::kDisabled;
	}

#যদি defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	নির্বাচন (conflict_level) {
	ক্ষেত্র detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			যদি (action != DaaAction::kDisabled) {
				ফেরত action;
			}

			action = action_param_to_daa_action(params.lvl_উপরের_act);

			যদি (action != DaaAction::kDisabled) {
				ফেরত action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			যদি (action != DaaAction::kDisabled) {
				ফেরত action;
			}

			ফেরত action_param_to_daa_action(params.lvl_নিচের_act);
		}

	ক্ষেত্র detect_and_avoid_s::DAA_CONFLICT_LVL_উপরের: {
			ধ্রুবক DaaAction action = action_param_to_daa_action(params.lvl_উপরের_act);
			ফেরত action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_নিচের_act);
		}

	ক্ষেত্র detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			ধ্রুবক DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			ফেরত action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_নিচের_act);
		}

	ক্ষেত্র detect_and_avoid_s::DAA_CONFLICT_LVL_নিচের: {
			ফেরত action_param_to_daa_action(params.lvl_নিচের_act);
		}
	}

	ফেরত DaaAction::kDisabled;
#অন্যথা
	ফেরত action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

যৌক্তিক DaaActionPolicy::action_escalates_above_nav_state(ধ্রুবক DaaAction requested_action, ধ্রুবক uint8_t nav_state)
{
	যদি (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		ফেরত মিথ্যা;
	}

	ধ্রুবক DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	যদি (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (পূর্ণসংখ্যা)requested_action,
			  (পূর্ণসংখ্যা)nav_state);
		ফেরত মিথ্যা;
	}

	ফেরত সত্য;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(ধ্রুবক uint8_t nav_state)
{
	নির্বাচন (nav_state) {
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			ফেরত DaaAction::kDisabled;
		}

	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_ORBIT:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			ফেরত DaaAction::kPositionHoldMode;
		}

	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			ফেরত DaaAction::kReturnMode;
		}

	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_DESCEND:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_MANUAL:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_POSCTL:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_ACRO:
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_STAB: {
			ফেরত DaaAction::kLandMode;
		}

	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			ফেরত DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			ফেরত DaaAction::k_Max_Action_মান;
		}

	পূর্বনির্ধারিত:
		বিরতি;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	ফেরত DaaAction::k_Max_Action_মান;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(ধ্রুবক int32_t action_param)
{
	নির্বাচন (action_param) {
	ক্ষেত্র 0:
		ফেরত DaaAction::kDisabled;

	ক্ষেত্র 1:
		ফেরত DaaAction::kWarnOnly;

	ক্ষেত্র 2:
		ফেরত DaaAction::kReturnMode;

	ক্ষেত্র 3:
		ফেরত DaaAction::kLandMode;

	ক্ষেত্র 4:
		ফেরত DaaAction::kPositionHoldMode;

	ক্ষেত্র 5:
		ফেরত DaaAction::kTerminate;

	পূর্বনির্ধারিত:
		ফেরত DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(ধ্রুবক DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	নির্বাচন (action) {
	ক্ষেত্র DaaAction::kDisabled:
		ফেরত 0;

	ক্ষেত্র DaaAction::kWarnOnly:
		ফেরত 1;

	ক্ষেত্র DaaAction::kReturnMode:
		ফেরত 2;

	ক্ষেত্র DaaAction::kLandMode:
		ফেরত 3;

	ক্ষেত্র DaaAction::kPositionHoldMode:
		ফেরত 4;

	ক্ষেত্র DaaAction::kTerminate:
		ফেরত 5;

	পূর্বনির্ধারিত:
		ফেরত 0;
	}
}
