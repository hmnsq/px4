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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/SquareMatrix.hpp>

ਨਾਮਾਕਾਸ਼ math
{

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਵਰਗ ਦੂਜਾ_ਕ੍ਰਮ_Reference_Model
{
ਜਨਤਕ:
	ਦੂਜਾ_ਕ੍ਰਮ_Reference_Model() = ਮੂਲ;

	/**
	 * Construct with system parameters
	 *
	 * @param[in] natural_freq The desired natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 */
	ਦੂਜਾ_ਕ੍ਰਮ_Reference_Model(ਅਚਲ ਦਸ਼ਮਲਵ natural_freq, ਅਚਲ ਦਸ਼ਮਲਵ damping_ਅਨੁਪਾਤ)
	{
		ਸੈੱਟ_Parameters(natural_freq, damping_ਅਨੁਪਾਤ);
	}

	/**
	 * Enumeration for available time discretization methods
	 */
	ਗਿਣਤੀ Discretization_ਵਿਧੀ {
		kForwardEuler,
		kBilinear
	};

	/**
	 * Set the time discretization method used for state integration
	 */
	ਖਾਲੀ ਸੈੱਟ_Discretization_ਵਿਧੀ(ਅਚਲ Discretization_ਵਿਧੀ ਵਿਧੀ) { discretization_ਵਿਧੀ = ਵਿਧੀ; }

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
	ਤਾਰਕਿਕ ਸੈੱਟ_Parameters(ਅਚਲ ਦਸ਼ਮਲਵ natural_freq, ਅਚਲ ਦਸ਼ਮਲਵ damping_ਅਨੁਪਾਤ)
	{
		ਜੇ (natural_freq < FLT_EPSILON || damping_ਅਨੁਪਾਤ < FLT_EPSILON) {

			// Deadzone the resulting constants (will result in zero filter acceleration)
			spring_constant_ = FLT_EPSILON;
			damping_coefficient_ = FLT_EPSILON;

			// Zero natural frequency means our time step is irrelevant
			cutoff_freq_ = FLT_EPSILON;
			max_ਸਮਾਂ_ਕਦਮ = INFINITY;

			// Fail
			ਵਾਪਸ ਝੂਠ;
		}

		// Note these are "effective" coefficients, as mass coefficient is considered baked in
		spring_constant_ = natural_freq * natural_freq;
		damping_coefficient_ = 2.0f * damping_ਅਨੁਪਾਤ * natural_freq;

		cutoff_freq_ = calculateCutoffFrequency(natural_freq, damping_ਅਨੁਪਾਤ);

		// Based on *conservative nyquist frequency via kSampleRateMultiplier >= 2
		max_ਸਮਾਂ_ਕਦਮ = 2.0f * M_PI_F / (cutoff_freq_ * k_Sample_ਦਰ_Multiplier);

		ਵਾਪਸ ਸੱਚ;
	}

	/**
	 * @return System state [units]
	 */
	ਅਚਲ T &getState() ਅਚਲ { ਵਾਪਸ filter_state_; }

	/**
	 * @return System rate [units/s]
	 */
	ਅਚਲ T &get_ਦਰ() ਅਚਲ { ਵਾਪਸ filter_ਦਰ; }

	/**
	 * @return System acceleration [units/s^2]
	 */
	ਅਚਲ T &getAccel() ਅਚਲ { ਵਾਪਸ filter_accel_; }

	/**
	 * Update the system states
	 *
	 * Units of rate_sample must correspond to the derivative of state_sample units.
	 *
	 * @param[in] time_step Time since last sample [s]
	 * @param[in] state_sample New state sample [units]
	 * @param[in] rate_sample New rate sample, if provided, otherwise defaults to zero(s) [units/s]
	 */
	ਖਾਲੀ update(ਅਚਲ ਦਸ਼ਮਲਵ ਸਮਾਂ_ਕਦਮ, ਅਚਲ T &state_sample, ਅਚਲ T &ਦਰ_sample = T())
	{
		ਜੇ ((ਸਮਾਂ_ਕਦਮ > max_ਸਮਾਂ_ਕਦਮ) || (ਸਮਾਂ_ਕਦਮ < 0.0f)) {
			// time step is too large or is negative, reset the filter
			reset(state_sample, ਦਰ_sample);
			ਵਾਪਸ;
		}

		// take a step forward from the last state (and input), update the filter states
		integrate_ਹਾਲਤਾਂ(ਸਮਾਂ_ਕਦਮ, ਆਖਰੀ_state_sample, ਆਖਰੀ_ਦਰ_sample);

		// instantaneous acceleration from current input / state
		filter_accel_ = calculateInstantaneousAcceleration(state_sample, ਦਰ_sample);

		// store the current samples
		ਆਖਰੀ_state_sample = state_sample;
		ਆਖਰੀ_ਦਰ_sample = ਦਰ_sample;
	}

	/**
	 * Reset the system states
	 *
	 * @param[in] state Initial state [units]
	 * @param[in] rate Initial rate, if provided, otherwise defaults to zero(s) [units/s]
	 */
	ਖਾਲੀ reset(ਅਚਲ T &state, ਅਚਲ T &ਦਰ = T())
	{
		filter_state_ = state;
		filter_ਦਰ = ਦਰ;
		filter_accel_ = T();

		ਆਖਰੀ_state_sample = state;
		ਆਖਰੀ_ਦਰ_sample = ਦਰ;
	}

ਨਿਜੀ:

	// A conservative multiplier (>=2) on sample frequency to bound the maximum time step
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ k_Sample_ਦਰ_Multiplier = 4.0f;

	// (effective, no mass) Spring constant for second order system [s^-2]
	ਦਸ਼ਮਲਵ spring_constant_{FLT_EPSILON};

	// (effective, no mass) Damping coefficient for second order system [s^-1]
	ਦਸ਼ਮਲਵ damping_coefficient_{FLT_EPSILON};

	// cutoff frequency [rad/s]
	ਦਸ਼ਮਲਵ cutoff_freq_{FLT_EPSILON};

	T filter_state_{}; // [units]
	T filter_ਦਰ{}; // [units/s]
	T filter_accel_{}; // [units/s^2]

	// the last samples need to be stored because we don't know the time step over which we integrate to update to the
	// next step a priori
	T ਆਖਰੀ_state_sample{}; // [units]
	T ਆਖਰੀ_ਦਰ_sample{}; // [units/s]

	// Maximum time step [s]
	ਦਸ਼ਮਲਵ max_ਸਮਾਂ_ਕਦਮ{INFINITY};

	// The selected time discretization method used for state integration
	Discretization_ਵਿਧੀ discretization_ਵਿਧੀ{kBilinear};

	/**
	 * Calculate the cutoff frequency in terms of undamped natural frequency and damping ratio
	 *
	 * @param[in] natural_freq The desired undamped natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 * @return Cutoff frequency [rad/s]
	 */
	ਦਸ਼ਮਲਵ calculateCutoffFrequency(ਅਚਲ ਦਸ਼ਮਲਵ natural_freq, ਅਚਲ ਦਸ਼ਮਲਵ damping_ਅਨੁਪਾਤ)
	{
		ਅਚਲ ਦਸ਼ਮਲਵ damping_ਅਨੁਪਾਤ_squared = damping_ਅਨੁਪਾਤ * damping_ਅਨੁਪਾਤ;
		ਵਾਪਸ natural_freq * sqrtf(1.0f - 2.0f * damping_ਅਨੁਪਾਤ_squared + sqrtf(4.0f * damping_ਅਨੁਪਾਤ_squared *
					    damping_ਅਨੁਪਾਤ_squared - 4.0f * damping_ਅਨੁਪਾਤ_squared + 2.0f));
	}

	/**
	 * Take one integration step using selected discretization method
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	ਖਾਲੀ integrate_ਹਾਲਤਾਂ(ਅਚਲ ਦਸ਼ਮਲਵ ਸਮਾਂ_ਕਦਮ, ਅਚਲ T &state_sample, ਅਚਲ T &ਦਰ_sample)
	{
		ਚੋਣ (discretization_ਵਿਧੀ) {
		ਹਾਲਤ Discretization_ਵਿਧੀ::kForwardEuler: {
				// forward-Euler discretization
				integrate_ਹਾਲਤਾਂ_Forward_Euler(ਸਮਾਂ_ਕਦਮ, state_sample, ਦਰ_sample);
				ਰੋਕੋ;
			}

		ਮੂਲ: {
				// default to bilinear transform
				integrate_ਹਾਲਤਾਂ_Bilinear(ਸਮਾਂ_ਕਦਮ, state_sample, ਦਰ_sample);
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
	ਖਾਲੀ integrate_ਹਾਲਤਾਂ_Forward_Euler(ਅਚਲ ਦਸ਼ਮਲਵ ਸਮਾਂ_ਕਦਮ, ਅਚਲ T &state_sample, ਅਚਲ T &ਦਰ_sample)
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
		ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> state_ਮੈਟ੍ਰਿਕਸ;
		state_ਮੈਟ੍ਰਿਕਸ(0, 0) = 1.0f;
		state_ਮੈਟ੍ਰਿਕਸ(0, 1) = ਸਮਾਂ_ਕਦਮ;
		state_ਮੈਟ੍ਰਿਕਸ(1, 0) = -spring_constant_ * ਸਮਾਂ_ਕਦਮ;
		state_ਮੈਟ੍ਰਿਕਸ(1, 1) = -damping_coefficient_ * ਸਮਾਂ_ਕਦਮ + 1.0f;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Bc * T
		ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ;
		ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(0, 0) = 0.0f;
		ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(0, 1) = 0.0f;
		ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(1, 0) = spring_constant_ * ਸਮਾਂ_ਕਦਮ;
		ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(1, 1) = damping_coefficient_ * ਸਮਾਂ_ਕਦਮ;

		// discrete state transition
		transition_ਹਾਲਤਾਂ(state_ਮੈਟ੍ਰਿਕਸ, ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ, state_sample, ਦਰ_sample);
	}

	/**
	 * Take one integration step using discrete state space calculated from bilinear transform
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	ਖਾਲੀ integrate_ਹਾਲਤਾਂ_Bilinear(ਅਚਲ ਦਸ਼ਮਲਵ ਸਮਾਂ_ਕਦਮ, ਅਚਲ T &state_sample, ਅਚਲ T &ਦਰ_sample)
	{
		ਅਚਲ ਦਸ਼ਮਲਵ ਸਮਾਂ_ਕਦਮ_squared = ਸਮਾਂ_ਕਦਮ * ਸਮਾਂ_ਕਦਮ;
		ਅਚਲ ਦਸ਼ਮਲਵ inv_denominator = 1.0f / (0.25f * spring_constant_ * ਸਮਾਂ_ਕਦਮ_squared + 0.5f * damping_coefficient_ *
						      ਸਮਾਂ_ਕਦਮ + 1.0f);

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
		ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> state_ਮੈਟ੍ਰਿਕਸ;
		state_ਮੈਟ੍ਰਿਕਸ(0, 0) = -0.25f * spring_constant_ * ਸਮਾਂ_ਕਦਮ_squared + 0.5f * damping_coefficient_ * ਸਮਾਂ_ਕਦਮ + 1.0f;
		state_ਮੈਟ੍ਰਿਕਸ(0, 1) = ਸਮਾਂ_ਕਦਮ;
		state_ਮੈਟ੍ਰਿਕਸ(1, 0) = -spring_constant_ * ਸਮਾਂ_ਕਦਮ;
		state_ਮੈਟ੍ਰਿਕਸ(1, 1) = -0.25f * spring_constant_ * ਸਮਾਂ_ਕਦਮ_squared - 0.5f * damping_coefficient_ * ਸਮਾਂ_ਕਦਮ + 1.0f;
		state_ਮੈਟ੍ਰਿਕਸ *= inv_denominator;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Ac^-1 * (Ad - I) * Bc
		ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ;
		ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(0, 0) = 0.5f * spring_constant_ * ਸਮਾਂ_ਕਦਮ_squared;
		ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(0, 1) = 0.5f * damping_coefficient_ * ਸਮਾਂ_ਕਦਮ_squared;
		ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(1, 0) = spring_constant_ * ਸਮਾਂ_ਕਦਮ;
		ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(1, 1) = damping_coefficient_ * ਸਮਾਂ_ਕਦਮ;
		ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ *= inv_denominator;

		// discrete state transition
		transition_ਹਾਲਤਾਂ(state_ਮੈਟ੍ਰਿਕਸ, ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ, state_sample, ਦਰ_sample);
	}

	/**
	 * Transition the states using discrete state space matrices
	 *
	 * @param[in] state_matrix Discrete state matrix (2x2)
	 * @param[in] input_matrix Discrete input matrix (2x2)
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	ਖਾਲੀ transition_ਹਾਲਤਾਂ(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> &state_ਮੈਟ੍ਰਿਕਸ,
			      ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> &ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ, ਅਚਲ T &state_sample, ਅਚਲ T &ਦਰ_sample)
	{
		ਅਚਲ T new_state = state_ਮੈਟ੍ਰਿਕਸ(0, 0) * filter_state_ + state_ਮੈਟ੍ਰਿਕਸ(0, 1) * filter_ਦਰ + ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(0,
				    0) * state_sample + ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(0, 1) * ਦਰ_sample;
		ਅਚਲ T new_ਦਰ = state_ਮੈਟ੍ਰਿਕਸ(1, 0) * filter_state_ + state_ਮੈਟ੍ਰਿਕਸ(1, 1) * filter_ਦਰ + ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(1,
				   0) * state_sample + ਨਿਵੇਸ਼_ਮੈਟ੍ਰਿਕਸ(1, 1) * ਦਰ_sample;
		filter_state_ = new_state;
		filter_ਦਰ = new_ਦਰ;
	}

	/**
	 * Calculate the instantaneous acceleration of the system
	 *
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	T calculateInstantaneousAcceleration(ਅਚਲ T &state_sample, ਅਚਲ T &ਦਰ_sample) ਅਚਲ
	{
		ਅਚਲ T state_error = state_sample - filter_state_;
		ਅਚਲ T ਦਰ_error = ਦਰ_sample - filter_ਦਰ;
		ਵਾਪਸ state_error * spring_constant_ + ਦਰ_error * damping_coefficient_;
	}
};

} // namespace math
