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

#അന്തര്ഭാവ <gtest/gtest.h>

#അന്തര്ഭാവ <lib/adsb/DaaActionPolicy.h>
#അന്തര്ഭാവ <uORB/topics/detect_and_avoid.h>
#അന്തര്ഭാവ <uORB/topics/vehicle_status.h>

നാമാകാശ
{

// Same action parameter value for every conflict level, so the decide() tests
// behave identically in F3442 and Crosstrack builds.
daa_action_params_s params_for_all_levels(സ്ഥിരം int32_t action_param)
{
	daa_action_params_s params{};
#എങ്കിൽ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	params.lvl_താഴെയുള്ള_act = action_param;
	params.lvl_med_act = action_param;
	params.lvl_മുകളിലെ_act = action_param;
	params.lvl_crit_act = action_param;
#അല്ലെങ്കിൽ
	params.traff_avoid = action_param;
#endif // CONFIG_NAVIGATOR_ADSB_F3442
	തിരികെ params;
}

// Navigation state groups by the weakest DAA action allowed to escalate above them.
സ്ഥിരം uint8_t kStatesEnableHold[] {
	vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION,
	vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF,
	vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET,
	vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF,
	vehicle_status_s::NAVIGATION_STATE_GUIDED_COURSE,
};

സ്ഥിരം uint8_t kStatesEnableRtl[] {
	vehicle_status_s::NAVIGATION_STATE_ORBIT,
	vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER,
};

സ്ഥിരം uint8_t kStatesEnableLand[] {
	vehicle_status_s::NAVIGATION_STATE_AUTO_RTL,
};

സ്ഥിരം uint8_t kStatesEnableTermination[] {
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

സ്ഥിരം uint8_t kStatesNeverEnable[] {
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

ഘടന escalation_case_s {
	DaaAction action{DaaAction::kDisabled};
	തർക്കം hold_allowed{അസത്യം};
	തർക്കം rtl_allowed{അസത്യം};
	തർക്കം land_allowed{അസത്യം};
	തർക്കം terminate_allowed{അസത്യം};
};

ശൂന്യം check_escalation_matrix(സ്ഥിരം escalation_case_s &test_case)
{
	വേണ്ടി (സ്ഥിരം uint8_t nav_state : kStatesEnableHold) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.hold_allowed) << "action " << (പൂർണ്ണസംഖ്യ)test_case.action << " nav_state " << (പൂർണ്ണസംഖ്യ)nav_state;
	}

	വേണ്ടി (സ്ഥിരം uint8_t nav_state : kStatesEnableRtl) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.rtl_allowed) << "action " << (പൂർണ്ണസംഖ്യ)test_case.action << " nav_state " << (പൂർണ്ണസംഖ്യ)nav_state;
	}

	വേണ്ടി (സ്ഥിരം uint8_t nav_state : kStatesEnableLand) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.land_allowed) << "action " << (പൂർണ്ണസംഖ്യ)test_case.action << " nav_state " << (പൂർണ്ണസംഖ്യ)nav_state;
	}

	വേണ്ടി (സ്ഥിരം uint8_t nav_state : kStatesEnableTermination) {
		EXPECT_EQ(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state),
			  test_case.terminate_allowed) << "action " << (പൂർണ്ണസംഖ്യ)test_case.action << " nav_state " << (പൂർണ്ണസംഖ്യ)nav_state;
	}

	വേണ്ടി (സ്ഥിരം uint8_t nav_state : kStatesNeverEnable) {
		EXPECT_FALSE(DaaActionPolicy::action_escalates_above_nav_state(test_case.action, nav_state))
				<< "action " << (പൂർണ്ണസംഖ്യ)test_case.action << " nav_state " << (പൂർണ്ണസംഖ്യ)nav_state;
	}
}

ശൂന്യം expect_no_response(സ്ഥിരം daa_action_decision_s &decision)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_FALSE(decision.warn_on_ground);
}

