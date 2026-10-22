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

#அந்தர்பாவ "DaaActionPolicy.h"

#அந்தர்பாவ <px4_platform_common/log.h>
#அந்தர்பாவ <uORB/topics/detect_and_avoid.h>
#அந்தர்பாவ <uORB/topics/vehicle_status.h>

daa_action_decision_s DaaActionPolicy::decide(மாறிலி uint8_t conflict_level, மாறிலி uint8_t prev_conflict_level,
		மாறிலி uint8_t nav_state, மாறிலி தருக்க landed, மாறிலி தருக்க armed,
		மாறிலி DaaAction previous_action, மாறிலி daa_action_params_s &params)
{
	daa_action_decision_s decision{};
	மாறிலி DaaAction requested_action = action_from_conflict_level(conflict_level, params);

	எனில் (landed) {
		எனில் (requested_action <= DaaAction::kWarnOnly) {
			திரும்பு decision;
		}

		decision.warn_on_ground = உண்மை;
		decision.ground_warning_cause = armed ? NotifyLandedActCause::kConflictAndArmed
						: NotifyLandedActCause::kConflictAndDisarmed;
		திரும்பு decision;
	}

	எனில் (conflict_level == prev_conflict_level) {
		திரும்பு decision;
	}

	மாறிலி தருக்க conflict_escalated = conflict_level > prev_conflict_level;

	எனில் (conflict_escalated && action_escalates_above_nav_state(requested_action, nav_state)) {
		decision.action_command = requested_action;
		decision.announce_action = previous_action != requested_action
					   || prev_conflict_level == detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
		திரும்பு decision;
	}

	எனில் (!conflict_escalated) {
		PX4_DEBUG("DAA: De-escalation, prev act %d, nav state %d, ", static_cast<முழு>(previous_action), nav_state);
	}

	திரும்பு decision;
}

DaaAction DaaActionPolicy::action_from_conflict_level(மாறிலி uint8_t conflict_level,
		மாறிலி daa_action_params_s &params)
{
	எனில் (conflict_level < detect_and_avoid_s::DAA_CONFLICT_LVL_கீழ்
	    || conflict_level > detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL) {
		திரும்பு DaaAction::kDisabled;
	}

#எனில் defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442

	தேர்வு (conflict_level) {
	நிலை detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL: {
			DaaAction action = action_param_to_daa_action(params.lvl_crit_act);

			எனில் (action != DaaAction::kDisabled) {
				திரும்பு action;
			}

			action = action_param_to_daa_action(params.lvl_உயர்வு_act);

			எனில் (action != DaaAction::kDisabled) {
				திரும்பு action;
			}

			action = action_param_to_daa_action(params.lvl_med_act);

			எனில் (action != DaaAction::kDisabled) {
				திரும்பு action;
			}

			திரும்பு action_param_to_daa_action(params.lvl_கீழ்_act);
		}

	நிலை detect_and_avoid_s::DAA_CONFLICT_LVL_உயர்வு: {
			மாறிலி DaaAction action = action_param_to_daa_action(params.lvl_உயர்வு_act);
			திரும்பு action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_கீழ்_act);
		}

	நிலை detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM: {
			மாறிலி DaaAction action = action_param_to_daa_action(params.lvl_med_act);
			திரும்பு action != DaaAction::kDisabled ? action : action_param_to_daa_action(params.lvl_கீழ்_act);
		}

	நிலை detect_and_avoid_s::DAA_CONFLICT_LVL_கீழ்: {
			திரும்பு action_param_to_daa_action(params.lvl_கீழ்_act);
		}
	}

	திரும்பு DaaAction::kDisabled;
#இல்லையெனில்
	திரும்பு action_param_to_daa_action(params.traff_avoid);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}

