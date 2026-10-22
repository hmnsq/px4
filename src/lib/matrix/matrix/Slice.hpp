/**
 * @file Slice.hpp
 *
 * A simple matrix template library.
 *
 * @author Julian Kent < julian@auterion.com >
 */

#pragma once

#ضمن <cassert>
#ضمن <cstdio>
#ضمن <cmath>

نطاق matrix
{

template<typename Type, size_t M, size_t N>
מחלקה Matrix;

template<typename Type, size_t M>
מחלקה Vector;

template <typename MatrixT, typename Type, size_t P, size_t Q, size_t M, size_t N>
מחלקה SliceT
{
عام:
	using Self = SliceT<MatrixT, Type, P, Q, M, N>;

	SliceT(size_t x0, size_t y0, MatrixT *data) :
		_x0(x0),
		_y0(y0),
		_data(data)
	{
		static_assert(P <= M, "Slice rows bigger than backing matrix");
		static_assert(Q <= N, "Slice cols bigger than backing matrix");
		أكد(x0 + P <= M);
		أكد(y0 + Q <= N);
	}

	SliceT(ثابت Self &other) = افتراضي;

	ثابت Type &operator()(size_t i, size_t j) ثابت
	{
		أكد(i < P);
		أكد(j < Q);

		החזר (*_data)(_x0 + i, _y0 + j);
	}

	Type &operator()(size_t i, size_t j)
	{
		أكد(i < P);
		أكد(j < Q);

		החזר (*_data)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &operator=(ثابت Self &other)
	{
		// Protect against self-assignment
		אם (هذا == &other) {
			החזר *هذا;
		}

		החזר هذا->operator=<M, N>(other);
	}

	template<size_t MM, size_t NN>
	Self &operator=(ثابت SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		החזר self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ثابت SliceT<ثابت Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		החזר self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ثابت Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		החזר self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ثابت Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		החזר self;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator-(ثابت SliceT<ثابت Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		החזר Matrix<Type, P, Q> {*هذا} - other;
	}


	Matrix<Type, P, Q> operator-(ثابت Matrix<Type, P, Q> &other)
	{
		החזר Matrix<Type, P, Q> {*هذا} - other;
	}

	Matrix<Type, P, Q> operator-(ثابت Type &other)
	{
		החזר Matrix<Type, P, Q> {*هذا} - other;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator+(ثابت SliceT<ثابت Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		החזר Matrix<Type, P, Q> {*هذا} + other;
	}

	Matrix<Type, P, Q> operator+(ثابت Matrix<Type, P, Q> &other)
	{
		החזר Matrix<Type, P, Q> {*هذا} + other;
	}

	Matrix<Type, P, Q> operator+(ثابت Type &other)
	{
		החזר Matrix<Type, P, Q> {*هذا} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	template <size_t DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &operator=(ثابت Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *هذا;

		עבור (size_t j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		החזר self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ثابت SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		החזר self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ثابت Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		החזר self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ثابت Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		החזר self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ثابت SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		החזר self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ثابت Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		החזר self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ثابت Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		החזר self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator*=(ثابت Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		החזר self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator/=(ثابت Type &scalar)
	{
		החזר operator*=(Type(1) / scalar);
	}

	Matrix<Type, P, Q> operator*(ثابت Type &scalar) ثابت
	{
		החזר Matrix<Type, P, Q> {*هذا} * scalar;
	}

	Matrix<Type, P, Q> operator/(ثابت Type &scalar) ثابت
	{
		החזר (*هذا) * (1 / scalar);
	}

	template<size_t R, size_t S>
	ثابت SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0) ثابت
	{
		החזר SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	template<size_t R, size_t S>
	SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0)
	{
		החזר SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	فراغ copyTo(Type dst[P * Q]) ثابت
	{
		ثابت SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	فراغ copyToColumnMajor(Type dst[P * Q]) ثابت
	{
		ثابت SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	Vector < Type, P < Q ? P : Q > diag() ثابت
	{
		ثابت SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;
		Vector < Type, P < Q ? P : Q > res;

		עבור (size_t j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		החזר res;
	}

	template<size_t MM, size_t NN>
	Type dot(ثابت SliceT<MatrixT, Type, P, Q, MM, NN> &other) ثابت
	{
		ثابت SliceT<MatrixT, Type, P, Q, M, N> &self = *هذا;
		Type accum(0);

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		החזר accum;
	}

	Type norm_squared() ثابت
	{
		החזר dot(*هذا);
	}

	Type norm() ثابت
	{
		החזר std::sqrt(norm_squared());
	}

	منطقي longerThan(Type testVal) ثابت
	{
		החזר norm_squared() > testVal * testVal;
	}

	Type max() ثابت
	{
		Type max_val = (*هذا)(0, 0);

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				Type val = (*هذا)(i, j);

				אם (val > max_val) {
					max_val = val;
				}
			}
		}

		החזר max_val;
	}

	Type min() ثابت
	{
		Type min_val = (*هذا)(0, 0);

		עבור (size_t i = 0; i < P; i++) {
			עבור (size_t j = 0; j < Q; j++) {
				Type val = (*هذا)(i, j);

				אם (val < min_val) {
					min_val = val;
				}
			}
		}

		החזר min_val;
	}

خاص:
	size_t _x0, _y0;
	MatrixT *_data;
};

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using ConstSlice = SliceT<ثابت Matrix<Type, M, N>, Type, P, Q, M, N>;

}
