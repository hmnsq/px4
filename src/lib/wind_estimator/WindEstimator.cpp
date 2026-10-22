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

#ضمن "WindEstimator.hpp"
#ضمن "python/generated/init_wind_using_airspeed.h"

منطقي
WindEstimator::initialise(ثابت میٹرکس::Vector3f &velI, ثابت عائم hor_vel_variance, ثابت عائم heading_rad,
			  ثابت عائم tas_meas, ثابت عائم tas_variance)
{
	إذا (PX4_ISFINITE(tas_meas) && PX4_ISFINITE(tas_variance)) {

		constexpr عائم initial_heading_var = sq(math::radians(kInitialHeadingErrorDeg));
		constexpr عائم initial_sideslip_var = sq(math::radians(kInitialBetaErrorDeg));

		میٹرکس::مربع_میٹرکس<عائم, 2> P_wind_init;
		میٹرکس::Vector2f wind_init;

		sym::InitWindUsingAirspeed(velI, heading_rad, tas_meas, hor_vel_variance, initial_heading_var, initial_sideslip_var,
					   tas_variance,
					   &wind_init, &P_wind_init);

		_state.xy() = wind_init;
		_state(اشاریہ_TAS_SCALE) = _scale_init;
		_P.ٹکڑا<2, 2>(0, 0) = P_wind_init;

	} وإلا {
		// no airspeed available
		_state.سیٹ_Zero();
		_state(اشاریہ_TAS_SCALE) = 1.0f;
		_P.سیٹ_Zero();
		_P(اشاریہ_W_N, اشاریہ_W_N) = _P(اشاریہ_W_E, اشاریہ_W_E) = sq(kInitialWindError);
	}

	_wind_estimator_reset = سچ;
	ویلا_initialised = hrt_absolute_ویلا();

	أرجع سچ;
}

فراغ
WindEstimator::update(uint64_t ویلا_now)
{
	إذا (!_initialised) {
		أرجع;
	}

	// set reset state to false (is set to true when initialise fuction is called later)
	_wind_estimator_reset = خطأ;

	// run covariance prediction at 1Hz
	إذا (ویلا_now - ویلا_آخری_update < 1_s || ویلا_آخری_update == 0) {
		إذا (ویلا_آخری_update == 0) {
			ویلا_آخری_update = ویلا_now;
		}

		أرجع;
	}

	ثابت عائم dt = (عائم)(ویلا_now - ویلا_آخری_update) * 1e-6f;
	ویلا_آخری_update = ویلا_now;

	// if airspeed scale is at default (1.0) and we are in the first 5 minutes of flight time, multiply kTasScalePsd by 100 for faster learning
	ثابت عائم tas_psd_multiplier = (fabsf(_scale_init - 1.0f) < FLT_EPSILON && (ویلا_now - ویلا_initialised < k_TASScale_Fast_Learn_ویلا)) ?
					 kTASScalePSDMultiplier : 1.f;

	میٹرکس::Matrix3f Qk;
	Qk(اشاریہ_W_N, اشاریہ_W_N) = _wind_psd * dt;
	Qk(اشاریہ_W_E, اشاریہ_W_E) = Qk(اشاریہ_W_N, اشاریہ_W_N);
	Qk(اشاریہ_TAS_SCALE, اشاریہ_TAS_SCALE) = kTasScalePsd * tas_psd_multiplier * dt;
	_P += Qk;
}

