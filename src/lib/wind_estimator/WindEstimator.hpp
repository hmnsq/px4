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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>

#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>

#ಅನ್ತರ್ಭಾವ "python/generated/fuse_airspeed.h"
#ಅನ್ತರ್ಭಾವ "python/generated/fuse_beta.h"
#ಅನ್ತರ್ಭಾವ "python/generated/init_wind_using_airspeed.h"

ಬಳಸಿ ನಾಮಾಕಾಶ ಸಮಯ_literals;

ವರ್ಗ WindEstimator
{
ಸಾರ್ವಜನಿಕ:
	WindEstimator() = ಪೂರ್ವನಿಯೋಜಿತ;
	~WindEstimator() = ಪೂರ್ವನಿಯೋಜಿತ;

	// no copy, assignment, move, move assignment
	WindEstimator(ಸ್ಥಿರಾಂಕ WindEstimator &) = ಅಳಿಸು;
	WindEstimator &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ WindEstimator &) = ಅಳಿಸು;
	WindEstimator(WindEstimator &&) = ಅಳಿಸು;
	WindEstimator &ಕ್ರಿಯಾಕಾರಕ=(WindEstimator &&) = ಅಳಿಸು;

	ಶೂನ್ಯ update(uint64_t ಸಮಯ_now);

	ಶೂನ್ಯ fuse_airspeed(uint64_t ಸಮಯ_now, ದಶಮಾಂಶ true_airspeed, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &velI,
			   ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ hor_vel_variance, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Quatf &q_att);
	ಶೂನ್ಯ fuse_beta(uint64_t ಸಮಯ_now, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &velI, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ hor_vel_variance,
		       ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Quatf &q_att);

	ತಾರ್ಕಿಕ is_estimate_valid() { ಹಿಂತಿರುಗಿ _initialised; }

	ತಾರ್ಕಿಕ check_if_meas_is_rejected(ದಶಮಾಂಶ innov, ದಶಮಾಂಶ innov_var, uint8_t gate_ಗಾತ್ರ);

	ಮಾತೃಕೆ::Vector2f get_wind() { ಹಿಂತಿರುಗಿ ಮಾತೃಕೆ::Vector2f{_state(ಸೂಚ್ಯಂಕ_W_N), _state(ಸೂಚ್ಯಂಕ_W_E)}; }

	// invert scale (CAS = IAS * scale), protect agains division by 0, constrain to [0.1, 10]
	ದಶಮಾಂಶ get_tas_scale() { ಹಿಂತಿರುಗಿ 1.f / math::constrain(_state(ಸೂಚ್ಯಂಕ_TAS_SCALE), 0.1f, 10.0f); }
	ದಶಮಾಂಶ get_tas_scale_var() { ಹಿಂತಿರುಗಿ _P(ಸೂಚ್ಯಂಕ_TAS_SCALE, ಸೂಚ್ಯಂಕ_TAS_SCALE); }
	ದಶಮಾಂಶ get_tas_innov() { ಹಿಂತಿರುಗಿ _tas_innov; }
	ದಶಮಾಂಶ get_tas_innov_var() { ಹಿಂತಿರುಗಿ _tas_innov_var; }
	ದಶಮಾಂಶ get_beta_innov() { ಹಿಂತಿರುಗಿ _beta_innov; }
	ದಶಮಾಂಶ get_beta_innov_var() { ಹಿಂತಿರುಗಿ _beta_innov_var; }
	ಮಾತೃಕೆ::Vector2f  get_wind_var() { ಹಿಂತಿರುಗಿ ಮಾತೃಕೆ::Vector2f{_P(ಸೂಚ್ಯಂಕ_W_N, ಸೂಚ್ಯಂಕ_W_N), _P(ಸೂಚ್ಯಂಕ_W_E, ಸೂಚ್ಯಂಕ_W_E)}; }
	ತಾರ್ಕಿಕ get_wind_estimator_reset() { ಹಿಂತಿರುಗಿ _wind_estimator_reset; }

	// unaided, the state uncertainty (diagonal of sqrt(P)) grows by the process noise spectral density every second
	ಶೂನ್ಯ ಗಣ_wind_process_noise_spectral_density(ದಶಮಾಂಶ wind_nsd) { _wind_psd = wind_nsd * wind_nsd; }

	ಶೂನ್ಯ ಗಣ_tas_noise(ದಶಮಾಂಶ tas_sigma) { _tas_var = tas_sigma * tas_sigma; }

	ಶೂನ್ಯ ಗಣ_scale_init(ದಶಮಾಂಶ scale_init) {_scale_init = 1.f / math::constrain(scale_init, 0.1f, 10.f); }

	ಶೂನ್ಯ reset_scale_ಗಮ್ಯ_init()
	{
		_state(ಸೂಚ್ಯಂಕ_TAS_SCALE) = _scale_init;
		ಸ್ವಯಂಚಾಲಿತ P_wind = _P.ತುಂಡು<2, 2>(0, 0);
		_P.ಗಣ_Zero();
		_P.ತುಂಡು<2, 2>(0, 0) = P_wind;
	}

