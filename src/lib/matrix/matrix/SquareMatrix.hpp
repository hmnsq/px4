/**
 * @file SquareMatrix.hpp
 *
 * A square matrix
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#अन्तर्भाव <float.h> // FLT_EPSILON

#अन्तर्भाव "Slice.hpp"

नामाकाश matrix
{

template <typename Type, size_t  M>
वर्ग SquareMatrix : सार्वजनिक Matrix<Type, M, M>
{
सार्वजनिक:
	SquareMatrix() = स्वभाव;

	explicit SquareMatrix(ध्रुव Type data_[M][M]) :
		Matrix<Type, M, M>(data_)
	{
	}

	explicit SquareMatrix(ध्रुव Type data_[M * M]) :
		Matrix<Type, M, M>(data_)
	{
	}

	SquareMatrix(ध्रुव Matrix<Type, M, M> &other) :
		Matrix<Type, M, M>(other)
	{
	}

	using base = Matrix<Type, M, M>;
	using base::base;

	SquareMatrix<Type, M> &operator=(ध्रुव Matrix<Type, M, M> &other)
	{
		Matrix<Type, M, M>::operator=(other);
		प्रत्यागम *एतत्;
	}

	template <size_t P, size_t Q>
	SquareMatrix<Type, M> &operator=(ध्रुव Slice<Type, M, M, P, Q> &in_slice)
	{
		Matrix<Type, M, M>::operator=(in_slice);
		प्रत्यागम *एतत्;
	}

	template<size_t P, size_t Q>
	ConstSlice<Type, P, Q, M, M> slice(size_t x0, size_t y0) ध्रुव
	{
		प्रत्यागम {x0, y0, एतत्};
	}

	template<size_t P, size_t Q>
	Slice<Type, P, Q, M, M> slice(size_t x0, size_t y0)
	{
		प्रत्यागम {x0, y0, एतत्};
	}

	// inverse alias
	अन्तःस्थ SquareMatrix<Type, M> I() ध्रुव
	{
		SquareMatrix<Type, M> i;

		यदि (inv(*एतत्, i)) {
			प्रत्यागम i;

		} अन्यथा {
			i.setZero();
			प्रत्यागम i;
		}
	}

	// inverse alias
	अन्तःस्थ तार्किक I(SquareMatrix<Type, M> &i) ध्रुव
	{
		प्रत्यागम inv(*एतत्, i);
	}


	Vector<Type, M> diag() ध्रुव
	{
		Vector<Type, M> res;
		ध्रुव SquareMatrix<Type, M> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			res(i) = self(i, i);
		}

		प्रत्यागम res;
	}

	// get matrix upper right triangle in a row-major vector format
	Vector < Type, M *(M + 1) / 2 > upper_उजवा_triangle() ध्रुव
	{
		Vector < Type, M * (M + 1) / 2 > res;
		ध्रुव SquareMatrix<Type, M> &self = *एतत्;

		unsigned idx = 0;

		प्रति (size_t x = 0; x < M; x++) {
			प्रति (size_t y = x; y < M; y++) {
				res(idx) = self(x, y);
				++idx;
			}
		}

		प्रत्यागम res;
	}

	template <size_t Width>
	Type trace(size_t first) ध्रुव
	{
		static_assert(Width <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + Width <= M);

		Type res = 0;
		ध्रुव SquareMatrix<Type, M> &self = *एतत्;

		प्रति (size_t i = first; i < (first + Width); i++) {
			res += self(i, i);
		}

		प्रत्यागम res;
	}

	Type trace() ध्रुव
	{
		ध्रुव SquareMatrix<Type, M> &self = *एतत्;
		प्रत्यागम self.trace<M>(0);
	}

	// keep the sub covariance matrix and zero all covariance elements related
	// to the rest of the matrix
	template <size_t Width>
	शून्य uncorrelateCovarianceBlock(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + Width <= M);

		SquareMatrix<Type, M> &self = *एतत्;
		SquareMatrix<Type, Width> cov = self.slice<Width, Width>(first, first);
		self.slice<M, Width>(0, first) = 0.f;
		self.slice<Width, M>(first, 0) = 0.f;
		self.slice<Width, Width>(first, first) = cov;
	}

	// zero all offdiagonal elements and keep corresponding diagonal elements
	template <size_t Width>
	शून्य uncorrelateCovariance(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + Width <= M);

		SquareMatrix<Type, M> &self = *एतत्;
		Vector<Type, Width> diag_elements = self.slice<Width, Width>(first, first).diag();
		self.uncorrelateCovarianceSetVariance(first, diag_elements);
	}

	template <size_t Width>
	शून्य uncorrelateCovarianceSetVariance(size_t first, ध्रुव Vector<Type, Width> &vec);

	template <size_t Width>
	शून्य uncorrelateCovarianceSetVariance(size_t first, Type val);

	// make block diagonal symmetric by taking the average of the two corresponding off diagonal values
	template <size_t Width>
	शून्य makeBlockSymmetric(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + Width <= M);

		SquareMatrix<Type, M> &self = *एतत्;

		यदि (Width > 1) {
			प्रति (size_t row_idx = first + 1; row_idx < first + Width; row_idx++) {
				प्रति (size_t col_idx = first; col_idx < row_idx; col_idx++) {
					Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
					self(row_idx, col_idx) = tmp;
					self(col_idx, row_idx) = tmp;
				}
			}
		}
	}

	// make rows and columns symmetric by taking the average of the two corresponding off diagonal values
	template <size_t Width>
	शून्य makeRowColSymmetric(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + Width <= M);

		SquareMatrix<Type, M> &self = *एतत्;
		self.makeBlockSymmetric<Width>(first);

		प्रति (size_t row_idx = first; row_idx < first + Width; row_idx++) {
			प्रति (size_t col_idx = 0; col_idx < first; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}

			प्रति (size_t col_idx = first + Width; col_idx < M; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}
		}
	}

	// checks if block diagonal is symmetric
	template <size_t Width>
	तार्किक isBlockSymmetric(size_t first, ध्रुव Type eps = Type(1e-8f))
	{
		static_assert(Width <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + Width <= M);

		SquareMatrix<Type, M> &self = *एतत्;

		यदि (Width > 1) {
			प्रति (size_t row_idx = first + 1; row_idx < first + Width; row_idx++) {
				प्रति (size_t col_idx = first; col_idx < row_idx; col_idx++) {
					यदि (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
						प्रत्यागम असत्य;
					}
				}
			}
		}

		प्रत्यागम सत्य;
	}

	// checks if rows and columns are symmetric
	template <size_t Width>
	तार्किक isRowColSymmetric(size_t first, ध्रुव Type eps = Type(1e-8f))
	{
		static_assert(Width <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + Width <= M);

		SquareMatrix<Type, M> &self = *एतत्;

		प्रति (size_t row_idx = first; row_idx < first + Width; row_idx++) {
			प्रति (size_t col_idx = 0; col_idx < first; col_idx++) {
				यदि (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					प्रत्यागम असत्य;
				}
			}

			प्रति (size_t col_idx = first + Width; col_idx < M; col_idx++) {
				यदि (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					प्रत्यागम असत्य;
				}
			}
		}

		प्रत्यागम self.isBlockSymmetric<Width>(first, eps);
	}

	शून्य copyLowerToUpperTriangle()
	{
		SquareMatrix<Type, M> &self = *एतत्;

		प्रति (size_t row_idx = 1; row_idx < M; row_idx++) {
			प्रति (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(col_idx, row_idx) = self(row_idx, col_idx);
			}
		}
	}

	शून्य copyUpperToLowerTriangle()
	{
		SquareMatrix<Type, M> &self = *एतत्;

		प्रति (size_t row_idx = 1; row_idx < M; row_idx++) {
			प्रति (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(row_idx, col_idx) = self(col_idx, row_idx);
			}
		}
	}
};

// defined out of the class so they are not implicitly inline, they are large and called often
template<typename Type, size_t M>
template<size_t Width>
शून्य SquareMatrix<Type, M>::uncorrelateCovarianceSetVariance(size_t first, ध्रुव Vector<Type, Width> &vec)
{
	static_assert(Width <= M, "Width bigger than matrix");
	प्रतिज्ञा(first + Width <= M);

	SquareMatrix<Type, M> &self = *एतत्;
	// zero rows and columns
	self.slice<Width, M>(first, 0) = Type(0);
	self.slice<M, Width>(0, first) = Type(0);

	// set diagonals
	unsigned vec_idx = 0;

	प्रति (size_t idx = first; idx < first + Width; idx++) {
		self(idx, idx) = vec(vec_idx);
		vec_idx ++;
	}
}

template<typename Type, size_t M>
template<size_t Width>
शून्य SquareMatrix<Type, M>::uncorrelateCovarianceSetVariance(size_t first, Type val)
{
	static_assert(Width <= M, "Width bigger than matrix");
	प्रतिज्ञा(first + Width <= M);

	SquareMatrix<Type, M> &self = *एतत्;
	// zero rows and columns
	self.slice<Width, M>(first, 0) = Type(0);
	self.slice<M, Width>(0, first) = Type(0);

	// set diagonals
	प्रति (size_t idx = first; idx < first + Width; idx++) {
		self(idx, idx) = val;
	}
}

using SquareMatrix2f = SquareMatrix<भिन्न, 2>;
using SquareMatrix3f = SquareMatrix<भिन्न, 3>;
using SquareMatrix3d = SquareMatrix<द्विगुण, 3>;

template<typename Type, size_t M>
SquareMatrix<Type, M> eye()
{
	SquareMatrix<Type, M> m;
	m.setIdentity();
	प्रत्यागम m;
}

template<typename Type, size_t M>
SquareMatrix<Type, M> diag(Vector<Type, M> d)
{
	SquareMatrix<Type, M> m;

	प्रति (size_t i = 0; i < M; i++) {
		m(i, i) = d(i);
	}

	प्रत्यागम m;
}

template<typename Type, size_t M>
SquareMatrix<Type, M> expm(ध्रुव Matrix<Type, M, M> &A, size_t order = 5)
{
	SquareMatrix<Type, M> res;
	SquareMatrix<Type, M> A_pow = A;
	res.setIdentity();
	size_t i_factorial = 1;

	प्रति (size_t i = 1; i <= order; i++) {
		i_factorial *= i;
		res += A_pow / Type(i_factorial);
		A_pow *= A_pow;
	}

	प्रत्यागम res;
}

/**
 * Deal with the special case where the square matrix is 1
 */
