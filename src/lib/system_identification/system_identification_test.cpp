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

#அந்தர்பாவ <gtest/gtest.h>
#அந்தர்பாவ <matrix/matrix/math.hpp>

#அந்தர்பாவ "system_identification.hpp"
#அந்தர்பாவ "test_data.h"

using நாமாகாஶ matrix;

வகுப்பு SystemIdentificationTest : பொது ::testing::Test
{
பொது:
	SystemIdentificationTest() {};
	தசம apply(தசம sample);
	வெறுமை setCoefficients(தசம a1, தசம a2, தசம b0, தசம b1, தசம b2)
	{
		_a1 = a1;
		_a2 = a2;
		_b0 = b0;
		_b1 = b1;
		_b2 = b2;
	}

தனிப்பட்ட:
	தசம _a1{};
	தசம _a2{};
	தசம _b0{};
	தசம _b1{};
	தசம _b2{};
	தசம _delay_element_1{};
	தசம _delay_element_2{};
};

தசம SystemIdentificationTest::apply(தசம sample)
{
	// Direct Form II implementation
	மாறிலி தசம delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};
	மாறிலி தசம output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

	_delay_element_2 = _delay_element_1;
	_delay_element_1 = delay_element_0;

	திரும்பு output;
}

TEST_F(SystemIdentificationTest, basicTest)
{
	constexpr தசம fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	க்கு (முழு i = 0; i < 5; i++) {
		// Fill the buffers with zeros
		_sys_id.update(0.f, 0.f);
	}

	க்கு (முழு i = 0; i < 10; i += 2) {
		_sys_id.update(தசம(i), தசம(i + 1));
	}

	மாறிலி Vector<தசம, 5> coefficients = _sys_id.getCoefficients();
	தசம data_check[] = {-3.96f, 1.30f, -2.28f, -0.33f, 0.369f};
	மாறிலி Vector<தசம, 5> coefficients_check(data_check);
	தசம eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, resetTest)
{
	constexpr தசம fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	க்கு (முழு i = 0; i < 10; i += 2) {
		_sys_id.update(தசம(i), தசம(i + 1));
	}

	மாறிலி Vector<தசம, 5> coefficients = _sys_id.getCoefficients();

	// WHEN: resetting
	_sys_id.reset();

	// THEN: the variances and coefficients should be properly reset
	EXPECT_TRUE(_sys_id.getCoefficients().abs().max() < 1e-8f);
	EXPECT_TRUE(_sys_id.getVariances().min() > 9e3f);

	// AND WHEN: running the same sequence of inputs-outputs

	க்கு (முழு i = 0; i < 10; i += 2) {
		_sys_id.update(தசம(i), தசம(i + 1));
	}

	// THEN: the result should be exactly the same
	EXPECT_TRUE((coefficients - _sys_id.getCoefficients()).abs().max() < 1e-8f);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, simulatedModelTest)
{
	constexpr தசம fs = 200.f;
	மாறிலி தசம gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, 1.f / fs);

	// Simulated model with integrator
	மாறிலி தசம a1 = -1.77f;
	மாறிலி தசம a2 = 0.77f;
	மாறிலி தசம b0 = 0.3812f;
	மாறிலி தசம b1 = -0.25f;
	மாறிலி தசம b2 = 0.2f;
	setCoefficients(a1, a2, b0, b1, b2);
	மாறிலி தசம u_bias = -0.1f; // constant control offset
	மாறிலி தசம y_bias = 0.2f; // measurement bias

	மாறிலி தசம dt = 1.f / fs;
	மாறிலி தசம duration = 2.f;
	தசம u = 0.f;
	தசம y = 0.f;

	க்கு (முழு i = 0; i < static_cast<முழு>(duration / dt); i++) {

		// Generate square input signal
		எனில் (_sys_id.areFiltersInitialized()
		    && (i % 30 == 0)) {
			எனில் (u > 0.f) {
				u = -1.f;

			} இல்லையெனில் {
				u = 1.f;
			}
		}

		_sys_id.update(u + u_bias, y + y_bias); // apply new input and previous output

		y = apply(u);
#எனில் 0
		மாறிலி தசம t = i * dt;
		printf("%.6f, %.6f, %.6f\n", (இரட்டை)t, (இரட்டை)u, (இரட்டை)y);
#endif
	}

	மாறிலி Vector<தசம, 5> coefficients = _sys_id.getCoefficients();
	தசம data_check[] = {a1, a2, b0, b1, b2};
	மாறிலி Vector<தசம, 5> coefficients_check(data_check);
	தசம eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, realDataTest)
{
	நிலையான constexpr முழு n_samples_dt_avg = 100;
	நிலையான constexpr முழு n_samples_test_data = அளவு(u_data) / அளவு(u_data[0]);
	தசம dt_sum = 0.f;

	க்கு (முழு i = 1; i <= n_samples_dt_avg; i++) {
		dt_sum += t_data[i] - t_data[i - 1];
	}

	மாறிலி தசம dt = dt_sum / n_samples_dt_avg;
	மாறிலி தசம fs = 1.f / dt;

	printf("dt = %.6f fs = %.3f\n", (இரட்டை)dt, (இரட்டை)(1.f / dt));

	மாறிலி தசம gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, dt);

	தசம u = 0.f;
	தசம y_prev = 0.f;

	க்கு (முழு i = 0; i < n_samples_test_data; i++) {
		u = u_data[i];
		_sys_id.update(u, y_prev); // apply new input and previous output

		y_prev = y_data[i];

#எனில் 0
		printf("%.6f, %.6f, %.6f\n", (இரட்டை)t_data[i], (இரட்டை)u, (இரட்டை)y);
#endif
	}

	மாறிலி Vector<தசம, 5> coefficients = _sys_id.getCoefficients();
	தசம data_check[] = {-1.916f, 0.916, -0.017f, -0.001f, 0.036f};
	மாறிலி Vector<தசம, 5> coefficients_check(data_check);
	தசம eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}
