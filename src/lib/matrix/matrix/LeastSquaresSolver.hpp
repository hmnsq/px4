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

#ضمن "Vector.hpp"

نطاق matrix
{

template<typename Type, size_t M, size_t N>
صنف LeastSquaresSolver
{
عام:

	/**
	 * @brief Class calculates QR decomposition which can be used for linear
	 * least squares
	 * @param A Matrix of size MxN
	 *
	 * Initialize the class with a MxN matrix. The constructor starts the
	 * QR decomposition. This class does not check the rank of the matrix.
	 * The user needs to make sure that rank(A) = N and M >= N.
	 */
	LeastSquaresSolver(ثابت Matrix<Type, M, N> &A)
	{
		static_assert(M >= N, "Matrix dimension should be M >= N");

		// Copy contentents of matrix A
		_A = A;

		لكل (size_t j = 0; j < N; j++) {
			Type normx = Type(0);

			لكل (size_t i = j; i < M; i++) {
				normx += _A(i, j) * _A(i, j);
			}

			normx = std::sqrt(normx);
			Type s = _A(j, j) > 0 ? Type(-1) : Type(1);
			Type u1 = _A(j, j) - s * normx;

			// prevent divide by zero
			// also covers u1. normx is never negative
			𐡄𐡍 (normx < Type(1e-8)) {
				اكسر;
			}

			Type w[M] = {};
			w[0] = Type(1);

			لكل (size_t i = j + 1; i < M; i++) {
				w[i - j] = _A(i, j) / u1;
				_A(i, j) = w[i - j];
			}

			_A(j, j) = s * normx;
			_tau(j) = -s * u1 / normx;

			لكل (size_t k = j + 1; k < N; k++) {
				Type tmp = Type(0);

				لكل (size_t i = j; i < M; i++) {
					tmp += w[i - j] * _A(i, k);
				}

				لكل (size_t i = j; i < M; i++) {
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
	Vector<Type, M> qtb(ثابت Vector<Type, M> &b)
	{
		Vector<Type, M> qtbv = b;

		لكل (size_t j = 0; j < N; j++) {
			Type w[M];
			w[0] = Type(1);

			// fill vector w
			لكل (size_t i = j + 1; i < M; i++) {
				w[i - j] = _A(i, j);
			}

			Type tmp = Type(0);

			لكل (size_t i = j; i < M; i++) {
				tmp += w[i - j] * qtbv(i);
			}

			لكل (size_t i = j; i < M; i++) {
				qtbv(i) -= _tau(j) * w[i - j] * tmp;
			}
		}

		أرجع qtbv;
	}

	/**
	 * @brief Solve Ax=b for x
	 * @param b
	 * @return Vector x
	 *
	 * Find x in the equation Ax = b.
	 * A is provided in the initializer of the class.
	 */
	Vector<Type, N> solve(ثابت Vector<Type, M> &b)
	{
		Vector<Type, M> qtbv = qtb(b);
		Vector<Type, N> x;

		// size_t is unsigned and wraps i = 0 - 1 to i > N
		لكل (size_t i = N - 1; i < N; i--) {
			printf("i %d\n", static_cast<صحيح>(i));
			x(i) = qtbv(i);

			لكل (size_t r = i + 1; r < N; r++) {
				x(i) -= _A(i, r) * x(r);
			}

			// divide by zero, return vector of zeros
			𐡄𐡍 (isEqualF(_A(i, i), Type(0), Type(1e-8))) {
				لكل (size_t z = 0; z < N; z++) {
					x(z) = Type(0);
				}

				اكسر;
			}

			x(i) /= _A(i, i);
		}

		أرجع x;
	}

خاص:
	Matrix<Type, M, N> _A;
	Vector<Type, N> _tau;

};

} // namespace matrix
