/**
 * @file Matrix.hpp
 *
 * A simple matrix template library.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#رهنمود once

#ضمن <cmath>
#ضمن <cstdio>
#ضمن <cstring>

#ضمن "helper_functions.hpp"
#ضمن "Slice.hpp"

نطاق ماتریس
{

قالب<نام_نوع Type, size_t M, size_t N>
صنف ماتریس_2
{
	Type داده_3[M][N] {};

عام:

	// Constructors
	ماتریس_2() = افتراضي;

	صریح ماتریس_2(ثابت Type داده_2[M * N])
	{
		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				داده_3[i][j] = داده_2[N * i + j];
			}
		}
	}

	صریح ماتریس_2(ثابت Type داده_2[M][N])
	{
		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				داده_3[i][j] = داده_2[i][j];
			}
		}
	}

	ماتریس_2(ثابت ماتریس_2 &other)
	{
		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				داده_3[i][j] = other(i, j);
			}
		}
	}

	قالب<نام_نوع S>
	ماتریس_2(ثابت ماتریس_2<S, M, N> &aa)
	{
		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				داده_3[i][j] = static_cast<Type>(aa(i, j));
			}
		}
	}

	قالب<size_t P, size_t Q>
	ماتریس_2(ثابت برش_2<Type, M, N, P, Q> &درون_برش)
	{
		ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) = درون_برش(i, j);
			}
		}
	}

	قالب<size_t P, size_t Q>
	ماتریس_2(ثابت Const_برش<Type, M, N, P, Q> &درون_برش)
	{
		ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) = درون_برش(i, j);
			}
		}
	}

	/**
	 * Accessors/ Assignment etc.
	 */


	مضمن ثابت Type &عملگر()(size_t i, size_t j) ثابت
	{
		أكد(i < M);
		أكد(j < N);

		أرجع داده_3[i][j];
	}

	مضمن Type &عملگر()(size_t i, size_t j)
	{
		أكد(i < M);
		أكد(j < N);

		أرجع داده_3[i][j];
	}

	ماتریس_2<Type, M, N> &عملگر=(ثابت ماتریس_2<Type, M, N> &other)
	{
		إذا (هذا != &other) {
			ماتریس_2<Type, M, N> &self = *هذا;

			لكل (size_t i = 0; i < M; i++) {
				لكل (size_t j = 0; j < N; j++) {
					self(i, j) = other(i, j);
				}
			}
		}

		أرجع (*هذا);
	}

	فراغ copy_مقصد(Type dst[M * N]) ثابت
	{
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				dst[N * i + j] = self(i, j);
			}
		}
	}

	فراغ copy_مقصد_ستونی_Major(Type dst[M * N]) ثابت
	{
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

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
	قالب<size_t P>
	ماتریس_2<Type, M, P> عملگر*(ثابت ماتریس_2<Type, N, P> &other) ثابت
	{
		ثابت ماتریس_2<Type, M, N> &self = *هذا;
		ماتریس_2<Type, M, P> res{};

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
	قالب<size_t P>
	ماتریس_2<Type, M, M> multiplyByTranspose(ثابت ماتریس_2<Type, P, N> &other) ثابت
	{
		ماتریس_2<Type, M, P> res;
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

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
	ماتریس_2<Type, M, N> emult(ثابت ماتریس_2<Type, M, N> &other) ثابت
	{
		ماتریس_2<Type, M, N> res;
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * other(i, j);
			}
		}

		أرجع res;
	}

	// Element-wise division
	ماتریس_2<Type, M, N> edivide(ثابت ماتریس_2<Type, M, N> &other) ثابت
	{
		ماتریس_2<Type, M, N> res;
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) / other(i, j);
			}
		}

		أرجع res;
	}

	ماتریس_2<Type, M, N> عملگر+(ثابت ماتریس_2<Type, M, N> &other) ثابت
	{
		ماتریس_2<Type, M, N> res;
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + other(i, j);
			}
		}

		أرجع res;
	}

	ماتریس_2<Type, M, N> عملگر-(ثابت ماتریس_2<Type, M, N> &other) ثابت
	{
		ماتریس_2<Type, M, N> res;
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) - other(i, j);
			}
		}

		أرجع res;
	}

	// unary minus
	ماتریس_2<Type, M, N> عملگر-() ثابت
	{
		ماتریس_2<Type, M, N> res;
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = -self(i, j);
			}
		}

		أرجع res;
	}

	فراغ عملگر+=(ثابت ماتریس_2<Type, M, N> &other)
	{
		ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) += other(i, j);
			}
		}
	}

	فراغ عملگر-=(ثابت ماتریس_2<Type, M, N> &other)
	{
		ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) -= other(i, j);
			}
		}
	}

	قالب<size_t P>
	فراغ عملگر*=(ثابت ماتریس_2<Type, N, P> &other)
	{
		ماتریس_2<Type, M, N> &self = *هذا;
		self = self * other;
	}

	/**
	 * Scalar Operations
	 */

	ماتریس_2<Type, M, N> عملگر*(Type scalar) ثابت
	{
		ماتریس_2<Type, M, N> res;
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * scalar;
			}
		}

		أرجع res;
	}

	مضمن ماتریس_2<Type, M, N> عملگر/(Type scalar) ثابت
	{
		أرجع (*هذا) * (1 / scalar);
	}

	ماتریس_2<Type, M, N> عملگر+(Type scalar) ثابت
	{
		ماتریس_2<Type, M, N> res;
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + scalar;
			}
		}

		أرجع res;
	}

	مضمن ماتریس_2<Type, M, N> عملگر-(Type scalar) ثابت
	{
		أرجع (*هذا) + (-1 * scalar);
	}

	فراغ عملگر*=(Type scalar)
	{
		ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) *= scalar;
			}
		}
	}

	فراغ عملگر/=(Type scalar)
	{
		ماتریس_2<Type, M, N> &self = *هذا;
		self *= (Type(1) / scalar);
	}

	مضمن فراغ عملگر+=(Type scalar)
	{
		ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) += scalar;
			}
		}
	}

	مضمن فراغ عملگر-=(Type scalar)
	{
		ماتریس_2<Type, M, N> &self = *هذا;
		self += (-scalar);
	}

	منطقي عملگر==(ثابت ماتریس_2<Type, M, N> &other) ثابت
	{
		أرجع isEqual(*هذا, other);
	}

	منطقي عملگر!=(ثابت ماتریس_2<Type, M, N> &other) ثابت
	{
		ثابت ماتریس_2<Type, M, N> &self = *هذا;
		أرجع !(self == other);
	}

	/**
	 * Misc. Functions
	 */

	فراغ write_string(حرف *buf, size_t n) ثابت
	{
		buf[0] = '\0'; // make an empty string to begin with (we need the '\0' for strlen to work)
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				snprintf(buf + طول_رشته(buf), n - طول_رشته(buf), "\t%8.8g", مزدوج(self(i, j))); // directly append to the string buffer
			}

			snprintf(buf + طول_رشته(buf), n - طول_رشته(buf), "\n");
		}
	}

	فراغ print(عائم eps = 1e-9) ثابت
	{
		// print column numbering
		إذا (N > 1) {
			چاپ_کن("  ");

			لكل (بیعلامت i = 0; i < N; i++) {
				چاپ_کن("|%2u      ", i);

			}

			چاپ_کن("\n");
		}

		ثابت ماتریس_2<Type, M, N> &self = *هذا;
		منطقي is_prev_متقارن = درست; // assume symmetric until one element is not

		لكل (بیعلامت i = 0; i < M; i++) {
			چاپ_کن("%2u|", i); // print row numbering

			لكل (بیعلامت j = 0; j < N; j++) {
				مزدوج d = static_cast<مزدوج>(self(i, j));

				// if symmetric don't print upper triangular elements
				إذا (is_prev_متقارن && (M == N) && (j > i) && (i < N) && (j < M)
				    && (fabs(d - static_cast<مزدوج>(self(j, i))) < (مزدوج)eps)
				   ) {
					// print empty space
					چاپ_کن("         ");

				} وإلا {
					// avoid -0.0 for display
					إذا (fabs(d - 0.0) < (مزدوج)eps) {
						// print fixed width zero
						چاپ_کن(" 0       ");

					} وإلا إذا ((fabs(d) < 1e-4) || (fabs(d) >= 10.0)) {
						چاپ_کن("% .1e ", d);

					} وإلا {
						چاپ_کن("% 6.5f ", d);
					}

					is_prev_متقارن = خطأ; // not symmetric if once inside here
				}
			}

			چاپ_کن("\n");
		}
	}

	ماتریس_2<Type, N, M> transpose() ثابت
	{
		ماتریس_2<Type, N, M> res;
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				res(j, i) = self(i, j);
			}
		}

		أرجع res;
	}

	// tranpose alias
	مضمن ماتریس_2<Type, N, M> T() ثابت
	{
		أرجع transpose();
	}

	قالب<size_t P, size_t Q>
	Const_برش<Type, P, Q, M, N> برش(size_t x0, size_t y0) ثابت
	{
		أرجع {x0, y0, هذا};
	}

	قالب<size_t P, size_t Q>
	برش_2<Type, P, Q, M, N> برش(size_t x0, size_t y0)
	{
		أرجع {x0, y0, هذا};
	}

	Const_برش<Type, 1, N, M, N> ردیف(size_t i) ثابت
	{
		أرجع برش<1, N>(i, 0);
	}

	برش_2<Type, 1, N, M, N> ردیف(size_t i)
	{
		أرجع برش<1, N>(i, 0);
	}

	Const_برش<Type, M, 1, M, N> ستون(size_t j) ثابت
	{
		أرجع برش<M, 1>(0, j);
	}

	برش_2<Type, M, 1, M, N> ستون(size_t j)
	{
		أرجع برش<M, 1>(0, j);
	}

	فراغ مجموعه_ردیف(size_t i, ثابت ماتریس_2<Type, N, 1> &ردیف_درون)
	{
		برش<1, N>(i, 0) = ردیف_درون.transpose();
	}

	فراغ مجموعه_ردیف(size_t i, Type val)
	{
		برش<1, N>(i, 0) = val;
	}

	فراغ مجموعه_ستون(size_t j, ثابت ماتریس_2<Type, M, 1> &ستونی)
	{
		برش<M, 1>(0, j) = ستونی;
	}

	فراغ مجموعه_ستون(size_t j, Type val)
	{
		برش<M, 1>(0, j) = val;
	}

	فراغ مجموعه_Zero()
	{
		memset(داده_3, 0, حجم(داده_3));
	}

	مضمن فراغ zero()
	{
		مجموعه_Zero();
	}

	فراغ مجموعه_All(Type val)
	{
		ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				self(i, j) = val;
			}
		}
	}

	مضمن فراغ مجموعه_One()
	{
		مجموعه_All(1);
	}

	مضمن فراغ مجموعه_Na_N()
	{
		مجموعه_All(NAN);
	}

	فراغ مجموعه_عنصر_خنثی()
	{
		مجموعه_Zero();
		ماتریس_2<Type, M, N> &self = *هذا;

		ثابت size_t min_i = M > N ? N : M;

		لكل (size_t i = 0; i < min_i; i++) {
			self(i, i) = 1;
		}
	}

	مضمن فراغ عنصر_خنثی()
	{
		مجموعه_عنصر_خنثی();
	}

	مضمن فراغ جابجا_ردیفها(size_t a, size_t b)
	{
		أكد(a < M);
		أكد(b < M);

		إذا (a == b) {
			أرجع;
		}

		ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t j = 0; j < N; j++) {
			Type tmp = self(a, j);
			self(a, j) = self(b, j);
			self(b, j) = tmp;
		}
	}

	مضمن فراغ جابجا_تعداد_ستون(size_t a, size_t b)
	{
		أكد(a < N);
		أكد(b < N);

		إذا (a == b) {
			أرجع;
		}

		ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			Type tmp = self(i, a);
			self(i, a) = self(i, b);
			self(i, b) = tmp;
		}
	}

	مضمن فراغ nan_مقصد_Zero()
	{

		ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				إذا (std::isnan(self(i, j))) {
					self(i, j) = static_cast<Type>(0.0f);
				}
			}
		}
	}

	ماتریس_2<Type, M, N> قدر_مطلق() ثابت
	{
		ماتریس_2<Type, M, N> r;

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
		ثابت ماتریس_2<Type, M, N> &self = *هذا;
		منطقي نتیجه = درست;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				نتیجه = نتیجه && std::isnan(self(i, j));
			}
		}

		أرجع نتیجه;
	}

	منطقي isAllFinite() ثابت
	{
		ثابت ماتریس_2<Type, M, N> &self = *هذا;

		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				إذا (!std::isfinite(self(i, j))) {
					أرجع خطأ;
				}
			}
		}

		أرجع درست;
	}
};

