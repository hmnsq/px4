/**
 * @file Slice.hpp
 *
 * A simple matrix template library.
 *
 * @author Julian Kent < julian@auterion.com >
 */

#pragma once

#அந்தர்பாவ <cassert>
#அந்தர்பாவ <cstdio>
#அந்தர்பாவ <cmath>

நாமாகாஶ matrix
{

template<typename Type, size_t M, size_t N>
வகுப்பு Matrix;

template<typename Type, size_t M>
வகுப்பு Vector;

template <typename MatrixT, typename Type, size_t P, size_t Q, size_t M, size_t N>
வகுப்பு SliceT
{
பொது:
	using Self = SliceT<MatrixT, Type, P, Q, M, N>;

	SliceT(size_t x0, size_t y0, MatrixT *data) :
		_x0(x0),
		_y0(y0),
		_data(data)
	{
		static_assert(P <= M, "Slice rows bigger than backing matrix");
		static_assert(Q <= N, "Slice cols bigger than backing matrix");
		ப்ரதிஜ்ஞா(x0 + P <= M);
		ப்ரதிஜ்ஞா(y0 + Q <= N);
	}

	SliceT(மாறிலி Self &other) = இயல்பு;

	மாறிலி Type &operator()(size_t i, size_t j) மாறிலி
	{
		ப்ரதிஜ்ஞா(i < P);
		ப்ரதிஜ்ஞா(j < Q);

		திரும்பு (*_data)(_x0 + i, _y0 + j);
	}

	Type &operator()(size_t i, size_t j)
	{
		ப்ரதிஜ்ஞா(i < P);
		ப்ரதிஜ்ஞா(j < Q);

		திரும்பு (*_data)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &operator=(மாறிலி Self &other)
	{
		// Protect against self-assignment
		எனில் (இது == &other) {
			திரும்பு *இது;
		}

		திரும்பு இது->operator=<M, N>(other);
	}

	template<size_t MM, size_t NN>
	Self &operator=(மாறிலி SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		திரும்பு self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(மாறிலி SliceT<மாறிலி Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		திரும்பு self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(மாறிலி Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		திரும்பு self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(மாறிலி Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		திரும்பு self;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator-(மாறிலி SliceT<மாறிலி Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		திரும்பு Matrix<Type, P, Q> {*இது} - other;
	}


	Matrix<Type, P, Q> operator-(மாறிலி Matrix<Type, P, Q> &other)
	{
		திரும்பு Matrix<Type, P, Q> {*இது} - other;
	}

	Matrix<Type, P, Q> operator-(மாறிலி Type &other)
	{
		திரும்பு Matrix<Type, P, Q> {*இது} - other;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator+(மாறிலி SliceT<மாறிலி Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		திரும்பு Matrix<Type, P, Q> {*இது} + other;
	}

	Matrix<Type, P, Q> operator+(மாறிலி Matrix<Type, P, Q> &other)
	{
		திரும்பு Matrix<Type, P, Q> {*இது} + other;
	}

	Matrix<Type, P, Q> operator+(மாறிலி Type &other)
	{
		திரும்பு Matrix<Type, P, Q> {*இது} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	template <size_t DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &operator=(மாறிலி Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *இது;

		க்கு (size_t j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		திரும்பு self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(மாறிலி SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		திரும்பு self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(மாறிலி Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		திரும்பு self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(மாறிலி Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		திரும்பு self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(மாறிலி SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		திரும்பு self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(மாறிலி Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		திரும்பு self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(மாறிலி Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		திரும்பு self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator*=(மாறிலி Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		திரும்பு self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator/=(மாறிலி Type &scalar)
	{
		திரும்பு operator*=(Type(1) / scalar);
	}

	Matrix<Type, P, Q> operator*(மாறிலி Type &scalar) மாறிலி
	{
		திரும்பு Matrix<Type, P, Q> {*இது} * scalar;
	}

	Matrix<Type, P, Q> operator/(மாறிலி Type &scalar) மாறிலி
	{
		திரும்பு (*இது) * (1 / scalar);
	}

	template<size_t R, size_t S>
	மாறிலி SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0) மாறிலி
	{
		திரும்பு SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	template<size_t R, size_t S>
	SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0)
	{
		திரும்பு SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	வெறுமை copyTo(Type dst[P * Q]) மாறிலி
	{
		மாறிலி SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	வெறுமை copyToColumnMajor(Type dst[P * Q]) மாறிலி
	{
		மாறிலி SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	Vector < Type, P < Q ? P : Q > diag() மாறிலி
	{
		மாறிலி SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;
		Vector < Type, P < Q ? P : Q > res;

		க்கு (size_t j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		திரும்பு res;
	}

	template<size_t MM, size_t NN>
	Type dot(மாறிலி SliceT<MatrixT, Type, P, Q, MM, NN> &other) மாறிலி
	{
		மாறிலி SliceT<MatrixT, Type, P, Q, M, N> &self = *இது;
		Type accum(0);

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		திரும்பு accum;
	}

	Type norm_squared() மாறிலி
	{
		திரும்பு dot(*இது);
	}

	Type norm() மாறிலி
	{
		திரும்பு std::sqrt(norm_squared());
	}

	தருக்க longerThan(Type testVal) மாறிலி
	{
		திரும்பு norm_squared() > testVal * testVal;
	}

	Type max() மாறிலி
	{
		Type max_val = (*இது)(0, 0);

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
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

		க்கு (size_t i = 0; i < P; i++) {
			க்கு (size_t j = 0; j < Q; j++) {
				Type val = (*இது)(i, j);

				எனில் (val < min_val) {
					min_val = val;
				}
			}
		}

		திரும்பு min_val;
	}

தனிப்பட்ட:
	size_t _x0, _y0;
	MatrixT *_data;
};

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using ConstSlice = SliceT<மாறிலி Matrix<Type, M, N>, Type, P, Q, M, N>;

}
