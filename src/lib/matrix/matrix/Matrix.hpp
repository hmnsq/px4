/**
 * @file Matrix.hpp
 *
 * A simple matrix template library.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#അന്തര്ഭാവ <cmath>
#അന്തര്ഭാവ <cstdio>
#അന്തര്ഭാവ <cstring>

#അന്തര്ഭാവ "helper_functions.hpp"
#അന്തര്ഭാവ "Slice.hpp"

നാമാകാശ matrix
{

template<typename Type, size_t M, size_t N>
വർഗ്ഗം Matrix
{
	Type _data[M][N] {};

പൊതു:

	// Constructors
	Matrix() = സ്ഥിരസ്ഥിതി;

	explicit Matrix(സ്ഥിരം Type data_[M * N])
	{
		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[N * i + j];
			}
		}
	}

	explicit Matrix(സ്ഥിരം Type data_[M][N])
	{
		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[i][j];
			}
		}
	}

	Matrix(സ്ഥിരം Matrix &other)
	{
		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				_data[i][j] = other(i, j);
			}
		}
	}

	template<typename S>
	Matrix(സ്ഥിരം Matrix<S, M, N> &aa)
	{
		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				_data[i][j] = static_cast<Type>(aa(i, j));
			}
		}
	}

	template<size_t P, size_t Q>
	Matrix(സ്ഥിരം Slice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	template<size_t P, size_t Q>
	Matrix(സ്ഥിരം ConstSlice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	/**
	 * Accessors/ Assignment etc.
	 */


	വരിയിൽ സ്ഥിരം Type &operator()(size_t i, size_t j) സ്ഥിരം
	{
		പ്രതിജ്ഞാ(i < M);
		പ്രതിജ്ഞാ(j < N);

		തിരികെ _data[i][j];
	}

	വരിയിൽ Type &operator()(size_t i, size_t j)
	{
		പ്രതിജ്ഞാ(i < M);
		പ്രതിജ്ഞാ(j < N);

		തിരികെ _data[i][j];
	}

	Matrix<Type, M, N> &operator=(സ്ഥിരം Matrix<Type, M, N> &other)
	{
		എങ്കിൽ (ഇത് != &other) {
			Matrix<Type, M, N> &self = *ഇത്;

			വേണ്ടി (size_t i = 0; i < M; i++) {
				വേണ്ടി (size_t j = 0; j < N; j++) {
					self(i, j) = other(i, j);
				}
			}
		}

		തിരികെ (*ഇത്);
	}

	ശൂന്യം copyTo(Type dst[M * N]) സ്ഥിരം
	{
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				dst[N * i + j] = self(i, j);
			}
		}
	}

	ശൂന്യം copyToColumnMajor(Type dst[M * N]) സ്ഥിരം
	{
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
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
	Matrix<Type, M, P> operator*(സ്ഥിരം Matrix<Type, N, P> &other) സ്ഥിരം
	{
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;
		Matrix<Type, M, P> res{};

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t k = 0; k < P; k++) {
				വേണ്ടി (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(j, k);
				}
			}
		}

		തിരികെ res;
	}

	// Using this function reduces the number of temporary variables needed to compute A * B.T
	template<size_t P>
	Matrix<Type, M, M> multiplyByTranspose(സ്ഥിരം Matrix<Type, P, N> &other) സ്ഥിരം
	{
		Matrix<Type, M, P> res;
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t k = 0; k < P; k++) {
				വേണ്ടി (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(k, j);
				}
			}
		}

		തിരികെ res;
	}

	// Element-wise multiplication
	Matrix<Type, M, N> emult(സ്ഥിരം Matrix<Type, M, N> &other) സ്ഥിരം
	{
		Matrix<Type, M, N> res;
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * other(i, j);
			}
		}

		തിരികെ res;
	}

	// Element-wise division
	Matrix<Type, M, N> edivide(സ്ഥിരം Matrix<Type, M, N> &other) സ്ഥിരം
	{
		Matrix<Type, M, N> res;
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) / other(i, j);
			}
		}

		തിരികെ res;
	}

	Matrix<Type, M, N> operator+(സ്ഥിരം Matrix<Type, M, N> &other) സ്ഥിരം
	{
		Matrix<Type, M, N> res;
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + other(i, j);
			}
		}

		തിരികെ res;
	}

	Matrix<Type, M, N> operator-(സ്ഥിരം Matrix<Type, M, N> &other) സ്ഥിരം
	{
		Matrix<Type, M, N> res;
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) - other(i, j);
			}
		}

		തിരികെ res;
	}

	// unary minus
	Matrix<Type, M, N> operator-() സ്ഥിരം
	{
		Matrix<Type, M, N> res;
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				res(i, j) = -self(i, j);
			}
		}

		തിരികെ res;
	}

	ശൂന്യം operator+=(സ്ഥിരം Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				self(i, j) += other(i, j);
			}
		}
	}

	ശൂന്യം operator-=(സ്ഥിരം Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				self(i, j) -= other(i, j);
			}
		}
	}

	template<size_t P>
	ശൂന്യം operator*=(സ്ഥിരം Matrix<Type, N, P> &other)
	{
		Matrix<Type, M, N> &self = *ഇത്;
		self = self * other;
	}

	/**
	 * Scalar Operations
	 */

	Matrix<Type, M, N> operator*(Type scalar) സ്ഥിരം
	{
		Matrix<Type, M, N> res;
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * scalar;
			}
		}

		തിരികെ res;
	}

	വരിയിൽ Matrix<Type, M, N> operator/(Type scalar) സ്ഥിരം
	{
		തിരികെ (*ഇത്) * (1 / scalar);
	}

	Matrix<Type, M, N> operator+(Type scalar) സ്ഥിരം
	{
		Matrix<Type, M, N> res;
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + scalar;
			}
		}

		തിരികെ res;
	}

	വരിയിൽ Matrix<Type, M, N> operator-(Type scalar) സ്ഥിരം
	{
		തിരികെ (*ഇത്) + (-1 * scalar);
	}

	ശൂന്യം operator*=(Type scalar)
	{
		Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				self(i, j) *= scalar;
			}
		}
	}

	ശൂന്യം operator/=(Type scalar)
	{
		Matrix<Type, M, N> &self = *ഇത്;
		self *= (Type(1) / scalar);
	}

	വരിയിൽ ശൂന്യം operator+=(Type scalar)
	{
		Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				self(i, j) += scalar;
			}
		}
	}

	വരിയിൽ ശൂന്യം operator-=(Type scalar)
	{
		Matrix<Type, M, N> &self = *ഇത്;
		self += (-scalar);
	}

	തർക്കം operator==(സ്ഥിരം Matrix<Type, M, N> &other) സ്ഥിരം
	{
		തിരികെ isEqual(*ഇത്, other);
	}

	തർക്കം operator!=(സ്ഥിരം Matrix<Type, M, N> &other) സ്ഥിരം
	{
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;
		തിരികെ !(self == other);
	}

	/**
	 * Misc. Functions
	 */

	ശൂന്യം write_string(അക്ഷരം *buf, size_t n) സ്ഥിരം
	{
		buf[0] = '\0'; // make an empty string to begin with (we need the '\0' for strlen to work)
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				snprintf(buf + strlen(buf), n - strlen(buf), "\t%8.8g", ഇരട്ട(self(i, j))); // directly append to the string buffer
			}

			snprintf(buf + strlen(buf), n - strlen(buf), "\n");
		}
	}

	ശൂന്യം print(ദശാംശം eps = 1e-9) സ്ഥിരം
	{
		// print column numbering
		എങ്കിൽ (N > 1) {
			printf("  ");

			വേണ്ടി (unsigned i = 0; i < N; i++) {
				printf("|%2u      ", i);

			}

			printf("\n");
		}

		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;
		തർക്കം is_prev_symmetric = സത്യം; // assume symmetric until one element is not

		വേണ്ടി (unsigned i = 0; i < M; i++) {
			printf("%2u|", i); // print row numbering

			വേണ്ടി (unsigned j = 0; j < N; j++) {
				ഇരട്ട d = static_cast<ഇരട്ട>(self(i, j));

				// if symmetric don't print upper triangular elements
				എങ്കിൽ (is_prev_symmetric && (M == N) && (j > i) && (i < N) && (j < M)
				    && (fabs(d - static_cast<ഇരട്ട>(self(j, i))) < (ഇരട്ട)eps)
				   ) {
					// print empty space
					printf("         ");

				} അല്ലെങ്കിൽ {
					// avoid -0.0 for display
					എങ്കിൽ (fabs(d - 0.0) < (ഇരട്ട)eps) {
						// print fixed width zero
						printf(" 0       ");

					} അല്ലെങ്കിൽ എങ്കിൽ ((fabs(d) < 1e-4) || (fabs(d) >= 10.0)) {
						printf("% .1e ", d);

					} അല്ലെങ്കിൽ {
						printf("% 6.5f ", d);
					}

					is_prev_symmetric = അസത്യം; // not symmetric if once inside here
				}
			}

			printf("\n");
		}
	}

	Matrix<Type, N, M> transpose() സ്ഥിരം
	{
		Matrix<Type, N, M> res;
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				res(j, i) = self(i, j);
			}
		}

		തിരികെ res;
	}

	// tranpose alias
	വരിയിൽ Matrix<Type, N, M> T() സ്ഥിരം
	{
		തിരികെ transpose();
	}

	template<size_t P, size_t Q>
	ConstSlice<Type, P, Q, M, N> slice(size_t x0, size_t y0) സ്ഥിരം
	{
		തിരികെ {x0, y0, ഇത്};
	}

	template<size_t P, size_t Q>
	Slice<Type, P, Q, M, N> slice(size_t x0, size_t y0)
	{
		തിരികെ {x0, y0, ഇത്};
	}

	ConstSlice<Type, 1, N, M, N> row(size_t i) സ്ഥിരം
	{
		തിരികെ slice<1, N>(i, 0);
	}

	Slice<Type, 1, N, M, N> row(size_t i)
	{
		തിരികെ slice<1, N>(i, 0);
	}

	ConstSlice<Type, M, 1, M, N> col(size_t j) സ്ഥിരം
	{
		തിരികെ slice<M, 1>(0, j);
	}

	Slice<Type, M, 1, M, N> col(size_t j)
	{
		തിരികെ slice<M, 1>(0, j);
	}

	ശൂന്യം setRow(size_t i, സ്ഥിരം Matrix<Type, N, 1> &row_in)
	{
		slice<1, N>(i, 0) = row_in.transpose();
	}

	ശൂന്യം setRow(size_t i, Type val)
	{
		slice<1, N>(i, 0) = val;
	}

	ശൂന്യം setCol(size_t j, സ്ഥിരം Matrix<Type, M, 1> &column)
	{
		slice<M, 1>(0, j) = column;
	}

	ശൂന്യം setCol(size_t j, Type val)
	{
		slice<M, 1>(0, j) = val;
	}

	ശൂന്യം setZero()
	{
		memset(_data, 0, വലുപ്പം(_data));
	}

	വരിയിൽ ശൂന്യം zero()
	{
		setZero();
	}

	ശൂന്യം setAll(Type val)
	{
		Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				self(i, j) = val;
			}
		}
	}

	വരിയിൽ ശൂന്യം setOne()
	{
		setAll(1);
	}

	വരിയിൽ ശൂന്യം setNaN()
	{
		setAll(NAN);
	}

	ശൂന്യം setIdentity()
	{
		setZero();
		Matrix<Type, M, N> &self = *ഇത്;

		സ്ഥിരം size_t min_i = M > N ? N : M;

		വേണ്ടി (size_t i = 0; i < min_i; i++) {
			self(i, i) = 1;
		}
	}

	വരിയിൽ ശൂന്യം identity()
	{
		setIdentity();
	}

	വരിയിൽ ശൂന്യം swapRows(size_t a, size_t b)
	{
		പ്രതിജ്ഞാ(a < M);
		പ്രതിജ്ഞാ(b < M);

		എങ്കിൽ (a == b) {
			തിരികെ;
		}

		Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t j = 0; j < N; j++) {
			Type tmp = self(a, j);
			self(a, j) = self(b, j);
			self(b, j) = tmp;
		}
	}

	വരിയിൽ ശൂന്യം swapCols(size_t a, size_t b)
	{
		പ്രതിജ്ഞാ(a < N);
		പ്രതിജ്ഞാ(b < N);

		എങ്കിൽ (a == b) {
			തിരികെ;
		}

		Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			Type tmp = self(i, a);
			self(i, a) = self(i, b);
			self(i, b) = tmp;
		}
	}

	വരിയിൽ ശൂന്യം nanToZero()
	{

		Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				എങ്കിൽ (std::isnan(self(i, j))) {
					self(i, j) = static_cast<Type>(0.0f);
				}
			}
		}
	}

	Matrix<Type, M, N> abs() സ്ഥിരം
	{
		Matrix<Type, M, N> r;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				r(i, j) = Type(std::fabs((*ഇത്)(i, j)));
			}
		}

		തിരികെ r;
	}

	Type max() സ്ഥിരം
	{
		Type max_val = (*ഇത്)(0, 0);

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
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

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				Type val = (*ഇത്)(i, j);

				എങ്കിൽ (val < min_val) {
					min_val = val;
				}
			}
		}

		തിരികെ min_val;
	}

	തർക്കം isAllNan() സ്ഥിരം
	{
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;
		തർക്കം result = സത്യം;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				result = result && std::isnan(self(i, j));
			}
		}

		തിരികെ result;
	}

	തർക്കം isAllFinite() സ്ഥിരം
	{
		സ്ഥിരം Matrix<Type, M, N> &self = *ഇത്;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				എങ്കിൽ (!std::isfinite(self(i, j))) {
					തിരികെ അസത്യം;
				}
			}
		}

		തിരികെ സത്യം;
	}
};

