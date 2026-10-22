/**
 * @file PseudoInverse.hpp
 *
 * Implementation of matrix pseudo inverse
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 * @author Julian Kent <julian@auterion.com>
 */

#pragma once

#अन्तर्भाव "SquareMatrix.hpp"
#अन्तर्भाव "Vector.hpp"

नामाकाश matrix
{

/**
 * Geninv
 * Fast pseudoinverse based on full rank cholesky factorisation
 *
 * Courrieu, P. (2008). Fast Computation of Moore-Penrose Inverse Matrices, 8(2), 25–29. http://arxiv.org/abs/0804.4809
 */
template<typename Type, size_t M, size_t N>
तार्किक geninv(ध्रुव Matrix<Type, M, N> &G, Matrix<Type, N, M> &res)
{
	size_t rank;

	सचे (M <= N) {
		SquareMatrix<Type, M> A = G * G.transpose();
		SquareMatrix<Type, M> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, M> X;

		सचे (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			पटिनिवत्त मिच्छा; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = G.transpose() * (L * A);

	} अञ्ञथा {
		SquareMatrix<Type, N> A = G.transpose() * G;
		SquareMatrix<Type, N> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, N> X;

		सचे (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			पटिनिवत्त मिच्छा; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = (L * A) * G.transpose();
	}

	पटिनिवत्त सच्च;
}


template<typename Type>
Type typeEpsilon();

template<> अन्तःस्थ
भिन्न typeEpsilon<भिन्न>()
{
	पटिनिवत्त FLT_EPSILON;
}

/**
 * Full rank Cholesky factorization of A
 */
template<typename Type, size_t N>
SquareMatrix<Type, N> fullRankCholesky(ध्रुव SquareMatrix<Type, N> &A,
				       size_t &rank)
{
	// Loses one ulp accuracy per row of diag, relative to largest magnitude
	ध्रुव Type tol = N * typeEpsilon<Type>() * A.diag().max();

	Matrix<Type, N, N> L;

	size_t r = 0;

	अत्थाय (size_t k = 0; k < N; k++) {

		सचे (r == 0) {
			अत्थाय (size_t i = k; i < N; i++) {
				L(i, r) = A(i, k);
			}

		} अञ्ञथा {
			अत्थाय (size_t i = k; i < N; i++) {
				// Compute LL = L[k:n, :r] * L[k, :r].T
				Type LL = Type();

				अत्थाय (size_t j = 0; j < r; j++) {
					LL += L(i, j) * L(k, j);
				}

				L(i, r) = A(i, k) - LL;
			}
		}

		सचे (L(k, r) > tol) {
			L(k, r) = std::sqrt(L(k, r));

			सचे (k < N - 1) {
				अत्थाय (size_t i = k + 1; i < N; i++) {
					L(i, r) = L(i, r) / L(k, r);
				}
			}

			r = r + 1;
		}
	}

	// Return rank
	rank = r;

	पटिनिवत्त L;
}

} // namespace matrix
