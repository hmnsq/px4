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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "PerformanceModel.hpp"

// to run: make tests TESTFILTER=PerformanceModel

ਵਰਗ PerformanceModelTest : ਜਨਤਕ ::testing::Test
{
ਜਨਤਕ:
	ਖਾਲੀ ਸੈੱਟ_ਉੱਪਰ() override
	{
		param_control_autosave(ਝੂਠ);
		param_reset_all();
	}

	ਖਾਲੀ ਸੈੱਟ_ਭਾਰ_Params(ਦਸ਼ਮਲਵ base, ਦਸ਼ਮਲਵ gross, ਦਸ਼ਮਲਵ fuel)
	{
		param_ਸੈੱਟ(param_handle(px4::params::ਭਾਰ_ਅਧਾਰ), &base);
		param_ਸੈੱਟ(param_handle(px4::params::ਭਾਰ_GROSS), &gross);
		param_ਸੈੱਟ(param_handle(px4::params::ਭਾਰ_FUEL), &fuel);
		_model.updateParameters();
	}

	PerformanceModel _model;
};

TEST_F(PerformanceModelTest, ਭਾਰ_ਅਨੁਪਾਤ_Defaults_ਮੰਜ਼ਿਲ_One)
{
	// GIVEN: default parameters (weight compensation disabled)
	// THEN: the weight ratio is 1
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 1.0f);
}

TEST_F(PerformanceModelTest, ਭਾਰ_ਅਨੁਪਾਤ_Without_Fuel_Compensation)
{
	// GIVEN: base and gross weight set, fuel compensation disabled
	ਸੈੱਟ_ਭਾਰ_Params(10.0f, 12.0f, -1.0f);

	// THEN: the weight ratio is gross/base, independent of any fuel fraction
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 1.2f);

	_model.ਸੈੱਟ_Fuel_Fraction_ਬਚਿਆ(0.5f);
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 1.2f);
}

TEST_F(PerformanceModelTest, ਭਾਰ_ਅਨੁਪਾਤ_Scales_With_Fuel_Fraction)
{
	// GIVEN: base and gross weight set, 4kg full fuel load
	ਸੈੱਟ_ਭਾਰ_Params(10.0f, 12.0f, 4.0f);

	// THEN: with unknown fuel fraction the full-tank gross weight is assumed
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 1.2f);

	// AND: the weight ratio scales linearly with the burned fuel mass
	_model.ਸੈੱਟ_Fuel_Fraction_ਬਚਿਆ(1.0f);
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 1.2f);

	_model.ਸੈੱਟ_Fuel_Fraction_ਬਚਿਆ(0.5f);
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 1.0f);

	_model.ਸੈੱਟ_Fuel_Fraction_ਬਚਿਆ(0.0f);
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 0.8f);
}

TEST_F(PerformanceModelTest, fuel_Fraction_Kept_On_Non_Finite_ਨਿਵੇਸ਼)
{
	// GIVEN: base and gross weight set, 4kg full fuel load, and a known fuel fraction
	ਸੈੱਟ_ਭਾਰ_Params(10.0f, 12.0f, 4.0f);
	_model.ਸੈੱਟ_Fuel_Fraction_ਬਚਿਆ(0.5f);
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 1.0f);

	// WHEN: the fuel fraction input becomes non-finite (e.g. sensor failure)
	_model.ਸੈੱਟ_Fuel_Fraction_ਬਚਿਆ(NAN);

	// THEN: the last known fuel fraction is kept
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 1.0f);
}

TEST_F(PerformanceModelTest, fuelFractionIsConstrained)
{
	// GIVEN: base and gross weight set, 4kg full fuel load
	ਸੈੱਟ_ਭਾਰ_Params(10.0f, 12.0f, 4.0f);

	// THEN: out-of-range fuel fractions are constrained to [0, 1]
	_model.ਸੈੱਟ_Fuel_Fraction_ਬਚਿਆ(-0.5f);
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 0.8f);

	_model.ਸੈੱਟ_Fuel_Fraction_ਬਚਿਆ(1.5f);
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 1.2f);
}

