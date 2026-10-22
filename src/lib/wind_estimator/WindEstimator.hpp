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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/math.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ "python/generated/fuse_airspeed.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "python/generated/fuse_beta.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "python/generated/init_wind_using_airspeed.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

ਵਰਗ WindEstimator
{
ਜਨਤਕ:
	WindEstimator() = ਮੂਲ;
	~WindEstimator() = ਮੂਲ;

	// no copy, assignment, move, move assignment
	WindEstimator(ਅਚਲ WindEstimator &) = ਮਿਟਾਓ;
	WindEstimator &ਸੰਚਾਲਕ=(ਅਚਲ WindEstimator &) = ਮਿਟਾਓ;
	WindEstimator(WindEstimator &&) = ਮਿਟਾਓ;
	WindEstimator &ਸੰਚਾਲਕ=(WindEstimator &&) = ਮਿਟਾਓ;

	ਖਾਲੀ update(uint64_t ਸਮਾਂ_now);

	ਖਾਲੀ fuse_airspeed(uint64_t ਸਮਾਂ_now, ਦਸ਼ਮਲਵ true_airspeed, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &velI,
			   ਅਚਲ ਦਸ਼ਮਲਵ hor_vel_variance, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Quatf &q_att);
	ਖਾਲੀ fuse_beta(uint64_t ਸਮਾਂ_now, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &velI, ਅਚਲ ਦਸ਼ਮਲਵ hor_vel_variance,
		       ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Quatf &q_att);

	ਤਾਰਕਿਕ is_estimate_valid() { ਵਾਪਸ _initialised; }

	ਤਾਰਕਿਕ check_if_meas_is_rejected(ਦਸ਼ਮਲਵ innov, ਦਸ਼ਮਲਵ innov_var, uint8_t gate_ਮਾਪ);

	ਮੈਟ੍ਰਿਕਸ::Vector2f get_wind() { ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ::Vector2f{_state(ਸੂਚਕਾਂਕ_W_N), _state(ਸੂਚਕਾਂਕ_W_E)}; }

	// invert scale (CAS = IAS * scale), protect agains division by 0, constrain to [0.1, 10]
	ਦਸ਼ਮਲਵ get_tas_scale() { ਵਾਪਸ 1.f / math::constrain(_state(ਸੂਚਕਾਂਕ_TAS_SCALE), 0.1f, 10.0f); }
	ਦਸ਼ਮਲਵ get_tas_scale_var() { ਵਾਪਸ _P(ਸੂਚਕਾਂਕ_TAS_SCALE, ਸੂਚਕਾਂਕ_TAS_SCALE); }
	ਦਸ਼ਮਲਵ get_tas_innov() { ਵਾਪਸ _tas_innov; }
	ਦਸ਼ਮਲਵ get_tas_innov_var() { ਵਾਪਸ _tas_innov_var; }
	ਦਸ਼ਮਲਵ get_beta_innov() { ਵਾਪਸ _beta_innov; }
	ਦਸ਼ਮਲਵ get_beta_innov_var() { ਵਾਪਸ _beta_innov_var; }
	ਮੈਟ੍ਰਿਕਸ::Vector2f  get_wind_var() { ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ::Vector2f{_P(ਸੂਚਕਾਂਕ_W_N, ਸੂਚਕਾਂਕ_W_N), _P(ਸੂਚਕਾਂਕ_W_E, ਸੂਚਕਾਂਕ_W_E)}; }
	ਤਾਰਕਿਕ get_wind_estimator_reset() { ਵਾਪਸ _wind_estimator_reset; }

	// unaided, the state uncertainty (diagonal of sqrt(P)) grows by the process noise spectral density every second
	ਖਾਲੀ ਸੈੱਟ_wind_process_noise_spectral_density(ਦਸ਼ਮਲਵ wind_nsd) { _wind_psd = wind_nsd * wind_nsd; }

	ਖਾਲੀ ਸੈੱਟ_tas_noise(ਦਸ਼ਮਲਵ tas_sigma) { _tas_var = tas_sigma * tas_sigma; }

	ਖਾਲੀ ਸੈੱਟ_scale_init(ਦਸ਼ਮਲਵ scale_init) {_scale_init = 1.f / math::constrain(scale_init, 0.1f, 10.f); }

	ਖਾਲੀ reset_scale_ਮੰਜ਼ਿਲ_init()
	{
		_state(ਸੂਚਕਾਂਕ_TAS_SCALE) = _scale_init;
		ਸਵੈਚਾਲਿਤ P_wind = _P.ਟੁਕੜਾ<2, 2>(0, 0);
		_P.ਸੈੱਟ_Zero();
		_P.ਟੁਕੜਾ<2, 2>(0, 0) = P_wind;
	}

ਨਿਜੀ:
	ਗਿਣਤੀ {
		ਸੂਚਕਾਂਕ_W_N = 0,
		ਸੂਚਕਾਂਕ_W_E,
		ਸੂਚਕਾਂਕ_TAS_SCALE
	};	///< enum which can be used to access state.

	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ kInitialWindError =
		2.5f; // initial uncertainty of each wind state when filter is initialised without airspeed [m/s]
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ kInitialBetaErrorDeg =
		15.0f;	// sidelip uncertainty used to initialise the filter with a true airspeed measurement [deg]
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ kInitialHeadingErrorDeg =
		10.0f; // heading uncertainty used to initialise the filter with a true airspeed measurement [deg]

	ਮੈਟ੍ਰਿਕਸ::Vector3f _state{0.f, 0.f, 1.f};
	ਮੈਟ੍ਰਿਕਸ::Matrix3f _P;		///< state covariance matrix

	ਦਸ਼ਮਲਵ _tas_innov{0.0f};	///< true airspeed innovation
	ਦਸ਼ਮਲਵ _tas_innov_var{0.0f};	///< true airspeed innovation variance

	ਦਸ਼ਮਲਵ _beta_innov{0.0f};	///< sideslip innovation
	ਦਸ਼ਮਲਵ _beta_innov_var{0.0f};	///< sideslip innovation variance

	ਤਾਰਕਿਕ _initialised{ਝੂਠ};	///< True: filter has been initialised

	ਦਸ਼ਮਲਵ _wind_psd{0.01f};	///< wind process noise power spectral density (m^2/s^4/Hz), (0.1 m/s^2/sqrt(Hz))^2
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ kTasScalePsd{1e-8f};	///< true airspeed scale process noise power spectral density (1/s^2/Hz), (0.0001 1/s/sqrt(Hz))^2
	ਦਸ਼ਮਲਵ _tas_var{1.96f};		///< true airspeed measurement noise variance (m/s)^2, (1.4 m/s)^2
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ kBetaVar{0.0225f};	///< sideslip measurement noise variance (rad)^2, (0.15 rad)^2
	ਸਥਿਰ constexpr uint8_t kTasGate{4};	///< airspeed fusion gate size (SD)
	ਸਥਿਰ constexpr uint8_t kBetaGate{1};	///< sideslip fusion gate size (SD)

	ਦਸ਼ਮਲਵ _scale_init{1.f};

	uint64_t ਸਮਾਂ_ਆਖਰੀ_airspeed_fuse = 0;	///< timestamp of last airspeed fusion
	uint64_t ਸਮਾਂ_ਆਖਰੀ_beta_fuse = 0;	///< timestamp of last sideslip fusion
	uint64_t ਸਮਾਂ_ਆਖਰੀ_update = 0;		///< timestamp of last covariance prediction
	uint64_t ਸਮਾਂ_initialised = 0;         ///< timestamp when estimator is initialised

	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ kTASScalePSDMultiplier = 100;
	ਸਥਿਰ constexpr hrt_abstime k_TASScale_Fast_Learn_ਸਮਾਂ = 300_s;

	ਤਾਰਕਿਕ _wind_estimator_reset = ਝੂਠ; ///< wind estimator was reset in this cycle

	// initialise state and state covariance matrix
	ਤਾਰਕਿਕ initialise(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &velI, ਅਚਲ ਦਸ਼ਮਲਵ hor_vel_variance, ਅਚਲ ਦਸ਼ਮਲਵ heading_rad,
			ਅਚਲ ਦਸ਼ਮਲਵ tas_meas = NAN, ਅਚਲ ਦਸ਼ਮਲਵ tas_variance = NAN);

	ਖਾਲੀ ਲੜੀਵਾਰ_sanity_checks();

	// return the square of two floating point numbers
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ sq(ਦਸ਼ਮਲਵ var) { ਵਾਪਸ var * var; }
};
