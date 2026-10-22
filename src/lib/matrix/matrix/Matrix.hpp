/**
 * @file Matrix.hpp
 *
 * A simple matrix template library.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#அந்தர்பாவ <cmath>
#அந்தர்பாவ <cstdio>
#அந்தர்பாவ <cstring>

#அந்தர்பாவ "helper_functions.hpp"
#அந்தர்பாவ "Slice.hpp"

நாமாகாஶ matrix
{

template<typename Type, size_t M, size_t N>
வகுப்பு Matrix
{
	Type _data[M][N] {};

பொது:

	// Constructors
	Matrix() = இயல்பு;

	explicit Matrix(மாறிலி Type data_[M * N])
	{
		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[N * i + j];
			}
		}
	}

	explicit Matrix(மாறிலி Type data_[M][N])
	{
		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[i][j];
			}
		}
	}

	Matrix(மாறிலி Matrix &other)
	{
		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				_data[i][j] = other(i, j);
			}
		}
	}

	template<typename S>
	Matrix(மாறிலி Matrix<S, M, N> &aa)
	{
		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				_data[i][j] = static_cast<Type>(aa(i, j));
			}
		}
	}

	template<size_t P, size_t Q>
	Matrix(மாறிலி Slice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	template<size_t P, size_t Q>
	Matrix(மாறிலி ConstSlice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	/**
	 * Accessors/ Assignment etc.
	 */


	உள்வரி மாறிலி Type &operator()(size_t i, size_t j) மாறிலி
	{
		ப்ரதிஜ்ஞா(i < M);
		ப்ரதிஜ்ஞா(j < N);

		திரும்பு _data[i][j];
	}

	உள்வரி Type &operator()(size_t i, size_t j)
	{
		ப்ரதிஜ்ஞா(i < M);
		ப்ரதிஜ்ஞா(j < N);

		திரும்பு _data[i][j];
	}

	Matrix<Type, M, N> &operator=(மாறிலி Matrix<Type, M, N> &other)
	{
		எனில் (இது != &other) {
			Matrix<Type, M, N> &self = *இது;

			க்கு (size_t i = 0; i < M; i++) {
				க்கு (size_t j = 0; j < N; j++) {
					self(i, j) = other(i, j);
				}
			}
		}

		திரும்பு (*இது);
	}

	வெறுமை copyTo(Type dst[M * N]) மாறிலி
	{
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				dst[N * i + j] = self(i, j);
			}
		}
	}

	வெறுமை copyToColumnMajor(Type dst[M * N]) மாறிலி
	{
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
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
	Matrix<Type, M, P> operator*(மாறிலி Matrix<Type, N, P> &other) மாறிலி
	{
		மாறிலி Matrix<Type, M, N> &self = *இது;
		Matrix<Type, M, P> res{};

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t k = 0; k < P; k++) {
				க்கு (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(j, k);
				}
			}
		}

		திரும்பு res;
	}

	// Using this function reduces the number of temporary variables needed to compute A * B.T
	template<size_t P>
	Matrix<Type, M, M> multiplyByTranspose(மாறிலி Matrix<Type, P, N> &other) மாறிலி
	{
		Matrix<Type, M, P> res;
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t k = 0; k < P; k++) {
				க்கு (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(k, j);
				}
			}
		}

		திரும்பு res;
	}

	// Element-wise multiplication
	Matrix<Type, M, N> emult(மாறிலி Matrix<Type, M, N> &other) மாறிலி
	{
		Matrix<Type, M, N> res;
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * other(i, j);
			}
		}

		திரும்பு res;
	}

	// Element-wise division
	Matrix<Type, M, N> edivide(மாறிலி Matrix<Type, M, N> &other) மாறிலி
	{
		Matrix<Type, M, N> res;
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) / other(i, j);
			}
		}

		திரும்பு res;
	}

	Matrix<Type, M, N> operator+(மாறிலி Matrix<Type, M, N> &other) மாறிலி
	{
		Matrix<Type, M, N> res;
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + other(i, j);
			}
		}

		திரும்பு res;
	}

	Matrix<Type, M, N> operator-(மாறிலி Matrix<Type, M, N> &other) மாறிலி
	{
		Matrix<Type, M, N> res;
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) - other(i, j);
			}
		}

		திரும்பு res;
	}

	// unary minus
	Matrix<Type, M, N> operator-() மாறிலி
	{
		Matrix<Type, M, N> res;
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				res(i, j) = -self(i, j);
			}
		}

		திரும்பு res;
	}

	வெறுமை operator+=(மாறிலி Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				self(i, j) += other(i, j);
			}
		}
	}

	வெறுமை operator-=(மாறிலி Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				self(i, j) -= other(i, j);
			}
		}
	}

	template<size_t P>
	வெறுமை operator*=(மாறிலி Matrix<Type, N, P> &other)
	{
		Matrix<Type, M, N> &self = *இது;
		self = self * other;
	}

	/**
	 * Scalar Operations
	 */

	Matrix<Type, M, N> operator*(Type scalar) மாறிலி
	{
		Matrix<Type, M, N> res;
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * scalar;
			}
		}

		திரும்பு res;
	}

	உள்வரி Matrix<Type, M, N> operator/(Type scalar) மாறிலி
	{
		திரும்பு (*இது) * (1 / scalar);
	}

	Matrix<Type, M, N> operator+(Type scalar) மாறிலி
	{
		Matrix<Type, M, N> res;
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + scalar;
			}
		}

		திரும்பு res;
	}

	உள்வரி Matrix<Type, M, N> operator-(Type scalar) மாறிலி
	{
		திரும்பு (*இது) + (-1 * scalar);
	}

	வெறுமை operator*=(Type scalar)
	{
		Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				self(i, j) *= scalar;
			}
		}
	}

	வெறுமை operator/=(Type scalar)
	{
		Matrix<Type, M, N> &self = *இது;
		self *= (Type(1) / scalar);
	}

	உள்வரி வெறுமை operator+=(Type scalar)
	{
		Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				self(i, j) += scalar;
			}
		}
	}

	உள்வரி வெறுமை operator-=(Type scalar)
	{
		Matrix<Type, M, N> &self = *இது;
		self += (-scalar);
	}

	தருக்க operator==(மாறிலி Matrix<Type, M, N> &other) மாறிலி
	{
		திரும்பு isEqual(*இது, other);
	}

	தருக்க operator!=(மாறிலி Matrix<Type, M, N> &other) மாறிலி
	{
		மாறிலி Matrix<Type, M, N> &self = *இது;
		திரும்பு !(self == other);
	}

	/**
	 * Misc. Functions
	 */

	வெறுமை write_string(எழுத்து *buf, size_t n) மாறிலி
	{
		buf[0] = '\0'; // make an empty string to begin with (we need the '\0' for strlen to work)
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				snprintf(buf + strlen(buf), n - strlen(buf), "\t%8.8g", இரட்டை(self(i, j))); // directly append to the string buffer
			}

			snprintf(buf + strlen(buf), n - strlen(buf), "\n");
		}
	}

	வெறுமை print(தசம eps = 1e-9) மாறிலி
	{
		// print column numbering
		எனில் (N > 1) {
			printf("  ");

			க்கு (unsigned i = 0; i < N; i++) {
				printf("|%2u      ", i);

			}

			printf("\n");
		}

		மாறிலி Matrix<Type, M, N> &self = *இது;
		தருக்க is_prev_symmetric = உண்மை; // assume symmetric until one element is not

		க்கு (unsigned i = 0; i < M; i++) {
			printf("%2u|", i); // print row numbering

			க்கு (unsigned j = 0; j < N; j++) {
				இரட்டை d = static_cast<இரட்டை>(self(i, j));

				// if symmetric don't print upper triangular elements
				எனில் (is_prev_symmetric && (M == N) && (j > i) && (i < N) && (j < M)
				    && (fabs(d - static_cast<இரட்டை>(self(j, i))) < (இரட்டை)eps)
				   ) {
					// print empty space
					printf("         ");

				} இல்லையெனில் {
					// avoid -0.0 for display
					எனில் (fabs(d - 0.0) < (இரட்டை)eps) {
						// print fixed width zero
						printf(" 0       ");

					} இல்லையெனில் எனில் ((fabs(d) < 1e-4) || (fabs(d) >= 10.0)) {
						printf("% .1e ", d);

					} இல்லையெனில் {
						printf("% 6.5f ", d);
					}

					is_prev_symmetric = பொய்; // not symmetric if once inside here
				}
			}

			printf("\n");
		}
	}

	Matrix<Type, N, M> transpose() மாறிலி
	{
		Matrix<Type, N, M> res;
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				res(j, i) = self(i, j);
			}
		}

		திரும்பு res;
	}

	// tranpose alias
	உள்வரி Matrix<Type, N, M> T() மாறிலி
	{
		திரும்பு transpose();
	}

	template<size_t P, size_t Q>
	ConstSlice<Type, P, Q, M, N> slice(size_t x0, size_t y0) மாறிலி
	{
		திரும்பு {x0, y0, இது};
	}

	template<size_t P, size_t Q>
	Slice<Type, P, Q, M, N> slice(size_t x0, size_t y0)
	{
		திரும்பு {x0, y0, இது};
	}

	ConstSlice<Type, 1, N, M, N> row(size_t i) மாறிலி
	{
		திரும்பு slice<1, N>(i, 0);
	}

	Slice<Type, 1, N, M, N> row(size_t i)
	{
		திரும்பு slice<1, N>(i, 0);
	}

	ConstSlice<Type, M, 1, M, N> col(size_t j) மாறிலி
	{
		திரும்பு slice<M, 1>(0, j);
	}

	Slice<Type, M, 1, M, N> col(size_t j)
	{
		திரும்பு slice<M, 1>(0, j);
	}

	வெறுமை setRow(size_t i, மாறிலி Matrix<Type, N, 1> &row_in)
	{
		slice<1, N>(i, 0) = row_in.transpose();
	}

	வெறுமை setRow(size_t i, Type val)
	{
		slice<1, N>(i, 0) = val;
	}

	வெறுமை setCol(size_t j, மாறிலி Matrix<Type, M, 1> &column)
	{
		slice<M, 1>(0, j) = column;
	}

	வெறுமை setCol(size_t j, Type val)
	{
		slice<M, 1>(0, j) = val;
	}

	வெறுமை setZero()
	{
		memset(_data, 0, அளவு(_data));
	}

	உள்வரி வெறுமை zero()
	{
		setZero();
	}

	வெறுமை setAll(Type val)
	{
		Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				self(i, j) = val;
			}
		}
	}

	உள்வரி வெறுமை setOne()
	{
		setAll(1);
	}

	உள்வரி வெறுமை setNaN()
	{
		setAll(NAN);
	}

	வெறுமை setIdentity()
	{
		setZero();
		Matrix<Type, M, N> &self = *இது;

		மாறிலி size_t min_i = M > N ? N : M;

		க்கு (size_t i = 0; i < min_i; i++) {
			self(i, i) = 1;
		}
	}

	உள்வரி வெறுமை identity()
	{
		setIdentity();
	}

	உள்வரி வெறுமை swapRows(size_t a, size_t b)
	{
		ப்ரதிஜ்ஞா(a < M);
		ப்ரதிஜ்ஞா(b < M);

		எனில் (a == b) {
			திரும்பு;
		}

		Matrix<Type, M, N> &self = *இது;

		க்கு (size_t j = 0; j < N; j++) {
			Type tmp = self(a, j);
			self(a, j) = self(b, j);
			self(b, j) = tmp;
		}
	}

	உள்வரி வெறுமை swapCols(size_t a, size_t b)
	{
		ப்ரதிஜ்ஞா(a < N);
		ப்ரதிஜ்ஞா(b < N);

		எனில் (a == b) {
			திரும்பு;
		}

		Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			Type tmp = self(i, a);
			self(i, a) = self(i, b);
			self(i, b) = tmp;
		}
	}

	உள்வரி வெறுமை nanToZero()
	{

		Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				எனில் (std::isnan(self(i, j))) {
					self(i, j) = static_cast<Type>(0.0f);
				}
			}
		}
	}

	Matrix<Type, M, N> abs() மாறிலி
	{
		Matrix<Type, M, N> r;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				r(i, j) = Type(std::fabs((*இது)(i, j)));
			}
		}

		திரும்பு r;
	}

	Type max() மாறிலி
	{
		Type max_val = (*இது)(0, 0);

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				Type val = (*இது)(i, j);

				எனில் (val > max_val) {
					max_val = val;
				}
			}
		}

		திரும்பு max_val;
	}

	Type min() மாறிலி
	{
		Type min_val = (*இது)(0, 0);

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				Type val = (*இது)(i, j);

				எனில் (val < min_val) {
					min_val = val;
				}
			}
		}

		திரும்பு min_val;
	}

	தருக்க isAllNan() மாறிலி
	{
		மாறிலி Matrix<Type, M, N> &self = *இது;
		தருக்க result = உண்மை;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				result = result && std::isnan(self(i, j));
			}
		}

		திரும்பு result;
	}

	தருக்க isAllFinite() மாறிலி
	{
		மாறிலி Matrix<Type, M, N> &self = *இது;

		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				எனில் (!std::isfinite(self(i, j))) {
					திரும்பு பொய்;
				}
			}
		}

		திரும்பு உண்மை;
	}
};

