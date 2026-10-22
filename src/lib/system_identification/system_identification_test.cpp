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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/matrix/math.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ "system_identification.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "test_data.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;

ਵਰਗ SystemIdentificationTest : ਜਨਤਕ ::testing::Test
{
ਜਨਤਕ:
	SystemIdentificationTest() {};
	ਦਸ਼ਮਲਵ apply(ਦਸ਼ਮਲਵ sample);
	ਖਾਲੀ ਸੈੱਟ_Coefficients(ਦਸ਼ਮਲਵ a1, ਦਸ਼ਮਲਵ a2, ਦਸ਼ਮਲਵ b0, ਦਸ਼ਮਲਵ b1, ਦਸ਼ਮਲਵ b2)
	{
		_a1 = a1;
		_a2 = a2;
		_b0 = b0;
		_b1 = b1;
		_b2 = b2;
	}

ਨਿਜੀ:
	ਦਸ਼ਮਲਵ _a1{};
	ਦਸ਼ਮਲਵ _a2{};
	ਦਸ਼ਮਲਵ _b0{};
	ਦਸ਼ਮਲਵ _b1{};
	ਦਸ਼ਮਲਵ _b2{};
	ਦਸ਼ਮਲਵ _delay_element_1{};
	ਦਸ਼ਮਲਵ _delay_element_2{};
};

ਦਸ਼ਮਲਵ SystemIdentificationTest::apply(ਦਸ਼ਮਲਵ sample)
{
	// Direct Form II implementation
	ਅਚਲ ਦਸ਼ਮਲਵ delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};
	ਅਚਲ ਦਸ਼ਮਲਵ output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

	_delay_element_2 = _delay_element_1;
	_delay_element_1 = delay_element_0;

	ਵਾਪਸ output;
}

