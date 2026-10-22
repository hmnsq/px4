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
ढाँचा<typename Type, माप_प्रकार M, माप_प्रकार N>
तार्किक geninv(स्थिरांक Matrix<Type, M, N> &G, Matrix<Type, N, M> &res)
{
	माप_प्रकार rank;

	जर (M <= N) {
		SquareMatrix<Type, M> A = G * G.transpose();
		SquareMatrix<Type, M> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, M> X;

		जर (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			परत असत्य; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = G.transpose() * (L * A);

	} अन्यथा {
		SquareMatrix<Type, N> A = G.transpose() * G;
		SquareMatrix<Type, N> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, N> X;

		जर (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			परत असत्य; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = (L * A) * G.transpose();
	}

	परत सत्य;
}


ढाँचा<typename Type>
Type typeEpsilon();

ढाँचा<> ओळीत
दशांश typeEpsilon<दशांश>()
{
	परत भग्न_अंतर;
}

/**
 * Full rank Cholesky factorization of A
 */
ढाँचा<typename Type, माप_प्रकार N>
SquareMatrix<Type, N> fullRankCholesky(स्थिरांक SquareMatrix<Type, N> &A,
				       माप_प्रकार &rank)
{
	// Loses one ulp accuracy per row of diag, relative to largest magnitude
	स्थिरांक Type tol = N * typeEpsilon<Type>() * A.diag().max();

	Matrix<Type, N, N> L;

	माप_प्रकार r = 0;

	साठी (माप_प्रकार k = 0; k < N; k++) {

		जर (r == 0) {
			साठी (माप_प्रकार i = k; i < N; i++) {
				L(i, r) = A(i, k);
			}

		} अन्यथा {
			साठी (माप_प्रकार i = k; i < N; i++) {
				// Compute LL = L[k:n, :r] * L[k, :r].T
				Type LL = Type();

				साठी (माप_प्रकार j = 0; j < r; j++) {
					LL += L(i, j) * L(k, j);
				}

				L(i, r) = A(i, k) - LL;
			}
		}

		जर (L(k, r) > tol) {
			L(k, r) = std::वर्ग_मूल(L(k, r));

			जर (k < N - 1) {
				साठी (माप_प्रकार i = k + 1; i < N; i++) {
					L(i, r) = L(i, r) / L(k, r);
				}
			}

			r = r + 1;
		}
	}

	// Return rank
	rank = r;

	परत L;
}

} // namespace matrix
