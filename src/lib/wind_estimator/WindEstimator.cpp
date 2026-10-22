/****************************************************************************
 *
 *   Copyright (c) 2018-2023 PX4 Development Team. All rights reserved.
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
 * @file WindEstimator.cpp
 * A wind and airspeed scale estimator.
 */

#ಅನ್ತರ್ಭಾವ "WindEstimator.hpp"
#ಅನ್ತರ್ಭಾವ "python/generated/init_wind_using_airspeed.h"

ತಾರ್ಕಿಕ
WindEstimator::initialise(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &velI, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ hor_vel_variance, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_rad,
			  ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ tas_meas, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ tas_variance)
{
	ಆದರೆ (PX4_ISFINITE(tas_meas) && PX4_ISFINITE(tas_variance)) {

		constexpr ದಶಮಾಂಶ initial_heading_var = sq(math::radians(kInitialHeadingErrorDeg));
		constexpr ದಶಮಾಂಶ initial_sideslip_var = sq(math::radians(kInitialBetaErrorDeg));

		ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> P_wind_init;
		ಮಾತೃಕೆ::Vector2f wind_init;

		sym::InitWindUsingAirspeed(velI, heading_rad, tas_meas, hor_vel_variance, initial_heading_var, initial_sideslip_var,
					   tas_variance,
					   &wind_init, &P_wind_init);

		_state.xy() = wind_init;
		_state(ಸೂಚ್ಯಂಕ_TAS_SCALE) = _scale_init;
		_P.ತುಂಡು<2, 2>(0, 0) = P_wind_init;

	} ಇಲ್ಲದಿದ್ದರೆ {
		// no airspeed available
		_state.ಗಣ_Zero();
		_state(ಸೂಚ್ಯಂಕ_TAS_SCALE) = 1.0f;
		_P.ಗಣ_Zero();
		_P(ಸೂಚ್ಯಂಕ_W_N, ಸೂಚ್ಯಂಕ_W_N) = _P(ಸೂಚ್ಯಂಕ_W_E, ಸೂಚ್ಯಂಕ_W_E) = sq(kInitialWindError);
	}

	_wind_estimator_reset = ನಿಜ;
	ಸಮಯ_initialised = hrt_absolute_ಸಮಯ();

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ
WindEstimator::update(uint64_t ಸಮಯ_now)
{
	ಆದರೆ (!_initialised) {
		ಹಿಂತಿರುಗಿ;
	}

	// set reset state to false (is set to true when initialise fuction is called later)
	_wind_estimator_reset = ಸುಳ್ಳು;

	// run covariance prediction at 1Hz
	ಆದರೆ (ಸಮಯ_now - ಸಮಯ_ಕೊನೆಯ_update < 1_s || ಸಮಯ_ಕೊನೆಯ_update == 0) {
		ಆದರೆ (ಸಮಯ_ಕೊನೆಯ_update == 0) {
			ಸಮಯ_ಕೊನೆಯ_update = ಸಮಯ_now;
		}

		ಹಿಂತಿರುಗಿ;
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt = (ದಶಮಾಂಶ)(ಸಮಯ_now - ಸಮಯ_ಕೊನೆಯ_update) * 1e-6f;
	ಸಮಯ_ಕೊನೆಯ_update = ಸಮಯ_now;

	// if airspeed scale is at default (1.0) and we are in the first 5 minutes of flight time, multiply kTasScalePsd by 100 for faster learning
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ tas_psd_multiplier = (fabsf(_scale_init - 1.0f) < FLT_EPSILON && (ಸಮಯ_now - ಸಮಯ_initialised < k_TASScale_Fast_Learn_ಸಮಯ)) ?
					 kTASScalePSDMultiplier : 1.f;

	ಮಾತೃಕೆ::Matrix3f Qk;
	Qk(ಸೂಚ್ಯಂಕ_W_N, ಸೂಚ್ಯಂಕ_W_N) = _wind_psd * dt;
	Qk(ಸೂಚ್ಯಂಕ_W_E, ಸೂಚ್ಯಂಕ_W_E) = Qk(ಸೂಚ್ಯಂಕ_W_N, ಸೂಚ್ಯಂಕ_W_N);
	Qk(ಸೂಚ್ಯಂಕ_TAS_SCALE, ಸೂಚ್ಯಂಕ_TAS_SCALE) = kTasScalePsd * tas_psd_multiplier * dt;
	_P += Qk;
}

ಶೂನ್ಯ
WindEstimator::fuse_airspeed(uint64_t ಸಮಯ_now, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ true_airspeed, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &velI,
			     ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ hor_vel_variance, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Quatf &q_att)
{

	ಆದರೆ (!_initialised) {
		// try to initialise
		_initialised = initialise(velI, hor_vel_variance, ಮಾತೃಕೆ::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		ಹಿಂತಿರುಗಿ;
	}

	// don't fuse faster than 10Hz
	ಆದರೆ (ಸಮಯ_now - ಸಮಯ_ಕೊನೆಯ_airspeed_fuse < 100_ms) {
		ಹಿಂತಿರುಗಿ;
	}

	ಸಮಯ_ಕೊನೆಯ_airspeed_fuse = ಸಮಯ_now;

	ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 1, 3> H_tas;
	ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 1> K;

	sym::FuseAirspeed(velI, _state, _P, true_airspeed, _tas_var, FLT_EPSILON,
			  &H_tas, &K, &_tas_innov_var, &_tas_innov);

	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ meas_is_rejected = check_if_meas_is_rejected(_tas_innov, _tas_innov_var, kTasGate);

	ಆದರೆ (_tas_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, ಮಾತೃಕೆ::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		ಹಿಂತಿರುಗಿ;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (meas_is_rejected) {
		ಹಿಂತಿರುಗಿ;
	}

	// apply correction to state
	_state(ಸೂಚ್ಯಂಕ_W_N) += _tas_innov * K(ಸೂಚ್ಯಂಕ_W_N, 0);
	_state(ಸೂಚ್ಯಂಕ_W_E) += _tas_innov * K(ಸೂಚ್ಯಂಕ_W_E, 0);
	_state(ಸೂಚ್ಯಂಕ_TAS_SCALE) += _tas_innov * K(ಸೂಚ್ಯಂಕ_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_tas * _P;

	ಸತತ_sanity_checks();
}

ಶೂನ್ಯ
WindEstimator::fuse_beta(uint64_t ಸಮಯ_now, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &velI, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ hor_vel_variance,
			 ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Quatf &q_att)
{
	ಆದರೆ (!_initialised) {
		_initialised = initialise(velI, hor_vel_variance, ಮಾತೃಕೆ::Eulerf(q_att).psi());
		ಹಿಂತಿರುಗಿ;
	}

	// don't fuse faster than 10Hz
	ಆದರೆ (ಸಮಯ_now - ಸಮಯ_ಕೊನೆಯ_beta_fuse < 100_ms) {
		ಹಿಂತಿರುಗಿ;
	}

	ಸಮಯ_ಕೊನೆಯ_beta_fuse = ಸಮಯ_now;


	ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 1, 3> H_beta;
	ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 1> K;

	sym::FuseBeta(velI, _state, _P, q_att, kBetaVar, FLT_EPSILON,
		      &H_beta, &K, &_beta_innov_var, &_beta_innov);

	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ meas_is_rejected = check_if_meas_is_rejected(_beta_innov, _beta_innov_var, kBetaGate);

	ಆದರೆ (_beta_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, ಮಾತೃಕೆ::Eulerf(q_att).psi());
		ಹಿಂತಿರುಗಿ;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (meas_is_rejected) {
		ಹಿಂತಿರುಗಿ;
	}

	// apply correction to state
	_state(ಸೂಚ್ಯಂಕ_W_N) += _beta_innov * K(ಸೂಚ್ಯಂಕ_W_N, 0);
	_state(ಸೂಚ್ಯಂಕ_W_E) += _beta_innov * K(ಸೂಚ್ಯಂಕ_W_E, 0);
	_state(ಸೂಚ್ಯಂಕ_TAS_SCALE) += _beta_innov * K(ಸೂಚ್ಯಂಕ_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_beta * _P;

	ಸತತ_sanity_checks();
}

ಶೂನ್ಯ
WindEstimator::ಸತತ_sanity_checks()
{
	ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i < 3; i++) {
		ಆದರೆ (_P(i, i) < 0.0f) {
			// ill-conditioned covariance matrix, reset filter
			_initialised = ಸುಳ್ಳು;
			ಹಿಂತಿರುಗಿ;
		}

		// limit covariance diagonals if they grow too large
		ಆದರೆ (i < 2) {
			_P(i, i) = _P(i, i) > 25.0f ? 25.0f : _P(i, i);

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 2) {
			_P(i, i) = _P(i, i) > 0.1f ? 0.1f : _P(i, i);
		}
	}

	ಆದರೆ (!_state.isAllFinite()) {
		_initialised = ಸುಳ್ಳು;
		ಹಿಂತಿರುಗಿ;
	}

	// attain symmetry
	ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಡ್ಡಸಾಲು = 0; ಅಡ್ಡಸಾಲು < 3; ಅಡ್ಡಸಾಲು++) {
		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಸ್ತಂಭ = 0; ಸ್ತಂಭ < ಅಡ್ಡಸಾಲು; ಸ್ತಂಭ++) {
			ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ tmp = (_P(ಅಡ್ಡಸಾಲು, ಸ್ತಂಭ) + _P(ಸ್ತಂಭ, ಅಡ್ಡಸಾಲು)) * 0.5f;
			_P(ಅಡ್ಡಸಾಲು, ಸ್ತಂಭ) = tmp;
			_P(ಸ್ತಂಭ, ಅಡ್ಡಸಾಲು) = tmp;
		}
	}
}

ತಾರ್ಕಿಕ
WindEstimator::check_if_meas_is_rejected(ದಶಮಾಂಶ innov, ದಶಮಾಂಶ innov_var, uint8_t gate_ಗಾತ್ರ)
{
	ಹಿಂತಿರುಗಿ (innov * innov > gate_ಗಾತ್ರ * gate_ಗಾತ್ರ * innov_var);
}
