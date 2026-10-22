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

#هدايت once

#ضمن <px4_platform_common/defines.h>
#ضمن <matrix/SquareMatrix.hpp>

نطاق math
{

سانچو<قسم_نالو T>
صنف ٻيو_ترتيب_Reference_Model
{
عام:
	ٻيو_ترتيب_Reference_Model() = افتراضي;

	/**
	 * Construct with system parameters
	 *
	 * @param[in] natural_freq The desired natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 */
	ٻيو_ترتيب_Reference_Model(ثابت عائم natural_freq, ثابت عائم damping_تناسب)
	{
		سيٽ_Parameters(natural_freq, damping_تناسب);
	}

	/**
	 * Enumeration for available time discretization methods
	 */
	تعداد Discretization_طريقو {
		kForwardEuler,
		kBilinear
	};

	/**
	 * Set the time discretization method used for state integration
	 */
	فراغ سيٽ_Discretization_طريقو(ثابت Discretization_طريقو طريقو) { discretization_طريقو = طريقو; }

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
	منطقي سيٽ_Parameters(ثابت عائم natural_freq, ثابت عائم damping_تناسب)
	{
		إذا (natural_freq < FLT_EPSILON || damping_تناسب < FLT_EPSILON) {

			// Deadzone the resulting constants (will result in zero filter acceleration)
			spring_constant_ = FLT_EPSILON;
			damping_coefficient_ = FLT_EPSILON;

			// Zero natural frequency means our time step is irrelevant
			cutoff_freq_ = FLT_EPSILON;
			max_وقت_قدم = INFINITY;

			// Fail
			أرجع خطأ;
		}

		// Note these are "effective" coefficients, as mass coefficient is considered baked in
		spring_constant_ = natural_freq * natural_freq;
		damping_coefficient_ = 2.0f * damping_تناسب * natural_freq;

		cutoff_freq_ = calculateCutoffFrequency(natural_freq, damping_تناسب);

		// Based on *conservative nyquist frequency via kSampleRateMultiplier >= 2
		max_وقت_قدم = 2.0f * M_PI_F / (cutoff_freq_ * k_Sample_شرح_Multiplier);

		أرجع سچ;
	}

	/**
	 * @return System state [units]
	 */
	ثابت T &getState() ثابت { أرجع filter_state_; }

	/**
	 * @return System rate [units/s]
	 */
	ثابت T &get_شرح() ثابت { أرجع filter_شرح; }

	/**
	 * @return System acceleration [units/s^2]
	 */
	ثابت T &getAccel() ثابت { أرجع filter_accel_; }

	/**
	 * Update the system states
	 *
	 * Units of rate_sample must correspond to the derivative of state_sample units.
	 *
	 * @param[in] time_step Time since last sample [s]
	 * @param[in] state_sample New state sample [units]
	 * @param[in] rate_sample New rate sample, if provided, otherwise defaults to zero(s) [units/s]
	 */
	فراغ update(ثابت عائم وقت_قدم, ثابت T &state_sample, ثابت T &شرح_sample = T())
	{
		إذا ((وقت_قدم > max_وقت_قدم) || (وقت_قدم < 0.0f)) {
			// time step is too large or is negative, reset the filter
			reset(state_sample, شرح_sample);
			أرجع;
		}

		// take a step forward from the last state (and input), update the filter states
		integrate_حالتون(وقت_قدم, آخري_state_sample, آخري_شرح_sample);

		// instantaneous acceleration from current input / state
		filter_accel_ = calculateInstantaneousAcceleration(state_sample, شرح_sample);

		// store the current samples
		آخري_state_sample = state_sample;
		آخري_شرح_sample = شرح_sample;
	}

	/**
	 * Reset the system states
	 *
	 * @param[in] state Initial state [units]
	 * @param[in] rate Initial rate, if provided, otherwise defaults to zero(s) [units/s]
	 */
	فراغ reset(ثابت T &state, ثابت T &شرح = T())
	{
		filter_state_ = state;
		filter_شرح = شرح;
		filter_accel_ = T();

		آخري_state_sample = state;
		آخري_شرح_sample = شرح;
	}

خاص:

	// A conservative multiplier (>=2) on sample frequency to bound the maximum time step
	ساكن constexpr عائم k_Sample_شرح_Multiplier = 4.0f;

	// (effective, no mass) Spring constant for second order system [s^-2]
	عائم spring_constant_{FLT_EPSILON};

	// (effective, no mass) Damping coefficient for second order system [s^-1]
	عائم damping_coefficient_{FLT_EPSILON};

	// cutoff frequency [rad/s]
	عائم cutoff_freq_{FLT_EPSILON};

	T filter_state_{}; // [units]
	T filter_شرح{}; // [units/s]
	T filter_accel_{}; // [units/s^2]

	// the last samples need to be stored because we don't know the time step over which we integrate to update to the
	// next step a priori
	T آخري_state_sample{}; // [units]
	T آخري_شرح_sample{}; // [units/s]

	// Maximum time step [s]
	عائم max_وقت_قدم{INFINITY};

	// The selected time discretization method used for state integration
	Discretization_طريقو discretization_طريقو{kBilinear};

	/**
	 * Calculate the cutoff frequency in terms of undamped natural frequency and damping ratio
	 *
	 * @param[in] natural_freq The desired undamped natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 * @return Cutoff frequency [rad/s]
	 */
	عائم calculateCutoffFrequency(ثابت عائم natural_freq, ثابت عائم damping_تناسب)
	{
		ثابت عائم damping_تناسب_squared = damping_تناسب * damping_تناسب;
		أرجع natural_freq * sqrtf(1.0f - 2.0f * damping_تناسب_squared + sqrtf(4.0f * damping_تناسب_squared *
					    damping_تناسب_squared - 4.0f * damping_تناسب_squared + 2.0f));
	}

	/**
	 * Take one integration step using selected discretization method
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	فراغ integrate_حالتون(ثابت عائم وقت_قدم, ثابت T &state_sample, ثابت T &شرح_sample)
	{
		بدل (discretization_طريقو) {
		حالة Discretization_طريقو::kForwardEuler: {
				// forward-Euler discretization
				integrate_حالتون_Forward_Euler(وقت_قدم, state_sample, شرح_sample);
				اكسر;
			}

		افتراضي: {
				// default to bilinear transform
				integrate_حالتون_Bilinear(وقت_قدم, state_sample, شرح_sample);
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
	فراغ integrate_حالتون_Forward_Euler(ثابت عائم وقت_قدم, ثابت T &state_sample, ثابت T &شرح_sample)
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
		ميٽرڪس::چورس_ميٽرڪس<عائم, 2> state_ميٽرڪس;
		state_ميٽرڪس(0, 0) = 1.0f;
		state_ميٽرڪس(0, 1) = وقت_قدم;
		state_ميٽرڪس(1, 0) = -spring_constant_ * وقت_قدم;
		state_ميٽرڪس(1, 1) = -damping_coefficient_ * وقت_قدم + 1.0f;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Bc * T
		ميٽرڪس::چورس_ميٽرڪس<عائم, 2> داخلا_ميٽرڪس;
		داخلا_ميٽرڪس(0, 0) = 0.0f;
		داخلا_ميٽرڪس(0, 1) = 0.0f;
		داخلا_ميٽرڪس(1, 0) = spring_constant_ * وقت_قدم;
		داخلا_ميٽرڪس(1, 1) = damping_coefficient_ * وقت_قدم;

		// discrete state transition
		transition_حالتون(state_ميٽرڪس, داخلا_ميٽرڪس, state_sample, شرح_sample);
	}

	/**
	 * Take one integration step using discrete state space calculated from bilinear transform
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	فراغ integrate_حالتون_Bilinear(ثابت عائم وقت_قدم, ثابت T &state_sample, ثابت T &شرح_sample)
	{
		ثابت عائم وقت_قدم_squared = وقت_قدم * وقت_قدم;
		ثابت عائم inv_denominator = 1.0f / (0.25f * spring_constant_ * وقت_قدم_squared + 0.5f * damping_coefficient_ *
						      وقت_قدم + 1.0f);

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
		ميٽرڪس::چورس_ميٽرڪس<عائم, 2> state_ميٽرڪس;
		state_ميٽرڪس(0, 0) = -0.25f * spring_constant_ * وقت_قدم_squared + 0.5f * damping_coefficient_ * وقت_قدم + 1.0f;
		state_ميٽرڪس(0, 1) = وقت_قدم;
		state_ميٽرڪس(1, 0) = -spring_constant_ * وقت_قدم;
		state_ميٽرڪس(1, 1) = -0.25f * spring_constant_ * وقت_قدم_squared - 0.5f * damping_coefficient_ * وقت_قدم + 1.0f;
		state_ميٽرڪس *= inv_denominator;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Ac^-1 * (Ad - I) * Bc
		ميٽرڪس::چورس_ميٽرڪس<عائم, 2> داخلا_ميٽرڪس;
		داخلا_ميٽرڪس(0, 0) = 0.5f * spring_constant_ * وقت_قدم_squared;
		داخلا_ميٽرڪس(0, 1) = 0.5f * damping_coefficient_ * وقت_قدم_squared;
		داخلا_ميٽرڪس(1, 0) = spring_constant_ * وقت_قدم;
		داخلا_ميٽرڪس(1, 1) = damping_coefficient_ * وقت_قدم;
		داخلا_ميٽرڪس *= inv_denominator;

		// discrete state transition
		transition_حالتون(state_ميٽرڪس, داخلا_ميٽرڪس, state_sample, شرح_sample);
	}

	/**
	 * Transition the states using discrete state space matrices
	 *
	 * @param[in] state_matrix Discrete state matrix (2x2)
	 * @param[in] input_matrix Discrete input matrix (2x2)
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	فراغ transition_حالتون(ثابت ميٽرڪس::چورس_ميٽرڪس<عائم, 2> &state_ميٽرڪس,
			      ثابت ميٽرڪس::چورس_ميٽرڪس<عائم, 2> &داخلا_ميٽرڪس, ثابت T &state_sample, ثابت T &شرح_sample)
	{
		ثابت T new_state = state_ميٽرڪس(0, 0) * filter_state_ + state_ميٽرڪس(0, 1) * filter_شرح + داخلا_ميٽرڪس(0,
				    0) * state_sample + داخلا_ميٽرڪس(0, 1) * شرح_sample;
		ثابت T new_شرح = state_ميٽرڪس(1, 0) * filter_state_ + state_ميٽرڪس(1, 1) * filter_شرح + داخلا_ميٽرڪس(1,
				   0) * state_sample + داخلا_ميٽرڪس(1, 1) * شرح_sample;
		filter_state_ = new_state;
		filter_شرح = new_شرح;
	}

	/**
	 * Calculate the instantaneous acceleration of the system
	 *
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	T calculateInstantaneousAcceleration(ثابت T &state_sample, ثابت T &شرح_sample) ثابت
	{
		ثابت T state_error = state_sample - filter_state_;
		ثابت T شرح_error = شرح_sample - filter_شرح;
		أرجع state_error * spring_constant_ + شرح_error * damping_coefficient_;
	}
};

} // namespace math
