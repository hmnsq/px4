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

#تعریف_کړه واټن_TIMEOUT 100000		// time in usec after which laser is considered dead

TerrainEstimator::TerrainEstimator() :
	واټن_وروستی(0.0f),
	_terrain_valid(خطأ),
	وخت_وروستی_واټن(0),
	وخت_وروستی_gnss(0)
{
	_x.zero();
	_u_z = 0.0f;
	_P.سېټ_بې_اغېزه_عنصر();
}

منطقي TerrainEstimator::is_واټن_valid(عائم واټن)
{
	أرجع (واټن < 40.0f && واټن > 0.00001f);
}

فراغ TerrainEstimator::predict(عائم dt, ثابت بنية vehicle_attitude_s *attitude,
			       ثابت بنية sensor_combined_s *sensor,
			       ثابت بنية واټن_sensor_s *واټن)
{
	ماتریکس::Dcmf R_att = ماتریکس::Quatf(attitude->q);
	ماتریکس::Vector3f a{sensor->accelerometer_m_s2[0], sensor->accelerometer_m_s2[1], sensor->accelerometer_m_s2[2]};
	ماتریکس::ویکټور<عائم, 3> u;
	u = R_att * a;
	_u_z = u(2) + CONSTANTS_ONE_G; // compensate for gravity

	// dynamics matrix
	ماتریکس::ماتریکس_2<عائم, n_x, n_x> A;
	A.سېټ_Zero();
	A(0, 1) = 1;
	A(1, 2) = 1;

	// input matrix
	ماتریکس::ماتریکس_2<عائم, n_x, 1>  B;
	B.سېټ_Zero();
	B(1, 0) = 1;

	// input noise variance
	عائم R = 0.135f;

	// process noise convariance
	ماتریکس::ماتریکس_2<عائم, n_x, n_x>  Q;
	Q(0, 0) = 0;
	Q(1, 1) = 0;

	// do prediction
	ماتریکس::ویکټور<عائم, n_x>  dx = (A * _x) * dt;
	dx(1) += B(1, 0) * _u_z * dt;

	// propagate state and covariance matrix
	_x += dx;
	_P += (A * _P + _P * A.transpose() +
	       B * R * B.transpose() + Q) * dt;
}

فراغ TerrainEstimator::measurement_update(uint64_t وخت_ref, ثابت بنية sensor_gnss_s *gnss,
		ثابت بنية واټن_sensor_s *واټن,
		ثابت بنية vehicle_attitude_s *attitude)
{
	// terrain estimate is invalid if we have range sensor timeout
	إذا (وخت_ref - واټن->timestamp > واټن_TIMEOUT) {
		_terrain_valid = خطأ;
	}

	إذا (واټن->timestamp > وخت_وروستی_واټن) {
		ماتریکس::Quatf q(attitude->q);
		ماتریکس::Eulerf euler(q);
		عائم d = واټن->current_واټن;

		ماتریکس::ماتریکس_2<عائم, 1, n_x> C;
		C(0, 0) = -1; // measured altitude,

		عائم R = 0.009f;

		ماتریکس::ویکټور<عائم, 1> y;
		y(0) = d * cosf(euler.phi()) * cosf(euler.theta());

		// residual
		ماتریکس::ماتریکس_2<عائم, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = ماتریکس::inv<عائم, 1> (S_I);
		ماتریکس::ویکټور<عائم, 1> r = y - C * _x;

		ماتریکس::ماتریکس_2<عائم, n_x, 1> K = _P * C.transpose() * S_I;

		// some sort of outlayer rejection
		إذا (fabsf(واټن->current_واټن - واټن_وروستی) < 1.0f) {
			_x += K * r;
			_P -= K * C * _P;
		}

		// if the current and the last range measurement are bad then we consider the terrain
		// estimate to be invalid
		إذا (!is_واټن_valid(واټن->current_واټن) && !is_واټن_valid(واټن_وروستی)) {
			_terrain_valid = خطأ;

		} وإلا {
			_terrain_valid = ریښتیا;
		}

		وخت_وروستی_واټن = واټن->timestamp;
		واټن_وروستی = واټن->current_واټن;
	}

	إذا (gnss->timestamp > وخت_وروستی_gnss && gnss->fix_type >= 3) {
		ماتریکس::ماتریکس_2<عائم, 1, n_x> C;
		C(0, 1) = 1;

		عائم R = 0.056f;

		ماتریکس::ویکټور<عائم, 1> y;
		y(0) = gnss->vel_ښکته;

		// residual
		ماتریکس::ماتریکس_2<عائم, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = ماتریکس::inv<عائم, 1>(S_I);
		ماتریکس::ویکټور<عائم, 1> r = y - C * _x;

		ماتریکس::ماتریکس_2<عائم, n_x, 1> K = _P * C.transpose() * S_I;
		_x += K * r;
		_P -= K * C * _P;

		وخت_وروستی_gnss = gnss->timestamp;
	}

	// reinitialise filter if we find bad data
	إذا (!_x.isAllFinite() || !_P.isAllFinite()) {
		_x.zero();
		_P.سېټ_Zero();
		_P(0, 0) = _P(1, 1) = _P(2, 2) = 0.1f;
	}

}
