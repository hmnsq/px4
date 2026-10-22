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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/adsb/DaaActionPolicy.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/detect_and_avoid.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/vehicle_status.h>

ਨਾਮਾਕਾਸ਼
{

// Same action parameter value for every conflict level, so the decide() tests
// behave identically in F3442 and Crosstrack builds.
daa_action_params_s params_for_all_ਪੱਧਰ(ਅਚਲ int32_t action_param)
{
	daa_action_params_s params{};
#ਜੇ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	params.lvl_ਨੀਵਾਂ_act = action_param;
	params.lvl_med_act = action_param;
	params.lvl_ਉੱਚਾ_act = action_param;
	params.lvl_crit_act = action_param;
#ਨਹੀਂਤਾਂ
	params.traff_avoid = action_param;
#ਜੇ_ਅੰਤ // CONFIG_NAVIGATOR_ADSB_F3442
	ਵਾਪਸ params;
}

// Navigation state groups by the weakest DAA action allowed to escalate above them.
ਅਚਲ uint8_t k_ਹਾਲਤਾਂ_Enable_Hold[] {
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_MISSION,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_TAKEOFF,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_FOLLOW_ਨਿਸ਼ਾਨਾ,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_GUIDED_ਕੋਰਸ,
};

ਅਚਲ uint8_t k_ਹਾਲਤਾਂ_Enable_Rtl[] {
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ORBIT,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_LOITER,
};

ਅਚਲ uint8_t k_ਹਾਲਤਾਂ_Enable_Land[] {
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_RTL,
};

ਅਚਲ uint8_t k_ਹਾਲਤਾਂ_Enable_Termination[] {
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_LAND,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_DESCEND,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_PRECLAND,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_MANUAL,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ALTCTL,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ALTITUDE_CRUISE,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_MANUAL_PARKING,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_POSCTL,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_POSITION_SLOW,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ACRO,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_STAB,
};

ਅਚਲ uint8_t k_ਹਾਲਤਾਂ_Never_Enable[] {
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_TERMINATION,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_OFFBOARD,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL1,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL2,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL3,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL4,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL5,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL6,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL7,
	vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL8,
};

ਬਣਤਰ escalation_case_s {
	DaaAction action{DaaAction::kDisabled};
	ਤਾਰਕਿਕ hold_allowed{ਝੂਠ};
	ਤਾਰਕਿਕ rtl_allowed{ਝੂਠ};
	ਤਾਰਕਿਕ land_allowed{ਝੂਠ};
	ਤਾਰਕਿਕ terminate_allowed{ਝੂਠ};
};

ਖਾਲੀ check_escalation_ਮੈਟ੍ਰਿਕਸ(ਅਚਲ escalation_case_s &test_case)
{
	ਲਈ (ਅਚਲ uint8_t nav_state : k_ਹਾਲਤਾਂ_Enable_Hold) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.hold_allowed) << "action " << (ਪੂਰਨ)test_case.action << " nav_state " << (ਪੂਰਨ)nav_state;
	}

	ਲਈ (ਅਚਲ uint8_t nav_state : k_ਹਾਲਤਾਂ_Enable_Rtl) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.rtl_allowed) << "action " << (ਪੂਰਨ)test_case.action << " nav_state " << (ਪੂਰਨ)nav_state;
	}

	ਲਈ (ਅਚਲ uint8_t nav_state : k_ਹਾਲਤਾਂ_Enable_Land) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.land_allowed) << "action " << (ਪੂਰਨ)test_case.action << " nav_state " << (ਪੂਰਨ)nav_state;
	}

	ਲਈ (ਅਚਲ uint8_t nav_state : k_ਹਾਲਤਾਂ_Enable_Termination) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.terminate_allowed) << "action " << (ਪੂਰਨ)test_case.action << " nav_state " << (ਪੂਰਨ)nav_state;
	}

	ਲਈ (ਅਚਲ uint8_t nav_state : k_ਹਾਲਤਾਂ_Never_Enable) {
		EXPECT_FALSE(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state))
				<< "action " << (ਪੂਰਨ)test_case.action << " nav_state " << (ਪੂਰਨ)nav_state;
	}
}

ਖਾਲੀ expect_no_response(ਅਚਲ daa_action_decision_s &decision)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_FALSE(decision.warn_on_ground);
}

ਖਾਲੀ expect_ground_warning(ਅਚਲ daa_action_decision_s &decision, ਅਚਲ NotifyLandedActCause expected_cause)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_TRUE(decision.warn_on_ground);
	EXPECT_EQ(decision.ground_warning_cause, expected_cause);
}

