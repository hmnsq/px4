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

#ਅਨ੍ਤਰ੍ਭਾਵ "terrain_estimator.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/geo/geo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੂਰੀ_TIMEOUT 100000		// time in usec after which laser is considered dead

TerrainEstimator::TerrainEstimator() :
	ਦੂਰੀ_ਆਖਰੀ(0.0f),
	_terrain_valid(ਝੂਠ),
	ਸਮਾਂ_ਆਖਰੀ_ਦੂਰੀ(0),
	ਸਮਾਂ_ਆਖਰੀ_gnss(0)
{
	_x.zero();
	_u_z = 0.0f;
	_P.ਸੈੱਟ_ਪਛਾਣ_ਤੱਤ();
}

ਤਾਰਕਿਕ TerrainEstimator::is_ਦੂਰੀ_valid(ਦਸ਼ਮਲਵ ਦੂਰੀ)
{
	ਵਾਪਸ (ਦੂਰੀ < 40.0f && ਦੂਰੀ > 0.00001f);
}

ਖਾਲੀ TerrainEstimator::predict(ਦਸ਼ਮਲਵ dt, ਅਚਲ ਬਣਤਰ vehicle_attitude_s *attitude,
			       ਅਚਲ ਬਣਤਰ sensor_combined_s *sensor,
			       ਅਚਲ ਬਣਤਰ ਦੂਰੀ_sensor_s *ਦੂਰੀ)
{
	ਮੈਟ੍ਰਿਕਸ::Dcmf R_att = ਮੈਟ੍ਰਿਕਸ::Quatf(attitude->q);
	ਮੈਟ੍ਰਿਕਸ::Vector3f a{sensor->accelerometer_m_s2[0], sensor->accelerometer_m_s2[1], sensor->accelerometer_m_s2[2]};
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 3> u;
	u = R_att * a;
	_u_z = u(2) + CONSTANTS_ONE_G; // compensate for gravity

	// dynamics matrix
	ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, n_x, n_x> A;
	A.ਸੈੱਟ_Zero();
	A(0, 1) = 1;
	A(1, 2) = 1;

	// input matrix
	ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, n_x, 1>  B;
	B.ਸੈੱਟ_Zero();
	B(1, 0) = 1;

	// input noise variance
	ਦਸ਼ਮਲਵ R = 0.135f;

	// process noise convariance
	ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, n_x, n_x>  Q;
	Q(0, 0) = 0;
	Q(1, 1) = 0;

	// do prediction
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, n_x>  dx = (A * _x) * dt;
	dx(1) += B(1, 0) * _u_z * dt;

	// propagate state and covariance matrix
	_x += dx;
	_P += (A * _P + _P * A.transpose() +
	       B * R * B.transpose() + Q) * dt;
}

ਖਾਲੀ TerrainEstimator::measurement_update(uint64_t ਸਮਾਂ_ref, ਅਚਲ ਬਣਤਰ sensor_gnss_s *gnss,
		ਅਚਲ ਬਣਤਰ ਦੂਰੀ_sensor_s *ਦੂਰੀ,
		ਅਚਲ ਬਣਤਰ vehicle_attitude_s *attitude)
{
	// terrain estimate is invalid if we have range sensor timeout
	ਜੇ (ਸਮਾਂ_ref - ਦੂਰੀ->timestamp > ਦੂਰੀ_TIMEOUT) {
		_terrain_valid = ਝੂਠ;
	}

	ਜੇ (ਦੂਰੀ->timestamp > ਸਮਾਂ_ਆਖਰੀ_ਦੂਰੀ) {
		ਮੈਟ੍ਰਿਕਸ::Quatf q(attitude->q);
		ਮੈਟ੍ਰਿਕਸ::Eulerf euler(q);
		ਦਸ਼ਮਲਵ d = ਦੂਰੀ->current_ਦੂਰੀ;

		ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 1, n_x> C;
		C(0, 0) = -1; // measured altitude,

		ਦਸ਼ਮਲਵ R = 0.009f;

		ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 1> y;
		y(0) = d * cosf(euler.phi()) * cosf(euler.theta());

		// residual
		ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = ਮੈਟ੍ਰਿਕਸ::inv<ਦਸ਼ਮਲਵ, 1> (S_I);
		ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 1> r = y - C * _x;

		ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, n_x, 1> K = _P * C.transpose() * S_I;

		// some sort of outlayer rejection
		ਜੇ (fabsf(ਦੂਰੀ->current_ਦੂਰੀ - ਦੂਰੀ_ਆਖਰੀ) < 1.0f) {
			_x += K * r;
			_P -= K * C * _P;
		}

		// if the current and the last range measurement are bad then we consider the terrain
		// estimate to be invalid
		ਜੇ (!is_ਦੂਰੀ_valid(ਦੂਰੀ->current_ਦੂਰੀ) && !is_ਦੂਰੀ_valid(ਦੂਰੀ_ਆਖਰੀ)) {
			_terrain_valid = ਝੂਠ;

		} ਨਹੀਂਤਾਂ {
			_terrain_valid = ਸੱਚ;
		}

		ਸਮਾਂ_ਆਖਰੀ_ਦੂਰੀ = ਦੂਰੀ->timestamp;
		ਦੂਰੀ_ਆਖਰੀ = ਦੂਰੀ->current_ਦੂਰੀ;
	}

	ਜੇ (gnss->timestamp > ਸਮਾਂ_ਆਖਰੀ_gnss && gnss->fix_type >= 3) {
		ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 1, n_x> C;
		C(0, 1) = 1;

		ਦਸ਼ਮਲਵ R = 0.056f;

		ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 1> y;
		y(0) = gnss->vel_ਥੱਲੇ;

		// residual
		ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = ਮੈਟ੍ਰਿਕਸ::inv<ਦਸ਼ਮਲਵ, 1>(S_I);
		ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 1> r = y - C * _x;

		ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, n_x, 1> K = _P * C.transpose() * S_I;
		_x += K * r;
		_P -= K * C * _P;

		ਸਮਾਂ_ਆਖਰੀ_gnss = gnss->timestamp;
	}

	// reinitialise filter if we find bad data
	ਜੇ (!_x.isAllFinite() || !_P.isAllFinite()) {
		_x.zero();
		_P.ਸੈੱਟ_Zero();
		_P(0, 0) = _P(1, 1) = _P(2, 2) = 0.1f;
	}

}
