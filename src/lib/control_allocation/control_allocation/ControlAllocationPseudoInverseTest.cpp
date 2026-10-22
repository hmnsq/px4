/****************************************************************************
 *
 *   Copyright (C) 2019 PX4 Development Team. All rights reserved.
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
 * @file ControlAllocationPseudoInverseTest.cpp
 *
 * Tests for Control Allocation Algorithms
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <ControlAllocationPseudoInverse.hpp>

ఉపయోగించి నామాకాశ మాతృక;

TEST(Control_కేటాయింపు_Test, AllZeroCase)
{
	Control_కేటాయింపు_Pseudo_Inverse పద్ధతి;

	మాతృక::సదిశ<దశాంశం, 6> control_sp;
	మాతృక::సదిశ<దశాంశం, 6> control_allocated;
	మాతృక::సదిశ<దశాంశం, 6> control_allocated_expected;
	మాతృక::మాతృక_2<దశాంశం, 6, 16> effectiveness;
	మాతృక::సదిశ<దశాంశం, 16> actuator_sp;
	మాతృక::సదిశ<దశాంశం, 16> actuator_trim;
	మాతృక::సదిశ<దశాంశం, 16> linearization_బిందువు;
	మాతృక::సదిశ<దశాంశం, 16> actuator_sp_expected;

	పద్ధతి.సమితి_Effectiveness_మాతృక(effectiveness, actuator_trim, linearization_బిందువు, 16, అబద్ధం);
	పద్ధతి.సమితి_Control_Setpoint(control_sp);
	పద్ధతి.allocate();
	పద్ధతి.clipActuatorSetpoint();
	actuator_sp = పద్ధతి.getActuatorSetpoint();
	control_allocated_expected = పద్ధతి.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

TEST(Control_కేటాయింపు_Metric_Test, AllZeroCase)
{
	Control_కేటాయింపు_Pseudo_Inverse పద్ధతి;

	మాతృక::సదిశ<దశాంశం, 6> control_sp;
	మాతృక::సదిశ<దశాంశం, 6> control_allocated;
	మాతృక::సదిశ<దశాంశం, 6> control_allocated_expected;
	మాతృక::మాతృక_2<దశాంశం, 6, 16> effectiveness;
	మాతృక::సదిశ<దశాంశం, 16> actuator_sp;
	మాతృక::సదిశ<దశాంశం, 16> actuator_trim;
	మాతృక::సదిశ<దశాంశం, 16> linearization_బిందువు;
	మాతృక::సదిశ<దశాంశం, 16> actuator_sp_expected;

	పద్ధతి.సమితి_Metric_కేటాయింపు(నిజం);
	పద్ధతి.సమితి_Effectiveness_మాతృక(effectiveness, actuator_trim, linearization_బిందువు, 16, అబద్ధం);
	పద్ధతి.సమితి_Control_Setpoint(control_sp);
	పద్ధతి.allocate();
	actuator_sp = పద్ధతి.getActuatorSetpoint();
	control_allocated_expected = పద్ధతి.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

నామాకాశ
{

// Surveyed hexarotor; rows roll/pitch/yaw/thrust xyz, one column per rotor
constexpr దశాంశం kSurveyedHexarotor[6][6] = {
	{-5.947500f,  5.934500f,  2.964000f, -2.853500f, -2.990000f,  2.840500f},
	{ 0.071500f,  0.071500f,  5.063500f, -4.972500f,  5.063500f, -4.972500f},
	{-0.325000f,  0.325000f, -0.325000f,  0.325000f,  0.325000f, -0.325000f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{-6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f}
};

మాతృక_2<దశాంశం, 6, 16> surveyedHexarotor(uint16_t stopped_ముసుగు)
{
	మాతృక_2<దశాంశం, 6, 16> effectiveness;
	effectiveness.సమితి_Zero();

	కొరకు (పూర్ణాంకం i = 0; i < 6; i++) {
		ఒకవేళ (stopped_ముసుగు & (1u << i)) {
			కొనసాగు;
		}

		కొరకు (పూర్ణాంకం axis = 0; axis < 6; axis++) {
			effectiveness(axis, i) = kSurveyedHexarotor[axis][i];
		}
	}

	తిరిగి effectiveness;
}

నిర్మాణం Rotor {
	దశాంశం px;
	దశాంశం py;
	దశాంశం km;
};

// upward rotors as in ActuatorEffectivenessRotors
మాతృక_2<దశాంశం, 6, 16> planarRotors(స్థిరరాశి Rotor *rotors, పూర్ణాంకం లెక్క, uint16_t stopped_ముసుగు = 0)
{
	constexpr దశాంశం ct = 6.5f;
	మాతృక_2<దశాంశం, 6, 16> effectiveness;
	effectiveness.సమితి_Zero();

	కొరకు (పూర్ణాంకం i = 0; i < లెక్క; i++) {
		ఒకవేళ (stopped_ముసుగు & (1u << i)) {
			కొనసాగు;
		}

		effectiveness(0, i) = -ct * rotors[i].py;
		effectiveness(1, i) = ct * rotors[i].px;
		effectiveness(2, i) = ct * rotors[i].km;
		effectiveness(5, i) = -ct;
	}

	తిరిగి effectiveness;
}

// In-tree geometries 4001_quad_x, 6001_hexa_x, 8001_octo_x
constexpr Rotor kQuadX[] = {{1.f, 1.f, 0.05f}, {-1.f, -1.f, 0.05f}, {1.f, -1.f, -0.05f}, {-1.f, 1.f, -0.05f}};
constexpr Rotor kHexaX[] = {{0.f, 0.5f, -0.05f}, {0.f, -0.5f, 0.05f}, {0.43f, -0.25f, -0.05f},
	{-0.43f, 0.25f, 0.05f}, {0.43f, 0.25f, 0.05f}, {-0.43f, -0.25f, -0.05f}
};
constexpr Rotor kOctoX[] = {{0.46f, 0.19f, -0.05f}, {-0.46f, -0.19f, -0.05f}, {0.19f, 0.46f, 0.05f},
	{-0.46f, 0.19f, 0.05f}, {0.46f, -0.19f, 0.05f}, {-0.19f, -0.46f, 0.05f}, {0.19f, -0.46f, -0.05f},
	{-0.19f, 0.46f, -0.05f}
};

} // namespace

TEST(Control_కేటాయింపు_Pseudo_Inverse_Test, DropDependentAxesKeepsIndependentGeometries)
{
	స్థిరరాశి మాతృక_2<దశాంశం, 6, 16> independent[] = {
		planarRotors(kQuadX, 4),
		planarRotors(kHexaX, 6),
		planarRotors(kOctoX, 8),
		surveyedHexarotor(0),
		surveyedHexarotor(1u << 0),		// one motor removed: still rank 4
		planarRotors(kHexaX, 6, 1u << 2),
		planarRotors(kOctoX, 8, (1u << 0) | (1u << 1)),
	};

	కొరకు (స్థిరరాశి స్వయంచాలక &original : independent) {
		మాతృక_2<దశాంశం, 6, 16> effectiveness = original;
		EXPECT_EQ(Control_కేటాయింపు_Pseudo_Inverse::dropDependentAxes(effectiveness), 0);
		EXPECT_EQ(effectiveness, original);
	}
}

TEST(Control_కేటాయింపు_Pseudo_Inverse_Test, DropDependentAxesDropsYawCollinearWithRoll)
{
	// motor and its opposite stopped: roll and yaw collinear, yaw must go, never roll
	స్థిరరాశి మాతృక_2<దశాంశం, 6, 16> reduced[] = {
		surveyedHexarotor((1u << 0) | (1u << 1)),
		surveyedHexarotor((1u << 2) | (1u << 3)),
		surveyedHexarotor((1u << 4) | (1u << 5)),
		planarRotors(kHexaX, 6, (1u << 0) | (1u << 1)),
		planarRotors(kHexaX, 6, (1u << 2) | (1u << 3)),
	};

	కొరకు (స్థిరరాశి స్వయంచాలక &original : reduced) {
		మాతృక_2<దశాంశం, 6, 16> effectiveness = original;
		స్థిరరాశి uint8_t dropped = Control_కేటాయింపు_Pseudo_Inverse::dropDependentAxes(effectiveness);
		EXPECT_EQ(dropped, 1u << Control_కేటాయింపు::YAW);

		కొరకు (పూర్ణాంకం axis = 0; axis < 6; axis++) {
			కొరకు (పూర్ణాంకం i = 0; i < 16; i++) {
				స్థిరరాశి దశాంశం expected = (axis == Control_కేటాయింపు::YAW) ? 0.f : original(axis, i);
				EXPECT_FLOAT_EQ(effectiveness(axis, i), expected);
			}
		}
	}
}

TEST(Control_కేటాయింపు_Pseudo_Inverse_Test, DroppedAxisIsUnallocatedOthersExact)
{
	Control_కేటాయింపు_Pseudo_Inverse పద్ధతి;
	సదిశ<దశాంశం, 16> actuator_trim;
	సదిశ<దశాంశం, 16> linearization_బిందువు;

	పద్ధతి.సమితి_Effectiveness_మాతృక(surveyedHexarotor((1u << 0) | (1u << 1)), actuator_trim, linearization_బిందువు,
				      16, అబద్ధం);
	EXPECT_EQ(పద్ధతి.getDroppedAxes(), 1u << Control_కేటాయింపు::YAW);

	// yaw demand has nothing to act on
	సదిశ<దశాంశం, 6> control_sp;
	control_sp(Control_కేటాయింపు::YAW) = 1.f;
	పద్ధతి.సమితి_Control_Setpoint(control_sp);
	పద్ధతి.allocate();
	స్థిరరాశి సదిశ<దశాంశం, 16> no_actuation;
	EXPECT_EQ(పద్ధతి.getActuatorSetpoint(), no_actuation);
	EXPECT_FLOAT_EQ(పద్ధతి.getAllocatedControl()(Control_కేటాయింపు::YAW), 0.f);

	// remaining axes allocated exactly
	control_sp.సమితి_Zero();
	control_sp(Control_కేటాయింపు::ROLL) = 0.01f;
	control_sp(Control_కేటాయింపు::PITCH) = 0.024f;
	control_sp(Control_కేటాయింపు::THRUST_Z) = -0.001f;
	పద్ధతి.సమితి_Control_Setpoint(control_sp);
	పద్ధతి.allocate();

	స్థిరరాశి సదిశ<దశాంశం, 6> allocated = పద్ధతి.getAllocatedControl();
	EXPECT_NEAR(allocated(Control_కేటాయింపు::ROLL), 0.01f, 1e-4f);
	EXPECT_NEAR(allocated(Control_కేటాయింపు::PITCH), 0.024f, 1e-4f);
	EXPECT_NEAR(allocated(Control_కేటాయింపు::THRUST_Z), -0.001f, 1e-4f);
	EXPECT_FLOAT_EQ(allocated(Control_కేటాయింపు::YAW), 0.f);

	కొరకు (పూర్ణాంకం i = 0; i < 16; i++) {
		EXPECT_LT(fabsf(పద్ధతి.getActuatorSetpoint()(i)), 0.01f) << "actuator " << i;
	}
}
