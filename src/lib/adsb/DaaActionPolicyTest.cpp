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
 * @file DaaActionPolicyTest.cpp
 * @brief Unit tests for the DAA action policy.
 *
 * @author Jonas Perolini <jonspero@me.com>
 */

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>

#ಅನ್ತರ್ಭಾವ <lib/adsb/DaaActionPolicy.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/detect_and_avoid.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/vehicle_status.h>

ನಾಮಾಕಾಶ
{

// Same action parameter value for every conflict level, so the decide() tests
// behave identically in F3442 and Crosstrack builds.
daa_action_params_s params_for_all_ಮಟ್ಟಗಳು(ಸ್ಥಿರಾಂಕ int32_t action_param)
{
	daa_action_params_s params{};
#ಆದರೆ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	params.lvl_ಕೆಳಗಿನ_act = action_param;
	params.lvl_med_act = action_param;
	params.lvl_ಮೇಲಿನ_act = action_param;
	params.lvl_crit_act = action_param;
#ಇಲ್ಲದಿದ್ದರೆ
	params.traff_avoid = action_param;
#ಆದರೆ_ಅಂತ್ಯ // CONFIG_NAVIGATOR_ADSB_F3442
	ಹಿಂತಿರುಗಿ params;
}

// Navigation state groups by the weakest DAA action allowed to escalate above them.
ಸ್ಥಿರಾಂಕ uint8_t k_ಸ್ಥಿತಿಗಳು_Enable_Hold[] {
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_MISSION,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_TAKEOFF,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_FOLLOW_ಗುರಿ,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_GUIDED_ಕೋರ್ಸು,
};

ಸ್ಥಿರಾಂಕ uint8_t k_ಸ್ಥಿತಿಗಳು_Enable_Rtl[] {
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ORBIT,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_LOITER,
};

ಸ್ಥಿರಾಂಕ uint8_t k_ಸ್ಥಿತಿಗಳು_Enable_Land[] {
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_RTL,
};

ಸ್ಥಿರಾಂಕ uint8_t k_ಸ್ಥಿತಿಗಳು_Enable_Termination[] {
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_LAND,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_DESCEND,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_PRECLAND,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_MANUAL,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ALTCTL,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ALTITUDE_CRUISE,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_MANUAL_PARKING,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_POSCTL,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_POSITION_SLOW,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ACRO,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_STAB,
};

ಸ್ಥಿರಾಂಕ uint8_t k_ಸ್ಥಿತಿಗಳು_Never_Enable[] {
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_TERMINATION,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_OFFBOARD,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL1,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL2,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL3,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL4,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL5,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL6,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL7,
	vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL8,
};

ರಚನೆ escalation_case_s {
	DaaAction action{DaaAction::kDisabled};
	ತಾರ್ಕಿಕ hold_allowed{ಸುಳ್ಳು};
	ತಾರ್ಕಿಕ rtl_allowed{ಸುಳ್ಳು};
	ತಾರ್ಕಿಕ land_allowed{ಸುಳ್ಳು};
	ತಾರ್ಕಿಕ terminate_allowed{ಸುಳ್ಳು};
};

ಶೂನ್ಯ check_escalation_ಮಾತೃಕೆ(ಸ್ಥಿರಾಂಕ escalation_case_s &test_case)
{
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ uint8_t nav_state : k_ಸ್ಥಿತಿಗಳು_Enable_Hold) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.hold_allowed) << "action " << (ಪೂರ್ಣಾಂಕ)test_case.action << " nav_state " << (ಪೂರ್ಣಾಂಕ)nav_state;
	}

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ uint8_t nav_state : k_ಸ್ಥಿತಿಗಳು_Enable_Rtl) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.rtl_allowed) << "action " << (ಪೂರ್ಣಾಂಕ)test_case.action << " nav_state " << (ಪೂರ್ಣಾಂಕ)nav_state;
	}

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ uint8_t nav_state : k_ಸ್ಥಿತಿಗಳು_Enable_Land) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.land_allowed) << "action " << (ಪೂರ್ಣಾಂಕ)test_case.action << " nav_state " << (ಪೂರ್ಣಾಂಕ)nav_state;
	}

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ uint8_t nav_state : k_ಸ್ಥಿತಿಗಳು_Enable_Termination) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.terminate_allowed) << "action " << (ಪೂರ್ಣಾಂಕ)test_case.action << " nav_state " << (ಪೂರ್ಣಾಂಕ)nav_state;
	}

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ uint8_t nav_state : k_ಸ್ಥಿತಿಗಳು_Never_Enable) {
		EXPECT_FALSE(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state))
				<< "action " << (ಪೂರ್ಣಾಂಕ)test_case.action << " nav_state " << (ಪೂರ್ಣಾಂಕ)nav_state;
	}
}

ಶೂನ್ಯ expect_no_response(ಸ್ಥಿರಾಂಕ daa_action_decision_s &decision)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_FALSE(decision.warn_on_ground);
}

