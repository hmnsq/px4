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

#pragma once

#अन्तर्भाव <matrix/matrix/math.hpp>

ढाँचा<माप_प्रकार N, माप_प्रकार M, माप_प्रकार D>
वर्ग ArxRls final
{
सार्वजनिक:
	ArxRls()
	{
		static_assert(N >= M, "The transfer function needs to be proper");

		reset();
	}

	~ArxRls() = स्वभाव;

	शून्य setForgettingFactor(भिन्न time_constant, भिन्न dt) { _lambda = 1.f - dt / time_constant; }
	शून्य setForgettingFactor(भिन्न lambda) { _lambda = lambda; }

	/*
	 * return the vector of estimated parameters
	 * [a_1 .. a_n b_0 .. b_m]'
	 */
	अचर matrix::Vector < भिन्न, N + M + 1 > &getCoefficients() अचर { फर्काऊ _theta_hat; }
	अचर matrix::Vector < भिन्न, N + M + 1 > getVariances() अचर { फर्काऊ _P.diag(); }
	भिन्न getInnovation() अचर { फर्काऊ _innovation; }
	अचर matrix::Vector < भिन्न, N + M + 1 > &getDiffEstimate() अचर { फर्काऊ _diff_theta_hat; }

	शून्य reset(अचर matrix::Vector < भिन्न, N + M + 1 > &theta_init = {})
	{
		/* _P.uncorrelateCovarianceSetVariance<N + M + 1>(0, 10e3f); // does not work */
		_P.setZero();

		लागि (माप_प्रकार i = 0; i < (N + M + 1); i++) {
			_P(i, i) = 10e3f;
		}

		_diff_theta_hat.setZero();

		_theta_hat = theta_init;

		लागि (माप_प्रकार i = 0; i < M + D + 1; i++) {
			_u[i] = 0.f;
		}

		लागि (माप_प्रकार i = 0; i < N + 1; i++) {
			_y[i] = 0.f;
		}

		_nb_samples = 0;
		_innovation = 0.f;
	}

	शून्य update(भिन्न u, भिन्न y)
	{
		अचर matrix::Vector < भिन्न, N + M + 1 > theta_prev = _theta_hat;

		add_आगत_Output(u, y);

		यदि (!isBufferFull()) {
			// Do not start to update the RLS algorithm when the
			// buffer still contains zeros
			फर्काऊ;
		}

		अचर matrix::Vector < भिन्न, N + M + 1 > phi = constructDesignVector();
		अचर matrix::Matrix < भिन्न, 1, N + M + 1 > phi_t = phi.transpose();

		_P = (_P - _P * phi * phi_t * _P / (_lambda + (phi_t * _P * phi)(0, 0))) / _lambda;
		_innovation = _y[N] - (phi_t * _theta_hat)(0, 0);
		_theta_hat = _theta_hat + _P * phi * _innovation;

		लागि (माप_प्रकार i = 0; i < N + M + 1; i++) {
			_diff_theta_hat(i) = fabsf(_theta_hat(i) - theta_prev(i));
		}

		/* fixCovarianceErrors(); // TODO: this could help against ill-conditioned matrix but needs more testing*/
	}

गुप्त:
	शून्य add_आगत_Output(भिन्न u, भिन्न y)
	{
		shiftRegisters();
		_u[M + D] = u;
		_y[N] = y;

		यदि (!isBufferFull()) {
			_nb_samples++;
		}
	}

	शून्य shiftRegisters()
	{
		लागि (माप_प्रकार i = 0; i < N; i++) {
			_y[i] = _y[i + 1];
		}

		लागि (माप_प्रकार i = 0; i < (M + D); i++) {
			_u[i] = _u[i + 1];
		}
	}

	तार्किक isBufferFull() अचर { फर्काऊ _nb_samples > (M + N + D); }

	matrix::Vector < भिन्न, N + M + 1 > constructDesignVector() अचर
	{
		matrix::Vector < भिन्न, N + M + 1 > phi;

		लागि (माप_प्रकार i = 0; i < N; i++) {
			phi(i) = -_y[N - i - 1];
		}

		पूर्णाङ्क j = 0;

		लागि (माप_प्रकार i = N; i < (N + M + 1); i++) {
			phi(i) = _u[M - j];
			j++;
		}

		फर्काऊ phi;
	}

	शून्य fixCovarianceErrors()
	{
		भिन्न max_var = 0.f;

		लागि (माप_प्रकार i = 0; i < (N + M + 1); i++) {
			यदि (_P(i, i) > max_var) {
				max_var = _P(i, i);
			}
		}

		अचर भिन्न min_var_allowed = max_var * 0.1f;

		लागि (माप_प्रकार i = 0; i < (N + M + 1); i++) {
			यदि (_P(i, i) < min_var_allowed) {
				_P(i, i) = min_var_allowed;
			}
		}
	}

	matrix::SquareMatrix < भिन्न, N + M + 1 > _P;
	matrix::Vector < भिन्न, N + M + 1 > _theta_hat;
	matrix::Vector < भिन्न, N + M + 1 > _diff_theta_hat;
	भिन्न _innovation{};
	भिन्न _u[M + D + 1] {};
	भिन्न _y[N + 1] {};
	अचिन्हित _nb_samples{0};
	भिन्न _lambda{1.f};
};