ਖਾਲੀ expect_command(ਅਚਲ daa_action_decision_s &decision, ਅਚਲ DaaAction expected_action,
		    ਅਚਲ ਤਾਰਕਿਕ expected_announcement)
{
	EXPECT_EQ(decision.action_command, expected_action);
	EXPECT_EQ(decision.announce_action, expected_announcement);
	EXPECT_FALSE(decision.warn_on_ground);
}

} // namespace

// Param numbering and the internal action ladder map both ways; out-of-range maps to Disabled.
TEST(DaaActionPolicyTest, Action_Param_Mapping_ਵਰਤਦਾ_Parameter_Numbering)
{
	ਬਣਤਰ action_param_case_s {
		int32_t param;
		DaaAction action;
	};

	ਅਚਲ action_param_case_s cases[] {
		{0, DaaAction::kDisabled},
		{1, DaaAction::kWarnOnly},
		{2, DaaAction::k_Return_ਬਹੁਲਕ},
		{3, DaaAction::k_Land_ਬਹੁਲਕ},
		{4, DaaAction::k_Position_Hold_ਬਹੁਲਕ},
		{5, DaaAction::kTerminate},
	};

	ਲਈ (ਅਚਲ action_param_case_s &test_case : cases) {
		EXPECT_EQ(DaaActionPolicy::action_param_ਮੰਜ਼ਿਲ_daa_action(test_case.param), test_case.action)
				<< "param " << test_case.param;
		EXPECT_EQ(DaaActionPolicy::daa_action_ਮੰਜ਼ਿਲ_action_param(test_case.action), test_case.param)
				<< "action " << static_cast<ਪੂਰਨ>(test_case.action);
	}

	// out-of-range -> Disabled
	EXPECT_EQ(DaaActionPolicy::action_param_ਮੰਜ਼ਿਲ_daa_action(-1), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_param_ਮੰਜ਼ਿਲ_daa_action(6), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::daa_action_ਮੰਜ਼ਿਲ_action_param(DaaAction::k_Max_Action_ਮੁੱਲ), 0);
}

// An action escalates only when stronger than the current navigator state (protects manual/offboard).
TEST(DaaActionPolicyTest, ActionEscalatesOnlyAboveNavState)
{
	check_escalation_ਮੈਟ੍ਰਿਕਸ({DaaAction::kDisabled, ਝੂਠ, ਝੂਠ, ਝੂਠ, ਝੂਠ});
	check_escalation_ਮੈਟ੍ਰਿਕਸ({DaaAction::kWarnOnly, ਝੂਠ, ਝੂਠ, ਝੂਠ, ਝੂਠ});
	check_escalation_ਮੈਟ੍ਰਿਕਸ({DaaAction::k_Position_Hold_ਬਹੁਲਕ, ਸੱਚ, ਝੂਠ, ਝੂਠ, ਝੂਠ});
	check_escalation_ਮੈਟ੍ਰਿਕਸ({DaaAction::k_Return_ਬਹੁਲਕ, ਸੱਚ, ਸੱਚ, ਝੂਠ, ਝੂਠ});
	check_escalation_ਮੈਟ੍ਰਿਕਸ({DaaAction::k_Land_ਬਹੁਲਕ, ਸੱਚ, ਸੱਚ, ਸੱਚ, ਝੂਠ});
	check_escalation_ਮੈਟ੍ਰਿਕਸ({DaaAction::kTerminate, ਸੱਚ, ਸੱਚ, ਸੱਚ, ਸੱਚ});
}

// NONE and out-of-range levels never request an action, even with actions configured everywhere.
TEST(DaaActionPolicyTest, No_Action_For_Invalid_ਪੱਧਰ)
{
	ਅਚਲ daa_action_params_s params = params_for_all_ਪੱਧਰ(5); // Terminate everywhere

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_NONE, params),
		  DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL + 1, params),
		  DaaAction::kDisabled);
}

