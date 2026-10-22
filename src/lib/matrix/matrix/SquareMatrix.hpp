/**
 * @file SquareMatrix.hpp
 *
 * A square matrix
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ಅನ್ತರ್ಭಾವ <float.h> // FLT_EPSILON

#ಅನ್ತರ್ಭಾವ "Slice.hpp"

ನಾಮಾಕಾಶ matrix
{

template <typename Type, size_t  M>
ವರ್ಗ SquareMatrix : ಸಾರ್ವಜನಿಕ Matrix<Type, M, M>
{
ಸಾರ್ವಜನಿಕ:
	SquareMatrix() = ಸ್ವಭಾವ;

	explicit SquareMatrix(ಧ್ರುವ Type data_[M][M]) :
		Matrix<Type, M, M>(data_)
	{
	}

	explicit SquareMatrix(ಧ್ರುವ Type data_[M * M]) :
		Matrix<Type, M, M>(data_)
	{
	}

	SquareMatrix(ಧ್ರುವ Matrix<Type, M, M> &other) :
		Matrix<Type, M, M>(other)
	{
	}

	using base = Matrix<Type, M, M>;
	using base::base;

	SquareMatrix<Type, M> &operator=(ಧ್ರುವ Matrix<Type, M, M> &other)
	{
		Matrix<Type, M, M>::operator=(other);
		ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
	}

	template <size_t P, size_t Q>
	SquareMatrix<Type, M> &operator=(ಧ್ರುವ Slice<Type, M, M, P, Q> &in_slice)
	{
		Matrix<Type, M, M>::operator=(in_slice);
		ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
	}

	template<size_t P, size_t Q>
	ConstSlice<Type, P, Q, M, M> slice(size_t x0, size_t y0) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ {x0, y0, ಏತತ್};
	}

	template<size_t P, size_t Q>
	Slice<Type, P, Q, M, M> slice(size_t x0, size_t y0)
	{
		ಪ್ರತ್ಯಾಗಮ {x0, y0, ಏತತ್};
	}

	// inverse alias
	ಅನ್ತಃಸ್ಥ SquareMatrix<Type, M> I() ಧ್ರುವ
	{
		SquareMatrix<Type, M> i;

		ಯದಿ (inv(*ಏತತ್, i)) {
			ಪ್ರತ್ಯಾಗಮ i;

		} ಅನ್ಯಥಾ {
			i.setZero();
			ಪ್ರತ್ಯಾಗಮ i;
		}
	}

	// inverse alias
	ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ I(SquareMatrix<Type, M> &i) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ inv(*ಏತತ್, i);
	}


	Vector<Type, M> diag() ಧ್ರುವ
	{
		Vector<Type, M> res;
		ಧ್ರುವ SquareMatrix<Type, M> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < M; i++) {
			res(i) = self(i, i);
		}

		ಪ್ರತ್ಯಾಗಮ res;
	}

	// get matrix upper right triangle in a row-major vector format
	Vector < Type, M *(M + 1) / 2 > upper_ಬಲ_triangle() ಧ್ರುವ
	{
		Vector < Type, M * (M + 1) / 2 > res;
		ಧ್ರುವ SquareMatrix<Type, M> &self = *ಏತತ್;

		unsigned idx = 0;

		ಪ್ರತಿ (size_t x = 0; x < M; x++) {
			ಪ್ರತಿ (size_t y = x; y < M; y++) {
				res(idx) = self(x, y);
				++idx;
			}
		}

		ಪ್ರತ್ಯಾಗಮ res;
	}

	template <size_t Width>
	Type trace(size_t first) ಧ್ರುವ
	{
		static_assert(Width <= M, "Width bigger than matrix");
		ಪ್ರತಿಜ್ಞಾ(first + Width <= M);

		Type res = 0;
		ಧ್ರುವ SquareMatrix<Type, M> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = first; i < (first + Width); i++) {
			res += self(i, i);
		}

		ಪ್ರತ್ಯಾಗಮ res;
	}

	Type trace() ಧ್ರುವ
	{
		ಧ್ರುವ SquareMatrix<Type, M> &self = *ಏತತ್;
		ಪ್ರತ್ಯಾಗಮ self.trace<M>(0);
	}

	// keep the sub covariance matrix and zero all covariance elements related
	// to the rest of the matrix
	template <size_t Width>
	ಶೂನ್ಯ uncorrelateCovarianceBlock(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		ಪ್ರತಿಜ್ಞಾ(first + Width <= M);

		SquareMatrix<Type, M> &self = *ಏತತ್;
		SquareMatrix<Type, Width> cov = self.slice<Width, Width>(first, first);
		self.slice<M, Width>(0, first) = 0.f;
		self.slice<Width, M>(first, 0) = 0.f;
		self.slice<Width, Width>(first, first) = cov;
	}

	// zero all offdiagonal elements and keep corresponding diagonal elements
	template <size_t Width>
	ಶೂನ್ಯ uncorrelateCovariance(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		ಪ್ರತಿಜ್ಞಾ(first + Width <= M);

		SquareMatrix<Type, M> &self = *ಏತತ್;
		Vector<Type, Width> diag_elements = self.slice<Width, Width>(first, first).diag();
		self.uncorrelateCovarianceSetVariance(first, diag_elements);
	}

	template <size_t Width>
	ಶೂನ್ಯ uncorrelateCovarianceSetVariance(size_t first, ಧ್ರುವ Vector<Type, Width> &vec);

	template <size_t Width>
	ಶೂನ್ಯ uncorrelateCovarianceSetVariance(size_t first, Type val);

	// make block diagonal symmetric by taking the average of the two corresponding off diagonal values
	template <size_t Width>
	ಶೂನ್ಯ makeBlockSymmetric(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		ಪ್ರತಿಜ್ಞಾ(first + Width <= M);

		SquareMatrix<Type, M> &self = *ಏತತ್;

		ಯದಿ (Width > 1) {
			ಪ್ರತಿ (size_t row_idx = first + 1; row_idx < first + Width; row_idx++) {
				ಪ್ರತಿ (size_t col_idx = first; col_idx < row_idx; col_idx++) {
					Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
					self(row_idx, col_idx) = tmp;
					self(col_idx, row_idx) = tmp;
				}
			}
		}
	}

	// make rows and columns symmetric by taking the average of the two corresponding off diagonal values
	template <size_t Width>
	ಶೂನ್ಯ makeRowColSymmetric(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		ಪ್ರತಿಜ್ಞಾ(first + Width <= M);

		SquareMatrix<Type, M> &self = *ಏತತ್;
		self.makeBlockSymmetric<Width>(first);

		ಪ್ರತಿ (size_t row_idx = first; row_idx < first + Width; row_idx++) {
			ಪ್ರತಿ (size_t col_idx = 0; col_idx < first; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}

			ಪ್ರತಿ (size_t col_idx = first + Width; col_idx < M; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}
		}
	}

	// checks if block diagonal is symmetric
	template <size_t Width>
	ತಾರ್ಕಿಕ isBlockSymmetric(size_t first, ಧ್ರುವ Type eps = Type(1e-8f))
	{
		static_assert(Width <= M, "Width bigger than matrix");
		ಪ್ರತಿಜ್ಞಾ(first + Width <= M);

		SquareMatrix<Type, M> &self = *ಏತತ್;

		ಯದಿ (Width > 1) {
			ಪ್ರತಿ (size_t row_idx = first + 1; row_idx < first + Width; row_idx++) {
				ಪ್ರತಿ (size_t col_idx = first; col_idx < row_idx; col_idx++) {
					ಯದಿ (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
						ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
					}
				}
			}
		}

		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	// checks if rows and columns are symmetric
	template <size_t Width>
	ತಾರ್ಕಿಕ isRowColSymmetric(size_t first, ಧ್ರುವ Type eps = Type(1e-8f))
	{
		static_assert(Width <= M, "Width bigger than matrix");
		ಪ್ರತಿಜ್ಞಾ(first + Width <= M);

		SquareMatrix<Type, M> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t row_idx = first; row_idx < first + Width; row_idx++) {
			ಪ್ರತಿ (size_t col_idx = 0; col_idx < first; col_idx++) {
				ಯದಿ (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				}
			}

			ಪ್ರತಿ (size_t col_idx = first + Width; col_idx < M; col_idx++) {
				ಯದಿ (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				}
			}
		}

		ಪ್ರತ್ಯಾಗಮ self.isBlockSymmetric<Width>(first, eps);
	}

	ಶೂನ್ಯ copyLowerToUpperTriangle()
	{
		SquareMatrix<Type, M> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t row_idx = 1; row_idx < M; row_idx++) {
			ಪ್ರತಿ (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(col_idx, row_idx) = self(row_idx, col_idx);
			}
		}
	}

	ಶೂನ್ಯ copyUpperToLowerTriangle()
	{
		SquareMatrix<Type, M> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t row_idx = 1; row_idx < M; row_idx++) {
			ಪ್ರತಿ (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(row_idx, col_idx) = self(col_idx, row_idx);
			}
		}
	}
};

// defined out of the class so they are not implicitly inline, they are large and called often
template<typename Type, size_t M>
template<size_t Width>
ಶೂನ್ಯ SquareMatrix<Type, M>::uncorrelateCovarianceSetVariance(size_t first, ಧ್ರುವ Vector<Type, Width> &vec)
{
	static_assert(Width <= M, "Width bigger than matrix");
	ಪ್ರತಿಜ್ಞಾ(first + Width <= M);

	SquareMatrix<Type, M> &self = *ಏತತ್;
	// zero rows and columns
	self.slice<Width, M>(first, 0) = Type(0);
	self.slice<M, Width>(0, first) = Type(0);

	// set diagonals
	unsigned vec_idx = 0;

	ಪ್ರತಿ (size_t idx = first; idx < first + Width; idx++) {
		self(idx, idx) = vec(vec_idx);
		vec_idx ++;
	}
}

template<typename Type, size_t M>
template<size_t Width>
ಶೂನ್ಯ SquareMatrix<Type, M>::uncorrelateCovarianceSetVariance(size_t first, Type val)
{
	static_assert(Width <= M, "Width bigger than matrix");
	ಪ್ರತಿಜ್ಞಾ(first + Width <= M);

	SquareMatrix<Type, M> &self = *ಏತತ್;
	// zero rows and columns
	self.slice<Width, M>(first, 0) = Type(0);
	self.slice<M, Width>(0, first) = Type(0);

	// set diagonals
	ಪ್ರತಿ (size_t idx = first; idx < first + Width; idx++) {
		self(idx, idx) = val;
	}
}

using SquareMatrix2f = SquareMatrix<ಭಿನ್ನ, 2>;
using SquareMatrix3f = SquareMatrix<ಭಿನ್ನ, 3>;
using SquareMatrix3d = SquareMatrix<ದ್ವಿಗುಣ, 3>;

template<typename Type, size_t M>
SquareMatrix<Type, M> eye()
{
	SquareMatrix<Type, M> m;
	m.setIdentity();
	ಪ್ರತ್ಯಾಗಮ m;
}

template<typename Type, size_t M>
SquareMatrix<Type, M> diag(Vector<Type, M> d)
{
	SquareMatrix<Type, M> m;

	ಪ್ರತಿ (size_t i = 0; i < M; i++) {
		m(i, i) = d(i);
	}

	ಪ್ರತ್ಯಾಗಮ m;
}

template<typename Type, size_t M>
SquareMatrix<Type, M> expm(ಧ್ರುವ Matrix<Type, M, M> &A, size_t order = 5)
{
	SquareMatrix<Type, M> res;
	SquareMatrix<Type, M> A_pow = A;
	res.setIdentity();
	size_t i_factorial = 1;

	ಪ್ರತಿ (size_t i = 1; i <= order; i++) {
		i_factorial *= i;
		res += A_pow / Type(i_factorial);
		A_pow *= A_pow;
	}

	ಪ್ರತ್ಯಾಗಮ res;
}

/**
 * Deal with the special case where the square matrix is 1
 */
