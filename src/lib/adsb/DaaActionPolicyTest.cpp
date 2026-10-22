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

#ଅନ୍ତର୍ଭାଵ <gtest/gtest.h>

#ଅନ୍ତର୍ଭାଵ <lib/adsb/DaaActionPolicy.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/detect_and_avoid.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/vehicle_status.h>

ନାମାକାଶ
{

// Same action parameter value for every conflict level, so the decide() tests
// behave identically in F3442 and Crosstrack builds.
daa_action_params_s params_for_all_levels(ଅଚଳ int32_t action_param)
{
	daa_action_params_s params{};
#ଯଦି defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	params.lvl_ତଳ_act = action_param;
	params.lvl_med_act = action_param;
	params.lvl_ଉପର_act = action_param;
	params.lvl_crit_act = action_param;
#ଅନ୍ୟଥା
	params.traff_avoid = action_param;
#endif // CONFIG_NAVIGATOR_ADSB_F3442
	ଫେରନ୍ତୁ params;
}

// Navigation state groups by the weakest DAA action allowed to escalate above them.
ଅଚଳ uint8_t kStatesEnableHold[] {
	vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION,
	vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF,
	vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET,
	vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF,
	vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE,
};

ଅଚଳ uint8_t kStatesEnableRtl[] {
	vehicle_status_s::NAVIGATION_STATE_ORBIT,
	vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER,
};

ଅଚଳ uint8_t kStatesEnableLand[] {
	vehicle_status_s::NAVIGATION_STATE_AUTO_RTL,
};

ଅଚଳ uint8_t kStatesEnableTermination[] {
	vehicle_status_s::NAVIGATION_STATE_AUTO_LAND,
	vehicle_status_s::NAVIGATION_STATE_DESCEND,
	vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND,
	vehicle_status_s::NAVIGATION_STATE_MANUAL,
	vehicle_status_s::NAVIGATION_STATE_ALTCTL,
	vehicle_status_s::NAVIGATION_STATE_ALTITUDE_CRUISE,
	vehicle_status_s::NAVIGATION_STATE_MANUAL_PARKING,
	vehicle_status_s::NAVIGATION_STATE_POSCTL,
	vehicle_status_s::NAVIGATION_STATE_POSITION_SLOW,
	vehicle_status_s::NAVIGATION_STATE_ACRO,
	vehicle_status_s::NAVIGATION_STATE_STAB,
};

ଅଚଳ uint8_t kStatesNeverEnable[] {
	vehicle_status_s::NAVIGATION_STATE_TERMINATION,
	vehicle_status_s::NAVIGATION_STATE_OFFBOARD,
	vehicle_status_s::NAVIGATION_STATE_EXTERNAL1,
	vehicle_status_s::NAVIGATION_STATE_EXTERNAL2,
	vehicle_status_s::NAVIGATION_STATE_EXTERNAL3,
	vehicle_status_s::NAVIGATION_STATE_EXTERNAL4,
	vehicle_status_s::NAVIGATION_STATE_EXTERNAL5,
	vehicle_status_s::NAVIGATION_STATE_EXTERNAL6,
	vehicle_status_s::NAVIGATION_STATE_EXTERNAL7,
	vehicle_status_s::NAVIGATION_STATE_EXTERNAL8,
};

ସଂରଚନା escalation_case_s {
	DaaAction action{DaaAction::kDisabled};
	ତାର୍କିକ hold_allowed{ମିଥ୍ୟା};
	ତାର୍କିକ rtl_allowed{ମିଥ୍ୟା};
	ତାର୍କିକ land_allowed{ମିଥ୍ୟା};
	ତାର୍କିକ terminate_allowed{ମିଥ୍ୟା};
};

ଶୂନ୍ୟ check_escalation_matrix(ଅଚଳ escalation_case_s &test_case)
{
	ପାଇଁ (ଅଚଳ uint8_t nav_state : kStatesEnableHold) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.hold_allowed) << "action " << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)test_case.action << " nav_state " << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nav_state;
	}

	ପାଇଁ (ଅଚଳ uint8_t nav_state : kStatesEnableRtl) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.rtl_allowed) << "action " << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)test_case.action << " nav_state " << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nav_state;
	}

	ପାଇଁ (ଅଚଳ uint8_t nav_state : kStatesEnableLand) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.land_allowed) << "action " << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)test_case.action << " nav_state " << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nav_state;
	}

	ପାଇଁ (ଅଚଳ uint8_t nav_state : kStatesEnableTermination) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.terminate_allowed) << "action " << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)test_case.action << " nav_state " << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nav_state;
	}

	ପାଇଁ (ଅଚଳ uint8_t nav_state : kStatesNeverEnable) {
		EXPECT_FALSE(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state))
				<< "action " << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)test_case.action << " nav_state " << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nav_state;
	}
}

