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
WindEstimator::initialise(स्थिरांक matrix::Vector3f &velI, स्थिरांक दशांश hor_vel_variance, स्थिरांक दशांश heading_rad,
			  स्थिरांक दशांश tas_meas, स्थिरांक दशांश tas_variance)
{
	जर (PX4_ISFINITE(tas_meas) && PX4_ISFINITE(tas_variance)) {

		constexpr दशांश initial_heading_var = sq(math::radians(kInitialHeadingErrorDeg));
		constexpr दशांश initial_sideslip_var = sq(math::radians(kInitialBetaErrorDeg));

		matrix::SquareMatrix<दशांश, 2> P_wind_init;
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

	परत सत्य;
}

शून्य
WindEstimator::update(uint64_t time_now)
{
	जर (!_initialised) {
		परत;
	}

	// set reset state to false (is set to true when initialise fuction is called later)
	_wind_estimator_reset = असत्य;

	// run covariance prediction at 1Hz
	जर (time_now - _time_last_update < 1_s || _time_last_update == 0) {
		जर (_time_last_update == 0) {
			_time_last_update = time_now;
		}

		परत;
	}

	स्थिरांक दशांश dt = (दशांश)(time_now - _time_last_update) * 1e-6f;
	_time_last_update = time_now;

	// if airspeed scale is at default (1.0) and we are in the first 5 minutes of flight time, multiply kTasScalePsd by 100 for faster learning
	स्थिरांक दशांश tas_psd_multiplier = (fabsf(_scale_init - 1.0f) < भग्न_अंतर && (time_now - _time_initialised < kTASScaleFastLearnTime)) ?
					 kTASScalePSDMultiplier : 1.f;

	matrix::Matrix3f Qk;
	Qk(INDEX_W_N, INDEX_W_N) = _wind_psd * dt;
	Qk(INDEX_W_E, INDEX_W_E) = Qk(INDEX_W_N, INDEX_W_N);
	Qk(INDEX_TAS_SCALE, INDEX_TAS_SCALE) = kTasScalePsd * tas_psd_multiplier * dt;
	_P += Qk;
}

शून्य
WindEstimator::fuse_airspeed(uint64_t time_now, स्थिरांक दशांश true_airspeed, स्थिरांक matrix::Vector3f &velI,
			     स्थिरांक दशांश hor_vel_variance, स्थिरांक matrix::Quatf &q_att)
{

	जर (!_initialised) {
		// try to initialise
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		परत;
	}

	// don't fuse faster than 10Hz
	जर (time_now - _time_last_airspeed_fuse < 100_ms) {
		परत;
	}

	_time_last_airspeed_fuse = time_now;

	matrix::Matrix<दशांश, 1, 3> H_tas;
	matrix::Matrix<दशांश, 3, 1> K;

	sym::FuseAirspeed(velI, _state, _P, true_airspeed, _tas_var, भग्न_अंतर,
			  &H_tas, &K, &_tas_innov_var, &_tas_innov);

	स्थिरांक तार्किक meas_is_rejected = check_if_meas_is_rejected(_tas_innov, _tas_innov_var, kTasGate);

	जर (_tas_innov_var < भग्न_अंतर) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		परत;

	} अन्यथा जर (meas_is_rejected) {
		परत;
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
WindEstimator::fuse_beta(uint64_t time_now, स्थिरांक matrix::Vector3f &velI, स्थिरांक दशांश hor_vel_variance,
			 स्थिरांक matrix::Quatf &q_att)
{
	जर (!_initialised) {
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi());
		परत;
	}

	// don't fuse faster than 10Hz
	जर (time_now - _time_last_beta_fuse < 100_ms) {
		परत;
	}

	_time_last_beta_fuse = time_now;


	matrix::Matrix<दशांश, 1, 3> H_beta;
	matrix::Matrix<दशांश, 3, 1> K;

	sym::FuseBeta(velI, _state, _P, q_att, kBetaVar, भग्न_अंतर,
		      &H_beta, &K, &_beta_innov_var, &_beta_innov);

	स्थिरांक तार्किक meas_is_rejected = check_if_meas_is_rejected(_beta_innov, _beta_innov_var, kBetaGate);

	जर (_beta_innov_var < भग्न_अंतर) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, matrix::Eulerf(q_att).psi());
		परत;

	} अन्यथा जर (meas_is_rejected) {
		परत;
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
	साठी (अचिन्हित i = 0; i < 3; i++) {
		जर (_P(i, i) < 0.0f) {
			// ill-conditioned covariance matrix, reset filter
			_initialised = असत्य;
			परत;
		}

		// limit covariance diagonals if they grow too large
		जर (i < 2) {
			_P(i, i) = _P(i, i) > 25.0f ? 25.0f : _P(i, i);

		} अन्यथा जर (i == 2) {
			_P(i, i) = _P(i, i) > 0.1f ? 0.1f : _P(i, i);
		}
	}

	जर (!_state.isAllFinite()) {
		_initialised = असत्य;
		परत;
	}

	// attain symmetry
	साठी (अचिन्हित row = 0; row < 3; row++) {
		साठी (अचिन्हित column = 0; column < row; column++) {
			स्थिरांक दशांश tmp = (_P(row, column) + _P(column, row)) * 0.5f;
			_P(row, column) = tmp;
			_P(column, row) = tmp;
		}
	}
}

तार्किक
WindEstimator::check_if_meas_is_rejected(दशांश innov, दशांश innov_var, uint8_t gate_size)
{
	परत (innov * innov > gate_size * gate_size * innov_var);
}
