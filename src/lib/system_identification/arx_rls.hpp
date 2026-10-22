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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>

ಮಾದರಿ<size_t N, size_t M, size_t D>
ವರ್ಗ ArxRls final
{
ಸಾರ್ವಜನಿಕ:
	ArxRls()
	{
		static_assert(N >= M, "The transfer function needs to be proper");

		reset();
	}

	~ArxRls() = ಪೂರ್ವನಿಯೋಜಿತ;

	ಶೂನ್ಯ ಗಣ_Forgetting_ಗುಣಕ(ದಶಮಾಂಶ ಸಮಯ_constant_2, ದಶಮಾಂಶ dt) { _lambda = 1.f - dt / ಸಮಯ_constant_2; }
	ಶೂನ್ಯ ಗಣ_Forgetting_ಗುಣಕ(ದಶಮಾಂಶ lambda) { _lambda = lambda; }

	/*
	 * return the vector of estimated parameters
	 * [a_1 .. a_n b_0 .. b_m]'
	 */
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ < ದಶಮಾಂಶ, N + M + 1 > &getCoefficients() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _theta_hat; }
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ < ದಶಮಾಂಶ, N + M + 1 > getVariances() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _P.diag(); }
	ದಶಮಾಂಶ getInnovation() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _innovation; }
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ < ದಶಮಾಂಶ, N + M + 1 > &getDiffEstimate() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _diff_theta_hat; }

	ಶೂನ್ಯ reset(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ < ದಶಮಾಂಶ, N + M + 1 > &theta_init = {})
	{
		/* _P.uncorrelateCovarianceSetVariance<N + M + 1>(0, 10e3f); // does not work */
		_P.ಗಣ_Zero();

		ಗಾಗಿ (size_t i = 0; i < (N + M + 1); i++) {
			_P(i, i) = 10e3f;
		}

		_diff_theta_hat.ಗಣ_Zero();

		_theta_hat = theta_init;

		ಗಾಗಿ (size_t i = 0; i < M + D + 1; i++) {
			_u[i] = 0.f;
		}

		ಗಾಗಿ (size_t i = 0; i < N + 1; i++) {
			_y[i] = 0.f;
		}

		_nb_samples = 0;
		_innovation = 0.f;
	}

	ಶೂನ್ಯ update(ದಶಮಾಂಶ u, ದಶಮಾಂಶ y)
	{
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ < ದಶಮಾಂಶ, N + M + 1 > theta_prev = _theta_hat;

		ಕೂಡಿಸು_ಒಳಹರಿವು_Output(u, y);

		ಆದರೆ (!is_ಬಫರ್_ತುಂಬಿದ()) {
			// Do not start to update the RLS algorithm when the
			// buffer still contains zeros
			ಹಿಂತಿರುಗಿ;
		}

		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ < ದಶಮಾಂಶ, N + M + 1 > phi = construct_Design_ಸದಿಶ();
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಮಾತೃಕೆ_2 < ದಶಮಾಂಶ, 1, N + M + 1 > phi_t = phi.transpose();

		_P = (_P - _P * phi * phi_t * _P / (_lambda + (phi_t * _P * phi)(0, 0))) / _lambda;
		_innovation = _y[N] - (phi_t * _theta_hat)(0, 0);
		_theta_hat = _theta_hat + _P * phi * _innovation;

		ಗಾಗಿ (size_t i = 0; i < N + M + 1; i++) {
			_diff_theta_hat(i) = fabsf(_theta_hat(i) - theta_prev(i));
		}

		/* fixCovarianceErrors(); // TODO: this could help against ill-conditioned matrix but needs more testing*/
	}

ಖಾಸಗಿ:
	ಶೂನ್ಯ ಕೂಡಿಸು_ಒಳಹರಿವು_Output(ದಶಮಾಂಶ u, ದಶಮಾಂಶ y)
	{
		ಸರಿಸು_Registers();
		_u[M + D] = u;
		_y[N] = y;

		ಆದರೆ (!is_ಬಫರ್_ತುಂಬಿದ()) {
			_nb_samples++;
		}
	}

	ಶೂನ್ಯ ಸರಿಸು_Registers()
	{
		ಗಾಗಿ (size_t i = 0; i < N; i++) {
			_y[i] = _y[i + 1];
		}

		ಗಾಗಿ (size_t i = 0; i < (M + D); i++) {
			_u[i] = _u[i + 1];
		}
	}

	ತಾರ್ಕಿಕ is_ಬಫರ್_ತುಂಬಿದ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _nb_samples > (M + N + D); }

	ಮಾತೃಕೆ::ಸದಿಶ < ದಶಮಾಂಶ, N + M + 1 > construct_Design_ಸದಿಶ() ಸ್ಥಿರಾಂಕ
	{
		ಮಾತೃಕೆ::ಸದಿಶ < ದಶಮಾಂಶ, N + M + 1 > phi;

		ಗಾಗಿ (size_t i = 0; i < N; i++) {
			phi(i) = -_y[N - i - 1];
		}

		ಪೂರ್ಣಾಂಕ j = 0;

		ಗಾಗಿ (size_t i = N; i < (N + M + 1); i++) {
			phi(i) = _u[M - j];
			j++;
		}

		ಹಿಂತಿರುಗಿ phi;
	}

	ಶೂನ್ಯ fixCovarianceErrors()
	{
		ದಶಮಾಂಶ max_var = 0.f;

		ಗಾಗಿ (size_t i = 0; i < (N + M + 1); i++) {
			ಆದರೆ (_P(i, i) > max_var) {
				max_var = _P(i, i);
			}
		}

		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ min_var_allowed = max_var * 0.1f;

		ಗಾಗಿ (size_t i = 0; i < (N + M + 1); i++) {
			ಆದರೆ (_P(i, i) < min_var_allowed) {
				_P(i, i) = min_var_allowed;
			}
		}
	}

	ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ < ದಶಮಾಂಶ, N + M + 1 > _P;
	ಮಾತೃಕೆ::ಸದಿಶ < ದಶಮಾಂಶ, N + M + 1 > _theta_hat;
	ಮಾತೃಕೆ::ಸದಿಶ < ದಶಮಾಂಶ, N + M + 1 > _diff_theta_hat;
	ದಶಮಾಂಶ _innovation{};
	ದಶಮಾಂಶ _u[M + D + 1] {};
	ದಶಮಾಂಶ _y[N + 1] {};
	ಚಿಹ್ನೆರಹಿತ _nb_samples{0};
	ದಶಮಾಂಶ _lambda{1.f};
};