قالب<نام_نوع Type, size_t M, size_t N>
ماتریس_2<Type, M, N> zeros()
{
	ماتریس_2<Type, M, N> m;
	m.مجموعه_Zero();
	أرجع m;
}

قالب<نام_نوع Type, size_t M, size_t N>
ماتریس_2<Type, M, N> ones()
{
	ماتریس_2<Type, M, N> m;
	m.مجموعه_One();
	أرجع m;
}

قالب<size_t M, size_t N>
ماتریس_2<عائم, M, N> nans()
{
	ماتریس_2<عائم, M, N> m;
	m.مجموعه_Na_N();
	أرجع m;
}

قالب<نام_نوع Type, size_t  M, size_t N>
ماتریس_2<Type, M, N> عملگر*(Type scalar, ثابت ماتریس_2<Type, M, N> &other)
{
	أرجع other * scalar;
}

قالب<نام_نوع Type, size_t  M, size_t N>
منطقي isEqual(ثابت ماتریس_2<Type, M, N> &x,
	     ثابت ماتریس_2<Type, M, N> &y, ثابت Type eps = Type(1e-4f))
{
	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			إذا (!isEqualF(x(i, j), y(i, j), eps)) {
				أرجع خطأ;
			}
		}
	}

	أرجع درست;
}

