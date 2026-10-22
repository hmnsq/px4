/**
 * @file Dual.hpp
 *
 * This is a dual number type for calculating automatic derivatives.
 *
 * Based roughly on the methods described in:
 * Automatic Differentiation, C++ Templates and Photogrammetry, by Dan Piponi
 * and
 * Ceres Solver's excellent Jet.h
 *
 * @author Julian Kent <julian@auterion.com>
 */

#رهنمود once

#ضمن <cmath>

#ضمن "Scalar.hpp"
#ضمن "Vector.hpp"

نطاق ماتریس
{

قالب <نام_نوع Scalar, size_t N>
بنية Dual {
	تعداد : size_t { عرض = N };

	Scalar value {};
	وکتور<Scalar, N> derivative;

	Dual() = افتراضي;

	صریح Dual(Scalar v, size_t درون_داد_Dimension = 65535)
	{
		value = v;

		إذا (درون_داد_Dimension < N) {
			derivative(درون_داد_Dimension) = Scalar(1);
		}
	}

	صریح Dual(Scalar v, ثابت وکتور<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &عملگر=(ثابت Scalar &a)
	{
		derivative.مجموعه_Zero();
		value = a;
		أرجع *هذا;
	}

	Dual<Scalar, N> &عملگر +=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا + a);
	}

	Dual<Scalar, N> &عملگر *=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا * a);
	}

	Dual<Scalar, N> &عملگر -=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا - a);
	}

	Dual<Scalar, N> &عملگر /=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا / a);
	}

	Dual<Scalar, N> &عملگر +=(Scalar a)
	{
		أرجع (*هذا = *هذا + a);
	}

	Dual<Scalar, N> &عملگر -=(Scalar a)
	{
		أرجع (*هذا = *هذا - a);
	}

	Dual<Scalar, N> &عملگر *=(Scalar a)
	{
		أرجع (*هذا = *هذا * a);
	}

	Dual<Scalar, N> &عملگر /=(Scalar a)
	{
		أرجع (*هذا = *هذا / a);
	}

	منطقي عملگر==(ثابت Dual<Scalar, N> &other) ثابت
	{
		أرجع isEqualF(value, other.value) && (derivative == other.derivative);
	}

	منطقي عملگر!=(ثابت Dual<Scalar, N> &other) ثابت
	{
		ثابت Dual<Scalar, N> &self = *هذا;
		أرجع !(self == other);
	}
};

// operators

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر+(ثابت Dual<Scalar, N> &a)
{
	أرجع a;
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر-(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(-a.value, -a.derivative);
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر+(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر-(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع a + (-b);
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر+(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع Dual<Scalar, N>(a.value + b, a.derivative);
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر-(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع a + (-b);
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر+(Scalar a, ثابت Dual<Scalar, N> &b)
{
	أرجع Dual<Scalar, N>(a + b.value, b.derivative);
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر-(Scalar a, ثابت Dual<Scalar, N> &b)
{
	أرجع a + (-b);
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر*(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر*(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع Dual<Scalar, N>(a.value * b, a.derivative * b);
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر*(Scalar a, ثابت Dual<Scalar, N> &b)
{
	أرجع b * a;
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر/(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	ثابت Scalar inv_b_real = Scalar(1) / b.value;
	أرجع Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر/(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع a * (Scalar(1) / b);
}

قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> عملگر/(Scalar a, ثابت Dual<Scalar, N> &b)
{
	ثابت Scalar inv_b_real = Scalar(1) / b.value;
	أرجع Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> جذر(ثابت Dual<Scalar, N> &a)
{
	Scalar real = std::جذر(a.value);
	أرجع Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> قدر_مطلق(ثابت Dual<Scalar, N> &a)
{
	أرجع a.value >= Scalar(0) ? a : -a;
}

// ceil
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> ceil(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::ceil(a.value));
}

// floor
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> floor(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::floor(a.value));
}

// fmod
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> fmod(ثابت Dual<Scalar, N> &a, Scalar mod)
{
	أرجع Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> max(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع a.value >= b.value ? a : b;
}

// min
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> min(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع a.value < b.value ? a : b;
}

// isnan
قالب <نام_نوع Scalar>
منطقي IsNan(Scalar a)
{
	أرجع std::isnan(a);
}

قالب <نام_نوع Scalar, size_t N>
منطقي IsNan(ثابت Dual<Scalar, N> &a)
{
	أرجع IsNan(a.value);
}

// isfinite
قالب <نام_نوع Scalar>
منطقي IsFinite(Scalar a)
{
	أرجع std::isfinite(a);
}

قالب <نام_نوع Scalar, size_t N>
منطقي IsFinite(ثابت Dual<Scalar, N> &a)
{
	أرجع IsFinite(a.value);
}

// isinf
قالب <نام_نوع Scalar>
منطقي IsInf(Scalar a)
{
	أرجع std::isinf(a);
}

قالب <نام_نوع Scalar, size_t N>
منطقي IsInf(ثابت Dual<Scalar, N> &a)
{
	أرجع IsInf(a.value);
}

// trig

// sin
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> سینوس(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::سینوس(a.value), std::کسینوس(a.value) * a.derivative);
}

// cos
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> کسینوس(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::کسینوس(a.value), -std::سینوس(a.value) * a.derivative);
}

// tan
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> tan(ثابت Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	أرجع Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> asin(ثابت Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::جذر(Scalar(1) - a.value * a.value);
	أرجع Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> acos(ثابت Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::جذر(Scalar(1) - a.value * a.value);
	أرجع Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> atan(ثابت Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	أرجع Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
قالب <نام_نوع Scalar, size_t N>
Dual<Scalar, N> atan2(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	أرجع Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
قالب <نام_نوع Scalar, size_t M, size_t N>
ماتریس_2<Scalar, M, N> collectDerivatives(ثابت ماتریس_2<Dual<Scalar, N>, M, 1> &درون_داد)
{
	ماتریس_2<Scalar, M, N> jac;

	لكل (size_t i = 0; i < M; i++) {
		jac.ردیف(i) = درون_داد(i, 0).derivative;
	}

	أرجع jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
قالب <نام_نوع Scalar, size_t M, size_t N, size_t D>
ماتریس_2<Scalar, M, N> collectReals(ثابت ماتریس_2<Dual<Scalar, D>, M, N> &درون_داد)
{
	ماتریس_2<Scalar, M, N> r;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			r(i, j) = درون_داد(i, j).value;
		}
	}

	أرجع r;
}

قالب<نام_نوع OStream, نام_نوع Type, size_t N>
OStream &عملگر<<(OStream &os, ثابت ماتریس::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	أرجع os;
}

}
