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

#ਅਨ੍ਤਰ੍ਭਾਵ "DaaActionPolicy.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/detect_and_avoid.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(ਅਚਲ uint8_t conflict_level, ਅਚਲ uint8_t prev_conflict_level,
		ਅਚਲ uint8_t nav_state, ਅਚਲ ਤਾਰਕਿਕ landed, ਅਚਲ ਤਾਰਕਿਕ armed,
		ਅਚਲ DaaAction ਪਿਛਲਾ_action, ਅਚਲ daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	ਅਚਲ DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	ਜੇ (landed) {
		ਜੇ (requested_action <= DaaAction::kWarnOnly) {
			ਵਾਪਸ decision;
		}

		decision.warn_on_ground = ਸੱਚ;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		ਵਾਪਸ decision;
	}

	ਜੇ (conflict_level == prev_conflict_level) {
		ਵਾਪਸ decision;
	}

	ਅਚਲ ਤਾਰਕਿਕ conflict_escalated = conflict_level > prev_conflict_level;

	ਜੇ (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = ਪਿਛਲਾ_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		ਵਾਪਸ decision;
	}

	ਜੇ (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<ਪੂਰਨ>(ਪਿਛਲਾ_action), nav_state);
	}

	ਵਾਪਸ decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(ਅਚਲ uint8_t conflict_level,
		ਅਚਲ daa_action_params_s &params)
{
	ਜੇ (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_ਨੀਵਾਂ
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		ਵਾਪਸ DaaAction::kDisabled;
	}

#ਜੇ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	ਚੋਣ (conflict_level) {
	ਹਾਲਤ detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_ਮੰਜ਼ਿਲ_daa_action(params.lvl_crit_act);

			ਜੇ (action != DaaAction::kDisabled) {
				ਵਾਪਸ action;
			}

			action = action_param_ਮੰਜ਼ਿਲ_daa_action(params.lvl_ਉੱਚਾ_act);

			ਜੇ (action != DaaAction::kDisabled) {
				ਵਾਪਸ action;
			}

			action = action_param_ਮੰਜ਼ਿਲ_daa_action(params.lvl_med_act);

			ਜੇ (action != DaaAction::kDisabled) {
				ਵਾਪਸ action;
			}

			ਵਾਪਸ action_param_ਮੰਜ਼ਿਲ_daa_action(params.lvl_ਨੀਵਾਂ_act);
		}

	ਹਾਲਤ detect_and_avoid_s::DAA_CONFLICT_LVL_ਉੱਚਾ: {
			ਅਚਲ DaaAction action = action_param_ਮੰਜ਼ਿਲ_daa_action(params.lvl_ਉੱਚਾ_act);
			ਵਾਪਸ action != DaaAction::kDisabled ? action : action_param_ਮੰਜ਼ਿਲ_daa_action(params.lvl_ਨੀਵਾਂ_act);
		}

	ਹਾਲਤ detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			ਅਚਲ DaaAction action = action_param_ਮੰਜ਼ਿਲ_daa_action(params.lvl_med_act);
			ਵਾਪਸ action != DaaAction::kDisabled ? action : action_param_ਮੰਜ਼ਿਲ_daa_action(params.lvl_ਨੀਵਾਂ_act);
		}

	ਹਾਲਤ detect_and_avoid_s::DAA_CONFLICT_LVL_ਨੀਵਾਂ: {
			ਵਾਪਸ action_param_ਮੰਜ਼ਿਲ_daa_action(params.lvl_ਨੀਵਾਂ_act);
		}
	}

	ਵਾਪਸ DaaAction::kDisabled;
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ action_param_ਮੰਜ਼ਿਲ_daa_action(params.traff_avoid);
#ਜੇ_ਅੰਤ // CONFIG_NAVIGATOR_ADSB_F3442
}

ਤਾਰਕਿਕ DaaActionPolicy::action_escalates_above_nav_state(ਅਚਲ DaaAction requested_action, ਅਚਲ uint8_t nav_state)
{
	ਜੇ (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		ਵਾਪਸ ਝੂਠ;
	}

	ਅਚਲ DaaAction current_nav_state_action = nav_state_ਮੰਜ਼ਿਲ_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	ਜੇ (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (ਪੂਰਨ)requested_action,
			  (ਪੂਰਨ)nav_state);
		ਵਾਪਸ ਝੂਠ;
	}

	ਵਾਪਸ ਸੱਚ;
}

DaaAction DaaActionPolicy::nav_state_ਮੰਜ਼ਿਲ_equivalent_daa_action(ਅਚਲ uint8_t nav_state)
{
	ਚੋਣ (nav_state) {
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_MISSION:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_FOLLOW_ਨਿਸ਼ਾਨਾ:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_GUIDED_ਕੋਰਸ: {
			ਵਾਪਸ DaaAction::kDisabled;
		}

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ORBIT:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_LOITER: {
			ਵਾਪਸ DaaAction::k_Position_Hold_ਬਹੁਲਕ;
		}

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_RTL: {
			ਵਾਪਸ DaaAction::k_Return_ਬਹੁਲਕ;
		}

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_LAND:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_DESCEND:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_PRECLAND:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_MANUAL:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ALTCTL:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_MANUAL_PARKING:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_POSCTL:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_POSITION_SLOW:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ACRO:
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_STAB: {
			ਵਾਪਸ DaaAction::k_Land_ਬਹੁਲਕ;
		}

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_TERMINATION: {
			ਵਾਪਸ DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_OFFBOARD: {
			ਵਾਪਸ DaaAction::k_Max_Action_ਮੁੱਲ;
		}

	ਮੂਲ:
		ਰੋਕੋ;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	ਵਾਪਸ DaaAction::k_Max_Action_ਮੁੱਲ;
}

DaaAction DaaActionPolicy::action_param_ਮੰਜ਼ਿਲ_daa_action(ਅਚਲ int32_t action_param)
{
	ਚੋਣ (action_param) {
	ਹਾਲਤ 0:
		ਵਾਪਸ DaaAction::kDisabled;

	ਹਾਲਤ 1:
		ਵਾਪਸ DaaAction::kWarnOnly;

	ਹਾਲਤ 2:
		ਵਾਪਸ DaaAction::k_Return_ਬਹੁਲਕ;

	ਹਾਲਤ 3:
		ਵਾਪਸ DaaAction::k_Land_ਬਹੁਲਕ;

	ਹਾਲਤ 4:
		ਵਾਪਸ DaaAction::k_Position_Hold_ਬਹੁਲਕ;

	ਹਾਲਤ 5:
		ਵਾਪਸ DaaAction::kTerminate;

	ਮੂਲ:
		ਵਾਪਸ DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_ਮੰਜ਼ਿਲ_action_param(ਅਚਲ DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	ਚੋਣ (action) {
	ਹਾਲਤ DaaAction::kDisabled:
		ਵਾਪਸ 0;

	ਹਾਲਤ DaaAction::kWarnOnly:
		ਵਾਪਸ 1;

	ਹਾਲਤ DaaAction::k_Return_ਬਹੁਲਕ:
		ਵਾਪਸ 2;

	ਹਾਲਤ DaaAction::k_Land_ਬਹੁਲਕ:
		ਵਾਪਸ 3;

	ਹਾਲਤ DaaAction::k_Position_Hold_ਬਹੁਲਕ:
		ਵਾਪਸ 4;

	ਹਾਲਤ DaaAction::kTerminate:
		ਵਾਪਸ 5;

	ਮੂਲ:
		ਵਾਪਸ 0;
	}
}
