/**
 * @file PseudoInverse.hpp
 *
 * Implementation of matrix pseudo inverse
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 * @author Julian Kent <julian@auterion.com>
 */

#निर्देशः once

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
प्रतिरूपम्<प्रकारनाम Type, size_t M, size_t N>
तार्किक geninv(ध्रुव Matrix<Type, M, N> &G, Matrix<Type, N, M> &res)
{
	size_t rank;

	यदि (M <= N) {
		वर्गफलम्_Matrix<Type, M> A = G * G.transpose();
		वर्गफलम्_Matrix<Type, M> L = पूर्णम्_Rank_Cholesky(A, rank);

		A = L.transpose() * L;
		वर्गफलम्_Matrix<Type, M> X;

		यदि (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			प्रत्यागम असत्य; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = G.transpose() * (L * A);

	} अन्यथा {
		वर्गफलम्_Matrix<Type, N> A = G.transpose() * G;
		वर्गफलम्_Matrix<Type, N> L = पूर्णम्_Rank_Cholesky(A, rank);

		A = L.transpose() * L;
		वर्गफलम्_Matrix<Type, N> X;

		यदि (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			प्रत्यागम असत्य; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		A = X * X * L.transpose();
		res = (L * A) * G.transpose();
	}

	प्रत्यागम सत्य;
}


प्रतिरूपम्<प्रकारनाम Type>
Type typeEpsilon();

प्रतिरूपम्<> अन्तःस्थ
भिन्न typeEpsilon<भिन्न>()
{
	प्रत्यागम FLT_EPSILON;
}

/**
 * Full rank Cholesky factorization of A
 */
प्रतिरूपम्<प्रकारनाम Type, size_t N>
वर्गफलम्_Matrix<Type, N> पूर्णम्_Rank_Cholesky(ध्रुव वर्गफलम्_Matrix<Type, N> &A,
				       size_t &rank)
{
	// Loses one ulp accuracy per row of diag, relative to largest magnitude
	ध्रुव Type tol = N * typeEpsilon<Type>() * A.diag().max();

	Matrix<Type, N, N> L;

	size_t r = 0;

	प्रति (size_t k = 0; k < N; k++) {

		यदि (r == 0) {
			प्रति (size_t i = k; i < N; i++) {
				L(i, r) = A(i, k);
			}

		} अन्यथा {
			प्रति (size_t i = k; i < N; i++) {
				// Compute LL = L[k:n, :r] * L[k, :r].T
				Type LL = Type();

				प्रति (size_t j = 0; j < r; j++) {
					LL += L(i, j) * L(k, j);
				}

				L(i, r) = A(i, k) - LL;
			}
		}

		यदि (L(k, r) > tol) {
			L(k, r) = std::वर्गमूलम्(L(k, r));

			यदि (k < N - 1) {
				प्रति (size_t i = k + 1; i < N; i++) {
					L(i, r) = L(i, r) / L(k, r);
				}
			}

			r = r + 1;
		}
	}

	// Return rank
	rank = r;

	प्रत्यागम L;
}

} // namespace matrix