template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> zeros()
{
	Matrix<Type, M, N> m;
	m.setZero();
	തിരികെ m;
}

template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> ones()
{
	Matrix<Type, M, N> m;
	m.setOne();
	തിരികെ m;
}

template<size_t M, size_t N>
Matrix<ദശാംശം, M, N> nans()
{
	Matrix<ദശാംശം, M, N> m;
	m.setNaN();
	തിരികെ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> operator*(Type scalar, സ്ഥിരം Matrix<Type, M, N> &other)
{
	തിരികെ other * scalar;
}

template<typename Type, size_t  M, size_t N>
തർക്കം isEqual(സ്ഥിരം Matrix<Type, M, N> &x,
	     സ്ഥിരം Matrix<Type, M, N> &y, സ്ഥിരം Type eps = Type(1e-4f))
{
	വേണ്ടി (size_t i = 0; i < M; i++) {
		വേണ്ടി (size_t j = 0; j < N; j++) {
			എങ്കിൽ (!isEqualF(x(i, j), y(i, j), eps)) {
				തിരികെ അസത്യം;
			}
		}
	}

	തിരികെ സത്യം;
}

നാമാകാശ typeFunction
{
template<typename Type>
Type min(സ്ഥിരം Type x, സ്ഥിരം Type y)
{
	തർക്കം x_is_nan = std::isnan(x);
	തർക്കം y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	എങ്കിൽ (x_is_nan || y_is_nan) {
		എങ്കിൽ (x_is_nan && !y_is_nan) {
			തിരികെ y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		തിരികെ x;
	}

	തിരികെ (x < y) ? x : y;
}

template<typename Type>
Type max(സ്ഥിരം Type x, സ്ഥിരം Type y)
{
	തർക്കം x_is_nan = std::isnan(x);
	തർക്കം y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	എങ്കിൽ (x_is_nan || y_is_nan) {
		എങ്കിൽ (x_is_nan && !y_is_nan) {
			തിരികെ y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		തിരികെ x;
	}

	തിരികെ (x > y) ? x : y;
}

template<typename Type>
Type constrain(സ്ഥിരം Type x, സ്ഥിരം Type lower_bound, സ്ഥിരം Type upper_bound)
{
	എങ്കിൽ (lower_bound > upper_bound) {
		തിരികെ NAN;

	} അല്ലെങ്കിൽ എങ്കിൽ (std::isnan(x)) {
		തിരികെ NAN;

	} അല്ലെങ്കിൽ {
		തിരികെ typeFunction::max(lower_bound, typeFunction::min(upper_bound, x));
	}
}
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(സ്ഥിരം Matrix<Type, M, N> &x, സ്ഥിരം Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	വേണ്ടി (size_t i = 0; i < M; i++) {
		വേണ്ടി (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x(i, j), scalar_upper_bound);
		}
	}

	തിരികെ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(സ്ഥിരം Type scalar_upper_bound, സ്ഥിരം Matrix<Type, M, N> &x)
{
	തിരികെ min(x, scalar_upper_bound);
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(സ്ഥിരം Matrix<Type, M, N> &x1, സ്ഥിരം Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	വേണ്ടി (size_t i = 0; i < M; i++) {
		വേണ്ടി (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x1(i, j), x2(i, j));
		}
	}

	തിരികെ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(സ്ഥിരം Matrix<Type, M, N> &x, സ്ഥിരം Type scalar_lower_bound)
{
	Matrix<Type, M, N> m;

	വേണ്ടി (size_t i = 0; i < M; i++) {
		വേണ്ടി (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x(i, j), scalar_lower_bound);
		}
	}

	തിരികെ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(സ്ഥിരം Type scalar_lower_bound, സ്ഥിരം Matrix<Type, M, N> &x)
{
	തിരികെ max(x, scalar_lower_bound);
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(സ്ഥിരം Matrix<Type, M, N> &x1, സ്ഥിരം Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	വേണ്ടി (size_t i = 0; i < M; i++) {
		വേണ്ടി (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x1(i, j), x2(i, j));
		}
	}

	തിരികെ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(സ്ഥിരം Matrix<Type, M, N> &x,
			     സ്ഥിരം Type scalar_lower_bound,
			     സ്ഥിരം Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	എങ്കിൽ (scalar_lower_bound > scalar_upper_bound) {
		m.setNaN();

	} അല്ലെങ്കിൽ {
		വേണ്ടി (size_t i = 0; i < M; i++) {
			വേണ്ടി (size_t j = 0; j < N; j++) {
				m(i, j) = typeFunction::constrain(x(i, j), scalar_lower_bound, scalar_upper_bound);
			}
		}
	}

	തിരികെ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(സ്ഥിരം Matrix<Type, M, N> &x,
			     സ്ഥിരം Matrix<Type, M, N> &x_lower_bound,
			     സ്ഥിരം Matrix<Type, M, N> &x_upper_bound)
{
	Matrix<Type, M, N> m;

	വേണ്ടി (size_t i = 0; i < M; i++) {
		വേണ്ടി (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::constrain(x(i, j), x_lower_bound(i, j), x_upper_bound(i, j));
		}
	}

	തിരികെ m;
}

template<typename OStream, typename Type, size_t M, size_t N>
OStream &operator<<(OStream &os, സ്ഥിരം matrix::Matrix<Type, M, N> &matrix)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * N * M + M + 1;
	അക്ഷരം string[n];
	matrix.write_string(string, n);
	os << string;
	തിരികെ os;
}

} // namespace matrix
