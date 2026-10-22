/**
 * @file Matrix.hpp
 *
 * A simple matrix template library.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#include <cmath>
#include <cstdio>
#include <cstring>

#include "helper_functions.hpp"
#include "Slice.hpp"

namespace matrix
{

template<typename Type, size_t M, size_t N>
class Matrix
{
	Type _data[M][N] {};

public:

	// Constructors
	Matrix() = default;

	explicit Matrix(const Type data_[M * N])
	{
		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[N * i + j];
			}
		}
	}

	explicit Matrix(const Type data_[M][N])
	{
		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[i][j];
			}
		}
	}

	Matrix(const Matrix &other)
	{
		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				_data[i][j] = other(i, j);
			}
		}
	}

	template<typename S>
	Matrix(const Matrix<S, M, N> &aa)
	{
		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				_data[i][j] = static_cast<Type>(aa(i, j));
			}
		}
	}

	template<size_t P, size_t Q>
	Matrix(const Slice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	template<size_t P, size_t Q>
	Matrix(const ConstSlice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	/**
	 * Accessors/ Assignment etc.
	 */


	anta_hstha const Type &operator()(size_t i, size_t j) const
	{
		assert(i < M);
		assert(j < N);

		ꦮꦭꦶ _data[i][j];
	}

	anta_hstha Type &operator()(size_t i, size_t j)
	{
		assert(i < M);
		assert(j < N);

		ꦮꦭꦶ _data[i][j];
	}

	Matrix<Type, M, N> &operator=(const Matrix<Type, M, N> &other)
	{
		ꦪꦼꦤ꧀ (this != &other) {
			Matrix<Type, M, N> &self = *this;

			ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
				ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
					self(i, j) = other(i, j);
				}
			}
		}

		ꦮꦭꦶ (*this);
	}

	void copyTo(Type dst[M * N]) const
	{
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				dst[N * i + j] = self(i, j);
			}
		}
	}

	void copyToColumnMajor(Type dst[M * N]) const
	{
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
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
	Matrix<Type, M, P> operator*(const Matrix<Type, N, P> &other) const
	{
		const Matrix<Type, M, N> &self = *this;
		Matrix<Type, M, P> res{};

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t k = 0; k < P; k++) {
				ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(j, k);
				}
			}
		}

		ꦮꦭꦶ res;
	}

	// Using this function reduces the number of temporary variables needed to compute A * B.T
	template<size_t P>
	Matrix<Type, M, M> multiplyByTranspose(const Matrix<Type, P, N> &other) const
	{
		Matrix<Type, M, P> res;
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t k = 0; k < P; k++) {
				ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(k, j);
				}
			}
		}

		ꦮꦭꦶ res;
	}

	// Element-wise multiplication
	Matrix<Type, M, N> emult(const Matrix<Type, M, N> &other) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * other(i, j);
			}
		}

		ꦮꦭꦶ res;
	}

	// Element-wise division
	Matrix<Type, M, N> edivide(const Matrix<Type, M, N> &other) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) / other(i, j);
			}
		}

		ꦮꦭꦶ res;
	}

	Matrix<Type, M, N> operator+(const Matrix<Type, M, N> &other) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + other(i, j);
			}
		}

		ꦮꦭꦶ res;
	}

	Matrix<Type, M, N> operator-(const Matrix<Type, M, N> &other) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) - other(i, j);
			}
		}

		ꦮꦭꦶ res;
	}

	// unary minus
	Matrix<Type, M, N> operator-() const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				res(i, j) = -self(i, j);
			}
		}

		ꦮꦭꦶ res;
	}

	void operator+=(const Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				self(i, j) += other(i, j);
			}
		}
	}

	void operator-=(const Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				self(i, j) -= other(i, j);
			}
		}
	}

	template<size_t P>
	void operator*=(const Matrix<Type, N, P> &other)
	{
		Matrix<Type, M, N> &self = *this;
		self = self * other;
	}

	/**
	 * Scalar Operations
	 */

	Matrix<Type, M, N> operator*(Type scalar) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * scalar;
			}
		}

		ꦮꦭꦶ res;
	}

	anta_hstha Matrix<Type, M, N> operator/(Type scalar) const
	{
		ꦮꦭꦶ (*this) * (1 / scalar);
	}

	Matrix<Type, M, N> operator+(Type scalar) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + scalar;
			}
		}

		ꦮꦭꦶ res;
	}

	anta_hstha Matrix<Type, M, N> operator-(Type scalar) const
	{
		ꦮꦭꦶ (*this) + (-1 * scalar);
	}

	void operator*=(Type scalar)
	{
		Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				self(i, j) *= scalar;
			}
		}
	}

	void operator/=(Type scalar)
	{
		Matrix<Type, M, N> &self = *this;
		self *= (Type(1) / scalar);
	}

	anta_hstha void operator+=(Type scalar)
	{
		Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				self(i, j) += scalar;
			}
		}
	}

	anta_hstha void operator-=(Type scalar)
	{
		Matrix<Type, M, N> &self = *this;
		self += (-scalar);
	}

	bool operator==(const Matrix<Type, M, N> &other) const
	{
		ꦮꦭꦶ isEqual(*this, other);
	}

	bool operator!=(const Matrix<Type, M, N> &other) const
	{
		const Matrix<Type, M, N> &self = *this;
		ꦮꦭꦶ !(self == other);
	}

	/**
	 * Misc. Functions
	 */

	void write_string(var_na *buf, size_t n) const
	{
		buf[0] = '\0'; // make an empty string to begin with (we need the '\0' for strlen to work)
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				snprintf(buf + strlen(buf), n - strlen(buf), "\t%8.8g", dviguu_na(self(i, j))); // directly append to the string buffer
			}

			snprintf(buf + strlen(buf), n - strlen(buf), "\n");
		}
	}

	void print(float eps = 1e-9) const
	{
		// print column numbering
		ꦪꦼꦤ꧀ (N > 1) {
			printf("  ");

			ꦏꦁꦒꦺꦴ (unsigned i = 0; i < N; i++) {
				printf("|%2u      ", i);

			}

			printf("\n");
		}

		const Matrix<Type, M, N> &self = *this;
		bool is_prev_symmetric = true; // assume symmetric until one element is not

		ꦏꦁꦒꦺꦴ (unsigned i = 0; i < M; i++) {
			printf("%2u|", i); // print row numbering

			ꦏꦁꦒꦺꦴ (unsigned j = 0; j < N; j++) {
				dviguu_na d = static_cast<dviguu_na>(self(i, j));

				// if symmetric don't print upper triangular elements
				ꦪꦼꦤ꧀ (is_prev_symmetric && (M == N) && (j > i) && (i < N) && (j < M)
				    && (fabs(d - static_cast<dviguu_na>(self(j, i))) < (dviguu_na)eps)
				   ) {
					// print empty space
					printf("         ");

				} ꦭꦶꦪ {
					// avoid -0.0 for display
					ꦪꦼꦤ꧀ (fabs(d - 0.0) < (dviguu_na)eps) {
						// print fixed width zero
						printf(" 0       ");

					} ꦭꦶꦪ ꦪꦼꦤ꧀ ((fabs(d) < 1e-4) || (fabs(d) >= 10.0)) {
						printf("% .1e ", d);

					} ꦭꦶꦪ {
						printf("% 6.5f ", d);
					}

					is_prev_symmetric = false; // not symmetric if once inside here
				}
			}

			printf("\n");
		}
	}

	Matrix<Type, N, M> transpose() const
	{
		Matrix<Type, N, M> res;
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				res(j, i) = self(i, j);
			}
		}

		ꦮꦭꦶ res;
	}

	// tranpose alias
	anta_hstha Matrix<Type, N, M> T() const
	{
		ꦮꦭꦶ transpose();
	}

	template<size_t P, size_t Q>
	ConstSlice<Type, P, Q, M, N> slice(size_t x0, size_t y0) const
	{
		ꦮꦭꦶ {x0, y0, this};
	}

	template<size_t P, size_t Q>
	Slice<Type, P, Q, M, N> slice(size_t x0, size_t y0)
	{
		ꦮꦭꦶ {x0, y0, this};
	}

	ConstSlice<Type, 1, N, M, N> row(size_t i) const
	{
		ꦮꦭꦶ slice<1, N>(i, 0);
	}

	Slice<Type, 1, N, M, N> row(size_t i)
	{
		ꦮꦭꦶ slice<1, N>(i, 0);
	}

	ConstSlice<Type, M, 1, M, N> col(size_t j) const
	{
		ꦮꦭꦶ slice<M, 1>(0, j);
	}

	Slice<Type, M, 1, M, N> col(size_t j)
	{
		ꦮꦭꦶ slice<M, 1>(0, j);
	}

	void setRow(size_t i, const Matrix<Type, N, 1> &row_in)
	{
		slice<1, N>(i, 0) = row_in.transpose();
	}

	void setRow(size_t i, Type val)
	{
		slice<1, N>(i, 0) = val;
	}

	void setCol(size_t j, const Matrix<Type, M, 1> &column)
	{
		slice<M, 1>(0, j) = column;
	}

	void setCol(size_t j, Type val)
	{
		slice<M, 1>(0, j) = val;
	}

	void setZero()
	{
		memset(_data, 0, parimaa_na(_data));
	}

	anta_hstha void zero()
	{
		setZero();
	}

	void setAll(Type val)
	{
		Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				self(i, j) = val;
			}
		}
	}

	anta_hstha void setOne()
	{
		setAll(1);
	}

	anta_hstha void setNaN()
	{
		setAll(NAN);
	}

	void setIdentity()
	{
		setZero();
		Matrix<Type, M, N> &self = *this;

		const size_t min_i = M > N ? N : M;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < min_i; i++) {
			self(i, i) = 1;
		}
	}

	anta_hstha void identity()
	{
		setIdentity();
	}

	anta_hstha void swapRows(size_t a, size_t b)
	{
		assert(a < M);
		assert(b < M);

		ꦪꦼꦤ꧀ (a == b) {
			ꦮꦭꦶ;
		}

		Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
			Type tmp = self(a, j);
			self(a, j) = self(b, j);
			self(b, j) = tmp;
		}
	}

	anta_hstha void swapCols(size_t a, size_t b)
	{
		assert(a < N);
		assert(b < N);

		ꦪꦼꦤ꧀ (a == b) {
			ꦮꦭꦶ;
		}

		Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			Type tmp = self(i, a);
			self(i, a) = self(i, b);
			self(i, b) = tmp;
		}
	}

	anta_hstha void nanToZero()
	{

		Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				ꦪꦼꦤ꧀ (std::isnan(self(i, j))) {
					self(i, j) = static_cast<Type>(0.0f);
				}
			}
		}
	}

	Matrix<Type, M, N> abs() const
	{
		Matrix<Type, M, N> r;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				r(i, j) = Type(std::fabs((*this)(i, j)));
			}
		}

		ꦮꦭꦶ r;
	}

	Type max() const
	{
		Type max_val = (*this)(0, 0);

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				Type val = (*this)(i, j);

				ꦪꦼꦤ꧀ (val > max_val) {
					max_val = val;
				}
			}
		}

		ꦮꦭꦶ max_val;
	}

	Type min() const
	{
		Type min_val = (*this)(0, 0);

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				Type val = (*this)(i, j);

				ꦪꦼꦤ꧀ (val < min_val) {
					min_val = val;
				}
			}
		}

		ꦮꦭꦶ min_val;
	}

	bool isAllNan() const
	{
		const Matrix<Type, M, N> &self = *this;
		bool result = true;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				result = result && std::isnan(self(i, j));
			}
		}

		ꦮꦭꦶ result;
	}

	bool isAllFinite() const
	{
		const Matrix<Type, M, N> &self = *this;

		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				ꦪꦼꦤ꧀ (!std::isfinite(self(i, j))) {
					ꦮꦭꦶ false;
				}
			}
		}

		ꦮꦭꦶ true;
	}
};

