/**
 * @file SquareMatrix.hpp
 *
 * A square matrix
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#અન્તર્ભાવ <float.h> // FLT_EPSILON

#અન્તર્ભાવ "Slice.hpp"

નામાકાશ matrix
{

template <typename Type, size_t  M>
વર્ગ SquareMatrix : જાહેર Matrix<Type, M, M>
{
જાહેર:
	SquareMatrix() = મૂળભૂત;

	explicit SquareMatrix(અચલ Type data_[M][M]) :
		Matrix<Type, M, M>(data_)
	{
	}

	explicit SquareMatrix(અચલ Type data_[M * M]) :
		Matrix<Type, M, M>(data_)
	{
	}

	SquareMatrix(અચલ Matrix<Type, M, M> &other) :
		Matrix<Type, M, M>(other)
	{
	}

	using base = Matrix<Type, M, M>;
	using base::base;

	SquareMatrix<Type, M> &operator=(અચલ Matrix<Type, M, M> &other)
	{
		Matrix<Type, M, M>::operator=(other);
		પાછા *આ;
	}

	template <size_t P, size_t Q>
	SquareMatrix<Type, M> &operator=(અચલ Slice<Type, M, M, P, Q> &in_slice)
	{
		Matrix<Type, M, M>::operator=(in_slice);
		પાછા *આ;
	}

	template<size_t P, size_t Q>
	ConstSlice<Type, P, Q, M, M> slice(size_t x0, size_t y0) અચલ
	{
		પાછા {x0, y0, આ};
	}

	template<size_t P, size_t Q>
	Slice<Type, P, Q, M, M> slice(size_t x0, size_t y0)
	{
		પાછા {x0, y0, આ};
	}

	// inverse alias
	અંતઃરેખ SquareMatrix<Type, M> I() અચલ
	{
		SquareMatrix<Type, M> i;

		જો (inv(*આ, i)) {
			પાછા i;

		} નહીંતર {
			i.setZero();
			પાછા i;
		}
	}

	// inverse alias
	અંતઃરેખ તાર્કિક I(SquareMatrix<Type, M> &i) અચલ
	{
		પાછા inv(*આ, i);
	}


	Vector<Type, M> diag() અચલ
	{
		Vector<Type, M> res;
		અચલ SquareMatrix<Type, M> &self = *આ;

		માટે (size_t i = 0; i < M; i++) {
			res(i) = self(i, i);
		}

		પાછા res;
	}

	// get matrix upper right triangle in a row-major vector format
	Vector < Type, M *(M + 1) / 2 > upper_જમણું_triangle() અચલ
	{
		Vector < Type, M * (M + 1) / 2 > res;
		અચલ SquareMatrix<Type, M> &self = *આ;

		unsigned idx = 0;

		માટે (size_t x = 0; x < M; x++) {
			માટે (size_t y = x; y < M; y++) {
				res(idx) = self(x, y);
				++idx;
			}
		}

		પાછા res;
	}

	template <size_t Width>
	Type trace(size_t first) અચલ
	{
		static_assert(Width <= M, "Width bigger than matrix");
		પ્રતિજ્ઞા(first + Width <= M);

		Type res = 0;
		અચલ SquareMatrix<Type, M> &self = *આ;

		માટે (size_t i = first; i < (first + Width); i++) {
			res += self(i, i);
		}

		પાછા res;
	}

	Type trace() અચલ
	{
		અચલ SquareMatrix<Type, M> &self = *આ;
		પાછા self.trace<M>(0);
	}

	// keep the sub covariance matrix and zero all covariance elements related
	// to the rest of the matrix
	template <size_t Width>
	શૂન્ય uncorrelateCovarianceBlock(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		પ્રતિજ્ઞા(first + Width <= M);

		SquareMatrix<Type, M> &self = *આ;
		SquareMatrix<Type, Width> cov = self.slice<Width, Width>(first, first);
		self.slice<M, Width>(0, first) = 0.f;
		self.slice<Width, M>(first, 0) = 0.f;
		self.slice<Width, Width>(first, first) = cov;
	}

	// zero all offdiagonal elements and keep corresponding diagonal elements
	template <size_t Width>
	શૂન્ય uncorrelateCovariance(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		પ્રતિજ્ઞા(first + Width <= M);

		SquareMatrix<Type, M> &self = *આ;
		Vector<Type, Width> diag_elements = self.slice<Width, Width>(first, first).diag();
		self.uncorrelateCovarianceSetVariance(first, diag_elements);
	}

	template <size_t Width>
	શૂન્ય uncorrelateCovarianceSetVariance(size_t first, અચલ Vector<Type, Width> &vec);

	template <size_t Width>
	શૂન્ય uncorrelateCovarianceSetVariance(size_t first, Type val);

	// make block diagonal symmetric by taking the average of the two corresponding off diagonal values
	template <size_t Width>
	શૂન્ય makeBlockSymmetric(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		પ્રતિજ્ઞા(first + Width <= M);

		SquareMatrix<Type, M> &self = *આ;

		જો (Width > 1) {
			માટે (size_t row_idx = first + 1; row_idx < first + Width; row_idx++) {
				માટે (size_t col_idx = first; col_idx < row_idx; col_idx++) {
					Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
					self(row_idx, col_idx) = tmp;
					self(col_idx, row_idx) = tmp;
				}
			}
		}
	}

	// make rows and columns symmetric by taking the average of the two corresponding off diagonal values
	template <size_t Width>
	શૂન્ય makeRowColSymmetric(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		પ્રતિજ્ઞા(first + Width <= M);

		SquareMatrix<Type, M> &self = *આ;
		self.makeBlockSymmetric<Width>(first);

		માટે (size_t row_idx = first; row_idx < first + Width; row_idx++) {
			માટે (size_t col_idx = 0; col_idx < first; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}

			માટે (size_t col_idx = first + Width; col_idx < M; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}
		}
	}

	// checks if block diagonal is symmetric
	template <size_t Width>
	તાર્કિક isBlockSymmetric(size_t first, અચલ Type eps = Type(1e-8f))
	{
		static_assert(Width <= M, "Width bigger than matrix");
		પ્રતિજ્ઞા(first + Width <= M);

		SquareMatrix<Type, M> &self = *આ;

		જો (Width > 1) {
			માટે (size_t row_idx = first + 1; row_idx < first + Width; row_idx++) {
				માટે (size_t col_idx = first; col_idx < row_idx; col_idx++) {
					જો (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
						પાછા ખોટું;
					}
				}
			}
		}

		પાછા સાચું;
	}

	// checks if rows and columns are symmetric
	template <size_t Width>
	તાર્કિક isRowColSymmetric(size_t first, અચલ Type eps = Type(1e-8f))
	{
		static_assert(Width <= M, "Width bigger than matrix");
		પ્રતિજ્ઞા(first + Width <= M);

		SquareMatrix<Type, M> &self = *આ;

		માટે (size_t row_idx = first; row_idx < first + Width; row_idx++) {
			માટે (size_t col_idx = 0; col_idx < first; col_idx++) {
				જો (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					પાછા ખોટું;
				}
			}

			માટે (size_t col_idx = first + Width; col_idx < M; col_idx++) {
				જો (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					પાછા ખોટું;
				}
			}
		}

		પાછા self.isBlockSymmetric<Width>(first, eps);
	}

	શૂન્ય copyLowerToUpperTriangle()
	{
		SquareMatrix<Type, M> &self = *આ;

		માટે (size_t row_idx = 1; row_idx < M; row_idx++) {
			માટે (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(col_idx, row_idx) = self(row_idx, col_idx);
			}
		}
	}

	શૂન્ય copyUpperToLowerTriangle()
	{
		SquareMatrix<Type, M> &self = *આ;

		માટે (size_t row_idx = 1; row_idx < M; row_idx++) {
			માટે (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(row_idx, col_idx) = self(col_idx, row_idx);
			}
		}
	}
};

// defined out of the class so they are not implicitly inline, they are large and called often
template<typename Type, size_t M>
template<size_t Width>
શૂન્ય SquareMatrix<Type, M>::uncorrelateCovarianceSetVariance(size_t first, અચલ Vector<Type, Width> &vec)
{
	static_assert(Width <= M, "Width bigger than matrix");
	પ્રતિજ્ઞા(first + Width <= M);

	SquareMatrix<Type, M> &self = *આ;
	// zero rows and columns
	self.slice<Width, M>(first, 0) = Type(0);
	self.slice<M, Width>(0, first) = Type(0);

	// set diagonals
	unsigned vec_idx = 0;

	માટે (size_t idx = first; idx < first + Width; idx++) {
		self(idx, idx) = vec(vec_idx);
		vec_idx ++;
	}
}

template<typename Type, size_t M>
template<size_t Width>
શૂન્ય SquareMatrix<Type, M>::uncorrelateCovarianceSetVariance(size_t first, Type val)
{
	static_assert(Width <= M, "Width bigger than matrix");
	પ્રતિજ્ઞા(first + Width <= M);

	SquareMatrix<Type, M> &self = *આ;
	// zero rows and columns
	self.slice<Width, M>(first, 0) = Type(0);
	self.slice<M, Width>(0, first) = Type(0);

	// set diagonals
	માટે (size_t idx = first; idx < first + Width; idx++) {
		self(idx, idx) = val;
	}
}

using SquareMatrix2f = SquareMatrix<દશાંશ, 2>;
using SquareMatrix3f = SquareMatrix<દશાંશ, 3>;
using SquareMatrix3d = SquareMatrix<દ્વિગુણ, 3>;

template<typename Type, size_t M>
SquareMatrix<Type, M> eye()
{
	SquareMatrix<Type, M> m;
	m.setIdentity();
	પાછા m;
}

template<typename Type, size_t M>
SquareMatrix<Type, M> diag(Vector<Type, M> d)
{
	SquareMatrix<Type, M> m;

	માટે (size_t i = 0; i < M; i++) {
		m(i, i) = d(i);
	}

	પાછા m;
}

template<typename Type, size_t M>
SquareMatrix<Type, M> expm(અચલ Matrix<Type, M, M> &A, size_t order = 5)
{
	SquareMatrix<Type, M> res;
	SquareMatrix<Type, M> A_pow = A;
	res.setIdentity();
	size_t i_factorial = 1;

	માટે (size_t i = 1; i <= order; i++) {
		i_factorial *= i;
		res += A_pow / Type(i_factorial);
		A_pow *= A_pow;
	}

	પાછા res;
}

/**
 * Deal with the special case where the square matrix is 1
 */
