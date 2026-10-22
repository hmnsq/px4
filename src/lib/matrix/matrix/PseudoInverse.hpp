/**
 * @file PseudoInverse.hpp
 *
 * Implementation of matrix pseudo inverse
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 * @author Julian Kent <julian@auterion.com>
 */

#pragma once

#અન્તર્ભાવ "SquareMatrix.hpp"
#અન્તર્ભાવ "Vector.hpp"

નામાકાશ matrix
{

/**
 * Geninv
 * Fast pseudoinverse based on full rank cholesky factorisation
 *
 * Courrieu, P. (2008). Fast Computation of Moore-Penrose Inverse Matrices, 8(2), 25–29. http://arxiv.org/abs/0804.4809
 */
template<typename Type, size_t M, size_t N>
તાર્કિક geninv(અચલ Matrix<Type, M, N> &G, Matrix<Type, N, M> &res)
{
	size_t rank;

	જો (M <= N) {
		SquareMatrix<Type, M> A = G * G.transpose();
		SquareMatrix<Type, M> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, M> X;

		જો (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			પાછા ખોટું; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = G.transpose() * (L * A);

	} નહીંતર {
		SquareMatrix<Type, N> A = G.transpose() * G;
		SquareMatrix<Type, N> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, N> X;

		જો (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			પાછા ખોટું; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = (L * A) * G.transpose();
	}

	પાછા સાચું;
}


template<typename Type>
Type typeEpsilon();

template<> અંતઃરેખ
દશાંશ typeEpsilon<દશાંશ>()
{
	પાછા FLT_EPSILON;
}

/**
 * Full rank Cholesky factorization of A
 */
template<typename Type, size_t N>
SquareMatrix<Type, N> fullRankCholesky(અચલ SquareMatrix<Type, N> &A,
				       size_t &rank)
{
	// Loses one ulp accuracy per row of diag, relative to largest magnitude
	અચલ Type tol = N * typeEpsilon<Type>() * A.diag().max();

	Matrix<Type, N, N> L;

	size_t r = 0;

	માટે (size_t k = 0; k < N; k++) {

		જો (r == 0) {
			માટે (size_t i = k; i < N; i++) {
				L(i, r) = A(i, k);
			}

		} નહીંતર {
			માટે (size_t i = k; i < N; i++) {
				// Compute LL = L[k:n, :r] * L[k, :r].T
				Type LL = Type();

				માટે (size_t j = 0; j < r; j++) {
					LL += L(i, j) * L(k, j);
				}

				L(i, r) = A(i, k) - LL;
			}
		}

		જો (L(k, r) > tol) {
			L(k, r) = std::sqrt(L(k, r));

			જો (k < N - 1) {
				માટે (size_t i = k + 1; i < N; i++) {
					L(i, r) = L(i, r) / L(k, r);
				}
			}

			r = r + 1;
		}
	}

	// Return rank
	rank = r;

	પાછા L;
}

} // namespace matrix