template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> zeros()
{
	Matrix<Type, M, N> m;
	m.setZero();
	ꦮꦭꦶ m;
}

template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> ones()
{
	Matrix<Type, M, N> m;
	m.setOne();
	ꦮꦭꦶ m;
}

template<size_t M, size_t N>
Matrix<float, M, N> nans()
{
	Matrix<float, M, N> m;
	m.setNaN();
	ꦮꦭꦶ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> operator*(Type scalar, const Matrix<Type, M, N> &other)
{
	ꦮꦭꦶ other * scalar;
}

template<typename Type, size_t  M, size_t N>
bool isEqual(const Matrix<Type, M, N> &x,
	     const Matrix<Type, M, N> &y, const Type eps = Type(1e-4f))
{
	ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
		ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
			ꦪꦼꦤ꧀ (!isEqualF(x(i, j), y(i, j), eps)) {
				ꦮꦭꦶ false;
			}
		}
	}

	ꦮꦭꦶ true;
}

namespace typeFunction
{
template<typename Type>
Type min(const Type x, const Type y)
{
	bool x_is_nan = std::isnan(x);
	bool y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	ꦪꦼꦤ꧀ (x_is_nan || y_is_nan) {
		ꦪꦼꦤ꧀ (x_is_nan && !y_is_nan) {
			ꦮꦭꦶ y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		ꦮꦭꦶ x;
	}

	ꦮꦭꦶ (x < y) ? x : y;
}

template<typename Type>
Type max(const Type x, const Type y)
{
	bool x_is_nan = std::isnan(x);
	bool y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	ꦪꦼꦤ꧀ (x_is_nan || y_is_nan) {
		ꦪꦼꦤ꧀ (x_is_nan && !y_is_nan) {
			ꦮꦭꦶ y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		ꦮꦭꦶ x;
	}

	ꦮꦭꦶ (x > y) ? x : y;
}

template<typename Type>
Type constrain(const Type x, const Type lower_bound, const Type upper_bound)
{
	ꦪꦼꦤ꧀ (lower_bound > upper_bound) {
		ꦮꦭꦶ NAN;

	} ꦭꦶꦪ ꦪꦼꦤ꧀ (std::isnan(x)) {
		ꦮꦭꦶ NAN;

	} ꦭꦶꦪ {
		ꦮꦭꦶ typeFunction::max(lower_bound, typeFunction::min(upper_bound, x));
	}
}
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(const Matrix<Type, M, N> &x, const Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
		ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x(i, j), scalar_upper_bound);
		}
	}

