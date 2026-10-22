/**
 * @file PseudoInverse.hpp
 *
 * Implementation of matrix pseudo inverse
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 * @author Julian Kent <julian@auterion.com>
 */

#pragma once

#அந்தர்பாவ "SquareMatrix.hpp"
#அந்தர்பாவ "Vector.hpp"

நாமாகாஶ matrix
{

/**
 * Geninv
 * Fast pseudoinverse based on full rank cholesky factorisation
 *
 * Courrieu, P. (2008). Fast Computation of Moore-Penrose Inverse Matrices, 8(2), 25–29. http://arxiv.org/abs/0804.4809
 */
template<typename Type, size_t M, size_t N>
தருக்க geninv(மாறிலி Matrix<Type, M, N> &G, Matrix<Type, N, M> &res)
{
	size_t rank;

	எனில் (M <= N) {
		SquareMatrix<Type, M> A = G * G.transpose();
		SquareMatrix<Type, M> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, M> X;

		எனில் (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			திரும்பு பொய்; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = G.transpose() * (L * A);

	} இல்லையெனில் {
		SquareMatrix<Type, N> A = G.transpose() * G;
		SquareMatrix<Type, N> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, N> X;

		எனில் (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			திரும்பு பொய்; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = (L * A) * G.transpose();
	}

	திரும்பு உண்மை;
}


template<typename Type>
Type typeEpsilon();

template<> உள்வரி
தசம typeEpsilon<தசம>()
{
	திரும்பு FLT_EPSILON;
}

/**
 * Full rank Cholesky factorization of A
 */
template<typename Type, size_t N>
SquareMatrix<Type, N> fullRankCholesky(மாறிலி SquareMatrix<Type, N> &A,
				       size_t &rank)
{
	// Loses one ulp accuracy per row of diag, relative to largest magnitude
	மாறிலி Type tol = N * typeEpsilon<Type>() * A.diag().max();

	Matrix<Type, N, N> L;

	size_t r = 0;

	க்கு (size_t k = 0; k < N; k++) {

		எனில் (r == 0) {
			க்கு (size_t i = k; i < N; i++) {
				L(i, r) = A(i, k);
			}

		} இல்லையெனில் {
			க்கு (size_t i = k; i < N; i++) {
				// Compute LL = L[k:n, :r] * L[k, :r].T
				Type LL = Type();

				க்கு (size_t j = 0; j < r; j++) {
					LL += L(i, j) * L(k, j);
				}

				L(i, r) = A(i, k) - LL;
			}
		}

		எனில் (L(k, r) > tol) {
			L(k, r) = std::sqrt(L(k, r));

			எனில் (k < N - 1) {
				க்கு (size_t i = k + 1; i < N; i++) {
					L(i, r) = L(i, r) / L(k, r);
				}
			}

			r = r + 1;
		}
	}

	// Return rank
	rank = r;

	திரும்பு L;
}

} // namespace matrix