ശൂന്യം expect_ground_warning(സ്ഥിരം daa_action_decision_s &decision, സ്ഥിരം NotifyLandedActCause expected_cause)
{
	EXPECT_EQ(decision.action_command, DaaAction::kDisabled);
	EXPECT_FALSE(decision.announce_action);
	EXPECT_TRUE(decision.warn_on_ground);
	EXPECT_EQ(decision.ground_warning_cause, expected_cause);
}

ശൂന്യം expect_command(സ്ഥിരം daa_action_decision_s &decision, സ്ഥിരം DaaAction expected_action,
		    സ്ഥിരം തർക്കം expected_announcement)
{
	EXPECT_EQ(decision.action_command, expected_action);
	EXPECT_EQ(decision.announce_action, expected_announcement);
	EXPECT_FALSE(decision.warn_on_ground);
}

} // namespace

// Param numbering and the internal action ladder map both ways; out-of-range maps to Disabled.
TEST(DaaActionPolicyTest, ActionParamMappingUsesParameterNumbering)
{
	ഘടന action_param_case_s {
		int32_t param;
		DaaAction action;
	};

	സ്ഥിരം action_param_case_s cases[] {
		{0, DaaAction::kDisabled},
		{1, DaaAction::kWarnOnly},
		{2, DaaAction::kReturnMode},
		{3, DaaAction::kLandMode},
		{4, DaaAction::kPositionHoldMode},
		{5, DaaAction::kTerminate},
	};

	വേണ്ടി (സ്ഥിരം action_param_case_s &test_case : cases) {
		EXPECT_EQ(DaaActionPolicy::action_param_to_daa_action(test_case.param), test_case.action)
				<< "param " << test_case.param;
		EXPECT_EQ(DaaActionPolicy::daa_action_to_action_param(test_case.action), test_case.param)
				<< "action " << static_cast<പൂർണ്ണസംഖ്യ>(test_case.action);
	}

	// out-of-range -> Disabled
	EXPECT_EQ(DaaActionPolicy::action_param_to_daa_action(-1), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_param_to_daa_action(6), DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::daa_action_to_action_param(DaaAction::k_Max_Action_മൂല്യം), 0);
}

// An action escalates only when stronger than the current navigator state (protects manual/offboard).
TEST(DaaActionPolicyTest, ActionEscalatesOnlyAboveNavState)
{
	check_escalation_matrix({DaaAction::kDisabled, അസത്യം, അസത്യം, അസത്യം, അസത്യം});
	check_escalation_matrix({DaaAction::kWarnOnly, അസത്യം, അസത്യം, അസത്യം, അസത്യം});
	check_escalation_matrix({DaaAction::kPositionHoldMode, സത്യം, അസത്യം, അസത്യം, അസത്യം});
	check_escalation_matrix({DaaAction::kReturnMode, സത്യം, സത്യം, അസത്യം, അസത്യം});
	check_escalation_matrix({DaaAction::kLandMode, സത്യം, സത്യം, സത്യം, അസത്യം});
	check_escalation_matrix({DaaAction::kTerminate, സത്യം, സത്യം, സത്യം, സത്യം});
}

// NONE and out-of-range levels never request an action, even with actions configured everywhere.
TEST(DaaActionPolicyTest, NoActionForInvalidLevels)
{
	സ്ഥിരം daa_action_params_s params = params_for_all_levels(5); // Terminate everywhere

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_NONE, params),
		  DaaAction::kDisabled);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL + 1, params),
		  DaaAction::kDisabled);
}

#എങ്കിൽ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
TEST(DaaActionPolicyTest, F3442FallbackUsesGuaranteedBreachedZones)
{
	daa_action_params_s params{};
	params.lvl_താഴെയുള്ള_act = 1;		// Warn only
	params.lvl_med_act = 4;		// Hold
	params.lvl_മുകളിലെ_act = 0;	// Disabled
	params.lvl_crit_act = 5;	// Terminate

	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_താഴെയുള്ള, params),
		  DaaAction::kWarnOnly);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM, params),
		  DaaAction::kPositionHoldMode);
	EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(detect_and_avoid_s::DAA_CONFLICT_LVL_മുകളിലെ, params),
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
#അല്ലെങ്കിൽ
// Crosstrack maps every level through the single NAV_TRAFF_AVOID action.
TEST(DaaActionPolicyTest, CrosstrackSingleActionMapping)
{
	സ്ഥിരം daa_action_params_s params = params_for_all_levels(3); // Land

	വേണ്ടി (uint8_t level = detect_and_avoid_s::DAA_CONFLICT_LVL_താഴെയുള്ള;
	     level <= detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL; ++level) {
		EXPECT_EQ(DaaActionPolicy::action_from_conflict_level(level, params), DaaAction::kLandMode);
	}
}
#endif // CONFIG_NAVIGATOR_ADSB_F3442

