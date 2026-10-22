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

#ଅନ୍ତର୍ଭାଵ "DaaActionPolicy.h"

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/log.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/detect_and_avoid.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(ଅଚଳ uint8_t conflict_level, ଅଚଳ uint8_t prev_conflict_level,
		ଅଚଳ uint8_t nav_state, ଅଚଳ ତାର୍କିକ landed, ଅଚଳ ତାର୍କିକ armed,
		ଅଚଳ DaaAction previous_action, ଅଚଳ daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	ଅଚଳ DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	ଯଦି (landed) {
		ଯଦି (requested_action <= DaaAction::kWarnOnly) {
			ଫେରନ୍ତୁ decision;
		}

		decision.warn_on_ground = ସତ୍ୟ;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		ଫେରନ୍ତୁ decision;
	}

	ଯଦି (conflict_level == prev_conflict_level) {
		ଫେରନ୍ତୁ decision;
	}

	ଅଚଳ ତାର୍କିକ conflict_escalated = conflict_level > prev_conflict_level;

	ଯଦି (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		ଫେରନ୍ତୁ decision;
	}

	ଯଦି (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(previous_action), nav_state);
	}

	ଫେରନ୍ତୁ decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(ଅଚଳ uint8_t conflict_level,
		ଅଚଳ daa_action_params_s &params)
{
	ଯଦି (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_ତଳ
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		ଫେରନ୍ତୁ DaaAction::kDisabled;
	}

#ଯଦି defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	ବାଛ (conflict_level) {
	ସ୍ଥିତି detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			ଯଦି (action != DaaAction::kDisabled) {
				ଫେରନ୍ତୁ action;
			}

			action = action_param_to_daa_action(params.lvl_ଉପର_act);

			ଯଦି (action != DaaAction::kDisabled) {
				ଫେରନ୍ତୁ action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			ଯଦି (action != DaaAction::kDisabled) {
				ଫେରନ୍ତୁ action;
			}

			ଫେରନ୍ତୁ action_param_to_daa_action(params.lvl_ତଳ_act);
		}

	ସ୍ଥିତି detect_and_avoid_s::DAA_CONFLICT_LVL_ଉପର: {
			ଅଚଳ DaaAction action = action_param_to_daa_action(params.lvl_ଉପର_act);
			ଫେରନ୍ତୁ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_ତଳ_act);
		}

	ସ୍ଥିତି detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			ଅଚଳ DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			ଫେରନ୍ତୁ action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_ତଳ_act);
		}

	ସ୍ଥିତି detect_and_avoid_s::DAA_CONFLICT_LVL_ତଳ: {
			ଫେରନ୍ତୁ action_param_to_daa_action(params.lvl_ତଳ_act);
		}
	}

	ଫେରନ୍ତୁ DaaAction::kDisabled;
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

ତାର୍କିକ DaaActionPolicy::action_escalates_above_nav_state(ଅଚଳ DaaAction requested_action, ଅଚଳ uint8_t nav_state)
{
	ଯଦି (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଅଚଳ DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	ଯଦି (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)requested_action,
			  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nav_state);
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(ଅଚଳ uint8_t nav_state)
{
	ବାଛ (nav_state) {
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			ଫେରନ୍ତୁ DaaAction::kDisabled;
		}

	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_ORBIT:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			ଫେରନ୍ତୁ DaaAction::kPositionHoldMode;
		}

	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			ଫେରନ୍ତୁ DaaAction::kReturnMode;
		}

	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_DESCEND:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_MANUAL:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_POSCTL:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_ACRO:
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_STAB: {
			ଫେରନ୍ତୁ DaaAction::kLandMode;
		}

	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			ଫେରନ୍ତୁ DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	ସ୍ଥିତି vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			ଫେରନ୍ତୁ DaaAction::k_Max_Action_ମୂଲ୍ୟ;
		}

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	ଫେରନ୍ତୁ DaaAction::k_Max_Action_ମୂଲ୍ୟ;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(ଅଚଳ int32_t action_param)
{
	ବାଛ (action_param) {
	ସ୍ଥିତି 0:
		ଫେରନ୍ତୁ DaaAction::kDisabled;

	ସ୍ଥିତି 1:
		ଫେରନ୍ତୁ DaaAction::kWarnOnly;

	ସ୍ଥିତି 2:
		ଫେରନ୍ତୁ DaaAction::kReturnMode;

	ସ୍ଥିତି 3:
		ଫେରନ୍ତୁ DaaAction::kLandMode;

	ସ୍ଥିତି 4:
		ଫେରନ୍ତୁ DaaAction::kPositionHoldMode;

	ସ୍ଥିତି 5:
		ଫେରନ୍ତୁ DaaAction::kTerminate;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(ଅଚଳ DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	ବାଛ (action) {
	ସ୍ଥିତି DaaAction::kDisabled:
		ଫେରନ୍ତୁ 0;

	ସ୍ଥିତି DaaAction::kWarnOnly:
		ଫେରନ୍ତୁ 1;

	ସ୍ଥିତି DaaAction::kReturnMode:
		ଫେରନ୍ତୁ 2;

	ସ୍ଥିତି DaaAction::kLandMode:
		ଫେରନ୍ତୁ 3;

	ସ୍ଥିତି DaaAction::kPositionHoldMode:
		ଫେରନ୍ତୁ 4;

	ସ୍ଥିତି DaaAction::kTerminate:
		ଫେରନ୍ତୁ 5;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ 0;
	}
}
