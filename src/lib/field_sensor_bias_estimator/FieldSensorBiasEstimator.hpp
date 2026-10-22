/****************************************************************************
 *
 *   Copyright (c) 2020 PX4 Development Team. All rights reserved.
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
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAfieldES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAfieldE.
 *
 ****************************************************************************/

/**
 * @file FieldSensorBiasEstimator.hpp
 *
 * Estimator for the magnetometer/accelerometer bias calibration parameters to run online with the help of Gyroscope data.
 *
 * @author Matthias Grob	<maetugr@gmail.com>
 * @author Mathieu Bresciani	<brescianimathieu@gmail.com>
 *
 * Publication documenting the algorithm:
 * Adaptive Estimation of Measurement Bias in Three-Dimensional Field Sensors with Angular-Rate Sensors: Theory and Comparative Experimental Evaluation
 * Giancarlo Troni and Louis L. Whitcomb, Department of Mechanical Engineering, Johns Hopkins University, Baltimore, Maryland 21218, USA
 *
 * http://www.roboticsproceedings.org/rss09/p50.pdf
 */

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>

ವರ್ಗ ಕ್ಷೇತ್ರ_Sensor_Bias_Estimator
{
ಸಾರ್ವಜನಿಕ:
	ಕ್ಷೇತ್ರ_Sensor_Bias_Estimator() = ಪೂರ್ವನಿಯೋಜಿತ;
	~ಕ್ಷೇತ್ರ_Sensor_Bias_Estimator() = ಪೂರ್ವನಿಯೋಜಿತ;

	// Set initial states
	ಶೂನ್ಯ ಗಣ_ಕ್ಷೇತ್ರ(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &ಕ್ಷೇತ್ರ) { ಕ್ಷೇತ್ರ_prev = ಕ್ಷೇತ್ರ; }
	ಶೂನ್ಯ ಗಣ_Bias(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &bias) { _state_bias = bias; }
	ಶೂನ್ಯ ಗಣ_Learning_Gain(ದಶಮಾಂಶ learning_gain) { _learning_gain = learning_gain; }

	/**
	 * Update the estimator and extract updated biases.
	 * @param gyro bias corrected gyroscope data in the same coordinate frame as the field sensor data
	 * @param field biased field sensor data
	 * @param dt time in seconds since the last update
	 */
	ಶೂನ್ಯ updateEstimate(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &gyro, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &ಕ್ಷೇತ್ರ, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt)
	{
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f ಕ್ಷೇತ್ರ_pred = ಕ್ಷೇತ್ರ_prev + (-gyro % (ಕ್ಷೇತ್ರ_prev - _state_bias)) * dt;
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f ಕ್ಷೇತ್ರ_innov = ಕ್ಷೇತ್ರ - ಕ್ಷೇತ್ರ_pred;
		_state_bias += _learning_gain * (-gyro % ಕ್ಷೇತ್ರ_innov) * dt;
		ಕ್ಷೇತ್ರ_prev = ಕ್ಷೇತ್ರ;
	}

	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &get_ಕ್ಷೇತ್ರ() { ಹಿಂತಿರುಗಿ ಕ್ಷೇತ್ರ_prev; }
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &getBias() { ಹಿಂತಿರುಗಿ _state_bias; }

ಖಾಸಗಿ:
	//ಸ್ಥಿತಿಗಳು
	ಮಾತೃಕೆ::Vector3f ಕ್ಷೇತ್ರ_prev{};
	ಮಾತೃಕೆ::Vector3f _state_bias{};
	ದಶಮಾಂಶ _learning_gain{20.f};
};
