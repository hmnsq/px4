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

#لارښوونه once

#ضمن <cmath>

#ضمن "Scalar.hpp"
#ضمن "Vector.hpp"

نطاق ماتریکس
{

کينډۍ <ډول_نوم Scalar, size_t N>
بنية Dual {
	تعداد : size_t { پلنوالی = N };

	Scalar value {};
	ویکټور<Scalar, N> derivative;

	Dual() = افتراضي;

	څرګند Dual(Scalar v, size_t ننوت_Dimension = 65535)
	{
		value = v;

		إذا (ننوت_Dimension < N) {
			derivative(ننوت_Dimension) = Scalar(1);
		}
	}

	څرګند Dual(Scalar v, ثابت ویکټور<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &چلوونکی=(ثابت Scalar &a)
	{
		derivative.سېټ_Zero();
		value = a;
		أرجع *هذا;
	}

	Dual<Scalar, N> &چلوونکی +=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا + a);
	}

	Dual<Scalar, N> &چلوونکی *=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا * a);
	}

	Dual<Scalar, N> &چلوونکی -=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا - a);
	}

	Dual<Scalar, N> &چلوونکی /=(ثابت Dual<Scalar, N> &a)
	{
		أرجع (*هذا = *هذا / a);
	}

	Dual<Scalar, N> &چلوونکی +=(Scalar a)
	{
		أرجع (*هذا = *هذا + a);
	}

	Dual<Scalar, N> &چلوونکی -=(Scalar a)
	{
		أرجع (*هذا = *هذا - a);
	}

	Dual<Scalar, N> &چلوونکی *=(Scalar a)
	{
		أرجع (*هذا = *هذا * a);
	}

	Dual<Scalar, N> &چلوونکی /=(Scalar a)
	{
		أرجع (*هذا = *هذا / a);
	}

	منطقي چلوونکی==(ثابت Dual<Scalar, N> &other) ثابت
	{
		أرجع isEqualF(value, other.value) && (derivative == other.derivative);
	}

	منطقي چلوونکی!=(ثابت Dual<Scalar, N> &other) ثابت
	{
		ثابت Dual<Scalar, N> &self = *هذا;
		أرجع !(self == other);
	}
};

// operators

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی+(ثابت Dual<Scalar, N> &a)
{
	أرجع a;
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی-(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(-a.value, -a.derivative);
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی+(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی-(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع a + (-b);
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی+(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع Dual<Scalar, N>(a.value + b, a.derivative);
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی-(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع a + (-b);
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی+(Scalar a, ثابت Dual<Scalar, N> &b)
{
	أرجع Dual<Scalar, N>(a + b.value, b.derivative);
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی-(Scalar a, ثابت Dual<Scalar, N> &b)
{
	أرجع a + (-b);
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی*(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی*(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع Dual<Scalar, N>(a.value * b, a.derivative * b);
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی*(Scalar a, ثابت Dual<Scalar, N> &b)
{
	أرجع b * a;
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی/(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	ثابت Scalar inv_b_real = Scalar(1) / b.value;
	أرجع Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی/(ثابت Dual<Scalar, N> &a, Scalar b)
{
	أرجع a * (Scalar(1) / b);
}

کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> چلوونکی/(Scalar a, ثابت Dual<Scalar, N> &b)
{
	ثابت Scalar inv_b_real = Scalar(1) / b.value;
	أرجع Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> جذر(ثابت Dual<Scalar, N> &a)
{
	Scalar real = std::جذر(a.value);
	أرجع Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> مطلق_قیمت(ثابت Dual<Scalar, N> &a)
{
	أرجع a.value >= Scalar(0) ? a : -a;
}

// ceil
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> ceil(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::ceil(a.value));
}

// floor
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> floor(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::floor(a.value));
}

// fmod
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> fmod(ثابت Dual<Scalar, N> &a, Scalar mod)
{
	أرجع Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> max(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع a.value >= b.value ? a : b;
}

// min
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> min(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	أرجع a.value < b.value ? a : b;
}

// isnan
کينډۍ <ډول_نوم Scalar>
منطقي IsNan(Scalar a)
{
	أرجع std::isnan(a);
}

کينډۍ <ډول_نوم Scalar, size_t N>
منطقي IsNan(ثابت Dual<Scalar, N> &a)
{
	أرجع IsNan(a.value);
}

// isfinite
کينډۍ <ډول_نوم Scalar>
منطقي IsFinite(Scalar a)
{
	أرجع std::isfinite(a);
}

کينډۍ <ډول_نوم Scalar, size_t N>
منطقي IsFinite(ثابت Dual<Scalar, N> &a)
{
	أرجع IsFinite(a.value);
}

// isinf
کينډۍ <ډول_نوم Scalar>
منطقي IsInf(Scalar a)
{
	أرجع std::isinf(a);
}

کينډۍ <ډول_نوم Scalar, size_t N>
منطقي IsInf(ثابت Dual<Scalar, N> &a)
{
	أرجع IsInf(a.value);
}

// trig

// sin
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> ساین(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::ساین(a.value), std::کوساین(a.value) * a.derivative);
}

// cos
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> کوساین(ثابت Dual<Scalar, N> &a)
{
	أرجع Dual<Scalar, N>(std::کوساین(a.value), -std::ساین(a.value) * a.derivative);
}

// tan
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> tan(ثابت Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	أرجع Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> asin(ثابت Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::جذر(Scalar(1) - a.value * a.value);
	أرجع Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> acos(ثابت Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::جذر(Scalar(1) - a.value * a.value);
	أرجع Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> atan(ثابت Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	أرجع Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
کينډۍ <ډول_نوم Scalar, size_t N>
Dual<Scalar, N> atan2(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	أرجع Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
کينډۍ <ډول_نوم Scalar, size_t M, size_t N>
ماتریکس_2<Scalar, M, N> collectDerivatives(ثابت ماتریکس_2<Dual<Scalar, N>, M, 1> &ننوت)
{
	ماتریکس_2<Scalar, M, N> jac;

	لكل (size_t i = 0; i < M; i++) {
		jac.لیکه(i) = ننوت(i, 0).derivative;
	}

	أرجع jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
کينډۍ <ډول_نوم Scalar, size_t M, size_t N, size_t D>
ماتریکس_2<Scalar, M, N> collectReals(ثابت ماتریکس_2<Dual<Scalar, D>, M, N> &ننوت)
{
	ماتریکس_2<Scalar, M, N> r;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			r(i, j) = ننوت(i, j).value;
		}
	}

	أرجع r;
}

کينډۍ<ډول_نوم OStream, ډول_نوم Type, size_t N>
OStream &چلوونکی<<(OStream &os, ثابت ماتریکس::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	أرجع os;
}

}
