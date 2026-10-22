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

#అన్తర్భావ <gtest/gtest.h>

#అన్తర్భావ <lib/adsb/DaaActionPolicy.h>
#అన్తర్భావ <uORB/topics/detect_and_avoid.h>
#అన్తర్భావ <uORB/topics/vehicle_status.h>

నామాకాశ
{

// Same action parameter value for every conflict level, so the decide() tests
// behave identically in F3442 and Crosstrack builds.
daa_action_params_s params_for_all_స్థాయిలు(స్థిరరాశి int32_t action_param)
{
	daa_action_params_s params{};
#ఒకవేళ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	params.lvl_దిగువ_act = action_param;
	params.lvl_med_act = action_param;
	params.lvl_ఎగువ_act = action_param;
	params.lvl_crit_act = action_param;
#లేకపోతే
	params.traff_avoid = action_param;
#అయితే_ముగింపు // CONFIG_NAVIGATOR_ADSB_F3442
	తిరిగి params;
}

// Navigation state groups by the weakest DAA action allowed to escalate above them.
స్థిరరాశి uint8_t k_స్థితులు_Enable_Hold[] {
	vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_MISSION,
	vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_TAKEOFF,
	vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_FOLLOW_లక్ష్యం,
	vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF,
	vehicle_స్థితి_s::NAVIGATION_STATE_GUIDED_కోర్సు,
};

స్థిరరాశి uint8_t k_స్థితులు_Enable_Rtl[] {
	vehicle_స్థితి_s::NAVIGATION_STATE_ORBIT,
	vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_LOITER,
};

స్థిరరాశి uint8_t k_స్థితులు_Enable_Land[] {
	vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_RTL,
};

స్థిరరాశి uint8_t k_స్థితులు_Enable_Termination[] {
	vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_LAND,
	vehicle_స్థితి_s::NAVIGATION_STATE_DESCEND,
	vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_PRECLAND,
	vehicle_స్థితి_s::NAVIGATION_STATE_MANUAL,
	vehicle_స్థితి_s::NAVIGATION_STATE_ALTCTL,
	vehicle_స్థితి_s::NAVIGATION_STATE_ALTITUDE_CRUISE,
	vehicle_స్థితి_s::NAVIGATION_STATE_MANUAL_PARKING,
	vehicle_స్థితి_s::NAVIGATION_STATE_POSCTL,
	vehicle_స్థితి_s::NAVIGATION_STATE_POSITION_SLOW,
	vehicle_స్థితి_s::NAVIGATION_STATE_ACRO,
	vehicle_స్థితి_s::NAVIGATION_STATE_STAB,
};

స్థిరరాశి uint8_t k_స్థితులు_Never_Enable[] {
	vehicle_స్థితి_s::NAVIGATION_STATE_TERMINATION,
	vehicle_స్థితి_s::NAVIGATION_STATE_OFFBOARD,
	vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL1,
	vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL2,
	vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL3,
	vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL4,
	vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL5,
	vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL6,
	vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL7,
	vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL8,
};

నిర్మాణం escalation_case_s {
	DaaAction action{DaaAction::kDisabled};
	తార్కిక hold_allowed{అబద్ధం};
	తార్కిక rtl_allowed{అబద్ధం};
	తార్కిక land_allowed{అబద్ధం};
	తార్కిక terminate_allowed{అబద్ధం};
};

శూన్య check_escalation_మాతృక(స్థిరరాశి escalation_case_s &test_case)
{
	కొరకు (స్థిరరాశి uint8_t nav_state : k_స్థితులు_Enable_Hold) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.hold_allowed) << "action " << (పూర్ణాంకం)test_case.action << " nav_state " << (పూర్ణాంకం)nav_state;
	}

	కొరకు (స్థిరరాశి uint8_t nav_state : k_స్థితులు_Enable_Rtl) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.rtl_allowed) << "action " << (పూర్ణాంకం)test_case.action << " nav_state " << (పూర్ణాంకం)nav_state;
	}

	కొరకు (స్థిరరాశి uint8_t nav_state : k_స్థితులు_Enable_Land) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.land_allowed) << "action " << (పూర్ణాంకం)test_case.action << " nav_state " << (పూర్ణాంకం)nav_state;
	}

	కొరకు (స్థిరరాశి uint8_t nav_state : k_స్థితులు_Enable_Termination) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.terminate_allowed) << "action " << (పూర్ణాంకం)test_case.action << " nav_state " << (పూర్ణాంకం)nav_state;
	}

	కొరకు (స్థిరరాశి uint8_t nav_state : k_స్థితులు_Never_Enable) {
		EXPECT_FALSE(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state))
				<< "action " << (పూర్ణాంకం)test_case.action << " nav_state " << (పూర్ణాంకం)nav_state;
	}
}

