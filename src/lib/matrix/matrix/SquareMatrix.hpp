/**
 * @file SquareMatrix.hpp
 *
 * A square matrix
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#অন্তর্ভাব <float.h> // FLT_EPSILON

#অন্তর্ভাব "Slice.hpp"

নামাকাশ matrix
{

template <typename Type, size_t  M>
শ্রেণী SquareMatrix : সর্বজনীন Matrix<Type, M, M>
{
সর্বজনীন:
	SquareMatrix() = পূর্বনির্ধারিত;

	explicit SquareMatrix(ধ্রুবক Type তথ্য_2[M][M]) :
		Matrix<Type, M, M>(তথ্য_2)
	{
	}

	explicit SquareMatrix(ধ্রুবক Type তথ্য_2[M * M]) :
		Matrix<Type, M, M>(তথ্য_2)
	{
	}

	SquareMatrix(ধ্রুবক Matrix<Type, M, M> &other) :
		Matrix<Type, M, M>(other)
	{
	}

	using base = Matrix<Type, M, M>;
	using base::base;

	SquareMatrix<Type, M> &operator=(ধ্রুবক Matrix<Type, M, M> &other)
	{
		Matrix<Type, M, M>::operator=(other);
		ফেরত *এই;
	}

	template <size_t P, size_t Q>
	SquareMatrix<Type, M> &operator=(ধ্রুবক Slice<Type, M, M, P, Q> &in_slice)
	{
		Matrix<Type, M, M>::operator=(in_slice);
		ফেরত *এই;
	}

	template<size_t P, size_t Q>
	ConstSlice<Type, P, Q, M, M> slice(size_t x0, size_t y0) ধ্রুবক
	{
		ফেরত {x0, y0, এই};
	}

	template<size_t P, size_t Q>
	Slice<Type, P, Q, M, M> slice(size_t x0, size_t y0)
	{
		ফেরত {x0, y0, এই};
	}

	// inverse alias
	অন্তঃরেখ SquareMatrix<Type, M> I() ধ্রুবক
	{
		SquareMatrix<Type, M> i;

		যদি (inv(*এই, i)) {
			ফেরত i;

		} অন্যথা {
			i.setZero();
			ফেরত i;
		}
	}

	// inverse alias
	অন্তঃরেখ যৌক্তিক I(SquareMatrix<Type, M> &i) ধ্রুবক
	{
		ফেরত inv(*এই, i);
	}


	Vector<Type, M> diag() ধ্রুবক
	{
		Vector<Type, M> res;
		ধ্রুবক SquareMatrix<Type, M> &self = *এই;

		জন্য (size_t i = 0; i < M; i++) {
			res(i) = self(i, i);
		}

		ফেরত res;
	}

	// get matrix upper right triangle in a row-major vector format
	Vector < Type, M *(M + 1) / 2 > upper_ডান_triangle() ধ্রুবক
	{
		Vector < Type, M * (M + 1) / 2 > res;
		ধ্রুবক SquareMatrix<Type, M> &self = *এই;

		unsigned idx = 0;

		জন্য (size_t x = 0; x < M; x++) {
			জন্য (size_t y = x; y < M; y++) {
				res(idx) = self(x, y);
				++idx;
			}
		}

		ফেরত res;
	}

	template <size_t Width>
	Type trace(size_t first) ধ্রুবক
	{
		static_assert(Width <= M, "Width bigger than matrix");
		প্রতিজ্ঞা(first + Width <= M);

		Type res = 0;
		ধ্রুবক SquareMatrix<Type, M> &self = *এই;

		জন্য (size_t i = first; i < (first + Width); i++) {
			res += self(i, i);
		}

		ফেরত res;
	}

	Type trace() ধ্রুবক
	{
		ধ্রুবক SquareMatrix<Type, M> &self = *এই;
		ফেরত self.trace<M>(0);
	}

	// keep the sub covariance matrix and zero all covariance elements related
	// to the rest of the matrix
	template <size_t Width>
	শূন্য uncorrelateCovarianceBlock(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		প্রতিজ্ঞা(first + Width <= M);

		SquareMatrix<Type, M> &self = *এই;
		SquareMatrix<Type, Width> cov = self.slice<Width, Width>(first, first);
		self.slice<M, Width>(0, first) = 0.f;
		self.slice<Width, M>(first, 0) = 0.f;
		self.slice<Width, Width>(first, first) = cov;
	}

	// zero all offdiagonal elements and keep corresponding diagonal elements
	template <size_t Width>
	শূন্য uncorrelateCovariance(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		প্রতিজ্ঞা(first + Width <= M);

		SquareMatrix<Type, M> &self = *এই;
		Vector<Type, Width> diag_elements = self.slice<Width, Width>(first, first).diag();
		self.uncorrelateCovarianceSetVariance(first, diag_elements);
	}

	template <size_t Width>
	শূন্য uncorrelateCovarianceSetVariance(size_t first, ধ্রুবক Vector<Type, Width> &vec);

	template <size_t Width>
	শূন্য uncorrelateCovarianceSetVariance(size_t first, Type val);

	// make block diagonal symmetric by taking the average of the two corresponding off diagonal values
	template <size_t Width>
	শূন্য makeBlockSymmetric(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		প্রতিজ্ঞা(first + Width <= M);

		SquareMatrix<Type, M> &self = *এই;

		যদি (Width > 1) {
			জন্য (size_t row_idx = first + 1; row_idx < first + Width; row_idx++) {
				জন্য (size_t col_idx = first; col_idx < row_idx; col_idx++) {
					Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
					self(row_idx, col_idx) = tmp;
					self(col_idx, row_idx) = tmp;
				}
			}
		}
	}

	// make rows and columns symmetric by taking the average of the two corresponding off diagonal values
	template <size_t Width>
	শূন্য makeRowColSymmetric(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		প্রতিজ্ঞা(first + Width <= M);

		SquareMatrix<Type, M> &self = *এই;
		self.makeBlockSymmetric<Width>(first);

		জন্য (size_t row_idx = first; row_idx < first + Width; row_idx++) {
			জন্য (size_t col_idx = 0; col_idx < first; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}

			জন্য (size_t col_idx = first + Width; col_idx < M; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}
		}
	}

	// checks if block diagonal is symmetric
	template <size_t Width>
	যৌক্তিক isBlockSymmetric(size_t first, ধ্রুবক Type eps = Type(1e-8f))
	{
		static_assert(Width <= M, "Width bigger than matrix");
		প্রতিজ্ঞা(first + Width <= M);

		SquareMatrix<Type, M> &self = *এই;

		যদি (Width > 1) {
			জন্য (size_t row_idx = first + 1; row_idx < first + Width; row_idx++) {
				জন্য (size_t col_idx = first; col_idx < row_idx; col_idx++) {
					যদি (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
						ফেরত মিথ্যা;
					}
				}
			}
		}

		ফেরত সত্য;
	}

	// checks if rows and columns are symmetric
	template <size_t Width>
	যৌক্তিক isRowColSymmetric(size_t first, ধ্রুবক Type eps = Type(1e-8f))
	{
		static_assert(Width <= M, "Width bigger than matrix");
		প্রতিজ্ঞা(first + Width <= M);

		SquareMatrix<Type, M> &self = *এই;

		জন্য (size_t row_idx = first; row_idx < first + Width; row_idx++) {
			জন্য (size_t col_idx = 0; col_idx < first; col_idx++) {
				যদি (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					ফেরত মিথ্যা;
				}
			}

			জন্য (size_t col_idx = first + Width; col_idx < M; col_idx++) {
				যদি (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					ফেরত মিথ্যা;
				}
			}
		}

		ফেরত self.isBlockSymmetric<Width>(first, eps);
	}

	শূন্য copyLowerToUpperTriangle()
	{
		SquareMatrix<Type, M> &self = *এই;

		জন্য (size_t row_idx = 1; row_idx < M; row_idx++) {
			জন্য (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(col_idx, row_idx) = self(row_idx, col_idx);
			}
		}
	}

	শূন্য copyUpperToLowerTriangle()
	{
		SquareMatrix<Type, M> &self = *এই;

		জন্য (size_t row_idx = 1; row_idx < M; row_idx++) {
			জন্য (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(row_idx, col_idx) = self(col_idx, row_idx);
			}
		}
	}
};

// defined out of the class so they are not implicitly inline, they are large and called often
template<typename Type, size_t M>
template<size_t Width>
শূন্য SquareMatrix<Type, M>::uncorrelateCovarianceSetVariance(size_t first, ধ্রুবক Vector<Type, Width> &vec)
{
	static_assert(Width <= M, "Width bigger than matrix");
	প্রতিজ্ঞা(first + Width <= M);

	SquareMatrix<Type, M> &self = *এই;
	// zero rows and columns
	self.slice<Width, M>(first, 0) = Type(0);
	self.slice<M, Width>(0, first) = Type(0);

	// set diagonals
	unsigned vec_idx = 0;

	জন্য (size_t idx = first; idx < first + Width; idx++) {
		self(idx, idx) = vec(vec_idx);
		vec_idx ++;
	}
}

template<typename Type, size_t M>
template<size_t Width>
শূন্য SquareMatrix<Type, M>::uncorrelateCovarianceSetVariance(size_t first, Type val)
{
	static_assert(Width <= M, "Width bigger than matrix");
	প্রতিজ্ঞা(first + Width <= M);

	SquareMatrix<Type, M> &self = *এই;
	// zero rows and columns
	self.slice<Width, M>(first, 0) = Type(0);
	self.slice<M, Width>(0, first) = Type(0);

	// set diagonals
	জন্য (size_t idx = first; idx < first + Width; idx++) {
		self(idx, idx) = val;
	}
}

using SquareMatrix2f = SquareMatrix<দশমিক, 2>;
using SquareMatrix3f = SquareMatrix<দশমিক, 3>;
using SquareMatrix3d = SquareMatrix<দ্বিগুণ, 3>;

template<typename Type, size_t M>
SquareMatrix<Type, M> eye()
{
	SquareMatrix<Type, M> m;
	m.setIdentity();
	ফেরত m;
}

template<typename Type, size_t M>
SquareMatrix<Type, M> diag(Vector<Type, M> d)
{
	SquareMatrix<Type, M> m;

	জন্য (size_t i = 0; i < M; i++) {
		m(i, i) = d(i);
	}

	ফেরত m;
}

template<typename Type, size_t M>
SquareMatrix<Type, M> expm(ধ্রুবক Matrix<Type, M, M> &A, size_t order = 5)
{
	SquareMatrix<Type, M> res;
	SquareMatrix<Type, M> A_pow = A;
	res.setIdentity();
	size_t i_factorial = 1;

	জন্য (size_t i = 1; i <= order; i++) {
		i_factorial *= i;
		res += A_pow / Type(i_factorial);
		A_pow *= A_pow;
	}

	ফেরত res;
}

/**
 * Deal with the special case where the square matrix is 1
 */
