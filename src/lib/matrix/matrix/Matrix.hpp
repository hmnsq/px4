/**
 * @file Matrix.hpp
 *
 * A simple matrix template library.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ <cmath>
#ଅନ୍ତର୍ଭାଵ <cstdio>
#ଅନ୍ତର୍ଭାଵ <cstring>

#ଅନ୍ତର୍ଭାଵ "helper_functions.hpp"
#ଅନ୍ତର୍ଭାଵ "Slice.hpp"

ନାମାକାଶ matrix
{

template<typename Type, size_t M, size_t N>
ଶ୍ରେଣୀ Matrix
{
	Type _data[M][N] {};

ସର୍ବସାଧାରଣ:

	// Constructors
	Matrix() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	explicit Matrix(ଅଚଳ Type data_[M * N])
	{
		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[N * i + j];
			}
		}
	}

	explicit Matrix(ଅଚଳ Type data_[M][N])
	{
		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[i][j];
			}
		}
	}

	Matrix(ଅଚଳ Matrix &other)
	{
		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				_data[i][j] = other(i, j);
			}
		}
	}

	template<typename S>
	Matrix(ଅଚଳ Matrix<S, M, N> &aa)
	{
		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				_data[i][j] = static_cast<Type>(aa(i, j));
			}
		}
	}

	template<size_t P, size_t Q>
	Matrix(ଅଚଳ Slice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	template<size_t P, size_t Q>
	Matrix(ଅଚଳ ConstSlice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	/**
	 * Accessors/ Assignment etc.
	 */


	ଧାଡ଼ିଭିତରେ ଅଚଳ Type &operator()(size_t i, size_t j) ଅଚଳ
	{
		ପ୍ରତିଜ୍ଞା(i < M);
		ପ୍ରତିଜ୍ଞା(j < N);

		ଫେରନ୍ତୁ _data[i][j];
	}

	ଧାଡ଼ିଭିତରେ Type &operator()(size_t i, size_t j)
	{
		ପ୍ରତିଜ୍ଞା(i < M);
		ପ୍ରତିଜ୍ଞା(j < N);

		ଫେରନ୍ତୁ _data[i][j];
	}

	Matrix<Type, M, N> &operator=(ଅଚଳ Matrix<Type, M, N> &other)
	{
		ଯଦି (ଏହି != &other) {
			Matrix<Type, M, N> &self = *ଏହି;

			ପାଇଁ (size_t i = 0; i < M; i++) {
				ପାଇଁ (size_t j = 0; j < N; j++) {
					self(i, j) = other(i, j);
				}
			}
		}

		ଫେରନ୍ତୁ (*ଏହି);
	}

	ଶୂନ୍ୟ copyTo(Type dst[M * N]) ଅଚଳ
	{
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				dst[N * i + j] = self(i, j);
			}
		}
	}

	ଶୂନ୍ୟ copyToColumnMajor(Type dst[M * N]) ଅଚଳ
	{
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
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
	Matrix<Type, M, P> operator*(ଅଚଳ Matrix<Type, N, P> &other) ଅଚଳ
	{
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;
		Matrix<Type, M, P> res{};

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t k = 0; k < P; k++) {
				ପାଇଁ (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(j, k);
				}
			}
		}

		ଫେରନ୍ତୁ res;
	}

	// Using this function reduces the number of temporary variables needed to compute A * B.T
	template<size_t P>
	Matrix<Type, M, M> multiplyByTranspose(ଅଚଳ Matrix<Type, P, N> &other) ଅଚଳ
	{
		Matrix<Type, M, P> res;
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t k = 0; k < P; k++) {
				ପାଇଁ (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(k, j);
				}
			}
		}

		ଫେରନ୍ତୁ res;
	}

	// Element-wise multiplication
	Matrix<Type, M, N> emult(ଅଚଳ Matrix<Type, M, N> &other) ଅଚଳ
	{
		Matrix<Type, M, N> res;
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * other(i, j);
			}
		}

		ଫେରନ୍ତୁ res;
	}

	// Element-wise division
	Matrix<Type, M, N> edivide(ଅଚଳ Matrix<Type, M, N> &other) ଅଚଳ
	{
		Matrix<Type, M, N> res;
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) / other(i, j);
			}
		}

		ଫେରନ୍ତୁ res;
	}

	Matrix<Type, M, N> operator+(ଅଚଳ Matrix<Type, M, N> &other) ଅଚଳ
	{
		Matrix<Type, M, N> res;
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + other(i, j);
			}
		}

		ଫେରନ୍ତୁ res;
	}

	Matrix<Type, M, N> operator-(ଅଚଳ Matrix<Type, M, N> &other) ଅଚଳ
	{
		Matrix<Type, M, N> res;
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) - other(i, j);
			}
		}

		ଫେରନ୍ତୁ res;
	}

	// unary minus
	Matrix<Type, M, N> operator-() ଅଚଳ
	{
		Matrix<Type, M, N> res;
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				res(i, j) = -self(i, j);
			}
		}

		ଫେରନ୍ତୁ res;
	}

	ଶୂନ୍ୟ operator+=(ଅଚଳ Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				self(i, j) += other(i, j);
			}
		}
	}

	ଶୂନ୍ୟ operator-=(ଅଚଳ Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				self(i, j) -= other(i, j);
			}
		}
	}

	template<size_t P>
	ଶୂନ୍ୟ operator*=(ଅଚଳ Matrix<Type, N, P> &other)
	{
		Matrix<Type, M, N> &self = *ଏହି;
		self = self * other;
	}

	/**
	 * Scalar Operations
	 */

	Matrix<Type, M, N> operator*(Type scalar) ଅଚଳ
	{
		Matrix<Type, M, N> res;
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * scalar;
			}
		}

		ଫେରନ୍ତୁ res;
	}

	ଧାଡ଼ିଭିତରେ Matrix<Type, M, N> operator/(Type scalar) ଅଚଳ
	{
		ଫେରନ୍ତୁ (*ଏହି) * (1 / scalar);
	}

	Matrix<Type, M, N> operator+(Type scalar) ଅଚଳ
	{
		Matrix<Type, M, N> res;
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + scalar;
			}
		}

		ଫେରନ୍ତୁ res;
	}

	ଧାଡ଼ିଭିତରେ Matrix<Type, M, N> operator-(Type scalar) ଅଚଳ
	{
		ଫେରନ୍ତୁ (*ଏହି) + (-1 * scalar);
	}

	ଶୂନ୍ୟ operator*=(Type scalar)
	{
		Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				self(i, j) *= scalar;
			}
		}
	}

	ଶୂନ୍ୟ operator/=(Type scalar)
	{
		Matrix<Type, M, N> &self = *ଏହି;
		self *= (Type(1) / scalar);
	}

	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ operator+=(Type scalar)
	{
		Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				self(i, j) += scalar;
			}
		}
	}

	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ operator-=(Type scalar)
	{
		Matrix<Type, M, N> &self = *ଏହି;
		self += (-scalar);
	}

	ତାର୍କିକ operator==(ଅଚଳ Matrix<Type, M, N> &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ isEqual(*ଏହି, other);
	}

	ତାର୍କିକ operator!=(ଅଚଳ Matrix<Type, M, N> &other) ଅଚଳ
	{
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;
		ଫେରନ୍ତୁ !(self == other);
	}

	/**
	 * Misc. Functions
	 */

	ଶୂନ୍ୟ write_string(ଅକ୍ଷର *buf, size_t n) ଅଚଳ
	{
		buf[0] = '\0'; // make an empty string to begin with (we need the '\0' for strlen to work)
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				snprintf(buf + strlen(buf), n - strlen(buf), "\t%8.8g", ଦ୍ୱିଗୁଣ(self(i, j))); // directly append to the string buffer
			}

			snprintf(buf + strlen(buf), n - strlen(buf), "\n");
		}
	}

	ଶୂନ୍ୟ print(ଦଶମିକ eps = 1e-9) ଅଚଳ
	{
		// print column numbering
		ଯଦି (N > 1) {
			printf("  ");

			ପାଇଁ (unsigned i = 0; i < N; i++) {
				printf("|%2u      ", i);

			}

			printf("\n");
		}

		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;
		ତାର୍କିକ is_prev_symmetric = ସତ୍ୟ; // assume symmetric until one element is not

		ପାଇଁ (unsigned i = 0; i < M; i++) {
			printf("%2u|", i); // print row numbering

			ପାଇଁ (unsigned j = 0; j < N; j++) {
				ଦ୍ୱିଗୁଣ d = static_cast<ଦ୍ୱିଗୁଣ>(self(i, j));

				// if symmetric don't print upper triangular elements
				ଯଦି (is_prev_symmetric && (M == N) && (j > i) && (i < N) && (j < M)
				    && (fabs(d - static_cast<ଦ୍ୱିଗୁଣ>(self(j, i))) < (ଦ୍ୱିଗୁଣ)eps)
				   ) {
					// print empty space
					printf("         ");

				} ଅନ୍ୟଥା {
					// avoid -0.0 for display
					ଯଦି (fabs(d - 0.0) < (ଦ୍ୱିଗୁଣ)eps) {
						// print fixed width zero
						printf(" 0       ");

					} ଅନ୍ୟଥା ଯଦି ((fabs(d) < 1e-4) || (fabs(d) >= 10.0)) {
						printf("% .1e ", d);

					} ଅନ୍ୟଥା {
						printf("% 6.5f ", d);
					}

					is_prev_symmetric = ମିଥ୍ୟା; // not symmetric if once inside here
				}
			}

			printf("\n");
		}
	}

	Matrix<Type, N, M> transpose() ଅଚଳ
	{
		Matrix<Type, N, M> res;
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				res(j, i) = self(i, j);
			}
		}

		ଫେରନ୍ତୁ res;
	}

	// tranpose alias
	ଧାଡ଼ିଭିତରେ Matrix<Type, N, M> T() ଅଚଳ
	{
		ଫେରନ୍ତୁ transpose();
	}

	template<size_t P, size_t Q>
	ConstSlice<Type, P, Q, M, N> slice(size_t x0, size_t y0) ଅଚଳ
	{
		ଫେରନ୍ତୁ {x0, y0, ଏହି};
	}

	template<size_t P, size_t Q>
	Slice<Type, P, Q, M, N> slice(size_t x0, size_t y0)
	{
		ଫେରନ୍ତୁ {x0, y0, ଏହି};
	}

	ConstSlice<Type, 1, N, M, N> row(size_t i) ଅଚଳ
	{
		ଫେରନ୍ତୁ slice<1, N>(i, 0);
	}

	Slice<Type, 1, N, M, N> row(size_t i)
	{
		ଫେରନ୍ତୁ slice<1, N>(i, 0);
	}

	ConstSlice<Type, M, 1, M, N> col(size_t j) ଅଚଳ
	{
		ଫେରନ୍ତୁ slice<M, 1>(0, j);
	}

	Slice<Type, M, 1, M, N> col(size_t j)
	{
		ଫେରନ୍ତୁ slice<M, 1>(0, j);
	}

	ଶୂନ୍ୟ setRow(size_t i, ଅଚଳ Matrix<Type, N, 1> &row_in)
	{
		slice<1, N>(i, 0) = row_in.transpose();
	}

	ଶୂନ୍ୟ setRow(size_t i, Type val)
	{
		slice<1, N>(i, 0) = val;
	}

	ଶୂନ୍ୟ setCol(size_t j, ଅଚଳ Matrix<Type, M, 1> &column)
	{
		slice<M, 1>(0, j) = column;
	}

	ଶୂନ୍ୟ setCol(size_t j, Type val)
	{
		slice<M, 1>(0, j) = val;
	}

	ଶୂନ୍ୟ setZero()
	{
		memset(_data, 0, ଆକାର(_data));
	}

	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ zero()
	{
		setZero();
	}

	ଶୂନ୍ୟ setAll(Type val)
	{
		Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				self(i, j) = val;
			}
		}
	}

	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ setOne()
	{
		setAll(1);
	}

	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ setNaN()
	{
		setAll(NAN);
	}

	ଶୂନ୍ୟ setIdentity()
	{
		setZero();
		Matrix<Type, M, N> &self = *ଏହି;

		ଅଚଳ size_t min_i = M > N ? N : M;

		ପାଇଁ (size_t i = 0; i < min_i; i++) {
			self(i, i) = 1;
		}
	}

	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ identity()
	{
		setIdentity();
	}

	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ swapRows(size_t a, size_t b)
	{
		ପ୍ରତିଜ୍ଞା(a < M);
		ପ୍ରତିଜ୍ଞା(b < M);

		ଯଦି (a == b) {
			ଫେରନ୍ତୁ;
		}

		Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t j = 0; j < N; j++) {
			Type tmp = self(a, j);
			self(a, j) = self(b, j);
			self(b, j) = tmp;
		}
	}

	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ swapCols(size_t a, size_t b)
	{
		ପ୍ରତିଜ୍ଞା(a < N);
		ପ୍ରତିଜ୍ଞା(b < N);

		ଯଦି (a == b) {
			ଫେରନ୍ତୁ;
		}

		Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			Type tmp = self(i, a);
			self(i, a) = self(i, b);
			self(i, b) = tmp;
		}
	}

	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ nanToZero()
	{

		Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				ଯଦି (std::isnan(self(i, j))) {
					self(i, j) = static_cast<Type>(0.0f);
				}
			}
		}
	}

	Matrix<Type, M, N> abs() ଅଚଳ
	{
		Matrix<Type, M, N> r;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				r(i, j) = Type(std::fabs((*ଏହି)(i, j)));
			}
		}

		ଫେରନ୍ତୁ r;
	}

	Type max() ଅଚଳ
	{
		Type max_val = (*ଏହି)(0, 0);

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
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

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				Type val = (*ଏହି)(i, j);

				ଯଦି (val < min_val) {
					min_val = val;
				}
			}
		}

		ଫେରନ୍ତୁ min_val;
	}

	ତାର୍କିକ isAllNan() ଅଚଳ
	{
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;
		ତାର୍କିକ result = ସତ୍ୟ;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				result = result && std::isnan(self(i, j));
			}
		}

		ଫେରନ୍ତୁ result;
	}

	ତାର୍କିକ isAllFinite() ଅଚଳ
	{
		ଅଚଳ Matrix<Type, M, N> &self = *ଏହି;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				ଯଦି (!std::isfinite(self(i, j))) {
					ଫେରନ୍ତୁ ମିଥ୍ୟା;
				}
			}
		}

		ଫେରନ୍ତୁ ସତ୍ୟ;
	}
};