தருக்க DaaActionPolicy::action_escalates_above_nav_state(மாறிலி DaaAction requested_action, மாறிலி uint8_t nav_state)
{
	எனில் (requested_action <= DaaAction::kWarnOnly) {
		PX4_DEBUG("DAA: Escalation to Warn, no action published");
		திரும்பு பொய்;
	}

	மாறிலி DaaAction current_nav_state_action = nav_state_to_equivalent_daa_action(nav_state);

	// Only publish command if requested command is more critical than current navigator state.
	எனில் (requested_action <= current_nav_state_action) {
		PX4_DEBUG("DAA: Requested action: %d less critical than current nav state: %d, no action published",
			  (முழு)requested_action,
			  (முழு)nav_state);
		திரும்பு பொய்;
	}

	திரும்பு உண்மை;
}

DaaAction DaaActionPolicy::nav_state_to_equivalent_daa_action(மாறிலி uint8_t nav_state)
{
	தேர்வு (nav_state) {
	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
	நிலை vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE: {
			திரும்பு DaaAction::kDisabled;
		}

	நிலை vehicle_status_s::NAVIGATION_STATE_ORBIT:
	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER: {
			திரும்பு DaaAction::kPositionHoldMode;
		}

	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: {
			திரும்பு DaaAction::kReturnMode;
		}

	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
	நிலை vehicle_status_s::NAVIGATION_STATE_DESCEND:
	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
	நிலை vehicle_status_s::NAVIGATION_STATE_MANUAL:
	நிலை vehicle_status_s::NAVIGATION_STATE_ALTCTL:
	நிலை vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE:
	நிலை vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING:
	நிலை vehicle_status_s::NAVIGATION_STATE_POSCTL:
	நிலை vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW:
	நிலை vehicle_status_s::NAVIGATION_STATE_ACRO:
	நிலை vehicle_status_s::NAVIGATION_STATE_STAB: {
			திரும்பு DaaAction::kLandMode;
		}

	நிலை vehicle_status_s::NAVIGATION_STATE_TERMINATION: {
			திரும்பு DaaAction::kTerminate;
		}

	// OFFBOARD special handling, responsibility is given to offboard
	நிலை vehicle_status_s::NAVIGATION_STATE_OFFBOARD: {
			திரும்பு DaaAction::k_Max_Action_மதிப்பு;
		}

	இயல்பு:
		நிறுத்து;
	}

	// Unknown states are treated as highest priority so DAA will not override them.
	திரும்பு DaaAction::k_Max_Action_மதிப்பு;
}

DaaAction DaaActionPolicy::action_param_to_daa_action(மாறிலி int32_t action_param)
{
	தேர்வு (action_param) {
	நிலை 0:
		திரும்பு DaaAction::kDisabled;

	நிலை 1:
		திரும்பு DaaAction::kWarnOnly;

	நிலை 2:
		திரும்பு DaaAction::kReturnMode;

	நிலை 3:
		திரும்பு DaaAction::kLandMode;

	நிலை 4:
		திரும்பு DaaAction::kPositionHoldMode;

	நிலை 5:
		திரும்பு DaaAction::kTerminate;

	இயல்பு:
		திரும்பு DaaAction::kDisabled;
	}
}

uint8_t DaaActionPolicy::daa_action_to_action_param(மாறிலி DaaAction action)
{
	// Inverse of action_param_to_daa_action. Operator messages report actions with the same
	// numbering as the NAV_TRAFF_AVOID / DAA_LVL_*_ACT parameters, not the internal severity ladder.
	தேர்வு (action) {
	நிலை DaaAction::kDisabled:
		திரும்பு 0;

	நிலை DaaAction::kWarnOnly:
		திரும்பு 1;

	நிலை DaaAction::kReturnMode:
		திரும்பு 2;

	நிலை DaaAction::kLandMode:
		திரும்பு 3;

	நிலை DaaAction::kPositionHoldMode:
		திரும்பு 4;

	நிலை DaaAction::kTerminate:
		திரும்பு 5;

	இயல்பு:
		திரும்பு 0;
	}
}
