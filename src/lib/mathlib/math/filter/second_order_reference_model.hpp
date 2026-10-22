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

#निर्देशः once

#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <matrix/SquareMatrix.hpp>

नामाकाश math
{

प्रतिरूपम्<प्रकारनाम T>
वर्ग Second_क्रमः_Reference_Model
{
सार्वजनिक:
	Second_क्रमः_Reference_Model() = स्वभाव;

	/**
	 * Construct with system parameters
	 *
	 * @param[in] natural_freq The desired natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 */
	Second_क्रमः_Reference_Model(ध्रुव भिन्न natural_freq, ध्रुव भिन्न damping_अनुपातः)
	{
		समुच्चयः_Parameters(natural_freq, damping_अनुपातः);
	}

	/**
	 * Enumeration for available time discretization methods
	 */
	गणना Discretization_विधानम् {
		kForwardEuler,
		kBilinear
	};

	/**
	 * Set the time discretization method used for state integration
	 */
	शून्य समुच्चयः_Discretization_विधानम्(ध्रुव Discretization_विधानम् विधानम्) { discretization_विधानम् = विधानम्; }

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
	तार्किक समुच्चयः_Parameters(ध्रुव भिन्न natural_freq, ध्रुव भिन्न damping_अनुपातः)
	{
		यदि (natural_freq < FLT_EPSILON || damping_अनुपातः < FLT_EPSILON) {

			// Deadzone the resulting constants (will result in zero filter acceleration)
			spring_constant_ = FLT_EPSILON;
			damping_coefficient_ = FLT_EPSILON;

			// Zero natural frequency means our time step is irrelevant
			cutoff_freq_ = FLT_EPSILON;
			max_time_चरणः = INFINITY;

			// Fail
			प्रत्यागम असत्य;
		}

		// Note these are "effective" coefficients, as mass coefficient is considered baked in
		spring_constant_ = natural_freq * natural_freq;
		damping_coefficient_ = 2.0f * damping_अनुपातः * natural_freq;

		cutoff_freq_ = calculateCutoffFrequency(natural_freq, damping_अनुपातः);

		// Based on *conservative nyquist frequency via kSampleRateMultiplier >= 2
		max_time_चरणः = 2.0f * M_PI_F / (cutoff_freq_ * k_Sample_दरः_Multiplier);

		प्रत्यागम सत्य;
	}

	/**
	 * @return System state [units]
	 */
	ध्रुव T &getState() ध्रुव { प्रत्यागम filter_state_; }

	/**
	 * @return System rate [units/s]
	 */
	ध्रुव T &get_दरः() ध्रुव { प्रत्यागम filter_दरः; }

	/**
	 * @return System acceleration [units/s^2]
	 */
	ध्रुव T &getAccel() ध्रुव { प्रत्यागम filter_accel_; }

	/**
	 * Update the system states
	 *
	 * Units of rate_sample must correspond to the derivative of state_sample units.
	 *
	 * @param[in] time_step Time since last sample [s]
	 * @param[in] state_sample New state sample [units]
	 * @param[in] rate_sample New rate sample, if provided, otherwise defaults to zero(s) [units/s]
	 */
	शून्य update(ध्रुव भिन्न time_चरणः, ध्रुव T &state_sample, ध्रुव T &दरः_sample = T())
	{
		यदि ((time_चरणः > max_time_चरणः) || (time_चरणः < 0.0f)) {
			// time step is too large or is negative, reset the filter
			reset(state_sample, दरः_sample);
			प्रत्यागम;
		}

		// take a step forward from the last state (and input), update the filter states
		integrate_अवस्थाः(time_चरणः, चरमः_state_sample, चरमः_दरः_sample);

		// instantaneous acceleration from current input / state
		filter_accel_ = calculateInstantaneousAcceleration(state_sample, दरः_sample);

		// store the current samples
		चरमः_state_sample = state_sample;
		चरमः_दरः_sample = दरः_sample;
	}

	/**
	 * Reset the system states
	 *
	 * @param[in] state Initial state [units]
	 * @param[in] rate Initial rate, if provided, otherwise defaults to zero(s) [units/s]
	 */
	शून्य reset(ध्रुव T &state, ध्रुव T &दरः = T())
	{
		filter_state_ = state;
		filter_दरः = दरः;
		filter_accel_ = T();

		चरमः_state_sample = state;
		चरमः_दरः_sample = दरः;
	}

गुप्त:

	// A conservative multiplier (>=2) on sample frequency to bound the maximum time step
	स्थिर constexpr भिन्न k_Sample_दरः_Multiplier = 4.0f;

	// (effective, no mass) Spring constant for second order system [s^-2]
	भिन्न spring_constant_{FLT_EPSILON};

	// (effective, no mass) Damping coefficient for second order system [s^-1]
	भिन्न damping_coefficient_{FLT_EPSILON};

	// cutoff frequency [rad/s]
	भिन्न cutoff_freq_{FLT_EPSILON};

	T filter_state_{}; // [units]
	T filter_दरः{}; // [units/s]
	T filter_accel_{}; // [units/s^2]

	// the last samples need to be stored because we don't know the time step over which we integrate to update to the
	// next step a priori
	T चरमः_state_sample{}; // [units]
	T चरमः_दरः_sample{}; // [units/s]

	// Maximum time step [s]
	भिन्न max_time_चरणः{INFINITY};

	// The selected time discretization method used for state integration
	Discretization_विधानम् discretization_विधानम्{kBilinear};

	/**
	 * Calculate the cutoff frequency in terms of undamped natural frequency and damping ratio
	 *
	 * @param[in] natural_freq The desired undamped natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 * @return Cutoff frequency [rad/s]
	 */
	भिन्न calculateCutoffFrequency(ध्रुव भिन्न natural_freq, ध्रुव भिन्न damping_अनुपातः)
	{
		ध्रुव भिन्न damping_अनुपातः_squared = damping_अनुपातः * damping_अनुपातः;
		प्रत्यागम natural_freq * sqrtf(1.0f - 2.0f * damping_अनुपातः_squared + sqrtf(4.0f * damping_अनुपातः_squared *
					    damping_अनुपातः_squared - 4.0f * damping_अनुपातः_squared + 2.0f));
	}

	/**
	 * Take one integration step using selected discretization method
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	शून्य integrate_अवस्थाः(ध्रुव भिन्न time_चरणः, ध्रुव T &state_sample, ध्रुव T &दरः_sample)
	{
		विकल्प (discretization_विधानम्) {
		पक्ष Discretization_विधानम्::kForwardEuler: {
				// forward-Euler discretization
				integrate_अवस्थाः_Forward_Euler(time_चरणः, state_sample, दरः_sample);
				भङ्ग;
			}

		स्वभाव: {
				// default to bilinear transform
				integrate_अवस्थाः_Bilinear(time_चरणः, state_sample, दरः_sample);
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
	शून्य integrate_अवस्थाः_Forward_Euler(ध्रुव भिन्न time_चरणः, ध्रुव T &state_sample, ध्रुव T &दरः_sample)
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
		matrix::वर्गफलम्_Matrix<भिन्न, 2> state_matrix;
		state_matrix(0, 0) = 1.0f;
		state_matrix(0, 1) = time_चरणः;
		state_matrix(1, 0) = -spring_constant_ * time_चरणः;
		state_matrix(1, 1) = -damping_coefficient_ * time_चरणः + 1.0f;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Bc * T
		matrix::वर्गफलम्_Matrix<भिन्न, 2> निवेशः_matrix;
		निवेशः_matrix(0, 0) = 0.0f;
		निवेशः_matrix(0, 1) = 0.0f;
		निवेशः_matrix(1, 0) = spring_constant_ * time_चरणः;
		निवेशः_matrix(1, 1) = damping_coefficient_ * time_चरणः;

		// discrete state transition
		transition_अवस्थाः(state_matrix, निवेशः_matrix, state_sample, दरः_sample);
	}

	/**
	 * Take one integration step using discrete state space calculated from bilinear transform
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	शून्य integrate_अवस्थाः_Bilinear(ध्रुव भिन्न time_चरणः, ध्रुव T &state_sample, ध्रुव T &दरः_sample)
	{
		ध्रुव भिन्न time_चरणः_squared = time_चरणः * time_चरणः;
		ध्रुव भिन्न inv_denominator = 1.0f / (0.25f * spring_constant_ * time_चरणः_squared + 0.5f * damping_coefficient_ *
						      time_चरणः + 1.0f);

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
		matrix::वर्गफलम्_Matrix<भिन्न, 2> state_matrix;
		state_matrix(0, 0) = -0.25f * spring_constant_ * time_चरणः_squared + 0.5f * damping_coefficient_ * time_चरणः + 1.0f;
		state_matrix(0, 1) = time_चरणः;
		state_matrix(1, 0) = -spring_constant_ * time_चरणः;
		state_matrix(1, 1) = -0.25f * spring_constant_ * time_चरणः_squared - 0.5f * damping_coefficient_ * time_चरणः + 1.0f;
		state_matrix *= inv_denominator;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Ac^-1 * (Ad - I) * Bc
		matrix::वर्गफलम्_Matrix<भिन्न, 2> निवेशः_matrix;
		निवेशः_matrix(0, 0) = 0.5f * spring_constant_ * time_चरणः_squared;
		निवेशः_matrix(0, 1) = 0.5f * damping_coefficient_ * time_चरणः_squared;
		निवेशः_matrix(1, 0) = spring_constant_ * time_चरणः;
		निवेशः_matrix(1, 1) = damping_coefficient_ * time_चरणः;
		निवेशः_matrix *= inv_denominator;

		// discrete state transition
		transition_अवस्थाः(state_matrix, निवेशः_matrix, state_sample, दरः_sample);
	}

	/**
	 * Transition the states using discrete state space matrices
	 *
	 * @param[in] state_matrix Discrete state matrix (2x2)
	 * @param[in] input_matrix Discrete input matrix (2x2)
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	शून्य transition_अवस्थाः(ध्रुव matrix::वर्गफलम्_Matrix<भिन्न, 2> &state_matrix,
			      ध्रुव matrix::वर्गफलम्_Matrix<भिन्न, 2> &निवेशः_matrix, ध्रुव T &state_sample, ध्रुव T &दरः_sample)
	{
		ध्रुव T new_state = state_matrix(0, 0) * filter_state_ + state_matrix(0, 1) * filter_दरः + निवेशः_matrix(0,
				    0) * state_sample + निवेशः_matrix(0, 1) * दरः_sample;
		ध्रुव T new_दरः = state_matrix(1, 0) * filter_state_ + state_matrix(1, 1) * filter_दरः + निवेशः_matrix(1,
				   0) * state_sample + निवेशः_matrix(1, 1) * दरः_sample;
		filter_state_ = new_state;
		filter_दरः = new_दरः;
	}

	/**
	 * Calculate the instantaneous acceleration of the system
	 *
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	T calculateInstantaneousAcceleration(ध्रुव T &state_sample, ध्रुव T &दरः_sample) ध्रुव
	{
		ध्रुव T state_error = state_sample - filter_state_;
		ध्रुव T दरः_error = दरः_sample - filter_दरः;
		प्रत्यागम state_error * spring_constant_ + दरः_error * damping_coefficient_;
	}
};

} // namespace math
