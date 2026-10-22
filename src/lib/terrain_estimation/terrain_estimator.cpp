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

#ضمن "terrain_estimator.h"
#ضمن <lib/geo/geo.h>
#ضمن <px4_platform_common/defines.h>

#وصف_ڪريو مفاصلو_TIMEOUT 100000		// time in usec after which laser is considered dead

TerrainEstimator::TerrainEstimator() :
	مفاصلو_آخري(0.0f),
	_terrain_valid(خطأ),
	وقت_آخري_مفاصلو(0),
	وقت_آخري_gnss(0)
{
	_x.zero();
	_u_z = 0.0f;
	_P.سيٽ_سڃاڻپ_عنصر();
}

منطقي TerrainEstimator::is_مفاصلو_valid(عائم مفاصلو)
{
	أرجع (مفاصلو < 40.0f && مفاصلو > 0.00001f);
}

فراغ TerrainEstimator::predict(عائم dt, ثابت بنية vehicle_attitude_s *attitude,
			       ثابت بنية sensor_combined_s *sensor,
			       ثابت بنية مفاصلو_sensor_s *مفاصلو)
{
	ميٽرڪس::Dcmf R_att = ميٽرڪس::Quatf(attitude->q);
	ميٽرڪس::Vector3f a{sensor->accelerometer_m_s2[0], sensor->accelerometer_m_s2[1], sensor->accelerometer_m_s2[2]};
	ميٽرڪس::ويڪٽر<عائم, 3> u;
	u = R_att * a;
	_u_z = u(2) + CONSTANTS_ONE_G; // compensate for gravity

	// dynamics matrix
	ميٽرڪس::ميٽرڪس_2<عائم, n_x, n_x> A;
	A.سيٽ_Zero();
	A(0, 1) = 1;
	A(1, 2) = 1;

	// input matrix
	ميٽرڪس::ميٽرڪس_2<عائم, n_x, 1>  B;
	B.سيٽ_Zero();
	B(1, 0) = 1;

	// input noise variance
	عائم R = 0.135f;

	// process noise convariance
	ميٽرڪس::ميٽرڪس_2<عائم, n_x, n_x>  Q;
	Q(0, 0) = 0;
	Q(1, 1) = 0;

	// do prediction
	ميٽرڪس::ويڪٽر<عائم, n_x>  dx = (A * _x) * dt;
	dx(1) += B(1, 0) * _u_z * dt;

	// propagate state and covariance matrix
	_x += dx;
	_P += (A * _P + _P * A.transpose() +
	       B * R * B.transpose() + Q) * dt;
}

فراغ TerrainEstimator::measurement_update(uint64_t وقت_ref, ثابت بنية sensor_gnss_s *gnss,
		ثابت بنية مفاصلو_sensor_s *مفاصلو,
		ثابت بنية vehicle_attitude_s *attitude)
{
	// terrain estimate is invalid if we have range sensor timeout
	إذا (وقت_ref - مفاصلو->timestamp > مفاصلو_TIMEOUT) {
		_terrain_valid = خطأ;
	}

	إذا (مفاصلو->timestamp > وقت_آخري_مفاصلو) {
		ميٽرڪس::Quatf q(attitude->q);
		ميٽرڪس::Eulerf euler(q);
		عائم d = مفاصلو->current_مفاصلو;

		ميٽرڪس::ميٽرڪس_2<عائم, 1, n_x> C;
		C(0, 0) = -1; // measured altitude,

		عائم R = 0.009f;

		ميٽرڪس::ويڪٽر<عائم, 1> y;
		y(0) = d * cosf(euler.phi()) * cosf(euler.theta());

		// residual
		ميٽرڪس::ميٽرڪس_2<عائم, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = ميٽرڪس::inv<عائم, 1> (S_I);
		ميٽرڪس::ويڪٽر<عائم, 1> r = y - C * _x;

		ميٽرڪس::ميٽرڪس_2<عائم, n_x, 1> K = _P * C.transpose() * S_I;

		// some sort of outlayer rejection
		إذا (fabsf(مفاصلو->current_مفاصلو - مفاصلو_آخري) < 1.0f) {
			_x += K * r;
			_P -= K * C * _P;
		}

		// if the current and the last range measurement are bad then we consider the terrain
		// estimate to be invalid
		إذا (!is_مفاصلو_valid(مفاصلو->current_مفاصلو) && !is_مفاصلو_valid(مفاصلو_آخري)) {
			_terrain_valid = خطأ;

		} وإلا {
			_terrain_valid = سچ;
		}

		وقت_آخري_مفاصلو = مفاصلو->timestamp;
		مفاصلو_آخري = مفاصلو->current_مفاصلو;
	}

	إذا (gnss->timestamp > وقت_آخري_gnss && gnss->fix_type >= 3) {
		ميٽرڪس::ميٽرڪس_2<عائم, 1, n_x> C;
		C(0, 1) = 1;

		عائم R = 0.056f;

		ميٽرڪس::ويڪٽر<عائم, 1> y;
		y(0) = gnss->vel_هيٺ_ڏانهن;

		// residual
		ميٽرڪس::ميٽرڪس_2<عائم, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = ميٽرڪس::inv<عائم, 1>(S_I);
		ميٽرڪس::ويڪٽر<عائم, 1> r = y - C * _x;

		ميٽرڪس::ميٽرڪس_2<عائم, n_x, 1> K = _P * C.transpose() * S_I;
		_x += K * r;
		_P -= K * C * _P;

		وقت_آخري_gnss = gnss->timestamp;
	}

	// reinitialise filter if we find bad data
	إذا (!_x.isAllFinite() || !_P.isAllFinite()) {
		_x.zero();
		_P.سيٽ_Zero();
		_P(0, 0) = _P(1, 1) = _P(2, 2) = 0.1f;
	}

}
