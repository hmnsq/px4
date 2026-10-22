/**
 * @file Slice.hpp
 *
 * A simple matrix template library.
 *
 * @author Julian Kent < julian@auterion.com >
 */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <cassert>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdio>
#ਅਨ੍ਤਰ੍ਭਾਵ <cmath>

ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ
{

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t M, size_t N>
ਵਰਗ ਮੈਟ੍ਰਿਕਸ_2;

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t M>
ਵਰਗ ਸਦਿਸ਼;

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ਮੈਟ੍ਰਿਕਸ_T, ਕਿਸਮ_ਨਾਂ Type, size_t P, size_t Q, size_t M, size_t N>
ਵਰਗ ਟੁਕੜਾ_T
{
ਜਨਤਕ:
	ਵਰਤੋਂ Self = ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N>;

	ਟੁਕੜਾ_T(size_t x0, size_t y0, ਮੈਟ੍ਰਿਕਸ_T *ਅੰਕੜੇ) :
		_x0(x0),
		_y0(y0),
		ਅੰਕੜੇ_3(ਅੰਕੜੇ)
	{
		static_assert(P <= M, "Slice rows bigger than backing matrix");
		static_assert(Q <= N, "Slice cols bigger than backing matrix");
		ਪ੍ਰਤਿਜ੍ਞਾ(x0 + P <= M);
		ਪ੍ਰਤਿਜ੍ਞਾ(y0 + Q <= N);
	}

	ਟੁਕੜਾ_T(ਅਚਲ Self &other) = ਮੂਲ;

	ਅਚਲ Type &ਸੰਚਾਲਕ()(size_t i, size_t j) ਅਚਲ
	{
		ਪ੍ਰਤਿਜ੍ਞਾ(i < P);
		ਪ੍ਰਤਿਜ੍ਞਾ(j < Q);

		ਵਾਪਸ (*ਅੰਕੜੇ_3)(_x0 + i, _y0 + j);
	}

	Type &ਸੰਚਾਲਕ()(size_t i, size_t j)
	{
		ਪ੍ਰਤਿਜ੍ਞਾ(i < P);
		ਪ੍ਰਤਿਜ੍ਞਾ(j < Q);

		ਵਾਪਸ (*ਅੰਕੜੇ_3)(_x0 + i, _y0 + j);
	}

	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &ਸੰਚਾਲਕ=(ਅਚਲ Self &other)
	{
		// Protect against self-assignment
		ਜੇ (ਇਹ == &other) {
			ਵਾਪਸ *ਇਹ;
		}

		ਵਾਪਸ ਇਹ->ਸੰਚਾਲਕ=<M, N>(other);
	}

	ਸਾਂਚਾ<size_t MM, size_t NN>
	Self &ਸੰਚਾਲਕ=(ਅਚਲ ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_2<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ਵਾਪਸ self;
	}

	ਸਾਂਚਾ<size_t MM, size_t NN>
	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &ਸੰਚਾਲਕ=(ਅਚਲ ਟੁਕੜਾ_T<ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ਵਾਪਸ self;
	}

	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &ਸੰਚਾਲਕ=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> &other)
	{
		ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		ਵਾਪਸ self;
	}

	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &ਸੰਚਾਲਕ=(ਅਚਲ Type &other)
	{
		ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		ਵਾਪਸ self;
	}

	ਸਾਂਚਾ<size_t MM, size_t NN>
	ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> ਸੰਚਾਲਕ-(ਅਚਲ ਟੁਕੜਾ_T<ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> {*ਇਹ} - other;
	}


	ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> ਸੰਚਾਲਕ-(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> &other)
	{
		ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> {*ਇਹ} - other;
	}

	ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> ਸੰਚਾਲਕ-(ਅਚਲ Type &other)
	{
		ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> {*ਇਹ} - other;
	}

	ਸਾਂਚਾ<size_t MM, size_t NN>
	ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> ਸੰਚਾਲਕ+(ਅਚਲ ਟੁਕੜਾ_T<ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> {*ਇਹ} + other;
	}

	ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> ਸੰਚਾਲਕ+(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> &other)
	{
		ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> {*ਇਹ} + other;
	}

	ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> ਸੰਚਾਲਕ+(ਅਚਲ Type &other)
	{
		ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> {*ਇਹ} + other;
	}

	// allow assigning vectors to a slice that are in the axis
	ਸਾਂਚਾ <size_t DUMMY = 1> // make this a template function since it only exists for some instantiations
	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, 1, Q, M, N> &ਸੰਚਾਲਕ=(ਅਚਲ ਸਦਿਸ਼<Type, Q> &other)
	{
		ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, 1, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		ਵਾਪਸ self;
	}

	ਸਾਂਚਾ<size_t MM, size_t NN>
	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &ਸੰਚਾਲਕ+=(ਅਚਲ ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, MM, NN> &other)
	{
		ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		ਵਾਪਸ self;
	}

	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &ਸੰਚਾਲਕ+=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> &other)
	{
		ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		ਵਾਪਸ self;
	}

	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &ਸੰਚਾਲਕ+=(ਅਚਲ Type &other)
	{
		ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		ਵਾਪਸ self;
	}

	ਸਾਂਚਾ<size_t MM, size_t NN>
	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &ਸੰਚਾਲਕ-=(ਅਚਲ ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, MM, NN> &other)
	{
		ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		ਵਾਪਸ self;
	}

	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &ਸੰਚਾਲਕ-=(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> &other)
	{
		ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		ਵਾਪਸ self;
	}

	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &ਸੰਚਾਲਕ-=(ਅਚਲ Type &other)
	{
		ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		ਵਾਪਸ self;
	}

	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &ਸੰਚਾਲਕ*=(ਅਚਲ Type &other)
	{
		ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		ਵਾਪਸ self;
	}

	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &ਸੰਚਾਲਕ/=(ਅਚਲ Type &scalar)
	{
		ਵਾਪਸ ਸੰਚਾਲਕ*=(Type(1) / scalar);
	}

	ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> ਸੰਚਾਲਕ*(ਅਚਲ Type &scalar) ਅਚਲ
	{
		ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> {*ਇਹ} * scalar;
	}

	ਮੈਟ੍ਰਿਕਸ_2<Type, P, Q> ਸੰਚਾਲਕ/(ਅਚਲ Type &scalar) ਅਚਲ
	{
		ਵਾਪਸ (*ਇਹ) * (1 / scalar);
	}

	ਸਾਂਚਾ<size_t R, size_t S>
	ਅਚਲ ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, R, S, M, N> ਟੁਕੜਾ(size_t x0, size_t y0) ਅਚਲ
	{
		ਵਾਪਸ ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, R, S, M, N>(x0 + _x0, y0 + _y0, ਅੰਕੜੇ_3);
	}

	ਸਾਂਚਾ<size_t R, size_t S>
	ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, R, S, M, N> ਟੁਕੜਾ(size_t x0, size_t y0)
	{
		ਵਾਪਸ ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, R, S, M, N>(x0 + _x0, y0 + _y0, ਅੰਕੜੇ_3);
	}

	ਖਾਲੀ copy_ਮੰਜ਼ਿਲ(Type dst[P * Q]) ਅਚਲ
	{
		ਅਚਲ ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	ਖਾਲੀ copy_ਮੰਜ਼ਿਲ_ਕਾਲਮ_Major(Type dst[P * Q]) ਅਚਲ
	{
		ਅਚਲ ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	ਸਦਿਸ਼ < Type, P < Q ? P : Q > diag() ਅਚਲ
	{
		ਅਚਲ ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;
		ਸਦਿਸ਼ < Type, P < Q ? P : Q > res;

		ਲਈ (size_t j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		ਵਾਪਸ res;
	}

	ਸਾਂਚਾ<size_t MM, size_t NN>
	Type dot(ਅਚਲ ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, MM, NN> &other) ਅਚਲ
	{
		ਅਚਲ ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_T, Type, P, Q, M, N> &self = *ਇਹ;
		Type accum(0);

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				accum += self(i, j) * other(i, j);
			}
		}

		ਵਾਪਸ accum;
	}

	Type norm_squared() ਅਚਲ
	{
		ਵਾਪਸ dot(*ਇਹ);
	}

	Type norm() ਅਚਲ
	{
		ਵਾਪਸ std::ਵਰਗਮੂਲ(norm_squared());
	}

	ਤਾਰਕਿਕ longerThan(Type testVal) ਅਚਲ
	{
		ਵਾਪਸ norm_squared() > testVal * testVal;
	}

	Type max() ਅਚਲ
	{
		Type max_val = (*ਇਹ)(0, 0);

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
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

		ਲਈ (size_t i = 0; i < P; i++) {
			ਲਈ (size_t j = 0; j < Q; j++) {
				Type val = (*ਇਹ)(i, j);

				ਜੇ (val < min_val) {
					min_val = val;
				}
			}
		}

		ਵਾਪਸ min_val;
	}

ਨਿਜੀ:
	size_t _x0, _y0;
	ਮੈਟ੍ਰਿਕਸ_T *ਅੰਕੜੇ_3;
};

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Type, size_t P, size_t Q, size_t M, size_t N>
ਵਰਤੋਂ ਟੁਕੜਾ_2 = ਟੁਕੜਾ_T<ਮੈਟ੍ਰਿਕਸ_2<Type, M, N>, Type, P, Q, M, N>;

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Type, size_t P, size_t Q, size_t M, size_t N>
ਵਰਤੋਂ Const_ਟੁਕੜਾ = ਟੁਕੜਾ_T<ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, M, N>, Type, P, Q, M, N>;

}
