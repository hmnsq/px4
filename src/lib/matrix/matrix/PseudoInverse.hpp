/**
 * @file PseudoInverse.hpp
 *
 * Implementation of matrix pseudo inverse
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 * @author Julian Kent <julian@auterion.com>
 */

#رهنمود once

#ضمن "SquareMatrix.hpp"
#ضمن "Vector.hpp"

نطاق ماتریس
{

/**
 * Geninv
 * Fast pseudoinverse based on full rank cholesky factorisation
 *
 * Courrieu, P. (2008). Fast Computation of Moore-Penrose Inverse Matrices, 8(2), 25–29. http://arxiv.org/abs/0804.4809
 */
قالب<نام_نوع Type, size_t M, size_t N>
منطقي geninv(ثابت ماتریس_2<Type, M, N> &G, ماتریس_2<Type, N, M> &res)
{
	size_t rank;

	إذا (M <= N) {
		مربع_ماتریس<Type, M> A = G * G.transpose();
		مربع_ماتریس<Type, M> L = پر_Rank_Cholesky(A, rank);

		A = L.transpose() * L;
		مربع_ماتریس<Type, M> X;

		إذا (!inv(A, X, rank)) {
			res = ماتریس_2<Type, N, M>();
			أرجع خطأ; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = G.transpose() * (L * A);

	} وإلا {
		مربع_ماتریس<Type, N> A = G.transpose() * G;
		مربع_ماتریس<Type, N> L = پر_Rank_Cholesky(A, rank);

		A = L.transpose() * L;
		مربع_ماتریس<Type, N> X;

		إذا (!inv(A, X, rank)) {
			res = ماتریس_2<Type, N, M>();
			أرجع خطأ; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = (L * A) * G.transpose();
	}

	أرجع درست;
}


قالب<نام_نوع Type>
Type typeEpsilon();

قالب<> مضمن
عائم typeEpsilon<عائم>()
{
	أرجع FLT_EPSILON;
}

/**
 * Full rank Cholesky factorization of A
 */
قالب<نام_نوع Type, size_t N>
مربع_ماتریس<Type, N> پر_Rank_Cholesky(ثابت مربع_ماتریس<Type, N> &A,
				       size_t &rank)
{
	// Loses one ulp accuracy per row of diag, relative to largest magnitude
	ثابت Type tol = N * typeEpsilon<Type>() * A.diag().max();

	ماتریس_2<Type, N, N> L;

	size_t r = 0;

	لكل (size_t k = 0; k < N; k++) {

		إذا (r == 0) {
			لكل (size_t i = k; i < N; i++) {
				L(i, r) = A(i, k);
			}

		} وإلا {
			لكل (size_t i = k; i < N; i++) {
				// Compute LL = L[k:n, :r] * L[k, :r].T
				Type LL = Type();

				لكل (size_t j = 0; j < r; j++) {
					LL += L(i, j) * L(k, j);
				}

				L(i, r) = A(i, k) - LL;
			}
		}

		إذا (L(k, r) > tol) {
			L(k, r) = std::جذر(L(k, r));

			إذا (k < N - 1) {
				لكل (size_t i = k + 1; i < N; i++) {
					L(i, r) = L(i, r) / L(k, r);
				}
			}

			r = r + 1;
		}
	}

	// Return rank
	rank = r;

	أرجع L;
}

} // namespace matrix
