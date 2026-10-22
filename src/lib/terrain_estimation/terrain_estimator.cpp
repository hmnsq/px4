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

#અન્તર્ભાવ "terrain_estimator.h"
#અન્તર્ભાવ <lib/geo/geo.h>
#અન્તર્ભાવ <px4_platform_common/defines.h>

#define DISTANCE_TIMEOUT 100000		// time in usec after which laser is considered dead

TerrainEstimator::TerrainEstimator() :
	_distance_last(0.0f),
	_terrain_valid(ખોટું),
	_time_last_distance(0),
	_time_last_gnss(0)
{
	_x.zero();
	_u_z = 0.0f;
	_P.setIdentity();
}

તાર્કિક TerrainEstimator::is_distance_valid(દશાંશ distance)
{
	પાછા (distance < 40.0f && distance > 0.00001f);
}

શૂન્ય TerrainEstimator::predict(દશાંશ dt, અચલ સંરચના vehicle_attitude_s *attitude,
			       અચલ સંરચના sensor_combined_s *sensor,
			       અચલ સંરચના distance_sensor_s *distance)
{
	matrix::Dcmf R_att = matrix::Quatf(attitude->q);
	matrix::Vector3f a{sensor->accelerometer_m_s2[0], sensor->accelerometer_m_s2[1], sensor->accelerometer_m_s2[2]};
	matrix::Vector<દશાંશ, 3> u;
	u = R_att * a;
	_u_z = u(2) + CONSTANTS_ONE_G; // compensate for gravity

	// dynamics matrix
	matrix::Matrix<દશાંશ, n_x, n_x> A;
	A.setZero();
	A(0, 1) = 1;
	A(1, 2) = 1;

	// input matrix
	matrix::Matrix<દશાંશ, n_x, 1>  B;
	B.setZero();
	B(1, 0) = 1;

	// input noise variance
	દશાંશ R = 0.135f;

	// process noise convariance
	matrix::Matrix<દશાંશ, n_x, n_x>  Q;
	Q(0, 0) = 0;
	Q(1, 1) = 0;

	// do prediction
	matrix::Vector<દશાંશ, n_x>  dx = (A * _x) * dt;
	dx(1) += B(1, 0) * _u_z * dt;

	// propagate state and covariance matrix
	_x += dx;
	_P += (A * _P + _P * A.transpose() +
	       B * R * B.transpose() + Q) * dt;
}

શૂન્ય TerrainEstimator::measurement_update(uint64_t time_ref, અચલ સંરચના sensor_gnss_s *gnss,
		અચલ સંરચના distance_sensor_s *distance,
		અચલ સંરચના vehicle_attitude_s *attitude)
{
	// terrain estimate is invalid if we have range sensor timeout
	જો (time_ref - distance->timestamp > DISTANCE_TIMEOUT) {
		_terrain_valid = ખોટું;
	}

	જો (distance->timestamp > _time_last_distance) {
		matrix::Quatf q(attitude->q);
		matrix::Eulerf euler(q);
		દશાંશ d = distance->current_distance;

		matrix::Matrix<દશાંશ, 1, n_x> C;
		C(0, 0) = -1; // measured altitude,

		દશાંશ R = 0.009f;

		matrix::Vector<દશાંશ, 1> y;
		y(0) = d * cosf(euler.phi()) * cosf(euler.theta());

		// residual
		matrix::Matrix<દશાંશ, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = matrix::inv<દશાંશ, 1> (S_I);
		matrix::Vector<દશાંશ, 1> r = y - C * _x;

		matrix::Matrix<દશાંશ, n_x, 1> K = _P * C.transpose() * S_I;

		// some sort of outlayer rejection
		જો (fabsf(distance->current_distance - _distance_last) < 1.0f) {
			_x += K * r;
			_P -= K * C * _P;
		}

		// if the current and the last range measurement are bad then we consider the terrain
		// estimate to be invalid
		જો (!is_distance_valid(distance->current_distance) && !is_distance_valid(_distance_last)) {
			_terrain_valid = ખોટું;

		} નહીંતર {
			_terrain_valid = સાચું;
		}

		_time_last_distance = distance->timestamp;
		_distance_last = distance->current_distance;
	}

	જો (gnss->timestamp > _time_last_gnss && gnss->fix_type >= 3) {
		matrix::Matrix<દશાંશ, 1, n_x> C;
		C(0, 1) = 1;

		દશાંશ R = 0.056f;

		matrix::Vector<દશાંશ, 1> y;
		y(0) = gnss->vel_down;

		// residual
		matrix::Matrix<દશાંશ, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = matrix::inv<દશાંશ, 1>(S_I);
		matrix::Vector<દશાંશ, 1> r = y - C * _x;

		matrix::Matrix<દશાંશ, n_x, 1> K = _P * C.transpose() * S_I;
		_x += K * r;
		_P -= K * C * _P;

		_time_last_gnss = gnss->timestamp;
	}

	// reinitialise filter if we find bad data
	જો (!_x.isAllFinite() || !_P.isAllFinite()) {
		_x.zero();
		_P.setZero();
		_P(0, 0) = _P(1, 1) = _P(2, 2) = 0.1f;
	}

}
