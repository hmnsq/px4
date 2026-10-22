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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/matrix/math.hpp>

ਸਾਂਚਾ<size_t N, size_t M, size_t D>
ਵਰਗ ArxRls final
{
ਜਨਤਕ:
	ArxRls()
	{
		static_assert(N >= M, "The transfer function needs to be proper");

		reset();
	}

	~ArxRls() = ਮੂਲ;

	ਖਾਲੀ ਸੈੱਟ_Forgetting_ਗੁਣਕ(ਦਸ਼ਮਲਵ ਸਮਾਂ_constant_2, ਦਸ਼ਮਲਵ dt) { _lambda = 1.f - dt / ਸਮਾਂ_constant_2; }
	ਖਾਲੀ ਸੈੱਟ_Forgetting_ਗੁਣਕ(ਦਸ਼ਮਲਵ lambda) { _lambda = lambda; }

	/*
	 * return the vector of estimated parameters
	 * [a_1 .. a_n b_0 .. b_m]'
	 */
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼ < ਦਸ਼ਮਲਵ, N + M + 1 > &getCoefficients() ਅਚਲ { ਵਾਪਸ _theta_hat; }
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼ < ਦਸ਼ਮਲਵ, N + M + 1 > getVariances() ਅਚਲ { ਵਾਪਸ _P.diag(); }
	ਦਸ਼ਮਲਵ getInnovation() ਅਚਲ { ਵਾਪਸ _innovation; }
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼ < ਦਸ਼ਮਲਵ, N + M + 1 > &getDiffEstimate() ਅਚਲ { ਵਾਪਸ _diff_theta_hat; }

	ਖਾਲੀ reset(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼ < ਦਸ਼ਮਲਵ, N + M + 1 > &theta_init = {})
	{
		/* _P.uncorrelateCovarianceSetVariance<N + M + 1>(0, 10e3f); // does not work */
		_P.ਸੈੱਟ_Zero();

		ਲਈ (size_t i = 0; i < (N + M + 1); i++) {
			_P(i, i) = 10e3f;
		}

		_diff_theta_hat.ਸੈੱਟ_Zero();

		_theta_hat = theta_init;

		ਲਈ (size_t i = 0; i < M + D + 1; i++) {
			_u[i] = 0.f;
		}

		ਲਈ (size_t i = 0; i < N + 1; i++) {
			_y[i] = 0.f;
		}

		_nb_samples = 0;
		_innovation = 0.f;
	}

	ਖਾਲੀ update(ਦਸ਼ਮਲਵ u, ਦਸ਼ਮਲਵ y)
	{
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼ < ਦਸ਼ਮਲਵ, N + M + 1 > theta_prev = _theta_hat;

		ਜੋੜੋ_ਨਿਵੇਸ਼_Output(u, y);

		ਜੇ (!is_ਬਫਰ_ਭਰਿਆ()) {
			// Do not start to update the RLS algorithm when the
			// buffer still contains zeros
			ਵਾਪਸ;
		}

		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼ < ਦਸ਼ਮਲਵ, N + M + 1 > phi = construct_Design_ਸਦਿਸ਼();
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2 < ਦਸ਼ਮਲਵ, 1, N + M + 1 > phi_t = phi.transpose();

		_P = (_P - _P * phi * phi_t * _P / (_lambda + (phi_t * _P * phi)(0, 0))) / _lambda;
		_innovation = _y[N] - (phi_t * _theta_hat)(0, 0);
		_theta_hat = _theta_hat + _P * phi * _innovation;

		ਲਈ (size_t i = 0; i < N + M + 1; i++) {
			_diff_theta_hat(i) = fabsf(_theta_hat(i) - theta_prev(i));
		}

		/* fixCovarianceErrors(); // TODO: this could help against ill-conditioned matrix but needs more testing*/
	}

ਨਿਜੀ:
	ਖਾਲੀ ਜੋੜੋ_ਨਿਵੇਸ਼_Output(ਦਸ਼ਮਲਵ u, ਦਸ਼ਮਲਵ y)
	{
		ਖਿਸਕਾਓ_Registers();
		_u[M + D] = u;
		_y[N] = y;

		ਜੇ (!is_ਬਫਰ_ਭਰਿਆ()) {
			_nb_samples++;
		}
	}

	ਖਾਲੀ ਖਿਸਕਾਓ_Registers()
	{
		ਲਈ (size_t i = 0; i < N; i++) {
			_y[i] = _y[i + 1];
		}

		ਲਈ (size_t i = 0; i < (M + D); i++) {
			_u[i] = _u[i + 1];
		}
	}

	ਤਾਰਕਿਕ is_ਬਫਰ_ਭਰਿਆ() ਅਚਲ { ਵਾਪਸ _nb_samples > (M + N + D); }

	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼ < ਦਸ਼ਮਲਵ, N + M + 1 > construct_Design_ਸਦਿਸ਼() ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼ < ਦਸ਼ਮਲਵ, N + M + 1 > phi;

		ਲਈ (size_t i = 0; i < N; i++) {
			phi(i) = -_y[N - i - 1];
		}

		ਪੂਰਨ j = 0;

		ਲਈ (size_t i = N; i < (N + M + 1); i++) {
			phi(i) = _u[M - j];
			j++;
		}

		ਵਾਪਸ phi;
	}

	ਖਾਲੀ fixCovarianceErrors()
	{
		ਦਸ਼ਮਲਵ max_var = 0.f;

		ਲਈ (size_t i = 0; i < (N + M + 1); i++) {
			ਜੇ (_P(i, i) > max_var) {
				max_var = _P(i, i);
			}
		}

		ਅਚਲ ਦਸ਼ਮਲਵ min_var_allowed = max_var * 0.1f;

		ਲਈ (size_t i = 0; i < (N + M + 1); i++) {
			ਜੇ (_P(i, i) < min_var_allowed) {
				_P(i, i) = min_var_allowed;
			}
		}
	}

	ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ < ਦਸ਼ਮਲਵ, N + M + 1 > _P;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼ < ਦਸ਼ਮਲਵ, N + M + 1 > _theta_hat;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼ < ਦਸ਼ਮਲਵ, N + M + 1 > _diff_theta_hat;
	ਦਸ਼ਮਲਵ _innovation{};
	ਦਸ਼ਮਲਵ _u[M + D + 1] {};
	ਦਸ਼ਮਲਵ _y[N + 1] {};
	ਬਿਨਾਨਿਸ਼ਾਨੀ _nb_samples{0};
	ਦਸ਼ਮਲਵ _lambda{1.f};
};
