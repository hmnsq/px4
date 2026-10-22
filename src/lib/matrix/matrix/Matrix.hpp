/**
 * @file Matrix.hpp
 *
 * A simple matrix template library.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#هدايت once

#ضمن <cmath>
#ضمن <cstdio>
#ضمن <cstring>

#ضمن "helper_functions.hpp"
#ضمن "Slice.hpp"

نطاق ميٽرڪس
{

سانچو<قسم_نالو Type, size_t M, size_t N>
صنف ميٽرڪس_2
{
	Type معلومات_3[M][N] {};

عام:

	// Constructors
	ميٽرڪس_2() = افتراضي;

	واضح ميٽرڪس_2(ثابت Type معلومات_2[M * N])
	{
		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				معلومات_3[i][j] = معلومات_2[N * i + j];
			}
		}
	}

	واضح ميٽرڪس_2(ثابت Type معلومات_2[M][N])
	{
		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				معلومات_3[i][j] = معلومات_2[i][j];
			}
		}
	}

	ميٽرڪس_2(ثابت ميٽرڪس_2 &other)
	{
		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				معلومات_3[i][j] = other(i, j);
			}
		}
	}

	سانچو<قسم_نالو S>
	ميٽرڪس_2(ثابت ميٽرڪس_2<S, M, N> &aa)
	{
		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				معلومات_3[i][j] = static_cast<Type>(aa(i, j));
			}
		}
	}

	سانچو<size_t P, size_t Q>
	ميٽرڪس_2(ثابت ٽڪرو_2<Type, M, N, P, Q> &اندريون_ٽڪرو)
	{
		ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) = اندريون_ٽڪرو(i, j);
			}
		}
	}

	سانچو<size_t P, size_t Q>
	ميٽرڪس_2(ثابت Const_ٽڪرو<Type, M, N, P, Q> &اندريون_ٽڪرو)
	{
		ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) = اندريون_ٽڪرو(i, j);
			}
		}
	}

	/**
	 * Accessors/ Assignment etc.
	 */


	مضمن ثابت Type &عامل()(size_t i, size_t j) ثابت
	{
		أكد(i < M);
		أكد(j < N);

		أرجع معلومات_3[i][j];
	}

	مضمن Type &عامل()(size_t i, size_t j)
	{
		أكد(i < M);
		أكد(j < N);

		أرجع معلومات_3[i][j];
	}

	ميٽرڪس_2<Type, M, N> &عامل=(ثابت ميٽرڪس_2<Type, M, N> &other)
	{
		إذا (هذا != &other) {
			ميٽرڪس_2<Type, M, N> &self = *هذا;

			لكل (size_t i = 0; i < M; i++) {
				لكل (size_t j = 0; j < N; j++) {
					self(i, j) = other(i, j);
				}
			}
		}

		أرجع (*هذا);
	}

	فراغ copy_منزل(Type dst[M * N]) ثابت
	{
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				dst[N * i + j] = self(i, j);
			}
		}
	}

	فراغ copy_منزل_ڪالم_Major(Type dst[M * N]) ثابت
	{
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
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
	سانچو<size_t P>
	ميٽرڪس_2<Type, M, P> عامل*(ثابت ميٽرڪس_2<Type, N, P> &other) ثابت
	{
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;
		ميٽرڪس_2<Type, M, P> res{};

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t k = 0; k < P; k++) {
				لكل (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(j, k);
				}
			}
		}

		أرجع res;
	}

	// Using this function reduces the number of temporary variables needed to compute A * B.T
	سانچو<size_t P>
	ميٽرڪس_2<Type, M, M> multiplyByTranspose(ثابت ميٽرڪس_2<Type, P, N> &other) ثابت
	{
		ميٽرڪس_2<Type, M, P> res;
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t k = 0; k < P; k++) {
				لكل (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(k, j);
				}
			}
		}

		أرجع res;
	}

	// Element-wise multiplication
	ميٽرڪس_2<Type, M, N> emult(ثابت ميٽرڪس_2<Type, M, N> &other) ثابت
	{
		ميٽرڪس_2<Type, M, N> res;
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * other(i, j);
			}
		}

		أرجع res;
	}

	// Element-wise division
	ميٽرڪس_2<Type, M, N> edivide(ثابت ميٽرڪس_2<Type, M, N> &other) ثابت
	{
		ميٽرڪس_2<Type, M, N> res;
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) / other(i, j);
			}
		}

		أرجع res;
	}

	ميٽرڪس_2<Type, M, N> عامل+(ثابت ميٽرڪس_2<Type, M, N> &other) ثابت
	{
		ميٽرڪس_2<Type, M, N> res;
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + other(i, j);
			}
		}

		أرجع res;
	}

	ميٽرڪس_2<Type, M, N> عامل-(ثابت ميٽرڪس_2<Type, M, N> &other) ثابت
	{
		ميٽرڪس_2<Type, M, N> res;
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) - other(i, j);
			}
		}

		أرجع res;
	}

	// unary minus
	ميٽرڪس_2<Type, M, N> عامل-() ثابت
	{
		ميٽرڪس_2<Type, M, N> res;
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = -self(i, j);
			}
		}

		أرجع res;
	}

	فراغ عامل+=(ثابت ميٽرڪس_2<Type, M, N> &other)
	{
		ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) += other(i, j);
			}
		}
	}

	فراغ عامل-=(ثابت ميٽرڪس_2<Type, M, N> &other)
	{
		ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) -= other(i, j);
			}
		}
	}

	سانچو<size_t P>
	فراغ عامل*=(ثابت ميٽرڪس_2<Type, N, P> &other)
	{
		ميٽرڪس_2<Type, M, N> &self = *هذا;
		self = self * other;
	}

	/**
	 * Scalar Operations
	 */

	ميٽرڪس_2<Type, M, N> عامل*(Type scalar) ثابت
	{
		ميٽرڪس_2<Type, M, N> res;
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * scalar;
			}
		}

		أرجع res;
	}

	مضمن ميٽرڪس_2<Type, M, N> عامل/(Type scalar) ثابت
	{
		أرجع (*هذا) * (1 / scalar);
	}

	ميٽرڪس_2<Type, M, N> عامل+(Type scalar) ثابت
	{
		ميٽرڪس_2<Type, M, N> res;
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + scalar;
			}
		}

		أرجع res;
	}

	مضمن ميٽرڪس_2<Type, M, N> عامل-(Type scalar) ثابت
	{
		أرجع (*هذا) + (-1 * scalar);
	}

	فراغ عامل*=(Type scalar)
	{
		ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) *= scalar;
			}
		}
	}

	فراغ عامل/=(Type scalar)
	{
		ميٽرڪس_2<Type, M, N> &self = *هذا;
		self *= (Type(1) / scalar);
	}

	مضمن فراغ عامل+=(Type scalar)
	{
		ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) += scalar;
			}
		}
	}

	مضمن فراغ عامل-=(Type scalar)
	{
		ميٽرڪس_2<Type, M, N> &self = *هذا;
		self += (-scalar);
	}

	منطقي عامل==(ثابت ميٽرڪس_2<Type, M, N> &other) ثابت
	{
		أرجع isEqual(*هذا, other);
	}

	منطقي عامل!=(ثابت ميٽرڪس_2<Type, M, N> &other) ثابت
	{
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;
		أرجع !(self == other);
	}

	/**
	 * Misc. Functions
	 */

	فراغ write_string(حرف *buf, size_t n) ثابت
	{
		buf[0] = '\0'; // make an empty string to begin with (we need the '\0' for strlen to work)
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				snprintf(buf + لڙي_ڊيگهه(buf), n - لڙي_ڊيگهه(buf), "\t%8.8g", مزدوج(self(i, j))); // directly append to the string buffer
			}

			snprintf(buf + لڙي_ڊيگهه(buf), n - لڙي_ڊيگهه(buf), "\n");
		}
	}

	فراغ print(عائم eps = 1e-9) ثابت
	{
		// print column numbering
		إذا (N > 1) {
			لکو("  ");

			لكل (بنانشان i = 0; i < N; i++) {
				لکو("|%2u      ", i);

			}

			لکو("\n");
		}

		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;
		منطقي is_prev_متناظر = سچ; // assume symmetric until one element is not

		لكل (بنانشان i = 0; i < M; i++) {
			لکو("%2u|", i); // print row numbering

			لكل (بنانشان j = 0; j < N; j++) {
				مزدوج d = static_cast<مزدوج>(self(i, j));

				// if symmetric don't print upper triangular elements
				إذا (is_prev_متناظر && (M == N) && (j > i) && (i < N) && (j < M)
				    && (fabs(d - static_cast<مزدوج>(self(j, i))) < (مزدوج)eps)
				   ) {
					// print empty space
					لکو("         ");

				} وإلا {
					// avoid -0.0 for display
					إذا (fabs(d - 0.0) < (مزدوج)eps) {
						// print fixed width zero
						لکو(" 0       ");

					} وإلا إذا ((fabs(d) < 1e-4) || (fabs(d) >= 10.0)) {
						لکو("% .1e ", d);

					} وإلا {
						لکو("% 6.5f ", d);
					}

					is_prev_متناظر = خطأ; // not symmetric if once inside here
				}
			}

			لکو("\n");
		}
	}

	ميٽرڪس_2<Type, N, M> transpose() ثابت
	{
		ميٽرڪس_2<Type, N, M> res;
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(j, i) = self(i, j);
			}
		}

		أرجع res;
	}

	// tranpose alias
	مضمن ميٽرڪس_2<Type, N, M> T() ثابت
	{
		أرجع transpose();
	}

	سانچو<size_t P, size_t Q>
	Const_ٽڪرو<Type, P, Q, M, N> ٽڪرو(size_t x0, size_t y0) ثابت
	{
		أرجع {x0, y0, هذا};
	}

	سانچو<size_t P, size_t Q>
	ٽڪرو_2<Type, P, Q, M, N> ٽڪرو(size_t x0, size_t y0)
	{
		أرجع {x0, y0, هذا};
	}

	Const_ٽڪرو<Type, 1, N, M, N> صف(size_t i) ثابت
	{
		أرجع ٽڪرو<1, N>(i, 0);
	}

	ٽڪرو_2<Type, 1, N, M, N> صف(size_t i)
	{
		أرجع ٽڪرو<1, N>(i, 0);
	}

	Const_ٽڪرو<Type, M, 1, M, N> کانو(size_t j) ثابت
	{
		أرجع ٽڪرو<M, 1>(0, j);
	}

	ٽڪرو_2<Type, M, 1, M, N> کانو(size_t j)
	{
		أرجع ٽڪرو<M, 1>(0, j);
	}

	فراغ سيٽ_صف(size_t i, ثابت ميٽرڪس_2<Type, N, 1> &صف_اندريون)
	{
		ٽڪرو<1, N>(i, 0) = صف_اندريون.transpose();
	}

	فراغ سيٽ_صف(size_t i, Type val)
	{
		ٽڪرو<1, N>(i, 0) = val;
	}

	فراغ سيٽ_کانو(size_t j, ثابت ميٽرڪس_2<Type, M, 1> &ڪالم)
	{
		ٽڪرو<M, 1>(0, j) = ڪالم;
	}

	فراغ سيٽ_کانو(size_t j, Type val)
	{
		ٽڪرو<M, 1>(0, j) = val;
	}

	فراغ سيٽ_Zero()
	{
		memset(معلومات_3, 0, حجم(معلومات_3));
	}

	مضمن فراغ zero()
	{
		سيٽ_Zero();
	}

	فراغ سيٽ_All(Type val)
	{
		ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) = val;
			}
		}
	}

	مضمن فراغ سيٽ_One()
	{
		سيٽ_All(1);
	}

	مضمن فراغ سيٽ_Na_N()
	{
		سيٽ_All(NAN);
	}

	فراغ سيٽ_سڃاڻپ_عنصر()
	{
		سيٽ_Zero();
		ميٽرڪس_2<Type, M, N> &self = *هذا;

		ثابت size_t min_i = M > N ? N : M;

		لكل (size_t i = 0; i < min_i; i++) {
			self(i, i) = 1;
		}
	}

	مضمن فراغ سڃاڻپ_عنصر()
	{
		سيٽ_سڃاڻپ_عنصر();
	}

	مضمن فراغ مٽا_سٽا_صفون(size_t a, size_t b)
	{
		أكد(a < M);
		أكد(b < M);

		إذا (a == b) {
			أرجع;
		}

		ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t j = 0; j < N; j++) {
			Type tmp = self(a, j);
			self(a, j) = self(b, j);
			self(b, j) = tmp;
		}
	}

	مضمن فراغ مٽا_سٽا_کانن_ڳڻپ(size_t a, size_t b)
	{
		أكد(a < N);
		أكد(b < N);

		إذا (a == b) {
			أرجع;
		}

		ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			Type tmp = self(i, a);
			self(i, a) = self(i, b);
			self(i, b) = tmp;
		}
	}

	مضمن فراغ nan_منزل_Zero()
	{

		ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				إذا (std::isnan(self(i, j))) {
					self(i, j) = static_cast<Type>(0.0f);
				}
			}
		}
	}

	ميٽرڪس_2<Type, M, N> مطلق_قدر() ثابت
	{
		ميٽرڪس_2<Type, M, N> r;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				r(i, j) = Type(std::fabs((*هذا)(i, j)));
			}
		}

		أرجع r;
	}

	Type max() ثابت
	{
		Type max_val = (*هذا)(0, 0);

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				Type val = (*هذا)(i, j);

				إذا (val > max_val) {
					max_val = val;
				}
			}
		}

		أرجع max_val;
	}

	Type min() ثابت
	{
		Type min_val = (*هذا)(0, 0);

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				Type val = (*هذا)(i, j);

				إذا (val < min_val) {
					min_val = val;
				}
			}
		}

		أرجع min_val;
	}

	منطقي isAllNan() ثابت
	{
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;
		منطقي نتيجو = سچ;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				نتيجو = نتيجو && std::isnan(self(i, j));
			}
		}

		أرجع نتيجو;
	}

	منطقي isAllFinite() ثابت
	{
		ثابت ميٽرڪس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				إذا (!std::isfinite(self(i, j))) {
					أرجع خطأ;
				}
			}
		}

		أرجع سچ;
	}
};