template<typename Type>
તાર્કિક inv(અચલ SquareMatrix<Type, 1> &A, SquareMatrix<Type, 1> &inv, size_t rank = 1)
{
	જો (std::fabs(A(0, 0)) < Type(FLT_EPSILON)) {
		પાછા ખોટું;
	}

	inv(0, 0) = Type(1) / A(0, 0);
	પાછા સાચું;
}

/**
 * inverse based on LU factorization with partial pivotting
 */
template<typename Type, size_t M>
તાર્કિક inv(અચલ SquareMatrix<Type, M> &A, SquareMatrix<Type, M> &inv, size_t rank = M)
{
	SquareMatrix<Type, M> L;
	L.setIdentity();
	SquareMatrix<Type, M> U = A;
	SquareMatrix<Type, M> P;
	P.setIdentity();

	//printf("A:\n"); A.print();

	// for all diagonal elements
	માટે (size_t n = 0; n < rank; n++) {

		// if diagonal is zero, swap with row below
		જો (std::fabs(U(n, n)) < Type(FLT_EPSILON)) {
			//printf("trying pivot for row %d\n",n);
			માટે (size_t i = n + 1; i < rank; i++) {

				//printf("\ttrying row %d\n",i);
				જો (std::fabs(U(i, n)) > Type(FLT_EPSILON)) {
					//printf("swapped %d\n",i);
					U.swapRows(i, n);
					P.swapRows(i, n);
					L.swapRows(i, n);
					L.swapCols(i, n);
					તોડો;
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
		જો (std::fabs(static_cast<દશાંશ>(U(n, n))) < FLT_EPSILON) {
			પાછા ખોટું;
		}

		// for all rows below diagonal
		માટે (size_t i = (n + 1); i < rank; i++) {
			L(i, n) = U(i, n) / U(n, n);

			// add i-th row and n-th row
			// multiplied by: -a(i,n)/a(n,n)
			માટે (size_t k = n; k < rank; k++) {
				U(i, k) -= L(i, n) * U(n, k);
			}
		}
	}

	//printf("L:\n"); L.print();
	//printf("U:\n"); U.print();

	// solve LY=P*I for Y by forward subst
	//SquareMatrix<Type, M> Y = P;

	// for all columns of Y
	માટે (size_t c = 0; c < rank; c++) {
		// for all rows of L
		માટે (size_t i = 0; i < rank; i++) {
			// for all columns of L
			માટે (size_t j = 0; j < i; j++) {
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
	માટે (size_t c = 0; c < rank; c++) {
		// for all rows of U
		માટે (size_t k = 0; k < rank; k++) {
			// have to go in reverse order
			size_t i = rank - 1 - k;

			// for all columns of U
			માટે (size_t j = i + 1; j < rank; j++) {
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
	માટે (size_t i = 0; i < rank; i++) {
		માટે (size_t j = 0; j < rank; j++) {
			જો (!std::isfinite(P(i, j))) {
				પાછા ખોટું;
			}
		}
	}

	//printf("X:\n"); X.print();
	inv = P;
	પાછા સાચું;
}

template<typename Type>
તાર્કિક inv(અચલ SquareMatrix<Type, 2> &A, SquareMatrix<Type, 2> &inv)
{
	Type det = A(0, 0) * A(1, 1) - A(1, 0) * A(0, 1);

	જો (std::fabs(static_cast<દશાંશ>(det)) < FLT_EPSILON || !std::isfinite(det)) {
		પાછા ખોટું;
	}

	inv(0, 0) = A(1, 1);
	inv(1, 0) = -A(1, 0);
	inv(0, 1) = -A(0, 1);
	inv(1, 1) = A(0, 0);
	inv /= det;
	પાછા સાચું;
}

template<typename Type>
તાર્કિક inv(અચલ SquareMatrix<Type, 3> &A, SquareMatrix<Type, 3> &inv)
{
	Type det = A(0, 0) * (A(1, 1) * A(2, 2) - A(2, 1) * A(1, 2)) -
		   A(0, 1) * (A(1, 0) * A(2, 2) - A(1, 2) * A(2, 0)) +
		   A(0, 2) * (A(1, 0) * A(2, 1) - A(1, 1) * A(2, 0));

	જો (std::fabs(static_cast<દશાંશ>(det)) < FLT_EPSILON || !std::isfinite(det)) {
		પાછા ખોટું;
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
	પાછા સાચું;
}

/**
 * inverse based on LU factorization with partial pivotting
 */
template<typename Type, size_t M>
SquareMatrix<Type, M> inv(અચલ SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> i;

	જો (inv(A, i)) {
		પાછા i;

	} નહીંતર {
		i.setZero();
		પાછા i;
	}
}

/**
 * cholesky decomposition
 *
 * Note: A must be positive definite
 */
template<typename Type, size_t M>
SquareMatrix <Type, M> cholesky(અચલ SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> L;

	માટે (size_t j = 0; j < M; j++) {
		માટે (size_t i = j; i < M; i++) {
			જો (i == j) {
				દશાંશ sum = 0;

				માટે (size_t k = 0; k < j; k++) {
					sum += L(j, k) * L(j, k);
				}

				Type res = A(j, j) - sum;

				જો (res <= 0) {
					L(j, j) = 0;

				} નહીંતર {
					L(j, j) = std::sqrt(res);
				}

			} નહીંતર {
				દશાંશ sum = 0;

				માટે (size_t k = 0; k < j; k++) {
					sum += L(i, k) * L(j, k);
				}

				જો (L(j, j) <= 0) {
					L(i, j) = 0;

				} નહીંતર {
					L(i, j) = (A(i, j) - sum) / L(j, j);
				}
			}
		}
	}

	પાછા L;
}

/**
 * cholesky inverse
 *
 * TODO: Check if gaussian elimination jumps straight to back-substitution
 * for L or we need to do it manually. Will impact speed otherwise.
 */
template<typename Type, size_t M>
SquareMatrix <Type, M> choleskyInv(અચલ SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> L_inv = inv(cholesky(A));
	પાછા L_inv.T() * L_inv;
}

using Matrix2f = SquareMatrix<દશાંશ, 2>;
using Matrix3f = SquareMatrix<દશાંશ, 3>;
using Matrix3d = SquareMatrix<દ્વિગુણ, 3>;

} // namespace matrix
