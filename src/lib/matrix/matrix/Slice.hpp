/**
 * @file Slice.hpp
 *
 * A simple matrix template library.
 *
 * @author Julian Kent < julian@auterion.com >
 */

#pragma once

#ಅನ್ತರ್ಭಾವ <cassert>
#ಅನ್ತರ್ಭಾವ <cstdio>
#ಅನ್ತರ್ಭಾವ <cmath>

ನಾಮಾಕಾಶ matrix
{

template<typename Type, size_t M, size_t N>
ವರ್ಗ Matrix;

template<typename Type, size_t M>
ವರ್ಗ Vector;

template <typename MatrixT, typename Type, size_t P, size_t Q, size_t M, size_t N>
ವರ್ಗ SliceT
{
ಸಾರ್ವಜನಿಕ:
	using Self = SliceT<MatrixT, Type, P, Q, M, N>;

	SliceT(size_t x0, size_t y0, MatrixT *data) :
		_x0(x0),
		_y0(y0),
		_data(data)
	{
		static_assert(P <= M, "Slice rows bigger than backing matrix");
		static_assert(Q <= N, "Slice cols bigger than backing matrix");
		ಪ್ರತಿಜ್ಞಾ(x0 + P <= M);
		ಪ್ರತಿಜ್ಞಾ(y0 + Q <= N);
	}

	SliceT(ಧ್ರುವ Self &other) = ಸ್ವಭಾವ;

	ಧ್ರುವ Type &operator()(size_t i, size_t j) ಧ್ರುವ
	{
		ಪ್ರತಿಜ್ಞಾ(i < P);
		ಪ್ರತಿಜ್ಞಾ(j < Q);

		ಪ್ರತ್ಯಾಗಮ (*_data)(_x0 + i, _y0 + j);
	}

	Type &operator()(size_t i, size_t j)
	{
		ಪ್ರತಿಜ್ಞಾ(i < P);
		ಪ್ರತಿಜ್ಞಾ(j < Q);

		ಪ್ರತ್ಯಾಗಮ (*_data)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &operator=(ಧ್ರುವ Self &other)
	{
		// Protect against self-assignment
		ಯದಿ (ಏತತ್ == &other) {
			ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
		}

		ಪ್ರತ್ಯಾಗಮ ಏತತ್->operator=<M, N>(other);
	}

	template<size_t MM, size_t NN>
	Self &operator=(ಧ್ರುವ SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ಧ್ರುವ SliceT<ಧ್ರುವ Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ಧ್ರುವ Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ಧ್ರುವ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator-(ಧ್ರುವ SliceT<ಧ್ರುವ Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		ಪ್ರತ್ಯಾಗಮ Matrix<Type, P, Q> {*ಏತತ್} - other;
	}


	Matrix<Type, P, Q> operator-(ಧ್ರುವ Matrix<Type, P, Q> &other)
	{
		ಪ್ರತ್ಯಾಗಮ Matrix<Type, P, Q> {*ಏತತ್} - other;
	}

	Matrix<Type, P, Q> operator-(ಧ್ರುವ Type &other)
	{
		ಪ್ರತ್ಯಾಗಮ Matrix<Type, P, Q> {*ಏತತ್} - other;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator+(ಧ್ರುವ SliceT<ಧ್ರುವ Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		ಪ್ರತ್ಯಾಗಮ Matrix<Type, P, Q> {*ಏತತ್} + other;
	}

	Matrix<Type, P, Q> operator+(ಧ್ರುವ Matrix<Type, P, Q> &other)
	{
		ಪ್ರತ್ಯಾಗಮ Matrix<Type, P, Q> {*ಏತತ್} + other;
	}

	Matrix<Type, P, Q> operator+(ಧ್ರುವ Type &other)
	{
		ಪ್ರತ್ಯಾಗಮ Matrix<Type, P, Q> {*ಏತತ್} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	template <size_t DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &operator=(ಧ್ರುವ Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ಧ್ರುವ SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ಧ್ರುವ Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ಧ್ರುವ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ಧ್ರುವ SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ಧ್ರುವ Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ಧ್ರುವ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator*=(ಧ್ರುವ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		ಪ್ರತ್ಯಾಗಮ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator/=(ಧ್ರುವ Type &scalar)
	{
		ಪ್ರತ್ಯಾಗಮ operator*=(Type(1) / scalar);
	}

	Matrix<Type, P, Q> operator*(ಧ್ರುವ Type &scalar) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix<Type, P, Q> {*ಏತತ್} * scalar;
	}

	Matrix<Type, P, Q> operator/(ಧ್ರುವ Type &scalar) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್) * (1 / scalar);
	}

	template<size_t R, size_t S>
	ಧ್ರುವ SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	template<size_t R, size_t S>
	SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0)
	{
		ಪ್ರತ್ಯಾಗಮ SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	ಶೂನ್ಯ copyTo(Type dst[P * Q]) ಧ್ರುವ
	{
		ಧ್ರುವ SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	ಶೂನ್ಯ copyToColumnMajor(Type dst[P * Q]) ಧ್ರುವ
	{
		ಧ್ರುವ SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	Vector < Type, P < Q ? P : Q > diag() ಧ್ರುವ
	{
		ಧ್ರುವ SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;
		Vector < Type, P < Q ? P : Q > res;

		ಪ್ರತಿ (size_t j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		ಪ್ರತ್ಯಾಗಮ res;
	}

	template<size_t MM, size_t NN>
	Type dot(ಧ್ರುವ SliceT<MatrixT, Type, P, Q, MM, NN> &other) ಧ್ರುವ
	{
		ಧ್ರುವ SliceT<MatrixT, Type, P, Q, M, N> &self = *ಏತತ್;
		Type accum(0);

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		ಪ್ರತ್ಯಾಗಮ accum;
	}

	Type norm_squared() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ dot(*ಏತತ್);
	}

	Type norm() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ std::sqrt(norm_squared());
	}

	ತಾರ್ಕಿಕ longerThan(Type testVal) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ norm_squared() > testVal * testVal;
	}

	Type max() ಧ್ರುವ
	{
		Type max_val = (*ಏತತ್)(0, 0);

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				Type val = (*ಏತತ್)(i, j);

				ಯದಿ (val > max_val) {
					max_val = val;
				}
			}
		}

		ಪ್ರತ್ಯಾಗಮ max_val;
	}

	Type min() ಧ್ರುವ
	{
		Type min_val = (*ಏತತ್)(0, 0);

		ಪ್ರತಿ (size_t i = 0; i < P; i++) {
			ಪ್ರತಿ (size_t j = 0; j < Q; j++) {
				Type val = (*ಏತತ್)(i, j);

				ಯದಿ (val < min_val) {
					min_val = val;
				}
			}
		}

		ಪ್ರತ್ಯಾಗಮ min_val;
	}

ಗುಪ್ತ:
	size_t _x0, _y0;
	MatrixT *_data;
};

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using ConstSlice = SliceT<ಧ್ರುವ Matrix<Type, M, N>, Type, P, Q, M, N>;

}
