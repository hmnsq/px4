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
 * @file DaaActionPolicy.h
 *
 * Maps conflict levels to actions and decides the
 * automated response for the most urgent conflict escalation.
 *
 * @author Jonas Perolini <jonspero@me.com>
 */

#رهنمود once

#ضمن <stdint.h>

#ضمن <px4_platform_common/defines.h> // Provides CONFIG_NAVIGATOR_ADSB_F3442

// Internal action priority order used for escalation comparisons.
// User-facing parameter values are translated separately so DAA_LVL_*_ACT can
// share the NAV_TRAFF_AVOID convention without changing the priority ladder.
تعداد class DaaAction : uint8_t {
	kDisabled = 0,
	kWarnOnly = 1,
	k_Position_Hold_نما = 2,
	k_Return_نما = 3,
	k_Land_نما = 4,
	kTerminate = 5,
	k_Max_Action_مقدار = 6
};

تعداد class NotifyLandedActCause : uint8_t {
	kConflictAndArmed = 0,
	kConflictAndDisarmed = 1
};

// Action parameter values (NAV_TRAFF_AVOID / DAA_LVL_*_ACT convention), read by the caller.
بنية daa_action_params_s {
#إذا defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	int32_t lvl_کم_act {0};
	int32_t lvl_med_act{0};
	int32_t lvl_بالا_act{0};
	int32_t lvl_crit_act{0};
#وإلا
	int32_t traff_avoid {0};
#پایان_اگر // CONFIG_NAVIGATOR_ADSB_F3442
};

// A command is only requested in the air; a ground warning is only requested when landed.
بنية daa_action_decision_s {
	DaaAction action_command{DaaAction::kDisabled};
	bool announce_action{false};
	bool warn_on_ground{false};
	NotifyLandedActCause ground_warning_cause{NotifyLandedActCause::kConflictAndDisarmed};
};

class DaaActionPolicy
{
عام:
	/**
	 * @brief Decide the automated response to a most urgent conflict level escalation.
	 *
	 * When landed, action-requiring conflicts request a ground warning instead of a command.
	 * The caller is responsible for rate-limiting repeated warnings.
	 *
	 * In the air, commands are requested only on level escalation and only when stronger
	 * than the current navigator state. Unchanged levels and de-escalations do not act.
	 */
	ساكن daa_action_decision_s decide(ثابت uint8_t conflict_level, ثابت uint8_t prev_conflict_level,
					    ثابت uint8_t nav_state, ثابت bool landed, ثابت bool armed,
					    ثابت DaaAction قبلی_action, ثابت daa_action_params_s &params);

	/**
	 * @brief Map a conflict level to a DAA action.
	 *
	 * For F3442, a disabled level falls back only to zones guaranteed to contain
	 * the reported zone.
	 */
	ساكن DaaAction action_from_conflict_level(ثابت uint8_t conflict_level, ثابت daa_action_params_s &params);

	// True if requested_action is more urgent than the mode equivalent of nav_state.
	ساكن bool action_escalates_above_nav_state(ثابت DaaAction requested_action, ثابت uint8_t nav_state);

	// DAA action equivalent to a navigation state (used to gate escalation).
	ساكن DaaAction nav_state_مقصد_equivalent_daa_action(ثابت uint8_t nav_state);

	// Convert a user-facing action param value into the internal DaaAction enum.
	ساكن DaaAction action_param_مقصد_daa_action(ثابت int32_t action_param);

	// Inverse of action_param_to_daa_action.
	ساكن uint8_t daa_action_مقصد_action_param(ثابت DaaAction action);
};
