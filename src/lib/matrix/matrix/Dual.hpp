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

#هدايت once

#ضمن <cmath>

#ضمن "Scalar.hpp"
#ضمن "Vector.hpp"

نطاق ميٽرڪس
{

سانچو <قسم_نالو Scalar, size_t N>
بنية Dual {
	تعداد : size_t { ويڪر = N };

	Scalar value {};
	ويڪٽر<Scalar, N> derivative;

	Dual() = افتراضي;

	واضح Dual(Scalar v, size_t داخلا_Dimension = 65535)
	{
		value = v;

		إذا (داخلا_Dimension < N) {
			derivative(داخلا_Dimension) = Scalar(1);
		}
	}

	واضح Dual(Scalar v, ثابت ويڪٽر<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &عامل=(ثابت Scalar &a)
	{
		derivative.سيٽ_Zero();
		value = a;
		أرجع *هذا;
	}

	Dual<Scalar, N> &عامل +=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا + a);
	}

	Dual<Scalar, N> &عامل *=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا * a);
	}

	Dual<Scalar, N> &عامل -=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا - a);
	}

	Dual<Scalar, N> &عامل /=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا / a);
	}

	Dual<Scalar, N> &عامل +=(Scalar a)
	{
		أرجع (*هذا = *هذا + a);
	}

	Dual<Scalar, N> &عامل -=(Scalar a)
	{
		أرجع (*هذا = *هذا - a);
	}

	Dual<Scalar, N> &عامل *=(Scalar a)
	{
		أرجع (*هذا = *هذا * a);
	}

	Dual<Scalar, N> &عامل /=(Scalar a)
	{
		أرجع (*هذا = *هذا / a);
	}

	منطقي عامل==(ثابت Dual<Scalar, N> &other) ثابت
	{
		أرجع isEqualF(value, other.value) && (derivative == other.derivative);
	}

	منطقي عامل!=(ثابت Dual<Scalar, N> &other) ثابت
	{
		ثابت Dual<Scalar, N> &self = *هذا;
		أرجع !(self == other);
	}
};

// operators

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل+(ثابت Dual<Scalar, N> &a)
{
	أرجع a;
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل-(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(-a.value, -a.derivative);
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل+(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل-(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع a + (-b);
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل+(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع Dual<Scalar, N>(a.value + b, a.derivative);
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل-(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع a + (-b);
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل+(Scalar a, ثابت Dual<Scalar, N> &b)
{
	أرجع Dual<Scalar, N>(a + b.value, b.derivative);
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل-(Scalar a, ثابت Dual<Scalar, N> &b)
{
	أرجع a + (-b);
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل*(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل*(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع Dual<Scalar, N>(a.value * b, a.derivative * b);
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل*(Scalar a, ثابت Dual<Scalar, N> &b)
{
	أرجع b * a;
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل/(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	ثابت Scalar inv_b_real = Scalar(1) / b.value;
	أرجع Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل/(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع a * (Scalar(1) / b);
}

سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> عامل/(Scalar a, ثابت Dual<Scalar, N> &b)
{
	ثابت Scalar inv_b_real = Scalar(1) / b.value;
	أرجع Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> جذر(ثابت Dual<Scalar, N> &a)
{
	Scalar real = std::جذر(a.value);
	أرجع Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> مطلق_قدر(ثابت Dual<Scalar, N> &a)
{
	أرجع a.value >= Scalar(0) ? a : -a;
}

// ceil
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> ceil(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::ceil(a.value));
}

// floor
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> floor(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::floor(a.value));
}

// fmod
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> fmod(ثابت Dual<Scalar, N> &a, Scalar mod)
{
	أرجع Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> max(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع a.value >= b.value ? a : b;
}

// min
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> min(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع a.value < b.value ? a : b;
}

// isnan
سانچو <قسم_نالو Scalar>
منطقي IsNan(Scalar a)
{
	أرجع std::isnan(a);
}

سانچو <قسم_نالو Scalar, size_t N>
منطقي IsNan(ثابت Dual<Scalar, N> &a)
{
	أرجع IsNan(a.value);
}

// isfinite
سانچو <قسم_نالو Scalar>
منطقي IsFinite(Scalar a)
{
	أرجع std::isfinite(a);
}

سانچو <قسم_نالو Scalar, size_t N>
منطقي IsFinite(ثابت Dual<Scalar, N> &a)
{
	أرجع IsFinite(a.value);
}

// isinf
سانچو <قسم_نالو Scalar>
منطقي IsInf(Scalar a)
{
	أرجع std::isinf(a);
}

سانچو <قسم_نالو Scalar, size_t N>
منطقي IsInf(ثابت Dual<Scalar, N> &a)
{
	أرجع IsInf(a.value);
}

// trig

// sin
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> جيب(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::جيب(a.value), std::جيب_تمام(a.value) * a.derivative);
}

// cos
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> جيب_تمام(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::جيب_تمام(a.value), -std::جيب(a.value) * a.derivative);
}

// tan
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> tan(ثابت Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	أرجع Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> asin(ثابت Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::جذر(Scalar(1) - a.value * a.value);
	أرجع Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> acos(ثابت Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::جذر(Scalar(1) - a.value * a.value);
	أرجع Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> atan(ثابت Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	أرجع Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
سانچو <قسم_نالو Scalar, size_t N>
Dual<Scalar, N> atan2(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	أرجع Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
سانچو <قسم_نالو Scalar, size_t M, size_t N>
ميٽرڪس_2<Scalar, M, N> collectDerivatives(ثابت ميٽرڪس_2<Dual<Scalar, N>, M, 1> &داخلا)
{
	ميٽرڪس_2<Scalar, M, N> jac;

	لكل (size_t i = 0; i < M; i++) {
		jac.صف(i) = داخلا(i, 0).derivative;
	}

	أرجع jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
سانچو <قسم_نالو Scalar, size_t M, size_t N, size_t D>
ميٽرڪس_2<Scalar, M, N> collectReals(ثابت ميٽرڪس_2<Dual<Scalar, D>, M, N> &داخلا)
{
	ميٽرڪس_2<Scalar, M, N> r;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			r(i, j) = داخلا(i, j).value;
		}
	}

	أرجع r;
}

سانچو<قسم_نالو OStream, قسم_نالو Type, size_t N>
OStream &عامل<<(OStream &os, ثابت ميٽرڪس::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	أرجع os;
}

}