فراغ
WindEstimator::fuse_airspeed(uint64_t ویلا_now, ثابت عائم true_airspeed, ثابت میٹرکس::Vector3f &velI,
			     ثابت عائم hor_vel_variance, ثابت میٹرکس::Quatf &q_att)
{

	إذا (!_initialised) {
		// try to initialise
		_initialised = initialise(velI, hor_vel_variance, میٹرکس::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		أرجع;
	}

	// don't fuse faster than 10Hz
	إذا (ویلا_now - ویلا_آخری_airspeed_fuse < 100_ms) {
		أرجع;
	}

	ویلا_آخری_airspeed_fuse = ویلا_now;

	میٹرکس::میٹرکس_2<عائم, 1, 3> H_tas;
	میٹرکس::میٹرکس_2<عائم, 3, 1> K;

	sym::FuseAirspeed(velI, _state, _P, true_airspeed, _tas_var, FLT_EPSILON,
			  &H_tas, &K, &_tas_innov_var, &_tas_innov);

	ثابت منطقي meas_is_rejected = check_if_meas_is_rejected(_tas_innov, _tas_innov_var, kTasGate);

	إذا (_tas_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, میٹرکس::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		أرجع;

	} وإلا إذا (meas_is_rejected) {
		أرجع;
	}

	// apply correction to state
	_state(اشاریہ_W_N) += _tas_innov * K(اشاریہ_W_N, 0);
	_state(اشاریہ_W_E) += _tas_innov * K(اشاریہ_W_E, 0);
	_state(اشاریہ_TAS_SCALE) += _tas_innov * K(اشاریہ_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_tas * _P;

	لگاتار_sanity_checks();
}

فراغ
WindEstimator::fuse_beta(uint64_t ویلا_now, ثابت میٹرکس::Vector3f &velI, ثابت عائم hor_vel_variance,
			 ثابت میٹرکس::Quatf &q_att)
{
	إذا (!_initialised) {
		_initialised = initialise(velI, hor_vel_variance, میٹرکس::Eulerf(q_att).psi());
		أرجع;
	}

	// don't fuse faster than 10Hz
	إذا (ویلا_now - ویلا_آخری_beta_fuse < 100_ms) {
		أرجع;
	}

	ویلا_آخری_beta_fuse = ویلا_now;


	میٹرکس::میٹرکس_2<عائم, 1, 3> H_beta;
	میٹرکس::میٹرکس_2<عائم, 3, 1> K;

	sym::FuseBeta(velI, _state, _P, q_att, kBetaVar, FLT_EPSILON,
		      &H_beta, &K, &_beta_innov_var, &_beta_innov);

	ثابت منطقي meas_is_rejected = check_if_meas_is_rejected(_beta_innov, _beta_innov_var, kBetaGate);

	إذا (_beta_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, میٹرکس::Eulerf(q_att).psi());
		أرجع;

	} وإلا إذا (meas_is_rejected) {
		أرجع;
	}

	// apply correction to state
	_state(اشاریہ_W_N) += _beta_innov * K(اشاریہ_W_N, 0);
	_state(اشاریہ_W_E) += _beta_innov * K(اشاریہ_W_E, 0);
	_state(اشاریہ_TAS_SCALE) += _beta_innov * K(اشاریہ_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_beta * _P;

	لگاتار_sanity_checks();
}

فراغ
WindEstimator::لگاتار_sanity_checks()
{
	لكل (بنانشانی i = 0; i < 3; i++) {
		إذا (_P(i, i) < 0.0f) {
			// ill-conditioned covariance matrix, reset filter
			_initialised = خطأ;
			أرجع;
		}

		// limit covariance diagonals if they grow too large
		إذا (i < 2) {
			_P(i, i) = _P(i, i) > 25.0f ? 25.0f : _P(i, i);

		} وإلا إذا (i == 2) {
			_P(i, i) = _P(i, i) > 0.1f ? 0.1f : _P(i, i);
		}
	}

	إذا (!_state.isAllFinite()) {
		_initialised = خطأ;
		أرجع;
	}

	// attain symmetry
	لكل (بنانشانی صف = 0; صف < 3; صف++) {
		لكل (بنانشانی کالم = 0; کالم < صف; کالم++) {
			ثابت عائم tmp = (_P(صف, کالم) + _P(کالم, صف)) * 0.5f;
			_P(صف, کالم) = tmp;
			_P(کالم, صف) = tmp;
		}
	}
}

منطقي
WindEstimator::check_if_meas_is_rejected(عائم innov, عائم innov_var, uint8_t gate_ناپ)
{
	أرجع (innov * innov > gate_ناپ * gate_ناپ * innov_var);
}