శూన్య expect_no_response(స్థిరరాశి daa_action_decision_s &decision)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_FALSE(decision.warn_on_ground);
}

శూన్య expect_ground_warning(స్థిరరాశి daa_action_decision_s &decision, స్థిరరాశి NotifyLandedActCause expected_cause)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_TRUE(decision.warn_on_ground);
	EXPECT_EQ(decision.ground_warning_cause, expected_cause);
}

శూన్య expect_command(స్థిరరాశి daa_action_decision_s &decision, స్థిరరాశి DaaAction expected_action,
		    స్థిరరాశి తార్కిక expected_announcement)
{
	EXPECT_EQ(decision.action_command, expected_action);
	EXPECT_EQ(decision.announce_action, expected_announcement);
	EXPECT_FALSE(decision.warn_on_ground);
}

} // namespace

// Param numbering and the internal action ladder map both ways; out-of-range maps to Disabled.
TEST(DaaActionPolicyTest, Action_Param_Mapping_ఉపయోగిస్తుంది_Parameter_Numbering)
{
	నిర్మాణం action_param_case_s {
		int32_t param;
		DaaAction action;
	};

	స్థిరరాశి action_param_case_s cases[] {
		{0, DaaAction::kDisabled},
		{1, DaaAction::kWarnOnly},
		{2, DaaAction::k_Return_బహుళకం},
		{3, DaaAction::k_Land_బహుళకం},
		{4, DaaAction::k_Position_Hold_బహుళకం},
		{5, DaaAction::kTerminate},
	};

	కొరకు (స్థిరరాశి action_param_case_s &test_case : cases) {
		EXPECT_EQ(DaaActionPolicy::action_param_గమ్యం_daa_action(test_case.param), test_case.action)
				<< "param " << test_case.param;
		EXPECT_EQ(DaaActionPolicy::daa_action_గమ్యం_action_param(test_case.action), test_case.param)
				<< "action " << static_cast<పూర్ణాంకం>(test_case.action);
	}

	// out-of-range -> Disabled
	EXPECT_EQ(DaaActionPolicy::action_param_గమ్యం_daa_action(-1), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_param_గమ్యం_daa_action(6), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::daa_action_గమ్యం_action_param(DaaAction::k_Max_Action_విలువ), 0);
}

// An action escalates only when stronger than the current navigator state (protects manual/offboard).
TEST(DaaActionPolicyTest, ActionEscalatesOnlyAboveNavState)
{
	check_escalation_మాతృక({DaaAction::kDisabled, అబద్ధం, అబద్ధం, అబద్ధం, అబద్ధం});
	check_escalation_మాతృక({DaaAction::kWarnOnly, అబద్ధం, అబద్ధం, అబద్ధం, అబద్ధం});
	check_escalation_మాతృక({DaaAction::k_Position_Hold_బహుళకం, నిజం, అబద్ధం, అబద్ధం, అబద్ధం});
	check_escalation_మాతృక({DaaAction::k_Return_బహుళకం, నిజం, నిజం, అబద్ధం, అబద్ధం});
	check_escalation_మాతృక({DaaAction::k_Land_బహుళకం, నిజం, నిజం, నిజం, అబద్ధం});
	check_escalation_మాతృక({DaaAction::kTerminate, నిజం, నిజం, నిజం, నిజం});
}

// NONE and out-of-range levels never request an action, even with actions configured everywhere.
TEST(DaaActionPolicyTest, No_Action_For_Invalid_స్థాయిలు)
{
	స్థిరరాశి daa_action_params_s params = params_for_all_స్థాయిలు(5); // Terminate everywhere

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_NONE, params),
		  DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL + 1, params),
		  DaaAction::kDisabled);
}

