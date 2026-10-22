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

#సూచన once

#అన్తర్భావ "Vector.hpp"

నామాకాశ మాతృక
{

మూస<రకం_పేరు Type, size_t M, size_t N>
తరగతి కనిష్ఠ_Squares_Solver
{
బహిరంగ:

	/**
	 * @brief Class calculates QR decomposition which can be used for linear
	 * least squares
	 * @param A Matrix of size MxN
	 *
	 * Initialize the class with a MxN matrix. The constructor starts the
	 * QR decomposition. This class does not check the rank of the matrix.
	 * The user needs to make sure that rank(A) = N and M >= N.
	 */
	కనిష్ఠ_Squares_Solver(స్థిరరాశి మాతృక_2<Type, M, N> &A)
	{
		static_assert(M >= N, "Matrix dimension should be M >= N");

		// Copy contentents of matrix A
		_A = A;

		కొరకు (size_t j = 0; j < N; j++) {
			Type normx = Type(0);

			కొరకు (size_t i = j; i < M; i++) {
				normx += _A(i, j) * _A(i, j);
			}

			normx = std::వర్గమూలం(normx);
			Type s = _A(j, j) > 0 ? Type(-1) : Type(1);
			Type u1 = _A(j, j) - s * normx;

			// prevent divide by zero
			// also covers u1. normx is never negative
			ఒకవేళ (normx < Type(1e-8)) {
				ఆపు;
			}

			Type w[M] = {};
			w[0] = Type(1);

			కొరకు (size_t i = j + 1; i < M; i++) {
				w[i - j] = _A(i, j) / u1;
				_A(i, j) = w[i - j];
			}

			_A(j, j) = s * normx;
			_tau(j) = -s * u1 / normx;

			కొరకు (size_t k = j + 1; k < N; k++) {
				Type tmp = Type(0);

				కొరకు (size_t i = j; i < M; i++) {
					tmp += w[i - j] * _A(i, k);
				}

				కొరకు (size_t i = j; i < M; i++) {
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
	సదిశ<Type, M> qtb(స్థిరరాశి సదిశ<Type, M> &b)
	{
		సదిశ<Type, M> qtbv = b;

		కొరకు (size_t j = 0; j < N; j++) {
			Type w[M];
			w[0] = Type(1);

			// fill vector w
			కొరకు (size_t i = j + 1; i < M; i++) {
				w[i - j] = _A(i, j);
			}

			Type tmp = Type(0);

			కొరకు (size_t i = j; i < M; i++) {
				tmp += w[i - j] * qtbv(i);
			}

			కొరకు (size_t i = j; i < M; i++) {
				qtbv(i) -= _tau(j) * w[i - j] * tmp;
			}
		}

		తిరిగి qtbv;
	}

	/**
	 * @brief Solve Ax=b for x
	 * @param b
	 * @return Vector x
	 *
	 * Find x in the equation Ax = b.
	 * A is provided in the initializer of the class.
	 */
	సదిశ<Type, N> solve(స్థిరరాశి సదిశ<Type, M> &b)
	{
		సదిశ<Type, M> qtbv = qtb(b);
		సదిశ<Type, N> x;

		// size_t is unsigned and wraps i = 0 - 1 to i > N
		కొరకు (size_t i = N - 1; i < N; i--) {
			రాయి("i %d\n", static_cast<పూర్ణాంకం>(i));
			x(i) = qtbv(i);

			కొరకు (size_t r = i + 1; r < N; r++) {
				x(i) -= _A(i, r) * x(r);
			}

			// divide by zero, return vector of zeros
			ఒకవేళ (isEqualF(_A(i, i), Type(0), Type(1e-8))) {
				కొరకు (size_t z = 0; z < N; z++) {
					x(z) = Type(0);
				}

				ఆపు;
			}

			x(i) /= _A(i, i);
		}

		తిరిగి x;
	}

అంతరంగ:
	మాతృక_2<Type, M, N> _A;
	సదిశ<Type, N> _tau;

};

} // namespace matrix