ଶୂନ୍ୟ expect_no_response(ଅଚଳ daa_action_decision_s &decision)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_FALSE(decision.warn_on_ground);
}

ଶୂନ୍ୟ expect_ground_warning(ଅଚଳ daa_action_decision_s &decision, ଅଚଳ NotifyLandedActCause expected_cause)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_TRUE(decision.warn_on_ground);
	EXPECT_EQ(decision.ground_warning_cause, expected_cause);
}

ଶୂନ୍ୟ expect_command(ଅଚଳ daa_action_decision_s &decision, ଅଚଳ DaaAction expected_action,
		    ଅଚଳ ତାର୍କିକ expected_announcement)
{
	EXPECT_EQ(decision.action_command, expected_action);
	EXPECT_EQ(decision.announce_action, expected_announcement);
	EXPECT_FALSE(decision.warn_on_ground);
}

} // namespace

// Param numbering and the internal action ladder map both ways; out-of-range maps to Disabled.
TEST(DaaActionPolicyTest, ActionParamMappingUsesParameterNumbering)
{
	ସଂରଚନା action_param_case_s {
		int32_t param;
		DaaAction action;
	};

	ଅଚଳ action_param_case_s cases[] {
		{0, DaaAction::kDisabled},
		{1, DaaAction::kWarnOnly},
		{2, DaaAction::kReturnMode},
		{3, DaaAction::kLandMode},
		{4, DaaAction::kPositionHoldMode},
		{5, DaaAction::kTerminate},
	};

	ପାଇଁ (ଅଚଳ action_param_case_s &test_case : cases) {
		EXPECT_EQ(DaaActionPolicy::action_param_to_daa_action(test_case.param), test_case.action)
				<< "param " << test_case.param;
		EXPECT_EQ(DaaActionPolicy::daa_action_to_action_param(test_case.action), test_case.param)
				<< "action " << static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(test_case.action);
	}

	// out-of-range -> Disabled
	EXPECT_EQ(DaaActionPolicy::action_param_to_daa_action(-1), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_param_to_daa_action(6), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::daa_action_to_action_param(DaaAction::k_Max_Action_ମୂଲ୍ୟ), 0);
}

// An action escalates only when stronger than the current navigator state (protects manual/offboard).
TEST(DaaActionPolicyTest, ActionEscalatesOnlyAboveNavState)
{
	check_escalation_matrix({DaaAction::kDisabled, ମିଥ୍ୟା, ମିଥ୍ୟା, ମିଥ୍ୟା, ମିଥ୍ୟା});
	check_escalation_matrix({DaaAction::kWarnOnly, ମିଥ୍ୟା, ମିଥ୍ୟା, ମିଥ୍ୟା, ମିଥ୍ୟା});
	check_escalation_matrix({DaaAction::kPositionHoldMode, ସତ୍ୟ, ମିଥ୍ୟା, ମିଥ୍ୟା, ମିଥ୍ୟା});
	check_escalation_matrix({DaaAction::kReturnMode, ସତ୍ୟ, ସତ୍ୟ, ମିଥ୍ୟା, ମିଥ୍ୟା});
	check_escalation_matrix({DaaAction::kLandMode, ସତ୍ୟ, ସତ୍ୟ, ସତ୍ୟ, ମିଥ୍ୟା});
	check_escalation_matrix({DaaAction::kTerminate, ସତ୍ୟ, ସତ୍ୟ, ସତ୍ୟ, ସତ୍ୟ});
}

// NONE and out-of-range levels never request an action, even with actions configured everywhere.
TEST(DaaActionPolicyTest, NoActionForInvalidLevels)
{
	ଅଚଳ daa_action_params_s params = params_for_all_levels(5); // Terminate everywhere

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_NONE, params),
		  DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL + 1, params),
		  DaaAction::kDisabled);
}