template<typename Type>
तार्किक inv(ध्रुव SquareMatrix<Type, 1> &A, SquareMatrix<Type, 1> &inv, size_t rank = 1)
{
	यदि (std::fabs(A(0, 0)) < Type(FLT_EPSILON)) {
		प्रत्यागम असत्य;
	}

	inv(0, 0) = Type(1) / A(0, 0);
	प्रत्यागम सत्य;
}

/**
 * inverse based on LU factorization with partial pivotting
 */
template<typename Type, size_t M>
तार्किक inv(ध्रुव SquareMatrix<Type, M> &A, SquareMatrix<Type, M> &inv, size_t rank = M)
{
	SquareMatrix<Type, M> L;
	L.setIdentity();
	SquareMatrix<Type, M> U = A;
	SquareMatrix<Type, M> P;
	P.setIdentity();

	//printf("A:\n"); A.print();

	// for all diagonal elements
	प्रति (size_t n = 0; n < rank; n++) {

		// if diagonal is zero, swap with row below
		यदि (std::fabs(U(n, n)) < Type(FLT_EPSILON)) {
			//printf("trying pivot for row %d\n",n);
			प्रति (size_t i = n + 1; i < rank; i++) {

				//printf("\ttrying row %d\n",i);
				यदि (std::fabs(U(i, n)) > Type(FLT_EPSILON)) {
					//printf("swapped %d\n",i);
					U.swapRows(i, n);
					P.swapRows(i, n);
					L.swapRows(i, n);
					L.swapCols(i, n);
					भङ्ग;
				}
			}
		}

#ifdef MATRIX_ASSERT
		//printf("A:\n"); A.print();
		//printf("U:\n"); U.print();
		//printf("P:\n"); P.print();
		//fflush(stdout);
		//ASSERT(fabs(U(n, n)) > 1e-8f);
#endif

		// failsafe, return zero matrix
		यदि (std::fabs(static_cast<भिन्न>(U(n, n))) < FLT_EPSILON) {
			प्रत्यागम असत्य;
		}

		// for all rows below diagonal
		प्रति (size_t i = (n + 1); i < rank; i++) {
			L(i, n) = U(i, n) / U(n, n);

			// add i-th row and n-th row
			// multiplied by: -a(i,n)/a(n,n)
			प्रति (size_t k = n; k < rank; k++) {
				U(i, k) -= L(i, n) * U(n, k);
			}
		}
	}

	//printf("L:\n"); L.print();
	//printf("U:\n"); U.print();

	// solve LY=P*I for Y by forward subst
	//SquareMatrix<Type, M> Y = P;

	// for all columns of Y
	प्रति (size_t c = 0; c < rank; c++) {
		// for all rows of L
		प्रति (size_t i = 0; i < rank; i++) {
			// for all columns of L
			प्रति (size_t j = 0; j < i; j++) {
				// for all existing y
				// subtract the component they
				// contribute to the solution
				P(i, c) -= L(i, j) * P(j, c);
			}

			// divide by the factor
			// on current
			// term to be solved
			// Y(i,c) /= L(i,i);
			// but L(i,i) = 1.0
		}
	}

	//printf("Y:\n"); Y.print();

	// solve Ux=y for x by back subst
	//SquareMatrix<Type, M> X = Y;

	// for all columns of X
	प्रति (size_t c = 0; c < rank; c++) {
		// for all rows of U
		प्रति (size_t k = 0; k < rank; k++) {
			// have to go in reverse order
			size_t i = rank - 1 - k;

			// for all columns of U
			प्रति (size_t j = i + 1; j < rank; j++) {
				// for all existing x
				// subtract the component they
				// contribute to the solution
				P(i, c) -= U(i, j) * P(j, c);
			}

			// divide by the factor
			// on current
			// term to be solved
			//
			// we know that U(i, i) != 0 from above
			P(i, c) /= U(i, i);
		}
	}

	//check sanity of results
	प्रति (size_t i = 0; i < rank; i++) {
		प्रति (size_t j = 0; j < rank; j++) {
			यदि (!std::isfinite(P(i, j))) {
				प्रत्यागम असत्य;
			}
		}
	}

	//printf("X:\n"); X.print();
	inv = P;
	प्रत्यागम सत्य;
}

