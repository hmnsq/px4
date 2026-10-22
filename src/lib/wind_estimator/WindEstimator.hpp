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

#رهنمود once

#ضمن <drivers/drv_hrt.h>

#ضمن <mathlib/mathlib.h>
#ضمن <matrix/math.hpp>

#ضمن "python/generated/fuse_airspeed.h"
#ضمن "python/generated/fuse_beta.h"
#ضمن "python/generated/init_wind_using_airspeed.h"

استفاده نطاق زمان_literals;

صنف WindEstimator
{
عام:
	WindEstimator() = افتراضي;
	~WindEstimator() = افتراضي;

	// no copy, assignment, move, move assignment
	WindEstimator(ثابت WindEstimator &) = حذف;
	WindEstimator &عملگر=(ثابت WindEstimator &) = حذف;
	WindEstimator(WindEstimator &&) = حذف;
	WindEstimator &عملگر=(WindEstimator &&) = حذف;

	فراغ update(uint64_t زمان_now);

	فراغ fuse_airspeed(uint64_t زمان_now, عائم true_airspeed, ثابت ماتریس::Vector3f &velI,
			   ثابت عائم hor_vel_variance, ثابت ماتریس::Quatf &q_att);
	فراغ fuse_beta(uint64_t زمان_now, ثابت ماتریس::Vector3f &velI, ثابت عائم hor_vel_variance,
		       ثابت ماتریس::Quatf &q_att);

	منطقي is_estimate_valid() { أرجع _initialised; }

	منطقي check_if_meas_is_rejected(عائم innov, عائم innov_var, uint8_t gate_اندازه);

	ماتریس::Vector2f get_wind() { أرجع ماتریس::Vector2f{_state(شاخص_W_N), _state(شاخص_W_E)}; }

	// invert scale (CAS = IAS * scale), protect agains division by 0, constrain to [0.1, 10]
	عائم get_tas_scale() { أرجع 1.f / math::constrain(_state(شاخص_TAS_SCALE), 0.1f, 10.0f); }
	عائم get_tas_scale_var() { أرجع _P(شاخص_TAS_SCALE, شاخص_TAS_SCALE); }
	عائم get_tas_innov() { أرجع _tas_innov; }
	عائم get_tas_innov_var() { أرجع _tas_innov_var; }
	عائم get_beta_innov() { أرجع _beta_innov; }
	عائم get_beta_innov_var() { أرجع _beta_innov_var; }
	ماتریس::Vector2f  get_wind_var() { أرجع ماتریس::Vector2f{_P(شاخص_W_N, شاخص_W_N), _P(شاخص_W_E, شاخص_W_E)}; }
	منطقي get_wind_estimator_reset() { أرجع _wind_estimator_reset; }

	// unaided, the state uncertainty (diagonal of sqrt(P)) grows by the process noise spectral density every second
	فراغ مجموعه_wind_process_noise_spectral_density(عائم wind_nsd) { _wind_psd = wind_nsd * wind_nsd; }

	فراغ مجموعه_tas_noise(عائم tas_sigma) { _tas_var = tas_sigma * tas_sigma; }

	فراغ مجموعه_scale_init(عائم scale_init) {_scale_init = 1.f / math::constrain(scale_init, 0.1f, 10.f); }

	فراغ reset_scale_مقصد_init()
	{
		_state(شاخص_TAS_SCALE) = _scale_init;
		خودکار P_wind = _P.برش<2, 2>(0, 0);
		_P.مجموعه_Zero();
		_P.برش<2, 2>(0, 0) = P_wind;
	}

خاص:
	تعداد {
		شاخص_W_N = 0,
		شاخص_W_E,
		شاخص_TAS_SCALE
	};	///< enum which can be used to access state.

	ساكن constexpr عائم kInitialWindError =
		2.5f; // initial uncertainty of each wind state when filter is initialised without airspeed [m/s]
	ساكن constexpr عائم kInitialBetaErrorDeg =
		15.0f;	// sidelip uncertainty used to initialise the filter with a true airspeed measurement [deg]
	ساكن constexpr عائم kInitialHeadingErrorDeg =
		10.0f; // heading uncertainty used to initialise the filter with a true airspeed measurement [deg]

	ماتریس::Vector3f _state{0.f, 0.f, 1.f};
	ماتریس::Matrix3f _P;		///< state covariance matrix

	عائم _tas_innov{0.0f};	///< true airspeed innovation
	عائم _tas_innov_var{0.0f};	///< true airspeed innovation variance

	عائم _beta_innov{0.0f};	///< sideslip innovation
	عائم _beta_innov_var{0.0f};	///< sideslip innovation variance

	منطقي _initialised{خطأ};	///< True: filter has been initialised

	عائم _wind_psd{0.01f};	///< wind process noise power spectral density (m^2/s^4/Hz), (0.1 m/s^2/sqrt(Hz))^2
	ساكن constexpr عائم kTasScalePsd{1e-8f};	///< true airspeed scale process noise power spectral density (1/s^2/Hz), (0.0001 1/s/sqrt(Hz))^2
	عائم _tas_var{1.96f};		///< true airspeed measurement noise variance (m/s)^2, (1.4 m/s)^2
	ساكن constexpr عائم kBetaVar{0.0225f};	///< sideslip measurement noise variance (rad)^2, (0.15 rad)^2
	ساكن constexpr uint8_t kTasGate{4};	///< airspeed fusion gate size (SD)
	ساكن constexpr uint8_t kBetaGate{1};	///< sideslip fusion gate size (SD)

	عائم _scale_init{1.f};

	uint64_t زمان_آخرین_airspeed_fuse = 0;	///< timestamp of last airspeed fusion
	uint64_t زمان_آخرین_beta_fuse = 0;	///< timestamp of last sideslip fusion
	uint64_t زمان_آخرین_update = 0;		///< timestamp of last covariance prediction
	uint64_t زمان_initialised = 0;         ///< timestamp when estimator is initialised

	ساكن constexpr عائم kTASScalePSDMultiplier = 100;
	ساكن constexpr hrt_abstime k_TASScale_Fast_Learn_زمان = 300_s;

	منطقي _wind_estimator_reset = خطأ; ///< wind estimator was reset in this cycle

	// initialise state and state covariance matrix
	منطقي initialise(ثابت ماتریس::Vector3f &velI, ثابت عائم hor_vel_variance, ثابت عائم heading_rad,
			ثابت عائم tas_meas = NAN, ثابت عائم tas_variance = NAN);

	فراغ پیاپی_sanity_checks();

	// return the square of two floating point numbers
	ساكن constexpr عائم sq(عائم var) { أرجع var * var; }
};
