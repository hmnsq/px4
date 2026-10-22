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

#निर्देशः once

#अन्तर्भाव <drivers/drv_hrt.h>

#अन्तर्भाव <mathlib/mathlib.h>
#अन्तर्भाव <matrix/math.hpp>

#अन्तर्भाव "python/generated/fuse_airspeed.h"
#अन्तर्भाव "python/generated/fuse_beta.h"
#अन्तर्भाव "python/generated/init_wind_using_airspeed.h"

उपयुज्य नामाकाश time_literals;

वर्ग WindEstimator
{
सार्वजनिक:
	WindEstimator() = स्वभाव;
	~WindEstimator() = स्वभाव;

	// no copy, assignment, move, move assignment
	WindEstimator(ध्रुव WindEstimator &) = विलोपय;
	WindEstimator &संक्रियकः=(ध्रुव WindEstimator &) = विलोपय;
	WindEstimator(WindEstimator &&) = विलोपय;
	WindEstimator &संक्रियकः=(WindEstimator &&) = विलोपय;

	शून्य update(uint64_t time_now);

	शून्य fuse_airspeed(uint64_t time_now, भिन्न true_airspeed, ध्रुव matrix::Vector3f &velI,
			   ध्रुव भिन्न hor_vel_variance, ध्रुव matrix::Quatf &q_att);
	शून्य fuse_beta(uint64_t time_now, ध्रुव matrix::Vector3f &velI, ध्रुव भिन्न hor_vel_variance,
		       ध्रुव matrix::Quatf &q_att);

	तार्किक is_estimate_valid() { प्रत्यागम _initialised; }

	तार्किक check_if_meas_is_rejected(भिन्न innov, भिन्न innov_var, uint8_t gate_आकार);

	matrix::Vector2f get_wind() { प्रत्यागम matrix::Vector2f{_state(सूचकांक_W_N), _state(सूचकांक_W_E)}; }

	// invert scale (CAS = IAS * scale), protect agains division by 0, constrain to [0.1, 10]
	भिन्न get_tas_scale() { प्रत्यागम 1.f / math::constrain(_state(सूचकांक_TAS_SCALE), 0.1f, 10.0f); }
	भिन्न get_tas_scale_var() { प्रत्यागम _P(सूचकांक_TAS_SCALE, सूचकांक_TAS_SCALE); }
	भिन्न get_tas_innov() { प्रत्यागम _tas_innov; }
	भिन्न get_tas_innov_var() { प्रत्यागम _tas_innov_var; }
	भिन्न get_beta_innov() { प्रत्यागम _beta_innov; }
	भिन्न get_beta_innov_var() { प्रत्यागम _beta_innov_var; }
	matrix::Vector2f  get_wind_var() { प्रत्यागम matrix::Vector2f{_P(सूचकांक_W_N, सूचकांक_W_N), _P(सूचकांक_W_E, सूचकांक_W_E)}; }
	तार्किक get_wind_estimator_reset() { प्रत्यागम _wind_estimator_reset; }

	// unaided, the state uncertainty (diagonal of sqrt(P)) grows by the process noise spectral density every second
	शून्य समुच्चयः_wind_process_noise_spectral_density(भिन्न wind_nsd) { _wind_psd = wind_nsd * wind_nsd; }

	शून्य समुच्चयः_tas_noise(भिन्न tas_sigma) { _tas_var = tas_sigma * tas_sigma; }

	शून्य समुच्चयः_scale_init(भिन्न scale_init) {_scale_init = 1.f / math::constrain(scale_init, 0.1f, 10.f); }

	शून्य reset_scale_गन्तव्यम्_init()
	{
		_state(सूचकांक_TAS_SCALE) = _scale_init;
		स्वचालित P_wind = _P.खण्डिका<2, 2>(0, 0);
		_P.समुच्चयः_Zero();
		_P.खण्डिका<2, 2>(0, 0) = P_wind;
	}

गुप्त:
	गणना {
		सूचकांक_W_N = 0,
		सूचकांक_W_E,
		सूचकांक_TAS_SCALE
	};	///< enum which can be used to access state.

	स्थिर constexpr भिन्न kInitialWindError =
		2.5f; // initial uncertainty of each wind state when filter is initialised without airspeed [m/s]
	स्थिर constexpr भिन्न kInitialBetaErrorDeg =
		15.0f;	// sidelip uncertainty used to initialise the filter with a true airspeed measurement [deg]
	स्थिर constexpr भिन्न kInitialHeadingErrorDeg =
		10.0f; // heading uncertainty used to initialise the filter with a true airspeed measurement [deg]

	matrix::Vector3f _state{0.f, 0.f, 1.f};
	matrix::Matrix3f _P;		///< state covariance matrix

	भिन्न _tas_innov{0.0f};	///< true airspeed innovation
	भिन्न _tas_innov_var{0.0f};	///< true airspeed innovation variance

	भिन्न _beta_innov{0.0f};	///< sideslip innovation
	भिन्न _beta_innov_var{0.0f};	///< sideslip innovation variance

	तार्किक _initialised{असत्य};	///< True: filter has been initialised

	भिन्न _wind_psd{0.01f};	///< wind process noise power spectral density (m^2/s^4/Hz), (0.1 m/s^2/sqrt(Hz))^2
	स्थिर constexpr भिन्न kTasScalePsd{1e-8f};	///< true airspeed scale process noise power spectral density (1/s^2/Hz), (0.0001 1/s/sqrt(Hz))^2
	भिन्न _tas_var{1.96f};		///< true airspeed measurement noise variance (m/s)^2, (1.4 m/s)^2
	स्थिर constexpr भिन्न kBetaVar{0.0225f};	///< sideslip measurement noise variance (rad)^2, (0.15 rad)^2
	स्थिर constexpr uint8_t kTasGate{4};	///< airspeed fusion gate size (SD)
	स्थिर constexpr uint8_t kBetaGate{1};	///< sideslip fusion gate size (SD)

	भिन्न _scale_init{1.f};

	uint64_t time_चरमः_airspeed_fuse = 0;	///< timestamp of last airspeed fusion
	uint64_t time_चरमः_beta_fuse = 0;	///< timestamp of last sideslip fusion
	uint64_t time_चरमः_update = 0;		///< timestamp of last covariance prediction
	uint64_t _time_initialised = 0;         ///< timestamp when estimator is initialised

	स्थिर constexpr भिन्न kTASScalePSDMultiplier = 100;
	स्थिर constexpr hrt_abstime kTASScaleFastLearnTime = 300_s;

	तार्किक _wind_estimator_reset = असत्य; ///< wind estimator was reset in this cycle

	// initialise state and state covariance matrix
	तार्किक initialise(ध्रुव matrix::Vector3f &velI, ध्रुव भिन्न hor_vel_variance, ध्रुव भिन्न heading_rad,
			ध्रुव भिन्न tas_meas = NAN, ध्रुव भिन्न tas_variance = NAN);

	शून्य सततम्_sanity_checks();

	// return the square of two floating point numbers
	स्थिर constexpr भिन्न sq(भिन्न var) { प्रत्यागम var * var; }
};
