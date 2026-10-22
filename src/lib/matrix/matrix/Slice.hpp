/**
 * @file Slice.hpp
 *
 * A simple matrix template library.
 *
 * @author Julian Kent < julian@auterion.com >
 */

#pragma once

#include <cassert>
#include <cstdio>
#include <cmath>

namespace matrix
{

template<typename Type, size_t M, size_t N>
class Matrix;

template<typename Type, size_t M>
class Vector;

template <typename MatrixT, typename Type, size_t P, size_t Q, size_t M, size_t N>
class SliceT
{
public:
	using Self = SliceT<MatrixT, Type, P, Q, M, N>;

	SliceT(size_t x0, size_t y0, MatrixT *data) :
		_x0(x0),
		_y0(y0),
		_data(data)
	{
		static_assert(P <= M, "Slice rows bigger than backing matrix");
		static_assert(Q <= N, "Slice cols bigger than backing matrix");
		assert(x0 + P <= M);
		assert(y0 + Q <= N);
	}

	SliceT(const Self &other) = default;

	const Type &operator()(size_t i, size_t j) const
	{
		assert(i < P);
		assert(j < Q);

		ལོག (*_data)(_x0 + i, _y0 + j);
	}

	Type &operator()(size_t i, size_t j)
	{
		assert(i < P);
		assert(j < Q);

		ལོག (*_data)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &operator=(const Self &other)
	{
		// Protect against self-assignment
		གལ་སྲིད (this == &other) {
			ལོག *this;
		}

		ལོག this->operator=<M, N>(other);
	}

	template<size_t MM, size_t NN>
	Self &operator=(const SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ལོག self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(const SliceT<const Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ལོག self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(const Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ལོག self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(const Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		ལོག self;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator-(const SliceT<const Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		ལོག Matrix<Type, P, Q> {*this} - other;
	}


	Matrix<Type, P, Q> operator-(const Matrix<Type, P, Q> &other)
	{
		ལོག Matrix<Type, P, Q> {*this} - other;
	}

	Matrix<Type, P, Q> operator-(const Type &other)
	{
		ལོག Matrix<Type, P, Q> {*this} - other;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator+(const SliceT<const Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		ལོག Matrix<Type, P, Q> {*this} + other;
	}

	Matrix<Type, P, Q> operator+(const Matrix<Type, P, Q> &other)
	{
		ལོག Matrix<Type, P, Q> {*this} + other;
	}

	Matrix<Type, P, Q> operator+(const Type &other)
	{
		ལོག Matrix<Type, P, Q> {*this} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	template <size_t DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &operator=(const Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *this;

		དོན་ལུ (size_t j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		ལོག self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(const SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		ལོག self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(const Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		ལོག self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(const Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		ལོག self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(const SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		ལོག self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(const Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		ལོག self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(const Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		ལོག self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator*=(const Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		ལོག self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator/=(const Type &scalar)
	{
		ལོག operator*=(Type(1) / scalar);
	}

	Matrix<Type, P, Q> operator*(const Type &scalar) const
	{
		ལོག Matrix<Type, P, Q> {*this} * scalar;
	}

	Matrix<Type, P, Q> operator/(const Type &scalar) const
	{
		ལོག (*this) * (1 / scalar);
	}

	template<size_t R, size_t S>
	const SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0) const
	{
		ལོག SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	template<size_t R, size_t S>
	SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0)
	{
		ལོག SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	void copyTo(Type dst[P * Q]) const
	{
		const SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	void copyToColumnMajor(Type dst[P * Q]) const
	{
		const SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	Vector < Type, P < Q ? P : Q > diag() const
	{
		const SliceT<MatrixT, Type, P, Q, M, N> &self = *this;
		Vector < Type, P < Q ? P : Q > res;

		དོན་ལུ (size_t j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		ལོག res;
	}

	template<size_t MM, size_t NN>
	Type dot(const SliceT<MatrixT, Type, P, Q, MM, NN> &other) const
	{
		const SliceT<MatrixT, Type, P, Q, M, N> &self = *this;
		Type accum(0);

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		ལོག accum;
	}

	Type norm_squared() const
	{
		ལོག dot(*this);
	}

	Type norm() const
	{
		ལོག std::sqrt(norm_squared());
	}

	bool longerThan(Type testVal) const
	{
		ལོག norm_squared() > testVal * testVal;
	}

	Type max() const
	{
		Type max_val = (*this)(0, 0);

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				Type val = (*this)(i, j);

				གལ་སྲིད (val > max_val) {
					max_val = val;
				}
			}
		}

		ལོག max_val;
	}

	Type min() const
	{
		Type min_val = (*this)(0, 0);

		དོན་ལུ (size_t i = 0; i < P; i++) {
			དོན་ལུ (size_t j = 0; j < Q; j++) {
				Type val = (*this)(i, j);

				གལ་སྲིད (val < min_val) {
					min_val = val;
				}
			}
		}

		ལོག min_val;
	}

private:
	size_t _x0, _y0;
	MatrixT *_data;
};

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using ConstSlice = SliceT<const Matrix<Type, M, N>, Type, P, Q, M, N>;

}
