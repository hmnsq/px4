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

#సూచన once

#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <matrix/SquareMatrix.hpp>

నామాకాశ math
{

మూస<రకం_పేరు T>
తరగతి రెండవ_క్రమం_Reference_Model
{
బహిరంగ:
	రెండవ_క్రమం_Reference_Model() = అప్రమేయ;

	/**
	 * Construct with system parameters
	 *
	 * @param[in] natural_freq The desired natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 */
	రెండవ_క్రమం_Reference_Model(స్థిరరాశి దశాంశం natural_freq, స్థిరరాశి దశాంశం damping_నిష్పత్తి)
	{
		సమితి_Parameters(natural_freq, damping_నిష్పత్తి);
	}

	/**
	 * Enumeration for available time discretization methods
	 */
	గణన Discretization_పద్ధతి {
		kForwardEuler,
		kBilinear
	};

	/**
	 * Set the time discretization method used for state integration
	 */
	శూన్య సమితి_Discretization_పద్ధతి(స్థిరరాశి Discretization_పద్ధతి పద్ధతి) { discretization_పద్ధతి = పద్ధతి; }

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
	తార్కిక సమితి_Parameters(స్థిరరాశి దశాంశం natural_freq, స్థిరరాశి దశాంశం damping_నిష్పత్తి)
	{
		ఒకవేళ (natural_freq < FLT_EPSILON || damping_నిష్పత్తి < FLT_EPSILON) {

			// Deadzone the resulting constants (will result in zero filter acceleration)
			spring_constant_ = FLT_EPSILON;
			damping_coefficient_ = FLT_EPSILON;

			// Zero natural frequency means our time step is irrelevant
			cutoff_freq_ = FLT_EPSILON;
			max_సమయం_అడుగు = INFINITY;

			// Fail
			తిరిగి అబద్ధం;
		}

		// Note these are "effective" coefficients, as mass coefficient is considered baked in
		spring_constant_ = natural_freq * natural_freq;
		damping_coefficient_ = 2.0f * damping_నిష్పత్తి * natural_freq;

		cutoff_freq_ = calculateCutoffFrequency(natural_freq, damping_నిష్పత్తి);

		// Based on *conservative nyquist frequency via kSampleRateMultiplier >= 2
		max_సమయం_అడుగు = 2.0f * M_PI_F / (cutoff_freq_ * k_Sample_రేటు_Multiplier);

		తిరిగి నిజం;
	}

	/**
	 * @return System state [units]
	 */
	స్థిరరాశి T &getState() స్థిరరాశి { తిరిగి filter_state_; }

	/**
	 * @return System rate [units/s]
	 */
	స్థిరరాశి T &get_రేటు() స్థిరరాశి { తిరిగి filter_రేటు; }

	/**
	 * @return System acceleration [units/s^2]
	 */
	స్థిరరాశి T &getAccel() స్థిరరాశి { తిరిగి filter_accel_; }

	/**
	 * Update the system states
	 *
	 * Units of rate_sample must correspond to the derivative of state_sample units.
	 *
	 * @param[in] time_step Time since last sample [s]
	 * @param[in] state_sample New state sample [units]
	 * @param[in] rate_sample New rate sample, if provided, otherwise defaults to zero(s) [units/s]
	 */
	శూన్య update(స్థిరరాశి దశాంశం సమయం_అడుగు, స్థిరరాశి T &state_sample, స్థిరరాశి T &రేటు_sample = T())
	{
		ఒకవేళ ((సమయం_అడుగు > max_సమయం_అడుగు) || (సమయం_అడుగు < 0.0f)) {
			// time step is too large or is negative, reset the filter
			reset(state_sample, రేటు_sample);
			తిరిగి;
		}

		// take a step forward from the last state (and input), update the filter states
		integrate_స్థితులు(సమయం_అడుగు, చివరి_state_sample, చివరి_రేటు_sample);

		// instantaneous acceleration from current input / state
		filter_accel_ = calculateInstantaneousAcceleration(state_sample, రేటు_sample);

		// store the current samples
		చివరి_state_sample = state_sample;
		చివరి_రేటు_sample = రేటు_sample;
	}

	/**
	 * Reset the system states
	 *
	 * @param[in] state Initial state [units]
	 * @param[in] rate Initial rate, if provided, otherwise defaults to zero(s) [units/s]
	 */
	శూన్య reset(స్థిరరాశి T &state, స్థిరరాశి T &రేటు = T())
	{
		filter_state_ = state;
		filter_రేటు = రేటు;
		filter_accel_ = T();

		చివరి_state_sample = state;
		చివరి_రేటు_sample = రేటు;
	}

అంతరంగ:

	// A conservative multiplier (>=2) on sample frequency to bound the maximum time step
	స్థిర constexpr దశాంశం k_Sample_రేటు_Multiplier = 4.0f;

	// (effective, no mass) Spring constant for second order system [s^-2]
	దశాంశం spring_constant_{FLT_EPSILON};

	// (effective, no mass) Damping coefficient for second order system [s^-1]
	దశాంశం damping_coefficient_{FLT_EPSILON};

	// cutoff frequency [rad/s]
	దశాంశం cutoff_freq_{FLT_EPSILON};

	T filter_state_{}; // [units]
	T filter_రేటు{}; // [units/s]
	T filter_accel_{}; // [units/s^2]

	// the last samples need to be stored because we don't know the time step over which we integrate to update to the
	// next step a priori
	T చివరి_state_sample{}; // [units]
	T చివరి_రేటు_sample{}; // [units/s]

	// Maximum time step [s]
	దశాంశం max_సమయం_అడుగు{INFINITY};

	// The selected time discretization method used for state integration
	Discretization_పద్ధతి discretization_పద్ధతి{kBilinear};

	/**
	 * Calculate the cutoff frequency in terms of undamped natural frequency and damping ratio
	 *
	 * @param[in] natural_freq The desired undamped natural frequency of the system [rad/s]
	 * @param[in] damping_ratio The desired damping ratio of the system
	 * @return Cutoff frequency [rad/s]
	 */
	దశాంశం calculateCutoffFrequency(స్థిరరాశి దశాంశం natural_freq, స్థిరరాశి దశాంశం damping_నిష్పత్తి)
	{
		స్థిరరాశి దశాంశం damping_నిష్పత్తి_squared = damping_నిష్పత్తి * damping_నిష్పత్తి;
		తిరిగి natural_freq * sqrtf(1.0f - 2.0f * damping_నిష్పత్తి_squared + sqrtf(4.0f * damping_నిష్పత్తి_squared *
					    damping_నిష్పత్తి_squared - 4.0f * damping_నిష్పత్తి_squared + 2.0f));
	}

	/**
	 * Take one integration step using selected discretization method
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	శూన్య integrate_స్థితులు(స్థిరరాశి దశాంశం సమయం_అడుగు, స్థిరరాశి T &state_sample, స్థిరరాశి T &రేటు_sample)
	{
		ఎంపిక (discretization_పద్ధతి) {
		స్థితి Discretization_పద్ధతి::kForwardEuler: {
				// forward-Euler discretization
				integrate_స్థితులు_Forward_Euler(సమయం_అడుగు, state_sample, రేటు_sample);
				ఆపు;
			}

		అప్రమేయ: {
				// default to bilinear transform
				integrate_స్థితులు_Bilinear(సమయం_అడుగు, state_sample, రేటు_sample);
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
	శూన్య integrate_స్థితులు_Forward_Euler(స్థిరరాశి దశాంశం సమయం_అడుగు, స్థిరరాశి T &state_sample, స్థిరరాశి T &రేటు_sample)
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
		మాతృక::వర్గం_మాతృక<దశాంశం, 2> state_మాతృక;
		state_మాతృక(0, 0) = 1.0f;
		state_మాతృక(0, 1) = సమయం_అడుగు;
		state_మాతృక(1, 0) = -spring_constant_ * సమయం_అడుగు;
		state_మాతృక(1, 1) = -damping_coefficient_ * సమయం_అడుగు + 1.0f;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Bc * T
		మాతృక::వర్గం_మాతృక<దశాంశం, 2> ఆదానం_మాతృక;
		ఆదానం_మాతృక(0, 0) = 0.0f;
		ఆదానం_మాతృక(0, 1) = 0.0f;
		ఆదానం_మాతృక(1, 0) = spring_constant_ * సమయం_అడుగు;
		ఆదానం_మాతృక(1, 1) = damping_coefficient_ * సమయం_అడుగు;

		// discrete state transition
		transition_స్థితులు(state_మాతృక, ఆదానం_మాతృక, state_sample, రేటు_sample);
	}

	/**
	 * Take one integration step using discrete state space calculated from bilinear transform
	 *
	 * @param[in] time_step Integration time [s]
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	శూన్య integrate_స్థితులు_Bilinear(స్థిరరాశి దశాంశం సమయం_అడుగు, స్థిరరాశి T &state_sample, స్థిరరాశి T &రేటు_sample)
	{
		స్థిరరాశి దశాంశం సమయం_అడుగు_squared = సమయం_అడుగు * సమయం_అడుగు;
		స్థిరరాశి దశాంశం inv_denominator = 1.0f / (0.25f * spring_constant_ * సమయం_అడుగు_squared + 0.5f * damping_coefficient_ *
						      సమయం_అడుగు + 1.0f);

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
		మాతృక::వర్గం_మాతృక<దశాంశం, 2> state_మాతృక;
		state_మాతృక(0, 0) = -0.25f * spring_constant_ * సమయం_అడుగు_squared + 0.5f * damping_coefficient_ * సమయం_అడుగు + 1.0f;
		state_మాతృక(0, 1) = సమయం_అడుగు;
		state_మాతృక(1, 0) = -spring_constant_ * సమయం_అడుగు;
		state_మాతృక(1, 1) = -0.25f * spring_constant_ * సమయం_అడుగు_squared - 0.5f * damping_coefficient_ * సమయం_అడుగు + 1.0f;
		state_మాతృక *= inv_denominator;

		// input matrix
		// Bc = [0  0 ]
		//      [Kx Kv]
		// Bd = Ac^-1 * (Ad - I) * Bc
		మాతృక::వర్గం_మాతృక<దశాంశం, 2> ఆదానం_మాతృక;
		ఆదానం_మాతృక(0, 0) = 0.5f * spring_constant_ * సమయం_అడుగు_squared;
		ఆదానం_మాతృక(0, 1) = 0.5f * damping_coefficient_ * సమయం_అడుగు_squared;
		ఆదానం_మాతృక(1, 0) = spring_constant_ * సమయం_అడుగు;
		ఆదానం_మాతృక(1, 1) = damping_coefficient_ * సమయం_అడుగు;
		ఆదానం_మాతృక *= inv_denominator;

		// discrete state transition
		transition_స్థితులు(state_మాతృక, ఆదానం_మాతృక, state_sample, రేటు_sample);
	}

	/**
	 * Transition the states using discrete state space matrices
	 *
	 * @param[in] state_matrix Discrete state matrix (2x2)
	 * @param[in] input_matrix Discrete input matrix (2x2)
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	శూన్య transition_స్థితులు(స్థిరరాశి మాతృక::వర్గం_మాతృక<దశాంశం, 2> &state_మాతృక,
			      స్థిరరాశి మాతృక::వర్గం_మాతృక<దశాంశం, 2> &ఆదానం_మాతృక, స్థిరరాశి T &state_sample, స్థిరరాశి T &రేటు_sample)
	{
		స్థిరరాశి T new_state = state_మాతృక(0, 0) * filter_state_ + state_మాతృక(0, 1) * filter_రేటు + ఆదానం_మాతృక(0,
				    0) * state_sample + ఆదానం_మాతృక(0, 1) * రేటు_sample;
		స్థిరరాశి T new_రేటు = state_మాతృక(1, 0) * filter_state_ + state_మాతృక(1, 1) * filter_రేటు + ఆదానం_మాతృక(1,
				   0) * state_sample + ఆదానం_మాతృక(1, 1) * రేటు_sample;
		filter_state_ = new_state;
		filter_రేటు = new_రేటు;
	}

	/**
	 * Calculate the instantaneous acceleration of the system
	 *
	 * @param[in] state_sample [units]
	 * @param[in] rate_sample [units/s]
	 */
	T calculateInstantaneousAcceleration(స్థిరరాశి T &state_sample, స్థిరరాశి T &రేటు_sample) స్థిరరాశి
	{
		స్థిరరాశి T state_error = state_sample - filter_state_;
		స్థిరరాశి T రేటు_error = రేటు_sample - filter_రేటు;
		తిరిగి state_error * spring_constant_ + రేటు_error * damping_coefficient_;
	}
};

} // namespace math
