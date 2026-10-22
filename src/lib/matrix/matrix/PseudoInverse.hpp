/**
 * @file PseudoInverse.hpp
 *
 * Implementation of matrix pseudo inverse
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 * @author Julian Kent <julian@auterion.com>
 */

#pragma once

#شامل "SquareMatrix.hpp"
#شامل "Vector.hpp"

فضاینام matrix
{

/**
 * Geninv
 * Fast pseudoinverse based on full rank cholesky factorisation
 *
 * Courrieu, P. (2008). Fast Computation of Moore-Penrose Inverse Matrices, 8(2), 25–29. http://arxiv.org/abs/0804.4809
 */
template<typename Type, size_t M, size_t N>
منطقی geninv(ثابت Matrix<Type, M, N> &G, Matrix<Type, N, M> &res)
{
	size_t rank;

	اگر (M <= N) {
		SquareMatrix<Type, M> A = G * G.transpose();
		SquareMatrix<Type, M> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, M> X;

		اگر (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			بازگشت نادرست; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = G.transpose() * (L * A);

	} وگرنه {
		SquareMatrix<Type, N> A = G.transpose() * G;
		SquareMatrix<Type, N> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, N> X;

		اگر (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			بازگشت نادرست; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = (L * A) * G.transpose();
	}

	بازگشت درست;
}


template<typename Type>
Type typeEpsilon();

template<> درونخطی
اعشاری typeEpsilon<اعشاری>()
{
	بازگشت FLT_EPSILON;
}

/**
 * Full rank Cholesky factorization of A
 */
template<typename Type, size_t N>
SquareMatrix<Type, N> fullRankCholesky(ثابت SquareMatrix<Type, N> &A,
				       size_t &rank)
{
	// Loses one ulp accuracy per row of diag, relative to largest magnitude
	ثابت Type tol = N * typeEpsilon<Type>() * A.diag().max();

	Matrix<Type, N, N> L;

	size_t r = 0;

	برای (size_t k = 0; k < N; k++) {

		اگر (r == 0) {
			برای (size_t i = k; i < N; i++) {
				L(i, r) = A(i, k);
			}

		} وگرنه {
			برای (size_t i = k; i < N; i++) {
				// Compute LL = L[k:n, :r] * L[k, :r].T
				Type LL = Type();

				برای (size_t j = 0; j < r; j++) {
					LL += L(i, j) * L(k, j);
				}

				L(i, r) = A(i, k) - LL;
			}
		}

		اگر (L(k, r) > tol) {
			L(k, r) = std::sqrt(L(k, r));

			اگر (k < N - 1) {
				برای (size_t i = k + 1; i < N; i++) {
					L(i, r) = L(i, r) / L(k, r);
				}
			}

			r = r + 1;
		}
	}

	// Return rank
	rank = r;

	بازگشت L;
}

} // namespace matrix
