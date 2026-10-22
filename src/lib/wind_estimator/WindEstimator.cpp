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
WindEstimator::initialise(ثابت ماتریس::Vector3f &velI, ثابت عائم hor_vel_variance, ثابت عائم heading_rad,
			  ثابت عائم tas_meas, ثابت عائم tas_variance)
{
	إذا (PX4_ISFINITE(tas_meas) && PX4_ISFINITE(tas_variance)) {

		constexpr عائم initial_heading_var = sq(math::radians(kInitialHeadingErrorDeg));
		constexpr عائم initial_sideslip_var = sq(math::radians(kInitialBetaErrorDeg));

		ماتریس::مربع_ماتریس<عائم, 2> P_wind_init;
		ماتریس::Vector2f wind_init;

		sym::InitWindUsingAirspeed(velI, heading_rad, tas_meas, hor_vel_variance, initial_heading_var, initial_sideslip_var,
					   tas_variance,
					   &wind_init, &P_wind_init);

		_state.xy() = wind_init;
		_state(شاخص_TAS_SCALE) = _scale_init;
		_P.برش<2, 2>(0, 0) = P_wind_init;

	} وإلا {
		// no airspeed available
		_state.مجموعه_Zero();
		_state(شاخص_TAS_SCALE) = 1.0f;
		_P.مجموعه_Zero();
		_P(شاخص_W_N, شاخص_W_N) = _P(شاخص_W_E, شاخص_W_E) = sq(kInitialWindError);
	}

	_wind_estimator_reset = درست;
	زمان_initialised = hrt_absolute_زمان();

	أرجع درست;
}

فراغ
WindEstimator::update(uint64_t زمان_now)
{
	إذا (!_initialised) {
		أرجع;
	}

	// set reset state to false (is set to true when initialise fuction is called later)
	_wind_estimator_reset = خطأ;

	// run covariance prediction at 1Hz
	إذا (زمان_now - زمان_آخرین_update < 1_s || زمان_آخرین_update == 0) {
		إذا (زمان_آخرین_update == 0) {
			زمان_آخرین_update = زمان_now;
		}

		أرجع;
	}

	ثابت عائم dt = (عائم)(زمان_now - زمان_آخرین_update) * 1e-6f;
	زمان_آخرین_update = زمان_now;

	// if airspeed scale is at default (1.0) and we are in the first 5 minutes of flight time, multiply kTasScalePsd by 100 for faster learning
	ثابت عائم tas_psd_multiplier = (fabsf(_scale_init - 1.0f) < FLT_EPSILON && (زمان_now - زمان_initialised < k_TASScale_Fast_Learn_زمان)) ?
					 kTASScalePSDMultiplier : 1.f;

	ماتریس::Matrix3f Qk;
	Qk(شاخص_W_N, شاخص_W_N) = _wind_psd * dt;
	Qk(شاخص_W_E, شاخص_W_E) = Qk(شاخص_W_N, شاخص_W_N);
	Qk(شاخص_TAS_SCALE, شاخص_TAS_SCALE) = kTasScalePsd * tas_psd_multiplier * dt;
	_P += Qk;
}

فراغ
WindEstimator::fuse_airspeed(uint64_t زمان_now, ثابت عائم true_airspeed, ثابت ماتریس::Vector3f &velI,
			     ثابت عائم hor_vel_variance, ثابت ماتریس::Quatf &q_att)
{

	إذا (!_initialised) {
		// try to initialise
		_initialised = initialise(velI, hor_vel_variance, ماتریس::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		أرجع;
	}

	// don't fuse faster than 10Hz
	إذا (زمان_now - زمان_آخرین_airspeed_fuse < 100_ms) {
		أرجع;
	}

	زمان_آخرین_airspeed_fuse = زمان_now;

	ماتریس::ماتریس_2<عائم, 1, 3> H_tas;
	ماتریس::ماتریس_2<عائم, 3, 1> K;

	sym::FuseAirspeed(velI, _state, _P, true_airspeed, _tas_var, FLT_EPSILON,
			  &H_tas, &K, &_tas_innov_var, &_tas_innov);

	ثابت منطقي meas_is_rejected = check_if_meas_is_rejected(_tas_innov, _tas_innov_var, kTasGate);

	إذا (_tas_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, ماتریس::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		أرجع;

	} وإلا إذا (meas_is_rejected) {
		أرجع;
	}

	// apply correction to state
	_state(شاخص_W_N) += _tas_innov * K(شاخص_W_N, 0);
	_state(شاخص_W_E) += _tas_innov * K(شاخص_W_E, 0);
	_state(شاخص_TAS_SCALE) += _tas_innov * K(شاخص_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_tas * _P;

	پیاپی_sanity_checks();
}

فراغ
WindEstimator::fuse_beta(uint64_t زمان_now, ثابت ماتریس::Vector3f &velI, ثابت عائم hor_vel_variance,
			 ثابت ماتریس::Quatf &q_att)
{
	إذا (!_initialised) {
		_initialised = initialise(velI, hor_vel_variance, ماتریس::Eulerf(q_att).psi());
		أرجع;
	}

	// don't fuse faster than 10Hz
	إذا (زمان_now - زمان_آخرین_beta_fuse < 100_ms) {
		أرجع;
	}

	زمان_آخرین_beta_fuse = زمان_now;


	ماتریس::ماتریس_2<عائم, 1, 3> H_beta;
	ماتریس::ماتریس_2<عائم, 3, 1> K;

	sym::FuseBeta(velI, _state, _P, q_att, kBetaVar, FLT_EPSILON,
		      &H_beta, &K, &_beta_innov_var, &_beta_innov);

	ثابت منطقي meas_is_rejected = check_if_meas_is_rejected(_beta_innov, _beta_innov_var, kBetaGate);

	إذا (_beta_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, ماتریس::Eulerf(q_att).psi());
		أرجع;

	} وإلا إذا (meas_is_rejected) {
		أرجع;
	}

	// apply correction to state
	_state(شاخص_W_N) += _beta_innov * K(شاخص_W_N, 0);
	_state(شاخص_W_E) += _beta_innov * K(شاخص_W_E, 0);
	_state(شاخص_TAS_SCALE) += _beta_innov * K(شاخص_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_beta * _P;

	پیاپی_sanity_checks();
}

فراغ
WindEstimator::پیاپی_sanity_checks()
{
	لكل (بیعلامت i = 0; i < 3; i++) {
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
	لكل (بیعلامت ردیف = 0; ردیف < 3; ردیف++) {
		لكل (بیعلامت ستونی = 0; ستونی < ردیف; ستونی++) {
			ثابت عائم tmp = (_P(ردیف, ستونی) + _P(ستونی, ردیف)) * 0.5f;
			_P(ردیف, ستونی) = tmp;
			_P(ستونی, ردیف) = tmp;
		}
	}
}

منطقي
WindEstimator::check_if_meas_is_rejected(عائم innov, عائم innov_var, uint8_t gate_اندازه)
{
	أرجع (innov * innov > gate_اندازه * gate_اندازه * innov_var);
}