نطاق typeFunction
{
قالب<نام_نوع Type>
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

قالب<نام_نوع Type>
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

قالب<نام_نوع Type>
Type constrain(ثابت Type x, ثابت Type پایینی_bound, ثابت Type فوقانی_bound)
{
	إذا (پایینی_bound > فوقانی_bound) {
		أرجع NAN;

	} وإلا إذا (std::isnan(x)) {
		أرجع NAN;

	} وإلا {
		أرجع typeFunction::max(پایینی_bound, typeFunction::min(فوقانی_bound, x));
	}
}
}

قالب<نام_نوع Type, size_t  M, size_t N>
ماتریس_2<Type, M, N> min(ثابت ماتریس_2<Type, M, N> &x, ثابت Type scalar_فوقانی_bound)
{
	ماتریس_2<Type, M, N> m;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x(i, j), scalar_فوقانی_bound);
		}
	}

	أرجع m;
}

قالب<نام_نوع Type, size_t  M, size_t N>
ماتریس_2<Type, M, N> min(ثابت Type scalar_فوقانی_bound, ثابت ماتریس_2<Type, M, N> &x)
{
	أرجع min(x, scalar_فوقانی_bound);
}

قالب<نام_نوع Type, size_t  M, size_t N>
ماتریس_2<Type, M, N> min(ثابت ماتریس_2<Type, M, N> &x1, ثابت ماتریس_2<Type, M, N> &x2)
{
	ماتریس_2<Type, M, N> m;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x1(i, j), x2(i, j));
		}
	}

	أرجع m;
}

