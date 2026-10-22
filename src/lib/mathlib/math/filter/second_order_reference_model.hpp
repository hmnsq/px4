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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <matrix/SquareMatrix.hpp>

ನಾಮಾಕಾಶ math
{

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ವರ್ಗ ಎರಡನೇ_ಕ್ರಮ_Reference_Model
{
ಸಾರ್ವಜನಿಕ:
	ಎರಡನೇ_ಕ್ರಮ_Reference_Model() = ಪೂರ್ವನಿಯೋಜಿತ;

	/**
	 * Construct with system parameters
	 *
	 * @param[in] natural_freq The desired natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 */
	ಎರಡನೇ_ಕ್ರಮ_Reference_Model(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ natural_freq, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ damping_ಅನುಪಾತ)
	{
		ಗಣ_Parameters(natural_freq, damping_ಅನುಪಾತ);
	}

	/**
	 * Enumeration for available time discretization methods
	 */
	ಎಣಿಕೆ Discretization_ವಿಧಾನ {
		kForwardEuler,
		kBilinear
	};

	/**
	 * Set the time discretization method used for state integration
	 */
	ಶೂನ್ಯ ಗಣ_Discretization_ವಿಧಾನ(ಸ್ಥಿರಾಂಕ Discretization_ವಿಧಾನ ವಿಧಾನ) { discretization_ವಿಧಾನ = ವಿಧಾನ; }

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
	ತಾರ್ಕಿಕ ಗಣ_Parameters(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ natural_freq, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ damping_ಅನುಪಾತ)
	{
		ಆದರೆ (natural_freq < FLT_EPSILON || damping_ಅನುಪಾತ < FLT_EPSILON) {

			// Deadzone the resulting constants (will result in zero filter acceleration)
			spring_constant_ = FLT_EPSILON;
			damping_coefficient_ = FLT_EPSILON;

			// Zero natural frequency means our time step is irrelevant
			cutoff_freq_ = FLT_EPSILON;
			max_ಸಮಯ_ಹೆಜ್ಜೆ = INFINITY;

			// Fail
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}

		// Note these are "effective" coefficients, as mass coefficient is considered baked in
		spring_constant_ = natural_freq * natural_freq;
		damping_coefficient_ = 2.0f * damping_ಅನುಪಾತ * natural_freq;

		cutoff_freq_ = calculateCutoffFrequency(natural_freq, damping_ಅನುಪಾತ);

		// Based on *conservative nyquist frequency via kSampleRateMultiplier >= 2
		max_ಸಮಯ_ಹೆಜ್ಜೆ = 2.0f * M_PI_F / (cutoff_freq_ * k_Sample_ದರ_Multiplier);

		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	/**
	 * @return System state [units]
	 */
	ಸ್ಥಿರಾಂಕ T &getState() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ filter_state_; }

	/**
	 * @return System rate [units/s]
	 */
	ಸ್ಥಿರಾಂಕ T &get_ದರ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ filter_ದರ; }

	/**
	 * @return System acceleration [units/s^2]
	 */
	ಸ್ಥಿರಾಂಕ T &getAccel() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ filter_accel_; }

	/**
	 * Update the system states
	 *
	 * Units of rate_sample must correspond to the derivative of state_sample units.
	 *
	 * @param[in] time_step Time since last sample [s]
	 * @param[in] state_sample New state sample [units]
	 * @param[in] rate_sample New rate sample, if provided, otherwise defaults to zero(s) [units/s]
	 */
	ಶೂನ್ಯ update(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಸಮಯ_ಹೆಜ್ಜೆ, ಸ್ಥಿರಾಂಕ T &state_sample, ಸ್ಥಿರಾಂಕ T &ದರ_sample = T())
	{
		ಆದರೆ ((ಸಮಯ_ಹೆಜ್ಜೆ > max_ಸಮಯ_ಹೆಜ್ಜೆ) || (ಸಮಯ_ಹೆಜ್ಜೆ < 0.0f)) {
			// time step is too large or is negative, reset the filter
			reset(state_sample, ದರ_sample);
			ಹಿಂತಿರುಗಿ;
		}

		// take a step forward from the last state (and input), update the filter states
		integrate_ಸ್ಥಿತಿಗಳು(ಸಮಯ_ಹೆಜ್ಜೆ, ಕೊನೆಯ_state_sample, ಕೊನೆಯ_ದರ_sample);

		// instantaneous acceleration from current input / state
		filter_accel_ = calculateInstantaneousAcceleration(state_sample, ದರ_sample);

		// store the current samples
		ಕೊನೆಯ_state_sample = state_sample;
		ಕೊನೆಯ_ದರ_sample = ದರ_sample;
	}

	/**
	 * Reset the system states
	 *
	 * @param[in] state Initial state [units]
	 * @param[in] rate Initial rate, if provided, otherwise defaults to zero(s) [units/s]
	 */
	ಶೂನ್ಯ reset(ಸ್ಥಿರಾಂಕ T &state, ಸ್ಥಿರಾಂಕ T &ದರ = T())
	{
		filter_state_ = state;
		filter_ದರ = ದರ;
		filter_accel_ = T();

		ಕೊನೆಯ_state_sample = state;
		ಕೊನೆಯ_ದರ_sample = ದರ;
	}

