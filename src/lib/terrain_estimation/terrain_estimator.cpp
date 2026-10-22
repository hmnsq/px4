/****************************************************************************
 *
 *   Copyright (c) 2015 Roman Bapst. All rights reserved.
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
 * @file terrain_estimator.cpp
 * A terrain estimation kalman filter.
 */

#ಅನ್ತರ್ಭಾವ "terrain_estimator.h"
#ಅನ್ತರ್ಭಾವ <lib/geo/geo.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>

#ವ್ಯಾಖ್ಯಾನಿಸು ದೂರ_TIMEOUT 100000		// time in usec after which laser is considered dead

TerrainEstimator::TerrainEstimator() :
	ದೂರ_ಕೊನೆಯ(0.0f),
	_terrain_valid(ಸುಳ್ಳು),
	ಸಮಯ_ಕೊನೆಯ_ದೂರ(0),
	ಸಮಯ_ಕೊನೆಯ_gnss(0)
{
	_x.zero();
	_u_z = 0.0f;
	_P.ಗಣ_ತಟಸ್ಥ();
}

ತಾರ್ಕಿಕ TerrainEstimator::is_ದೂರ_valid(ದಶಮಾಂಶ ದೂರ)
{
	ಹಿಂತಿರುಗಿ (ದೂರ < 40.0f && ದೂರ > 0.00001f);
}

ಶೂನ್ಯ TerrainEstimator::predict(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ ರಚನೆ vehicle_attitude_s *attitude,
			       ಸ್ಥಿರಾಂಕ ರಚನೆ sensor_combined_s *sensor,
			       ಸ್ಥಿರಾಂಕ ರಚನೆ ದೂರ_sensor_s *ದೂರ)
{
	ಮಾತೃಕೆ::Dcmf R_att = ಮಾತೃಕೆ::Quatf(attitude->q);
	ಮಾತೃಕೆ::Vector3f a{sensor->accelerometer_m_s2[0], sensor->accelerometer_m_s2[1], sensor->accelerometer_m_s2[2]};
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 3> u;
	u = R_att * a;
	_u_z = u(2) + CONSTANTS_ONE_G; // compensate for gravity

	// dynamics matrix
	ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, n_x, n_x> A;
	A.ಗಣ_Zero();
	A(0, 1) = 1;
	A(1, 2) = 1;

	// input matrix
	ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, n_x, 1>  B;
	B.ಗಣ_Zero();
	B(1, 0) = 1;

	// input noise variance
	ದಶಮಾಂಶ R = 0.135f;

	// process noise convariance
	ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, n_x, n_x>  Q;
	Q(0, 0) = 0;
	Q(1, 1) = 0;

	// do prediction
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, n_x>  dx = (A * _x) * dt;
	dx(1) += B(1, 0) * _u_z * dt;

	// propagate state and covariance matrix
	_x += dx;
	_P += (A * _P + _P * A.transpose() +
	       B * R * B.transpose() + Q) * dt;
}

ಶೂನ್ಯ TerrainEstimator::measurement_update(uint64_t ಸಮಯ_ref, ಸ್ಥಿರಾಂಕ ರಚನೆ sensor_gnss_s *gnss,
		ಸ್ಥಿರಾಂಕ ರಚನೆ ದೂರ_sensor_s *ದೂರ,
		ಸ್ಥಿರಾಂಕ ರಚನೆ vehicle_attitude_s *attitude)
{
	// terrain estimate is invalid if we have range sensor timeout
	ಆದರೆ (ಸಮಯ_ref - ದೂರ->timestamp > ದೂರ_TIMEOUT) {
		_terrain_valid = ಸುಳ್ಳು;
	}

	ಆದರೆ (ದೂರ->timestamp > ಸಮಯ_ಕೊನೆಯ_ದೂರ) {
		ಮಾತೃಕೆ::Quatf q(attitude->q);
		ಮಾತೃಕೆ::Eulerf euler(q);
		ದಶಮಾಂಶ d = ದೂರ->current_ದೂರ;

		ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 1, n_x> C;
		C(0, 0) = -1; // measured altitude,

		ದಶಮಾಂಶ R = 0.009f;

		ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 1> y;
		y(0) = d * cosf(euler.phi()) * cosf(euler.theta());

		// residual
		ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = ಮಾತೃಕೆ::inv<ದಶಮಾಂಶ, 1> (S_I);
		ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 1> r = y - C * _x;

		ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, n_x, 1> K = _P * C.transpose() * S_I;

		// some sort of outlayer rejection
		ಆದರೆ (fabsf(ದೂರ->current_ದೂರ - ದೂರ_ಕೊನೆಯ) < 1.0f) {
			_x += K * r;
			_P -= K * C * _P;
		}

		// if the current and the last range measurement are bad then we consider the terrain
		// estimate to be invalid
		ಆದರೆ (!is_ದೂರ_valid(ದೂರ->current_ದೂರ) && !is_ದೂರ_valid(ದೂರ_ಕೊನೆಯ)) {
			_terrain_valid = ಸುಳ್ಳು;

		} ಇಲ್ಲದಿದ್ದರೆ {
			_terrain_valid = ನಿಜ;
		}

		ಸಮಯ_ಕೊನೆಯ_ದೂರ = ದೂರ->timestamp;
		ದೂರ_ಕೊನೆಯ = ದೂರ->current_ದೂರ;
	}

	ಆದರೆ (gnss->timestamp > ಸಮಯ_ಕೊನೆಯ_gnss && gnss->fix_type >= 3) {
		ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 1, n_x> C;
		C(0, 1) = 1;

		ದಶಮಾಂಶ R = 0.056f;

		ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 1> y;
		y(0) = gnss->vel_ಕೆಳಕ್ಕೆ;

		// residual
		ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = ಮಾತೃಕೆ::inv<ದಶಮಾಂಶ, 1>(S_I);
		ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 1> r = y - C * _x;

		ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, n_x, 1> K = _P * C.transpose() * S_I;
		_x += K * r;
		_P -= K * C * _P;

		ಸಮಯ_ಕೊನೆಯ_gnss = gnss->timestamp;
	}

	// reinitialise filter if we find bad data
	ಆದರೆ (!_x.isAllFinite() || !_P.isAllFinite()) {
		_x.zero();
		_P.ಗಣ_Zero();
		_P(0, 0) = _P(1, 1) = _P(2, 2) = 0.1f;
	}

}
