/**
 * @file Slice.hpp
 *
 * A simple matrix template library.
 *
 * @author Julian Kent < julian@auterion.com >
 */

#pragma once

#অন্তর্ভাৱ <cassert>
#অন্তর্ভাৱ <cstdio>
#অন্তর্ভাৱ <cmath>

নামাকাশ matrix
{

template<typename Type, size_t M, size_t N>
শ্ৰেণী Matrix;

template<typename Type, size_t M>
শ্ৰেণী Vector;

template <typename MatrixT, typename Type, size_t P, size_t Q, size_t M, size_t N>
শ্ৰেণী SliceT
{
সার্ৱজনিক:
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

	SliceT(ধ্রুৱ Self &other) = স্ৱভাৱ;

	ধ্রুৱ Type &operator()(size_t i, size_t j) ধ্রুৱ
	{
		প্রতিজ্ঞা(i < P);
		প্রতিজ্ঞা(j < Q);

		ঘূৰাই (*_data)(_x0 + i, _y0 + j);
	}

	Type &operator()(size_t i, size_t j)
	{
		প্রতিজ্ঞা(i < P);
		প্রতিজ্ঞা(j < Q);

		ঘূৰাই (*_data)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &operator=(ধ্রুৱ Self &other)
	{
		// Protect against self-assignment
		যদি (এতত্ == &other) {
			ঘূৰাই *এতত্;
		}

		ঘূৰাই এতত্->operator=<M, N>(other);
	}

	template<size_t MM, size_t NN>
	Self &operator=(ধ্রুৱ SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ঘূৰাই self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ধ্রুৱ SliceT<ধ্রুৱ Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ঘূৰাই self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ধ্রুৱ Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ঘূৰাই self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(ধ্রুৱ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		ঘূৰাই self;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator-(ধ্রুৱ SliceT<ধ্রুৱ Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		ঘূৰাই Matrix<Type, P, Q> {*এতত্} - other;
	}


	Matrix<Type, P, Q> operator-(ধ্রুৱ Matrix<Type, P, Q> &other)
	{
		ঘূৰাই Matrix<Type, P, Q> {*এতত্} - other;
	}

	Matrix<Type, P, Q> operator-(ধ্রুৱ Type &other)
	{
		ঘূৰাই Matrix<Type, P, Q> {*এতত্} - other;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator+(ধ্রুৱ SliceT<ধ্রুৱ Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		ঘূৰাই Matrix<Type, P, Q> {*এতত্} + other;
	}

	Matrix<Type, P, Q> operator+(ধ্রুৱ Matrix<Type, P, Q> &other)
	{
		ঘূৰাই Matrix<Type, P, Q> {*এতত্} + other;
	}

	Matrix<Type, P, Q> operator+(ধ্রুৱ Type &other)
	{
		ঘূৰাই Matrix<Type, P, Q> {*এতত্} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	template <size_t DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &operator=(ধ্রুৱ Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *এতত্;

		বাবে (size_t j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		ঘূৰাই self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ধ্রুৱ SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		ঘূৰাই self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ধ্রুৱ Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		ঘূৰাই self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(ধ্রুৱ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		ঘূৰাই self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ধ্রুৱ SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		ঘূৰাই self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ধ্রুৱ Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		ঘূৰাই self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(ধ্রুৱ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		ঘূৰাই self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator*=(ধ্রুৱ Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		ঘূৰাই self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator/=(ধ্রুৱ Type &scalar)
	{
		ঘূৰাই operator*=(Type(1) / scalar);
	}

	Matrix<Type, P, Q> operator*(ধ্রুৱ Type &scalar) ধ্রুৱ
	{
		ঘূৰাই Matrix<Type, P, Q> {*এতত্} * scalar;
	}

	Matrix<Type, P, Q> operator/(ধ্রুৱ Type &scalar) ধ্রুৱ
	{
		ঘূৰাই (*এতত্) * (1 / scalar);
	}

	template<size_t R, size_t S>
	ধ্রুৱ SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0) ধ্রুৱ
	{
		ঘূৰাই SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	template<size_t R, size_t S>
	SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0)
	{
		ঘূৰাই SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	শূন্য copyTo(Type dst[P * Q]) ধ্রুৱ
	{
		ধ্রুৱ SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	শূন্য copyToColumnMajor(Type dst[P * Q]) ধ্রুৱ
	{
		ধ্রুৱ SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	Vector < Type, P < Q ? P : Q > diag() ধ্রুৱ
	{
		ধ্রুৱ SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;
		Vector < Type, P < Q ? P : Q > res;

		বাবে (size_t j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		ঘূৰাই res;
	}

	template<size_t MM, size_t NN>
	Type dot(ধ্রুৱ SliceT<MatrixT, Type, P, Q, MM, NN> &other) ধ্রুৱ
	{
		ধ্রুৱ SliceT<MatrixT, Type, P, Q, M, N> &self = *এতত্;
		Type accum(0);

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		ঘূৰাই accum;
	}

	Type norm_squared() ধ্রুৱ
	{
		ঘূৰাই dot(*এতত্);
	}

	Type norm() ধ্রুৱ
	{
		ঘূৰাই std::sqrt(norm_squared());
	}

	তার্কিক longerThan(Type testVal) ধ্রুৱ
	{
		ঘূৰাই norm_squared() > testVal * testVal;
	}

	Type max() ধ্রুৱ
	{
		Type max_val = (*এতত্)(0, 0);

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				Type val = (*এতত্)(i, j);

				যদি (val > max_val) {
					max_val = val;
				}
			}
		}

		ঘূৰাই max_val;
	}

	Type min() ধ্রুৱ
	{
		Type min_val = (*এতত্)(0, 0);

		বাবে (size_t i = 0; i < P; i++) {
			বাবে (size_t j = 0; j < Q; j++) {
				Type val = (*এতত্)(i, j);

				যদি (val < min_val) {
					min_val = val;
				}
			}
		}

		ঘূৰাই min_val;
	}

গুপ্ত:
	size_t _x0, _y0;
	MatrixT *_data;
};

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using ConstSlice = SliceT<ধ্রুৱ Matrix<Type, M, N>, Type, P, Q, M, N>;

}
