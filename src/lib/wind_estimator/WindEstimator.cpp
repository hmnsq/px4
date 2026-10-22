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

#ଅନ୍ତର୍ଭାଵ "WindEstimator.hpp"
#ଅନ୍ତର୍ଭାଵ "python/generated/init_wind_using_airspeed.h"

ତାର୍କିକ
WindEstimator::initialise(ଅଚଳ matrix::Vector3f &velI, ଅଚଳ ଦଶମିକ hor_vel_variance, ଅଚଳ ଦଶମିକ heading_rad,
			  ଅଚଳ ଦଶମିକ tas_meas, ଅଚଳ ଦଶମିକ tas_variance)
{
	ଯଦି (PX4_ISFINITE(tas_meas) && PX4_ISFINITE(tas_variance)) {

		constexpr ଦଶମିକ initial_heading_var = sq(math::radians(kInitialHeadingErrorDeg));
		constexpr ଦଶମିକ initial_sideslip_var = sq(math::radians(kInitialBetaErrorDeg));

		matrix::SquareMatrix<ଦଶମିକ, 2> P_wind_init;
		matrix::Vector2f wind_init;

		sym::InitWindUsingAirspeed(velI, heading_rad, tas_meas, hor_vel_variance, initial_heading_var, initial_sideslip_var,
					   tas_variance,
					   &wind_init, &P_wind_init);

		_state.xy() = wind_init;
		_state(INDEX_TAS_SCALE) = _scale_init;
		_P.slice<2, 2>(0, 0) = P_wind_init;

	} ଅନ୍ୟଥା {
		// no airspeed available
		_state.setZero();
		_state(INDEX_TAS_SCALE) = 1.0f;
		_P.setZero();
		_P(INDEX_W_N, INDEX_W_N) = _P(INDEX_W_E, INDEX_W_E) = sq(kInitialWindError);
	}

	_wind_estimator_reset = ସତ୍ୟ;
	_time_initialised = hrt_absolute_time();

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ
WindEstimator::update(uint64_t time_now)
{
	ଯଦି (!_initialised) {
		ଫେରନ୍ତୁ;
	}

	// set reset state to false (is set to true when initialise fuction is called later)
	_wind_estimator_reset = ମିଥ୍ୟା;

	// run covariance prediction at 1Hz
	ଯଦି (time_now - _time_last_update < 1_s || _time_last_update == 0) {
		ଯଦି (_time_last_update == 0) {
			_time_last_update = time_now;
		}

		ଫେରନ୍ତୁ;
	}

	ଅଚଳ ଦଶମିକ dt = (ଦଶମିକ)(time_now - _time_last_update) * 1e-6f;
	_time_last_update = time_now;

	// if airspeed scale is at default (1.0) and we are in the first 5 minutes of flight time, multiply kTasScalePsd by 100 for faster learning
	ଅଚଳ ଦଶମିକ tas_psd_multiplier = (fabsf(_scale_init - 1.0f) < FLT_EPSILON && (time_now - _time_initialised < kTASScaleFastLearnTime)) ?
					 kTASScalePSDMultiplier : 1.f;

	matrix::Matrix3f Qk;
	Qk(INDEX_W_N, INDEX_W_N) = _wind_psd * dt;
	Qk(INDEX_W_E, INDEX_W_E) = Qk(INDEX_W_N, INDEX_W_N);
	Qk(INDEX_TAS_SCALE, INDEX_TAS_SCALE) = kTasScalePsd * tas_psd_multiplier * dt;
	_P += Qk;
}

ଶୂନ୍ୟ
WindEstimator::fuse_airspeed(uint64_t time_now, ଅଚଳ ଦଶମିକ true_airspeed, ଅଚଳ matrix::Vector3f &velI,
			     ଅଚଳ ଦଶମିକ hor_vel_variance, ଅଚଳ matrix::Quatf &q_att)
{

	ଯଦି (!_initialised) {
		// try to initialise
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		ଫେରନ୍ତୁ;
	}

	// don't fuse faster than 10Hz
	ଯଦି (time_now - _time_last_airspeed_fuse < 100_ms) {
		ଫେରନ୍ତୁ;
	}

	_time_last_airspeed_fuse = time_now;

	matrix::Matrix<ଦଶମିକ, 1, 3> H_tas;
	matrix::Matrix<ଦଶମିକ, 3, 1> K;

	sym::FuseAirspeed(velI, _state, _P, true_airspeed, _tas_var, FLT_EPSILON,
			  &H_tas, &K, &_tas_innov_var, &_tas_innov);

	ଅଚଳ ତାର୍କିକ meas_is_rejected = check_if_meas_is_rejected(_tas_innov, _tas_innov_var, kTasGate);

	ଯଦି (_tas_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		ଫେରନ୍ତୁ;

	} ଅନ୍ୟଥା ଯଦି (meas_is_rejected) {
		ଫେରନ୍ତୁ;
	}

	// apply correction to state
	_state(INDEX_W_N) += _tas_innov * K(INDEX_W_N, 0);
	_state(INDEX_W_E) += _tas_innov * K(INDEX_W_E, 0);
	_state(INDEX_TAS_SCALE) += _tas_innov * K(INDEX_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_tas * _P;

	run_sanity_checks();
}

ଶୂନ୍ୟ
WindEstimator::fuse_beta(uint64_t time_now, ଅଚଳ matrix::Vector3f &velI, ଅଚଳ ଦଶମିକ hor_vel_variance,
			 ଅଚଳ matrix::Quatf &q_att)
{
	ଯଦି (!_initialised) {
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi());
		ଫେରନ୍ତୁ;
	}

	// don't fuse faster than 10Hz
	ଯଦି (time_now - _time_last_beta_fuse < 100_ms) {
		ଫେରନ୍ତୁ;
	}

	_time_last_beta_fuse = time_now;


	matrix::Matrix<ଦଶମିକ, 1, 3> H_beta;
	matrix::Matrix<ଦଶମିକ, 3, 1> K;

	sym::FuseBeta(velI, _state, _P, q_att, kBetaVar, FLT_EPSILON,
		      &H_beta, &K, &_beta_innov_var, &_beta_innov);

	ଅଚଳ ତାର୍କିକ meas_is_rejected = check_if_meas_is_rejected(_beta_innov, _beta_innov_var, kBetaGate);

	ଯଦି (_beta_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi());
		ଫେରନ୍ତୁ;

	} ଅନ୍ୟଥା ଯଦି (meas_is_rejected) {
		ଫେରନ୍ତୁ;
	}

	// apply correction to state
	_state(INDEX_W_N) += _beta_innov * K(INDEX_W_N, 0);
	_state(INDEX_W_E) += _beta_innov * K(INDEX_W_E, 0);
	_state(INDEX_TAS_SCALE) += _beta_innov * K(INDEX_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_beta * _P;

	run_sanity_checks();
}

ଶୂନ୍ୟ
WindEstimator::run_sanity_checks()
{
	ପାଇଁ (unsigned i = 0; i < 3; i++) {
		ଯଦି (_P(i, i) < 0.0f) {
			// ill-conditioned covariance matrix, reset filter
			_initialised = ମିଥ୍ୟା;
			ଫେରନ୍ତୁ;
		}

		// limit covariance diagonals if they grow too large
		ଯଦି (i < 2) {
			_P(i, i) = _P(i, i) > 25.0f ? 25.0f : _P(i, i);

		} ଅନ୍ୟଥା ଯଦି (i == 2) {
			_P(i, i) = _P(i, i) > 0.1f ? 0.1f : _P(i, i);
		}
	}

	ଯଦି (!_state.isAllFinite()) {
		_initialised = ମିଥ୍ୟା;
		ଫେରନ୍ତୁ;
	}

	// attain symmetry
	ପାଇଁ (unsigned row = 0; row < 3; row++) {
		ପାଇଁ (unsigned column = 0; column < row; column++) {
			ଅଚଳ ଦଶମିକ tmp = (_P(row, column) + _P(column, row)) * 0.5f;
			_P(row, column) = tmp;
			_P(column, row) = tmp;
		}
	}
}

ତାର୍କିକ
WindEstimator::check_if_meas_is_rejected(ଦଶମିକ innov, ଦଶମିକ innov_var, uint8_t gate_size)
{
	ଫେରନ୍ତୁ (innov * innov > gate_size * gate_size * innov_var);
}