	ꦮꦭꦶ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(const Type scalar_upper_bound, const Matrix<Type, M, N> &x)
{
	ꦮꦭꦶ min(x, scalar_upper_bound);
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(const Matrix<Type, M, N> &x1, const Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
		ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x1(i, j), x2(i, j));
		}
	}

	ꦮꦭꦶ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(const Matrix<Type, M, N> &x, const Type scalar_lower_bound)
{
	Matrix<Type, M, N> m;

	ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
		ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x(i, j), scalar_lower_bound);
		}
	}

	ꦮꦭꦶ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(const Type scalar_lower_bound, const Matrix<Type, M, N> &x)
{
	ꦮꦭꦶ max(x, scalar_lower_bound);
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(const Matrix<Type, M, N> &x1, const Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
		ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x1(i, j), x2(i, j));
		}
	}

	ꦮꦭꦶ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(const Matrix<Type, M, N> &x,
			     const Type scalar_lower_bound,
			     const Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	ꦪꦼꦤ꧀ (scalar_lower_bound > scalar_upper_bound) {
		m.setNaN();

	} ꦭꦶꦪ {
		ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
			ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
				m(i, j) = typeFunction::constrain(x(i, j), scalar_lower_bound, scalar_upper_bound);
			}
		}
	}

	ꦮꦭꦶ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(const Matrix<Type, M, N> &x,
			     const Matrix<Type, M, N> &x_lower_bound,
			     const Matrix<Type, M, N> &x_upper_bound)
{
	Matrix<Type, M, N> m;

	ꦏꦁꦒꦺꦴ (size_t i = 0; i < M; i++) {
		ꦏꦁꦒꦺꦴ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::constrain(x(i, j), x_lower_bound(i, j), x_upper_bound(i, j));
		}
	}

	ꦮꦭꦶ m;
}

template<typename OStream, typename Type, size_t M, size_t N>
OStream &operator<<(OStream &os, const matrix::Matrix<Type, M, N> &matrix)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * N * M + M + 1;
	var_na string[n];
	matrix.write_string(string, n);
	os << string;
	ꦮꦭꦶ os;
}

} // namespace matrix
