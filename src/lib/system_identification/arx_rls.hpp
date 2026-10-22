/****************************************************************************
 *
 *   Copyright (c) 2020-2021 PX4 Development Team. All rights reserved.
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
 * @file arx_rls.hpp
 * @brief Efficient recursive weighted least-squares algorithm without matrix inversion
 *
 * Assumes an ARX (autoregressive) model:
 * A(q^-1)y(k) = q^-d * B(q^-1)u(k) + A(q^-1)e(k)
 *
 * with:
 * q^-i  backward shift operator
 * A(q^-1) = 1 + a_1*q^-1 +...+ a_n*q^-n
 * B(q^-1) = b_0 + b_1*q^-1...+ b_m*q^-m
 * n  order of A(q^-1)
 * m  order of B(q^-1)
 * d  delay
 * u  input of the system
 * y  output of the system
 * e  white noise input
 *
 * References:
 * - Identification de systemes dynamiques, D.Bonvin and A.Karimi, epfl, 2011
 *
 * @author Mathieu Bresciani <mathieu@auterion.com>
 */

#సూచన once

#అన్తర్భావ <matrix/matrix/math.hpp>

మూస<size_t N, size_t M, size_t D>
తరగతి ArxRls final
{
బహిరంగ:
	ArxRls()
	{
		static_assert(N >= M, "The transfer function needs to be proper");

		reset();
	}

	~ArxRls() = అప్రమేయ;

	శూన్య సమితి_Forgetting_గుణకం(దశాంశం సమయం_constant_2, దశాంశం dt) { _lambda = 1.f - dt / సమయం_constant_2; }
	శూన్య సమితి_Forgetting_గుణకం(దశాంశం lambda) { _lambda = lambda; }

	/*
	 * return the vector of estimated parameters
	 * [a_1 .. a_n b_0 .. b_m]'
	 */
	స్థిరరాశి మాతృక::సదిశ < దశాంశం, N + M + 1 > &getCoefficients() స్థిరరాశి { తిరిగి _theta_hat; }
	స్థిరరాశి మాతృక::సదిశ < దశాంశం, N + M + 1 > getVariances() స్థిరరాశి { తిరిగి _P.diag(); }
	దశాంశం getInnovation() స్థిరరాశి { తిరిగి _innovation; }
	స్థిరరాశి మాతృక::సదిశ < దశాంశం, N + M + 1 > &getDiffEstimate() స్థిరరాశి { తిరిగి _diff_theta_hat; }

	శూన్య reset(స్థిరరాశి మాతృక::సదిశ < దశాంశం, N + M + 1 > &theta_init = {})
	{
		/* _P.uncorrelateCovarianceSetVariance<N + M + 1>(0, 10e3f); // does not work */
		_P.సమితి_Zero();

		కొరకు (size_t i = 0; i < (N + M + 1); i++) {
			_P(i, i) = 10e3f;
		}

		_diff_theta_hat.సమితి_Zero();

		_theta_hat = theta_init;

		కొరకు (size_t i = 0; i < M + D + 1; i++) {
			_u[i] = 0.f;
		}

		కొరకు (size_t i = 0; i < N + 1; i++) {
			_y[i] = 0.f;
		}

		_nb_samples = 0;
		_innovation = 0.f;
	}

	శూన్య update(దశాంశం u, దశాంశం y)
	{
		స్థిరరాశి మాతృక::సదిశ < దశాంశం, N + M + 1 > theta_prev = _theta_hat;

		కూడు_ఆదానం_Output(u, y);

		ఒకవేళ (!is_బఫరు_నిండిన()) {
			// Do not start to update the RLS algorithm when the
			// buffer still contains zeros
			తిరిగి;
		}

		స్థిరరాశి మాతృక::సదిశ < దశాంశం, N + M + 1 > phi = construct_Design_సదిశ();
		స్థిరరాశి మాతృక::మాతృక_2 < దశాంశం, 1, N + M + 1 > phi_t = phi.transpose();

		_P = (_P - _P * phi * phi_t * _P / (_lambda + (phi_t * _P * phi)(0, 0))) / _lambda;
		_innovation = _y[N] - (phi_t * _theta_hat)(0, 0);
		_theta_hat = _theta_hat + _P * phi * _innovation;

		కొరకు (size_t i = 0; i < N + M + 1; i++) {
			_diff_theta_hat(i) = fabsf(_theta_hat(i) - theta_prev(i));
		}

		/* fixCovarianceErrors(); // TODO: this could help against ill-conditioned matrix but needs more testing*/
	}

అంతరంగ:
	శూన్య కూడు_ఆదానం_Output(దశాంశం u, దశాంశం y)
	{
		జరుపు_Registers();
		_u[M + D] = u;
		_y[N] = y;

		ఒకవేళ (!is_బఫరు_నిండిన()) {
			_nb_samples++;
		}
	}

	శూన్య జరుపు_Registers()
	{
		కొరకు (size_t i = 0; i < N; i++) {
			_y[i] = _y[i + 1];
		}

		కొరకు (size_t i = 0; i < (M + D); i++) {
			_u[i] = _u[i + 1];
		}
	}

	తార్కిక is_బఫరు_నిండిన() స్థిరరాశి { తిరిగి _nb_samples > (M + N + D); }

	మాతృక::సదిశ < దశాంశం, N + M + 1 > construct_Design_సదిశ() స్థిరరాశి
	{
		మాతృక::సదిశ < దశాంశం, N + M + 1 > phi;

		కొరకు (size_t i = 0; i < N; i++) {
			phi(i) = -_y[N - i - 1];
		}

		పూర్ణాంకం j = 0;

		కొరకు (size_t i = N; i < (N + M + 1); i++) {
			phi(i) = _u[M - j];
			j++;
		}

		తిరిగి phi;
	}

	శూన్య fixCovarianceErrors()
	{
		దశాంశం max_var = 0.f;

		కొరకు (size_t i = 0; i < (N + M + 1); i++) {
			ఒకవేళ (_P(i, i) > max_var) {
				max_var = _P(i, i);
			}
		}

		స్థిరరాశి దశాంశం min_var_allowed = max_var * 0.1f;

		కొరకు (size_t i = 0; i < (N + M + 1); i++) {
			ఒకవేళ (_P(i, i) < min_var_allowed) {
				_P(i, i) = min_var_allowed;
			}
		}
	}

	మాతృక::వర్గం_మాతృక < దశాంశం, N + M + 1 > _P;
	మాతృక::సదిశ < దశాంశం, N + M + 1 > _theta_hat;
	మాతృక::సదిశ < దశాంశం, N + M + 1 > _diff_theta_hat;
	దశాంశం _innovation{};
	దశాంశం _u[M + D + 1] {};
	దశాంశం _y[N + 1] {};
	గుర్తులేని _nb_samples{0};
	దశాంశం _lambda{1.f};
};