// Unchanged level: no command or warning.
TEST(DaaActionPolicyTest, DecideNoResponseWithoutLevelChange)
{
	സ്ഥിരം daa_action_decision_s decision = DaaActionPolicy::decide(
			detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL,
			vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION, അസത്യം, സത്യം,
			DaaAction::kDisabled, params_for_all_levels(5));

	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, DecideWarnsOnGroundInsteadOfActing)
{
	സ്ഥിരം uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	സ്ഥിരം uint8_t none = detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	സ്ഥിരം uint8_t nav_state = vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION;

	// armed -> takeoff warning
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, none, nav_state, സത്യം, സത്യം,
					 DaaAction::kDisabled, params_for_all_levels(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndArmed);

	// disarmed -> arming warning
	decision = DaaActionPolicy::decide(critical, none, nav_state, സത്യം, അസത്യം,
					   DaaAction::kDisabled, params_for_all_levels(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// Repeated requests allow the caller to emit rate-limited ground warnings.
	decision = DaaActionPolicy::decide(critical, critical, nav_state, സത്യം, അസത്യം,
					   DaaAction::kDisabled, params_for_all_levels(5));
	expect_ground_warning(decision, NotifyLandedActCause::kConflictAndDisarmed);

	// warn-only config -> nothing
	decision = DaaActionPolicy::decide(critical, none, nav_state, സത്യം, സത്യം,
					   DaaAction::kDisabled, params_for_all_levels(1));
	expect_no_response(decision);
}

TEST(DaaActionPolicyTest, DecideRequestsCommandOnAirborneEscalationOnly)
{
	സ്ഥിരം uint8_t critical = detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	സ്ഥിരം uint8_t താഴെയുള്ള = detect_and_avoid_s::DAA_CONFLICT_LVL_താഴെയുള്ള;
	സ്ഥിരം daa_action_params_s land_params = params_for_all_levels(3); // Land

	// escalation -> Land command, announced
	daa_action_decision_s decision = DaaActionPolicy::decide(critical, താഴെയുള്ള,
					 vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION, അസത്യം, സത്യം,
					 DaaAction::kDisabled, land_params);
	expect_command(decision, DaaAction::kLandMode, സത്യം);

	// A later level escalation that maps to the same action is not re-announced.
	decision = DaaActionPolicy::decide(critical, താഴെയുള്ള, vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION,
					   അസത്യം, സത്യം, DaaAction::kLandMode, land_params);
	expect_command(decision, DaaAction::kLandMode, അസത്യം);

	// The first escalation after all conflicts cleared starts a new action episode.
	decision = DaaActionPolicy::decide(critical, detect_and_avoid_s::DAA_CONFLICT_LVL_NONE,
					   vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION,
					   അസത്യം, സത്യം, DaaAction::kLandMode, land_params);
	expect_command(decision, DaaAction::kLandMode, സത്യം);

	// already in a stronger mode -> nothing
	decision = DaaActionPolicy::decide(critical, താഴെയുള്ള, vehicle_status_s::NAVIGATION_STATE_TERMINATION,
					   അസത്യം, സത്യം, DaaAction::kDisabled, land_params);
	expect_no_response(decision);

	// de-escalation -> nothing
	decision = DaaActionPolicy::decide(താഴെയുള്ള, critical, vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION,
					   അസത്യം, സത്യം, DaaAction::kLandMode, land_params);
	expect_no_response(decision);
}
