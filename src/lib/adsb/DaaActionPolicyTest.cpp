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

#अन्तर्भाव <gtest/gtest.h>

#अन्तर्भाव <lib/adsb/DaaActionPolicy.h>
#अन्तर्भाव <uORB/topics/detect_and_avoid.h>
#अन्तर्भाव <uORB/topics/vehicle_status.h>

नामाकाश
{

// Same action parameter value for every conflict level, so the decide() tests
// behave identically in F3442 and Crosstrack builds.
daa_action_params_s params_for_all_स्तराः(ध्रुव int32_t action_param)
{
	daa_action_params_s params{};
#यदि defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	params.lvl_नीचः_act = action_param;
	params.lvl_med_act = action_param;
	params.lvl_उच्चः_act = action_param;
	params.lvl_crit_act = action_param;
#अन्यथा
	params.traff_avoid = action_param;
#यदि_अन्तः // CONFIG_NAVIGATOR_ADSB_F3442
	प्रत्यागम params;
}

// Navigation state groups by the weakest DAA action allowed to escalate above them.
ध्रुव uint8_t k_अवस्थाः_Enable_Hold[] {
	vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_MISSION,
	vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_TAKEOFF,
	vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET,
	vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF,
	vehicle_स्थितिः_s::NAVIGATION_STATE_GUIDED_COURSE,
};

ध्रुव uint8_t k_अवस्थाः_Enable_Rtl[] {
	vehicle_स्थितिः_s::NAVIGATION_STATE_ORBIT,
	vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_LOITER,
};

ध्रुव uint8_t k_अवस्थाः_Enable_Land[] {
	vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_RTL,
};

ध्रुव uint8_t k_अवस्थाः_Enable_Termination[] {
	vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_LAND,
	vehicle_स्थितिः_s::NAVIGATION_STATE_DESCEND,
	vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_PRECLAND,
	vehicle_स्थितिः_s::NAVIGATION_STATE_MANUAL,
	vehicle_स्थितिः_s::NAVIGATION_STATE_ALTCTL,
	vehicle_स्थितिः_s::NAVIGATION_STATE_ALTITUDE_CRUISE,
	vehicle_स्थितिः_s::NAVIGATION_STATE_MANUAL_PARKING,
	vehicle_स्थितिः_s::NAVIGATION_STATE_POSCTL,
	vehicle_स्थितिः_s::NAVIGATION_STATE_POSITION_SLOW,
	vehicle_स्थितिः_s::NAVIGATION_STATE_ACRO,
	vehicle_स्थितिः_s::NAVIGATION_STATE_STAB,
};

ध्रुव uint8_t k_अवस्थाः_Never_Enable[] {
	vehicle_स्थितिः_s::NAVIGATION_STATE_TERMINATION,
	vehicle_स्थितिः_s::NAVIGATION_STATE_OFFBOARD,
	vehicle_स्थितिः_s::NAVIGATION_STATE_EXTERNAL1,
	vehicle_स्थितिः_s::NAVIGATION_STATE_EXTERNAL2,
	vehicle_स्थितिः_s::NAVIGATION_STATE_EXTERNAL3,
	vehicle_स्थितिः_s::NAVIGATION_STATE_EXTERNAL4,
	vehicle_स्थितिः_s::NAVIGATION_STATE_EXTERNAL5,
	vehicle_स्थितिः_s::NAVIGATION_STATE_EXTERNAL6,
	vehicle_स्थितिः_s::NAVIGATION_STATE_EXTERNAL7,
	vehicle_स्थितिः_s::NAVIGATION_STATE_EXTERNAL8,
};

संरचना escalation_case_s {
	DaaAction action{DaaAction::kDisabled};
	तार्किक hold_allowed{असत्य};
	तार्किक rtl_allowed{असत्य};
	तार्किक land_allowed{असत्य};
	तार्किक terminate_allowed{असत्य};
};

शून्य check_escalation_matrix(ध्रुव escalation_case_s &test_case)
{
	प्रति (ध्रुव uint8_t nav_state : k_अवस्थाः_Enable_Hold) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.hold_allowed) << "action " << (पूर्णाङ्क)test_case.action << " nav_state " << (पूर्णाङ्क)nav_state;
	}

	प्रति (ध्रुव uint8_t nav_state : k_अवस्थाः_Enable_Rtl) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.rtl_allowed) << "action " << (पूर्णाङ्क)test_case.action << " nav_state " << (पूर्णाङ्क)nav_state;
	}

	प्रति (ध्रुव uint8_t nav_state : k_अवस्थाः_Enable_Land) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.land_allowed) << "action " << (पूर्णाङ्क)test_case.action << " nav_state " << (पूर्णाङ्क)nav_state;
	}

	प्रति (ध्रुव uint8_t nav_state : k_अवस्थाः_Enable_Termination) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.terminate_allowed) << "action " << (पूर्णाङ्क)test_case.action << " nav_state " << (पूर्णाङ्क)nav_state;
	}

	प्रति (ध्रुव uint8_t nav_state : k_अवस्थाः_Never_Enable) {
		EXPECT_FALSE(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state))
				<< "action " << (पूर्णाङ्क)test_case.action << " nav_state " << (पूर्णाङ्क)nav_state;
	}
}