TEST_F(SystemIdentificationTest, basicTest)
{
	constexpr ਦਸ਼ਮਲਵ fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.ਸੈੱਟ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.ਸੈੱਟ_Lpf_Cutoff_Frequency(fs, 30.f);
	_sys_id.ਸੈੱਟ_Forgetting_ਗੁਣਕ(80.f, 1.f / fs);

	ਲਈ (ਪੂਰਨ i = 0; i < 5; i++) {
		// Fill the buffers with zeros
		_sys_id.update(0.f, 0.f);
	}

	ਲਈ (ਪੂਰਨ i = 0; i < 10; i += 2) {
		_sys_id.update(ਦਸ਼ਮਲਵ(i), ਦਸ਼ਮਲਵ(i + 1));
	}

	ਅਚਲ ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 5> coefficients = _sys_id.getCoefficients();
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_check[] = {-3.96f, 1.30f, -2.28f, -0.33f, 0.369f};
	ਅਚਲ ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 5> coefficients_check(ਅੰਕੜੇ_check);
	ਦਸ਼ਮਲਵ eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).ਨਿਰਪੇਖ().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, resetTest)
{
	constexpr ਦਸ਼ਮਲਵ fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.ਸੈੱਟ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.ਸੈੱਟ_Lpf_Cutoff_Frequency(fs, 30.f);
	_sys_id.ਸੈੱਟ_Forgetting_ਗੁਣਕ(80.f, 1.f / fs);

	ਲਈ (ਪੂਰਨ i = 0; i < 10; i += 2) {
		_sys_id.update(ਦਸ਼ਮਲਵ(i), ਦਸ਼ਮਲਵ(i + 1));
	}

	ਅਚਲ ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 5> coefficients = _sys_id.getCoefficients();

	// WHEN: resetting
	_sys_id.reset();

	// THEN: the variances and coefficients should be properly reset
	EXPECT_TRUE(_sys_id.getCoefficients().ਨਿਰਪੇਖ().max() < 1e-8f);
	EXPECT_TRUE(_sys_id.getVariances().min() > 9e3f);

	// AND WHEN: running the same sequence of inputs-outputs

	ਲਈ (ਪੂਰਨ i = 0; i < 10; i += 2) {
		_sys_id.update(ਦਸ਼ਮਲਵ(i), ਦਸ਼ਮਲਵ(i + 1));
	}

	// THEN: the result should be exactly the same
	EXPECT_TRUE((coefficients - _sys_id.getCoefficients()).ਨਿਰਪੇਖ().max() < 1e-8f);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, simulatedModelTest)
{
	constexpr ਦਸ਼ਮਲਵ fs = 200.f;
	ਅਚਲ ਦਸ਼ਮਲਵ gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.ਸੈੱਟ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.ਸੈੱਟ_Lpf_Cutoff_Frequency(fs, gyro_lpf_cutoff);
	_sys_id.ਸੈੱਟ_Forgetting_ਗੁਣਕ(60.f, 1.f / fs);

	// Simulated model with integrator
	ਅਚਲ ਦਸ਼ਮਲਵ a1 = -1.77f;
	ਅਚਲ ਦਸ਼ਮਲਵ a2 = 0.77f;
	ਅਚਲ ਦਸ਼ਮਲਵ b0 = 0.3812f;
	ਅਚਲ ਦਸ਼ਮਲਵ b1 = -0.25f;
	ਅਚਲ ਦਸ਼ਮਲਵ b2 = 0.2f;
	ਸੈੱਟ_Coefficients(a1, a2, b0, b1, b2);
	ਅਚਲ ਦਸ਼ਮਲਵ u_bias = -0.1f; // constant control offset
	ਅਚਲ ਦਸ਼ਮਲਵ y_bias = 0.2f; // measurement bias

	ਅਚਲ ਦਸ਼ਮਲਵ dt = 1.f / fs;
	ਅਚਲ ਦਸ਼ਮਲਵ duration = 2.f;
	ਦਸ਼ਮਲਵ u = 0.f;
	ਦਸ਼ਮਲਵ y = 0.f;

	ਲਈ (ਪੂਰਨ i = 0; i < static_cast<ਪੂਰਨ>(duration / dt); i++) {

		// Generate square input signal
		ਜੇ (_sys_id.areFiltersInitialized()
		    && (i % 30 == 0)) {
			ਜੇ (u > 0.f) {
				u = -1.f;

			} ਨਹੀਂਤਾਂ {
				u = 1.f;
			}
		}

		_sys_id.update(u + u_bias, y + y_bias); // apply new input and previous output

		y = apply(u);
#ਜੇ 0
		ਅਚਲ ਦਸ਼ਮਲਵ t = i * dt;
		ਲਿਖੋ("%.6f, %.6f, %.6f\n", (ਦੁੱਗਣਾ)t, (ਦੁੱਗਣਾ)u, (ਦੁੱਗਣਾ)y);
#ਜੇ_ਅੰਤ
	}

	ਅਚਲ ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 5> coefficients = _sys_id.getCoefficients();
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_check[] = {a1, a2, b0, b1, b2};
	ਅਚਲ ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 5> coefficients_check(ਅੰਕੜੇ_check);
	ਦਸ਼ਮਲਵ eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).ਨਿਰਪੇਖ().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, real_ਅੰਕੜੇ_Test)
{
	ਸਥਿਰ constexpr ਪੂਰਨ n_samples_dt_avg = 100;
	ਸਥਿਰ constexpr ਪੂਰਨ n_samples_test_ਅੰਕੜੇ = ਆਕਾਰ(u_ਅੰਕੜੇ) / ਆਕਾਰ(u_ਅੰਕੜੇ[0]);
	ਦਸ਼ਮਲਵ dt_ਜੋੜ = 0.f;

	ਲਈ (ਪੂਰਨ i = 1; i <= n_samples_dt_avg; i++) {
		dt_ਜੋੜ += t_ਅੰਕੜੇ[i] - t_ਅੰਕੜੇ[i - 1];
	}

	ਅਚਲ ਦਸ਼ਮਲਵ dt = dt_ਜੋੜ / n_samples_dt_avg;
	ਅਚਲ ਦਸ਼ਮਲਵ fs = 1.f / dt;

	ਲਿਖੋ("dt = %.6f fs = %.3f\n", (ਦੁੱਗਣਾ)dt, (ਦੁੱਗਣਾ)(1.f / dt));

	ਅਚਲ ਦਸ਼ਮਲਵ gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.ਸੈੱਟ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.ਸੈੱਟ_Lpf_Cutoff_Frequency(fs, gyro_lpf_cutoff);
	_sys_id.ਸੈੱਟ_Forgetting_ਗੁਣਕ(60.f, dt);

	ਦਸ਼ਮਲਵ u = 0.f;
	ਦਸ਼ਮਲਵ y_prev = 0.f;

	ਲਈ (ਪੂਰਨ i = 0; i < n_samples_test_ਅੰਕੜੇ; i++) {
		u = u_ਅੰਕੜੇ[i];
		_sys_id.update(u, y_prev); // apply new input and previous output

		y_prev = y_ਅੰਕੜੇ[i];

#ਜੇ 0
		ਲਿਖੋ("%.6f, %.6f, %.6f\n", (ਦੁੱਗਣਾ)t_ਅੰਕੜੇ[i], (ਦੁੱਗਣਾ)u, (ਦੁੱਗਣਾ)y);
#ਜੇ_ਅੰਤ
	}

	ਅਚਲ ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 5> coefficients = _sys_id.getCoefficients();
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_check[] = {-1.916f, 0.916, -0.017f, -0.001f, 0.036f};
	ਅਚਲ ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 5> coefficients_check(ਅੰਕੜੇ_check);
	ਦਸ਼ਮਲਵ eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).ਨਿਰਪੇਖ().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}