ಶೂನ್ಯ expect_ground_warning(ಸ್ಥಿರಾಂಕ daa_action_decision_s &decision, ಸ್ಥಿರಾಂಕ NotifyLandedActCause expected_cause)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_TRUE(decision.warn_on_ground);
	EXPECT_EQ(decision.ground_warning_cause, expected_cause);
}

ಶೂನ್ಯ expect_command(ಸ್ಥಿರಾಂಕ daa_action_decision_s &decision, ಸ್ಥಿರಾಂಕ DaaAction expected_action,
		    ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ expected_announcement)
{
	EXPECT_EQ(decision.action_command, expected_action);
	EXPECT_EQ(decision.announce_action, expected_announcement);
	EXPECT_FALSE(decision.warn_on_ground);
}

} // namespace

// Param numbering and the internal action ladder map both ways; out-of-range maps to Disabled.
TEST(DaaActionPolicyTest, Action_Param_Mapping_ಬಳಸುತ್ತದೆ_Parameter_Numbering)
{
	ರಚನೆ action_param_case_s {
		int32_t param;
		DaaAction action;
	};

	ಸ್ಥಿರಾಂಕ action_param_case_s cases[] {
		{0, DaaAction::kDisabled},
		{1, DaaAction::kWarnOnly},
		{2, DaaAction::k_Return_ಬಹುಲಕ},
		{3, DaaAction::k_Land_ಬಹುಲಕ},
		{4, DaaAction::k_Position_Hold_ಬಹುಲಕ},
		{5, DaaAction::kTerminate},
	};

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ action_param_case_s &test_case : cases) {
		EXPECT_EQ(DaaActionPolicy::action_param_ಗಮ್ಯ_daa_action(test_case.param), test_case.action)
				<< "param " << test_case.param;
		EXPECT_EQ(DaaActionPolicy::daa_action_ಗಮ್ಯ_action_param(test_case.action), test_case.param)
				<< "action " << static_cast<ಪೂರ್ಣಾಂಕ>(test_case.action);
	}

	// out-of-range -> Disabled
	EXPECT_EQ(DaaActionPolicy::action_param_ಗಮ್ಯ_daa_action(-1), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_param_ಗಮ್ಯ_daa_action(6), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::daa_action_ಗಮ್ಯ_action_param(DaaAction::k_Max_Action_ಮೌಲ್ಯ), 0);
}

// An action escalates only when stronger than the current navigator state (protects manual/offboard).
TEST(DaaActionPolicyTest, ActionEscalatesOnlyAboveNavState)
{
	check_escalation_ಮಾತೃಕೆ({DaaAction::kDisabled, ಸುಳ್ಳು, ಸುಳ್ಳು, ಸುಳ್ಳು, ಸುಳ್ಳು});
	check_escalation_ಮಾತೃಕೆ({DaaAction::kWarnOnly, ಸುಳ್ಳು, ಸುಳ್ಳು, ಸುಳ್ಳು, ಸುಳ್ಳು});
	check_escalation_ಮಾತೃಕೆ({DaaAction::k_Position_Hold_ಬಹುಲಕ, ನಿಜ, ಸುಳ್ಳು, ಸುಳ್ಳು, ಸುಳ್ಳು});
	check_escalation_ಮಾತೃಕೆ({DaaAction::k_Return_ಬಹುಲಕ, ನಿಜ, ನಿಜ, ಸುಳ್ಳು, ಸುಳ್ಳು});
	check_escalation_ಮಾತೃಕೆ({DaaAction::k_Land_ಬಹುಲಕ, ನಿಜ, ನಿಜ, ನಿಜ, ಸುಳ್ಳು});
	check_escalation_ಮಾತೃಕೆ({DaaAction::kTerminate, ನಿಜ, ನಿಜ, ನಿಜ, ನಿಜ});
}

// NONE and out-of-range levels never request an action, even with actions configured everywhere.
TEST(DaaActionPolicyTest, No_Action_For_Invalid_ಮಟ್ಟಗಳು)
{
	ಸ್ಥಿರಾಂಕ daa_action_params_s params = params_for_all_ಮಟ್ಟಗಳು(5); // Terminate everywhere

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_NONE, params),
		  DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL + 1, params),
		  DaaAction::kDisabled);
}

