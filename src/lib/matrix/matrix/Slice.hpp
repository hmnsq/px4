/**
 * @file Slice.hpp
 *
 * A simple matrix template library.
 *
 * @author Julian Kent < julian@auterion.com >
 */

#pragma once

#অন্তর্ভাব <cassert>
#অন্তর্ভাব <cstdio>
#অন্তর্ভাব <cmath>

নামাকাশ matrix
{

template<typename Type, size_t M, size_t N>
বর্গ Matrix;

template<typename Type, size_t M>
বর্গ Vector;

template <typename MatrixT, typename Type, size_t P, size_t Q, size_t M, size_t N>
বর্গ SliceT
{
সার্বজনিক:
	using Self = SliceT<MatrixT, Type, P, Q, M, N>;

	SliceT(size_t x0, size_t y0, MatrixT *data) :
		_x0(x0),
		_y0(y0),
		_data(data)
	{
		static_assert(P <= M, "Slice rows bigger than backing matrix");
		static_assert(Q <= N, "Slice cols bigger than backing matrix");
		প্রতিজ্ঞা(x0 + P <= M);
		প্রতিজ্ঞা(y0 + Q <= N);
	}

	SliceT(ধ্রুব Self &other) = স্বভাব;

	ধ্রুব Type &operator()(size_t i, size_t j) ধ্রুব
	{
		প্রতিজ্ঞা(i < P);
		প্রতিজ্ঞা(j < Q);

		প্রত্যাগম (*_data)(_x0 + i, _y0 + j);
	}

	Type &operator()(size_t i, size_t j)
	{
		প্রতিজ্ঞা(i < P);
		প্রতিজ্ঞা(j < Q);

		প্রত্যাগম (*_data)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &operator=(ধ্রুব Self &other)
	{
		// Protect against self-assignment
		যদি (এতত্ == &other) {
			প্রত্যাগম *এতত্;
		}

		প্রত্যাগম এতত্->operator=<M, N>(other);
	}

	template<size_t MM, size_t NN>
	Self &operator=(ধ্রুব SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		প্রত্যাগম self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ধ্রুব SliceT<ধ্রুব Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		প্রত্যাগম self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ধ্রুব Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		প্রত্যাগম self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ধ্রুব Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		প্রত্যাগম self;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator-(ধ্রুব SliceT<ধ্রুব Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		প্রত্যাগম Matrix<Type, P, Q> {*এতত্} - other;
	}


	Matrix<Type, P, Q> operator-(ধ্রুব Matrix<Type, P, Q> &other)
	{
		প্রত্যাগম Matrix<Type, P, Q> {*এতত্} - other;
	}

	Matrix<Type, P, Q> operator-(ধ্রুব Type &other)
	{
		প্রত্যাগম Matrix<Type, P, Q> {*এতত্} - other;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator+(ধ্রুব SliceT<ধ্রুব Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		প্রত্যাগম Matrix<Type, P, Q> {*এতত্} + other;
	}

	Matrix<Type, P, Q> operator+(ধ্রুব Matrix<Type, P, Q> &other)
	{
		প্রত্যাগম Matrix<Type, P, Q> {*এতত্} + other;
	}

	Matrix<Type, P, Q> operator+(ধ্রুব Type &other)
	{
		প্রত্যাগম Matrix<Type, P, Q> {*এতত্} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	template <size_t DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &operator=(ধ্রুব Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *এতত্;

		প্রতি (size_t j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		প্রত্যাগম self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ধ্রুব SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		প্রত্যাগম self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ধ্রুব Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		প্রত্যাগম self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ধ্রুব Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		প্রত্যাগম self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ধ্রুব SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		প্রত্যাগম self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ধ্রুব Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		প্রত্যাগম self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ধ্রুব Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		প্রত্যাগম self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator*=(ধ্রুব Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		প্রত্যাগম self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator/=(ধ্রুব Type &scalar)
	{
		প্রত্যাগম operator*=(Type(1) / scalar);
	}

	Matrix<Type, P, Q> operator*(ধ্রুব Type &scalar) ধ্রুব
	{
		প্রত্যাগম Matrix<Type, P, Q> {*এতত্} * scalar;
	}

	Matrix<Type, P, Q> operator/(ধ্রুব Type &scalar) ধ্রুব
	{
		প্রত্যাগম (*এতত্) * (1 / scalar);
	}

	template<size_t R, size_t S>
	ধ্রুব SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0) ধ্রুব
	{
		প্রত্যাগম SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	template<size_t R, size_t S>
	SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0)
	{
		প্রত্যাগম SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	শূন্য copyTo(Type dst[P * Q]) ধ্রুব
	{
		ধ্রুব SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	শূন্য copyToColumnMajor(Type dst[P * Q]) ধ্রুব
	{
		ধ্রুব SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	Vector < Type, P < Q ? P : Q > diag() ধ্রুব
	{
		ধ্রুব SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;
		Vector < Type, P < Q ? P : Q > res;

		প্রতি (size_t j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		প্রত্যাগম res;
	}

	template<size_t MM, size_t NN>
	Type dot(ধ্রুব SliceT<MatrixT, Type, P, Q, MM, NN> &other) ধ্রুব
	{
		ধ্রুব SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;
		Type accum(0);

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		প্রত্যাগম accum;
	}

	Type norm_squared() ধ্রুব
	{
		প্রত্যাগম dot(*এতত্);
	}

	Type norm() ধ্রুব
	{
		প্রত্যাগম std::sqrt(norm_squared());
	}

	তার্কিক longerThan(Type testVal) ধ্রুব
	{
		প্রত্যাগম norm_squared() > testVal * testVal;
	}

	Type max() ধ্রুব
	{
		Type max_val = (*এতত্)(0, 0);

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				Type val = (*এতত্)(i, j);

				যদি (val > max_val) {
					max_val = val;
				}
			}
		}

		প্রত্যাগম max_val;
	}

	Type min() ধ্রুব
	{
		Type min_val = (*এতত্)(0, 0);

		প্রতি (size_t i = 0; i < P; i++) {
			প্রতি (size_t j = 0; j < Q; j++) {
				Type val = (*এতত্)(i, j);

				যদি (val < min_val) {
					min_val = val;
				}
			}
		}

		প্রত্যাগম min_val;
	}

গুপ্ত:
	size_t _x0, _y0;
	MatrixT *_data;
};

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using ConstSlice = SliceT<ধ্রুব Matrix<Type, M, N>, Type, P, Q, M, N>;

}