#ఒకవేళ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
TEST(DaaActionPolicyTest, F3442_Fallback_ఉపయోగిస్తుంది_Guaranteed_Breached_Zones)
{
	daa_action_params_s params{};
	params.lvl_దిగువ_act = 1;		// Warn only
	params.lvl_med_act = 4;		// Hold
	params.lvl_ఎగువ_act = 0;	// Disabled
	params.lvl_crit_act = 5;	// Terminate

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_దిగువ, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM, params),
		  DaaAction::k_Position_Hold_బహుళకం);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_ఎగువ, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::kTerminate);

	// CRITICAL guarantees that HIGH, MEDIUM, and LOW are also breached.
	params.lvl_crit_act = 0;
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::k_Position_Hold_బహుళకం);

	// all levels disabled -> fallback chain ends disabled
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			params_for_all_స్థాయిలు(0)), DaaAction::kDisabled);
}
#లేకపోతే
// Crosstrack maps every level through the single NAV_TRAFF_AVOID action.
TEST(DaaActionPolicyTest, CrosstrackSingleActionMapping)
{
	స్థిరరాశి daa_action_params_s params = params_for_all_స్థాయిలు(3); // Land

	కొరకు (uint8_t level = detect_and_avoid_s::DAA_CONFLICT_LVL_దిగువ;
	     level <= detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL; ++level) {
		EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(level, params), DaaAction::k_Land_బహుళకం);
	}
}
#అయితే_ముగింపు // CONFIG_NAVIGATOR_ADSB_F3442

// Unchanged level: no command or warning.
TEST(DaaActionPolicyTest, DecideNoResponseWithoutLevelChange)
{
	స్థిరరాశి daa_action_decision_s decision = DaaActionPolicy::decide(
			detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_MISSION, అబద్ధం, నిజం,
			DaaAction::kDisabled, params_for_all_స్థాయిలు(5));

	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, Decide_Warns_On_Ground_Instead_యొక్క_Acting)
{
	స్థిరరాశి uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	స్థిరరాశి uint8_t none = detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	స్థిరరాశి uint8_t nav_state = vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_MISSION;

	// armed -> takeoff warning
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, none, nav_state, నిజం, నిజం,
					 DaaAction::kDisabled, params_for_all_స్థాయిలు(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndArmed);

	// disarmed -> arming warning
	decision = DaaActionPolicy::decide(critical, none, nav_state, నిజం, అబద్ధం,
					   DaaAction::kDisabled, params_for_all_స్థాయిలు(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// Repeated requests allow the caller to emit rate-limited ground warnings.
	decision = DaaActionPolicy::decide(critical, critical, nav_state, నిజం, అబద్ధం,
					   DaaAction::kDisabled, params_for_all_స్థాయిలు(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// warn-only config -> nothing
	decision = DaaActionPolicy::decide(critical, none, nav_state, నిజం, నిజం,
					   DaaAction::kDisabled, params_for_all_స్థాయిలు(1));
	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, Decide_అభ్యర్థనలు_Command_On_Airborne_Escalation_Only)
{
	స్థిరరాశి uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	స్థిరరాశి uint8_t దిగువ = detect_and_avoid_s::DAA_CONFLICT_LVL_దిగువ;
	స్థిరరాశి daa_action_params_s land_params = params_for_all_స్థాయిలు(3); // Land

	// escalation -> Land command, announced
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, దిగువ,
					 vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_MISSION, అబద్ధం, నిజం,
					 DaaAction::kDisabled, land_params);
	expect_command(decision, DaaAction::k_Land_బహుళకం, నిజం);

	// A later level escalation that maps to the same action is not re-announced.
	decision = DaaActionPolicy::decide(critical, దిగువ, vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_MISSION,
					   అబద్ధం, నిజం, DaaAction::k_Land_బహుళకం, land_params);
	expect_command(decision, DaaAction::k_Land_బహుళకం, అబద్ధం);

	// The first escalation after all conflicts cleared starts a new action episode.
	decision = DaaActionPolicy::decide(critical, detect_and_avoid_s::DAA_CONFLICT_LVL_NONE,
					   vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_MISSION,
					   అబద్ధం, నిజం, DaaAction::k_Land_బహుళకం, land_params);
	expect_command(decision, DaaAction::k_Land_బహుళకం, నిజం);

	// already in a stronger mode -> nothing
	decision = DaaActionPolicy::decide(critical, దిగువ, vehicle_స్థితి_s::NAVIGATION_STATE_TERMINATION,
					   అబద్ధం, నిజం, DaaAction::kDisabled, land_params);
	expect_no_response(decision);

	// de-escalation -> nothing
	decision = DaaActionPolicy::decide(దిగువ, critical, vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_MISSION,
					   అబద్ధం, నిజం, DaaAction::k_Land_బహుళకం, land_params);
	expect_no_response(decision);
}