#ಆದರೆ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
TEST(DaaActionPolicyTest, F3442_Fallback_ಬಳಸುತ್ತದೆ_Guaranteed_Breached_Zones)
{
	daa_action_params_s params{};
	params.lvl_ಕೆಳಗಿನ_act = 1;		// Warn only
	params.lvl_med_act = 4;		// Hold
	params.lvl_ಮೇಲಿನ_act = 0;	// Disabled
	params.lvl_crit_act = 5;	// Terminate

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_ಕೆಳಗಿನ, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM, params),
		  DaaAction::k_Position_Hold_ಬಹುಲಕ);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_ಮೇಲಿನ, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::kTerminate);

	// CRITICAL guarantees that HIGH, MEDIUM, and LOW are also breached.
	params.lvl_crit_act = 0;
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::k_Position_Hold_ಬಹುಲಕ);

	// all levels disabled -> fallback chain ends disabled
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			params_for_all_ಮಟ್ಟಗಳು(0)), DaaAction::kDisabled);
}
#ಇಲ್ಲದಿದ್ದರೆ
// Crosstrack maps every level through the single NAV_TRAFF_AVOID action.
TEST(DaaActionPolicyTest, CrosstrackSingleActionMapping)
{
	ಸ್ಥಿರಾಂಕ daa_action_params_s params = params_for_all_ಮಟ್ಟಗಳು(3); // Land

	ಗಾಗಿ (uint8_t level = detect_and_avoid_s::DAA_CONFLICT_LVL_ಕೆಳಗಿನ;
	     level <= detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL; ++level) {
		EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(level, params), DaaAction::k_Land_ಬಹುಲಕ);
	}
}
#ಆದರೆ_ಅಂತ್ಯ // CONFIG_NAVIGATOR_ADSB_F3442

// Unchanged level: no command or warning.
TEST(DaaActionPolicyTest, DecideNoResponseWithoutLevelChange)
{
	ಸ್ಥಿರಾಂಕ daa_action_decision_s decision = DaaActionPolicy::decide(
			detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_MISSION, ಸುಳ್ಳು, ನಿಜ,
			DaaAction::kDisabled, params_for_all_ಮಟ್ಟಗಳು(5));

	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, Decide_Warns_On_Ground_Instead_ದ_Acting)
{
	ಸ್ಥಿರಾಂಕ uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	ಸ್ಥಿರಾಂಕ uint8_t none = detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	ಸ್ಥಿರಾಂಕ uint8_t nav_state = vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_MISSION;

	// armed -> takeoff warning
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, none, nav_state, ನಿಜ, ನಿಜ,
					 DaaAction::kDisabled, params_for_all_ಮಟ್ಟಗಳು(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndArmed);

	// disarmed -> arming warning
	decision = DaaActionPolicy::decide(critical, none, nav_state, ನಿಜ, ಸುಳ್ಳು,
					   DaaAction::kDisabled, params_for_all_ಮಟ್ಟಗಳು(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// Repeated requests allow the caller to emit rate-limited ground warnings.
	decision = DaaActionPolicy::decide(critical, critical, nav_state, ನಿಜ, ಸುಳ್ಳು,
					   DaaAction::kDisabled, params_for_all_ಮಟ್ಟಗಳು(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// warn-only config -> nothing
	decision = DaaActionPolicy::decide(critical, none, nav_state, ನಿಜ, ನಿಜ,
					   DaaAction::kDisabled, params_for_all_ಮಟ್ಟಗಳು(1));
	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, Decide_ವಿನಂತಿಗಳು_Command_On_Airborne_Escalation_Only)
{
	ಸ್ಥಿರಾಂಕ uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	ಸ್ಥಿರಾಂಕ uint8_t ಕೆಳಗಿನ = detect_and_avoid_s::DAA_CONFLICT_LVL_ಕೆಳಗಿನ;
	ಸ್ಥಿರಾಂಕ daa_action_params_s land_params = params_for_all_ಮಟ್ಟಗಳು(3); // Land

	// escalation -> Land command, announced
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, ಕೆಳಗಿನ,
					 vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_MISSION, ಸುಳ್ಳು, ನಿಜ,
					 DaaAction::kDisabled, land_params);
	expect_command(decision, DaaAction::k_Land_ಬಹುಲಕ, ನಿಜ);

	// A later level escalation that maps to the same action is not re-announced.
	decision = DaaActionPolicy::decide(critical, ಕೆಳಗಿನ, vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_MISSION,
					   ಸುಳ್ಳು, ನಿಜ, DaaAction::k_Land_ಬಹುಲಕ, land_params);
	expect_command(decision, DaaAction::k_Land_ಬಹುಲಕ, ಸುಳ್ಳು);

	// The first escalation after all conflicts cleared starts a new action episode.
	decision = DaaActionPolicy::decide(critical, detect_and_avoid_s::DAA_CONFLICT_LVL_NONE,
					   vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_MISSION,
					   ಸುಳ್ಳು, ನಿಜ, DaaAction::k_Land_ಬಹುಲಕ, land_params);
	expect_command(decision, DaaAction::k_Land_ಬಹುಲಕ, ನಿಜ);

	// already in a stronger mode -> nothing
	decision = DaaActionPolicy::decide(critical, ಕೆಳಗಿನ, vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_TERMINATION,
					   ಸುಳ್ಳು, ನಿಜ, DaaAction::kDisabled, land_params);
	expect_no_response(decision);

	// de-escalation -> nothing
	decision = DaaActionPolicy::decide(ಕೆಳಗಿನ, critical, vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_MISSION,
					   ಸುಳ್ಳು, ನಿಜ, DaaAction::k_Land_ಬಹುಲಕ, land_params);
	expect_no_response(decision);
}
