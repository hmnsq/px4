/****************************************************************************
 *
 *   Copyright (C) 2020-2021 PX4 Development Team. All rights reserved.
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
 * Test code for the SystemIdentification class
 * Run this test only using make tests TESTFILTER=system_identification
 */

#شامل <gtest/gtest.h>
#شامل <matrix/matrix/math.hpp>

#شامل "system_identification.hpp"
#شامل "test_data.h"

استعمال نامخلا میٹرکس;

جماعت SystemIdentificationTest : عوامی ::testing::Test
{
عوامی:
	SystemIdentificationTest() {};
	اعشاری apply(اعشاری sample);
	خالی سیٹ_Coefficients(اعشاری a1, اعشاری a2, اعشاری b0, اعشاری b1, اعشاری b2)
	{
		_a1 = a1;
		_a2 = a2;
		_b0 = b0;
		_b1 = b1;
		_b2 = b2;
	}

نجی:
	اعشاری _a1{};
	اعشاری _a2{};
	اعشاری _b0{};
	اعشاری _b1{};
	اعشاری _b2{};
	اعشاری _delay_element_1{};
	اعشاری _delay_element_2{};
};

اعشاری SystemIdentificationTest::apply(اعشاری sample)
{
	// Direct Form II implementation
	ثابت اعشاری delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};
	ثابت اعشاری output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

	_delay_element_2 = _delay_element_1;
	_delay_element_1 = delay_element_0;

	واپس output;
}

TEST_F(SystemIdentificationTest, basicTest)
{
	constexpr اعشاری fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.سیٹ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.سیٹ_Lpf_Cutoff_Frequency(fs, 30.f);
	_sys_id.سیٹ_Forgetting_ضربی_عدد(80.f, 1.f / fs);

	برائے (صحیح i = 0; i < 5; i++) {
		// Fill the buffers with zeros
		_sys_id.update(0.f, 0.f);
	}

	برائے (صحیح i = 0; i < 10; i += 2) {
		_sys_id.update(اعشاری(i), اعشاری(i + 1));
	}

	ثابت سمتیہ<اعشاری, 5> coefficients = _sys_id.getCoefficients();
	اعشاری ڈیٹا_check[] = {-3.96f, 1.30f, -2.28f, -0.33f, 0.369f};
	ثابت سمتیہ<اعشاری, 5> coefficients_check(ڈیٹا_check);
	اعشاری eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).مطلق_قدر().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, resetTest)
{
	constexpr اعشاری fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.سیٹ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.سیٹ_Lpf_Cutoff_Frequency(fs, 30.f);
	_sys_id.سیٹ_Forgetting_ضربی_عدد(80.f, 1.f / fs);

	برائے (صحیح i = 0; i < 10; i += 2) {
		_sys_id.update(اعشاری(i), اعشاری(i + 1));
	}

	ثابت سمتیہ<اعشاری, 5> coefficients = _sys_id.getCoefficients();

	// WHEN: resetting
	_sys_id.reset();

	// THEN: the variances and coefficients should be properly reset
	EXPECT_TRUE(_sys_id.getCoefficients().مطلق_قدر().max() < 1e-8f);
	EXPECT_TRUE(_sys_id.getVariances().min() > 9e3f);

	// AND WHEN: running the same sequence of inputs-outputs

	برائے (صحیح i = 0; i < 10; i += 2) {
		_sys_id.update(اعشاری(i), اعشاری(i + 1));
	}

	// THEN: the result should be exactly the same
	EXPECT_TRUE((coefficients - _sys_id.getCoefficients()).مطلق_قدر().max() < 1e-8f);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, simulatedModelTest)
{
	constexpr اعشاری fs = 200.f;
	ثابت اعشاری gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.سیٹ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.سیٹ_Lpf_Cutoff_Frequency(fs, gyro_lpf_cutoff);
	_sys_id.سیٹ_Forgetting_ضربی_عدد(60.f, 1.f / fs);

	// Simulated model with integrator
	ثابت اعشاری a1 = -1.77f;
	ثابت اعشاری a2 = 0.77f;
	ثابت اعشاری b0 = 0.3812f;
	ثابت اعشاری b1 = -0.25f;
	ثابت اعشاری b2 = 0.2f;
	سیٹ_Coefficients(a1, a2, b0, b1, b2);
	ثابت اعشاری u_bias = -0.1f; // constant control offset
	ثابت اعشاری y_bias = 0.2f; // measurement bias

	ثابت اعشاری dt = 1.f / fs;
	ثابت اعشاری duration = 2.f;
	اعشاری u = 0.f;
	اعشاری y = 0.f;

	برائے (صحیح i = 0; i < static_cast<صحیح>(duration / dt); i++) {

		// Generate square input signal
		اگر (_sys_id.areFiltersInitialized()
		    && (i % 30 == 0)) {
			اگر (u > 0.f) {
				u = -1.f;

			} ورنہ {
				u = 1.f;
			}
		}

		_sys_id.update(u + u_bias, y + y_bias); // apply new input and previous output

		y = apply(u);
#اگر 0
		ثابت اعشاری t = i * dt;
		لکھو("%.6f, %.6f, %.6f\n", (دوہرا)t, (دوہرا)u, (دوہرا)y);
#اگر_ختم
	}

	ثابت سمتیہ<اعشاری, 5> coefficients = _sys_id.getCoefficients();
	اعشاری ڈیٹا_check[] = {a1, a2, b0, b1, b2};
	ثابت سمتیہ<اعشاری, 5> coefficients_check(ڈیٹا_check);
	اعشاری eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).مطلق_قدر().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, real_ڈیٹا_Test)
{
	ساکن constexpr صحیح n_samples_dt_avg = 100;
	ساکن constexpr صحیح n_samples_test_ڈیٹا = حجم(u_ڈیٹا) / حجم(u_ڈیٹا[0]);
	اعشاری dt_مجموع = 0.f;

	برائے (صحیح i = 1; i <= n_samples_dt_avg; i++) {
		dt_مجموع += t_ڈیٹا[i] - t_ڈیٹا[i - 1];
	}

	ثابت اعشاری dt = dt_مجموع / n_samples_dt_avg;
	ثابت اعشاری fs = 1.f / dt;

	لکھو("dt = %.6f fs = %.3f\n", (دوہرا)dt, (دوہرا)(1.f / dt));

	ثابت اعشاری gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.سیٹ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.سیٹ_Lpf_Cutoff_Frequency(fs, gyro_lpf_cutoff);
	_sys_id.سیٹ_Forgetting_ضربی_عدد(60.f, dt);

	اعشاری u = 0.f;
	اعشاری y_پچھلا_2 = 0.f;

	برائے (صحیح i = 0; i < n_samples_test_ڈیٹا; i++) {
		u = u_ڈیٹا[i];
		_sys_id.update(u, y_پچھلا_2); // apply new input and previous output

		y_پچھلا_2 = y_ڈیٹا[i];

#اگر 0
		لکھو("%.6f, %.6f, %.6f\n", (دوہرا)t_ڈیٹا[i], (دوہرا)u, (دوہرا)y);
#اگر_ختم
	}

	ثابت سمتیہ<اعشاری, 5> coefficients = _sys_id.getCoefficients();
	اعشاری ڈیٹا_check[] = {-1.916f, 0.916, -0.017f, -0.001f, 0.036f};
	ثابت سمتیہ<اعشاری, 5> coefficients_check(ڈیٹا_check);
	اعشاری eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).مطلق_قدر().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}
