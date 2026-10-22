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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/matrix/math.hpp>

ਵਰਗ ਖੇਤਰ_Sensor_Bias_Estimator
{
ਜਨਤਕ:
	ਖੇਤਰ_Sensor_Bias_Estimator() = ਮੂਲ;
	~ਖੇਤਰ_Sensor_Bias_Estimator() = ਮੂਲ;

	// Set initial states
	ਖਾਲੀ ਸੈੱਟ_ਖੇਤਰ(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &ਖੇਤਰ) { ਖੇਤਰ_prev = ਖੇਤਰ; }
	ਖਾਲੀ ਸੈੱਟ_Bias(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &bias) { _state_bias = bias; }
	ਖਾਲੀ ਸੈੱਟ_Learning_Gain(ਦਸ਼ਮਲਵ learning_gain) { _learning_gain = learning_gain; }

	/**
	 * Update the estimator and extract updated biases.
	 * @param gyro bias corrected gyroscope data in the same coordinate frame as the field sensor data
	 * @param field biased field sensor data
	 * @param dt time in seconds since the last update
	 */
	ਖਾਲੀ updateEstimate(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &gyro, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &ਖੇਤਰ, ਅਚਲ ਦਸ਼ਮਲਵ dt)
	{
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f ਖੇਤਰ_pred = ਖੇਤਰ_prev + (-gyro % (ਖੇਤਰ_prev - _state_bias)) * dt;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f ਖੇਤਰ_innov = ਖੇਤਰ - ਖੇਤਰ_pred;
		_state_bias += _learning_gain * (-gyro % ਖੇਤਰ_innov) * dt;
		ਖੇਤਰ_prev = ਖੇਤਰ;
	}

	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &get_ਖੇਤਰ() { ਵਾਪਸ ਖੇਤਰ_prev; }
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &getBias() { ਵਾਪਸ _state_bias; }

ਨਿਜੀ:
	//ਹਾਲਤਾਂ
	ਮੈਟ੍ਰਿਕਸ::Vector3f ਖੇਤਰ_prev{};
	ਮੈਟ੍ਰਿਕਸ::Vector3f _state_bias{};
	ਦਸ਼ਮਲਵ _learning_gain{20.f};
};
