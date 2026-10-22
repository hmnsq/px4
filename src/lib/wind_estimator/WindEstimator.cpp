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

#ਅਨ੍ਤਰ੍ਭਾਵ "WindEstimator.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "python/generated/init_wind_using_airspeed.h"

ਤਾਰਕਿਕ
WindEstimator::initialise(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &velI, ਅਚਲ ਦਸ਼ਮਲਵ hor_vel_variance, ਅਚਲ ਦਸ਼ਮਲਵ heading_rad,
			  ਅਚਲ ਦਸ਼ਮਲਵ tas_meas, ਅਚਲ ਦਸ਼ਮਲਵ tas_variance)
{
	ਜੇ (PX4_ISFINITE(tas_meas) && PX4_ISFINITE(tas_variance)) {

		constexpr ਦਸ਼ਮਲਵ initial_heading_var = sq(math::radians(kInitialHeadingErrorDeg));
		constexpr ਦਸ਼ਮਲਵ initial_sideslip_var = sq(math::radians(kInitialBetaErrorDeg));

		ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> P_wind_init;
		ਮੈਟ੍ਰਿਕਸ::Vector2f wind_init;

		sym::InitWindUsingAirspeed(velI, heading_rad, tas_meas, hor_vel_variance, initial_heading_var, initial_sideslip_var,
					   tas_variance,
					   &wind_init, &P_wind_init);

		_state.xy() = wind_init;
		_state(ਸੂਚਕਾਂਕ_TAS_SCALE) = _scale_init;
		_P.ਟੁਕੜਾ<2, 2>(0, 0) = P_wind_init;

	} ਨਹੀਂਤਾਂ {
		// no airspeed available
		_state.ਸੈੱਟ_Zero();
		_state(ਸੂਚਕਾਂਕ_TAS_SCALE) = 1.0f;
		_P.ਸੈੱਟ_Zero();
		_P(ਸੂਚਕਾਂਕ_W_N, ਸੂਚਕਾਂਕ_W_N) = _P(ਸੂਚਕਾਂਕ_W_E, ਸੂਚਕਾਂਕ_W_E) = sq(kInitialWindError);
	}

	_wind_estimator_reset = ਸੱਚ;
	ਸਮਾਂ_initialised = hrt_absolute_ਸਮਾਂ();

	ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ
WindEstimator::update(uint64_t ਸਮਾਂ_now)
{
	ਜੇ (!_initialised) {
		ਵਾਪਸ;
	}

	// set reset state to false (is set to true when initialise fuction is called later)
	_wind_estimator_reset = ਝੂਠ;

	// run covariance prediction at 1Hz
	ਜੇ (ਸਮਾਂ_now - ਸਮਾਂ_ਆਖਰੀ_update < 1_s || ਸਮਾਂ_ਆਖਰੀ_update == 0) {
		ਜੇ (ਸਮਾਂ_ਆਖਰੀ_update == 0) {
			ਸਮਾਂ_ਆਖਰੀ_update = ਸਮਾਂ_now;
		}

		ਵਾਪਸ;
	}

	ਅਚਲ ਦਸ਼ਮਲਵ dt = (ਦਸ਼ਮਲਵ)(ਸਮਾਂ_now - ਸਮਾਂ_ਆਖਰੀ_update) * 1e-6f;
	ਸਮਾਂ_ਆਖਰੀ_update = ਸਮਾਂ_now;

	// if airspeed scale is at default (1.0) and we are in the first 5 minutes of flight time, multiply kTasScalePsd by 100 for faster learning
	ਅਚਲ ਦਸ਼ਮਲਵ tas_psd_multiplier = (fabsf(_scale_init - 1.0f) < FLT_EPSILON && (ਸਮਾਂ_now - ਸਮਾਂ_initialised < k_TASScale_Fast_Learn_ਸਮਾਂ)) ?
					 kTASScalePSDMultiplier : 1.f;

	ਮੈਟ੍ਰਿਕਸ::Matrix3f Qk;
	Qk(ਸੂਚਕਾਂਕ_W_N, ਸੂਚਕਾਂਕ_W_N) = _wind_psd * dt;
	Qk(ਸੂਚਕਾਂਕ_W_E, ਸੂਚਕਾਂਕ_W_E) = Qk(ਸੂਚਕਾਂਕ_W_N, ਸੂਚਕਾਂਕ_W_N);
	Qk(ਸੂਚਕਾਂਕ_TAS_SCALE, ਸੂਚਕਾਂਕ_TAS_SCALE) = kTasScalePsd * tas_psd_multiplier * dt;
	_P += Qk;
}

ਖਾਲੀ
WindEstimator::fuse_airspeed(uint64_t ਸਮਾਂ_now, ਅਚਲ ਦਸ਼ਮਲਵ true_airspeed, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &velI,
			     ਅਚਲ ਦਸ਼ਮਲਵ hor_vel_variance, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Quatf &q_att)
{

	ਜੇ (!_initialised) {
		// try to initialise
		_initialised = initialise(velI, hor_vel_variance, ਮੈਟ੍ਰਿਕਸ::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		ਵਾਪਸ;
	}

	// don't fuse faster than 10Hz
	ਜੇ (ਸਮਾਂ_now - ਸਮਾਂ_ਆਖਰੀ_airspeed_fuse < 100_ms) {
		ਵਾਪਸ;
	}

	ਸਮਾਂ_ਆਖਰੀ_airspeed_fuse = ਸਮਾਂ_now;

	ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 1, 3> H_tas;
	ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 1> K;

	sym::FuseAirspeed(velI, _state, _P, true_airspeed, _tas_var, FLT_EPSILON,
			  &H_tas, &K, &_tas_innov_var, &_tas_innov);

	ਅਚਲ ਤਾਰਕਿਕ meas_is_rejected = check_if_meas_is_rejected(_tas_innov, _tas_innov_var, kTasGate);

	ਜੇ (_tas_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, ਮੈਟ੍ਰਿਕਸ::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		ਵਾਪਸ;

	} ਨਹੀਂਤਾਂ ਜੇ (meas_is_rejected) {
		ਵਾਪਸ;
	}

	// apply correction to state
	_state(ਸੂਚਕਾਂਕ_W_N) += _tas_innov * K(ਸੂਚਕਾਂਕ_W_N, 0);
	_state(ਸੂਚਕਾਂਕ_W_E) += _tas_innov * K(ਸੂਚਕਾਂਕ_W_E, 0);
	_state(ਸੂਚਕਾਂਕ_TAS_SCALE) += _tas_innov * K(ਸੂਚਕਾਂਕ_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_tas * _P;

	ਲੜੀਵਾਰ_sanity_checks();
}

ਖਾਲੀ
WindEstimator::fuse_beta(uint64_t ਸਮਾਂ_now, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &velI, ਅਚਲ ਦਸ਼ਮਲਵ hor_vel_variance,
			 ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Quatf &q_att)
{
	ਜੇ (!_initialised) {
		_initialised = initialise(velI, hor_vel_variance, ਮੈਟ੍ਰਿਕਸ::Eulerf(q_att).psi());
		ਵਾਪਸ;
	}

	// don't fuse faster than 10Hz
	ਜੇ (ਸਮਾਂ_now - ਸਮਾਂ_ਆਖਰੀ_beta_fuse < 100_ms) {
		ਵਾਪਸ;
	}

	ਸਮਾਂ_ਆਖਰੀ_beta_fuse = ਸਮਾਂ_now;


	ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 1, 3> H_beta;
	ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 1> K;

	sym::FuseBeta(velI, _state, _P, q_att, kBetaVar, FLT_EPSILON,
		      &H_beta, &K, &_beta_innov_var, &_beta_innov);

	ਅਚਲ ਤਾਰਕਿਕ meas_is_rejected = check_if_meas_is_rejected(_beta_innov, _beta_innov_var, kBetaGate);

	ਜੇ (_beta_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, ਮੈਟ੍ਰਿਕਸ::Eulerf(q_att).psi());
		ਵਾਪਸ;

	} ਨਹੀਂਤਾਂ ਜੇ (meas_is_rejected) {
		ਵਾਪਸ;
	}

	// apply correction to state
	_state(ਸੂਚਕਾਂਕ_W_N) += _beta_innov * K(ਸੂਚਕਾਂਕ_W_N, 0);
	_state(ਸੂਚਕਾਂਕ_W_E) += _beta_innov * K(ਸੂਚਕਾਂਕ_W_E, 0);
	_state(ਸੂਚਕਾਂਕ_TAS_SCALE) += _beta_innov * K(ਸੂਚਕਾਂਕ_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_beta * _P;

	ਲੜੀਵਾਰ_sanity_checks();
}

ਖਾਲੀ
WindEstimator::ਲੜੀਵਾਰ_sanity_checks()
{
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < 3; i++) {
		ਜੇ (_P(i, i) < 0.0f) {
			// ill-conditioned covariance matrix, reset filter
			_initialised = ਝੂਠ;
			ਵਾਪਸ;
		}

		// limit covariance diagonals if they grow too large
		ਜੇ (i < 2) {
			_P(i, i) = _P(i, i) > 25.0f ? 25.0f : _P(i, i);

		} ਨਹੀਂਤਾਂ ਜੇ (i == 2) {
			_P(i, i) = _P(i, i) > 0.1f ? 0.1f : _P(i, i);
		}
	}

	ਜੇ (!_state.isAllFinite()) {
		_initialised = ਝੂਠ;
		ਵਾਪਸ;
	}

	// attain symmetry
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੰਕਤੀ = 0; ਪੰਕਤੀ < 3; ਪੰਕਤੀ++) {
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਕਾਲਮ = 0; ਕਾਲਮ < ਪੰਕਤੀ; ਕਾਲਮ++) {
			ਅਚਲ ਦਸ਼ਮਲਵ tmp = (_P(ਪੰਕਤੀ, ਕਾਲਮ) + _P(ਕਾਲਮ, ਪੰਕਤੀ)) * 0.5f;
			_P(ਪੰਕਤੀ, ਕਾਲਮ) = tmp;
			_P(ਕਾਲਮ, ਪੰਕਤੀ) = tmp;
		}
	}
}

ਤਾਰਕਿਕ
WindEstimator::check_if_meas_is_rejected(ਦਸ਼ਮਲਵ innov, ਦਸ਼ਮਲਵ innov_var, uint8_t gate_ਮਾਪ)
{
	ਵਾਪਸ (innov * innov > gate_ਮਾਪ * gate_ਮਾਪ * innov_var);
}
