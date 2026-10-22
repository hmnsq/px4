/**
 * @file Slice.hpp
 *
 * A simple matrix template library.
 *
 * @author Julian Kent < julian@auterion.com >
 */

#pragma once

#അന്തര്ഭാവ <cassert>
#അന്തര്ഭാവ <cstdio>
#അന്തര്ഭാവ <cmath>

നാമാകാശ matrix
{

template<typename Type, size_t M, size_t N>
വർഗ്ഗം Matrix;

template<typename Type, size_t M>
വർഗ്ഗം Vector;

template <typename MatrixT, typename Type, size_t P, size_t Q, size_t M, size_t N>
വർഗ്ഗം SliceT
{
പൊതു:
	using Self = SliceT<MatrixT, Type, P, Q, M, N>;

	SliceT(size_t x0, size_t y0, MatrixT *data) :
		_x0(x0),
		_y0(y0),
		_data(data)
	{
		static_assert(P <= M, "Slice rows bigger than backing matrix");
		static_assert(Q <= N, "Slice cols bigger than backing matrix");
		പ്രതിജ്ഞാ(x0 + P <= M);
		പ്രതിജ്ഞാ(y0 + Q <= N);
	}

	SliceT(സ്ഥിരം Self &other) = സ്ഥിരസ്ഥിതി;

	സ്ഥിരം Type &operator()(size_t i, size_t j) സ്ഥിരം
	{
		പ്രതിജ്ഞാ(i < P);
		പ്രതിജ്ഞാ(j < Q);

		തിരികെ (*_data)(_x0 + i, _y0 + j);
	}

	Type &operator()(size_t i, size_t j)
	{
		പ്രതിജ്ഞാ(i < P);
		പ്രതിജ്ഞാ(j < Q);

		തിരികെ (*_data)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &operator=(സ്ഥിരം Self &other)
	{
		// Protect against self-assignment
		എങ്കിൽ (ഇത് == &other) {
			തിരികെ *ഇത്;
		}

		തിരികെ ഇത്->operator=<M, N>(other);
	}

	template<size_t MM, size_t NN>
	Self &operator=(സ്ഥിരം SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		തിരികെ self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(സ്ഥിരം SliceT<സ്ഥിരം Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		തിരികെ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(സ്ഥിരം Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		തിരികെ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator=(സ്ഥിരം Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		തിരികെ self;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator-(സ്ഥിരം SliceT<സ്ഥിരം Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		തിരികെ Matrix<Type, P, Q> {*ഇത്} - other;
	}


	Matrix<Type, P, Q> operator-(സ്ഥിരം Matrix<Type, P, Q> &other)
	{
		തിരികെ Matrix<Type, P, Q> {*ഇത്} - other;
	}

	Matrix<Type, P, Q> operator-(സ്ഥിരം Type &other)
	{
		തിരികെ Matrix<Type, P, Q> {*ഇത്} - other;
	}

	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator+(സ്ഥിരം SliceT<സ്ഥിരം Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		തിരികെ Matrix<Type, P, Q> {*ഇത്} + other;
	}

	Matrix<Type, P, Q> operator+(സ്ഥിരം Matrix<Type, P, Q> &other)
	{
		തിരികെ Matrix<Type, P, Q> {*ഇത്} + other;
	}

	Matrix<Type, P, Q> operator+(സ്ഥിരം Type &other)
	{
		തിരികെ Matrix<Type, P, Q> {*ഇത്} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	template <size_t DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &operator=(സ്ഥിരം Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		തിരികെ self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(സ്ഥിരം SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		തിരികെ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(സ്ഥിരം Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		തിരികെ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(സ്ഥിരം Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		തിരികെ self;
	}

	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(സ്ഥിരം SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		തിരികെ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(സ്ഥിരം Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		തിരികെ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(സ്ഥിരം Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		തിരികെ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator*=(സ്ഥിരം Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		തിരികെ self;
	}

	SliceT<MatrixT, Type, P, Q, M, N> &operator/=(സ്ഥിരം Type &scalar)
	{
		തിരികെ operator*=(Type(1) / scalar);
	}

	Matrix<Type, P, Q> operator*(സ്ഥിരം Type &scalar) സ്ഥിരം
	{
		തിരികെ Matrix<Type, P, Q> {*ഇത്} * scalar;
	}

	Matrix<Type, P, Q> operator/(സ്ഥിരം Type &scalar) സ്ഥിരം
	{
		തിരികെ (*ഇത്) * (1 / scalar);
	}

	template<size_t R, size_t S>
	സ്ഥിരം SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0) സ്ഥിരം
	{
		തിരികെ SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	template<size_t R, size_t S>
	SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0)
	{
		തിരികെ SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	ശൂന്യം copyTo(Type dst[P * Q]) സ്ഥിരം
	{
		സ്ഥിരം SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	ശൂന്യം copyToColumnMajor(Type dst[P * Q]) സ്ഥിരം
	{
		സ്ഥിരം SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	Vector < Type, P < Q ? P : Q > diag() സ്ഥിരം
	{
		സ്ഥിരം SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;
		Vector < Type, P < Q ? P : Q > res;

		വേണ്ടി (size_t j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		തിരികെ res;
	}

	template<size_t MM, size_t NN>
	Type dot(സ്ഥിരം SliceT<MatrixT, Type, P, Q, MM, NN> &other) സ്ഥിരം
	{
		സ്ഥിരം SliceT<MatrixT, Type, P, Q, M, N> &self = *ഇത്;
		Type accum(0);

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		തിരികെ accum;
	}

	Type norm_squared() സ്ഥിരം
	{
		തിരികെ dot(*ഇത്);
	}

	Type norm() സ്ഥിരം
	{
		തിരികെ std::sqrt(norm_squared());
	}

	തർക്കം longerThan(Type testVal) സ്ഥിരം
	{
		തിരികെ norm_squared() > testVal * testVal;
	}

	Type max() സ്ഥിരം
	{
		Type max_val = (*ഇത്)(0, 0);

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				Type val = (*ഇത്)(i, j);

				എങ്കിൽ (val > max_val) {
					max_val = val;
				}
			}
		}

		തിരികെ max_val;
	}

	Type min() സ്ഥിരം
	{
		Type min_val = (*ഇത്)(0, 0);

		വേണ്ടി (size_t i = 0; i < P; i++) {
			വേണ്ടി (size_t j = 0; j < Q; j++) {
				Type val = (*ഇത്)(i, j);

				എങ്കിൽ (val < min_val) {
					min_val = val;
				}
			}
		}

		തിരികെ min_val;
	}

സ്വകാര്യ:
	size_t _x0, _y0;
	MatrixT *_data;
};

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using ConstSlice = SliceT<സ്ഥിരം Matrix<Type, M, N>, Type, P, Q, M, N>;

}
