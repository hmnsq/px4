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

#अन्तर्भाव <drivers/drv_hrt.h>

#अन्तर्भाव <mathlib/mathlib.h>
#अन्तर्भाव <matrix/math.hpp>

#अन्तर्भाव "python/generated/fuse_airspeed.h"
#अन्तर्भाव "python/generated/fuse_beta.h"
#अन्तर्भाव "python/generated/init_wind_using_airspeed.h"

using नामाकाश time_literals;

वर्ग WindEstimator
{
सार्वजनिक:
	WindEstimator() = पूर्वनिर्धारित;
	~WindEstimator() = पूर्वनिर्धारित;

	// no copy, assignment, move, move assignment
	WindEstimator(स्थिरांक WindEstimator &) = delete;
	WindEstimator &चालक=(स्थिरांक WindEstimator &) = delete;
	WindEstimator(WindEstimator &&) = delete;
	WindEstimator &चालक=(WindEstimator &&) = delete;

	शून्य update(uint64_t time_now);

	शून्य fuse_airspeed(uint64_t time_now, दशांश true_airspeed, स्थिरांक matrix::Vector3f &velI,
			   स्थिरांक दशांश hor_vel_variance, स्थिरांक matrix::Quatf &q_att);
	शून्य fuse_beta(uint64_t time_now, स्थिरांक matrix::Vector3f &velI, स्थिरांक दशांश hor_vel_variance,
		       स्थिरांक matrix::Quatf &q_att);

	तार्किक is_estimate_valid() { परत _initialised; }

	तार्किक check_if_meas_is_rejected(दशांश innov, दशांश innov_var, uint8_t gate_size);

	matrix::Vector2f get_wind() { परत matrix::Vector2f{_state(INDEX_W_N), _state(INDEX_W_E)}; }

	// invert scale (CAS = IAS * scale), protect agains division by 0, constrain to [0.1, 10]
	दशांश get_tas_scale() { परत 1.f / math::constrain(_state(INDEX_TAS_SCALE), 0.1f, 10.0f); }
	दशांश get_tas_scale_var() { परत _P(INDEX_TAS_SCALE, INDEX_TAS_SCALE); }
	दशांश get_tas_innov() { परत _tas_innov; }
	दशांश get_tas_innov_var() { परत _tas_innov_var; }
	दशांश get_beta_innov() { परत _beta_innov; }
	दशांश get_beta_innov_var() { परत _beta_innov_var; }
	matrix::Vector2f  get_wind_var() { परत matrix::Vector2f{_P(INDEX_W_N, INDEX_W_N), _P(INDEX_W_E, INDEX_W_E)}; }
	तार्किक get_wind_estimator_reset() { परत _wind_estimator_reset; }

	// unaided, the state uncertainty (diagonal of sqrt(P)) grows by the process noise spectral density every second
	शून्य set_wind_process_noise_spectral_density(दशांश wind_nsd) { _wind_psd = wind_nsd * wind_nsd; }

	शून्य set_tas_noise(दशांश tas_sigma) { _tas_var = tas_sigma * tas_sigma; }

	शून्य set_scale_init(दशांश scale_init) {_scale_init = 1.f / math::constrain(scale_init, 0.1f, 10.f); }

	शून्य reset_scale_to_init()
	{
		_state(INDEX_TAS_SCALE) = _scale_init;
		स्वतः P_wind = _P.slice<2, 2>(0, 0);
		_P.setZero();
		_P.slice<2, 2>(0, 0) = P_wind;
	}

खाजगी:
	गणना {
		INDEX_W_N = 0,
		INDEX_W_E,
		INDEX_TAS_SCALE
	};	///< enum which can be used to access state.

	स्थिर constexpr दशांश kInitialWindError =
		2.5f; // initial uncertainty of each wind state when filter is initialised without airspeed [m/s]
	स्थिर constexpr दशांश kInitialBetaErrorDeg =
		15.0f;	// sidelip uncertainty used to initialise the filter with a true airspeed measurement [deg]
	स्थिर constexpr दशांश kInitialHeadingErrorDeg =
		10.0f; // heading uncertainty used to initialise the filter with a true airspeed measurement [deg]

	matrix::Vector3f _state{0.f, 0.f, 1.f};
	matrix::Matrix3f _P;		///< state covariance matrix

	दशांश _tas_innov{0.0f};	///< true airspeed innovation
	दशांश _tas_innov_var{0.0f};	///< true airspeed innovation variance

	दशांश _beta_innov{0.0f};	///< sideslip innovation
	दशांश _beta_innov_var{0.0f};	///< sideslip innovation variance

	तार्किक _initialised{असत्य};	///< True: filter has been initialised

	दशांश _wind_psd{0.01f};	///< wind process noise power spectral density (m^2/s^4/Hz), (0.1 m/s^2/sqrt(Hz))^2
	स्थिर constexpr दशांश kTasScalePsd{1e-8f};	///< true airspeed scale process noise power spectral density (1/s^2/Hz), (0.0001 1/s/sqrt(Hz))^2
	दशांश _tas_var{1.96f};		///< true airspeed measurement noise variance (m/s)^2, (1.4 m/s)^2
	स्थिर constexpr दशांश kBetaVar{0.0225f};	///< sideslip measurement noise variance (rad)^2, (0.15 rad)^2
	स्थिर constexpr uint8_t kTasGate{4};	///< airspeed fusion gate size (SD)
	स्थिर constexpr uint8_t kBetaGate{1};	///< sideslip fusion gate size (SD)

	दशांश _scale_init{1.f};

	uint64_t _time_last_airspeed_fuse = 0;	///< timestamp of last airspeed fusion
	uint64_t _time_last_beta_fuse = 0;	///< timestamp of last sideslip fusion
	uint64_t _time_last_update = 0;		///< timestamp of last covariance prediction
	uint64_t _time_initialised = 0;         ///< timestamp when estimator is initialised

	स्थिर constexpr दशांश kTASScalePSDMultiplier = 100;
	स्थिर constexpr hrt_abstime kTASScaleFastLearnTime = 300_s;

	तार्किक _wind_estimator_reset = असत्य; ///< wind estimator was reset in this cycle

	// initialise state and state covariance matrix
	तार्किक initialise(स्थिरांक matrix::Vector3f &velI, स्थिरांक दशांश hor_vel_variance, स्थिरांक दशांश heading_rad,
			स्थिरांक दशांश tas_meas = न_अंक, स्थिरांक दशांश tas_variance = न_अंक);

	शून्य run_sanity_checks();

	// return the square of two floating point numbers
	स्थिर constexpr दशांश sq(दशांश var) { परत var * var; }
};
