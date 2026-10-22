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

#অন্তর্ভাব <matrix/matrix/math.hpp>

template<size_t N, size_t M, size_t D>
বর্গ ArxRls final
{
সার্বজনিক:
	ArxRls()
	{
		static_assert(N >= M, "The transfer function needs to be proper");

		reset();
	}

	~ArxRls() = স্বভাব;

	শূন্য setForgettingFactor(ভিন্ন time_constant, ভিন্ন dt) { _lambda = 1.f - dt / time_constant; }
	শূন্য setForgettingFactor(ভিন্ন lambda) { _lambda = lambda; }

	/*
	 * return the vector of estimated parameters
	 * [a_1 .. a_n b_0 .. b_m]'
	 */
	ধ্রুব matrix::Vector < ভিন্ন, N + M + 1 > &getCoefficients() ধ্রুব { প্রত্যাগম _theta_hat; }
	ধ্রুব matrix::Vector < ভিন্ন, N + M + 1 > getVariances() ধ্রুব { প্রত্যাগম _P.diag(); }
	ভিন্ন getInnovation() ধ্রুব { প্রত্যাগম _innovation; }
	ধ্রুব matrix::Vector < ভিন্ন, N + M + 1 > &getDiffEstimate() ধ্রুব { প্রত্যাগম _diff_theta_hat; }

	শূন্য reset(ধ্রুব matrix::Vector < ভিন্ন, N + M + 1 > &theta_init = {})
	{
		/* _P.uncorrelateCovarianceSetVariance<N + M + 1>(0, 10e3f); // does not work */
		_P.setZero();

		প্রতি (size_t i = 0; i < (N + M + 1); i++) {
			_P(i, i) = 10e3f;
		}

		_diff_theta_hat.setZero();

		_theta_hat = theta_init;

		প্রতি (size_t i = 0; i < M + D + 1; i++) {
			_u[i] = 0.f;
		}

		প্রতি (size_t i = 0; i < N + 1; i++) {
			_y[i] = 0.f;
		}

		_nb_samples = 0;
		_innovation = 0.f;
	}

	শূন্য update(ভিন্ন u, ভিন্ন y)
	{
		ধ্রুব matrix::Vector < ভিন্ন, N + M + 1 > theta_prev = _theta_hat;

		add_প্রবেশ_Output(u, y);

		যদি (!isBufferFull()) {
			// Do not start to update the RLS algorithm when the
			// buffer still contains zeros
			প্রত্যাগম;
		}

		ধ্রুব matrix::Vector < ভিন্ন, N + M + 1 > phi = constructDesignVector();
		ধ্রুব matrix::Matrix < ভিন্ন, 1, N + M + 1 > phi_t = phi.transpose();

		_P = (_P - _P * phi * phi_t * _P / (_lambda + (phi_t * _P * phi)(0, 0))) / _lambda;
		_innovation = _y[N] - (phi_t * _theta_hat)(0, 0);
		_theta_hat = _theta_hat + _P * phi * _innovation;

		প্রতি (size_t i = 0; i < N + M + 1; i++) {
			_diff_theta_hat(i) = fabsf(_theta_hat(i) - theta_prev(i));
		}

		/* fixCovarianceErrors(); // TODO: this could help against ill-conditioned matrix but needs more testing*/
	}

গুপ্ত:
	শূন্য add_প্রবেশ_Output(ভিন্ন u, ভিন্ন y)
	{
		shiftRegisters();
		_u[M + D] = u;
		_y[N] = y;

		যদি (!isBufferFull()) {
			_nb_samples++;
		}
	}

	শূন্য shiftRegisters()
	{
		প্রতি (size_t i = 0; i < N; i++) {
			_y[i] = _y[i + 1];
		}

		প্রতি (size_t i = 0; i < (M + D); i++) {
			_u[i] = _u[i + 1];
		}
	}

	তার্কিক isBufferFull() ধ্রুব { প্রত্যাগম _nb_samples > (M + N + D); }

	matrix::Vector < ভিন্ন, N + M + 1 > constructDesignVector() ধ্রুব
	{
		matrix::Vector < ভিন্ন, N + M + 1 > phi;

		প্রতি (size_t i = 0; i < N; i++) {
			phi(i) = -_y[N - i - 1];
		}

		পূর্ণাঙ্ক j = 0;

		প্রতি (size_t i = N; i < (N + M + 1); i++) {
			phi(i) = _u[M - j];
			j++;
		}

		প্রত্যাগম phi;
	}

	শূন্য fixCovarianceErrors()
	{
		ভিন্ন max_var = 0.f;

		প্রতি (size_t i = 0; i < (N + M + 1); i++) {
			যদি (_P(i, i) > max_var) {
				max_var = _P(i, i);
			}
		}

		ধ্রুব ভিন্ন min_var_allowed = max_var * 0.1f;

		প্রতি (size_t i = 0; i < (N + M + 1); i++) {
			যদি (_P(i, i) < min_var_allowed) {
				_P(i, i) = min_var_allowed;
			}
		}
	}

	matrix::SquareMatrix < ভিন্ন, N + M + 1 > _P;
	matrix::Vector < ভিন্ন, N + M + 1 > _theta_hat;
	matrix::Vector < ভিন্ন, N + M + 1 > _diff_theta_hat;
	ভিন্ন _innovation{};
	ভিন্ন _u[M + D + 1] {};
	ভিন্ন _y[N + 1] {};
	unsigned _nb_samples{0};
	ভিন্ন _lambda{1.f};
};
