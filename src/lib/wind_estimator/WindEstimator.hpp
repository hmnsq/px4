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

#অন্তর্ভাৱ <drivers/drv_hrt.h>

#অন্তর্ভাৱ <mathlib/mathlib.h>
#অন্তর্ভাৱ <matrix/math.hpp>

#অন্তর্ভাৱ "python/generated/fuse_airspeed.h"
#অন্তর্ভাৱ "python/generated/fuse_beta.h"
#অন্তর্ভাৱ "python/generated/init_wind_using_airspeed.h"

using নামাকাশ time_literals;

শ্ৰেণী WindEstimator
{
সার্ৱজনিক:
	WindEstimator() = স্ৱভাৱ;
	~WindEstimator() = স্ৱভাৱ;

	// no copy, assignment, move, move assignment
	WindEstimator(ধ্রুৱ WindEstimator &) = delete;
	WindEstimator &operator=(ধ্রুৱ WindEstimator &) = delete;
	WindEstimator(WindEstimator &&) = delete;
	WindEstimator &operator=(WindEstimator &&) = delete;

	শূন্য update(uint64_t time_now);

	শূন্য fuse_airspeed(uint64_t time_now, ভিন্ন true_airspeed, ধ্রুৱ matrix::Vector3f &velI,
			   ধ্রুৱ ভিন্ন hor_vel_variance, ধ্রুৱ matrix::Quatf &q_att);
	শূন্য fuse_beta(uint64_t time_now, ধ্রুৱ matrix::Vector3f &velI, ধ্রুৱ ভিন্ন hor_vel_variance,
		       ধ্রুৱ matrix::Quatf &q_att);

	তার্কিক is_estimate_valid() { ঘূৰাই _initialised; }

	তার্কিক check_if_meas_is_rejected(ভিন্ন innov, ভিন্ন innov_var, uint8_t gate_size);

	matrix::Vector2f get_wind() { ঘূৰাই matrix::Vector2f{_state(INDEX_W_N), _state(INDEX_W_E)}; }

	// invert scale (CAS = IAS * scale), protect agains division by 0, constrain to [0.1, 10]
	ভিন্ন get_tas_scale() { ঘূৰাই 1.f / math::constrain(_state(INDEX_TAS_SCALE), 0.1f, 10.0f); }
	ভিন্ন get_tas_scale_var() { ঘূৰাই _P(INDEX_TAS_SCALE, INDEX_TAS_SCALE); }
	ভিন্ন get_tas_innov() { ঘূৰাই _tas_innov; }
	ভিন্ন get_tas_innov_var() { ঘূৰাই _tas_innov_var; }
	ভিন্ন get_beta_innov() { ঘূৰাই _beta_innov; }
	ভিন্ন get_beta_innov_var() { ঘূৰাই _beta_innov_var; }
	matrix::Vector2f  get_wind_var() { ঘূৰাই matrix::Vector2f{_P(INDEX_W_N, INDEX_W_N), _P(INDEX_W_E, INDEX_W_E)}; }
	তার্কিক get_wind_estimator_reset() { ঘূৰাই _wind_estimator_reset; }

	// unaided, the state uncertainty (diagonal of sqrt(P)) grows by the process noise spectral density every second
	শূন্য set_wind_process_noise_spectral_density(ভিন্ন wind_nsd) { _wind_psd = wind_nsd * wind_nsd; }

	শূন্য set_tas_noise(ভিন্ন tas_sigma) { _tas_var = tas_sigma * tas_sigma; }

	শূন্য set_scale_init(ভিন্ন scale_init) {_scale_init = 1.f / math::constrain(scale_init, 0.1f, 10.f); }

	শূন্য reset_scale_to_init()
	{
		_state(INDEX_TAS_SCALE) = _scale_init;
		auto P_wind = _P.slice<2, 2>(0, 0);
		_P.setZero();
		_P.slice<2, 2>(0, 0) = P_wind;
	}

গুপ্ত:
	গণনা {
		INDEX_W_N = 0,
		INDEX_W_E,
		INDEX_TAS_SCALE
	};	///< enum which can be used to access state.

	স্থির constexpr ভিন্ন kInitialWindError =
		2.5f; // initial uncertainty of each wind state when filter is initialised without airspeed [m/s]
	স্থির constexpr ভিন্ন kInitialBetaErrorDeg =
		15.0f;	// sidelip uncertainty used to initialise the filter with a true airspeed measurement [deg]
	স্থির constexpr ভিন্ন kInitialHeadingErrorDeg =
		10.0f; // heading uncertainty used to initialise the filter with a true airspeed measurement [deg]

	matrix::Vector3f _state{0.f, 0.f, 1.f};
	matrix::Matrix3f _P;		///< state covariance matrix

	ভিন্ন _tas_innov{0.0f};	///< true airspeed innovation
	ভিন্ন _tas_innov_var{0.0f};	///< true airspeed innovation variance

	ভিন্ন _beta_innov{0.0f};	///< sideslip innovation
	ভিন্ন _beta_innov_var{0.0f};	///< sideslip innovation variance

	তার্কিক _initialised{মিছা};	///< True: filter has been initialised

	ভিন্ন _wind_psd{0.01f};	///< wind process noise power spectral density (m^2/s^4/Hz), (0.1 m/s^2/sqrt(Hz))^2
	স্থির constexpr ভিন্ন kTasScalePsd{1e-8f};	///< true airspeed scale process noise power spectral density (1/s^2/Hz), (0.0001 1/s/sqrt(Hz))^2
	ভিন্ন _tas_var{1.96f};		///< true airspeed measurement noise variance (m/s)^2, (1.4 m/s)^2
	স্থির constexpr ভিন্ন kBetaVar{0.0225f};	///< sideslip measurement noise variance (rad)^2, (0.15 rad)^2
	স্থির constexpr uint8_t kTasGate{4};	///< airspeed fusion gate size (SD)
	স্থির constexpr uint8_t kBetaGate{1};	///< sideslip fusion gate size (SD)

	ভিন্ন _scale_init{1.f};

	uint64_t _time_last_airspeed_fuse = 0;	///< timestamp of last airspeed fusion
	uint64_t _time_last_beta_fuse = 0;	///< timestamp of last sideslip fusion
	uint64_t _time_last_update = 0;		///< timestamp of last covariance prediction
	uint64_t _time_initialised = 0;         ///< timestamp when estimator is initialised

	স্থির constexpr ভিন্ন kTASScalePSDMultiplier = 100;
	স্থির constexpr hrt_abstime kTASScaleFastLearnTime = 300_s;

	তার্কিক _wind_estimator_reset = মিছা; ///< wind estimator was reset in this cycle

	// initialise state and state covariance matrix
	তার্কিক initialise(ধ্রুৱ matrix::Vector3f &velI, ধ্রুৱ ভিন্ন hor_vel_variance, ধ্রুৱ ভিন্ন heading_rad,
			ধ্রুৱ ভিন্ন tas_meas = NAN, ধ্রুৱ ভিন্ন tas_variance = NAN);

	শূন্য run_sanity_checks();

	// return the square of two floating point numbers
	স্থির constexpr ভিন্ন sq(ভিন্ন var) { ঘূৰাই var * var; }
};
