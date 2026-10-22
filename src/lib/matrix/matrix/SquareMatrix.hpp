/**
 * @file SquareMatrix.hpp
 *
 * A square matrix
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#निर्देशः once

#अन्तर्भाव <float.h> // FLT_EPSILON

#अन्तर्भाव "Slice.hpp"

नामाकाश matrix
{

प्रतिरूपम् <प्रकारनाम Type, size_t  M>
वर्ग वर्गफलम्_Matrix : सार्वजनिक Matrix<Type, M, M>
{
सार्वजनिक:
	वर्गफलम्_Matrix() = स्वभाव;

	स्पष्ट वर्गफलम्_Matrix(ध्रुव Type दत्त_2[M][M]) :
		Matrix<Type, M, M>(दत्त_2)
	{
	}

	स्पष्ट वर्गफलम्_Matrix(ध्रुव Type दत्त_2[M * M]) :
		Matrix<Type, M, M>(दत्त_2)
	{
	}

	वर्गफलम्_Matrix(ध्रुव Matrix<Type, M, M> &other) :
		Matrix<Type, M, M>(other)
	{
	}

	उपयुज्य base = Matrix<Type, M, M>;
	उपयुज्य base::base;

	वर्गफलम्_Matrix<Type, M> &संक्रियकः=(ध्रुव Matrix<Type, M, M> &other)
	{
		Matrix<Type, M, M>::संक्रियकः=(other);
		प्रत्यागम *एतत्;
	}

	प्रतिरूपम् <size_t P, size_t Q>
	वर्गफलम्_Matrix<Type, M> &संक्रियकः=(ध्रुव खण्डिका_2<Type, M, M, P, Q> &अन्तर्_खण्डिका)
	{
		Matrix<Type, M, M>::संक्रियकः=(अन्तर्_खण्डिका);
		प्रत्यागम *एतत्;
	}

	प्रतिरूपम्<size_t P, size_t Q>
	Const_खण्डिका<Type, P, Q, M, M> खण्डिका(size_t x0, size_t y0) ध्रुव
	{
		प्रत्यागम {x0, y0, एतत्};
	}

	प्रतिरूपम्<size_t P, size_t Q>
	खण्डिका_2<Type, P, Q, M, M> खण्डिका(size_t x0, size_t y0)
	{
		प्रत्यागम {x0, y0, एतत्};
	}

	// inverse alias
	अन्तःस्थ वर्गफलम्_Matrix<Type, M> I() ध्रुव
	{
		वर्गफलम्_Matrix<Type, M> i;

		यदि (inv(*एतत्, i)) {
			प्रत्यागम i;

		} अन्यथा {
			i.समुच्चयः_Zero();
			प्रत्यागम i;
		}
	}

	// inverse alias
	अन्तःस्थ तार्किक I(वर्गफलम्_Matrix<Type, M> &i) ध्रुव
	{
		प्रत्यागम inv(*एतत्, i);
	}


	सदिशम्<Type, M> diag() ध्रुव
	{
		सदिशम्<Type, M> res;
		ध्रुव वर्गफलम्_Matrix<Type, M> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			res(i) = self(i, i);
		}

		प्रत्यागम res;
	}

	// get matrix upper right triangle in a row-major vector format
	सदिशम् < Type, M *(M + 1) / 2 > ऊर्ध्वस्थम्_दक्षिणः_triangle() ध्रुव
	{
		सदिशम् < Type, M * (M + 1) / 2 > res;
		ध्रुव वर्गफलम्_Matrix<Type, M> &self = *एतत्;

		अचिह्नित idx = 0;

		प्रति (size_t x = 0; x < M; x++) {
			प्रति (size_t y = x; y < M; y++) {
				res(idx) = self(x, y);
				++idx;
			}
		}

		प्रत्यागम res;
	}

	प्रतिरूपम् <size_t विस्तारः_2>
	Type trace(size_t first) ध्रुव
	{
		static_assert(विस्तारः_2 <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + विस्तारः_2 <= M);

		Type res = 0;
		ध्रुव वर्गफलम्_Matrix<Type, M> &self = *एतत्;

		प्रति (size_t i = first; i < (first + विस्तारः_2); i++) {
			res += self(i, i);
		}

		प्रत्यागम res;
	}

	Type trace() ध्रुव
	{
		ध्रुव वर्गफलम्_Matrix<Type, M> &self = *एतत्;
		प्रत्यागम self.trace<M>(0);
	}

	// keep the sub covariance matrix and zero all covariance elements related
	// to the rest of the matrix
	प्रतिरूपम् <size_t विस्तारः_2>
	शून्य uncorrelate_Covariance_खण्डम्(size_t first)
	{
		static_assert(विस्तारः_2 <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + विस्तारः_2 <= M);

		वर्गफलम्_Matrix<Type, M> &self = *एतत्;
		वर्गफलम्_Matrix<Type, विस्तारः_2> cov = self.खण्डिका<विस्तारः_2, विस्तारः_2>(first, first);
		self.खण्डिका<M, विस्तारः_2>(0, first) = 0.f;
		self.खण्डिका<विस्तारः_2, M>(first, 0) = 0.f;
		self.खण्डिका<विस्तारः_2, विस्तारः_2>(first, first) = cov;
	}

	// zero all offdiagonal elements and keep corresponding diagonal elements
	प्रतिरूपम् <size_t विस्तारः_2>
	शून्य uncorrelateCovariance(size_t first)
	{
		static_assert(विस्तारः_2 <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + विस्तारः_2 <= M);

		वर्गफलम्_Matrix<Type, M> &self = *एतत्;
		सदिशम्<Type, विस्तारः_2> diag_elements = self.खण्डिका<विस्तारः_2, विस्तारः_2>(first, first).diag();
		self.uncorrelate_Covariance_समुच्चयः_Variance(first, diag_elements);
	}

	प्रतिरूपम् <size_t विस्तारः_2>
	शून्य uncorrelate_Covariance_समुच्चयः_Variance(size_t first, ध्रुव सदिशम्<Type, विस्तारः_2> &vec);

	प्रतिरूपम् <size_t विस्तारः_2>
	शून्य uncorrelate_Covariance_समुच्चयः_Variance(size_t first, Type val);

	// make block diagonal symmetric by taking the average of the two corresponding off diagonal values
	प्रतिरूपम् <size_t विस्तारः_2>
	शून्य make_खण्डम्_सममितम्(size_t first)
	{
		static_assert(विस्तारः_2 <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + विस्तारः_2 <= M);

		वर्गफलम्_Matrix<Type, M> &self = *एतत्;

		यदि (विस्तारः_2 > 1) {
			प्रति (size_t row_idx = first + 1; row_idx < first + विस्तारः_2; row_idx++) {
				प्रति (size_t col_idx = first; col_idx < row_idx; col_idx++) {
					Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
					self(row_idx, col_idx) = tmp;
					self(col_idx, row_idx) = tmp;
				}
			}
		}
	}

	// make rows and columns symmetric by taking the average of the two corresponding off diagonal values
	प्रतिरूपम् <size_t विस्तारः_2>
	शून्य make_Row_Col_सममितम्(size_t first)
	{
		static_assert(विस्तारः_2 <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + विस्तारः_2 <= M);

		वर्गफलम्_Matrix<Type, M> &self = *एतत्;
		self.make_खण्डम्_सममितम्<विस्तारः_2>(first);

		प्रति (size_t row_idx = first; row_idx < first + विस्तारः_2; row_idx++) {
			प्रति (size_t col_idx = 0; col_idx < first; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}

			प्रति (size_t col_idx = first + विस्तारः_2; col_idx < M; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}
		}
	}

	// checks if block diagonal is symmetric
	प्रतिरूपम् <size_t विस्तारः_2>
	तार्किक is_खण्डम्_सममितम्(size_t first, ध्रुव Type eps = Type(1e-8f))
	{
		static_assert(विस्तारः_2 <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + विस्तारः_2 <= M);

		वर्गफलम्_Matrix<Type, M> &self = *एतत्;

		यदि (विस्तारः_2 > 1) {
			प्रति (size_t row_idx = first + 1; row_idx < first + विस्तारः_2; row_idx++) {
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
	प्रतिरूपम् <size_t विस्तारः_2>
	तार्किक is_Row_Col_सममितम्(size_t first, ध्रुव Type eps = Type(1e-8f))
	{
		static_assert(विस्तारः_2 <= M, "Width bigger than matrix");
		प्रतिज्ञा(first + विस्तारः_2 <= M);

		वर्गफलम्_Matrix<Type, M> &self = *एतत्;

		प्रति (size_t row_idx = first; row_idx < first + विस्तारः_2; row_idx++) {
			प्रति (size_t col_idx = 0; col_idx < first; col_idx++) {
				यदि (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					प्रत्यागम असत्य;
				}
			}

			प्रति (size_t col_idx = first + विस्तारः_2; col_idx < M; col_idx++) {
				यदि (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					प्रत्यागम असत्य;
				}
			}
		}

		प्रत्यागम self.is_खण्डम्_सममितम्<विस्तारः_2>(first, eps);
	}

	शून्य copy_अधस्तनम्_गन्तव्यम्_ऊर्ध्वस्थम्_Triangle()
	{
		वर्गफलम्_Matrix<Type, M> &self = *एतत्;

		प्रति (size_t row_idx = 1; row_idx < M; row_idx++) {
			प्रति (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(col_idx, row_idx) = self(row_idx, col_idx);
			}
		}
	}

	शून्य copy_ऊर्ध्वस्थम्_गन्तव्यम्_अधस्तनम्_Triangle()
	{
		वर्गफलम्_Matrix<Type, M> &self = *एतत्;

		प्रति (size_t row_idx = 1; row_idx < M; row_idx++) {
			प्रति (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(row_idx, col_idx) = self(col_idx, row_idx);
			}
		}
	}
};

// defined out of the class so they are not implicitly inline, they are large and called often
प्रतिरूपम्<प्रकारनाम Type, size_t M>
प्रतिरूपम्<size_t विस्तारः_2>
शून्य वर्गफलम्_Matrix<Type, M>::uncorrelate_Covariance_समुच्चयः_Variance(size_t first, ध्रुव सदिशम्<Type, विस्तारः_2> &vec)
{
	static_assert(विस्तारः_2 <= M, "Width bigger than matrix");
	प्रतिज्ञा(first + विस्तारः_2 <= M);

	वर्गफलम्_Matrix<Type, M> &self = *एतत्;
	// zero rows and columns
	self.खण्डिका<विस्तारः_2, M>(first, 0) = Type(0);
	self.खण्डिका<M, विस्तारः_2>(0, first) = Type(0);

	// set diagonals
	अचिह्नित vec_idx = 0;

	प्रति (size_t idx = first; idx < first + विस्तारः_2; idx++) {
		self(idx, idx) = vec(vec_idx);
		vec_idx ++;
	}
}

प्रतिरूपम्<प्रकारनाम Type, size_t M>
प्रतिरूपम्<size_t विस्तारः_2>
शून्य वर्गफलम्_Matrix<Type, M>::uncorrelate_Covariance_समुच्चयः_Variance(size_t first, Type val)
{
	static_assert(विस्तारः_2 <= M, "Width bigger than matrix");
	प्रतिज्ञा(first + विस्तारः_2 <= M);

	वर्गफलम्_Matrix<Type, M> &self = *एतत्;
	// zero rows and columns
	self.खण्डिका<विस्तारः_2, M>(first, 0) = Type(0);
	self.खण्डिका<M, विस्तारः_2>(0, first) = Type(0);

	// set diagonals
	प्रति (size_t idx = first; idx < first + विस्तारः_2; idx++) {
		self(idx, idx) = val;
	}
}

उपयुज्य वर्गफलम्_Matrix2f = वर्गफलम्_Matrix<भिन्न, 2>;
उपयुज्य वर्गफलम्_Matrix3f = वर्गफलम्_Matrix<भिन्न, 3>;
उपयुज्य वर्गफलम्_Matrix3d = वर्गफलम्_Matrix<द्विगुण, 3>;

प्रतिरूपम्<प्रकारनाम Type, size_t M>
वर्गफलम्_Matrix<Type, M> eye()
{
	वर्गफलम्_Matrix<Type, M> m;
	m.समुच्चयः_तटस्थः();
	प्रत्यागम m;
}

प्रतिरूपम्<प्रकारनाम Type, size_t M>
वर्गफलम्_Matrix<Type, M> diag(सदिशम्<Type, M> d)
{
	वर्गफलम्_Matrix<Type, M> m;

	प्रति (size_t i = 0; i < M; i++) {
		m(i, i) = d(i);
	}

	प्रत्यागम m;
}

प्रतिरूपम्<प्रकारनाम Type, size_t M>
वर्गफलम्_Matrix<Type, M> expm(ध्रुव Matrix<Type, M, M> &A, size_t क्रमः = 5)
{
	वर्गफलम्_Matrix<Type, M> res;
	वर्गफलम्_Matrix<Type, M> A_pow = A;
	res.समुच्चयः_तटस्थः();
	size_t i_क्रमगुणितम् = 1;

	प्रति (size_t i = 1; i <= क्रमः; i++) {
		i_क्रमगुणितम् *= i;
		res += A_pow / Type(i_क्रमगुणितम्);
		A_pow *= A_pow;
	}

	प्रत्यागम res;
}

/**
 * Deal with the special case where the square matrix is 1
 */