template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> zeros()
{
	Matrix<Type, M, N> m;
	m.setZero();
	ଫେରନ୍ତୁ m;
}

template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> ones()
{
	Matrix<Type, M, N> m;
	m.setOne();
	ଫେରନ୍ତୁ m;
}

template<size_t M, size_t N>
Matrix<ଦଶମିକ, M, N> nans()
{
	Matrix<ଦଶମିକ, M, N> m;
	m.setNaN();
	ଫେରନ୍ତୁ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> operator*(Type scalar, ଅଚଳ Matrix<Type, M, N> &other)
{
	ଫେରନ୍ତୁ other * scalar;
}

template<typename Type, size_t  M, size_t N>
ତାର୍କିକ isEqual(ଅଚଳ Matrix<Type, M, N> &x,
	     ଅଚଳ Matrix<Type, M, N> &y, ଅଚଳ Type eps = Type(1e-4f))
{
	ପାଇଁ (size_t i = 0; i < M; i++) {
		ପାଇଁ (size_t j = 0; j < N; j++) {
			ଯଦି (!isEqualF(x(i, j), y(i, j), eps)) {
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			}
		}
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ନାମାକାଶ typeFunction
{
template<typename Type>
Type min(ଅଚଳ Type x, ଅଚଳ Type y)
{
	ତାର୍କିକ x_is_nan = std::isnan(x);
	ତାର୍କିକ y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	ଯଦି (x_is_nan || y_is_nan) {
		ଯଦି (x_is_nan && !y_is_nan) {
			ଫେରନ୍ତୁ y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		ଫେରନ୍ତୁ x;
	}

	ଫେରନ୍ତୁ (x < y) ? x : y;
}

template<typename Type>
Type max(ଅଚଳ Type x, ଅଚଳ Type y)
{
	ତାର୍କିକ x_is_nan = std::isnan(x);
	ତାର୍କିକ y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	ଯଦି (x_is_nan || y_is_nan) {
		ଯଦି (x_is_nan && !y_is_nan) {
			ଫେରନ୍ତୁ y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		ଫେରନ୍ତୁ x;
	}

	ଫେରନ୍ତୁ (x > y) ? x : y;
}

template<typename Type>
Type constrain(ଅଚଳ Type x, ଅଚଳ Type lower_bound, ଅଚଳ Type upper_bound)
{
	ଯଦି (lower_bound > upper_bound) {
		ଫେରନ୍ତୁ NAN;

	} ଅନ୍ୟଥା ଯଦି (std::isnan(x)) {
		ଫେରନ୍ତୁ NAN;

	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ typeFunction::max(lower_bound, typeFunction::min(upper_bound, x));
	}
}
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(ଅଚଳ Matrix<Type, M, N> &x, ଅଚଳ Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	ପାଇଁ (size_t i = 0; i < M; i++) {
		ପାଇଁ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x(i, j), scalar_upper_bound);
		}
	}

	ଫେରନ୍ତୁ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(ଅଚଳ Type scalar_upper_bound, ଅଚଳ Matrix<Type, M, N> &x)
{
	ଫେରନ୍ତୁ min(x, scalar_upper_bound);
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(ଅଚଳ Matrix<Type, M, N> &x1, ଅଚଳ Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	ପାଇଁ (size_t i = 0; i < M; i++) {
		ପାଇଁ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x1(i, j), x2(i, j));
		}
	}

	ଫେରନ୍ତୁ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(ଅଚଳ Matrix<Type, M, N> &x, ଅଚଳ Type scalar_lower_bound)
{
	Matrix<Type, M, N> m;

	ପାଇଁ (size_t i = 0; i < M; i++) {
		ପାଇଁ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x(i, j), scalar_lower_bound);
		}
	}

	ଫେରନ୍ତୁ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(ଅଚଳ Type scalar_lower_bound, ଅଚଳ Matrix<Type, M, N> &x)
{
	ଫେରନ୍ତୁ max(x, scalar_lower_bound);
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(ଅଚଳ Matrix<Type, M, N> &x1, ଅଚଳ Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	ପାଇଁ (size_t i = 0; i < M; i++) {
		ପାଇଁ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x1(i, j), x2(i, j));
		}
	}

	ଫେରନ୍ତୁ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(ଅଚଳ Matrix<Type, M, N> &x,
			     ଅଚଳ Type scalar_lower_bound,
			     ଅଚଳ Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	ଯଦି (scalar_lower_bound > scalar_upper_bound) {
		m.setNaN();

	} ଅନ୍ୟଥା {
		ପାଇଁ (size_t i = 0; i < M; i++) {
			ପାଇଁ (size_t j = 0; j < N; j++) {
				m(i, j) = typeFunction::constrain(x(i, j), scalar_lower_bound, scalar_upper_bound);
			}
		}
	}

	ଫେରନ୍ତୁ m;
}

template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(ଅଚଳ Matrix<Type, M, N> &x,
			     ଅଚଳ Matrix<Type, M, N> &x_lower_bound,
			     ଅଚଳ Matrix<Type, M, N> &x_upper_bound)
{
	Matrix<Type, M, N> m;

	ପାଇଁ (size_t i = 0; i < M; i++) {
		ପାଇଁ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::constrain(x(i, j), x_lower_bound(i, j), x_upper_bound(i, j));
		}
	}

	ଫେରନ୍ତୁ m;
}

template<typename OStream, typename Type, size_t M, size_t N>
OStream &operator<<(OStream &os, ଅଚଳ matrix::Matrix<Type, M, N> &matrix)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * N * M + M + 1;
	ଅକ୍ଷର string[n];
	matrix.write_string(string, n);
	os << string;
	ଫେରନ୍ତୁ os;
}

} // namespace matrix
