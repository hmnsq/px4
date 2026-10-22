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

#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>

#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>

#ಅನ್ತರ್ಭಾವ "python/generated/fuse_airspeed.h"
#ಅನ್ತರ್ಭಾವ "python/generated/fuse_beta.h"
#ಅನ್ತರ್ಭಾವ "python/generated/init_wind_using_airspeed.h"

using ನಾಮಾಕಾಶ time_literals;

ವರ್ಗ WindEstimator
{
ಸಾರ್ವಜನಿಕ:
	WindEstimator() = ಸ್ವಭಾವ;
	~WindEstimator() = ಸ್ವಭಾವ;

	// no copy, assignment, move, move assignment
	WindEstimator(ಧ್ರುವ WindEstimator &) = delete;
	WindEstimator &operator=(ಧ್ರುವ WindEstimator &) = delete;
	WindEstimator(WindEstimator &&) = delete;
	WindEstimator &operator=(WindEstimator &&) = delete;

	ಶೂನ್ಯ update(uint64_t time_now);

	ಶೂನ್ಯ fuse_airspeed(uint64_t time_now, ಭಿನ್ನ true_airspeed, ಧ್ರುವ matrix::Vector3f &velI,
			   ಧ್ರುವ ಭಿನ್ನ hor_vel_variance, ಧ್ರುವ matrix::Quatf &q_att);
	ಶೂನ್ಯ fuse_beta(uint64_t time_now, ಧ್ರುವ matrix::Vector3f &velI, ಧ್ರುವ ಭಿನ್ನ hor_vel_variance,
		       ಧ್ರುವ matrix::Quatf &q_att);

	ತಾರ್ಕಿಕ is_estimate_valid() { ಪ್ರತ್ಯಾಗಮ _initialised; }

	ತಾರ್ಕಿಕ check_if_meas_is_rejected(ಭಿನ್ನ innov, ಭಿನ್ನ innov_var, uint8_t gate_size);

	matrix::Vector2f get_wind() { ಪ್ರತ್ಯಾಗಮ matrix::Vector2f{_state(INDEX_W_N), _state(INDEX_W_E)}; }

	// invert scale (CAS = IAS * scale), protect agains division by 0, constrain to [0.1, 10]
	ಭಿನ್ನ get_tas_scale() { ಪ್ರತ್ಯಾಗಮ 1.f / math::constrain(_state(INDEX_TAS_SCALE), 0.1f, 10.0f); }
	ಭಿನ್ನ get_tas_scale_var() { ಪ್ರತ್ಯಾಗಮ _P(INDEX_TAS_SCALE, INDEX_TAS_SCALE); }
	ಭಿನ್ನ get_tas_innov() { ಪ್ರತ್ಯಾಗಮ _tas_innov; }
	ಭಿನ್ನ get_tas_innov_var() { ಪ್ರತ್ಯಾಗಮ _tas_innov_var; }
	ಭಿನ್ನ get_beta_innov() { ಪ್ರತ್ಯಾಗಮ _beta_innov; }
	ಭಿನ್ನ get_beta_innov_var() { ಪ್ರತ್ಯಾಗಮ _beta_innov_var; }
	matrix::Vector2f  get_wind_var() { ಪ್ರತ್ಯಾಗಮ matrix::Vector2f{_P(INDEX_W_N, INDEX_W_N), _P(INDEX_W_E, INDEX_W_E)}; }
	ತಾರ್ಕಿಕ get_wind_estimator_reset() { ಪ್ರತ್ಯಾಗಮ _wind_estimator_reset; }

	// unaided, the state uncertainty (diagonal of sqrt(P)) grows by the process noise spectral density every second
	ಶೂನ್ಯ set_wind_process_noise_spectral_density(ಭಿನ್ನ wind_nsd) { _wind_psd = wind_nsd * wind_nsd; }

	ಶೂನ್ಯ set_tas_noise(ಭಿನ್ನ tas_sigma) { _tas_var = tas_sigma * tas_sigma; }

	ಶೂನ್ಯ set_scale_init(ಭಿನ್ನ scale_init) {_scale_init = 1.f / math::constrain(scale_init, 0.1f, 10.f); }

	ಶೂನ್ಯ reset_scale_to_init()
	{
		_state(INDEX_TAS_SCALE) = _scale_init;
		auto P_wind = _P.slice<2, 2>(0, 0);
		_P.setZero();
		_P.slice<2, 2>(0, 0) = P_wind;
	}

ಗುಪ್ತ:
	ಗಣನಾ {
		INDEX_W_N = 0,
		INDEX_W_E,
		INDEX_TAS_SCALE
	};	///< enum which can be used to access state.

	ಸ್ಥಿರ constexpr ಭಿನ್ನ kInitialWindError =
		2.5f; // initial uncertainty of each wind state when filter is initialised without airspeed [m/s]
	ಸ್ಥಿರ constexpr ಭಿನ್ನ kInitialBetaErrorDeg =
		15.0f;	// sidelip uncertainty used to initialise the filter with a true airspeed measurement [deg]
	ಸ್ಥಿರ constexpr ಭಿನ್ನ kInitialHeadingErrorDeg =
		10.0f; // heading uncertainty used to initialise the filter with a true airspeed measurement [deg]

	matrix::Vector3f _state{0.f, 0.f, 1.f};
	matrix::Matrix3f _P;		///< state covariance matrix

	ಭಿನ್ನ _tas_innov{0.0f};	///< true airspeed innovation
	ಭಿನ್ನ _tas_innov_var{0.0f};	///< true airspeed innovation variance

	ಭಿನ್ನ _beta_innov{0.0f};	///< sideslip innovation
	ಭಿನ್ನ _beta_innov_var{0.0f};	///< sideslip innovation variance

	ತಾರ್ಕಿಕ _initialised{ಅಸತ್ಯ};	///< True: filter has been initialised

	ಭಿನ್ನ _wind_psd{0.01f};	///< wind process noise power spectral density (m^2/s^4/Hz), (0.1 m/s^2/sqrt(Hz))^2
	ಸ್ಥಿರ constexpr ಭಿನ್ನ kTasScalePsd{1e-8f};	///< true airspeed scale process noise power spectral density (1/s^2/Hz), (0.0001 1/s/sqrt(Hz))^2
	ಭಿನ್ನ _tas_var{1.96f};		///< true airspeed measurement noise variance (m/s)^2, (1.4 m/s)^2
	ಸ್ಥಿರ constexpr ಭಿನ್ನ kBetaVar{0.0225f};	///< sideslip measurement noise variance (rad)^2, (0.15 rad)^2
	ಸ್ಥಿರ constexpr uint8_t kTasGate{4};	///< airspeed fusion gate size (SD)
	ಸ್ಥಿರ constexpr uint8_t kBetaGate{1};	///< sideslip fusion gate size (SD)

	ಭಿನ್ನ _scale_init{1.f};

	uint64_t _time_last_airspeed_fuse = 0;	///< timestamp of last airspeed fusion
	uint64_t _time_last_beta_fuse = 0;	///< timestamp of last sideslip fusion
	uint64_t _time_last_update = 0;		///< timestamp of last covariance prediction
	uint64_t _time_initialised = 0;         ///< timestamp when estimator is initialised

	ಸ್ಥಿರ constexpr ಭಿನ್ನ kTASScalePSDMultiplier = 100;
	ಸ್ಥಿರ constexpr hrt_abstime kTASScaleFastLearnTime = 300_s;

	ತಾರ್ಕಿಕ _wind_estimator_reset = ಅಸತ್ಯ; ///< wind estimator was reset in this cycle

	// initialise state and state covariance matrix
	ತಾರ್ಕಿಕ initialise(ಧ್ರುವ matrix::Vector3f &velI, ಧ್ರುವ ಭಿನ್ನ hor_vel_variance, ಧ್ರುವ ಭಿನ್ನ heading_rad,
			ಧ್ರುವ ಭಿನ್ನ tas_meas = NAN, ಧ್ರುವ ಭಿನ್ನ tas_variance = NAN);

	ಶೂನ್ಯ run_sanity_checks();

	// return the square of two floating point numbers
	ಸ್ಥಿರ constexpr ಭಿನ್ನ sq(ಭಿನ್ನ var) { ಪ್ರತ್ಯಾಗಮ var * var; }
};