سانچو<قسم_نالو Type, size_t M, size_t N>
ميٽرڪس_2<Type, M, N> zeros()
{
	ميٽرڪس_2<Type, M, N> m;
	m.سيٽ_Zero();
	أرجع m;
}

سانچو<قسم_نالو Type, size_t M, size_t N>
ميٽرڪس_2<Type, M, N> ones()
{
	ميٽرڪس_2<Type, M, N> m;
	m.سيٽ_One();
	أرجع m;
}

سانچو<size_t M, size_t N>
ميٽرڪس_2<عائم, M, N> nans()
{
	ميٽرڪس_2<عائم, M, N> m;
	m.سيٽ_Na_N();
	أرجع m;
}

سانچو<قسم_نالو Type, size_t  M, size_t N>
ميٽرڪس_2<Type, M, N> عامل*(Type scalar, ثابت ميٽرڪس_2<Type, M, N> &other)
{
	أرجع other * scalar;
}

سانچو<قسم_نالو Type, size_t  M, size_t N>
منطقي isEqual(ثابت ميٽرڪس_2<Type, M, N> &x,
	     ثابت ميٽرڪس_2<Type, M, N> &y, ثابت Type eps = Type(1e-4f))
{
	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			إذا (!isEqualF(x(i, j), y(i, j), eps)) {
				أرجع خطأ;
			}
		}
	}

	أرجع سچ;
}