template<typename Type>
तार्किक inv(ध्रुव SquareMatrix<Type, 2> &A, SquareMatrix<Type, 2> &inv)
{
	Type det = A(0, 0) * A(1, 1) - A(1, 0) * A(0, 1);

	यदि (std::fabs(static_cast<भिन्न>(det)) < FLT_EPSILON || !std::isfinite(det)) {
		प्रत्यागम असत्य;
	}

	inv(0, 0) = A(1, 1);
	inv(1, 0) = -A(1, 0);
	inv(0, 1) = -A(0, 1);
	inv(1, 1) = A(0, 0);
	inv /= det;
	प्रत्यागम सत्य;
}

template<typename Type>
तार्किक inv(ध्रुव SquareMatrix<Type, 3> &A, SquareMatrix<Type, 3> &inv)
{
	Type det = A(0, 0) * (A(1, 1) * A(2, 2) - A(2, 1) * A(1, 2)) -
		   A(0, 1) * (A(1, 0) * A(2, 2) - A(1, 2) * A(2, 0)) +
		   A(0, 2) * (A(1, 0) * A(2, 1) - A(1, 1) * A(2, 0));

	यदि (std::fabs(static_cast<भिन्न>(det)) < FLT_EPSILON || !std::isfinite(det)) {
		प्रत्यागम असत्य;
	}

	inv(0, 0) = A(1, 1) * A(2, 2) - A(2, 1) * A(1, 2);
	inv(0, 1) = A(0, 2) * A(2, 1) - A(0, 1) * A(2, 2);
	inv(0, 2) = A(0, 1) * A(1, 2) - A(0, 2) * A(1, 1);
	inv(1, 0) = A(1, 2) * A(2, 0) - A(1, 0) * A(2, 2);
	inv(1, 1) = A(0, 0) * A(2, 2) - A(0, 2) * A(2, 0);
	inv(1, 2) = A(1, 0) * A(0, 2) - A(0, 0) * A(1, 2);
	inv(2, 0) = A(1, 0) * A(2, 1) - A(2, 0) * A(1, 1);
	inv(2, 1) = A(2, 0) * A(0, 1) - A(0, 0) * A(2, 1);
	inv(2, 2) = A(0, 0) * A(1, 1) - A(1, 0) * A(0, 1);
	inv /= det;
	प्रत्यागम सत्य;
}

