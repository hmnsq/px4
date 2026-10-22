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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ "PerformanceModel.hpp"

// to run: make tests TESTFILTER=PerformanceModel

తరగతి PerformanceModelTest : బహిరంగ ::testing::Test
{
బహిరంగ:
	శూన్య సమితి_పైకి() override
	{
		param_control_autosave(అబద్ధం);
		param_reset_all();
	}

	శూన్య సమితి_బరువు_Params(దశాంశం base, దశాంశం gross, దశాంశం fuel)
	{
		param_సమితి(param_handle(px4::params::బరువు_పునాది), &base);
		param_సమితి(param_handle(px4::params::బరువు_GROSS), &gross);
		param_సమితి(param_handle(px4::params::బరువు_FUEL), &fuel);
		_model.updateParameters();
	}

	PerformanceModel _model;
};

TEST_F(PerformanceModelTest, బరువు_నిష్పత్తి_Defaults_గమ్యం_One)
{
	// GIVEN: default parameters (weight compensation disabled)
	// THEN: the weight ratio is 1
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 1.0f);
}

TEST_F(PerformanceModelTest, బరువు_నిష్పత్తి_Without_Fuel_Compensation)
{
	// GIVEN: base and gross weight set, fuel compensation disabled
	సమితి_బరువు_Params(10.0f, 12.0f, -1.0f);

	// THEN: the weight ratio is gross/base, independent of any fuel fraction
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 1.2f);

	_model.సమితి_Fuel_Fraction_మిగిలిన(0.5f);
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 1.2f);
}

TEST_F(PerformanceModelTest, బరువు_నిష్పత్తి_Scales_With_Fuel_Fraction)
{
	// GIVEN: base and gross weight set, 4kg full fuel load
	సమితి_బరువు_Params(10.0f, 12.0f, 4.0f);

	// THEN: with unknown fuel fraction the full-tank gross weight is assumed
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 1.2f);

	// AND: the weight ratio scales linearly with the burned fuel mass
	_model.సమితి_Fuel_Fraction_మిగిలిన(1.0f);
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 1.2f);

	_model.సమితి_Fuel_Fraction_మిగిలిన(0.5f);
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 1.0f);

	_model.సమితి_Fuel_Fraction_మిగిలిన(0.0f);
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 0.8f);
}

TEST_F(PerformanceModelTest, fuel_Fraction_Kept_On_Non_Finite_ఆదానం)
{
	// GIVEN: base and gross weight set, 4kg full fuel load, and a known fuel fraction
	సమితి_బరువు_Params(10.0f, 12.0f, 4.0f);
	_model.సమితి_Fuel_Fraction_మిగిలిన(0.5f);
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 1.0f);

	// WHEN: the fuel fraction input becomes non-finite (e.g. sensor failure)
	_model.సమితి_Fuel_Fraction_మిగిలిన(NAN);

	// THEN: the last known fuel fraction is kept
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 1.0f);
}

TEST_F(PerformanceModelTest, fuelFractionIsConstrained)
{
	// GIVEN: base and gross weight set, 4kg full fuel load
	సమితి_బరువు_Params(10.0f, 12.0f, 4.0f);

	// THEN: out-of-range fuel fractions are constrained to [0, 1]
	_model.సమితి_Fuel_Fraction_మిగిలిన(-0.5f);
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 0.8f);

	_model.సమితి_Fuel_Fraction_మిగిలిన(1.5f);
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 1.2f);
}

TEST_F(PerformanceModelTest, బరువు_నిష్పత్తి_Is_Constrained)
{
	// GIVEN: a fuel weight larger than the gross weight (misconfiguration)
	సమితి_బరువు_Params(10.0f, 12.0f, 12.0f);

	// THEN: the weight ratio is constrained to its lower bound
	_model.సమితి_Fuel_Fraction_మిగిలిన(0.0f);
	EXPECT_FLOAT_EQ(_model.get_బరువు_నిష్పత్తి(), 0.5f);
}

TEST_F(PerformanceModelTest, fuel_Fraction_From_Tank_స్థితి_Fallback_Chain)
{
	// GIVEN: a fuel tank status with no measurements
	fuel_tank_స్థితి_s స్థితి_2{};
	స్థితి_2.గరిష్ఠ_fuel_సామర్థ్యం = NAN;
	స్థితి_2.మిగిలిన_fuel = NAN;
	స్థితి_2.consumed_fuel = NAN;
	స్థితి_2.percent_మిగిలిన = UINT8_MAX;

	// THEN: no fuel fraction is available
	EXPECT_TRUE(std::isnan(PerformanceModel::get_Fuel_Fraction_మిగిలిన(స్థితి_2)));

	// AND: the consumed fuel is the last fallback, assuming a full tank at boot
	స్థితి_2.గరిష్ఠ_fuel_సామర్థ్యం = 2000.f;
	స్థితి_2.consumed_fuel = 500.f;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_మిగిలిన(స్థితి_2), 0.75f);

	// AND: the measured remaining fuel takes precedence over the consumed fuel
	స్థితి_2.మిగిలిన_fuel = 1000.f;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_మిగిలిన(స్థితి_2), 0.5f);

	// AND: a valid remaining percentage takes precedence over everything else
	స్థితి_2.percent_మిగిలిన = 40;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_మిగిలిన(స్థితి_2), 0.4f);
}

TEST_F(PerformanceModelTest, fuel_Fraction_From_Tank_స్థితి_Is_Constrained)
{
	// GIVEN: a remaining fuel measurement exceeding the maximum capacity
	fuel_tank_స్థితి_s స్థితి_2{};
	స్థితి_2.గరిష్ఠ_fuel_సామర్థ్యం = 2000.f;
	స్థితి_2.మిగిలిన_fuel = 3000.f;
	స్థితి_2.consumed_fuel = NAN;
	స్థితి_2.percent_మిగిలిన = UINT8_MAX;

	// THEN: the fuel fraction is constrained to 1
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_మిగిలిన(స్థితి_2), 1.0f);

	// AND: more consumed fuel than the maximum capacity results in a fraction of 0
	స్థితి_2.మిగిలిన_fuel = NAN;
	స్థితి_2.consumed_fuel = 3000.f;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_మిగిలిన(స్థితి_2), 0.0f);

	// AND: a remaining fuel measurement without a valid maximum capacity is unusable
	స్థితి_2.గరిష్ఠ_fuel_సామర్థ్యం = 0.f;
	స్థితి_2.మిగిలిన_fuel = 1000.f;
	EXPECT_TRUE(std::isnan(PerformanceModel::get_Fuel_Fraction_మిగిలిన(స్థితి_2)));
}