نطاق typeFunction
{
سانچو<قسم_نالو Type>
Type min(ثابت Type x, ثابت Type y)
{
	منطقي x_is_nan = std::isnan(x);
	منطقي y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	إذا (x_is_nan || y_is_nan) {
		إذا (x_is_nan && !y_is_nan) {
			أرجع y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		أرجع x;
	}

	أرجع (x < y) ? x : y;
}

سانچو<قسم_نالو Type>
Type max(ثابت Type x, ثابت Type y)
{
	منطقي x_is_nan = std::isnan(x);
	منطقي y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	إذا (x_is_nan || y_is_nan) {
		إذا (x_is_nan && !y_is_nan) {
			أرجع y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		أرجع x;
	}

	أرجع (x > y) ? x : y;
}

سانچو<قسم_نالو Type>
Type constrain(ثابت Type x, ثابت Type هيٺاهون_bound, ثابت Type مٿانهون_bound)
{
	إذا (هيٺاهون_bound > مٿانهون_bound) {
		أرجع NAN;

	} وإلا إذا (std::isnan(x)) {
		أرجع NAN;

	} وإلا {
		أرجع typeFunction::max(هيٺاهون_bound, typeFunction::min(مٿانهون_bound, x));
	}
}
}

سانچو<قسم_نالو Type, size_t  M, size_t N>
ميٽرڪس_2<Type, M, N> min(ثابت ميٽرڪس_2<Type, M, N> &x, ثابت Type scalar_مٿانهون_bound)
{
	ميٽرڪس_2<Type, M, N> m;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x(i, j), scalar_مٿانهون_bound);
		}
	}

	أرجع m;
}

