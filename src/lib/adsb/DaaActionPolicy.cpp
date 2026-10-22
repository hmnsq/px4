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

#అన్తర్భావ "DaaActionPolicy.h"

#అన్తర్భావ <px4_platform_common/log.h>
#అన్తర్భావ <uORB/topics/detect_and_avoid.h>
#అన్తర్భావ <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(స్థిరరాశి uint8_t conflict_level, స్థిరరాశి uint8_t prev_conflict_level,
		స్థిరరాశి uint8_t nav_state, స్థిరరాశి తార్కిక landed, స్థిరరాశి తార్కిక armed,
		స్థిరరాశి DaaAction మునుపటి_action, స్థిరరాశి daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	స్థిరరాశి DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	ఒకవేళ (landed) {
		ఒకవేళ (requested_action <= DaaAction::kWarnOnly) {
			తిరిగి decision;
		}

		decision.warn_on_ground = నిజం;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		తిరిగి decision;
	}

	ఒకవేళ (conflict_level == prev_conflict_level) {
		తిరిగి decision;
	}

	స్థిరరాశి తార్కిక conflict_escalated = conflict_level > prev_conflict_level;

	ఒకవేళ (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = మునుపటి_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		తిరిగి decision;
	}

	ఒకవేళ (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<పూర్ణాంకం>(మునుపటి_action), nav_state);
	}

	తిరిగి decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(స్థిరరాశి uint8_t conflict_level,
		స్థిరరాశి daa_action_params_s &params)
{
	ఒకవేళ (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_దిగువ
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		తిరిగి DaaAction::kDisabled;
	}

#ఒకవేళ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	ఎంపిక (conflict_level) {
	స్థితి detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_గమ్యం_daa_action(params.lvl_crit_act);

			ఒకవేళ (action != DaaAction::kDisabled) {
				తిరిగి action;
			}

			action = action_param_గమ్యం_daa_action(params.lvl_ఎగువ_act);

			ఒకవేళ (action != DaaAction::kDisabled) {
				తిరిగి action;
			}

			action = action_param_గమ్యం_daa_action(params.lvl_med_act);

			ఒకవేళ (action != DaaAction::kDisabled) {
				తిరిగి action;
			}

			తిరిగి action_param_గమ్యం_daa_action(params.lvl_దిగువ_act);
		}

	స్థితి detect_and_avoid_s::DAA_CONFLICT_LVL_ఎగువ: {
			స్థిరరాశి DaaAction action = action_param_గమ్యం_daa_action(params.lvl_ఎగువ_act);
			తిరిగి action != DaaAction::kDisabled ? action : action_param_గమ్యం_daa_action(params.lvl_దిగువ_act);
		}

	స్థితి detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			స్థిరరాశి DaaAction action = action_param_గమ్యం_daa_action(params.lvl_med_act);
			తిరిగి action != DaaAction::kDisabled ? action : action_param_గమ్యం_daa_action(params.lvl_దిగువ_act);
		}

	స్థితి detect_and_avoid_s::DAA_CONFLICT_LVL_దిగువ: {
			తిరిగి action_param_గమ్యం_daa_action(params.lvl_దిగువ_act);
		}
	}

	తిరిగి DaaAction::kDisabled;
#లేకపోతే
	తిరిగి action_param_గమ్యం_daa_action(params.traff_avoid);
#అయితే_ముగింపు // CONFIG_NAVIGATOR_ADSB_F3442
}

తార్కిక DaaActionPolicy::action_escalates_above_nav_state(స్థిరరాశి DaaAction requested_action, స్థిరరాశి uint8_t nav_state)
{
	ఒకవేళ (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		తిరిగి అబద్ధం;
	}

	స్థిరరాశి DaaAction current_nav_state_action = nav_state_గమ్యం_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	ఒకవేళ (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (పూర్ణాంకం)requested_action,
			  (పూర్ణాంకం)nav_state);
		తిరిగి అబద్ధం;
	}

	తిరిగి నిజం;
}

DaaAction DaaActionPolicy::nav_state_గమ్యం_equivalent_daa_action(స్థిరరాశి uint8_t nav_state)
{
	ఎంపిక (nav_state) {
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_MISSION:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_FOLLOW_లక్ష్యం:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_GUIDED_కోర్సు: {
			తిరిగి DaaAction::kDisabled;
		}

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_ORBIT:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_LOITER: {
			తిరిగి DaaAction::k_Position_Hold_బహుళకం;
		}

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_RTL: {
			తిరిగి DaaAction::k_Return_బహుళకం;
		}

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_LAND:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_DESCEND:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_PRECLAND:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_MANUAL:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_ALTCTL:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_MANUAL_PARKING:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_POSCTL:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_POSITION_SLOW:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_ACRO:
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_STAB: {
			తిరిగి DaaAction::k_Land_బహుళకం;
		}

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_TERMINATION: {
			తిరిగి DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_OFFBOARD: {
			తిరిగి DaaAction::k_Max_Action_విలువ;
		}

	అప్రమేయ:
		ఆపు;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	తిరిగి DaaAction::k_Max_Action_విలువ;
}

DaaAction DaaActionPolicy::action_param_గమ్యం_daa_action(స్థిరరాశి int32_t action_param)
{
	ఎంపిక (action_param) {
	స్థితి 0:
		తిరిగి DaaAction::kDisabled;

	స్థితి 1:
		తిరిగి DaaAction::kWarnOnly;

	స్థితి 2:
		తిరిగి DaaAction::k_Return_బహుళకం;

	స్థితి 3:
		తిరిగి DaaAction::k_Land_బహుళకం;

	స్థితి 4:
		తిరిగి DaaAction::k_Position_Hold_బహుళకం;

	స్థితి 5:
		తిరిగి DaaAction::kTerminate;

	అప్రమేయ:
		తిరిగి DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_గమ్యం_action_param(స్థిరరాశి DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	ఎంపిక (action) {
	స్థితి DaaAction::kDisabled:
		తిరిగి 0;

	స్థితి DaaAction::kWarnOnly:
		తిరిగి 1;

	స్థితి DaaAction::k_Return_బహుళకం:
		తిరిగి 2;

	స్థితి DaaAction::k_Land_బహుళకం:
		తిరిగి 3;

	స్థితి DaaAction::k_Position_Hold_బహుళకం:
		తిరిగి 4;

	స్థితి DaaAction::kTerminate:
		తిరిగి 5;

	అప్రమేయ:
		తిరిగి 0;
	}
}