#ଯଦି defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
TEST(DaaActionPolicyTest, F3442FallbackUsesGuaranteedBreachedZones)
{
	daa_action_params_s params{};
	params.lvl_ତଳ_act = 1;		// Warn only
	params.lvl_med_act = 4;		// Hold
	params.lvl_ଉପର_act = 0;	// Disabled
	params.lvl_crit_act = 5;	// Terminate

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_ତଳ, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM, params),
		  DaaAction::kPositionHoldMode);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_ଉପର, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::kTerminate);

	// CRITICAL guarantees that HIGH, MEDIUM, and LOW are also breached.
	params.lvl_crit_act = 0;
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, params),
		  DaaAction::kPositionHoldMode);

	// all levels disabled -> fallback chain ends disabled
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			params_for_all_levels(0)), DaaAction::kDisabled);
}
#ଅନ୍ୟଥା
// Crosstrack maps every level through the single NAV_TRAFF_AVOID action.
TEST(DaaActionPolicyTest, CrosstrackSingleActionMapping)
{
	ଅଚଳ daa_action_params_s params = params_for_all_levels(3); // Land

	ପାଇଁ (uint8_t level = detect_and_avoid_s::DAA_CONFLICT_LVL_ତଳ;
	     level <= detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL; ++level) {
		EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(level, params), DaaAction::kLandMode);
	}
}
#endif // CONFIG_NAVIGATOR_ADSB_F3442

// Unchanged level: no command or warning.
TEST(DaaActionPolicyTest, DecideNoResponseWithoutLevelChange)
{
	ଅଚଳ daa_action_decision_s decision = DaaActionPolicy::decide(
			detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION, ମିଥ୍ୟା, ସତ୍ୟ,
			DaaAction::kDisabled, params_for_all_levels(5));

	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, DecideWarnsOnGroundInsteadOfActing)
{
	ଅଚଳ uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	ଅଚଳ uint8_t none = detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	ଅଚଳ uint8_t nav_state = vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION;

	// armed -> takeoff warning
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, none, nav_state, ସତ୍ୟ, ସତ୍ୟ,
					 DaaAction::kDisabled, params_for_all_levels(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndArmed);

	// disarmed -> arming warning
	decision = DaaActionPolicy::decide(critical, none, nav_state, ସତ୍ୟ, ମିଥ୍ୟା,
					   DaaAction::kDisabled, params_for_all_levels(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// Repeated requests allow the caller to emit rate-limited ground warnings.
	decision = DaaActionPolicy::decide(critical, critical, nav_state, ସତ୍ୟ, ମିଥ୍ୟା,
					   DaaAction::kDisabled, params_for_all_levels(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// warn-only config -> nothing
	decision = DaaActionPolicy::decide(critical, none, nav_state, ସତ୍ୟ, ସତ୍ୟ,
					   DaaAction::kDisabled, params_for_all_levels(1));
	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, DecideRequestsCommandOnAirborneEscalationOnly)
{
	ଅଚଳ uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	ଅଚଳ uint8_t ତଳ = detect_and_avoid_s::DAA_CONFLICT_LVL_ତଳ;
	ଅଚଳ daa_action_params_s land_params = params_for_all_levels(3); // Land

	// escalation -> Land command, announced
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, ତଳ,
					 vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION, ମିଥ୍ୟା, ସତ୍ୟ,
					 DaaAction::kDisabled, land_params);
	expect_command(decision, DaaAction::kLandMode, ସତ୍ୟ);

	// A later level escalation that maps to the same action is not re-announced.
	decision = DaaActionPolicy::decide(critical, ତଳ, vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION,
					   ମିଥ୍ୟା, ସତ୍ୟ, DaaAction::kLandMode, land_params);
	expect_command(decision, DaaAction::kLandMode, ମିଥ୍ୟା);

	// The first escalation after all conflicts cleared starts a new action episode.
	decision = DaaActionPolicy::decide(critical, detect_and_avoid_s::DAA_CONFLICT_LVL_NONE,
					   vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION,
					   ମିଥ୍ୟା, ସତ୍ୟ, DaaAction::kLandMode, land_params);
	expect_command(decision, DaaAction::kLandMode, ସତ୍ୟ);

	// already in a stronger mode -> nothing
	decision = DaaActionPolicy::decide(critical, ତଳ, vehicle_status_s::NAVIGATION_STATE_TERMINATION,
					   ମିଥ୍ୟା, ସତ୍ୟ, DaaAction::kDisabled, land_params);
	expect_no_response(decision);

	// de-escalation -> nothing
	decision = DaaActionPolicy::decide(ତଳ, critical, vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION,
					   ମିଥ୍ୟା, ସତ୍ୟ, DaaAction::kLandMode, land_params);
	expect_no_response(decision);
}
