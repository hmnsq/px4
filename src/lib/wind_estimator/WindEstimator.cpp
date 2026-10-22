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

#अन्तर्भाव "WindEstimator.hpp"
#अन्तर्भाव "python/generated/init_wind_using_airspeed.h"

तार्किक
WindEstimator::initialise(ध्रुव matrix::Vector3f &velI, ध्रुव भिन्न hor_vel_variance, ध्रुव भिन्न heading_rad,
			  ध्रुव भिन्न tas_meas, ध्रुव भिन्न tas_variance)
{
	यदि (PX4_ISFINITE(tas_meas) && PX4_ISFINITE(tas_variance)) {

		constexpr भिन्न initial_heading_var = sq(math::radians(kInitialHeadingErrorDeg));
		constexpr भिन्न initial_sideslip_var = sq(math::radians(kInitialBetaErrorDeg));

		matrix::SquareMatrix<भिन्न, 2> P_wind_init;
		matrix::Vector2f wind_init;

		sym::InitWindUsingAirspeed(velI, heading_rad, tas_meas, hor_vel_variance, initial_heading_var, initial_sideslip_var,
					   tas_variance,
					   &wind_init, &P_wind_init);

		_state.xy() = wind_init;
		_state(INDEX_TAS_SCALE) = _scale_init;
		_P.slice<2, 2>(0, 0) = P_wind_init;

	} अन्यथा {
		// no airspeed available
		_state.setZero();
		_state(INDEX_TAS_SCALE) = 1.0f;
		_P.setZero();
		_P(INDEX_W_N, INDEX_W_N) = _P(INDEX_W_E, INDEX_W_E) = sq(kInitialWindError);
	}

	_wind_estimator_reset = सत्य;
	_time_initialised = hrt_absolute_time();

	प्रत्यागम सत्य;
}

शून्य
WindEstimator::update(uint64_t time_now)
{
	यदि (!_initialised) {
		प्रत्यागम;
	}

	// set reset state to false (is set to true when initialise fuction is called later)
	_wind_estimator_reset = असत्य;

	// run covariance prediction at 1Hz
	यदि (time_now - _time_last_update < 1_s || _time_last_update == 0) {
		यदि (_time_last_update == 0) {
			_time_last_update = time_now;
		}

		प्रत्यागम;
	}

	ध्रुव भिन्न dt = (भिन्न)(time_now - _time_last_update) * 1e-6f;
	_time_last_update = time_now;

	// if airspeed scale is at default (1.0) and we are in the first 5 minutes of flight time, multiply kTasScalePsd by 100 for faster learning
	ध्रुव भिन्न tas_psd_multiplier = (fabsf(_scale_init - 1.0f) < FLT_EPSILON && (time_now - _time_initialised < kTASScaleFastLearnTime)) ?
					 kTASScalePSDMultiplier : 1.f;

	matrix::Matrix3f Qk;
	Qk(INDEX_W_N, INDEX_W_N) = _wind_psd * dt;
	Qk(INDEX_W_E, INDEX_W_E) = Qk(INDEX_W_N, INDEX_W_N);
	Qk(INDEX_TAS_SCALE, INDEX_TAS_SCALE) = kTasScalePsd * tas_psd_multiplier * dt;
	_P += Qk;
}

शून्य
WindEstimator::fuse_airspeed(uint64_t time_now, ध्रुव भिन्न true_airspeed, ध्रुव matrix::Vector3f &velI,
			     ध्रुव भिन्न hor_vel_variance, ध्रुव matrix::Quatf &q_att)
{

	यदि (!_initialised) {
		// try to initialise
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		प्रत्यागम;
	}

	// don't fuse faster than 10Hz
	यदि (time_now - _time_last_airspeed_fuse < 100_ms) {
		प्रत्यागम;
	}

	_time_last_airspeed_fuse = time_now;

	matrix::Matrix<भिन्न, 1, 3> H_tas;
	matrix::Matrix<भिन्न, 3, 1> K;

	sym::FuseAirspeed(velI, _state, _P, true_airspeed, _tas_var, FLT_EPSILON,
			  &H_tas, &K, &_tas_innov_var, &_tas_innov);

	ध्रुव तार्किक meas_is_rejected = check_if_meas_is_rejected(_tas_innov, _tas_innov_var, kTasGate);

	यदि (_tas_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		प्रत्यागम;

	} अन्यथा यदि (meas_is_rejected) {
		प्रत्यागम;
	}

	// apply correction to state
	_state(INDEX_W_N) += _tas_innov * K(INDEX_W_N, 0);
	_state(INDEX_W_E) += _tas_innov * K(INDEX_W_E, 0);
	_state(INDEX_TAS_SCALE) += _tas_innov * K(INDEX_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_tas * _P;

	run_sanity_checks();
}

शून्य
WindEstimator::fuse_beta(uint64_t time_now, ध्रुव matrix::Vector3f &velI, ध्रुव भिन्न hor_vel_variance,
			 ध्रुव matrix::Quatf &q_att)
{
	यदि (!_initialised) {
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi());
		प्रत्यागम;
	}

	// don't fuse faster than 10Hz
	यदि (time_now - _time_last_beta_fuse < 100_ms) {
		प्रत्यागम;
	}

	_time_last_beta_fuse = time_now;


	matrix::Matrix<भिन्न, 1, 3> H_beta;
	matrix::Matrix<भिन्न, 3, 1> K;

	sym::FuseBeta(velI, _state, _P, q_att, kBetaVar, FLT_EPSILON,
		      &H_beta, &K, &_beta_innov_var, &_beta_innov);

	ध्रुव तार्किक meas_is_rejected = check_if_meas_is_rejected(_beta_innov, _beta_innov_var, kBetaGate);

	यदि (_beta_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi());
		प्रत्यागम;

	} अन्यथा यदि (meas_is_rejected) {
		प्रत्यागम;
	}

	// apply correction to state
	_state(INDEX_W_N) += _beta_innov * K(INDEX_W_N, 0);
	_state(INDEX_W_E) += _beta_innov * K(INDEX_W_E, 0);
	_state(INDEX_TAS_SCALE) += _beta_innov * K(INDEX_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_beta * _P;

	run_sanity_checks();
}

शून्य
WindEstimator::run_sanity_checks()
{
	प्रति (unsigned i = 0; i < 3; i++) {
		यदि (_P(i, i) < 0.0f) {
			// ill-conditioned covariance matrix, reset filter
			_initialised = असत्य;
			प्रत्यागम;
		}

		// limit covariance diagonals if they grow too large
		यदि (i < 2) {
			_P(i, i) = _P(i, i) > 25.0f ? 25.0f : _P(i, i);

		} अन्यथा यदि (i == 2) {
			_P(i, i) = _P(i, i) > 0.1f ? 0.1f : _P(i, i);
		}
	}

	यदि (!_state.isAllFinite()) {
		_initialised = असत्य;
		प्रत्यागम;
	}

	// attain symmetry
	प्रति (unsigned row = 0; row < 3; row++) {
		प्रति (unsigned column = 0; column < row; column++) {
			ध्रुव भिन्न tmp = (_P(row, column) + _P(column, row)) * 0.5f;
			_P(row, column) = tmp;
			_P(column, row) = tmp;
		}
	}
}

तार्किक
WindEstimator::check_if_meas_is_rejected(भिन्न innov, भिन्न innov_var, uint8_t gate_size)
{
	प्रत्यागम (innov * innov > gate_size * gate_size * innov_var);
}