/**
 * inverse based on LU factorization with partial pivotting
 */
template<typename Type, size_t M>
SquareMatrix<Type, M> inv(ध्रुव SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> i;

	यदि (inv(A, i)) {
		प्रत्यागम i;

	} अन्यथा {
		i.setZero();
		प्रत्यागम i;
	}
}

/**
 * cholesky decomposition
 *
 * Note: A must be positive definite
 */
template<typename Type, size_t M>
SquareMatrix <Type, M> cholesky(ध्रुव SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> L;

	प्रति (size_t j = 0; j < M; j++) {
		प्रति (size_t i = j; i < M; i++) {
			यदि (i == j) {
				भिन्न sum = 0;

				प्रति (size_t k = 0; k < j; k++) {
					sum += L(j, k) * L(j, k);
				}

				Type res = A(j, j) - sum;

				यदि (res <= 0) {
					L(j, j) = 0;

				} अन्यथा {
					L(j, j) = std::sqrt(res);
				}

			} अन्यथा {
				भिन्न sum = 0;

				प्रति (size_t k = 0; k < j; k++) {
					sum += L(i, k) * L(j, k);
				}

				यदि (L(j, j) <= 0) {
					L(i, j) = 0;

				} अन्यथा {
					L(i, j) = (A(i, j) - sum) / L(j, j);
				}
			}
		}
	}

	प्रत्यागम L;
}

/**
 * cholesky inverse
 *
 * TODO: Check if gaussian elimination jumps straight to back-substitution
 * for L or we need to do it manually. Will impact speed otherwise.
 */
template<typename Type, size_t M>
SquareMatrix <Type, M> choleskyInv(ध्रुव SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> L_inv = inv(cholesky(A));
	प्रत्यागम L_inv.T() * L_inv;
}

using Matrix2f = SquareMatrix<भिन्न, 2>;
using Matrix3f = SquareMatrix<भिन्न, 3>;
using Matrix3d = SquareMatrix<द्विगुण, 3>;

} // namespace matrix
