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

#شامل <drivers/drv_hrt.h>

#شامل <mathlib/mathlib.h>
#شامل <matrix/math.hpp>

#شامل "python/generated/fuse_airspeed.h"
#شامل "python/generated/fuse_beta.h"
#شامل "python/generated/init_wind_using_airspeed.h"

using فضاینام time_literals;

رده WindEstimator
{
عمومی:
	WindEstimator() = پیشفرض;
	~WindEstimator() = پیشفرض;

	// no copy, assignment, move, move assignment
	WindEstimator(ثابت WindEstimator &) = delete;
	WindEstimator &operator=(ثابت WindEstimator &) = delete;
	WindEstimator(WindEstimator &&) = delete;
	WindEstimator &operator=(WindEstimator &&) = delete;

	تهی update(uint64_t time_now);

	تهی fuse_airspeed(uint64_t time_now, اعشاری true_airspeed, ثابت matrix::Vector3f &velI,
			   ثابت اعشاری hor_vel_variance, ثابت matrix::Quatf &q_att);
	تهی fuse_beta(uint64_t time_now, ثابت matrix::Vector3f &velI, ثابت اعشاری hor_vel_variance,
		       ثابت matrix::Quatf &q_att);

	منطقی is_estimate_valid() { بازگشت _initialised; }

	منطقی check_if_meas_is_rejected(اعشاری innov, اعشاری innov_var, uint8_t gate_size);

	matrix::Vector2f get_wind() { بازگشت matrix::Vector2f{_state(INDEX_W_N), _state(INDEX_W_E)}; }

	// invert scale (CAS = IAS * scale), protect agains division by 0, constrain to [0.1, 10]
	اعشاری get_tas_scale() { بازگشت 1.f / math::constrain(_state(INDEX_TAS_SCALE), 0.1f, 10.0f); }
	اعشاری get_tas_scale_var() { بازگشت _P(INDEX_TAS_SCALE, INDEX_TAS_SCALE); }
	اعشاری get_tas_innov() { بازگشت _tas_innov; }
	اعشاری get_tas_innov_var() { بازگشت _tas_innov_var; }
	اعشاری get_beta_innov() { بازگشت _beta_innov; }
	اعشاری get_beta_innov_var() { بازگشت _beta_innov_var; }
	matrix::Vector2f  get_wind_var() { بازگشت matrix::Vector2f{_P(INDEX_W_N, INDEX_W_N), _P(INDEX_W_E, INDEX_W_E)}; }
	منطقی get_wind_estimator_reset() { بازگشت _wind_estimator_reset; }

	// unaided, the state uncertainty (diagonal of sqrt(P)) grows by the process noise spectral density every second
	تهی set_wind_process_noise_spectral_density(اعشاری wind_nsd) { _wind_psd = wind_nsd * wind_nsd; }

	تهی set_tas_noise(اعشاری tas_sigma) { _tas_var = tas_sigma * tas_sigma; }

	تهی set_scale_init(اعشاری scale_init) {_scale_init = 1.f / math::constrain(scale_init, 0.1f, 10.f); }

	تهی reset_scale_to_init()
	{
		_state(INDEX_TAS_SCALE) = _scale_init;
		auto P_wind = _P.slice<2, 2>(0, 0);
		_P.setZero();
		_P.slice<2, 2>(0, 0) = P_wind;
	}

خصوصی:
	شمارش {
		INDEX_W_N = 0,
		INDEX_W_E,
		INDEX_TAS_SCALE
	};	///< enum which can be used to access state.

	ایستا constexpr اعشاری kInitialWindError =
		2.5f; // initial uncertainty of each wind state when filter is initialised without airspeed [m/s]
	ایستا constexpr اعشاری kInitialBetaErrorDeg =
		15.0f;	// sidelip uncertainty used to initialise the filter with a true airspeed measurement [deg]
	ایستا constexpr اعشاری kInitialHeadingErrorDeg =
		10.0f; // heading uncertainty used to initialise the filter with a true airspeed measurement [deg]

	matrix::Vector3f _state{0.f, 0.f, 1.f};
	matrix::Matrix3f _P;		///< state covariance matrix

	اعشاری _tas_innov{0.0f};	///< true airspeed innovation
	اعشاری _tas_innov_var{0.0f};	///< true airspeed innovation variance

	اعشاری _beta_innov{0.0f};	///< sideslip innovation
	اعشاری _beta_innov_var{0.0f};	///< sideslip innovation variance

	منطقی _initialised{نادرست};	///< True: filter has been initialised

	اعشاری _wind_psd{0.01f};	///< wind process noise power spectral density (m^2/s^4/Hz), (0.1 m/s^2/sqrt(Hz))^2
	ایستا constexpr اعشاری kTasScalePsd{1e-8f};	///< true airspeed scale process noise power spectral density (1/s^2/Hz), (0.0001 1/s/sqrt(Hz))^2
	اعشاری _tas_var{1.96f};		///< true airspeed measurement noise variance (m/s)^2, (1.4 m/s)^2
	ایستا constexpr اعشاری kBetaVar{0.0225f};	///< sideslip measurement noise variance (rad)^2, (0.15 rad)^2
	ایستا constexpr uint8_t kTasGate{4};	///< airspeed fusion gate size (SD)
	ایستا constexpr uint8_t kBetaGate{1};	///< sideslip fusion gate size (SD)

	اعشاری _scale_init{1.f};

	uint64_t _time_last_airspeed_fuse = 0;	///< timestamp of last airspeed fusion
	uint64_t _time_last_beta_fuse = 0;	///< timestamp of last sideslip fusion
	uint64_t _time_last_update = 0;		///< timestamp of last covariance prediction
	uint64_t _time_initialised = 0;         ///< timestamp when estimator is initialised

	ایستا constexpr اعشاری kTASScalePSDMultiplier = 100;
	ایستا constexpr hrt_abstime kTASScaleFastLearnTime = 300_s;

	منطقی _wind_estimator_reset = نادرست; ///< wind estimator was reset in this cycle

	// initialise state and state covariance matrix
	منطقی initialise(ثابت matrix::Vector3f &velI, ثابت اعشاری hor_vel_variance, ثابت اعشاری heading_rad,
			ثابت اعشاری tas_meas = NAN, ثابت اعشاری tas_variance = NAN);

	تهی run_sanity_checks();

	// return the square of two floating point numbers
	ایستا constexpr اعشاری sq(اعشاری var) { بازگشت var * var; }
};