template<typename Type>
ತಾರ್ಕಿಕ inv(ಧ್ರುವ SquareMatrix<Type, 1> &A, SquareMatrix<Type, 1> &inv, size_t rank = 1)
{
	ಯದಿ (std::fabs(A(0, 0)) < Type(FLT_EPSILON)) {
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	inv(0, 0) = Type(1) / A(0, 0);
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/**
 * inverse based on LU factorization with partial pivotting
 */
template<typename Type, size_t M>
ತಾರ್ಕಿಕ inv(ಧ್ರುವ SquareMatrix<Type, M> &A, SquareMatrix<Type, M> &inv, size_t rank = M)
{
	SquareMatrix<Type, M> L;
	L.setIdentity();
	SquareMatrix<Type, M> U = A;
	SquareMatrix<Type, M> P;
	P.setIdentity();

	//printf("A:\n"); A.print();

	// for all diagonal elements
	ಪ್ರತಿ (size_t n = 0; n < rank; n++) {

		// if diagonal is zero, swap with row below
		ಯದಿ (std::fabs(U(n, n)) < Type(FLT_EPSILON)) {
			//printf("trying pivot for row %d\n",n);
			ಪ್ರತಿ (size_t i = n + 1; i < rank; i++) {

				//printf("\ttrying row %d\n",i);
				ಯದಿ (std::fabs(U(i, n)) > Type(FLT_EPSILON)) {
					//printf("swapped %d\n",i);
					U.swapRows(i, n);
					P.swapRows(i, n);
					L.swapRows(i, n);
					L.swapCols(i, n);
					ಭಙ್ಗ;
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
		ಯದಿ (std::fabs(static_cast<ಭಿನ್ನ>(U(n, n))) < FLT_EPSILON) {
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}

		// for all rows below diagonal
		ಪ್ರತಿ (size_t i = (n + 1); i < rank; i++) {
			L(i, n) = U(i, n) / U(n, n);

			// add i-th row and n-th row
			// multiplied by: -a(i,n)/a(n,n)
			ಪ್ರತಿ (size_t k = n; k < rank; k++) {
				U(i, k) -= L(i, n) * U(n, k);
			}
		}
	}

	//printf("L:\n"); L.print();
	//printf("U:\n"); U.print();

	// solve LY=P*I for Y by forward subst
	//SquareMatrix<Type, M> Y = P;

	// for all columns of Y
	ಪ್ರತಿ (size_t c = 0; c < rank; c++) {
		// for all rows of L
		ಪ್ರತಿ (size_t i = 0; i < rank; i++) {
			// for all columns of L
			ಪ್ರತಿ (size_t j = 0; j < i; j++) {
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
	ಪ್ರತಿ (size_t c = 0; c < rank; c++) {
		// for all rows of U
		ಪ್ರತಿ (size_t k = 0; k < rank; k++) {
			// have to go in reverse order
			size_t i = rank - 1 - k;

			// for all columns of U
			ಪ್ರತಿ (size_t j = i + 1; j < rank; j++) {
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
	ಪ್ರತಿ (size_t i = 0; i < rank; i++) {
		ಪ್ರತಿ (size_t j = 0; j < rank; j++) {
			ಯದಿ (!std::isfinite(P(i, j))) {
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
		}
	}

	//printf("X:\n"); X.print();
	inv = P;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

template<typename Type>
ತಾರ್ಕಿಕ inv(ಧ್ರುವ SquareMatrix<Type, 2> &A, SquareMatrix<Type, 2> &inv)
{
	Type det = A(0, 0) * A(1, 1) - A(1, 0) * A(0, 1);

	ಯದಿ (std::fabs(static_cast<ಭಿನ್ನ>(det)) < FLT_EPSILON || !std::isfinite(det)) {
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	inv(0, 0) = A(1, 1);
	inv(1, 0) = -A(1, 0);
	inv(0, 1) = -A(0, 1);
	inv(1, 1) = A(0, 0);
	inv /= det;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

template<typename Type>
ತಾರ್ಕಿಕ inv(ಧ್ರುವ SquareMatrix<Type, 3> &A, SquareMatrix<Type, 3> &inv)
{
	Type det = A(0, 0) * (A(1, 1) * A(2, 2) - A(2, 1) * A(1, 2)) -
		   A(0, 1) * (A(1, 0) * A(2, 2) - A(1, 2) * A(2, 0)) +
		   A(0, 2) * (A(1, 0) * A(2, 1) - A(1, 1) * A(2, 0));

	ಯದಿ (std::fabs(static_cast<ಭಿನ್ನ>(det)) < FLT_EPSILON || !std::isfinite(det)) {
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
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
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/**
 * inverse based on LU factorization with partial pivotting
 */
template<typename Type, size_t M>
SquareMatrix<Type, M> inv(ಧ್ರುವ SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> i;

	ಯದಿ (inv(A, i)) {
		ಪ್ರತ್ಯಾಗಮ i;

	} ಅನ್ಯಥಾ {
		i.setZero();
		ಪ್ರತ್ಯಾಗಮ i;
	}
}

/**
 * cholesky decomposition
 *
 * Note: A must be positive definite
 */
template<typename Type, size_t M>
SquareMatrix <Type, M> cholesky(ಧ್ರುವ SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> L;

	ಪ್ರತಿ (size_t j = 0; j < M; j++) {
		ಪ್ರತಿ (size_t i = j; i < M; i++) {
			ಯದಿ (i == j) {
				ಭಿನ್ನ sum = 0;

				ಪ್ರತಿ (size_t k = 0; k < j; k++) {
					sum += L(j, k) * L(j, k);
				}

				Type res = A(j, j) - sum;

				ಯದಿ (res <= 0) {
					L(j, j) = 0;

				} ಅನ್ಯಥಾ {
					L(j, j) = std::sqrt(res);
				}

			} ಅನ್ಯಥಾ {
				ಭಿನ್ನ sum = 0;

				ಪ್ರತಿ (size_t k = 0; k < j; k++) {
					sum += L(i, k) * L(j, k);
				}

				ಯದಿ (L(j, j) <= 0) {
					L(i, j) = 0;

				} ಅನ್ಯಥಾ {
					L(i, j) = (A(i, j) - sum) / L(j, j);
				}
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ L;
}

/**
 * cholesky inverse
 *
 * TODO: Check if gaussian elimination jumps straight to back-substitution
 * for L or we need to do it manually. Will impact speed otherwise.
 */
template<typename Type, size_t M>
SquareMatrix <Type, M> choleskyInv(ಧ್ರುವ SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> L_inv = inv(cholesky(A));
	ಪ್ರತ್ಯಾಗಮ L_inv.T() * L_inv;
}

using Matrix2f = SquareMatrix<ಭಿನ್ನ, 2>;
using Matrix3f = SquareMatrix<ಭಿನ್ನ, 3>;
using Matrix3d = SquareMatrix<ದ್ವಿಗುಣ, 3>;

} // namespace matrix
