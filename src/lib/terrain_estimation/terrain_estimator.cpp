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

#అన్తర్భావ "terrain_estimator.h"
#అన్తర్భావ <lib/geo/geo.h>
#అన్తర్భావ <px4_platform_common/defines.h>

#నిర్వచించు దూరం_TIMEOUT 100000		// time in usec after which laser is considered dead

TerrainEstimator::TerrainEstimator() :
	దూరం_చివరి(0.0f),
	_terrain_valid(అబద్ధం),
	సమయం_చివరి_దూరం(0),
	సమయం_చివరి_gnss(0)
{
	_x.zero();
	_u_z = 0.0f;
	_P.సమితి_తటస్థం();
}

తార్కిక TerrainEstimator::is_దూరం_valid(దశాంశం దూరం)
{
	తిరిగి (దూరం < 40.0f && దూరం > 0.00001f);
}

శూన్య TerrainEstimator::predict(దశాంశం dt, స్థిరరాశి నిర్మాణం vehicle_attitude_s *attitude,
			       స్థిరరాశి నిర్మాణం sensor_combined_s *sensor,
			       స్థిరరాశి నిర్మాణం దూరం_sensor_s *దూరం)
{
	మాతృక::Dcmf R_att = మాతృక::Quatf(attitude->q);
	మాతృక::Vector3f a{sensor->accelerometer_m_s2[0], sensor->accelerometer_m_s2[1], sensor->accelerometer_m_s2[2]};
	మాతృక::సదిశ<దశాంశం, 3> u;
	u = R_att * a;
	_u_z = u(2) + CONSTANTS_ONE_G; // compensate for gravity

	// dynamics matrix
	మాతృక::మాతృక_2<దశాంశం, n_x, n_x> A;
	A.సమితి_Zero();
	A(0, 1) = 1;
	A(1, 2) = 1;

	// input matrix
	మాతృక::మాతృక_2<దశాంశం, n_x, 1>  B;
	B.సమితి_Zero();
	B(1, 0) = 1;

	// input noise variance
	దశాంశం R = 0.135f;

	// process noise convariance
	మాతృక::మాతృక_2<దశాంశం, n_x, n_x>  Q;
	Q(0, 0) = 0;
	Q(1, 1) = 0;

	// do prediction
	మాతృక::సదిశ<దశాంశం, n_x>  dx = (A * _x) * dt;
	dx(1) += B(1, 0) * _u_z * dt;

	// propagate state and covariance matrix
	_x += dx;
	_P += (A * _P + _P * A.transpose() +
	       B * R * B.transpose() + Q) * dt;
}

శూన్య TerrainEstimator::measurement_update(uint64_t సమయం_ref, స్థిరరాశి నిర్మాణం sensor_gnss_s *gnss,
		స్థిరరాశి నిర్మాణం దూరం_sensor_s *దూరం,
		స్థిరరాశి నిర్మాణం vehicle_attitude_s *attitude)
{
	// terrain estimate is invalid if we have range sensor timeout
	ఒకవేళ (సమయం_ref - దూరం->timestamp > దూరం_TIMEOUT) {
		_terrain_valid = అబద్ధం;
	}

	ఒకవేళ (దూరం->timestamp > సమయం_చివరి_దూరం) {
		మాతృక::Quatf q(attitude->q);
		మాతృక::Eulerf euler(q);
		దశాంశం d = దూరం->current_దూరం;

		మాతృక::మాతృక_2<దశాంశం, 1, n_x> C;
		C(0, 0) = -1; // measured altitude,

		దశాంశం R = 0.009f;

		మాతృక::సదిశ<దశాంశం, 1> y;
		y(0) = d * cosf(euler.phi()) * cosf(euler.theta());

		// residual
		మాతృక::మాతృక_2<దశాంశం, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = మాతృక::inv<దశాంశం, 1> (S_I);
		మాతృక::సదిశ<దశాంశం, 1> r = y - C * _x;

		మాతృక::మాతృక_2<దశాంశం, n_x, 1> K = _P * C.transpose() * S_I;

		// some sort of outlayer rejection
		ఒకవేళ (fabsf(దూరం->current_దూరం - దూరం_చివరి) < 1.0f) {
			_x += K * r;
			_P -= K * C * _P;
		}

		// if the current and the last range measurement are bad then we consider the terrain
		// estimate to be invalid
		ఒకవేళ (!is_దూరం_valid(దూరం->current_దూరం) && !is_దూరం_valid(దూరం_చివరి)) {
			_terrain_valid = అబద్ధం;

		} లేకపోతే {
			_terrain_valid = నిజం;
		}

		సమయం_చివరి_దూరం = దూరం->timestamp;
		దూరం_చివరి = దూరం->current_దూరం;
	}

	ఒకవేళ (gnss->timestamp > సమయం_చివరి_gnss && gnss->fix_type >= 3) {
		మాతృక::మాతృక_2<దశాంశం, 1, n_x> C;
		C(0, 1) = 1;

		దశాంశం R = 0.056f;

		మాతృక::సదిశ<దశాంశం, 1> y;
		y(0) = gnss->vel_కిందికి;

		// residual
		మాతృక::మాతృక_2<దశాంశం, 1, 1> S_I = (C * _P * C.transpose());
		S_I(0, 0) += R;
		S_I = మాతృక::inv<దశాంశం, 1>(S_I);
		మాతృక::సదిశ<దశాంశం, 1> r = y - C * _x;

		మాతృక::మాతృక_2<దశాంశం, n_x, 1> K = _P * C.transpose() * S_I;
		_x += K * r;
		_P -= K * C * _P;

		సమయం_చివరి_gnss = gnss->timestamp;
	}

	// reinitialise filter if we find bad data
	ఒకవేళ (!_x.isAllFinite() || !_P.isAllFinite()) {
		_x.zero();
		_P.సమితి_Zero();
		_P(0, 0) = _P(1, 1) = _P(2, 2) = 0.1f;
	}

}
