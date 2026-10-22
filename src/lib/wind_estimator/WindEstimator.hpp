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

#അന്തര്ഭാവ <drivers/drv_hrt.h>

#അന്തര്ഭാവ <mathlib/mathlib.h>
#അന്തര്ഭാവ <matrix/math.hpp>

#അന്തര്ഭാവ "python/generated/fuse_airspeed.h"
#അന്തര്ഭാവ "python/generated/fuse_beta.h"
#അന്തര്ഭാവ "python/generated/init_wind_using_airspeed.h"

using നാമാകാശ time_literals;

വർഗ്ഗം WindEstimator
{
പൊതു:
	WindEstimator() = സ്ഥിരസ്ഥിതി;
	~WindEstimator() = സ്ഥിരസ്ഥിതി;

	// no copy, assignment, move, move assignment
	WindEstimator(സ്ഥിരം WindEstimator &) = delete;
	WindEstimator &operator=(സ്ഥിരം WindEstimator &) = delete;
	WindEstimator(WindEstimator &&) = delete;
	WindEstimator &operator=(WindEstimator &&) = delete;

	ശൂന്യം update(uint64_t time_now);

	ശൂന്യം fuse_airspeed(uint64_t time_now, ദശാംശം true_airspeed, സ്ഥിരം matrix::Vector3f &velI,
			   സ്ഥിരം ദശാംശം hor_vel_variance, സ്ഥിരം matrix::Quatf &q_att);
	ശൂന്യം fuse_beta(uint64_t time_now, സ്ഥിരം matrix::Vector3f &velI, സ്ഥിരം ദശാംശം hor_vel_variance,
		       സ്ഥിരം matrix::Quatf &q_att);

	തർക്കം is_estimate_valid() { തിരികെ _initialised; }

	തർക്കം check_if_meas_is_rejected(ദശാംശം innov, ദശാംശം innov_var, uint8_t gate_size);

	matrix::Vector2f get_wind() { തിരികെ matrix::Vector2f{_state(INDEX_W_N), _state(INDEX_W_E)}; }

	// invert scale (CAS = IAS * scale), protect agains division by 0, constrain to [0.1, 10]
	ദശാംശം get_tas_scale() { തിരികെ 1.f / math::constrain(_state(INDEX_TAS_SCALE), 0.1f, 10.0f); }
	ദശാംശം get_tas_scale_var() { തിരികെ _P(INDEX_TAS_SCALE, INDEX_TAS_SCALE); }
	ദശാംശം get_tas_innov() { തിരികെ _tas_innov; }
	ദശാംശം get_tas_innov_var() { തിരികെ _tas_innov_var; }
	ദശാംശം get_beta_innov() { തിരികെ _beta_innov; }
	ദശാംശം get_beta_innov_var() { തിരികെ _beta_innov_var; }
	matrix::Vector2f  get_wind_var() { തിരികെ matrix::Vector2f{_P(INDEX_W_N, INDEX_W_N), _P(INDEX_W_E, INDEX_W_E)}; }
	തർക്കം get_wind_estimator_reset() { തിരികെ _wind_estimator_reset; }

	// unaided, the state uncertainty (diagonal of sqrt(P)) grows by the process noise spectral density every second
	ശൂന്യം set_wind_process_noise_spectral_density(ദശാംശം wind_nsd) { _wind_psd = wind_nsd * wind_nsd; }

	ശൂന്യം set_tas_noise(ദശാംശം tas_sigma) { _tas_var = tas_sigma * tas_sigma; }

	ശൂന്യം set_scale_init(ദശാംശം scale_init) {_scale_init = 1.f / math::constrain(scale_init, 0.1f, 10.f); }

	ശൂന്യം reset_scale_to_init()
	{
		_state(INDEX_TAS_SCALE) = _scale_init;
		auto P_wind = _P.slice<2, 2>(0, 0);
		_P.setZero();
		_P.slice<2, 2>(0, 0) = P_wind;
	}

സ്വകാര്യ:
	എണ്ണൽ {
		INDEX_W_N = 0,
		INDEX_W_E,
		INDEX_TAS_SCALE
	};	///< enum which can be used to access state.

	സ്ഥിതം constexpr ദശാംശം kInitialWindError =
		2.5f; // initial uncertainty of each wind state when filter is initialised without airspeed [m/s]
	സ്ഥിതം constexpr ദശാംശം kInitialBetaErrorDeg =
		15.0f;	// sidelip uncertainty used to initialise the filter with a true airspeed measurement [deg]
	സ്ഥിതം constexpr ദശാംശം kInitialHeadingErrorDeg =
		10.0f; // heading uncertainty used to initialise the filter with a true airspeed measurement [deg]

	matrix::Vector3f _state{0.f, 0.f, 1.f};
	matrix::Matrix3f _P;		///< state covariance matrix

	ദശാംശം _tas_innov{0.0f};	///< true airspeed innovation
	ദശാംശം _tas_innov_var{0.0f};	///< true airspeed innovation variance

	ദശാംശം _beta_innov{0.0f};	///< sideslip innovation
	ദശാംശം _beta_innov_var{0.0f};	///< sideslip innovation variance

	തർക്കം _initialised{അസത്യം};	///< True: filter has been initialised

	ദശാംശം _wind_psd{0.01f};	///< wind process noise power spectral density (m^2/s^4/Hz), (0.1 m/s^2/sqrt(Hz))^2
	സ്ഥിതം constexpr ദശാംശം kTasScalePsd{1e-8f};	///< true airspeed scale process noise power spectral density (1/s^2/Hz), (0.0001 1/s/sqrt(Hz))^2
	ദശാംശം _tas_var{1.96f};		///< true airspeed measurement noise variance (m/s)^2, (1.4 m/s)^2
	സ്ഥിതം constexpr ദശാംശം kBetaVar{0.0225f};	///< sideslip measurement noise variance (rad)^2, (0.15 rad)^2
	സ്ഥിതം constexpr uint8_t kTasGate{4};	///< airspeed fusion gate size (SD)
	സ്ഥിതം constexpr uint8_t kBetaGate{1};	///< sideslip fusion gate size (SD)

	ദശാംശം _scale_init{1.f};

	uint64_t _time_last_airspeed_fuse = 0;	///< timestamp of last airspeed fusion
	uint64_t _time_last_beta_fuse = 0;	///< timestamp of last sideslip fusion
	uint64_t _time_last_update = 0;		///< timestamp of last covariance prediction
	uint64_t _time_initialised = 0;         ///< timestamp when estimator is initialised

	സ്ഥിതം constexpr ദശാംശം kTASScalePSDMultiplier = 100;
	സ്ഥിതം constexpr hrt_abstime kTASScaleFastLearnTime = 300_s;

	തർക്കം _wind_estimator_reset = അസത്യം; ///< wind estimator was reset in this cycle

	// initialise state and state covariance matrix
	തർക്കം initialise(സ്ഥിരം matrix::Vector3f &velI, സ്ഥിരം ദശാംശം hor_vel_variance, സ്ഥിരം ദശാംശം heading_rad,
			സ്ഥിരം ദശാംശം tas_meas = NAN, സ്ഥിരം ദശാംശം tas_variance = NAN);

	ശൂന്യം run_sanity_checks();

	// return the square of two floating point numbers
	സ്ഥിതം constexpr ദശാംശം sq(ദശാംശം var) { തിരികെ var * var; }
};
