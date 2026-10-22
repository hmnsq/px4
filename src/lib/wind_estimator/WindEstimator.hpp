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
 * @file WindEstimator.hpp
 * A wind and airspeed scale estimator.
 */

#pragma once

#અન્તર્ભાવ <drivers/drv_hrt.h>

#અન્તર્ભાવ <mathlib/mathlib.h>
#અન્તર્ભાવ <matrix/math.hpp>

#અન્તર્ભાવ "python/generated/fuse_airspeed.h"
#અન્તર્ભાવ "python/generated/fuse_beta.h"
#અન્તર્ભાવ "python/generated/init_wind_using_airspeed.h"

using નામાકાશ time_literals;

વર્ગ WindEstimator
{
જાહેર:
	WindEstimator() = મૂળભૂત;
	~WindEstimator() = મૂળભૂત;

	// no copy, assignment, move, move assignment
	WindEstimator(અચલ WindEstimator &) = delete;
	WindEstimator &operator=(અચલ WindEstimator &) = delete;
	WindEstimator(WindEstimator &&) = delete;
	WindEstimator &operator=(WindEstimator &&) = delete;

	શૂન્ય update(uint64_t time_now);

	શૂન્ય fuse_airspeed(uint64_t time_now, દશાંશ true_airspeed, અચલ matrix::Vector3f &velI,
			   અચલ દશાંશ hor_vel_variance, અચલ matrix::Quatf &q_att);
	શૂન્ય fuse_beta(uint64_t time_now, અચલ matrix::Vector3f &velI, અચલ દશાંશ hor_vel_variance,
		       અચલ matrix::Quatf &q_att);

	તાર્કિક is_estimate_valid() { પાછા _initialised; }

	તાર્કિક check_if_meas_is_rejected(દશાંશ innov, દશાંશ innov_var, uint8_t gate_size);

	matrix::Vector2f get_wind() { પાછા matrix::Vector2f{_state(INDEX_W_N), _state(INDEX_W_E)}; }

	// invert scale (CAS = IAS * scale), protect agains division by 0, constrain to [0.1, 10]
	દશાંશ get_tas_scale() { પાછા 1.f / math::constrain(_state(INDEX_TAS_SCALE), 0.1f, 10.0f); }
	દશાંશ get_tas_scale_var() { પાછા _P(INDEX_TAS_SCALE, INDEX_TAS_SCALE); }
	દશાંશ get_tas_innov() { પાછા _tas_innov; }
	દશાંશ get_tas_innov_var() { પાછા _tas_innov_var; }
	દશાંશ get_beta_innov() { પાછા _beta_innov; }
	દશાંશ get_beta_innov_var() { પાછા _beta_innov_var; }
	matrix::Vector2f  get_wind_var() { પાછા matrix::Vector2f{_P(INDEX_W_N, INDEX_W_N), _P(INDEX_W_E, INDEX_W_E)}; }
	તાર્કિક get_wind_estimator_reset() { પાછા _wind_estimator_reset; }

	// unaided, the state uncertainty (diagonal of sqrt(P)) grows by the process noise spectral density every second
	શૂન્ય set_wind_process_noise_spectral_density(દશાંશ wind_nsd) { _wind_psd = wind_nsd * wind_nsd; }

	શૂન્ય set_tas_noise(દશાંશ tas_sigma) { _tas_var = tas_sigma * tas_sigma; }

	શૂન્ય set_scale_init(દશાંશ scale_init) {_scale_init = 1.f / math::constrain(scale_init, 0.1f, 10.f); }

	શૂન્ય reset_scale_to_init()
	{
		_state(INDEX_TAS_SCALE) = _scale_init;
		auto P_wind = _P.slice<2, 2>(0, 0);
		_P.setZero();
		_P.slice<2, 2>(0, 0) = P_wind;
	}

ખાનગી:
	ગણના {
		INDEX_W_N = 0,
		INDEX_W_E,
		INDEX_TAS_SCALE
	};	///< enum which can be used to access state.

	સ્થિર constexpr દશાંશ kInitialWindError =
		2.5f; // initial uncertainty of each wind state when filter is initialised without airspeed [m/s]
	સ્થિર constexpr દશાંશ kInitialBetaErrorDeg =
		15.0f;	// sidelip uncertainty used to initialise the filter with a true airspeed measurement [deg]
	સ્થિર constexpr દશાંશ kInitialHeadingErrorDeg =
		10.0f; // heading uncertainty used to initialise the filter with a true airspeed measurement [deg]

	matrix::Vector3f _state{0.f, 0.f, 1.f};
	matrix::Matrix3f _P;		///< state covariance matrix

	દશાંશ _tas_innov{0.0f};	///< true airspeed innovation
	દશાંશ _tas_innov_var{0.0f};	///< true airspeed innovation variance

	દશાંશ _beta_innov{0.0f};	///< sideslip innovation
	દશાંશ _beta_innov_var{0.0f};	///< sideslip innovation variance

	તાર્કિક _initialised{ખોટું};	///< True: filter has been initialised

	દશાંશ _wind_psd{0.01f};	///< wind process noise power spectral density (m^2/s^4/Hz), (0.1 m/s^2/sqrt(Hz))^2
	સ્થિર constexpr દશાંશ kTasScalePsd{1e-8f};	///< true airspeed scale process noise power spectral density (1/s^2/Hz), (0.0001 1/s/sqrt(Hz))^2
	દશાંશ _tas_var{1.96f};		///< true airspeed measurement noise variance (m/s)^2, (1.4 m/s)^2
	સ્થિર constexpr દશાંશ kBetaVar{0.0225f};	///< sideslip measurement noise variance (rad)^2, (0.15 rad)^2
	સ્થિર constexpr uint8_t kTasGate{4};	///< airspeed fusion gate size (SD)
	સ્થિર constexpr uint8_t kBetaGate{1};	///< sideslip fusion gate size (SD)

	દશાંશ _scale_init{1.f};

	uint64_t _time_last_airspeed_fuse = 0;	///< timestamp of last airspeed fusion
	uint64_t _time_last_beta_fuse = 0;	///< timestamp of last sideslip fusion
	uint64_t _time_last_update = 0;		///< timestamp of last covariance prediction
	uint64_t _time_initialised = 0;         ///< timestamp when estimator is initialised

	સ્થિર constexpr દશાંશ kTASScalePSDMultiplier = 100;
	સ્થિર constexpr hrt_abstime kTASScaleFastLearnTime = 300_s;

	તાર્કિક _wind_estimator_reset = ખોટું; ///< wind estimator was reset in this cycle

	// initialise state and state covariance matrix
	તાર્કિક initialise(અચલ matrix::Vector3f &velI, અચલ દશાંશ hor_vel_variance, અચલ દશાંશ heading_rad,
			અચલ દશાંશ tas_meas = NAN, અચલ દશાંશ tas_variance = NAN);

	શૂન્ય run_sanity_checks();

	// return the square of two floating point numbers
	સ્થિર constexpr દશાંશ sq(દશાંશ var) { પાછા var * var; }
};
