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

#అన్తర్భావ "WindEstimator.hpp"
#అన్తర్భావ "python/generated/init_wind_using_airspeed.h"

తార్కిక
WindEstimator::initialise(స్థిరరాశి మాతృక::Vector3f &velI, స్థిరరాశి దశాంశం hor_vel_variance, స్థిరరాశి దశాంశం heading_rad,
			  స్థిరరాశి దశాంశం tas_meas, స్థిరరాశి దశాంశం tas_variance)
{
	ఒకవేళ (PX4_ISFINITE(tas_meas) && PX4_ISFINITE(tas_variance)) {

		constexpr దశాంశం initial_heading_var = sq(math::radians(kInitialHeadingErrorDeg));
		constexpr దశాంశం initial_sideslip_var = sq(math::radians(kInitialBetaErrorDeg));

		మాతృక::వర్గం_మాతృక<దశాంశం, 2> P_wind_init;
		మాతృక::Vector2f wind_init;

		sym::InitWindUsingAirspeed(velI, heading_rad, tas_meas, hor_vel_variance, initial_heading_var, initial_sideslip_var,
					   tas_variance,
					   &wind_init, &P_wind_init);

		_state.xy() = wind_init;
		_state(సూచీ_TAS_SCALE) = _scale_init;
		_P.ముక్క<2, 2>(0, 0) = P_wind_init;

	} లేకపోతే {
		// no airspeed available
		_state.సమితి_Zero();
		_state(సూచీ_TAS_SCALE) = 1.0f;
		_P.సమితి_Zero();
		_P(సూచీ_W_N, సూచీ_W_N) = _P(సూచీ_W_E, సూచీ_W_E) = sq(kInitialWindError);
	}

	_wind_estimator_reset = నిజం;
	సమయం_initialised = hrt_absolute_సమయం();

	తిరిగి నిజం;
}

శూన్య
WindEstimator::update(uint64_t సమయం_now)
{
	ఒకవేళ (!_initialised) {
		తిరిగి;
	}

	// set reset state to false (is set to true when initialise fuction is called later)
	_wind_estimator_reset = అబద్ధం;

	// run covariance prediction at 1Hz
	ఒకవేళ (సమయం_now - సమయం_చివరి_update < 1_s || సమయం_చివరి_update == 0) {
		ఒకవేళ (సమయం_చివరి_update == 0) {
			సమయం_చివరి_update = సమయం_now;
		}

		తిరిగి;
	}

	స్థిరరాశి దశాంశం dt = (దశాంశం)(సమయం_now - సమయం_చివరి_update) * 1e-6f;
	సమయం_చివరి_update = సమయం_now;

	// if airspeed scale is at default (1.0) and we are in the first 5 minutes of flight time, multiply kTasScalePsd by 100 for faster learning
	స్థిరరాశి దశాంశం tas_psd_multiplier = (fabsf(_scale_init - 1.0f) < FLT_EPSILON && (సమయం_now - సమయం_initialised < k_TASScale_Fast_Learn_సమయం)) ?
					 kTASScalePSDMultiplier : 1.f;

	మాతృక::Matrix3f Qk;
	Qk(సూచీ_W_N, సూచీ_W_N) = _wind_psd * dt;
	Qk(సూచీ_W_E, సూచీ_W_E) = Qk(సూచీ_W_N, సూచీ_W_N);
	Qk(సూచీ_TAS_SCALE, సూచీ_TAS_SCALE) = kTasScalePsd * tas_psd_multiplier * dt;
	_P += Qk;
}

