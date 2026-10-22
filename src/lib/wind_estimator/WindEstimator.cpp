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

#அந்தர்பாவ "WindEstimator.hpp"
#அந்தர்பாவ "python/generated/init_wind_using_airspeed.h"

தருக்க
WindEstimator::initialise(மாறிலி matrix::Vector3f &velI, மாறிலி தசம hor_vel_variance, மாறிலி தசம heading_rad,
			  மாறிலி தசம tas_meas, மாறிலி தசம tas_variance)
{
	எனில் (PX4_ISFINITE(tas_meas) && PX4_ISFINITE(tas_variance)) {

		constexpr தசம initial_heading_var = sq(math::radians(kInitialHeadingErrorDeg));
		constexpr தசம initial_sideslip_var = sq(math::radians(kInitialBetaErrorDeg));

		matrix::SquareMatrix<தசம, 2> P_wind_init;
		matrix::Vector2f wind_init;

		sym::InitWindUsingAirspeed(velI, heading_rad, tas_meas, hor_vel_variance, initial_heading_var, initial_sideslip_var,
					   tas_variance,
					   &wind_init, &P_wind_init);

		_state.xy() = wind_init;
		_state(INDEX_TAS_SCALE) = _scale_init;
		_P.slice<2, 2>(0, 0) = P_wind_init;

	} இல்லையெனில் {
		// no airspeed available
		_state.setZero();
		_state(INDEX_TAS_SCALE) = 1.0f;
		_P.setZero();
		_P(INDEX_W_N, INDEX_W_N) = _P(INDEX_W_E, INDEX_W_E) = sq(kInitialWindError);
	}

	_wind_estimator_reset = உண்மை;
	_time_initialised = hrt_absolute_time();

	திரும்பு உண்மை;
}

வெறுமை
WindEstimator::update(uint64_t time_now)
{
	எனில் (!_initialised) {
		திரும்பு;
	}

	// set reset state to false (is set to true when initialise fuction is called later)
	_wind_estimator_reset = பொய்;

	// run covariance prediction at 1Hz
	எனில் (time_now - _time_last_update < 1_s || _time_last_update == 0) {
		எனில் (_time_last_update == 0) {
			_time_last_update = time_now;
		}

		திரும்பு;
	}

	மாறிலி தசம dt = (தசம)(time_now - _time_last_update) * 1e-6f;
	_time_last_update = time_now;

	// if airspeed scale is at default (1.0) and we are in the first 5 minutes of flight time, multiply kTasScalePsd by 100 for faster learning
	மாறிலி தசம tas_psd_multiplier = (fabsf(_scale_init - 1.0f) < FLT_EPSILON && (time_now - _time_initialised < kTASScaleFastLearnTime)) ?
					 kTASScalePSDMultiplier : 1.f;

	matrix::Matrix3f Qk;
	Qk(INDEX_W_N, INDEX_W_N) = _wind_psd * dt;
	Qk(INDEX_W_E, INDEX_W_E) = Qk(INDEX_W_N, INDEX_W_N);
	Qk(INDEX_TAS_SCALE, INDEX_TAS_SCALE) = kTasScalePsd * tas_psd_multiplier * dt;
	_P += Qk;
}

வெறுமை
WindEstimator::fuse_airspeed(uint64_t time_now, மாறிலி தசம true_airspeed, மாறிலி matrix::Vector3f &velI,
			     மாறிலி தசம hor_vel_variance, மாறிலி matrix::Quatf &q_att)
{

	எனில் (!_initialised) {
		// try to initialise
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		திரும்பு;
	}

	// don't fuse faster than 10Hz
	எனில் (time_now - _time_last_airspeed_fuse < 100_ms) {
		திரும்பு;
	}

	_time_last_airspeed_fuse = time_now;

	matrix::Matrix<தசம, 1, 3> H_tas;
	matrix::Matrix<தசம, 3, 1> K;

	sym::FuseAirspeed(velI, _state, _P, true_airspeed, _tas_var, FLT_EPSILON,
			  &H_tas, &K, &_tas_innov_var, &_tas_innov);

	மாறிலி தருக்க meas_is_rejected = check_if_meas_is_rejected(_tas_innov, _tas_innov_var, kTasGate);

	எனில் (_tas_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		திரும்பு;

	} இல்லையெனில் எனில் (meas_is_rejected) {
		திரும்பு;
	}

	// apply correction to state
	_state(INDEX_W_N) += _tas_innov * K(INDEX_W_N, 0);
	_state(INDEX_W_E) += _tas_innov * K(INDEX_W_E, 0);
	_state(INDEX_TAS_SCALE) += _tas_innov * K(INDEX_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_tas * _P;

	run_sanity_checks();
}

வெறுமை
WindEstimator::fuse_beta(uint64_t time_now, மாறிலி matrix::Vector3f &velI, மாறிலி தசம hor_vel_variance,
			 மாறிலி matrix::Quatf &q_att)
{
	எனில் (!_initialised) {
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi());
		திரும்பு;
	}

	// don't fuse faster than 10Hz
	எனில் (time_now - _time_last_beta_fuse < 100_ms) {
		திரும்பு;
	}

	_time_last_beta_fuse = time_now;


	matrix::Matrix<தசம, 1, 3> H_beta;
	matrix::Matrix<தசம, 3, 1> K;

	sym::FuseBeta(velI, _state, _P, q_att, kBetaVar, FLT_EPSILON,
		      &H_beta, &K, &_beta_innov_var, &_beta_innov);

	மாறிலி தருக்க meas_is_rejected = check_if_meas_is_rejected(_beta_innov, _beta_innov_var, kBetaGate);

	எனில் (_beta_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi());
		திரும்பு;

	} இல்லையெனில் எனில் (meas_is_rejected) {
		திரும்பு;
	}

	// apply correction to state
	_state(INDEX_W_N) += _beta_innov * K(INDEX_W_N, 0);
	_state(INDEX_W_E) += _beta_innov * K(INDEX_W_E, 0);
	_state(INDEX_TAS_SCALE) += _beta_innov * K(INDEX_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_beta * _P;

	run_sanity_checks();
}

வெறுமை
WindEstimator::run_sanity_checks()
{
	க்கு (unsigned i = 0; i < 3; i++) {
		எனில் (_P(i, i) < 0.0f) {
			// ill-conditioned covariance matrix, reset filter
			_initialised = பொய்;
			திரும்பு;
		}

		// limit covariance diagonals if they grow too large
		எனில் (i < 2) {
			_P(i, i) = _P(i, i) > 25.0f ? 25.0f : _P(i, i);

		} இல்லையெனில் எனில் (i == 2) {
			_P(i, i) = _P(i, i) > 0.1f ? 0.1f : _P(i, i);
		}
	}

	எனில் (!_state.isAllFinite()) {
		_initialised = பொய்;
		திரும்பு;
	}

	// attain symmetry
	க்கு (unsigned row = 0; row < 3; row++) {
		க்கு (unsigned column = 0; column < row; column++) {
			மாறிலி தசம tmp = (_P(row, column) + _P(column, row)) * 0.5f;
			_P(row, column) = tmp;
			_P(column, row) = tmp;
		}
	}
}

தருக்க
WindEstimator::check_if_meas_is_rejected(தசம innov, தசம innov_var, uint8_t gate_size)
{
	திரும்பு (innov * innov > gate_size * gate_size * innov_var);
}