سانچو<قسم_نالو Type, size_t  M, size_t N>
ميٽرڪس_2<Type, M, N> min(ثابت Type scalar_مٿانهون_bound, ثابت ميٽرڪس_2<Type, M, N> &x)
{
	أرجع min(x, scalar_مٿانهون_bound);
}

سانچو<قسم_نالو Type, size_t  M, size_t N>
ميٽرڪس_2<Type, M, N> min(ثابت ميٽرڪس_2<Type, M, N> &x1, ثابت ميٽرڪس_2<Type, M, N> &x2)
{
	ميٽرڪس_2<Type, M, N> m;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x1(i, j), x2(i, j));
		}
	}

	أرجع m;
}

سانچو<قسم_نالو Type, size_t  M, size_t N>
ميٽرڪس_2<Type, M, N> max(ثابت ميٽرڪس_2<Type, M, N> &x, ثابت Type scalar_هيٺاهون_bound)
{
	ميٽرڪس_2<Type, M, N> m;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x(i, j), scalar_هيٺاهون_bound);
		}
	}

	أرجع m;
}

سانچو<قسم_نالو Type, size_t  M, size_t N>
ميٽرڪس_2<Type, M, N> max(ثابت Type scalar_هيٺاهون_bound, ثابت ميٽرڪس_2<Type, M, N> &x)
{
	أرجع max(x, scalar_هيٺاهون_bound);
}

