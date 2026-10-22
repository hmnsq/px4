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

#شامل "terrain_estimator.h"
#شامل <lib/geo/geo.h>
#شامل <px4_platform_common/defines.h>

#تعریف_کرو فاصلہ_TIMEOUT 100000		// time in usec after which laser is considered dead

TerrainEstimator::TerrainEstimator() :
	فاصلہ_آخری(0.0f),
	_terrain_valid(جھوٹ),
	وقت_آخری_فاصلہ(0),
	وقت_آخری_gnss(0)
{
	_x.zero();
	_u_z = 0.0f;
	_P.سیٹ_شناختی_عنصر();
}

منطقی TerrainEstimator::is_فاصلہ_valid(اعشاری فاصلہ)
{
	واپس (فاصلہ < 40.0f && فاصلہ > 0.00001f);
}

خالی TerrainEstimator::predict(اعشاری dt, ثابت ساخت vehicle_attitude_s *attitude,
			       ثابت ساخت sensor_combined_s *sensor,
			       ثابت ساخت فاصلہ_sensor_s *فاصلہ)
{
	میٹرکس::Dcmf R_att = میٹرکس::Quatf(attitude->q);
	میٹرکس::Vector3f a{sensor->accelerometer_m_s2[0], sensor->accelerometer_m_s2[1], sensor->accelerometer_m_s2[2]};
	میٹرکس::سمتیہ<اعشاری, 3> u;
	u = R_att * a;
	_u_z = u(2) + CONSTANTS_ONE_G; // compensate for gravity

	// dynamics matrix
	میٹرکس::میٹرکس_2<اعشاری, n_x, n_x> A;
	A.سیٹ_Zero();
	A(0, 1) = 1;
	A(1, 2) = 1;

	// input matrix
	میٹرکس::میٹرکس_2<اعشاری, n_x, 1>  B;
	B.سیٹ_Zero();
	B(1, 0) = 1;

	// input noise variance
	اعشاری R = 0.135f;

	// process noise convariance
	میٹرکس::میٹرکس_2<اعشاری, n_x, n_x>  Q;
	Q(0, 0) = 0;
	Q(1, 1) = 0;

	// do prediction
	میٹرکس::سمتیہ<اعشاری, n_x>  dx = (A * _x) * dt;
	dx(1) += B(1, 0) * _u_z * dt;

	// propagate state and covariance matrix
	_x += dx;
	_P += (A * _P + _P * A.transpose() +
	       B * R * B.transpose() + Q) * dt;
}

خالی TerrainEstimator::measurement_update(uint64_t وقت_ref, ثابت ساخت sensor_gnss_s *gnss,
		ثابت ساخت فاصلہ_sensor_s *فاصلہ,
		ثابت ساخت vehicle_attitude_s *attitude)
{
	// terrain estimate is invalid if we have range sensor timeout
	اگر (وقت_ref - فاصلہ->timestamp > فاصلہ_TIMEOUT) {
		_terrain_valid = جھوٹ;
	}

	اگر (فاصلہ->timestamp > وقت_آخری_فاصلہ) {
		میٹرکس::Quatf q(attitude->q);
		میٹرکس::Eulerf euler(q);
		اعشاری d = فاصلہ->current_فاصلہ;

		میٹرکس::میٹرکس_2<اعشاری, 1, n_x> C;
		C(0, 0) = -1; // measured altitude,

		اعشاری R = 0.009f;

		میٹرکس::سمتیہ<اعشاری, 1> y;
		y(0) = d * cosf(euler.phi()) * cosf(euler.theta());

		// residual
		میٹرکس::میٹرکس_2<اعشاری, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = میٹرکس::inv<اعشاری, 1> (S_I);
		میٹرکس::سمتیہ<اعشاری, 1> r = y - C * _x;

		میٹرکس::میٹرکس_2<اعشاری, n_x, 1> K = _P * C.transpose() * S_I;

		// some sort of outlayer rejection
		اگر (fabsf(فاصلہ->current_فاصلہ - فاصلہ_آخری) < 1.0f) {
			_x += K * r;
			_P -= K * C * _P;
		}

		// if the current and the last range measurement are bad then we consider the terrain
		// estimate to be invalid
		اگر (!is_فاصلہ_valid(فاصلہ->current_فاصلہ) && !is_فاصلہ_valid(فاصلہ_آخری)) {
			_terrain_valid = جھوٹ;

		} ورنہ {
			_terrain_valid = سچ;
		}

		وقت_آخری_فاصلہ = فاصلہ->timestamp;
		فاصلہ_آخری = فاصلہ->current_فاصلہ;
	}

	اگر (gnss->timestamp > وقت_آخری_gnss && gnss->fix_type >= 3) {
		میٹرکس::میٹرکس_2<اعشاری, 1, n_x> C;
		C(0, 1) = 1;

		اعشاری R = 0.056f;

		میٹرکس::سمتیہ<اعشاری, 1> y;
		y(0) = gnss->vel_نیچے_کو;

		// residual
		میٹرکس::میٹرکس_2<اعشاری, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = میٹرکس::inv<اعشاری, 1>(S_I);
		میٹرکس::سمتیہ<اعشاری, 1> r = y - C * _x;

		میٹرکس::میٹرکس_2<اعشاری, n_x, 1> K = _P * C.transpose() * S_I;
		_x += K * r;
		_P -= K * C * _P;

		وقت_آخری_gnss = gnss->timestamp;
	}

	// reinitialise filter if we find bad data
	اگر (!_x.isAllFinite() || !_P.isAllFinite()) {
		_x.zero();
		_P.سیٹ_Zero();
		_P(0, 0) = _P(1, 1) = _P(2, 2) = 0.1f;
	}

}