template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> zeros()
{
	Matrix<Type, M, N> m;
	m.setZero();
	திரும்பு m;
}

template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> ones()
{
	Matrix<Type, M, N> m;
	m.setOne();
	திரும்பு m;
}

template<size_t M, size_t N>
Matrix<தசம, M, N> nans()
{
	Matrix<தசம, M, N> m;
	m.setNaN();
	திரும்பு m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> operator*(Type scalar, மாறிலி Matrix<Type, M, N> &other)
{
	திரும்பு other * scalar;
}

template<typename Type, size_t  M, size_t N>
தருக்க isEqual(மாறிலி Matrix<Type, M, N> &x,
	     மாறிலி Matrix<Type, M, N> &y, மாறிலி Type eps = Type(1e-4f))
{
	க்கு (size_t i = 0; i < M; i++) {
		க்கு (size_t j = 0; j < N; j++) {
			எனில் (!isEqualF(x(i, j), y(i, j), eps)) {
				திரும்பு பொய்;
			}
		}
	}

	திரும்பு உண்மை;
}

நாமாகாஶ typeFunction
{
template<typename Type>
Type min(மாறிலி Type x, மாறிலி Type y)
{
	தருக்க x_is_nan = std::isnan(x);
	தருக்க y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	எனில் (x_is_nan || y_is_nan) {
		எனில் (x_is_nan && !y_is_nan) {
			திரும்பு y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		திரும்பு x;
	}

	திரும்பு (x < y) ? x : y;
}

template<typename Type>
Type max(மாறிலி Type x, மாறிலி Type y)
{
	தருக்க x_is_nan = std::isnan(x);
	தருக்க y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	எனில் (x_is_nan || y_is_nan) {
		எனில் (x_is_nan && !y_is_nan) {
			திரும்பு y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		திரும்பு x;
	}

	திரும்பு (x > y) ? x : y;
}

template<typename Type>
Type constrain(மாறிலி Type x, மாறிலி Type lower_bound, மாறிலி Type upper_bound)
{
	எனில் (lower_bound > upper_bound) {
		திரும்பு NAN;

	} இல்லையெனில் எனில் (std::isnan(x)) {
		திரும்பு NAN;

	} இல்லையெனில் {
		திரும்பு typeFunction::max(lower_bound, typeFunction::min(upper_bound, x));
	}
}
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(மாறிலி Matrix<Type, M, N> &x, மாறிலி Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	க்கு (size_t i = 0; i < M; i++) {
		க்கு (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x(i, j), scalar_upper_bound);
		}
	}

	திரும்பு m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(மாறிலி Type scalar_upper_bound, மாறிலி Matrix<Type, M, N> &x)
{
	திரும்பு min(x, scalar_upper_bound);
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(மாறிலி Matrix<Type, M, N> &x1, மாறிலி Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	க்கு (size_t i = 0; i < M; i++) {
		க்கு (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x1(i, j), x2(i, j));
		}
	}

	திரும்பு m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(மாறிலி Matrix<Type, M, N> &x, மாறிலி Type scalar_lower_bound)
{
	Matrix<Type, M, N> m;

	க்கு (size_t i = 0; i < M; i++) {
		க்கு (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x(i, j), scalar_lower_bound);
		}
	}

	திரும்பு m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(மாறிலி Type scalar_lower_bound, மாறிலி Matrix<Type, M, N> &x)
{
	திரும்பு max(x, scalar_lower_bound);
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(மாறிலி Matrix<Type, M, N> &x1, மாறிலி Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	க்கு (size_t i = 0; i < M; i++) {
		க்கு (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x1(i, j), x2(i, j));
		}
	}

	திரும்பு m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(மாறிலி Matrix<Type, M, N> &x,
			     மாறிலி Type scalar_lower_bound,
			     மாறிலி Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	எனில் (scalar_lower_bound > scalar_upper_bound) {
		m.setNaN();

	} இல்லையெனில் {
		க்கு (size_t i = 0; i < M; i++) {
			க்கு (size_t j = 0; j < N; j++) {
				m(i, j) = typeFunction::constrain(x(i, j), scalar_lower_bound, scalar_upper_bound);
			}
		}
	}

	திரும்பு m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(மாறிலி Matrix<Type, M, N> &x,
			     மாறிலி Matrix<Type, M, N> &x_lower_bound,
			     மாறிலி Matrix<Type, M, N> &x_upper_bound)
{
	Matrix<Type, M, N> m;

	க்கு (size_t i = 0; i < M; i++) {
		க்கு (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::constrain(x(i, j), x_lower_bound(i, j), x_upper_bound(i, j));
		}
	}

	திரும்பு m;
}

template<typename OStream, typename Type, size_t M, size_t N>
OStream &operator<<(OStream &os, மாறிலி matrix::Matrix<Type, M, N> &matrix)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * N * M + M + 1;
	எழுத்து string[n];
	matrix.write_string(string, n);
	os << string;
	திரும்பு os;
}

} // namespace matrix
