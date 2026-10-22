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

#شامل <gtest/gtest.h>

#شامل <lib/adsb/DaaActionPolicy.h>
#شامل <uORB/topics/detect_and_avoid.h>
#شامل <uORB/topics/vehicle_status.h>

نامخلا
{

// Same action parameter value for every conflict level, so the decide() tests
// behave identically in F3442 and Crosstrack builds.
daa_action_params_s params_for_all_سطحیں(ثابت int32_t action_param)
{
	daa_action_params_s params{};
#اگر defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	params.lvl_کم_act = action_param;
	params.lvl_med_act = action_param;
	params.lvl_بلند_act = action_param;
	params.lvl_crit_act = action_param;
#ورنہ
	params.traff_avoid = action_param;
#اگر_ختم // CONFIG_NAVIGATOR_ADSB_F3442
	واپس params;
}

// Navigation state groups by the weakest DAA action allowed to escalate above them.
ثابت uint8_t k_حالتیں_Enable_Hold[] {
	vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION,
	vehicle_حالت_s::NAVIGATION_STATE_AUTO_TAKEOFF,
	vehicle_حالت_s::NAVIGATION_STATE_AUTO_FOLLOW_ہدف,
	vehicle_حالت_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF,
	vehicle_حالت_s::NAVIGATION_STATE_GUIDED_نصاب,
};

ثابت uint8_t k_حالتیں_Enable_Rtl[] {
	vehicle_حالت_s::NAVIGATION_STATE_ORBIT,
	vehicle_حالت_s::NAVIGATION_STATE_AUTO_LOITER,
};

ثابت uint8_t k_حالتیں_Enable_Land[] {
	vehicle_حالت_s::NAVIGATION_STATE_AUTO_RTL,
};

ثابت uint8_t k_حالتیں_Enable_Termination[] {
	vehicle_حالت_s::NAVIGATION_STATE_AUTO_LAND,
	vehicle_حالت_s::NAVIGATION_STATE_DESCEND,
	vehicle_حالت_s::NAVIGATION_STATE_AUTO_PRECLAND,
	vehicle_حالت_s::NAVIGATION_STATE_MANUAL,
	vehicle_حالت_s::NAVIGATION_STATE_ALTCTL,
	vehicle_حالت_s::NAVIGATION_STATE_ALTITUDE_CRUISE,
	vehicle_حالت_s::NAVIGATION_STATE_MANUAL_PARKING,
	vehicle_حالت_s::NAVIGATION_STATE_POSCTL,
	vehicle_حالت_s::NAVIGATION_STATE_POSITION_SLOW,
	vehicle_حالت_s::NAVIGATION_STATE_ACRO,
	vehicle_حالت_s::NAVIGATION_STATE_STAB,
};

ثابت uint8_t k_حالتیں_Never_Enable[] {
	vehicle_حالت_s::NAVIGATION_STATE_TERMINATION,
	vehicle_حالت_s::NAVIGATION_STATE_OFFBOARD,
	vehicle_حالت_s::NAVIGATION_STATE_EXTERNAL1,
	vehicle_حالت_s::NAVIGATION_STATE_EXTERNAL2,
	vehicle_حالت_s::NAVIGATION_STATE_EXTERNAL3,
	vehicle_حالت_s::NAVIGATION_STATE_EXTERNAL4,
	vehicle_حالت_s::NAVIGATION_STATE_EXTERNAL5,
	vehicle_حالت_s::NAVIGATION_STATE_EXTERNAL6,
	vehicle_حالت_s::NAVIGATION_STATE_EXTERNAL7,
	vehicle_حالت_s::NAVIGATION_STATE_EXTERNAL8,
};

ساخت escalation_case_s {
	DaaAction action{DaaAction::kDisabled};
	منطقی hold_allowed{جھوٹ};
	منطقی rtl_allowed{جھوٹ};
	منطقی land_allowed{جھوٹ};
	منطقی terminate_allowed{جھوٹ};
};

خالی check_escalation_میٹرکس(ثابت escalation_case_s &test_case)
{
	برائے (ثابت uint8_t nav_state : k_حالتیں_Enable_Hold) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.hold_allowed) << "action " << (صحیح)test_case.action << " nav_state " << (صحیح)nav_state;
	}

	برائے (ثابت uint8_t nav_state : k_حالتیں_Enable_Rtl) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.rtl_allowed) << "action " << (صحیح)test_case.action << " nav_state " << (صحیح)nav_state;
	}

	برائے (ثابت uint8_t nav_state : k_حالتیں_Enable_Land) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.land_allowed) << "action " << (صحیح)test_case.action << " nav_state " << (صحیح)nav_state;
	}

	برائے (ثابت uint8_t nav_state : k_حالتیں_Enable_Termination) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.terminate_allowed) << "action " << (صحیح)test_case.action << " nav_state " << (صحیح)nav_state;
	}

	برائے (ثابت uint8_t nav_state : k_حالتیں_Never_Enable) {
		EXPECT_FALSE(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state))
				<< "action " << (صحیح)test_case.action << " nav_state " << (صحیح)nav_state;
	}
}

