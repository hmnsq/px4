/**
 * @file LeastSquaresSolver.hpp
 *
 * Least Squares Solver using QR householder decomposition.
 * It calculates x for Ax = b.
 * A = Q*R
 * where R is an upper triangular matrix.
 *
 * R*x = Q^T*b
 * This is efficiently solved for x because of the upper triangular property of R.
 *
 * @author Bart Slinger <bartslinger@gmail.com>
 */

#pragma once

#अन्तर्भाव "Vector.hpp"

नामाकाश matrix
{

ढाँचा<typename Type, माप_प्रकार M, माप_प्रकार N>
वर्ग LeastSquaresSolver
{
सार्वजनिक:

	/**
	 * @brief Class calculates QR decomposition which can be used for linear
	 * least squares
	 * @param A Matrix of size MxN
	 *
	 * Initialize the class with a MxN matrix. The constructor starts the
	 * QR decomposition. This class does not check the rank of the matrix.
	 * The user needs to make sure that rank(A) = N and M >= N.
	 */
	LeastSquaresSolver(ध्रुव Matrix<Type, M, N> &A)
	{
		static_assert(M >= N, "Matrix dimension should be M >= N");

		// Copy contentents of matrix A
		_A = A;

		प्रति (माप_प्रकार j = 0; j < N; j++) {
			Type normx = Type(0);

			प्रति (माप_प्रकार i = j; i < M; i++) {
				normx += _A(i, j) * _A(i, j);
			}

			normx = std::वर्ग_मूल(normx);
			Type s = _A(j, j) > 0 ? Type(-1) : Type(1);
			Type u1 = _A(j, j) - s * normx;

			// prevent divide by zero
			// also covers u1. normx is never negative
			यदि (normx < Type(1e-8)) {
				भङ्ग;
			}

			Type w[M] = {};
			w[0] = Type(1);

			प्रति (माप_प्रकार i = j + 1; i < M; i++) {
				w[i - j] = _A(i, j) / u1;
				_A(i, j) = w[i - j];
			}

			_A(j, j) = s * normx;
			_tau(j) = -s * u1 / normx;

			प्रति (माप_प्रकार k = j + 1; k < N; k++) {
				Type tmp = Type(0);

				प्रति (माप_प्रकार i = j; i < M; i++) {
					tmp += w[i - j] * _A(i, k);
				}

				प्रति (माप_प्रकार i = j; i < M; i++) {
					_A(i, k) -= _tau(j) * w[i - j] * tmp;
				}
			}

		}
	}

	/**
	 * @brief qtb Calculate Q^T * b
	 * @param b
	 * @return Q^T*b
	 *
	 * This function calculates Q^T * b. This is useful for the solver
	 * because R*x = Q^T*b.
	 */
	Vector<Type, M> qtb(ध्रुव Vector<Type, M> &b)
	{
		Vector<Type, M> qtbv = b;

		प्रति (माप_प्रकार j = 0; j < N; j++) {
			Type w[M];
			w[0] = Type(1);

			// fill vector w
			प्रति (माप_प्रकार i = j + 1; i < M; i++) {
				w[i - j] = _A(i, j);
			}

			Type tmp = Type(0);

			प्रति (माप_प्रकार i = j; i < M; i++) {
				tmp += w[i - j] * qtbv(i);
			}

			प्रति (माप_प्रकार i = j; i < M; i++) {
				qtbv(i) -= _tau(j) * w[i - j] * tmp;
			}
		}

		प्रत्यागम qtbv;
	}

	/**
	 * @brief Solve Ax=b for x
	 * @param b
	 * @return Vector x
	 *
	 * Find x in the equation Ax = b.
	 * A is provided in the initializer of the class.
	 */
	Vector<Type, N> solve(ध्रुव Vector<Type, M> &b)
	{
		Vector<Type, M> qtbv = qtb(b);
		Vector<Type, N> x;

		// size_t is unsigned and wraps i = 0 - 1 to i > N
		प्रति (माप_प्रकार i = N - 1; i < N; i--) {
			म_लिखो("i %d\n", static_cast<पूर्णाङ्क>(i));
			x(i) = qtbv(i);

			प्रति (माप_प्रकार r = i + 1; r < N; r++) {
				x(i) -= _A(i, r) * x(r);
			}

			// divide by zero, return vector of zeros
			यदि (isEqualF(_A(i, i), Type(0), Type(1e-8))) {
				प्रति (माप_प्रकार z = 0; z < N; z++) {
					x(z) = Type(0);
				}

				भङ्ग;
			}

			x(i) /= _A(i, i);
		}

		प्रत्यागम x;
	}

गुप्त:
	Matrix<Type, M, N> _A;
	Vector<Type, N> _tau;

};

} // namespace matrix
