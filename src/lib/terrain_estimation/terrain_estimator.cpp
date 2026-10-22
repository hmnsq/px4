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

#تعریف_کن فاصله_TIMEOUT 100000		// time in usec after which laser is considered dead

TerrainEstimator::TerrainEstimator() :
	فاصله_آخرین(0.0f),
	_terrain_valid(خطأ),
	زمان_آخرین_فاصله(0),
	زمان_آخرین_gnss(0)
{
	_x.zero();
	_u_z = 0.0f;
	_P.مجموعه_عنصر_خنثی();
}

منطقي TerrainEstimator::is_فاصله_valid(عائم فاصله)
{
	أرجع (فاصله < 40.0f && فاصله > 0.00001f);
}

فراغ TerrainEstimator::predict(عائم dt, ثابت بنية vehicle_attitude_s *attitude,
			       ثابت بنية sensor_combined_s *sensor,
			       ثابت بنية فاصله_sensor_s *فاصله)
{
	ماتریس::Dcmf R_att = ماتریس::Quatf(attitude->q);
	ماتریس::Vector3f a{sensor->accelerometer_m_s2[0], sensor->accelerometer_m_s2[1], sensor->accelerometer_m_s2[2]};
	ماتریس::وکتور<عائم, 3> u;
	u = R_att * a;
	_u_z = u(2) + CONSTANTS_ONE_G; // compensate for gravity

	// dynamics matrix
	ماتریس::ماتریس_2<عائم, n_x, n_x> A;
	A.مجموعه_Zero();
	A(0, 1) = 1;
	A(1, 2) = 1;

	// input matrix
	ماتریس::ماتریس_2<عائم, n_x, 1>  B;
	B.مجموعه_Zero();
	B(1, 0) = 1;

	// input noise variance
	عائم R = 0.135f;

	// process noise convariance
	ماتریس::ماتریس_2<عائم, n_x, n_x>  Q;
	Q(0, 0) = 0;
	Q(1, 1) = 0;

	// do prediction
	ماتریس::وکتور<عائم, n_x>  dx = (A * _x) * dt;
	dx(1) += B(1, 0) * _u_z * dt;

	// propagate state and covariance matrix
	_x += dx;
	_P += (A * _P + _P * A.transpose() +
	       B * R * B.transpose() + Q) * dt;
}

فراغ TerrainEstimator::measurement_update(uint64_t زمان_ref, ثابت بنية sensor_gnss_s *gnss,
		ثابت بنية فاصله_sensor_s *فاصله,
		ثابت بنية vehicle_attitude_s *attitude)
{
	// terrain estimate is invalid if we have range sensor timeout
	إذا (زمان_ref - فاصله->timestamp > فاصله_TIMEOUT) {
		_terrain_valid = خطأ;
	}

	إذا (فاصله->timestamp > زمان_آخرین_فاصله) {
		ماتریس::Quatf q(attitude->q);
		ماتریس::Eulerf euler(q);
		عائم d = فاصله->current_فاصله;

		ماتریس::ماتریس_2<عائم, 1, n_x> C;
		C(0, 0) = -1; // measured altitude,

		عائم R = 0.009f;

		ماتریس::وکتور<عائم, 1> y;
		y(0) = d * cosf(euler.phi()) * cosf(euler.theta());

		// residual
		ماتریس::ماتریس_2<عائم, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = ماتریس::inv<عائم, 1> (S_I);
		ماتریس::وکتور<عائم, 1> r = y - C * _x;

		ماتریس::ماتریس_2<عائم, n_x, 1> K = _P * C.transpose() * S_I;

		// some sort of outlayer rejection
		إذا (fabsf(فاصله->current_فاصله - فاصله_آخرین) < 1.0f) {
			_x += K * r;
			_P -= K * C * _P;
		}

		// if the current and the last range measurement are bad then we consider the terrain
		// estimate to be invalid
		إذا (!is_فاصله_valid(فاصله->current_فاصله) && !is_فاصله_valid(فاصله_آخرین)) {
			_terrain_valid = خطأ;

		} وإلا {
			_terrain_valid = درست;
		}

		زمان_آخرین_فاصله = فاصله->timestamp;
		فاصله_آخرین = فاصله->current_فاصله;
	}

	إذا (gnss->timestamp > زمان_آخرین_gnss && gnss->fix_type >= 3) {
		ماتریس::ماتریس_2<عائم, 1, n_x> C;
		C(0, 1) = 1;

		عائم R = 0.056f;

		ماتریس::وکتور<عائم, 1> y;
		y(0) = gnss->vel_پایین;

		// residual
		ماتریس::ماتریس_2<عائم, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = ماتریس::inv<عائم, 1>(S_I);
		ماتریس::وکتور<عائم, 1> r = y - C * _x;

		ماتریس::ماتریس_2<عائم, n_x, 1> K = _P * C.transpose() * S_I;
		_x += K * r;
		_P -= K * C * _P;

		زمان_آخرین_gnss = gnss->timestamp;
	}

	// reinitialise filter if we find bad data
	إذا (!_x.isAllFinite() || !_P.isAllFinite()) {
		_x.zero();
		_P.مجموعه_Zero();
		_P(0, 0) = _P(1, 1) = _P(2, 2) = 0.1f;
	}

}
