/**
 * @file Matrix.hpp
 *
 * A simple matrix template library.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#अन्तर्भाव <cmath>
#अन्तर्भाव <cstdio>
#अन्तर्भाव <cstring>

#अन्तर्भाव "helper_functions.hpp"
#अन्तर्भाव "Slice.hpp"

नामाकाश matrix
{

template<typename Type, size_t M, size_t N>
वर्ग Matrix
{
	Type _data[M][N] {};

सार्वजनिक:

	// Constructors
	Matrix() = स्वभाव;

	explicit Matrix(ध्रुव Type data_[M * N])
	{
		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[N * i + j];
			}
		}
	}

	explicit Matrix(ध्रुव Type data_[M][N])
	{
		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[i][j];
			}
		}
	}

	Matrix(ध्रुव Matrix &other)
	{
		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				_data[i][j] = other(i, j);
			}
		}
	}

	template<typename S>
	Matrix(ध्रुव Matrix<S, M, N> &aa)
	{
		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				_data[i][j] = static_cast<Type>(aa(i, j));
			}
		}
	}

	template<size_t P, size_t Q>
	Matrix(ध्रुव Slice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	template<size_t P, size_t Q>
	Matrix(ध्रुव ConstSlice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	/**
	 * Accessors/ Assignment etc.
	 */


	अन्तःस्थ ध्रुव Type &operator()(size_t i, size_t j) ध्रुव
	{
		प्रतिज्ञा(i < M);
		प्रतिज्ञा(j < N);

		प्रत्यागम _data[i][j];
	}

	अन्तःस्थ Type &operator()(size_t i, size_t j)
	{
		प्रतिज्ञा(i < M);
		प्रतिज्ञा(j < N);

		प्रत्यागम _data[i][j];
	}

	Matrix<Type, M, N> &operator=(ध्रुव Matrix<Type, M, N> &other)
	{
		जइ (एतत् != &other) {
			Matrix<Type, M, N> &self = *एतत्;

			प्रति (size_t i = 0; i < M; i++) {
				प्रति (size_t j = 0; j < N; j++) {
					self(i, j) = other(i, j);
				}
			}
		}

		प्रत्यागम (*एतत्);
	}

	शून्य copyTo(Type dst[M * N]) ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				dst[N * i + j] = self(i, j);
			}
		}
	}

	शून्य copyToColumnMajor(Type dst[M * N]) ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	/**
	 * Matrix Operations
	 */

	// this might use a lot of programming memory
	// since it instantiates a class for every
	// required mult pair, but it provides
	// compile time size_t checking
	template<size_t P>
	Matrix<Type, M, P> operator*(ध्रुव Matrix<Type, N, P> &other) ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;
		Matrix<Type, M, P> res{};

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t k = 0; k < P; k++) {
				प्रति (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(j, k);
				}
			}
		}

		प्रत्यागम res;
	}

	// Using this function reduces the number of temporary variables needed to compute A * B.T
	template<size_t P>
	Matrix<Type, M, M> multiplyByTranspose(ध्रुव Matrix<Type, P, N> &other) ध्रुव
	{
		Matrix<Type, M, P> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t k = 0; k < P; k++) {
				प्रति (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(k, j);
				}
			}
		}

		प्रत्यागम res;
	}

	// Element-wise multiplication
	Matrix<Type, M, N> emult(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * other(i, j);
			}
		}

		प्रत्यागम res;
	}

	// Element-wise division
	Matrix<Type, M, N> edivide(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) / other(i, j);
			}
		}

		प्रत्यागम res;
	}

	Matrix<Type, M, N> operator+(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + other(i, j);
			}
		}

		प्रत्यागम res;
	}

	Matrix<Type, M, N> operator-(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) - other(i, j);
			}
		}

		प्रत्यागम res;
	}

	// unary minus
	Matrix<Type, M, N> operator-() ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = -self(i, j);
			}
		}

		प्रत्यागम res;
	}

	शून्य operator+=(ध्रुव Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) += other(i, j);
			}
		}
	}

	शून्य operator-=(ध्रुव Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) -= other(i, j);
			}
		}
	}

	template<size_t P>
	शून्य operator*=(ध्रुव Matrix<Type, N, P> &other)
	{
		Matrix<Type, M, N> &self = *एतत्;
		self = self * other;
	}

	/**
	 * Scalar Operations
	 */

	Matrix<Type, M, N> operator*(Type scalar) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * scalar;
			}
		}

		प्रत्यागम res;
	}

	अन्तःस्थ Matrix<Type, M, N> operator/(Type scalar) ध्रुव
	{
		प्रत्यागम (*एतत्) * (1 / scalar);
	}

	Matrix<Type, M, N> operator+(Type scalar) ध्रुव
	{
		Matrix<Type, M, N> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + scalar;
			}
		}

		प्रत्यागम res;
	}

	अन्तःस्थ Matrix<Type, M, N> operator-(Type scalar) ध्रुव
	{
		प्रत्यागम (*एतत्) + (-1 * scalar);
	}

	शून्य operator*=(Type scalar)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) *= scalar;
			}
		}
	}

	शून्य operator/=(Type scalar)
	{
		Matrix<Type, M, N> &self = *एतत्;
		self *= (Type(1) / scalar);
	}

	अन्तःस्थ शून्य operator+=(Type scalar)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) += scalar;
			}
		}
	}

	अन्तःस्थ शून्य operator-=(Type scalar)
	{
		Matrix<Type, M, N> &self = *एतत्;
		self += (-scalar);
	}

	तार्किक operator==(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		प्रत्यागम isEqual(*एतत्, other);
	}

	तार्किक operator!=(ध्रुव Matrix<Type, M, N> &other) ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;
		प्रत्यागम !(self == other);
	}

	/**
	 * Misc. Functions
	 */

	शून्य write_string(वर्ण *buf, size_t n) ध्रुव
	{
		buf[0] = '\0'; // make an empty string to begin with (we need the '\0' for strlen to work)
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				snprintf(buf + strlen(buf), n - strlen(buf), "\t%8.8g", द्विगुण(self(i, j))); // directly append to the string buffer
			}

			snprintf(buf + strlen(buf), n - strlen(buf), "\n");
		}
	}

	शून्य print(भिन्न eps = 1e-9) ध्रुव
	{
		// print column numbering
		जइ (N > 1) {
			printf("  ");

			प्रति (unsigned i = 0; i < N; i++) {
				printf("|%2u      ", i);

			}

			printf("\n");
		}

		ध्रुव Matrix<Type, M, N> &self = *एतत्;
		तार्किक is_prev_symmetric = सच्च; // assume symmetric until one element is not

		प्रति (unsigned i = 0; i < M; i++) {
			printf("%2u|", i); // print row numbering

			प्रति (unsigned j = 0; j < N; j++) {
				द्विगुण d = static_cast<द्विगुण>(self(i, j));

				// if symmetric don't print upper triangular elements
				जइ (is_prev_symmetric && (M == N) && (j > i) && (i < N) && (j < M)
				    && (fabs(d - static_cast<द्विगुण>(self(j, i))) < (द्विगुण)eps)
				   ) {
					// print empty space
					printf("         ");

				} अण्णहा {
					// avoid -0.0 for display
					जइ (fabs(d - 0.0) < (द्विगुण)eps) {
						// print fixed width zero
						printf(" 0       ");

					} अण्णहा जइ ((fabs(d) < 1e-4) || (fabs(d) >= 10.0)) {
						printf("% .1e ", d);

					} अण्णहा {
						printf("% 6.5f ", d);
					}

					is_prev_symmetric = मिच्छ; // not symmetric if once inside here
				}
			}

			printf("\n");
		}
	}

	Matrix<Type, N, M> transpose() ध्रुव
	{
		Matrix<Type, N, M> res;
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				res(j, i) = self(i, j);
			}
		}

		प्रत्यागम res;
	}

	// tranpose alias
	अन्तःस्थ Matrix<Type, N, M> T() ध्रुव
	{
		प्रत्यागम transpose();
	}

	template<size_t P, size_t Q>
	ConstSlice<Type, P, Q, M, N> slice(size_t x0, size_t y0) ध्रुव
	{
		प्रत्यागम {x0, y0, एतत्};
	}

	template<size_t P, size_t Q>
	Slice<Type, P, Q, M, N> slice(size_t x0, size_t y0)
	{
		प्रत्यागम {x0, y0, एतत्};
	}

	ConstSlice<Type, 1, N, M, N> row(size_t i) ध्रुव
	{
		प्रत्यागम slice<1, N>(i, 0);
	}

	Slice<Type, 1, N, M, N> row(size_t i)
	{
		प्रत्यागम slice<1, N>(i, 0);
	}

	ConstSlice<Type, M, 1, M, N> col(size_t j) ध्रुव
	{
		प्रत्यागम slice<M, 1>(0, j);
	}

	Slice<Type, M, 1, M, N> col(size_t j)
	{
		प्रत्यागम slice<M, 1>(0, j);
	}

	शून्य setRow(size_t i, ध्रुव Matrix<Type, N, 1> &row_in)
	{
		slice<1, N>(i, 0) = row_in.transpose();
	}

	शून्य setRow(size_t i, Type val)
	{
		slice<1, N>(i, 0) = val;
	}

	शून्य setCol(size_t j, ध्रुव Matrix<Type, M, 1> &column)
	{
		slice<M, 1>(0, j) = column;
	}

	शून्य setCol(size_t j, Type val)
	{
		slice<M, 1>(0, j) = val;
	}

	शून्य setZero()
	{
		memset(_data, 0, परिमाण(_data));
	}

	अन्तःस्थ शून्य zero()
	{
		setZero();
	}

	शून्य setAll(Type val)
	{
		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				self(i, j) = val;
			}
		}
	}

	अन्तःस्थ शून्य setOne()
	{
		setAll(1);
	}

	अन्तःस्थ शून्य setNaN()
	{
		setAll(NAN);
	}

	शून्य setIdentity()
	{
		setZero();
		Matrix<Type, M, N> &self = *एतत्;

		ध्रुव size_t min_i = M > N ? N : M;

		प्रति (size_t i = 0; i < min_i; i++) {
			self(i, i) = 1;
		}
	}

	अन्तःस्थ शून्य identity()
	{
		setIdentity();
	}

	अन्तःस्थ शून्य swapRows(size_t a, size_t b)
	{
		प्रतिज्ञा(a < M);
		प्रतिज्ञा(b < M);

		जइ (a == b) {
			प्रत्यागम;
		}

		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t j = 0; j < N; j++) {
			Type tmp = self(a, j);
			self(a, j) = self(b, j);
			self(b, j) = tmp;
		}
	}

	अन्तःस्थ शून्य swapCols(size_t a, size_t b)
	{
		प्रतिज्ञा(a < N);
		प्रतिज्ञा(b < N);

		जइ (a == b) {
			प्रत्यागम;
		}

		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			Type tmp = self(i, a);
			self(i, a) = self(i, b);
			self(i, b) = tmp;
		}
	}

	अन्तःस्थ शून्य nanToZero()
	{

		Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				जइ (std::isnan(self(i, j))) {
					self(i, j) = static_cast<Type>(0.0f);
				}
			}
		}
	}

	Matrix<Type, M, N> abs() ध्रुव
	{
		Matrix<Type, M, N> r;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				r(i, j) = Type(std::fabs((*एतत्)(i, j)));
			}
		}

		प्रत्यागम r;
	}

	Type max() ध्रुव
	{
		Type max_val = (*एतत्)(0, 0);

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				Type val = (*एतत्)(i, j);

				जइ (val > max_val) {
					max_val = val;
				}
			}
		}

		प्रत्यागम max_val;
	}

	Type min() ध्रुव
	{
		Type min_val = (*एतत्)(0, 0);

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				Type val = (*एतत्)(i, j);

				जइ (val < min_val) {
					min_val = val;
				}
			}
		}

		प्रत्यागम min_val;
	}

	तार्किक isAllNan() ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;
		तार्किक result = सच्च;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				result = result && std::isnan(self(i, j));
			}
		}

		प्रत्यागम result;
	}

	तार्किक isAllFinite() ध्रुव
	{
		ध्रुव Matrix<Type, M, N> &self = *एतत्;

		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				जइ (!std::isfinite(self(i, j))) {
					प्रत्यागम मिच्छ;
				}
			}
		}

		प्रत्यागम सच्च;
	}
};

