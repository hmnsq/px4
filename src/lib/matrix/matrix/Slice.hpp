/**
 * @file Slice.hpp
 *
 * A simple matrix template library.
 *
 * @author Julian Kent < julian@auterion.com >
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ <cassert>
#ଅନ୍ତର୍ଭାଵ <cstdio>
#ଅନ୍ତର୍ଭାଵ <cmath>

ନାମାକାଶ matrix
{

template<typename Type, size_t M, size_t N>
ଶ୍ରେଣୀ Matrix;

template<typename Type, size_t M>
ଶ୍ରେଣୀ Vector;

template <typename MatrixT, typename Type, size_t P, size_t Q, size_t M, size_t N>
ଶ୍ରେଣୀ SliceT
{
ସର୍ବସାଧାରଣ:
	using Self = SliceT<MatrixT, Type, P, Q, M, N>;

	SliceT(size_t x0, size_t y0, MatrixT *data) :
		_x0(x0),
		_y0(y0),
		_data(data)
	{
		static_assert(P <= M, "Slice rows bigger than backing matrix");
		static_assert(Q <= N, "Slice cols bigger than backing matrix");
		ପ୍ରତିଜ୍ଞା(x0 + P <= M);
		ପ୍ରତିଜ୍ଞା(y0 + Q <= N);
	}

	SliceT(ଅଚଳ Self &other) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଅଚଳ Type &operator()(size_t i, size_t j) ଅଚଳ
	{
		ପ୍ରତିଜ୍ଞା(i < P);
		ପ୍ରତିଜ୍ଞା(j < Q);

		ଫେରନ୍ତୁ (*_data)(_x0 + i, _y0 + j);
	}

	Type &operator()(size_t i, size_t j)
	{
		ପ୍ରତିଜ୍ଞା(i < P);
		ପ୍ରତିଜ୍ଞା(j < Q);

		ଫେରନ୍ତୁ (*_data)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &operator=(ଅଚଳ Self &other)
	{
		// Protect against self-assignment
		ଯଦି (ଏହି == &other) {
			ଫେରନ୍ତୁ *ଏହି;
		}

		ଫେରନ୍ତୁ ଏହି->operator=<M, N>(other);
	}

	template<size_t MM, size_t NN>
	Self &operator=(ଅଚଳ SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ଫେରନ୍ତୁ self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ଅଚଳ SliceT<ଅଚଳ Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ଫେରନ୍ତୁ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ଅଚଳ Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ଫେରନ୍ତୁ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ଅଚଳ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		ଫେରନ୍ତୁ self;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator-(ଅଚଳ SliceT<ଅଚଳ Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		ଫେରନ୍ତୁ Matrix<Type, P, Q> {*ଏହି} - other;
	}


	Matrix<Type, P, Q> operator-(ଅଚଳ Matrix<Type, P, Q> &other)
	{
		ଫେରନ୍ତୁ Matrix<Type, P, Q> {*ଏହି} - other;
	}

	Matrix<Type, P, Q> operator-(ଅଚଳ Type &other)
	{
		ଫେରନ୍ତୁ Matrix<Type, P, Q> {*ଏହି} - other;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator+(ଅଚଳ SliceT<ଅଚଳ Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		ଫେରନ୍ତୁ Matrix<Type, P, Q> {*ଏହି} + other;
	}

	Matrix<Type, P, Q> operator+(ଅଚଳ Matrix<Type, P, Q> &other)
	{
		ଫେରନ୍ତୁ Matrix<Type, P, Q> {*ଏହି} + other;
	}

	Matrix<Type, P, Q> operator+(ଅଚଳ Type &other)
	{
		ଫେରନ୍ତୁ Matrix<Type, P, Q> {*ଏହି} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	template <size_t DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &operator=(ଅଚଳ Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		ଫେରନ୍ତୁ self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ଅଚଳ SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		ଫେରନ୍ତୁ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ଅଚଳ Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		ଫେରନ୍ତୁ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ଅଚଳ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		ଫେରନ୍ତୁ self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ଅଚଳ SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		ଫେରନ୍ତୁ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ଅଚଳ Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		ଫେରନ୍ତୁ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ଅଚଳ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		ଫେରନ୍ତୁ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator*=(ଅଚଳ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		ଫେରନ୍ତୁ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator/=(ଅଚଳ Type &scalar)
	{
		ଫେରନ୍ତୁ operator*=(Type(1) / scalar);
	}

	Matrix<Type, P, Q> operator*(ଅଚଳ Type &scalar) ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix<Type, P, Q> {*ଏହି} * scalar;
	}

	Matrix<Type, P, Q> operator/(ଅଚଳ Type &scalar) ଅଚଳ
	{
		ଫେରନ୍ତୁ (*ଏହି) * (1 / scalar);
	}

	template<size_t R, size_t S>
	ଅଚଳ SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0) ଅଚଳ
	{
		ଫେରନ୍ତୁ SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	template<size_t R, size_t S>
	SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0)
	{
		ଫେରନ୍ତୁ SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	ଶୂନ୍ୟ copyTo(Type dst[P * Q]) ଅଚଳ
	{
		ଅଚଳ SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	ଶୂନ୍ୟ copyToColumnMajor(Type dst[P * Q]) ଅଚଳ
	{
		ଅଚଳ SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	Vector < Type, P < Q ? P : Q > diag() ଅଚଳ
	{
		ଅଚଳ SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;
		Vector < Type, P < Q ? P : Q > res;

		ପାଇଁ (size_t j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		ଫେରନ୍ତୁ res;
	}

	template<size_t MM, size_t NN>
	Type dot(ଅଚଳ SliceT<MatrixT, Type, P, Q, MM, NN> &other) ଅଚଳ
	{
		ଅଚଳ SliceT<MatrixT, Type, P, Q, M, N> &self = *ଏହି;
		Type accum(0);

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		ଫେରନ୍ତୁ accum;
	}

	Type norm_squared() ଅଚଳ
	{
		ଫେରନ୍ତୁ dot(*ଏହି);
	}

	Type norm() ଅଚଳ
	{
		ଫେରନ୍ତୁ std::sqrt(norm_squared());
	}

	ତାର୍କିକ longerThan(Type testVal) ଅଚଳ
	{
		ଫେରନ୍ତୁ norm_squared() > testVal * testVal;
	}

	Type max() ଅଚଳ
	{
		Type max_val = (*ଏହି)(0, 0);

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				Type val = (*ଏହି)(i, j);

				ଯଦି (val > max_val) {
					max_val = val;
				}
			}
		}

		ଫେରନ୍ତୁ max_val;
	}

	Type min() ଅଚଳ
	{
		Type min_val = (*ଏହି)(0, 0);

		ପାଇଁ (size_t i = 0; i < P; i++) {
			ପାଇଁ (size_t j = 0; j < Q; j++) {
				Type val = (*ଏହି)(i, j);

				ଯଦି (val < min_val) {
					min_val = val;
				}
			}
		}

		ଫେରନ୍ତୁ min_val;
	}

ବ୍ୟକ୍ତିଗତ:
	size_t _x0, _y0;
	MatrixT *_data;
};

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using ConstSlice = SliceT<ଅଚଳ Matrix<Type, M, N>, Type, P, Q, M, N>;

}