శూన్య
WindEstimator::fuse_airspeed(uint64_t సమయం_now, స్థిరరాశి దశాంశం true_airspeed, స్థిరరాశి మాతృక::Vector3f &velI,
			     స్థిరరాశి దశాంశం hor_vel_variance, స్థిరరాశి మాతృక::Quatf &q_att)
{

	ఒకవేళ (!_initialised) {
		// try to initialise
		_initialised = initialise(velI, hor_vel_variance, మాతృక::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		తిరిగి;
	}

	// don't fuse faster than 10Hz
	ఒకవేళ (సమయం_now - సమయం_చివరి_airspeed_fuse < 100_ms) {
		తిరిగి;
	}

	సమయం_చివరి_airspeed_fuse = సమయం_now;

	మాతృక::మాతృక_2<దశాంశం, 1, 3> H_tas;
	మాతృక::మాతృక_2<దశాంశం, 3, 1> K;

	sym::FuseAirspeed(velI, _state, _P, true_airspeed, _tas_var, FLT_EPSILON,
			  &H_tas, &K, &_tas_innov_var, &_tas_innov);

	స్థిరరాశి తార్కిక meas_is_rejected = check_if_meas_is_rejected(_tas_innov, _tas_innov_var, kTasGate);

	ఒకవేళ (_tas_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, మాతృక::Eulerf(q_att).psi(), true_airspeed, _tas_var);
		తిరిగి;

	} లేకపోతే ఒకవేళ (meas_is_rejected) {
		తిరిగి;
	}

	// apply correction to state
	_state(సూచీ_W_N) += _tas_innov * K(సూచీ_W_N, 0);
	_state(సూచీ_W_E) += _tas_innov * K(సూచీ_W_E, 0);
	_state(సూచీ_TAS_SCALE) += _tas_innov * K(సూచీ_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_tas * _P;

	నిరంతరం_sanity_checks();
}

శూన్య
WindEstimator::fuse_beta(uint64_t సమయం_now, స్థిరరాశి మాతృక::Vector3f &velI, స్థిరరాశి దశాంశం hor_vel_variance,
			 స్థిరరాశి మాతృక::Quatf &q_att)
{
	ఒకవేళ (!_initialised) {
		_initialised = initialise(velI, hor_vel_variance, మాతృక::Eulerf(q_att).psi());
		తిరిగి;
	}

	// don't fuse faster than 10Hz
	ఒకవేళ (సమయం_now - సమయం_చివరి_beta_fuse < 100_ms) {
		తిరిగి;
	}

	సమయం_చివరి_beta_fuse = సమయం_now;


	మాతృక::మాతృక_2<దశాంశం, 1, 3> H_beta;
	మాతృక::మాతృక_2<దశాంశం, 3, 1> K;

	sym::FuseBeta(velI, _state, _P, q_att, kBetaVar, FLT_EPSILON,
		      &H_beta, &K, &_beta_innov_var, &_beta_innov);

	స్థిరరాశి తార్కిక meas_is_rejected = check_if_meas_is_rejected(_beta_innov, _beta_innov_var, kBetaGate);

	ఒకవేళ (_beta_innov_var < FLT_EPSILON) {
		// re init filter in case of a negative variance, and trigger early return to not fuse measurement
		_initialised = initialise(velI, hor_vel_variance, మాతృక::Eulerf(q_att).psi());
		తిరిగి;

	} లేకపోతే ఒకవేళ (meas_is_rejected) {
		తిరిగి;
	}

	// apply correction to state
	_state(సూచీ_W_N) += _beta_innov * K(సూచీ_W_N, 0);
	_state(సూచీ_W_E) += _beta_innov * K(సూచీ_W_E, 0);
	_state(సూచీ_TAS_SCALE) += _beta_innov * K(సూచీ_TAS_SCALE, 0);

	// update covariance matrix
	_P = _P - K * H_beta * _P;

	నిరంతరం_sanity_checks();
}

శూన్య
WindEstimator::నిరంతరం_sanity_checks()
{
	కొరకు (గుర్తులేని i = 0; i < 3; i++) {
		ఒకవేళ (_P(i, i) < 0.0f) {
			// ill-conditioned covariance matrix, reset filter
			_initialised = అబద్ధం;
			తిరిగి;
		}

		// limit covariance diagonals if they grow too large
		ఒకవేళ (i < 2) {
			_P(i, i) = _P(i, i) > 25.0f ? 25.0f : _P(i, i);

		} లేకపోతే ఒకవేళ (i == 2) {
			_P(i, i) = _P(i, i) > 0.1f ? 0.1f : _P(i, i);
		}
	}

	ఒకవేళ (!_state.isAllFinite()) {
		_initialised = అబద్ధం;
		తిరిగి;
	}

	// attain symmetry
	కొరకు (గుర్తులేని అడ్డువరుస = 0; అడ్డువరుస < 3; అడ్డువరుస++) {
		కొరకు (గుర్తులేని స్తంభం = 0; స్తంభం < అడ్డువరుస; స్తంభం++) {
			స్థిరరాశి దశాంశం tmp = (_P(అడ్డువరుస, స్తంభం) + _P(స్తంభం, అడ్డువరుస)) * 0.5f;
			_P(అడ్డువరుస, స్తంభం) = tmp;
			_P(స్తంభం, అడ్డువరుస) = tmp;
		}
	}
}

తార్కిక
WindEstimator::check_if_meas_is_rejected(దశాంశం innov, దశాంశం innov_var, uint8_t gate_కొలత)
{
	తిరిగి (innov * innov > gate_కొలత * gate_కొలత * innov_var);
}
