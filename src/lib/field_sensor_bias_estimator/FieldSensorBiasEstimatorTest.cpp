/****************************************************************************
 *
 *   Copyright (C) 2020 PX4 Development Team. All rights reserved.
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
 * Test code for the Field Sensor Bias Estimator
 * Run this test only using make tests TESTFILTER=FieldSensorBiasEstimator
 */

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <FieldSensorBiasEstimator.hpp>

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;

TEST(MagnetometerBiasEstimatorTest, constantZRotation)
{
	ಕ್ಷೇತ್ರ_Sensor_Bias_Estimator ಕ್ಷೇತ್ರ_sensor_bias_estimator;
	ಕ್ಷೇತ್ರ_sensor_bias_estimator.ಗಣ_Learning_Gain(10000.f);
	ಸ್ಥಿರಾಂಕ Vector3f virtual_gyro = Vector3f(0.f, 0.f, 0.1f);
	Vector3f virtual_unbiased_mag = Vector3f(0.9f, 0.f, 1.79f); // taken from SITL jmavsim initialization
	ಸ್ಥಿರಾಂಕ Vector3f virtual_bias(0.2f, -0.4f, 0.5f);
	Vector3f virtual_mag = virtual_unbiased_mag + virtual_bias;

	// Initialize with the current measurement
	ಕ್ಷೇತ್ರ_sensor_bias_estimator.ಗಣ_ಕ್ಷೇತ್ರ(virtual_mag);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i <= 1000; i++) {
		ದಶಮಾಂಶ dt = .01f;

		// turn the mag values according to the gyro

		virtual_mag = virtual_unbiased_mag + virtual_bias;
		// printf("---- %i\n", i);
		// printf("virtual_gyro\n"); virtual_gyro.print();
		// printf("virtual_mag\n"); virtual_mag.print();

		ಕ್ಷೇತ್ರ_sensor_bias_estimator.updateEstimate(virtual_gyro, virtual_mag, dt);
		virtual_unbiased_mag = Dcmf(AxisAnglef(-virtual_gyro * dt)) * virtual_unbiased_mag;
	}

	ಸ್ಥಿರಾಂಕ Vector3f bias_est = ಕ್ಷೇತ್ರ_sensor_bias_estimator.getBias();
	EXPECT_NEAR(bias_est(0), virtual_bias(0), 1e-3f) << "Estimated X bias " << bias_est(0);
	EXPECT_NEAR(bias_est(1), virtual_bias(1), 1e-3f) << "Estimated Y bias " << bias_est(1);
	// The Z bias is not observable due to pure yaw rotation
	EXPECT_NEAR(bias_est(2), 0.f, 1e-3f) << "Estimated Z bias " << bias_est(2);
}
