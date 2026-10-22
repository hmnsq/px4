/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
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
 * @file second_order_reference_model.hpp
 *
 * @brief Implementation of a second order system with optional rate feed-forward.
 *
 * @author Thomas Stastny <thomas.stastny@auterion.com>
 */

#pragma once

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <matrix/SquareMatrix.hpp>

ನಾಮಾಕಾಶ math
{

template<typename T>
ವರ್ಗ SecondOrderReferenceModel
{
ಸಾರ್ವಜನಿಕ:
	SecondOrderReferenceModel() = ಸ್ವಭಾವ;

	/**
	 * Construct with system parameters
	 *
	 * @param[in] natural_freq The desired natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 */
	SecondOrderReferenceModel(ಧ್ರುವ ಭಿನ್ನ natural_freq, ಧ್ರುವ ಭಿನ್ನ damping_ratio)
	{
		setParameters(natural_freq, damping_ratio);
	}

	/**
	 * Enumeration for available time discretization methods
	 */
	ಗಣನಾ DiscretizationMethod {
		kForwardEuler,
		kBilinear
	};

	/**
	 * Set the time discretization method used for state integration
	 */
	ಶೂನ್ಯ setDiscretizationMethod(ಧ್ರುವ DiscretizationMethod method) { discretization_method_ = method; }

	/**
	 * Set the system parameters
	 *
	 * Calculates the damping coefficient, spring constant, and maximum allowed
	 * time step based on the natural frequency.
	 *
	 * @param[in] natural_freq The desired undamped natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 * @return Whether or not the param set was successful
	 */
	ತಾರ್ಕಿಕ setParameters(ಧ್ರುವ ಭಿನ್ನ natural_freq, ಧ್ರುವ ಭಿನ್ನ damping_ratio)
	{
		ಯದಿ (natural_freq < FLT_EPSILON || damping_ratio < FLT_EPSILON) {

			// Deadzone the resulting constants (will result in zero filter acceleration)
			spring_constant_ = FLT_EPSILON;
			damping_coefficient_ = FLT_EPSILON;

			// Zero natural frequency means our time step is irrelevant
			cutoff_freq_ = FLT_EPSILON;
			max_time_step_ = INFINITY;

			// Fail
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}

		// Note these are "effective" coefficients, as mass coefficient is considered baked in
		spring_constant_ = natural_freq * natural_freq;
		damping_coefficient_ = 2.0f * damping_ratio * natural_freq;

		cutoff_freq_ = calculateCutoffFrequency(natural_freq, damping_ratio);

		// Based on *conservative nyquist frequency via kSampleRateMultiplier >= 2
		max_time_step_ = 2.0f * M_PI_F / (cutoff_freq_ * kSampleRateMultiplier);

		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	/**
	 * @return System state [units]
	 */
	ಧ್ರುವ T &getState() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ filter_state_; }

	/**
	 * @return System rate [units/s]
	 */
	ಧ್ರುವ T &getRate() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ filter_rate_; }

	/**
	 * @return System acceleration [units/s^2]
	 */
	ಧ್ರುವ T &getAccel() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ filter_accel_; }

	/**
	 * Update the system states
	 *
	 * Units of rate_sample must correspond to the derivative of state_sample units.
	 *
	 * @param[in] time_step Time since last sample [s]
	 * @param[in] state_sample New state sample [units]
	 * @param[in] rate_sample New rate sample, if provided, otherwise defaults to zero(s) [units/s]
	 */
	ಶೂನ್ಯ update(ಧ್ರುವ ಭಿನ್ನ time_step, ಧ್ರುವ T &state_sample, ಧ್ರುವ T &rate_sample = T())
	{
		ಯದಿ ((time_step > max_time_step_) || (time_step < 0.0f)) {
			// time step is too large or is negative, reset the filter
			reset(state_sample, rate_sample);
			ಪ್ರತ್ಯಾಗಮ;
		}

		// take a step forward from the last state (and input), update the filter states
		integrateStates(time_step, last_state_sample_, last_rate_sample_);

		// instantaneous acceleration from current input / state
		filter_accel_ = calculateInstantaneousAcceleration(state_sample, rate_sample);

		// store the current samples
		last_state_sample_ = state_sample;
		last_rate_sample_ = rate_sample;
	}

	/**
	 * Reset the system states
	 *
	 * @param[in] state Initial state [units]
	 * @param[in] rate Initial rate, if provided, otherwise defaults to zero(s) [units/s]
	 */
	ಶೂನ್ಯ reset(ಧ್ರುವ T &state, ಧ್ರುವ T &rate = T())
	{
		filter_state_ = state;
		filter_rate_ = rate;
		filter_accel_ = T();

		last_state_sample_ = state;
		last_rate_sample_ = rate;
	}