ಖಾಸಗಿ:
	ಎಣಿಕೆ {
		ಸೂಚ್ಯಂಕ_W_N = 0,
		ಸೂಚ್ಯಂಕ_W_E,
		ಸೂಚ್ಯಂಕ_TAS_SCALE
	};	///< enum which can be used to access state.

	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ kInitialWindError =
		2.5f; // initial uncertainty of each wind state when filter is initialised without airspeed [m/s]
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ kInitialBetaErrorDeg =
		15.0f;	// sidelip uncertainty used to initialise the filter with a true airspeed measurement [deg]
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ kInitialHeadingErrorDeg =
		10.0f; // heading uncertainty used to initialise the filter with a true airspeed measurement [deg]

	ಮಾತೃಕೆ::Vector3f _state{0.f, 0.f, 1.f};
	ಮಾತೃಕೆ::Matrix3f _P;		///< state covariance matrix

	ದಶಮಾಂಶ _tas_innov{0.0f};	///< true airspeed innovation
	ದಶಮಾಂಶ _tas_innov_var{0.0f};	///< true airspeed innovation variance

	ದಶಮಾಂಶ _beta_innov{0.0f};	///< sideslip innovation
	ದಶಮಾಂಶ _beta_innov_var{0.0f};	///< sideslip innovation variance

	ತಾರ್ಕಿಕ _initialised{ಸುಳ್ಳು};	///< True: filter has been initialised

	ದಶಮಾಂಶ _wind_psd{0.01f};	///< wind process noise power spectral density (m^2/s^4/Hz), (0.1 m/s^2/sqrt(Hz))^2
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ kTasScalePsd{1e-8f};	///< true airspeed scale process noise power spectral density (1/s^2/Hz), (0.0001 1/s/sqrt(Hz))^2
	ದಶಮಾಂಶ _tas_var{1.96f};		///< true airspeed measurement noise variance (m/s)^2, (1.4 m/s)^2
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ kBetaVar{0.0225f};	///< sideslip measurement noise variance (rad)^2, (0.15 rad)^2
	ಸ್ಥಿರ constexpr uint8_t kTasGate{4};	///< airspeed fusion gate size (SD)
	ಸ್ಥಿರ constexpr uint8_t kBetaGate{1};	///< sideslip fusion gate size (SD)

	ದಶಮಾಂಶ _scale_init{1.f};

	uint64_t ಸಮಯ_ಕೊನೆಯ_airspeed_fuse = 0;	///< timestamp of last airspeed fusion
	uint64_t ಸಮಯ_ಕೊನೆಯ_beta_fuse = 0;	///< timestamp of last sideslip fusion
	uint64_t ಸಮಯ_ಕೊನೆಯ_update = 0;		///< timestamp of last covariance prediction
	uint64_t ಸಮಯ_initialised = 0;         ///< timestamp when estimator is initialised

	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ kTASScalePSDMultiplier = 100;
	ಸ್ಥಿರ constexpr hrt_abstime k_TASScale_Fast_Learn_ಸಮಯ = 300_s;

	ತಾರ್ಕಿಕ _wind_estimator_reset = ಸುಳ್ಳು; ///< wind estimator was reset in this cycle

	// initialise state and state covariance matrix
	ತಾರ್ಕಿಕ initialise(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &velI, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ hor_vel_variance, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_rad,
			ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ tas_meas = NAN, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ tas_variance = NAN);

	ಶೂನ್ಯ ಸತತ_sanity_checks();

	// return the square of two floating point numbers
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ sq(ದಶಮಾಂಶ var) { ಹಿಂತಿರುಗಿ var * var; }
};
