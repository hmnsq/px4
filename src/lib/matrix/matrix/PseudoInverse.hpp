/**
 * @file PseudoInverse.hpp
 *
 * Implementation of matrix pseudo inverse
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 * @author Julian Kent <julian@auterion.com>
 */

#pragma once

#অন্তর্ভাব "SquareMatrix.hpp"
#অন্তর্ভাব "Vector.hpp"

নামাকাশ matrix
{

/**
 * Geninv
 * Fast pseudoinverse based on full rank cholesky factorisation
 *
 * Courrieu, P. (2008). Fast Computation of Moore-Penrose Inverse Matrices, 8(2), 25–29. http://arxiv.org/abs/0804.4809
 */
template<typename Type, size_t M, size_t N>
তার্কিক geninv(ধ্রুব Matrix<Type, M, N> &G, Matrix<Type, N, M> &res)
{
	size_t rank;

	যদি (M <= N) {
		SquareMatrix<Type, M> A = G * G.transpose();
		SquareMatrix<Type, M> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, M> X;

		যদি (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			প্রত্যাগম অসত্য; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = G.transpose() * (L * A);

	} অন্যথা {
		SquareMatrix<Type, N> A = G.transpose() * G;
		SquareMatrix<Type, N> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, N> X;

		যদি (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			প্রত্যাগম অসত্য; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = (L * A) * G.transpose();
	}

	প্রত্যাগম সত্য;
}


template<typename Type>
Type typeEpsilon();

template<> অন্তঃস্থ
ভিন্ন typeEpsilon<ভিন্ন>()
{
	প্রত্যাগম FLT_EPSILON;
}

/**
 * Full rank Cholesky factorization of A
 */
template<typename Type, size_t N>
SquareMatrix<Type, N> fullRankCholesky(ধ্রুব SquareMatrix<Type, N> &A,
				       size_t &rank)
{
	// Loses one ulp accuracy per row of diag, relative to largest magnitude
	ধ্রুব Type tol = N * typeEpsilon<Type>() * A.diag().max();

	Matrix<Type, N, N> L;

	size_t r = 0;

	প্রতি (size_t k = 0; k < N; k++) {

		যদি (r == 0) {
			প্রতি (size_t i = k; i < N; i++) {
				L(i, r) = A(i, k);
			}

		} অন্যথা {
			প্রতি (size_t i = k; i < N; i++) {
				// Compute LL = L[k:n, :r] * L[k, :r].T
				Type LL = Type();

				প্রতি (size_t j = 0; j < r; j++) {
					LL += L(i, j) * L(k, j);
				}

				L(i, r) = A(i, k) - LL;
			}
		}

		যদি (L(k, r) > tol) {
			L(k, r) = std::sqrt(L(k, r));

			যদি (k < N - 1) {
				প্রতি (size_t i = k + 1; i < N; i++) {
					L(i, r) = L(i, r) / L(k, r);
				}
			}

			r = r + 1;
		}
	}

	// Return rank
	rank = r;

	প্রত্যাগম L;
}

} // namespace matrix