प्रतिरूपम्<प्रकारनाम Type>
तार्किक inv(ध्रुव वर्गफलम्_Matrix<Type, 1> &A, वर्गफलम्_Matrix<Type, 1> &inv, size_t rank = 1)
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
प्रतिरूपम्<प्रकारनाम Type, size_t M>
तार्किक inv(ध्रुव वर्गफलम्_Matrix<Type, M> &A, वर्गफलम्_Matrix<Type, M> &inv, size_t rank = M)
{
	वर्गफलम्_Matrix<Type, M> L;
	L.समुच्चयः_तटस्थः();
	वर्गफलम्_Matrix<Type, M> U = A;
	वर्गफलम्_Matrix<Type, M> P;
	P.समुच्चयः_तटस्थः();

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
					U.swap_पङ्क्तिकाः(i, n);
					P.swap_पङ्क्तिकाः(i, n);
					L.swap_पङ्क्तिकाः(i, n);
					L.swap_स्तम्भसङ्ख्या(i, n);
					भङ्ग;
				}
			}
		}

#यदि_परिभाषितम् MATRIX_ASSERT
		//printf("A:\n"); A.print();
		//printf("U:\n"); U.print();
		//printf("P:\n"); P.print();
		//fflush(stdout);
		//ASSERT(fabs(U(n, n)) > 1e-8f);