خالی expect_no_response(ثابت daa_action_decision_s &decision)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_FALSE(decision.warn_on_ground);
}

خالی expect_ground_warning(ثابت daa_action_decision_s &decision, ثابت NotifyLandedActCause expected_cause)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_TRUE(decision.warn_on_ground);
	EXPECT_EQ(decision.ground_warning_cause, expected_cause);
}

خالی expect_command(ثابت daa_action_decision_s &decision, ثابت DaaAction expected_action,
		    ثابت منطقی expected_announcement)
{
	EXPECT_EQ(decision.action_command, expected_action);
	EXPECT_EQ(decision.announce_action, expected_announcement);
	EXPECT_FALSE(decision.warn_on_ground);
}

} // namespace

// Param numbering and the internal action ladder map both ways; out-of-range maps to Disabled.
TEST(DaaActionPolicyTest, Action_Param_Mapping_استعمال_کرتا_Parameter_Numbering)
{
	ساخت action_param_case_s {
		int32_t param;
		DaaAction action;
	};

	ثابت action_param_case_s cases[] {
		{0, DaaAction::kDisabled},
		{1, DaaAction::kWarnOnly},
		{2, DaaAction::k_Return_منوال},
		{3, DaaAction::k_Land_منوال},
		{4, DaaAction::k_Position_Hold_منوال},
		{5, DaaAction::kTerminate},
	};

	برائے (ثابت action_param_case_s &test_case : cases) {
		EXPECT_EQ(DaaActionPolicy::action_param_مقصد_daa_action(test_case.param), test_case.action)
				<< "param " << test_case.param;
		EXPECT_EQ(DaaActionPolicy::daa_action_مقصد_action_param(test_case.action), test_case.param)
				<< "action " << static_cast<صحیح>(test_case.action);
	}

	// out-of-range -> Disabled
	EXPECT_EQ(DaaActionPolicy::action_param_مقصد_daa_action(-1), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_param_مقصد_daa_action(6), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::daa_action_مقصد_action_param(DaaAction::k_Max_Action_قدر), 0);
}

// An action escalates only when stronger than the current navigator state (protects manual/offboard).
TEST(DaaActionPolicyTest, ActionEscalatesOnlyAboveNavState)
{
	check_escalation_میٹرکس({DaaAction::kDisabled, جھوٹ, جھوٹ, جھوٹ, جھوٹ});
	check_escalation_میٹرکس({DaaAction::kWarnOnly, جھوٹ, جھوٹ, جھوٹ, جھوٹ});
	check_escalation_میٹرکس({DaaAction::k_Position_Hold_منوال, سچ, جھوٹ, جھوٹ, جھوٹ});
	check_escalation_میٹرکس({DaaAction::k_Return_منوال, سچ, سچ, جھوٹ, جھوٹ});
	check_escalation_میٹرکس({DaaAction::k_Land_منوال, سچ, سچ, سچ, جھوٹ});
	check_escalation_میٹرکس({DaaAction::kTerminate, سچ, سچ, سچ, سچ});
}

// NONE and out-of-range levels never request an action, even with actions configured everywhere.
TEST(DaaActionPolicyTest, No_Action_For_Invalid_سطحیں)
{
	ثابت daa_action_params_s params = params_for_all_سطحیں(5); // Terminate everywhere

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_NONE, params),
		  DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL + 1, params),
		  DaaAction::kDisabled);
}