سانچو<قسم_نالو Type, size_t  M, size_t N>
ميٽرڪس_2<Type, M, N> max(ثابت ميٽرڪس_2<Type, M, N> &x1, ثابت ميٽرڪس_2<Type, M, N> &x2)
{
	ميٽرڪس_2<Type, M, N> m;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x1(i, j), x2(i, j));
		}
	}

	أرجع m;
}

سانچو<قسم_نالو Type, size_t  M, size_t N>
ميٽرڪس_2<Type, M, N> constrain(ثابت ميٽرڪس_2<Type, M, N> &x,
			     ثابت Type scalar_هيٺاهون_bound,
			     ثابت Type scalar_مٿانهون_bound)
{
	ميٽرڪس_2<Type, M, N> m;

	إذا (scalar_هيٺاهون_bound > scalar_مٿانهون_bound) {
		m.سيٽ_Na_N();

	} وإلا {
		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				m(i, j) = typeFunction::constrain(x(i, j), scalar_هيٺاهون_bound, scalar_مٿانهون_bound);
			}
		}
	}

	أرجع m;
}

سانچو<قسم_نالو Type, size_t  M, size_t N>
ميٽرڪس_2<Type, M, N> constrain(ثابت ميٽرڪس_2<Type, M, N> &x,
			     ثابت ميٽرڪس_2<Type, M, N> &x_هيٺاهون_bound,
			     ثابت ميٽرڪس_2<Type, M, N> &x_مٿانهون_bound)
{
	ميٽرڪس_2<Type, M, N> m;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::constrain(x(i, j), x_هيٺاهون_bound(i, j), x_مٿانهون_bound(i, j));
		}
	}

	أرجع m;
}

سانچو<قسم_نالو OStream, قسم_نالو Type, size_t M, size_t N>
OStream &عامل<<(OStream &os, ثابت ميٽرڪس::ميٽرڪس_2<Type, M, N> &ميٽرڪس)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * N * M + M + 1;
	حرف string[n];
	ميٽرڪس.write_string(string, n);
	os << string;
	أرجع os;
}

} // namespace matrix
