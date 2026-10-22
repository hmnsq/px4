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

daa_action_decision_s DaaActionPolicy::decide(ধ্রুব uint8_t conflict_level, ধ্রুব uint8_t prev_conflict_level,
		ধ্রুব uint8_t nav_state, ধ্রুব তার্কিক landed, ধ্রুব তার্কিক armed,
		ধ্রুব DaaAction previous_action, ধ্রুব daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	ধ্রুব DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	যদি (landed) {
		যদি (requested_action <= DaaAction::kWarnOnly) {
			প্রত্যাগম decision;
		}

		decision.warn_on_ground = সত্য;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		প্রত্যাগম decision;
	}

	যদি (conflict_level == prev_conflict_level) {
		প্রত্যাগম decision;
	}

	ধ্রুব তার্কিক conflict_escalated = conflict_level > prev_conflict_level;

	যদি (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		প্রত্যাগম decision;
	}

	যদি (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<পূর্ণাঙ্ক>(previous_action), nav_state);
	}

	প্রত্যাগম decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(ধ্রুব uint8_t conflict_level,
		ধ্রুব daa_action_params_s &params)
{
	যদি (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_নিচের
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		প্রত্যাগম DaaAction::kDisabled;
	}

#যদি defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	বিকল্প (conflict_level) {
	পক্ষ detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			যদি (action != DaaAction::kDisabled) {
				প্রত্যাগম action;
			}

			action = action_param_to_daa_action(params.lvl_উপরের_act);

			যদি (action != DaaAction::kDisabled) {
				প্রত্যাগম action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			যদি (action != DaaAction::kDisabled) {
				প্রত্যাগম action;
			}

			প্রত্যাগম action_param_to_daa_action(params.lvl_নিচের_act);
		}

	পক্ষ detect_and_avoid_s::DAA_CONFLICT_LVL_উপরের: {
			ধ্রুব DaaAction action = action_param_to_daa_action(params.lvl_উপরের_act);
			প্রত্যাগম action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_নিচের_act);
		}

	পক্ষ detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			ধ্রুব DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			প্রত্যাগম action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_নিচের_act);
		}

	পক্ষ detect_and_avoid_s::DAA_CONFLICT_LVL_নিচের: {
			প্রত্যাগম action_param_to_daa_action(params.lvl_নিচের_act);
		}
	}

	প্রত্যাগম DaaAction::kDisabled;
#অন্যথা
	প্রত্যাগম action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

তার্কিক DaaActionPolicy::action_escalates_above_nav_state(ধ্রুব DaaAction requested_action, ধ্রুব uint8_t nav_state)
{
	যদি (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		প্রত্যাগম অসত্য;
	}

	ধ্রুব DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	যদি (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (পূর্ণাঙ্ক)requested_action,
			  (পূর্ণাঙ্ক)nav_state);
		প্রত্যাগম অসত্য;
	}

	প্রত্যাগম সত্য;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(ধ্রুব uint8_t nav_state)
{
	বিকল্প (nav_state) {
	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			প্রত্যাগম DaaAction::kDisabled;
		}

	পক্ষ vehicle_status_s::NAVIGATION_STATE_ORBIT:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			প্রত্যাগম DaaAction::kPositionHoldMode;
		}

	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			প্রত্যাগম DaaAction::kReturnMode;
		}

	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_DESCEND:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_MANUAL:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_POSCTL:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_ACRO:
	পক্ষ vehicle_status_s::NAVIGATION_STATE_STAB: {
			প্রত্যাগম DaaAction::kLandMode;
		}

	পক্ষ vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			প্রত্যাগম DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	পক্ষ vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			প্রত্যাগম DaaAction::k_Max_Action_মান;
		}

	স্বভাব:
		ভঙ্গ;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	প্রত্যাগম DaaAction::k_Max_Action_মান;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(ধ্রুব int32_t action_param)
{
	বিকল্প (action_param) {
	পক্ষ 0:
		প্রত্যাগম DaaAction::kDisabled;

	পক্ষ 1:
		প্রত্যাগম DaaAction::kWarnOnly;

	পক্ষ 2:
		প্রত্যাগম DaaAction::kReturnMode;

	পক্ষ 3:
		প্রত্যাগম DaaAction::kLandMode;

	পক্ষ 4:
		প্রত্যাগম DaaAction::kPositionHoldMode;

	পক্ষ 5:
		প্রত্যাগম DaaAction::kTerminate;

	স্বভাব:
		প্রত্যাগম DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(ধ্রুব DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	বিকল্প (action) {
	পক্ষ DaaAction::kDisabled:
		প্রত্যাগম 0;

	পক্ষ DaaAction::kWarnOnly:
		প্রত্যাগম 1;

	পক্ষ DaaAction::kReturnMode:
		প্রত্যাগম 2;

	পক্ষ DaaAction::kLandMode:
		প্রত্যাগম 3;

	পক্ষ DaaAction::kPositionHoldMode:
		প্রত্যাগম 4;

	পক্ষ DaaAction::kTerminate:
		প্রত্যাগম 5;

	স্বভাব:
		প্রত্যাগম 0;
	}
}