#اگر defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
TEST(DaaActionPolicyTest, F3442_Fallback_استعمال_کرتا_Guaranteed_Breached_Zones)
{
	daa_action_params_s params{};
	params.lvl_کم_act = 1;		// Warn only
	params.lvl_med_act = 4;		// Hold
	params.lvl_بلند_act = 0;	// Disabled
	params.lvl_crit_act = 5;	// Terminate

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_کم, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM, params),
		  DaaAction::k_Position_Hold_منوال);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_بلند, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::kTerminate);

	// CRITICAL guarantees that HIGH, MEDIUM, and LOW are also breached.
	params.lvl_crit_act = 0;
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::k_Position_Hold_منوال);

	// all levels disabled -> fallback chain ends disabled
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			params_for_all_سطحیں(0)), DaaAction::kDisabled);
}
#ورنہ
// Crosstrack maps every level through the single NAV_TRAFF_AVOID action.
TEST(DaaActionPolicyTest, CrosstrackSingleActionMapping)
{
	ثابت daa_action_params_s params = params_for_all_سطحیں(3); // Land

	برائے (uint8_t level = detect_and_avoid_s::DAA_CONFLICT_LVL_کم;
	     level <= detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL; ++level) {
		EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(level, params), DaaAction::k_Land_منوال);
	}
}
#اگر_ختم // CONFIG_NAVIGATOR_ADSB_F3442

// Unchanged level: no command or warning.
TEST(DaaActionPolicyTest, DecideNoResponseWithoutLevelChange)
{
	ثابت daa_action_decision_s decision = DaaActionPolicy::decide(
			detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION, جھوٹ, سچ,
			DaaAction::kDisabled, params_for_all_سطحیں(5));

	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, Decide_Warns_On_Ground_Instead_کا_Acting)
{
	ثابت uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	ثابت uint8_t none = detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	ثابت uint8_t nav_state = vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION;

	// armed -> takeoff warning
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, none, nav_state, سچ, سچ,
					 DaaAction::kDisabled, params_for_all_سطحیں(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndArmed);

	// disarmed -> arming warning
	decision = DaaActionPolicy::decide(critical, none, nav_state, سچ, جھوٹ,
					   DaaAction::kDisabled, params_for_all_سطحیں(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// Repeated requests allow the caller to emit rate-limited ground warnings.
	decision = DaaActionPolicy::decide(critical, critical, nav_state, سچ, جھوٹ,
					   DaaAction::kDisabled, params_for_all_سطحیں(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// warn-only config -> nothing
	decision = DaaActionPolicy::decide(critical, none, nav_state, سچ, سچ,
					   DaaAction::kDisabled, params_for_all_سطحیں(1));
	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, Decide_درخواستیں_Command_On_Airborne_Escalation_Only)
{
	ثابت uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	ثابت uint8_t کم = detect_and_avoid_s::DAA_CONFLICT_LVL_کم;
	ثابت daa_action_params_s land_params = params_for_all_سطحیں(3); // Land

	// escalation -> Land command, announced
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, کم,
					 vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION, جھوٹ, سچ,
					 DaaAction::kDisabled, land_params);
	expect_command(decision, DaaAction::k_Land_منوال, سچ);

	// A later level escalation that maps to the same action is not re-announced.
	decision = DaaActionPolicy::decide(critical, کم, vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION,
					   جھوٹ, سچ, DaaAction::k_Land_منوال, land_params);
	expect_command(decision, DaaAction::k_Land_منوال, جھوٹ);

	// The first escalation after all conflicts cleared starts a new action episode.
	decision = DaaActionPolicy::decide(critical, detect_and_avoid_s::DAA_CONFLICT_LVL_NONE,
					   vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION,
					   جھوٹ, سچ, DaaAction::k_Land_منوال, land_params);
	expect_command(decision, DaaAction::k_Land_منوال, سچ);

	// already in a stronger mode -> nothing
	decision = DaaActionPolicy::decide(critical, کم, vehicle_حالت_s::NAVIGATION_STATE_TERMINATION,
					   جھوٹ, سچ, DaaAction::kDisabled, land_params);
	expect_no_response(decision);

	// de-escalation -> nothing
	decision = DaaActionPolicy::decide(کم, critical, vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION,
					   جھوٹ, سچ, DaaAction::k_Land_منوال, land_params);
	expect_no_response(decision);
}
