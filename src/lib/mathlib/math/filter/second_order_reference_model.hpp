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

#رهنمود once

#ضمن <px4_platform_common/defines.h>
#ضمن <matrix/SquareMatrix.hpp>

نطاق math
{

قالب<نام_نوع T>
صنف دومی_ترتیب_Reference_Model
{
عام:
	دومی_ترتیب_Reference_Model() = افتراضي;

	/**
	 * Construct with system parameters
	 *
	 * @param[in] natural_freq The desired natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 */
	دومی_ترتیب_Reference_Model(ثابت عائم natural_freq, ثابت عائم damping_نسبت)
	{
		مجموعه_Parameters(natural_freq, damping_نسبت);
	}

	/**
	 * Enumeration for available time discretization methods
	 */
	تعداد Discretization_روش {
		kForwardEuler,
		kBilinear
	};

	/**
	 * Set the time discretization method used for state integration
	 */
	فراغ مجموعه_Discretization_روش(ثابت Discretization_روش روش) { discretization_روش = روش; }

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
	منطقي مجموعه_Parameters(ثابت عائم natural_freq, ثابت عائم damping_نسبت)
	{
		إذا (natural_freq < FLT_EPSILON || damping_نسبت < FLT_EPSILON) {

			// Deadzone the resulting constants (will result in zero filter acceleration)
			spring_constant_ = FLT_EPSILON;
			damping_coefficient_ = FLT_EPSILON;

			// Zero natural frequency means our time step is irrelevant
			cutoff_freq_ = FLT_EPSILON;
			max_زمان_گام = INFINITY;

			// Fail
			أرجع خطأ;
		}

		// Note these are "effective" coefficients, as mass coefficient is considered baked in
		spring_constant_ = natural_freq * natural_freq;
		damping_coefficient_ = 2.0f * damping_نسبت * natural_freq;

		cutoff_freq_ = calculateCutoffFrequency(natural_freq, damping_نسبت);

		// Based on *conservative nyquist frequency via kSampleRateMultiplier >= 2
		max_زمان_گام = 2.0f * M_PI_F / (cutoff_freq_ * k_Sample_نرخ_Multiplier);

		أرجع درست;
	}

	/**
	 * @return System state [units]
	 */
	ثابت T &getState() ثابت { أرجع filter_state_; }

	/**
	 * @return System rate [units/s]
	 */
	ثابت T &get_نرخ() ثابت { أرجع filter_نرخ; }

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
	فراغ update(ثابت عائم زمان_گام, ثابت T &state_sample, ثابت T &نرخ_sample = T())
	{
		إذا ((زمان_گام > max_زمان_گام) || (زمان_گام < 0.0f)) {
			// time step is too large or is negative, reset the filter
			reset(state_sample, نرخ_sample);
			أرجع;
		}

		// take a step forward from the last state (and input), update the filter states
		integrate_حالتها(زمان_گام, آخرین_state_sample, آخرین_نرخ_sample);

		// instantaneous acceleration from current input / state
		filter_accel_ = calculateInstantaneousAcceleration(state_sample, نرخ_sample);

		// store the current samples
		آخرین_state_sample = state_sample;
		آخرین_نرخ_sample = نرخ_sample;
	}

	/**
	 * Reset the system states
	 *
	 * @param[in] state Initial state [units]
	 * @param[in] rate Initial rate, if provided, otherwise defaults to zero(s) [units/s]
	 */
	فراغ reset(ثابت T &state, ثابت T &نرخ = T())
	{
		filter_state_ = state;
		filter_نرخ = نرخ;
		filter_accel_ = T();

		آخرین_state_sample = state;
		آخرین_نرخ_sample = نرخ;
	}

خاص:

	// A conservative multiplier (>=2) on sample frequency to bound the maximum time step
	ساكن constexpr عائم k_Sample_نرخ_Multiplier = 4.0f;

	// (effective, no mass) Spring constant for second order system [s^-2]
	عائم spring_constant_{FLT_EPSILON};

	// (effective, no mass) Damping coefficient for second order system [s^-1]
	عائم damping_coefficient_{FLT_EPSILON};

	// cutoff frequency [rad/s]
	عائم cutoff_freq_{FLT_EPSILON};

	T filter_state_{}; // [units]
	T filter_نرخ{}; // [units/s]
	T filter_accel_{}; // [units/s^2]

	// the last samples need to be stored because we don't know the time step over which we integrate to update to the
	// next step a priori
	T آخرین_state_sample{}; // [units]
	T آخرین_نرخ_sample{}; // [units/s]

	// Maximum time step [s]
	عائم max_زمان_گام{INFINITY};

	// The selected time discretization method used for state integration
	Discretization_روش discretization_روش{kBilinear};

	/**
	 * Calculate the cutoff frequency in terms of undamped natural frequency and damping ratio
	 *
	 * @param[in] natural_freq The desired undamped natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 * @return Cutoff frequency [rad/s]
	 */
	عائم calculateCutoffFrequency(ثابت عائم natural_freq, ثابت عائم damping_نسبت)
	{
		ثابت عائم damping_نسبت_squared = damping_نسبت * damping_نسبت;
		أرجع natural_freq * sqrtf(1.0f - 2.0f * damping_نسبت_squared + sqrtf(4.0f * damping_نسبت_squared *
					    damping_نسبت_squared - 4.0f * damping_نسبت_squared + 2.0f));
	}

	/**
	 * Take one integration step using selected discretization method
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	فراغ integrate_حالتها(ثابت عائم زمان_گام, ثابت T &state_sample, ثابت T &نرخ_sample)
	{
		بدل (discretization_روش) {
		حالة Discretization_روش::kForwardEuler: {
				// forward-Euler discretization
				integrate_حالتها_Forward_Euler(زمان_گام, state_sample, نرخ_sample);
				اكسر;
			}

		افتراضي: {
				// default to bilinear transform
				integrate_حالتها_Bilinear(زمان_گام, state_sample, نرخ_sample);
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
	فراغ integrate_حالتها_Forward_Euler(ثابت عائم زمان_گام, ثابت T &state_sample, ثابت T &نرخ_sample)
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
		ماتریس::مربع_ماتریس<عائم, 2> state_ماتریس;
		state_ماتریس(0, 0) = 1.0f;
		state_ماتریس(0, 1) = زمان_گام;
		state_ماتریس(1, 0) = -spring_constant_ * زمان_گام;
		state_ماتریس(1, 1) = -damping_coefficient_ * زمان_گام + 1.0f;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Bc * T
		ماتریس::مربع_ماتریس<عائم, 2> درون_داد_ماتریس;
		درون_داد_ماتریس(0, 0) = 0.0f;
		درون_داد_ماتریس(0, 1) = 0.0f;
		درون_داد_ماتریس(1, 0) = spring_constant_ * زمان_گام;
		درون_داد_ماتریس(1, 1) = damping_coefficient_ * زمان_گام;

		// discrete state transition
		transition_حالتها(state_ماتریس, درون_داد_ماتریس, state_sample, نرخ_sample);
	}

	/**
	 * Take one integration step using discrete state space calculated from bilinear transform
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	فراغ integrate_حالتها_Bilinear(ثابت عائم زمان_گام, ثابت T &state_sample, ثابت T &نرخ_sample)
	{
		ثابت عائم زمان_گام_squared = زمان_گام * زمان_گام;
		ثابت عائم inv_denominator = 1.0f / (0.25f * spring_constant_ * زمان_گام_squared + 0.5f * damping_coefficient_ *
						      زمان_گام + 1.0f);

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
		ماتریس::مربع_ماتریس<عائم, 2> state_ماتریس;
		state_ماتریس(0, 0) = -0.25f * spring_constant_ * زمان_گام_squared + 0.5f * damping_coefficient_ * زمان_گام + 1.0f;
		state_ماتریس(0, 1) = زمان_گام;
		state_ماتریس(1, 0) = -spring_constant_ * زمان_گام;
		state_ماتریس(1, 1) = -0.25f * spring_constant_ * زمان_گام_squared - 0.5f * damping_coefficient_ * زمان_گام + 1.0f;
		state_ماتریس *= inv_denominator;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Ac^-1 * (Ad - I) * Bc
		ماتریس::مربع_ماتریس<عائم, 2> درون_داد_ماتریس;
		درون_داد_ماتریس(0, 0) = 0.5f * spring_constant_ * زمان_گام_squared;
		درون_داد_ماتریس(0, 1) = 0.5f * damping_coefficient_ * زمان_گام_squared;
		درون_داد_ماتریس(1, 0) = spring_constant_ * زمان_گام;
		درون_داد_ماتریس(1, 1) = damping_coefficient_ * زمان_گام;
		درون_داد_ماتریس *= inv_denominator;

		// discrete state transition
		transition_حالتها(state_ماتریس, درون_داد_ماتریس, state_sample, نرخ_sample);
	}

	/**
	 * Transition the states using discrete state space matrices
	 *
	 * @param[in] state_matrix Discrete state matrix (2x2)
	 * @param[in] input_matrix Discrete input matrix (2x2)
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	فراغ transition_حالتها(ثابت ماتریس::مربع_ماتریس<عائم, 2> &state_ماتریس,
			      ثابت ماتریس::مربع_ماتریس<عائم, 2> &درون_داد_ماتریس, ثابت T &state_sample, ثابت T &نرخ_sample)
	{
		ثابت T new_state = state_ماتریس(0, 0) * filter_state_ + state_ماتریس(0, 1) * filter_نرخ + درون_داد_ماتریس(0,
				    0) * state_sample + درون_داد_ماتریس(0, 1) * نرخ_sample;
		ثابت T new_نرخ = state_ماتریس(1, 0) * filter_state_ + state_ماتریس(1, 1) * filter_نرخ + درون_داد_ماتریس(1,
				   0) * state_sample + درون_داد_ماتریس(1, 1) * نرخ_sample;
		filter_state_ = new_state;
		filter_نرخ = new_نرخ;
	}

	/**
	 * Calculate the instantaneous acceleration of the system
	 *
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	T calculateInstantaneousAcceleration(ثابت T &state_sample, ثابت T &نرخ_sample) ثابت
	{
		ثابت T state_error = state_sample - filter_state_;
		ثابت T نرخ_error = نرخ_sample - filter_نرخ;
		أرجع state_error * spring_constant_ + نرخ_error * damping_coefficient_;
	}
};

} // namespace math