template<typename Type>
যৌক্তিক inv(ধ্রুবক SquareMatrix<Type, 1> &A, SquareMatrix<Type, 1> &inv, size_t rank = 1)
{
	যদি (std::fabs(A(0, 0)) < Type(FLT_EPSILON)) {
		ফেরত মিথ্যা;
	}

	inv(0, 0) = Type(1) / A(0, 0);
	ফেরত সত্য;
}

/**
 * inverse based on LU factorization with partial pivotting
 */
template<typename Type, size_t M>
যৌক্তিক inv(ধ্রুবক SquareMatrix<Type, M> &A, SquareMatrix<Type, M> &inv, size_t rank = M)
{
	SquareMatrix<Type, M> L;
	L.setIdentity();
	SquareMatrix<Type, M> U = A;
	SquareMatrix<Type, M> P;
	P.setIdentity();

	//printf("A:\n"); A.print();

	// for all diagonal elements
	জন্য (size_t n = 0; n < rank; n++) {

		// if diagonal is zero, swap with row below
		যদি (std::fabs(U(n, n)) < Type(FLT_EPSILON)) {
			//printf("trying pivot for row %d\n",n);
			জন্য (size_t i = n + 1; i < rank; i++) {

				//printf("\ttrying row %d\n",i);
				যদি (std::fabs(U(i, n)) > Type(FLT_EPSILON)) {
					//printf("swapped %d\n",i);
					U.swapRows(i, n);
					P.swapRows(i, n);
					L.swapRows(i, n);
					L.swapCols(i, n);
					বিরতি;
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
		যদি (std::fabs(static_cast<দশমিক>(U(n, n))) < FLT_EPSILON) {
			ফেরত মিথ্যা;
		}

		// for all rows below diagonal
		জন্য (size_t i = (n + 1); i < rank; i++) {
			L(i, n) = U(i, n) / U(n, n);

			// add i-th row and n-th row
			// multiplied by: -a(i,n)/a(n,n)
			জন্য (size_t k = n; k < rank; k++) {
				U(i, k) -= L(i, n) * U(n, k);
			}
		}
	}

	//printf("L:\n"); L.print();
	//printf("U:\n"); U.print();

	// solve LY=P*I for Y by forward subst
	//SquareMatrix<Type, M> Y = P;

	// for all columns of Y
	জন্য (size_t c = 0; c < rank; c++) {
		// for all rows of L
		জন্য (size_t i = 0; i < rank; i++) {
			// for all columns of L
			জন্য (size_t j = 0; j < i; j++) {
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
	জন্য (size_t c = 0; c < rank; c++) {
		// for all rows of U
		জন্য (size_t k = 0; k < rank; k++) {
			// have to go in reverse order
			size_t i = rank - 1 - k;

			// for all columns of U
			জন্য (size_t j = i + 1; j < rank; j++) {
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
	জন্য (size_t i = 0; i < rank; i++) {
		জন্য (size_t j = 0; j < rank; j++) {
			যদি (!std::isfinite(P(i, j))) {
				ফেরত মিথ্যা;
			}
		}
	}

	//printf("X:\n"); X.print();
	inv = P;
	ফেরত সত্য;
}

template<typename Type>
যৌক্তিক inv(ধ্রুবক SquareMatrix<Type, 2> &A, SquareMatrix<Type, 2> &inv)
{
	Type det = A(0, 0) * A(1, 1) - A(1, 0) * A(0, 1);

	যদি (std::fabs(static_cast<দশমিক>(det)) < FLT_EPSILON || !std::isfinite(det)) {
		ফেরত মিথ্যা;
	}

	inv(0, 0) = A(1, 1);
	inv(1, 0) = -A(1, 0);
	inv(0, 1) = -A(0, 1);
	inv(1, 1) = A(0, 0);
	inv /= det;
	ফেরত সত্য;
}

template<typename Type>
যৌক্তিক inv(ধ্রুবক SquareMatrix<Type, 3> &A, SquareMatrix<Type, 3> &inv)
{
	Type det = A(0, 0) * (A(1, 1) * A(2, 2) - A(2, 1) * A(1, 2)) -
		   A(0, 1) * (A(1, 0) * A(2, 2) - A(1, 2) * A(2, 0)) +
		   A(0, 2) * (A(1, 0) * A(2, 1) - A(1, 1) * A(2, 0));

	যদি (std::fabs(static_cast<দশমিক>(det)) < FLT_EPSILON || !std::isfinite(det)) {
		ফেরত মিথ্যা;
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
	ফেরত সত্য;
}

/**
 * inverse based on LU factorization with partial pivotting
 */
template<typename Type, size_t M>
SquareMatrix<Type, M> inv(ধ্রুবক SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> i;

	যদি (inv(A, i)) {
		ফেরত i;

	} অন্যথা {
		i.setZero();
		ফেরত i;
	}
}

/**
 * cholesky decomposition
 *
 * Note: A must be positive definite
 */
template<typename Type, size_t M>
SquareMatrix <Type, M> cholesky(ধ্রুবক SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> L;

	জন্য (size_t j = 0; j < M; j++) {
		জন্য (size_t i = j; i < M; i++) {
			যদি (i == j) {
				দশমিক যোগ = 0;

				জন্য (size_t k = 0; k < j; k++) {
					যোগ += L(j, k) * L(j, k);
				}

				Type res = A(j, j) - যোগ;

				যদি (res <= 0) {
					L(j, j) = 0;

				} অন্যথা {
					L(j, j) = std::sqrt(res);
				}

			} অন্যথা {
				দশমিক যোগ = 0;

				জন্য (size_t k = 0; k < j; k++) {
					যোগ += L(i, k) * L(j, k);
				}

				যদি (L(j, j) <= 0) {
					L(i, j) = 0;

				} অন্যথা {
					L(i, j) = (A(i, j) - যোগ) / L(j, j);
				}
			}
		}
	}

	ফেরত L;
}

/**
 * cholesky inverse
 *
 * TODO: Check if gaussian elimination jumps straight to back-substitution
 * for L or we need to do it manually. Will impact speed otherwise.
 */
template<typename Type, size_t M>
SquareMatrix <Type, M> choleskyInv(ধ্রুবক SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> L_inv = inv(cholesky(A));
	ফেরত L_inv.T() * L_inv;
}

using Matrix2f = SquareMatrix<দশমিক, 2>;
using Matrix3f = SquareMatrix<দশমিক, 3>;
using Matrix3d = SquareMatrix<দ্বিগুণ, 3>;

} // namespace matrix
