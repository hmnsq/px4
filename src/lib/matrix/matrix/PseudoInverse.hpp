/**
 * @file PseudoInverse.hpp
 *
 * Implementation of matrix pseudo inverse
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 * @author Julian Kent <julian@auterion.com>
 */

#ہدایت once

#ضمن "SquareMatrix.hpp"
#ضمن "Vector.hpp"

نطاق میٹرکس
{

/**
 * Geninv
 * Fast pseudoinverse based on full rank cholesky factorisation
 *
 * Courrieu, P. (2008). Fast Computation of Moore-Penrose Inverse Matrices, 8(2), 25–29. http://arxiv.org/abs/0804.4809
 */
سانچا<قسم_ناں Type, size_t M, size_t N>
منطقي geninv(ثابت میٹرکس_2<Type, M, N> &G, میٹرکس_2<Type, N, M> &res)
{
	size_t rank;

	إذا (M <= N) {
		مربع_میٹرکس<Type, M> A = G * G.transpose();
		مربع_میٹرکس<Type, M> L = بھریا_Rank_Cholesky(A, rank);

		A = L.transpose() * L;
		مربع_میٹرکس<Type, M> X;

		إذا (!inv(A, X, rank)) {
			res = میٹرکس_2<Type, N, M>();
			أرجع خطأ; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = G.transpose() * (L * A);

	} وإلا {
		مربع_میٹرکس<Type, N> A = G.transpose() * G;
		مربع_میٹرکس<Type, N> L = بھریا_Rank_Cholesky(A, rank);

		A = L.transpose() * L;
		مربع_میٹرکس<Type, N> X;

		إذا (!inv(A, X, rank)) {
			res = میٹرکس_2<Type, N, M>();
			أرجع خطأ; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = (L * A) * G.transpose();
	}

	أرجع سچ;
}


سانچا<قسم_ناں Type>
Type typeEpsilon();

سانچا<> مضمن
عائم typeEpsilon<عائم>()
{
	أرجع FLT_EPSILON;
}

/**
 * Full rank Cholesky factorization of A
 */
سانچا<قسم_ناں Type, size_t N>
مربع_میٹرکس<Type, N> بھریا_Rank_Cholesky(ثابت مربع_میٹرکس<Type, N> &A,
				       size_t &rank)
{
	// Loses one ulp accuracy per row of diag, relative to largest magnitude
	ثابت Type tol = N * typeEpsilon<Type>() * A.diag().max();

	میٹرکس_2<Type, N, N> L;

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