template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> zeros()
{
	Matrix<Type, M, N> m;
	m.setZero();
	प्रत्यागम m;
}

template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> ones()
{
	Matrix<Type, M, N> m;
	m.setOne();
	प्रत्यागम m;
}

template<size_t M, size_t N>
Matrix<भिन्न, M, N> nans()
{
	Matrix<भिन्न, M, N> m;
	m.setNaN();
	प्रत्यागम m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> operator*(Type scalar, ध्रुव Matrix<Type, M, N> &other)
{
	प्रत्यागम other * scalar;
}

template<typename Type, size_t  M, size_t N>
तार्किक isEqual(ध्रुव Matrix<Type, M, N> &x,
	     ध्रुव Matrix<Type, M, N> &y, ध्रुव Type eps = Type(1e-4f))
{
	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			जइ (!isEqualF(x(i, j), y(i, j), eps)) {
				प्रत्यागम मिच्छ;
			}
		}
	}

	प्रत्यागम सच्च;
}

नामाकाश typeFunction
{
template<typename Type>
Type min(ध्रुव Type x, ध्रुव Type y)
{
	तार्किक x_is_nan = std::isnan(x);
	तार्किक y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	जइ (x_is_nan || y_is_nan) {
		जइ (x_is_nan && !y_is_nan) {
			प्रत्यागम y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		प्रत्यागम x;
	}

	प्रत्यागम (x < y) ? x : y;
}

template<typename Type>
Type max(ध्रुव Type x, ध्रुव Type y)
{
	तार्किक x_is_nan = std::isnan(x);
	तार्किक y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	जइ (x_is_nan || y_is_nan) {
		जइ (x_is_nan && !y_is_nan) {
			प्रत्यागम y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		प्रत्यागम x;
	}

	प्रत्यागम (x > y) ? x : y;
}

template<typename Type>
Type constrain(ध्रुव Type x, ध्रुव Type lower_bound, ध्रुव Type upper_bound)
{
	जइ (lower_bound > upper_bound) {
		प्रत्यागम NAN;

	} अण्णहा जइ (std::isnan(x)) {
		प्रत्यागम NAN;

	} अण्णहा {
		प्रत्यागम typeFunction::max(lower_bound, typeFunction::min(upper_bound, x));
	}
}
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(ध्रुव Matrix<Type, M, N> &x, ध्रुव Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x(i, j), scalar_upper_bound);
		}
	}

	प्रत्यागम m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(ध्रुव Type scalar_upper_bound, ध्रुव Matrix<Type, M, N> &x)
{
	प्रत्यागम min(x, scalar_upper_bound);
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(ध्रुव Matrix<Type, M, N> &x1, ध्रुव Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x1(i, j), x2(i, j));
		}
	}

	प्रत्यागम m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(ध्रुव Matrix<Type, M, N> &x, ध्रुव Type scalar_lower_bound)
{
	Matrix<Type, M, N> m;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x(i, j), scalar_lower_bound);
		}
	}

	प्रत्यागम m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(ध्रुव Type scalar_lower_bound, ध्रुव Matrix<Type, M, N> &x)
{
	प्रत्यागम max(x, scalar_lower_bound);
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(ध्रुव Matrix<Type, M, N> &x1, ध्रुव Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x1(i, j), x2(i, j));
		}
	}

	प्रत्यागम m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(ध्रुव Matrix<Type, M, N> &x,
			     ध्रुव Type scalar_lower_bound,
			     ध्रुव Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	जइ (scalar_lower_bound > scalar_upper_bound) {
		m.setNaN();

	} अण्णहा {
		प्रति (size_t i = 0; i < M; i++) {
			प्रति (size_t j = 0; j < N; j++) {
				m(i, j) = typeFunction::constrain(x(i, j), scalar_lower_bound, scalar_upper_bound);
			}
		}
	}

	प्रत्यागम m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(ध्रुव Matrix<Type, M, N> &x,
			     ध्रुव Matrix<Type, M, N> &x_lower_bound,
			     ध्रुव Matrix<Type, M, N> &x_upper_bound)
{
	Matrix<Type, M, N> m;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::constrain(x(i, j), x_lower_bound(i, j), x_upper_bound(i, j));
		}
	}

	प्रत्यागम m;
}

template<typename OStream, typename Type, size_t M, size_t N>
OStream &operator<<(OStream &os, ध्रुव matrix::Matrix<Type, M, N> &matrix)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * N * M + M + 1;
	वर्ण string[n];
	matrix.write_string(string, n);
	os << string;
	प्रत्यागम os;
}

} // namespace matrix
