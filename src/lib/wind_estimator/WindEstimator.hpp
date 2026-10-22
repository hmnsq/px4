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

#సూచన once

#అన్తర్భావ <drivers/drv_hrt.h>

#అన్తర్భావ <mathlib/mathlib.h>
#అన్తర్భావ <matrix/math.hpp>

#అన్తర్భావ "python/generated/fuse_airspeed.h"
#అన్తర్భావ "python/generated/fuse_beta.h"
#అన్తర్భావ "python/generated/init_wind_using_airspeed.h"

ఉపయోగించి నామాకాశ సమయం_literals;

తరగతి WindEstimator
{
బహిరంగ:
	WindEstimator() = అప్రమేయ;
	~WindEstimator() = అప్రమేయ;

	// no copy, assignment, move, move assignment
	WindEstimator(స్థిరరాశి WindEstimator &) = తొలగించు;
	WindEstimator &క్రియాకారకం=(స్థిరరాశి WindEstimator &) = తొలగించు;
	WindEstimator(WindEstimator &&) = తొలగించు;
	WindEstimator &క్రియాకారకం=(WindEstimator &&) = తొలగించు;

	శూన్య update(uint64_t సమయం_now);

	శూన్య fuse_airspeed(uint64_t సమయం_now, దశాంశం true_airspeed, స్థిరరాశి మాతృక::Vector3f &velI,
			   స్థిరరాశి దశాంశం hor_vel_variance, స్థిరరాశి మాతృక::Quatf &q_att);
	శూన్య fuse_beta(uint64_t సమయం_now, స్థిరరాశి మాతృక::Vector3f &velI, స్థిరరాశి దశాంశం hor_vel_variance,
		       స్థిరరాశి మాతృక::Quatf &q_att);

	తార్కిక is_estimate_valid() { తిరిగి _initialised; }

	తార్కిక check_if_meas_is_rejected(దశాంశం innov, దశాంశం innov_var, uint8_t gate_కొలత);

	మాతృక::Vector2f get_wind() { తిరిగి మాతృక::Vector2f{_state(సూచీ_W_N), _state(సూచీ_W_E)}; }

	// invert scale (CAS = IAS * scale), protect agains division by 0, constrain to [0.1, 10]
	దశాంశం get_tas_scale() { తిరిగి 1.f / math::constrain(_state(సూచీ_TAS_SCALE), 0.1f, 10.0f); }
	దశాంశం get_tas_scale_var() { తిరిగి _P(సూచీ_TAS_SCALE, సూచీ_TAS_SCALE); }
	దశాంశం get_tas_innov() { తిరిగి _tas_innov; }
	దశాంశం get_tas_innov_var() { తిరిగి _tas_innov_var; }
	దశాంశం get_beta_innov() { తిరిగి _beta_innov; }
	దశాంశం get_beta_innov_var() { తిరిగి _beta_innov_var; }
	మాతృక::Vector2f  get_wind_var() { తిరిగి మాతృక::Vector2f{_P(సూచీ_W_N, సూచీ_W_N), _P(సూచీ_W_E, సూచీ_W_E)}; }
	తార్కిక get_wind_estimator_reset() { తిరిగి _wind_estimator_reset; }

	// unaided, the state uncertainty (diagonal of sqrt(P)) grows by the process noise spectral density every second
	శూన్య సమితి_wind_process_noise_spectral_density(దశాంశం wind_nsd) { _wind_psd = wind_nsd * wind_nsd; }

	శూన్య సమితి_tas_noise(దశాంశం tas_sigma) { _tas_var = tas_sigma * tas_sigma; }

	శూన్య సమితి_scale_init(దశాంశం scale_init) {_scale_init = 1.f / math::constrain(scale_init, 0.1f, 10.f); }

	శూన్య reset_scale_గమ్యం_init()
	{
		_state(సూచీ_TAS_SCALE) = _scale_init;
		స్వయంచాలక P_wind = _P.ముక్క<2, 2>(0, 0);
		_P.సమితి_Zero();
		_P.ముక్క<2, 2>(0, 0) = P_wind;
	}

అంతరంగ:
	గణన {
		సూచీ_W_N = 0,
		సూచీ_W_E,
		సూచీ_TAS_SCALE
	};	///< enum which can be used to access state.

	స్థిర constexpr దశాంశం kInitialWindError =
		2.5f; // initial uncertainty of each wind state when filter is initialised without airspeed [m/s]
	స్థిర constexpr దశాంశం kInitialBetaErrorDeg =
		15.0f;	// sidelip uncertainty used to initialise the filter with a true airspeed measurement [deg]
	స్థిర constexpr దశాంశం kInitialHeadingErrorDeg =
		10.0f; // heading uncertainty used to initialise the filter with a true airspeed measurement [deg]

	మాతృక::Vector3f _state{0.f, 0.f, 1.f};
	మాతృక::Matrix3f _P;		///< state covariance matrix

	దశాంశం _tas_innov{0.0f};	///< true airspeed innovation
	దశాంశం _tas_innov_var{0.0f};	///< true airspeed innovation variance

	దశాంశం _beta_innov{0.0f};	///< sideslip innovation
	దశాంశం _beta_innov_var{0.0f};	///< sideslip innovation variance

	తార్కిక _initialised{అబద్ధం};	///< True: filter has been initialised

	దశాంశం _wind_psd{0.01f};	///< wind process noise power spectral density (m^2/s^4/Hz), (0.1 m/s^2/sqrt(Hz))^2
	స్థిర constexpr దశాంశం kTasScalePsd{1e-8f};	///< true airspeed scale process noise power spectral density (1/s^2/Hz), (0.0001 1/s/sqrt(Hz))^2
	దశాంశం _tas_var{1.96f};		///< true airspeed measurement noise variance (m/s)^2, (1.4 m/s)^2
	స్థిర constexpr దశాంశం kBetaVar{0.0225f};	///< sideslip measurement noise variance (rad)^2, (0.15 rad)^2
	స్థిర constexpr uint8_t kTasGate{4};	///< airspeed fusion gate size (SD)
	స్థిర constexpr uint8_t kBetaGate{1};	///< sideslip fusion gate size (SD)

	దశాంశం _scale_init{1.f};

	uint64_t సమయం_చివరి_airspeed_fuse = 0;	///< timestamp of last airspeed fusion
	uint64_t సమయం_చివరి_beta_fuse = 0;	///< timestamp of last sideslip fusion
	uint64_t సమయం_చివరి_update = 0;		///< timestamp of last covariance prediction
	uint64_t సమయం_initialised = 0;         ///< timestamp when estimator is initialised

	స్థిర constexpr దశాంశం kTASScalePSDMultiplier = 100;
	స్థిర constexpr hrt_abstime k_TASScale_Fast_Learn_సమయం = 300_s;

	తార్కిక _wind_estimator_reset = అబద్ధం; ///< wind estimator was reset in this cycle

	// initialise state and state covariance matrix
	తార్కిక initialise(స్థిరరాశి మాతృక::Vector3f &velI, స్థిరరాశి దశాంశం hor_vel_variance, స్థిరరాశి దశాంశం heading_rad,
			స్థిరరాశి దశాంశం tas_meas = NAN, స్థిరరాశి దశాంశం tas_variance = NAN);

	శూన్య నిరంతరం_sanity_checks();

	// return the square of two floating point numbers
	స్థిర constexpr దశాంశం sq(దశాంశం var) { తిరిగి var * var; }
};