ಗುಪ್ತ:

	// A conservative multiplier (>=2) on sample frequency to bound the maximum time step
	ಸ್ಥಿರ constexpr ಭಿನ್ನ kSampleRateMultiplier = 4.0f;

	// (effective, no mass) Spring constant for second order system [s^-2]
	ಭಿನ್ನ spring_constant_{FLT_EPSILON};

	// (effective, no mass) Damping coefficient for second order system [s^-1]
	ಭಿನ್ನ damping_coefficient_{FLT_EPSILON};

	// cutoff frequency [rad/s]
	ಭಿನ್ನ cutoff_freq_{FLT_EPSILON};

	T filter_state_{}; // [units]
	T filter_rate_{}; // [units/s]
	T filter_accel_{}; // [units/s^2]

	// the last samples need to be stored because we don't know the time step over which we integrate to update to the
	// next step a priori
	T last_state_sample_{}; // [units]
	T last_rate_sample_{}; // [units/s]

	// Maximum time step [s]
	ಭಿನ್ನ max_time_step_{INFINITY};

	// The selected time discretization method used for state integration
	DiscretizationMethod discretization_method_{kBilinear};

	/**
	 * Calculate the cutoff frequency in terms of undamped natural frequency and damping ratio
	 *
	 * @param[in] natural_freq The desired undamped natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 * @return Cutoff frequency [rad/s]
	 */
	ಭಿನ್ನ calculateCutoffFrequency(ಧ್ರುವ ಭಿನ್ನ natural_freq, ಧ್ರುವ ಭಿನ್ನ damping_ratio)
	{
		ಧ್ರುವ ಭಿನ್ನ damping_ratio_squared = damping_ratio * damping_ratio;
		ಪ್ರತ್ಯಾಗಮ natural_freq * sqrtf(1.0f - 2.0f * damping_ratio_squared + sqrtf(4.0f * damping_ratio_squared *
					    damping_ratio_squared - 4.0f * damping_ratio_squared + 2.0f));
	}

	/**
	 * Take one integration step using selected discretization method
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	ಶೂನ್ಯ integrateStates(ಧ್ರುವ ಭಿನ್ನ time_step, ಧ್ರುವ T &state_sample, ಧ್ರುವ T &rate_sample)
	{
		ವಿಕಲ್ಪ (discretization_method_) {
		ಪಕ್ಷ DiscretizationMethod::kForwardEuler: {
				// forward-Euler discretization
				integrateStatesForwardEuler(time_step, state_sample, rate_sample);
				ಭಙ್ಗ;
			}

		ಸ್ವಭಾವ: {
				// default to bilinear transform
				integrateStatesBilinear(time_step, state_sample, rate_sample);
			}
		}
	}

	/**
	 * Take one integration step using Euler-forward integration
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	ಶೂನ್ಯ integrateStatesForwardEuler(ಧ್ರುವ ಭಿನ್ನ time_step, ಧ್ರುವ T &state_sample, ಧ್ರುವ T &rate_sample)
	{
		// general notation for what follows:
		// c: continuous
		// d: discrete
		// Kx: spring constant
		// Kv: damping coefficient
		// T: sample time

		// state matrix
		// Ac = [ 0   1 ]
		//      [-Kx -Kv]
		// Ad = I + Ac * T
		matrix::SquareMatrix<ಭಿನ್ನ, 2> state_matrix;
		state_matrix(0, 0) = 1.0f;
		state_matrix(0, 1) = time_step;
		state_matrix(1, 0) = -spring_constant_ * time_step;
		state_matrix(1, 1) = -damping_coefficient_ * time_step + 1.0f;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Bc * T
		matrix::SquareMatrix<ಭಿನ್ನ, 2> ಒಳಹರಿವು_matrix;
		ಒಳಹರಿವು_matrix(0, 0) = 0.0f;
		ಒಳಹರಿವು_matrix(0, 1) = 0.0f;
		ಒಳಹರಿವು_matrix(1, 0) = spring_constant_ * time_step;
		ಒಳಹರಿವು_matrix(1, 1) = damping_coefficient_ * time_step;

		// discrete state transition
		transitionStates(state_matrix, ಒಳಹರಿವು_matrix, state_sample, rate_sample);
	}

	/**
	 * Take one integration step using discrete state space calculated from bilinear transform
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	ಶೂನ್ಯ integrateStatesBilinear(ಧ್ರುವ ಭಿನ್ನ time_step, ಧ್ರುವ T &state_sample, ಧ್ರುವ T &rate_sample)
	{
		ಧ್ರುವ ಭಿನ್ನ time_step_squared = time_step * time_step;
		ಧ್ರುವ ಭಿನ್ನ inv_denominator = 1.0f / (0.25f * spring_constant_ * time_step_squared + 0.5f * damping_coefficient_ *
						      time_step + 1.0f);

		// general notation for what follows:
		// c: continuous
		// d: discrete
		// Kx: spring constant
		// Kv: damping coefficient
		// T: sample time

		// state matrix
		// Ac = [ 0   1 ]
		//      [-Kx -Kv]
		// Ad = (I + 1/2 * Ac * T) * (I - 1/2 * Ac * T)^-1
		matrix::SquareMatrix<ಭಿನ್ನ, 2> state_matrix;
		state_matrix(0, 0) = -0.25f * spring_constant_ * time_step_squared + 0.5f * damping_coefficient_ * time_step + 1.0f;
		state_matrix(0, 1) = time_step;
		state_matrix(1, 0) = -spring_constant_ * time_step;
		state_matrix(1, 1) = -0.25f * spring_constant_ * time_step_squared - 0.5f * damping_coefficient_ * time_step + 1.0f;
		state_matrix *= inv_denominator;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Ac^-1 * (Ad - I) * Bc
		matrix::SquareMatrix<ಭಿನ್ನ, 2> ಒಳಹರಿವು_matrix;
		ಒಳಹರಿವು_matrix(0, 0) = 0.5f * spring_constant_ * time_step_squared;
		ಒಳಹರಿವು_matrix(0, 1) = 0.5f * damping_coefficient_ * time_step_squared;
		ಒಳಹರಿವು_matrix(1, 0) = spring_constant_ * time_step;
		ಒಳಹರಿವು_matrix(1, 1) = damping_coefficient_ * time_step;
		ಒಳಹರಿವು_matrix *= inv_denominator;

		// discrete state transition
		transitionStates(state_matrix, ಒಳಹರಿವು_matrix, state_sample, rate_sample);
	}

	/**
	 * Transition the states using discrete state space matrices
	 *
	 * @param[in] state_matrix Discrete state matrix (2x2)
	 * @param[in] input_matrix Discrete input matrix (2x2)
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	ಶೂನ್ಯ transitionStates(ಧ್ರುವ matrix::SquareMatrix<ಭಿನ್ನ, 2> &state_matrix,
			      ಧ್ರುವ matrix::SquareMatrix<ಭಿನ್ನ, 2> &ಒಳಹರಿವು_matrix, ಧ್ರುವ T &state_sample, ಧ್ರುವ T &rate_sample)
	{
		ಧ್ರುವ T new_state = state_matrix(0, 0) * filter_state_ + state_matrix(0, 1) * filter_rate_ + ಒಳಹರಿವು_matrix(0,
				    0) * state_sample + ಒಳಹರಿವು_matrix(0, 1) * rate_sample;
		ಧ್ರುವ T new_rate = state_matrix(1, 0) * filter_state_ + state_matrix(1, 1) * filter_rate_ + ಒಳಹರಿವು_matrix(1,
				   0) * state_sample + ಒಳಹರಿವು_matrix(1, 1) * rate_sample;
		filter_state_ = new_state;
		filter_rate_ = new_rate;
	}

	/**
	 * Calculate the instantaneous acceleration of the system
	 *
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	T calculateInstantaneousAcceleration(ಧ್ರುವ T &state_sample, ಧ್ರುವ T &rate_sample) ಧ್ರುವ
	{
		ಧ್ರುವ T state_error = state_sample - filter_state_;
		ಧ್ರುವ T rate_error = rate_sample - filter_rate_;
		ಪ್ರತ್ಯಾಗಮ state_error * spring_constant_ + rate_error * damping_coefficient_;
	}
};

} // namespace math