शून्य expect_no_response(ध्रुव daa_action_decision_s &decision)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_FALSE(decision.warn_on_ground);
}

शून्य expect_ground_warning(ध्रुव daa_action_decision_s &decision, ध्रुव NotifyLandedActCause expected_cause)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_TRUE(decision.warn_on_ground);
	EXPECT_EQ(decision.ground_warning_cause, expected_cause);
}

शून्य expect_command(ध्रुव daa_action_decision_s &decision, ध्रुव DaaAction expected_action,
		    ध्रुव तार्किक expected_announcement)
{
	EXPECT_EQ(decision.action_command, expected_action);
	EXPECT_EQ(decision.announce_action, expected_announcement);
	EXPECT_FALSE(decision.warn_on_ground);
}

} // namespace

// Param numbering and the internal action ladder map both ways; out-of-range maps to Disabled.
TEST(DaaActionPolicyTest, Action_Param_Mapping_उपयुङ्क्ते_Parameter_Numbering)
{
	संरचना action_param_case_s {
		int32_t param;
		DaaAction action;
	};

	ध्रुव action_param_case_s cases[] {
		{0, DaaAction::kDisabled},
		{1, DaaAction::kWarnOnly},
		{2, DaaAction::k_Return_बहुलकम्},
		{3, DaaAction::k_Land_बहुलकम्},
		{4, DaaAction::k_Position_Hold_बहुलकम्},
		{5, DaaAction::kTerminate},
	};

	प्रति (ध्रुव action_param_case_s &test_case : cases) {
		EXPECT_EQ(DaaActionPolicy::action_param_गन्तव्यम्_daa_action(test_case.param), test_case.action)
				<< "param " << test_case.param;
		EXPECT_EQ(DaaActionPolicy::daa_action_गन्तव्यम्_action_param(test_case.action), test_case.param)
				<< "action " << static_cast<पूर्णाङ्क>(test_case.action);
	}

	// out-of-range -> Disabled
	EXPECT_EQ(DaaActionPolicy::action_param_गन्तव्यम्_daa_action(-1), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_param_गन्तव्यम्_daa_action(6), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::daa_action_गन्तव्यम्_action_param(DaaAction::k_Max_Action_मानम्), 0);
}

// An action escalates only when stronger than the current navigator state (protects manual/offboard).
TEST(DaaActionPolicyTest, ActionEscalatesOnlyAboveNavState)
{
	check_escalation_matrix({DaaAction::kDisabled, असत्य, असत्य, असत्य, असत्य});
	check_escalation_matrix({DaaAction::kWarnOnly, असत्य, असत्य, असत्य, असत्य});
	check_escalation_matrix({DaaAction::k_Position_Hold_बहुलकम्, सत्य, असत्य, असत्य, असत्य});
	check_escalation_matrix({DaaAction::k_Return_बहुलकम्, सत्य, सत्य, असत्य, असत्य});
	check_escalation_matrix({DaaAction::k_Land_बहुलकम्, सत्य, सत्य, सत्य, असत्य});
	check_escalation_matrix({DaaAction::kTerminate, सत्य, सत्य, सत्य, सत्य});
}

// NONE and out-of-range levels never request an action, even with actions configured everywhere.
TEST(DaaActionPolicyTest, No_Action_For_Invalid_स्तराः)
{
	ध्रुव daa_action_params_s params = params_for_all_स्तराः(5); // Terminate everywhere

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_NONE, params),
		  DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL + 1, params),
		  DaaAction::kDisabled);
}