#ਜੇ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
TEST(DaaActionPolicyTest, F3442_Fallback_ਵਰਤਦਾ_Guaranteed_Breached_Zones)
{
	daa_action_params_s params{};
	params.lvl_ਨੀਵਾਂ_act = 1;		// Warn only
	params.lvl_med_act = 4;		// Hold
	params.lvl_ਉੱਚਾ_act = 0;	// Disabled
	params.lvl_crit_act = 5;	// Terminate

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_ਨੀਵਾਂ, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM, params),
		  DaaAction::k_Position_Hold_ਬਹੁਲਕ);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_ਉੱਚਾ, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::kTerminate);

	// CRITICAL guarantees that HIGH, MEDIUM, and LOW are also breached.
	params.lvl_crit_act = 0;
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::k_Position_Hold_ਬਹੁਲਕ);

	// all levels disabled -> fallback chain ends disabled
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			params_for_all_ਪੱਧਰ(0)), DaaAction::kDisabled);
}
#ਨਹੀਂਤਾਂ
// Crosstrack maps every level through the single NAV_TRAFF_AVOID action.
TEST(DaaActionPolicyTest, CrosstrackSingleActionMapping)
{
	ਅਚਲ daa_action_params_s params = params_for_all_ਪੱਧਰ(3); // Land

	ਲਈ (uint8_t level = detect_and_avoid_s::DAA_CONFLICT_LVL_ਨੀਵਾਂ;
	     level <= detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL; ++level) {
		EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(level, params), DaaAction::k_Land_ਬਹੁਲਕ);
	}
}
#ਜੇ_ਅੰਤ // CONFIG_NAVIGATOR_ADSB_F3442

// Unchanged level: no command or warning.
TEST(DaaActionPolicyTest, DecideNoResponseWithoutLevelChange)
{
	ਅਚਲ daa_action_decision_s decision = DaaActionPolicy::decide(
			detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_MISSION, ਝੂਠ, ਸੱਚ,
			DaaAction::kDisabled, params_for_all_ਪੱਧਰ(5));

	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, Decide_Warns_On_Ground_Instead_ਦਾ_Acting)
{
	ਅਚਲ uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	ਅਚਲ uint8_t none = detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	ਅਚਲ uint8_t nav_state = vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_MISSION;

	// armed -> takeoff warning
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, none, nav_state, ਸੱਚ, ਸੱਚ,
					 DaaAction::kDisabled, params_for_all_ਪੱਧਰ(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndArmed);

	// disarmed -> arming warning
	decision = DaaActionPolicy::decide(critical, none, nav_state, ਸੱਚ, ਝੂਠ,
					   DaaAction::kDisabled, params_for_all_ਪੱਧਰ(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// Repeated requests allow the caller to emit rate-limited ground warnings.
	decision = DaaActionPolicy::decide(critical, critical, nav_state, ਸੱਚ, ਝੂਠ,
					   DaaAction::kDisabled, params_for_all_ਪੱਧਰ(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// warn-only config -> nothing
	decision = DaaActionPolicy::decide(critical, none, nav_state, ਸੱਚ, ਸੱਚ,
					   DaaAction::kDisabled, params_for_all_ਪੱਧਰ(1));
	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, Decide_ਬੇਨਤੀਆਂ_Command_On_Airborne_Escalation_Only)
{
	ਅਚਲ uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	ਅਚਲ uint8_t ਨੀਵਾਂ = detect_and_avoid_s::DAA_CONFLICT_LVL_ਨੀਵਾਂ;
	ਅਚਲ daa_action_params_s land_params = params_for_all_ਪੱਧਰ(3); // Land

	// escalation -> Land command, announced
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, ਨੀਵਾਂ,
					 vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_MISSION, ਝੂਠ, ਸੱਚ,
					 DaaAction::kDisabled, land_params);
	expect_command(decision, DaaAction::k_Land_ਬਹੁਲਕ, ਸੱਚ);

	// A later level escalation that maps to the same action is not re-announced.
	decision = DaaActionPolicy::decide(critical, ਨੀਵਾਂ, vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_MISSION,
					   ਝੂਠ, ਸੱਚ, DaaAction::k_Land_ਬਹੁਲਕ, land_params);
	expect_command(decision, DaaAction::k_Land_ਬਹੁਲਕ, ਝੂਠ);

	// The first escalation after all conflicts cleared starts a new action episode.
	decision = DaaActionPolicy::decide(critical, detect_and_avoid_s::DAA_CONFLICT_LVL_NONE,
					   vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_MISSION,
					   ਝੂਠ, ਸੱਚ, DaaAction::k_Land_ਬਹੁਲਕ, land_params);
	expect_command(decision, DaaAction::k_Land_ਬਹੁਲਕ, ਸੱਚ);

	// already in a stronger mode -> nothing
	decision = DaaActionPolicy::decide(critical, ਨੀਵਾਂ, vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_TERMINATION,
					   ਝੂਠ, ਸੱਚ, DaaAction::kDisabled, land_params);
	expect_no_response(decision);

	// de-escalation -> nothing
	decision = DaaActionPolicy::decide(ਨੀਵਾਂ, critical, vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_MISSION,
					   ਝੂਠ, ਸੱਚ, DaaAction::k_Land_ਬਹੁਲਕ, land_params);
	expect_no_response(decision);
}
