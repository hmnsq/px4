/**
 * @file Slice.hpp
 *
 * A simple matrix template library.
 *
 * @author Julian Kent < julian@auterion.com >
 */

#pragma once

#شامل <cassert>
#شامل <cstdio>
#شامل <cmath>

فضاینام matrix
{

template<typename Type, size_t M, size_t N>
رده Matrix;

template<typename Type, size_t M>
رده Vector;

template <typename MatrixT, typename Type, size_t P, size_t Q, size_t M, size_t N>
رده SliceT
{
عمومی:
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

	SliceT(ثابت Self &other) = پیشفرض;

	ثابت Type &operator()(size_t i, size_t j) ثابت
	{
		أكد(i < P);
		أكد(j < Q);

		بازگشت (*_data)(_x0 + i, _y0 + j);
	}

	Type &operator()(size_t i, size_t j)
	{
		أكد(i < P);
		أكد(j < Q);

		بازگشت (*_data)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &operator=(ثابت Self &other)
	{
		// Protect against self-assignment
		اگر (این == &other) {
			بازگشت *این;
		}

		بازگشت این->operator=<M, N>(other);
	}

	template<size_t MM, size_t NN>
	Self &operator=(ثابت SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		بازگشت self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ثابت SliceT<ثابت Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		بازگشت self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ثابت Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		بازگشت self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ثابت Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		بازگشت self;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator-(ثابت SliceT<ثابت Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		بازگشت Matrix<Type, P, Q> {*این} - other;
	}


	Matrix<Type, P, Q> operator-(ثابت Matrix<Type, P, Q> &other)
	{
		بازگشت Matrix<Type, P, Q> {*این} - other;
	}

	Matrix<Type, P, Q> operator-(ثابت Type &other)
	{
		بازگشت Matrix<Type, P, Q> {*این} - other;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator+(ثابت SliceT<ثابت Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		بازگشت Matrix<Type, P, Q> {*این} + other;
	}

	Matrix<Type, P, Q> operator+(ثابت Matrix<Type, P, Q> &other)
	{
		بازگشت Matrix<Type, P, Q> {*این} + other;
	}

	Matrix<Type, P, Q> operator+(ثابت Type &other)
	{
		بازگشت Matrix<Type, P, Q> {*این} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	template <size_t DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &operator=(ثابت Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *این;

		برای (size_t j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		بازگشت self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ثابت SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		بازگشت self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ثابت Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		بازگشت self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ثابت Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		بازگشت self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ثابت SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		بازگشت self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ثابت Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		بازگشت self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ثابت Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		بازگشت self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator*=(ثابت Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		بازگشت self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator/=(ثابت Type &scalar)
	{
		بازگشت operator*=(Type(1) / scalar);
	}

	Matrix<Type, P, Q> operator*(ثابت Type &scalar) ثابت
	{
		بازگشت Matrix<Type, P, Q> {*این} * scalar;
	}

	Matrix<Type, P, Q> operator/(ثابت Type &scalar) ثابت
	{
		بازگشت (*این) * (1 / scalar);
	}

	template<size_t R, size_t S>
	ثابت SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0) ثابت
	{
		بازگشت SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	template<size_t R, size_t S>
	SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0)
	{
		بازگشت SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	تهی copyTo(Type dst[P * Q]) ثابت
	{
		ثابت SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	تهی copyToColumnMajor(Type dst[P * Q]) ثابت
	{
		ثابت SliceT<MatrixT, Type, P, Q, M, N> &self = *این;

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	Vector < Type, P < Q ? P : Q > diag() ثابت
	{
		ثابت SliceT<MatrixT, Type, P, Q, M, N> &self = *این;
		Vector < Type, P < Q ? P : Q > res;

		برای (size_t j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		بازگشت res;
	}

	template<size_t MM, size_t NN>
	Type dot(ثابت SliceT<MatrixT, Type, P, Q, MM, NN> &other) ثابت
	{
		ثابت SliceT<MatrixT, Type, P, Q, M, N> &self = *این;
		Type accum(0);

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		بازگشت accum;
	}

	Type norm_squared() ثابت
	{
		بازگشت dot(*این);
	}

	Type norm() ثابت
	{
		بازگشت std::sqrt(norm_squared());
	}

	منطقی longerThan(Type testVal) ثابت
	{
		بازگشت norm_squared() > testVal * testVal;
	}

	Type max() ثابت
	{
		Type max_val = (*این)(0, 0);

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				Type val = (*این)(i, j);

				اگر (val > max_val) {
					max_val = val;
				}
			}
		}

		بازگشت max_val;
	}

	Type min() ثابت
	{
		Type min_val = (*این)(0, 0);

		برای (size_t i = 0; i < P; i++) {
			برای (size_t j = 0; j < Q; j++) {
				Type val = (*این)(i, j);

				اگر (val < min_val) {
					min_val = val;
				}
			}
		}

		بازگشت min_val;
	}

خصوصی:
	size_t _x0, _y0;
	MatrixT *_data;
};

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using ConstSlice = SliceT<ثابت Matrix<Type, M, N>, Type, P, Q, M, N>;

}
