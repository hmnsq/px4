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

#ضمن <gtest/gtest.h>
#ضمن "PerformanceModel.hpp"

// to run: make tests TESTFILTER=PerformanceModel

صنف PerformanceModelTest : عام ::testing::Test
{
عام:
	فراغ مجموعه_به_بالا() override
	{
		param_control_autosave(خطأ);
		param_reset_all();
	}

	فراغ مجموعه_وزن_Params(عائم base, عائم gross, عائم fuel)
	{
		param_مجموعه(param_handle(px4::params::وزن_پایه), &base);
		param_مجموعه(param_handle(px4::params::وزن_GROSS), &gross);
		param_مجموعه(param_handle(px4::params::وزن_FUEL), &fuel);
		_model.updateParameters();
	}

	PerformanceModel _model;
};

TEST_F(PerformanceModelTest, وزن_نسبت_Defaults_مقصد_One)
{
	// GIVEN: default parameters (weight compensation disabled)
	// THEN: the weight ratio is 1
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 1.0f);
}

TEST_F(PerformanceModelTest, وزن_نسبت_Without_Fuel_Compensation)
{
	// GIVEN: base and gross weight set, fuel compensation disabled
	مجموعه_وزن_Params(10.0f, 12.0f, -1.0f);

	// THEN: the weight ratio is gross/base, independent of any fuel fraction
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 1.2f);

	_model.مجموعه_Fuel_Fraction_مانده(0.5f);
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 1.2f);
}

TEST_F(PerformanceModelTest, وزن_نسبت_Scales_With_Fuel_Fraction)
{
	// GIVEN: base and gross weight set, 4kg full fuel load
	مجموعه_وزن_Params(10.0f, 12.0f, 4.0f);

	// THEN: with unknown fuel fraction the full-tank gross weight is assumed
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 1.2f);

	// AND: the weight ratio scales linearly with the burned fuel mass
	_model.مجموعه_Fuel_Fraction_مانده(1.0f);
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 1.2f);

	_model.مجموعه_Fuel_Fraction_مانده(0.5f);
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 1.0f);

	_model.مجموعه_Fuel_Fraction_مانده(0.0f);
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 0.8f);
}

TEST_F(PerformanceModelTest, fuel_Fraction_Kept_On_Non_Finite_درون_داد)
{
	// GIVEN: base and gross weight set, 4kg full fuel load, and a known fuel fraction
	مجموعه_وزن_Params(10.0f, 12.0f, 4.0f);
	_model.مجموعه_Fuel_Fraction_مانده(0.5f);
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 1.0f);

	// WHEN: the fuel fraction input becomes non-finite (e.g. sensor failure)
	_model.مجموعه_Fuel_Fraction_مانده(NAN);

	// THEN: the last known fuel fraction is kept
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 1.0f);
}

TEST_F(PerformanceModelTest, fuelFractionIsConstrained)
{
	// GIVEN: base and gross weight set, 4kg full fuel load
	مجموعه_وزن_Params(10.0f, 12.0f, 4.0f);

	// THEN: out-of-range fuel fractions are constrained to [0, 1]
	_model.مجموعه_Fuel_Fraction_مانده(-0.5f);
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 0.8f);

	_model.مجموعه_Fuel_Fraction_مانده(1.5f);
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 1.2f);
}

TEST_F(PerformanceModelTest, وزن_نسبت_Is_Constrained)
{
	// GIVEN: a fuel weight larger than the gross weight (misconfiguration)
	مجموعه_وزن_Params(10.0f, 12.0f, 12.0f);

	// THEN: the weight ratio is constrained to its lower bound
	_model.مجموعه_Fuel_Fraction_مانده(0.0f);
	EXPECT_FLOAT_EQ(_model.get_وزن_نسبت(), 0.5f);
}

TEST_F(PerformanceModelTest, fuel_Fraction_From_Tank_وضعیت_Fallback_Chain)
{
	// GIVEN: a fuel tank status with no measurements
	fuel_tank_وضعیت_s وضعیت{};
	وضعیت.بیشینه_fuel_ظرفیت = NAN;
	وضعیت.مانده_fuel = NAN;
	وضعیت.consumed_fuel = NAN;
	وضعیت.percent_مانده = UINT8_MAX;

	// THEN: no fuel fraction is available
	EXPECT_TRUE(std::isnan(PerformanceModel::get_Fuel_Fraction_مانده(وضعیت)));

	// AND: the consumed fuel is the last fallback, assuming a full tank at boot
	وضعیت.بیشینه_fuel_ظرفیت = 2000.f;
	وضعیت.consumed_fuel = 500.f;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_مانده(وضعیت), 0.75f);

	// AND: the measured remaining fuel takes precedence over the consumed fuel
	وضعیت.مانده_fuel = 1000.f;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_مانده(وضعیت), 0.5f);

	// AND: a valid remaining percentage takes precedence over everything else
	وضعیت.percent_مانده = 40;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_مانده(وضعیت), 0.4f);
}

TEST_F(PerformanceModelTest, fuel_Fraction_From_Tank_وضعیت_Is_Constrained)
{
	// GIVEN: a remaining fuel measurement exceeding the maximum capacity
	fuel_tank_وضعیت_s وضعیت{};
	وضعیت.بیشینه_fuel_ظرفیت = 2000.f;
	وضعیت.مانده_fuel = 3000.f;
	وضعیت.consumed_fuel = NAN;
	وضعیت.percent_مانده = UINT8_MAX;

	// THEN: the fuel fraction is constrained to 1
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_مانده(وضعیت), 1.0f);

	// AND: more consumed fuel than the maximum capacity results in a fraction of 0
	وضعیت.مانده_fuel = NAN;
	وضعیت.consumed_fuel = 3000.f;
	EXPECT_FLOAT_EQ(PerformanceModel::get_Fuel_Fraction_مانده(وضعیت), 0.0f);

	// AND: a remaining fuel measurement without a valid maximum capacity is unusable
	وضعیت.بیشینه_fuel_ظرفیت = 0.f;
	وضعیت.مانده_fuel = 1000.f;
	EXPECT_TRUE(std::isnan(PerformanceModel::get_Fuel_Fraction_مانده(وضعیت)));
}
