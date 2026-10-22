/**
 * @file Matrix.hpp
 *
 * A simple matrix template library.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <cmath>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdio>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>

#ਅਨ੍ਤਰ੍ਭਾਵ "helper_functions.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "Slice.hpp"

ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ
{

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t M, size_t N>
ਵਰਗ ਮੈਟ੍ਰਿਕਸ_2
{
	Type ਅੰਕੜੇ_3[M][N] {};

ਜਨਤਕ:

	// Constructors
	ਮੈਟ੍ਰਿਕਸ_2() = ਮੂਲ;

	ਸਪਸ਼ਟ ਮੈਟ੍ਰਿਕਸ_2(ਅਚਲ Type ਅੰਕੜੇ_2[M * N])
	{
		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				ਅੰਕੜੇ_3[i][j] = ਅੰਕੜੇ_2[N * i + j];
			}
		}
	}

	ਸਪਸ਼ਟ ਮੈਟ੍ਰਿਕਸ_2(ਅਚਲ Type ਅੰਕੜੇ_2[M][N])
	{
		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				ਅੰਕੜੇ_3[i][j] = ਅੰਕੜੇ_2[i][j];
			}
		}
	}

	ਮੈਟ੍ਰਿਕਸ_2(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2 &other)
	{
		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				ਅੰਕੜੇ_3[i][j] = other(i, j);
			}
		}
	}

	ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ S>
	ਮੈਟ੍ਰਿਕਸ_2(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<S, M, N> &aa)
	{
		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				ਅੰਕੜੇ_3[i][j] = static_cast<Type>(aa(i, j));
			}
		}
	}

	ਸਾਂਚਾ<size_t P, size_t Q>
	ਮੈਟ੍ਰਿਕਸ_2(ਅਚਲ ਟੁਕੜਾ_2<Type, M, N, P, Q> &ਵਿੱਚ_ਟੁਕੜਾ)
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				self(i, j) = ਵਿੱਚ_ਟੁਕੜਾ(i, j);
			}
		}
	}

	ਸਾਂਚਾ<size_t P, size_t Q>
	ਮੈਟ੍ਰਿਕਸ_2(ਅਚਲ Const_ਟੁਕੜਾ<Type, M, N, P, Q> &ਵਿੱਚ_ਟੁਕੜਾ)
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				self(i, j) = ਵਿੱਚ_ਟੁਕੜਾ(i, j);
			}
		}
	}

	/**
	 * Accessors/ Assignment etc.
	 */


	ਸਤਰਅੰਦਰ ਅਚਲ Type &ਸੰਚਾਲਕ()(size_t i, size_t j) ਅਚਲ
	{
		ਪ੍ਰਤਿਜ੍ਞਾ(i < M);
		ਪ੍ਰਤਿਜ੍ਞਾ(j < N);

		ਵਾਪਸ ਅੰਕੜੇ_3[i][j];
	}

	ਸਤਰਅੰਦਰ Type &ਸੰਚਾਲਕ()(size_t i, size_t j)
	{
		ਪ੍ਰਤਿਜ੍ਞਾ(i < M);
		ਪ੍ਰਤਿਜ੍ਞਾ(j < N);

		ਵਾਪਸ ਅੰਕੜੇ_3[i][j];
	}

	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &ਸੰਚਾਲਕ=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &other)
	{
		ਜੇ (ਇਹ != &other) {
			ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

			ਲਈ (size_t i = 0; i < M; i++) {
				ਲਈ (size_t j = 0; j < N; j++) {
					self(i, j) = other(i, j);
				}
			}
		}

		ਵਾਪਸ (*ਇਹ);
	}

	ਖਾਲੀ copy_ਮੰਜ਼ਿਲ(Type dst[M * N]) ਅਚਲ
	{
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				dst[N * i + j] = self(i, j);
			}
		}
	}

	ਖਾਲੀ copy_ਮੰਜ਼ਿਲ_ਕਾਲਮ_Major(Type dst[M * N]) ਅਚਲ
	{
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
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
	ਸਾਂਚਾ<size_t P>
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, P> ਸੰਚਾਲਕ*(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, N, P> &other) ਅਚਲ
	{
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, P> res{};

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t k = 0; k < P; k++) {
				ਲਈ (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(j, k);
				}
			}
		}

		ਵਾਪਸ res;
	}

	// Using this function reduces the number of temporary variables needed to compute A * B.T
	ਸਾਂਚਾ<size_t P>
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, M> multiplyByTranspose(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, P, N> &other) ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, P> res;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t k = 0; k < P; k++) {
				ਲਈ (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(k, j);
				}
			}
		}

		ਵਾਪਸ res;
	}

	// Element-wise multiplication
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> emult(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &other) ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> res;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * other(i, j);
			}
		}

		ਵਾਪਸ res;
	}

	// Element-wise division
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> edivide(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &other) ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> res;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) / other(i, j);
			}
		}

		ਵਾਪਸ res;
	}

	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> ਸੰਚਾਲਕ+(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &other) ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> res;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + other(i, j);
			}
		}

		ਵਾਪਸ res;
	}

	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> ਸੰਚਾਲਕ-(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &other) ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> res;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) - other(i, j);
			}
		}

		ਵਾਪਸ res;
	}

	// unary minus
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> ਸੰਚਾਲਕ-() ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> res;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				res(i, j) = -self(i, j);
			}
		}

		ਵਾਪਸ res;
	}

	ਖਾਲੀ ਸੰਚਾਲਕ+=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &other)
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				self(i, j) += other(i, j);
			}
		}
	}

	ਖਾਲੀ ਸੰਚਾਲਕ-=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &other)
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				self(i, j) -= other(i, j);
			}
		}
	}

	ਸਾਂਚਾ<size_t P>
	ਖਾਲੀ ਸੰਚਾਲਕ*=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, N, P> &other)
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;
		self = self * other;
	}

	/**
	 * Scalar Operations
	 */

	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> ਸੰਚਾਲਕ*(Type scalar) ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> res;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * scalar;
			}
		}

		ਵਾਪਸ res;
	}

	ਸਤਰਅੰਦਰ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> ਸੰਚਾਲਕ/(Type scalar) ਅਚਲ
	{
		ਵਾਪਸ (*ਇਹ) * (1 / scalar);
	}

	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> ਸੰਚਾਲਕ+(Type scalar) ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> res;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + scalar;
			}
		}

		ਵਾਪਸ res;
	}

	ਸਤਰਅੰਦਰ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> ਸੰਚਾਲਕ-(Type scalar) ਅਚਲ
	{
		ਵਾਪਸ (*ਇਹ) + (-1 * scalar);
	}

	ਖਾਲੀ ਸੰਚਾਲਕ*=(Type scalar)
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				self(i, j) *= scalar;
			}
		}
	}

	ਖਾਲੀ ਸੰਚਾਲਕ/=(Type scalar)
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;
		self *= (Type(1) / scalar);
	}

	ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੰਚਾਲਕ+=(Type scalar)
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				self(i, j) += scalar;
			}
		}
	}

	ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੰਚਾਲਕ-=(Type scalar)
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;
		self += (-scalar);
	}

	ਤਾਰਕਿਕ ਸੰਚਾਲਕ==(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &other) ਅਚਲ
	{
		ਵਾਪਸ isEqual(*ਇਹ, other);
	}

	ਤਾਰਕਿਕ ਸੰਚਾਲਕ!=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &other) ਅਚਲ
	{
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;
		ਵਾਪਸ !(self == other);
	}

	/**
	 * Misc. Functions
	 */

	ਖਾਲੀ write_string(ਅੱਖਰ *buf, size_t n) ਅਚਲ
	{
		buf[0] = '\0'; // make an empty string to begin with (we need the '\0' for strlen to work)
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				snprintf(buf + ਲੜੀ_ਲੰਬਾਈ(buf), n - ਲੜੀ_ਲੰਬਾਈ(buf), "\t%8.8g", ਦੁੱਗਣਾ(self(i, j))); // directly append to the string buffer
			}

			snprintf(buf + ਲੜੀ_ਲੰਬਾਈ(buf), n - ਲੜੀ_ਲੰਬਾਈ(buf), "\n");
		}
	}

	ਖਾਲੀ print(ਦਸ਼ਮਲਵ eps = 1e-9) ਅਚਲ
	{
		// print column numbering
		ਜੇ (N > 1) {
			ਲਿਖੋ("  ");

			ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < N; i++) {
				ਲਿਖੋ("|%2u      ", i);

			}

			ਲਿਖੋ("\n");
		}

		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;
		ਤਾਰਕਿਕ is_prev_ਸਮਮਿਤ = ਸੱਚ; // assume symmetric until one element is not

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < M; i++) {
			ਲਿਖੋ("%2u|", i); // print row numbering

			ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ j = 0; j < N; j++) {
				ਦੁੱਗਣਾ d = static_cast<ਦੁੱਗਣਾ>(self(i, j));

				// if symmetric don't print upper triangular elements
				ਜੇ (is_prev_ਸਮਮਿਤ && (M == N) && (j > i) && (i < N) && (j < M)
				    && (fabs(d - static_cast<ਦੁੱਗਣਾ>(self(j, i))) < (ਦੁੱਗਣਾ)eps)
				   ) {
					// print empty space
					ਲਿਖੋ("         ");

				} ਨਹੀਂਤਾਂ {
					// avoid -0.0 for display
					ਜੇ (fabs(d - 0.0) < (ਦੁੱਗਣਾ)eps) {
						// print fixed width zero
						ਲਿਖੋ(" 0       ");

					} ਨਹੀਂਤਾਂ ਜੇ ((fabs(d) < 1e-4) || (fabs(d) >= 10.0)) {
						ਲਿਖੋ("% .1e ", d);

					} ਨਹੀਂਤਾਂ {
						ਲਿਖੋ("% 6.5f ", d);
					}

					is_prev_ਸਮਮਿਤ = ਝੂਠ; // not symmetric if once inside here
				}
			}

			ਲਿਖੋ("\n");
		}
	}

	ਮੈਟ੍ਰਿਕਸ_2<Type, N, M> transpose() ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, N, M> res;
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				res(j, i) = self(i, j);
			}
		}

		ਵਾਪਸ res;
	}

	// tranpose alias
	ਸਤਰਅੰਦਰ ਮੈਟ੍ਰਿਕਸ_2<Type, N, M> T() ਅਚਲ
	{
		ਵਾਪਸ transpose();
	}

	ਸਾਂਚਾ<size_t P, size_t Q>
	Const_ਟੁਕੜਾ<Type, P, Q, M, N> ਟੁਕੜਾ(size_t x0, size_t y0) ਅਚਲ
	{
		ਵਾਪਸ {x0, y0, ਇਹ};
	}

	ਸਾਂਚਾ<size_t P, size_t Q>
	ਟੁਕੜਾ_2<Type, P, Q, M, N> ਟੁਕੜਾ(size_t x0, size_t y0)
	{
		ਵਾਪਸ {x0, y0, ਇਹ};
	}

	Const_ਟੁਕੜਾ<Type, 1, N, M, N> ਪੰਕਤੀ(size_t i) ਅਚਲ
	{
		ਵਾਪਸ ਟੁਕੜਾ<1, N>(i, 0);
	}

	ਟੁਕੜਾ_2<Type, 1, N, M, N> ਪੰਕਤੀ(size_t i)
	{
		ਵਾਪਸ ਟੁਕੜਾ<1, N>(i, 0);
	}

	Const_ਟੁਕੜਾ<Type, M, 1, M, N> ਖਾਨਾ(size_t j) ਅਚਲ
	{
		ਵਾਪਸ ਟੁਕੜਾ<M, 1>(0, j);
	}

	ਟੁਕੜਾ_2<Type, M, 1, M, N> ਖਾਨਾ(size_t j)
	{
		ਵਾਪਸ ਟੁਕੜਾ<M, 1>(0, j);
	}

	ਖਾਲੀ ਸੈੱਟ_ਪੰਕਤੀ(size_t i, ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, N, 1> &ਪੰਕਤੀ_ਵਿੱਚ)
	{
		ਟੁਕੜਾ<1, N>(i, 0) = ਪੰਕਤੀ_ਵਿੱਚ.transpose();
	}

	ਖਾਲੀ ਸੈੱਟ_ਪੰਕਤੀ(size_t i, Type val)
	{
		ਟੁਕੜਾ<1, N>(i, 0) = val;
	}

	ਖਾਲੀ ਸੈੱਟ_ਖਾਨਾ(size_t j, ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, 1> &ਕਾਲਮ)
	{
		ਟੁਕੜਾ<M, 1>(0, j) = ਕਾਲਮ;
	}

	ਖਾਲੀ ਸੈੱਟ_ਖਾਨਾ(size_t j, Type val)
	{
		ਟੁਕੜਾ<M, 1>(0, j) = val;
	}

	ਖਾਲੀ ਸੈੱਟ_Zero()
	{
		memset(ਅੰਕੜੇ_3, 0, ਆਕਾਰ(ਅੰਕੜੇ_3));
	}

	ਸਤਰਅੰਦਰ ਖਾਲੀ zero()
	{
		ਸੈੱਟ_Zero();
	}

	ਖਾਲੀ ਸੈੱਟ_All(Type val)
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				self(i, j) = val;
			}
		}
	}

	ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੈੱਟ_One()
	{
		ਸੈੱਟ_All(1);
	}

	ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੈੱਟ_Na_N()
	{
		ਸੈੱਟ_All(NAN);
	}

	ਖਾਲੀ ਸੈੱਟ_ਪਛਾਣ_ਤੱਤ()
	{
		ਸੈੱਟ_Zero();
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਅਚਲ size_t min_i = M > N ? N : M;

		ਲਈ (size_t i = 0; i < min_i; i++) {
			self(i, i) = 1;
		}
	}

	ਸਤਰਅੰਦਰ ਖਾਲੀ ਪਛਾਣ_ਤੱਤ()
	{
		ਸੈੱਟ_ਪਛਾਣ_ਤੱਤ();
	}

	ਸਤਰਅੰਦਰ ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ_ਪੰਕਤੀਆਂ(size_t a, size_t b)
	{
		ਪ੍ਰਤਿਜ੍ਞਾ(a < M);
		ਪ੍ਰਤਿਜ੍ਞਾ(b < M);

		ਜੇ (a == b) {
			ਵਾਪਸ;
		}

		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t j = 0; j < N; j++) {
			Type tmp = self(a, j);
			self(a, j) = self(b, j);
			self(b, j) = tmp;
		}
	}

	ਸਤਰਅੰਦਰ ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ_ਖਾਨੇ_ਗਿਣਤੀ(size_t a, size_t b)
	{
		ਪ੍ਰਤਿਜ੍ਞਾ(a < N);
		ਪ੍ਰਤਿਜ੍ਞਾ(b < N);

		ਜੇ (a == b) {
			ਵਾਪਸ;
		}

		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			Type tmp = self(i, a);
			self(i, a) = self(i, b);
			self(i, b) = tmp;
		}
	}

	ਸਤਰਅੰਦਰ ਖਾਲੀ nan_ਮੰਜ਼ਿਲ_Zero()
	{

		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				ਜੇ (std::isnan(self(i, j))) {
					self(i, j) = static_cast<Type>(0.0f);
				}
			}
		}
	}

	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> ਨਿਰਪੇਖ() ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> r;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				r(i, j) = Type(std::fabs((*ਇਹ)(i, j)));
			}
		}

		ਵਾਪਸ r;
	}

	Type max() ਅਚਲ
	{
		Type max_val = (*ਇਹ)(0, 0);

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				Type val = (*ਇਹ)(i, j);

				ਜੇ (val > max_val) {
					max_val = val;
				}
			}
		}

		ਵਾਪਸ max_val;
	}

	Type min() ਅਚਲ
	{
		Type min_val = (*ਇਹ)(0, 0);

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				Type val = (*ਇਹ)(i, j);

				ਜੇ (val < min_val) {
					min_val = val;
				}
			}
		}

		ਵਾਪਸ min_val;
	}

	ਤਾਰਕਿਕ isAllNan() ਅਚਲ
	{
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;
		ਤਾਰਕਿਕ ਨਤੀਜਾ = ਸੱਚ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				ਨਤੀਜਾ = ਨਤੀਜਾ && std::isnan(self(i, j));
			}
		}

		ਵਾਪਸ ਨਤੀਜਾ;
	}

	ਤਾਰਕਿਕ isAllFinite() ਅਚਲ
	{
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				ਜੇ (!std::isfinite(self(i, j))) {
					ਵਾਪਸ ਝੂਠ;
				}
			}
		}

		ਵਾਪਸ ਸੱਚ;
	}
};

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> zeros()
{
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> m;
	m.ਸੈੱਟ_Zero();
	ਵਾਪਸ m;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> ones()
{
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> m;
	m.ਸੈੱਟ_One();
	ਵਾਪਸ m;
}

ਸਾਂਚਾ<size_t M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, M, N> nans()
{
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, M, N> m;
	m.ਸੈੱਟ_Na_N();
	ਵਾਪਸ m;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t  M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> ਸੰਚਾਲਕ*(Type scalar, ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &other)
{
	ਵਾਪਸ other * scalar;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t  M, size_t N>
ਤਾਰਕਿਕ isEqual(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x,
	     ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &y, ਅਚਲ Type eps = Type(1e-4f))
{
	ਲਈ (size_t i = 0; i < M; i++) {
		ਲਈ (size_t j = 0; j < N; j++) {
			ਜੇ (!isEqualF(x(i, j), y(i, j), eps)) {
				ਵਾਪਸ ਝੂਠ;
			}
		}
	}

	ਵਾਪਸ ਸੱਚ;
}

ਨਾਮਾਕਾਸ਼ typeFunction
{
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type>
Type min(ਅਚਲ Type x, ਅਚਲ Type y)
{
	ਤਾਰਕਿਕ x_is_nan = std::isnan(x);
	ਤਾਰਕਿਕ y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	ਜੇ (x_is_nan || y_is_nan) {
		ਜੇ (x_is_nan && !y_is_nan) {
			ਵਾਪਸ y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		ਵਾਪਸ x;
	}

	ਵਾਪਸ (x < y) ? x : y;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type>
Type max(ਅਚਲ Type x, ਅਚਲ Type y)
{
	ਤਾਰਕਿਕ x_is_nan = std::isnan(x);
	ਤਾਰਕਿਕ y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	ਜੇ (x_is_nan || y_is_nan) {
		ਜੇ (x_is_nan && !y_is_nan) {
			ਵਾਪਸ y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		ਵਾਪਸ x;
	}

	ਵਾਪਸ (x > y) ? x : y;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type>
Type constrain(ਅਚਲ Type x, ਅਚਲ Type ਹੇਠਲਾ_bound, ਅਚਲ Type ਉੱਤਲਾ_bound)
{
	ਜੇ (ਹੇਠਲਾ_bound > ਉੱਤਲਾ_bound) {
		ਵਾਪਸ NAN;

	} ਨਹੀਂਤਾਂ ਜੇ (std::isnan(x)) {
		ਵਾਪਸ NAN;

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ typeFunction::max(ਹੇਠਲਾ_bound, typeFunction::min(ਉੱਤਲਾ_bound, x));
	}
}
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t  M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> min(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x, ਅਚਲ Type scalar_ਉੱਤਲਾ_bound)
{
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> m;

	ਲਈ (size_t i = 0; i < M; i++) {
		ਲਈ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x(i, j), scalar_ਉੱਤਲਾ_bound);
		}
	}

	ਵਾਪਸ m;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t  M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> min(ਅਚਲ Type scalar_ਉੱਤਲਾ_bound, ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x)
{
	ਵਾਪਸ min(x, scalar_ਉੱਤਲਾ_bound);
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t  M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> min(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x1, ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x2)
{
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> m;

	ਲਈ (size_t i = 0; i < M; i++) {
		ਲਈ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x1(i, j), x2(i, j));
		}
	}

	ਵਾਪਸ m;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t  M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> max(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x, ਅਚਲ Type scalar_ਹੇਠਲਾ_bound)
{
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> m;

	ਲਈ (size_t i = 0; i < M; i++) {
		ਲਈ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x(i, j), scalar_ਹੇਠਲਾ_bound);
		}
	}

	ਵਾਪਸ m;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t  M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> max(ਅਚਲ Type scalar_ਹੇਠਲਾ_bound, ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x)
{
	ਵਾਪਸ max(x, scalar_ਹੇਠਲਾ_bound);
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t  M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> max(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x1, ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x2)
{
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> m;

	ਲਈ (size_t i = 0; i < M; i++) {
		ਲਈ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x1(i, j), x2(i, j));
		}
	}

	ਵਾਪਸ m;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t  M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> constrain(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x,
			     ਅਚਲ Type scalar_ਹੇਠਲਾ_bound,
			     ਅਚਲ Type scalar_ਉੱਤਲਾ_bound)
{
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> m;

	ਜੇ (scalar_ਹੇਠਲਾ_bound > scalar_ਉੱਤਲਾ_bound) {
		m.ਸੈੱਟ_Na_N();

	} ਨਹੀਂਤਾਂ {
		ਲਈ (size_t i = 0; i < M; i++) {
			ਲਈ (size_t j = 0; j < N; j++) {
				m(i, j) = typeFunction::constrain(x(i, j), scalar_ਹੇਠਲਾ_bound, scalar_ਉੱਤਲਾ_bound);
			}
		}
	}

	ਵਾਪਸ m;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t  M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> constrain(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x,
			     ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x_ਹੇਠਲਾ_bound,
			     ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &x_ਉੱਤਲਾ_bound)
{
	ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> m;

	ਲਈ (size_t i = 0; i < M; i++) {
		ਲਈ (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::constrain(x(i, j), x_ਹੇਠਲਾ_bound(i, j), x_ਉੱਤਲਾ_bound(i, j));
		}
	}

	ਵਾਪਸ m;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ OStream, ਕਿਸਮ_ਨਾਂ Type, size_t M, size_t N>
OStream &ਸੰਚਾਲਕ<<(OStream &os, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<Type, M, N> &ਮੈਟ੍ਰਿਕਸ)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * N * M + M + 1;
	ਅੱਖਰ string[n];
	ਮੈਟ੍ਰਿਕਸ.write_string(string, n);
	os << string;
	ਵਾਪਸ os;
}

} // namespace matrix
