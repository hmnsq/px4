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

#ہدایت once

#شامل <matrix/matrix/math.hpp>

جماعت میدان_Sensor_Bias_Estimator
{
عوامی:
	میدان_Sensor_Bias_Estimator() = طےشدہ;
	~میدان_Sensor_Bias_Estimator() = طےشدہ;

	// Set initial states
	خالی سیٹ_میدان(ثابت میٹرکس::Vector3f &میدان) { میدان_پچھلا = میدان; }
	خالی سیٹ_Bias(ثابت میٹرکس::Vector3f &bias) { _state_bias = bias; }
	خالی سیٹ_Learning_Gain(اعشاری learning_gain) { _learning_gain = learning_gain; }

	/**
	 * Update the estimator and extract updated biases.
	 * @param gyro bias corrected gyroscope data in the same coordinate frame as the field sensor data
	 * @param field biased field sensor data
	 * @param dt time in seconds since the last update
	 */
	خالی updateEstimate(ثابت میٹرکس::Vector3f &gyro, ثابت میٹرکس::Vector3f &میدان, ثابت اعشاری dt)
	{
		ثابت میٹرکس::Vector3f میدان_pred = میدان_پچھلا + (-gyro % (میدان_پچھلا - _state_bias)) * dt;
		ثابت میٹرکس::Vector3f میدان_innov = میدان - میدان_pred;
		_state_bias += _learning_gain * (-gyro % میدان_innov) * dt;
		میدان_پچھلا = میدان;
	}

	ثابت میٹرکس::Vector3f &get_میدان() { واپس میدان_پچھلا; }
	ثابت میٹرکس::Vector3f &getBias() { واپس _state_bias; }

نجی:
	//حالتیں
	میٹرکس::Vector3f میدان_پچھلا{};
	میٹرکس::Vector3f _state_bias{};
	اعشاری _learning_gain{20.f};
};