#यदि defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
TEST(DaaActionPolicyTest, F3442_Fallback_उपयुङ्क्ते_Guaranteed_Breached_Zones)
{
	daa_action_params_s params{};
	params.lvl_नीचः_act = 1;		// Warn only
	params.lvl_med_act = 4;		// Hold
	params.lvl_उच्चः_act = 0;	// Disabled
	params.lvl_crit_act = 5;	// Terminate

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_नीचः, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM, params),
		  DaaAction::k_Position_Hold_बहुलकम्);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_उच्चः, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::kTerminate);

	// CRITICAL guarantees that HIGH, MEDIUM, and LOW are also breached.
	params.lvl_crit_act = 0;
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::k_Position_Hold_बहुलकम्);

	// all levels disabled -> fallback chain ends disabled
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			params_for_all_स्तराः(0)), DaaAction::kDisabled);
}
#अन्यथा
// Crosstrack maps every level through the single NAV_TRAFF_AVOID action.
TEST(DaaActionPolicyTest, CrosstrackSingleActionMapping)
{
	ध्रुव daa_action_params_s params = params_for_all_स्तराः(3); // Land

	प्रति (uint8_t level = detect_and_avoid_s::DAA_CONFLICT_LVL_नीचः;
	     level <= detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL; ++level) {
		EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(level, params), DaaAction::k_Land_बहुलकम्);
	}
}
#यदि_अन्तः // CONFIG_NAVIGATOR_ADSB_F3442

// Unchanged level: no command or warning.
TEST(DaaActionPolicyTest, DecideNoResponseWithoutLevelChange)
{
	ध्रुव daa_action_decision_s decision = DaaActionPolicy::decide(
			detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_MISSION, असत्य, सत्य,
			DaaAction::kDisabled, params_for_all_स्तराः(5));

	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, Decide_Warns_On_Ground_Instead_सम्बन्धि_Acting)
{
	ध्रुव uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	ध्रुव uint8_t none = detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	ध्रुव uint8_t nav_state = vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_MISSION;

	// armed -> takeoff warning
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, none, nav_state, सत्य, सत्य,
					 DaaAction::kDisabled, params_for_all_स्तराः(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndArmed);

	// disarmed -> arming warning
	decision = DaaActionPolicy::decide(critical, none, nav_state, सत्य, असत्य,
					   DaaAction::kDisabled, params_for_all_स्तराः(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// Repeated requests allow the caller to emit rate-limited ground warnings.
	decision = DaaActionPolicy::decide(critical, critical, nav_state, सत्य, असत्य,
					   DaaAction::kDisabled, params_for_all_स्तराः(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// warn-only config -> nothing
	decision = DaaActionPolicy::decide(critical, none, nav_state, सत्य, सत्य,
					   DaaAction::kDisabled, params_for_all_स्तराः(1));
	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, Decide_अनुरोधाः_Command_On_Airborne_Escalation_Only)
{
	ध्रुव uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	ध्रुव uint8_t नीचः = detect_and_avoid_s::DAA_CONFLICT_LVL_नीचः;
	ध्रुव daa_action_params_s land_params = params_for_all_स्तराः(3); // Land

	// escalation -> Land command, announced
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, नीचः,
					 vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_MISSION, असत्य, सत्य,
					 DaaAction::kDisabled, land_params);
	expect_command(decision, DaaAction::k_Land_बहुलकम्, सत्य);

	// A later level escalation that maps to the same action is not re-announced.
	decision = DaaActionPolicy::decide(critical, नीचः, vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_MISSION,
					   असत्य, सत्य, DaaAction::k_Land_बहुलकम्, land_params);
	expect_command(decision, DaaAction::k_Land_बहुलकम्, असत्य);

	// The first escalation after all conflicts cleared starts a new action episode.
	decision = DaaActionPolicy::decide(critical, detect_and_avoid_s::DAA_CONFLICT_LVL_NONE,
					   vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_MISSION,
					   असत्य, सत्य, DaaAction::k_Land_बहुलकम्, land_params);
	expect_command(decision, DaaAction::k_Land_बहुलकम्, सत्य);

	// already in a stronger mode -> nothing
	decision = DaaActionPolicy::decide(critical, नीचः, vehicle_स्थितिः_s::NAVIGATION_STATE_TERMINATION,
					   असत्य, सत्य, DaaAction::kDisabled, land_params);
	expect_no_response(decision);

	// de-escalation -> nothing
	decision = DaaActionPolicy::decide(नीचः, critical, vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_MISSION,
					   असत्य, सत्य, DaaAction::k_Land_बहुलकम्, land_params);
	expect_no_response(decision);
}