قالب<نام_نوع Type, size_t  M, size_t N>
ماتریس_2<Type, M, N> max(ثابت ماتریس_2<Type, M, N> &x, ثابت Type scalar_پایینی_bound)
{
	ماتریس_2<Type, M, N> m;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x(i, j), scalar_پایینی_bound);
		}
	}

	أرجع m;
}

قالب<نام_نوع Type, size_t  M, size_t N>
ماتریس_2<Type, M, N> max(ثابت Type scalar_پایینی_bound, ثابت ماتریس_2<Type, M, N> &x)
{
	أرجع max(x, scalar_پایینی_bound);
}

قالب<نام_نوع Type, size_t  M, size_t N>
ماتریس_2<Type, M, N> max(ثابت ماتریس_2<Type, M, N> &x1, ثابت ماتریس_2<Type, M, N> &x2)
{
	ماتریس_2<Type, M, N> m;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x1(i, j), x2(i, j));
		}
	}

	أرجع m;
}

قالب<نام_نوع Type, size_t  M, size_t N>
ماتریس_2<Type, M, N> constrain(ثابت ماتریس_2<Type, M, N> &x,
			     ثابت Type scalar_پایینی_bound,
			     ثابت Type scalar_فوقانی_bound)
{
	ماتریس_2<Type, M, N> m;

	إذا (scalar_پایینی_bound > scalar_فوقانی_bound) {
		m.مجموعه_Na_N();

	} وإلا {
		لكل (size_t i = 0; i < M; i++) {
			لكل (size_t j = 0; j < N; j++) {
				m(i, j) = typeFunction::constrain(x(i, j), scalar_پایینی_bound, scalar_فوقانی_bound);
			}
		}
	}

	أرجع m;
}

قالب<نام_نوع Type, size_t  M, size_t N>
ماتریس_2<Type, M, N> constrain(ثابت ماتریس_2<Type, M, N> &x,
			     ثابت ماتریس_2<Type, M, N> &x_پایینی_bound,
			     ثابت ماتریس_2<Type, M, N> &x_فوقانی_bound)
{
	ماتریس_2<Type, M, N> m;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::constrain(x(i, j), x_پایینی_bound(i, j), x_فوقانی_bound(i, j));
		}
	}

	أرجع m;
}

قالب<نام_نوع OStream, نام_نوع Type, size_t M, size_t N>
OStream &عملگر<<(OStream &os, ثابت ماتریس::ماتریس_2<Type, M, N> &ماتریس)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * N * M + M + 1;
	حرف string[n];
	ماتریس.write_string(string, n);
	os << string;
	أرجع os;
}

} // namespace matrix