TEST_F(PerformanceModelTest, ਭਾਰ_ਅਨੁਪਾਤ_Is_Constrained)
{
	// GIVEN: a fuel weight larger than the gross weight (misconfiguration)
	ਸੈੱਟ_ਭਾਰ_Params(10.0f, 12.0f, 12.0f);

	// THEN: the weight ratio is constrained to its lower bound
	_model.ਸੈੱਟ_Fuel_Fraction_ਬਚਿਆ(0.0f);
	EXPECT_FLOAT_EQ(_model.get_ਭਾਰ_ਅਨੁਪਾਤ(), 0.5f);
}

TEST_F(PerformanceModelTest, fuel_Fraction_From_Tank_ਸਥਿਤੀ_Fallback_Chain)
{
	// GIVEN: a fuel tank status with no measurements
	fuel_tank_ਸਥਿਤੀ_s ਸਥਿਤੀ{};
	ਸਥਿਤੀ.ਵੱਧ_ਤੋਂ_ਵੱਧ_fuel_ਸਮਰੱਥਾ = NAN;
	ਸਥਿਤੀ.ਬਚਿਆ_fuel = NAN;
	ਸਥਿਤੀ.consumed_fuel = NAN;
	ਸਥਿਤੀ.percent_ਬਚਿਆ = UINT8_MAX;

	// THEN: no fuel fraction is available
	EXPECT_TRUE(std::isnan(PerformanceModel::get_Fuel_Fraction_ਬਚਿਆ(ਸਥਿਤੀ)));

	// AND: the consumed fuel is the last fallback, assuming a full tank at boot
	ਸਥਿਤੀ.ਵੱਧ_ਤੋਂ_ਵੱਧ_fuel_ਸਮਰੱਥਾ = 2000.f;
	ਸਥਿਤੀ.consumed_fuel = 500.f;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_ਬਚਿਆ(ਸਥਿਤੀ), 0.75f);

	// AND: the measured remaining fuel takes precedence over the consumed fuel
	ਸਥਿਤੀ.ਬਚਿਆ_fuel = 1000.f;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_ਬਚਿਆ(ਸਥਿਤੀ), 0.5f);

	// AND: a valid remaining percentage takes precedence over everything else
	ਸਥਿਤੀ.percent_ਬਚਿਆ = 40;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_ਬਚਿਆ(ਸਥਿਤੀ), 0.4f);
}

TEST_F(PerformanceModelTest, fuel_Fraction_From_Tank_ਸਥਿਤੀ_Is_Constrained)
{
	// GIVEN: a remaining fuel measurement exceeding the maximum capacity
	fuel_tank_ਸਥਿਤੀ_s ਸਥਿਤੀ{};
	ਸਥਿਤੀ.ਵੱਧ_ਤੋਂ_ਵੱਧ_fuel_ਸਮਰੱਥਾ = 2000.f;
	ਸਥਿਤੀ.ਬਚਿਆ_fuel = 3000.f;
	ਸਥਿਤੀ.consumed_fuel = NAN;
	ਸਥਿਤੀ.percent_ਬਚਿਆ = UINT8_MAX;

	// THEN: the fuel fraction is constrained to 1
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_ਬਚਿਆ(ਸਥਿਤੀ), 1.0f);

	// AND: more consumed fuel than the maximum capacity results in a fraction of 0
	ਸਥਿਤੀ.ਬਚਿਆ_fuel = NAN;
	ਸਥਿਤੀ.consumed_fuel = 3000.f;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_ਬਚਿਆ(ਸਥਿਤੀ), 0.0f);

	// AND: a remaining fuel measurement without a valid maximum capacity is unusable
	ਸਥਿਤੀ.ਵੱਧ_ਤੋਂ_ਵੱਧ_fuel_ਸਮਰੱਥਾ = 0.f;
	ਸਥਿਤੀ.ਬਚਿਆ_fuel = 1000.f;
	EXPECT_TRUE(std::isnan(PerformanceModel::get_Fuel_Fraction_ਬਚਿਆ(ਸਥਿਤੀ)));
}