#यदि_अन्तः

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

प्रतिरूपम्<प्रकारनाम Type>
तार्किक inv(ध्रुव वर्गफलम्_Matrix<Type, 2> &A, वर्गफलम्_Matrix<Type, 2> &inv)
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

प्रतिरूपम्<प्रकारनाम Type>
तार्किक inv(ध्रुव वर्गफलम्_Matrix<Type, 3> &A, वर्गफलम्_Matrix<Type, 3> &inv)
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
प्रतिरूपम्<प्रकारनाम Type, size_t M>
वर्गफलम्_Matrix<Type, M> inv(ध्रुव वर्गफलम्_Matrix<Type, M> &A)
{
	वर्गफलम्_Matrix<Type, M> i;

	यदि (inv(A, i)) {
		प्रत्यागम i;

	} अन्यथा {
		i.समुच्चयः_Zero();
		प्रत्यागम i;
	}
}

/**
 * cholesky decomposition
 *
 * Note: A must be positive definite
 */
प्रतिरूपम्<प्रकारनाम Type, size_t M>
वर्गफलम्_Matrix <Type, M> cholesky(ध्रुव वर्गफलम्_Matrix<Type, M> &A)
{
	वर्गफलम्_Matrix<Type, M> L;

	प्रति (size_t j = 0; j < M; j++) {
		प्रति (size_t i = j; i < M; i++) {
			यदि (i == j) {
				भिन्न योग = 0;

				प्रति (size_t k = 0; k < j; k++) {
					योग += L(j, k) * L(j, k);
				}

				Type res = A(j, j) - योग;

				यदि (res <= 0) {
					L(j, j) = 0;

				} अन्यथा {
					L(j, j) = std::वर्गमूलम्(res);
				}

			} अन्यथा {
				भिन्न योग = 0;

				प्रति (size_t k = 0; k < j; k++) {
					योग += L(i, k) * L(j, k);
				}

				यदि (L(j, j) <= 0) {
					L(i, j) = 0;

				} अन्यथा {
					L(i, j) = (A(i, j) - योग) / L(j, j);
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
प्रतिरूपम्<प्रकारनाम Type, size_t M>
वर्गफलम्_Matrix <Type, M> choleskyInv(ध्रुव वर्गफलम्_Matrix<Type, M> &A)
{
	वर्गफलम्_Matrix<Type, M> L_inv = inv(cholesky(A));
	प्रत्यागम L_inv.T() * L_inv;
}

उपयुज्य Matrix2f = वर्गफलम्_Matrix<भिन्न, 2>;
उपयुज्य Matrix3f = वर्गफलम्_Matrix<भिन्न, 3>;
उपयुज्य Matrix3d = वर्गफलम्_Matrix<द्विगुण, 3>;

} // namespace matrix