ಖಾಸಗಿ:

	// A conservative multiplier (>=2) on sample frequency to bound the maximum time step
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ k_Sample_ದರ_Multiplier = 4.0f;

	// (effective, no mass) Spring constant for second order system [s^-2]
	ದಶಮಾಂಶ spring_constant_{FLT_EPSILON};

	// (effective, no mass) Damping coefficient for second order system [s^-1]
	ದಶಮಾಂಶ damping_coefficient_{FLT_EPSILON};

	// cutoff frequency [rad/s]
	ದಶಮಾಂಶ cutoff_freq_{FLT_EPSILON};

	T filter_state_{}; // [units]
	T filter_ದರ{}; // [units/s]
	T filter_accel_{}; // [units/s^2]

	// the last samples need to be stored because we don't know the time step over which we integrate to update to the
	// next step a priori
	T ಕೊನೆಯ_state_sample{}; // [units]
	T ಕೊನೆಯ_ದರ_sample{}; // [units/s]

	// Maximum time step [s]
	ದಶಮಾಂಶ max_ಸಮಯ_ಹೆಜ್ಜೆ{INFINITY};

	// The selected time discretization method used for state integration
	Discretization_ವಿಧಾನ discretization_ವಿಧಾನ{kBilinear};

	/**
	 * Calculate the cutoff frequency in terms of undamped natural frequency and damping ratio
	 *
	 * @param[in] natural_freq The desired undamped natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 * @return Cutoff frequency [rad/s]
	 */
	ದಶಮಾಂಶ calculateCutoffFrequency(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ natural_freq, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ damping_ಅನುಪಾತ)
	{
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ damping_ಅನುಪಾತ_squared = damping_ಅನುಪಾತ * damping_ಅನುಪಾತ;
		ಹಿಂತಿರುಗಿ natural_freq * sqrtf(1.0f - 2.0f * damping_ಅನುಪಾತ_squared + sqrtf(4.0f * damping_ಅನುಪಾತ_squared *
					    damping_ಅನುಪಾತ_squared - 4.0f * damping_ಅನುಪಾತ_squared + 2.0f));
	}

	/**
	 * Take one integration step using selected discretization method
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	ಶೂನ್ಯ integrate_ಸ್ಥಿತಿಗಳು(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಸಮಯ_ಹೆಜ್ಜೆ, ಸ್ಥಿರಾಂಕ T &state_sample, ಸ್ಥಿರಾಂಕ T &ದರ_sample)
	{
		ಆಯ್ಕೆ (discretization_ವಿಧಾನ) {
		ಸ್ಥಿತಿ Discretization_ವಿಧಾನ::kForwardEuler: {
				// forward-Euler discretization
				integrate_ಸ್ಥಿತಿಗಳು_Forward_Euler(ಸಮಯ_ಹೆಜ್ಜೆ, state_sample, ದರ_sample);
				ನಿಲ್ಲಿಸು;
			}

		ಪೂರ್ವನಿಯೋಜಿತ: {
				// default to bilinear transform
				integrate_ಸ್ಥಿತಿಗಳು_Bilinear(ಸಮಯ_ಹೆಜ್ಜೆ, state_sample, ದರ_sample);
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
	ಶೂನ್ಯ integrate_ಸ್ಥಿತಿಗಳು_Forward_Euler(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಸಮಯ_ಹೆಜ್ಜೆ, ಸ್ಥಿರಾಂಕ T &state_sample, ಸ್ಥಿರಾಂಕ T &ದರ_sample)
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
		ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> state_ಮಾತೃಕೆ;
		state_ಮಾತೃಕೆ(0, 0) = 1.0f;
		state_ಮಾತೃಕೆ(0, 1) = ಸಮಯ_ಹೆಜ್ಜೆ;
		state_ಮಾತೃಕೆ(1, 0) = -spring_constant_ * ಸಮಯ_ಹೆಜ್ಜೆ;
		state_ಮಾತೃಕೆ(1, 1) = -damping_coefficient_ * ಸಮಯ_ಹೆಜ್ಜೆ + 1.0f;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Bc * T
		ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> ಒಳಹರಿವು_ಮಾತೃಕೆ;
		ಒಳಹರಿವು_ಮಾತೃಕೆ(0, 0) = 0.0f;
		ಒಳಹರಿವು_ಮಾತೃಕೆ(0, 1) = 0.0f;
		ಒಳಹರಿವು_ಮಾತೃಕೆ(1, 0) = spring_constant_ * ಸಮಯ_ಹೆಜ್ಜೆ;
		ಒಳಹರಿವು_ಮಾತೃಕೆ(1, 1) = damping_coefficient_ * ಸಮಯ_ಹೆಜ್ಜೆ;

		// discrete state transition
		transition_ಸ್ಥಿತಿಗಳು(state_ಮಾತೃಕೆ, ಒಳಹರಿವು_ಮಾತೃಕೆ, state_sample, ದರ_sample);
	}

	/**
	 * Take one integration step using discrete state space calculated from bilinear transform
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	ಶೂನ್ಯ integrate_ಸ್ಥಿತಿಗಳು_Bilinear(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಸಮಯ_ಹೆಜ್ಜೆ, ಸ್ಥಿರಾಂಕ T &state_sample, ಸ್ಥಿರಾಂಕ T &ದರ_sample)
	{
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಸಮಯ_ಹೆಜ್ಜೆ_squared = ಸಮಯ_ಹೆಜ್ಜೆ * ಸಮಯ_ಹೆಜ್ಜೆ;
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ inv_denominator = 1.0f / (0.25f * spring_constant_ * ಸಮಯ_ಹೆಜ್ಜೆ_squared + 0.5f * damping_coefficient_ *
						      ಸಮಯ_ಹೆಜ್ಜೆ + 1.0f);

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
		ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> state_ಮಾತೃಕೆ;
		state_ಮಾತೃಕೆ(0, 0) = -0.25f * spring_constant_ * ಸಮಯ_ಹೆಜ್ಜೆ_squared + 0.5f * damping_coefficient_ * ಸಮಯ_ಹೆಜ್ಜೆ + 1.0f;
		state_ಮಾತೃಕೆ(0, 1) = ಸಮಯ_ಹೆಜ್ಜೆ;
		state_ಮಾತೃಕೆ(1, 0) = -spring_constant_ * ಸಮಯ_ಹೆಜ್ಜೆ;
		state_ಮಾತೃಕೆ(1, 1) = -0.25f * spring_constant_ * ಸಮಯ_ಹೆಜ್ಜೆ_squared - 0.5f * damping_coefficient_ * ಸಮಯ_ಹೆಜ್ಜೆ + 1.0f;
		state_ಮಾತೃಕೆ *= inv_denominator;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Ac^-1 * (Ad - I) * Bc
		ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> ಒಳಹರಿವು_ಮಾತೃಕೆ;
		ಒಳಹರಿವು_ಮಾತೃಕೆ(0, 0) = 0.5f * spring_constant_ * ಸಮಯ_ಹೆಜ್ಜೆ_squared;
		ಒಳಹರಿವು_ಮಾತೃಕೆ(0, 1) = 0.5f * damping_coefficient_ * ಸಮಯ_ಹೆಜ್ಜೆ_squared;
		ಒಳಹರಿವು_ಮಾತೃಕೆ(1, 0) = spring_constant_ * ಸಮಯ_ಹೆಜ್ಜೆ;
		ಒಳಹರಿವು_ಮಾತೃಕೆ(1, 1) = damping_coefficient_ * ಸಮಯ_ಹೆಜ್ಜೆ;
		ಒಳಹರಿವು_ಮಾತೃಕೆ *= inv_denominator;

		// discrete state transition
		transition_ಸ್ಥಿತಿಗಳು(state_ಮಾತೃಕೆ, ಒಳಹರಿವು_ಮಾತೃಕೆ, state_sample, ದರ_sample);
	}

	/**
	 * Transition the states using discrete state space matrices
	 *
	 * @param[in] state_matrix Discrete state matrix (2x2)
	 * @param[in] input_matrix Discrete input matrix (2x2)
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	ಶೂನ್ಯ transition_ಸ್ಥಿತಿಗಳು(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> &state_ಮಾತೃಕೆ,
			      ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> &ಒಳಹರಿವು_ಮಾತೃಕೆ, ಸ್ಥಿರಾಂಕ T &state_sample, ಸ್ಥಿರಾಂಕ T &ದರ_sample)
	{
		ಸ್ಥಿರಾಂಕ T new_state = state_ಮಾತೃಕೆ(0, 0) * filter_state_ + state_ಮಾತೃಕೆ(0, 1) * filter_ದರ + ಒಳಹರಿವು_ಮಾತೃಕೆ(0,
				    0) * state_sample + ಒಳಹರಿವು_ಮಾತೃಕೆ(0, 1) * ದರ_sample;
		ಸ್ಥಿರಾಂಕ T new_ದರ = state_ಮಾತೃಕೆ(1, 0) * filter_state_ + state_ಮಾತೃಕೆ(1, 1) * filter_ದರ + ಒಳಹರಿವು_ಮಾತೃಕೆ(1,
				   0) * state_sample + ಒಳಹರಿವು_ಮಾತೃಕೆ(1, 1) * ದರ_sample;
		filter_state_ = new_state;
		filter_ದರ = new_ದರ;
	}

	/**
	 * Calculate the instantaneous acceleration of the system
	 *
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	T calculateInstantaneousAcceleration(ಸ್ಥಿರಾಂಕ T &state_sample, ಸ್ಥಿರಾಂಕ T &ದರ_sample) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ಥಿರಾಂಕ T state_error = state_sample - filter_state_;
		ಸ್ಥಿರಾಂಕ T ದರ_error = ದರ_sample - filter_ದರ;
		ಹಿಂತಿರುಗಿ state_error * spring_constant_ + ದರ_error * damping_coefficient_;
	}
};

} // namespace math
