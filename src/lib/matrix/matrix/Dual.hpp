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

#pragma once

#ضمن <cmath>

#ضمن "Scalar.hpp"
#ضمن "Vector.hpp"

نطاق matrix
{

template <typename Scalar, size_t N>
بنية Dual {
	تعداد : size_t { WIDTH = N };

	Scalar value {};
	Vector<Scalar, N> derivative;

	Dual() = افتراضي;

	explicit Dual(Scalar v, size_t inputDimension = 65535)
	{
		value = v;

		ܐܢ (inputDimension < N) {
			derivative(inputDimension) = Scalar(1);
		}
	}

	explicit Dual(Scalar v, ثابت Vector<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &operator=(ثابت Scalar &a)
	{
		derivative.setZero();
		value = a;
		ܦܢܐ *هذا;
	}

	Dual<Scalar, N> &operator +=(ثابت Dual<Scalar, N> &a)
	{
		ܦܢܐ (*هذا = *هذا + a);
	}

	Dual<Scalar, N> &operator *=(ثابت Dual<Scalar, N> &a)
	{
		ܦܢܐ (*هذا = *هذا * a);
	}

	Dual<Scalar, N> &operator -=(ثابت Dual<Scalar, N> &a)
	{
		ܦܢܐ (*هذا = *هذا - a);
	}

	Dual<Scalar, N> &operator /=(ثابت Dual<Scalar, N> &a)
	{
		ܦܢܐ (*هذا = *هذا / a);
	}

	Dual<Scalar, N> &operator +=(Scalar a)
	{
		ܦܢܐ (*هذا = *هذا + a);
	}

	Dual<Scalar, N> &operator -=(Scalar a)
	{
		ܦܢܐ (*هذا = *هذا - a);
	}

	Dual<Scalar, N> &operator *=(Scalar a)
	{
		ܦܢܐ (*هذا = *هذا * a);
	}

	Dual<Scalar, N> &operator /=(Scalar a)
	{
		ܦܢܐ (*هذا = *هذا / a);
	}

	منطقي operator==(ثابت Dual<Scalar, N> &other) ثابت
	{
		ܦܢܐ isEqualF(value, other.value) && (derivative == other.derivative);
	}

	منطقي operator!=(ثابت Dual<Scalar, N> &other) ثابت
	{
		ثابت Dual<Scalar, N> &self = *هذا;
		ܦܢܐ !(self == other);
	}
};

// operators

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ثابت Dual<Scalar, N> &a)
{
	ܦܢܐ a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ثابت Dual<Scalar, N> &a)
{
	ܦܢܐ Dual<Scalar, N>(-a.value, -a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	ܦܢܐ Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	ܦܢܐ a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ثابت Dual<Scalar, N> &a, Scalar b)
{
	ܦܢܐ Dual<Scalar, N>(a.value + b, a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ثابت Dual<Scalar, N> &a, Scalar b)
{
	ܦܢܐ a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(Scalar a, ثابت Dual<Scalar, N> &b)
{
	ܦܢܐ Dual<Scalar, N>(a + b.value, b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(Scalar a, ثابت Dual<Scalar, N> &b)
{
	ܦܢܐ a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	ܦܢܐ Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ثابت Dual<Scalar, N> &a, Scalar b)
{
	ܦܢܐ Dual<Scalar, N>(a.value * b, a.derivative * b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(Scalar a, ثابت Dual<Scalar, N> &b)
{
	ܦܢܐ b * a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	ثابت Scalar inv_b_real = Scalar(1) / b.value;
	ܦܢܐ Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ثابت Dual<Scalar, N> &a, Scalar b)
{
	ܦܢܐ a * (Scalar(1) / b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(Scalar a, ثابت Dual<Scalar, N> &b)
{
	ثابت Scalar inv_b_real = Scalar(1) / b.value;
	ܦܢܐ Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
template <typename Scalar, size_t N>
Dual<Scalar, N> sqrt(ثابت Dual<Scalar, N> &a)
{
	Scalar real = std::sqrt(a.value);
	ܦܢܐ Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
template <typename Scalar, size_t N>
Dual<Scalar, N> abs(ثابت Dual<Scalar, N> &a)
{
	ܦܢܐ a.value >= Scalar(0) ? a : -a;
}

// ceil
template <typename Scalar, size_t N>
Dual<Scalar, N> ceil(ثابت Dual<Scalar, N> &a)
{
	ܦܢܐ Dual<Scalar, N>(std::ceil(a.value));
}

// floor
template <typename Scalar, size_t N>
Dual<Scalar, N> floor(ثابت Dual<Scalar, N> &a)
{
	ܦܢܐ Dual<Scalar, N>(std::floor(a.value));
}

// fmod
template <typename Scalar, size_t N>
Dual<Scalar, N> fmod(ثابت Dual<Scalar, N> &a, Scalar mod)
{
	ܦܢܐ Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
template <typename Scalar, size_t N>
Dual<Scalar, N> max(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	ܦܢܐ a.value >= b.value ? a : b;
}

// min
template <typename Scalar, size_t N>
Dual<Scalar, N> min(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	ܦܢܐ a.value < b.value ? a : b;
}

// isnan
template <typename Scalar>
منطقي IsNan(Scalar a)
{
	ܦܢܐ std::isnan(a);
}

template <typename Scalar, size_t N>
منطقي IsNan(ثابت Dual<Scalar, N> &a)
{
	ܦܢܐ IsNan(a.value);
}

// isfinite
template <typename Scalar>
منطقي IsFinite(Scalar a)
{
	ܦܢܐ std::isfinite(a);
}

template <typename Scalar, size_t N>
منطقي IsFinite(ثابت Dual<Scalar, N> &a)
{
	ܦܢܐ IsFinite(a.value);
}

// isinf
template <typename Scalar>
منطقي IsInf(Scalar a)
{
	ܦܢܐ std::isinf(a);
}

template <typename Scalar, size_t N>
منطقي IsInf(ثابت Dual<Scalar, N> &a)
{
	ܦܢܐ IsInf(a.value);
}

// trig

// sin
template <typename Scalar, size_t N>
Dual<Scalar, N> sin(ثابت Dual<Scalar, N> &a)
{
	ܦܢܐ Dual<Scalar, N>(std::sin(a.value), std::cos(a.value) * a.derivative);
}

// cos
template <typename Scalar, size_t N>
Dual<Scalar, N> cos(ثابت Dual<Scalar, N> &a)
{
	ܦܢܐ Dual<Scalar, N>(std::cos(a.value), -std::sin(a.value) * a.derivative);
}

// tan
template <typename Scalar, size_t N>
Dual<Scalar, N> tan(ثابت Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	ܦܢܐ Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
template <typename Scalar, size_t N>
Dual<Scalar, N> asin(ثابت Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	ܦܢܐ Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
template <typename Scalar, size_t N>
Dual<Scalar, N> acos(ثابت Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	ܦܢܐ Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
template <typename Scalar, size_t N>
Dual<Scalar, N> atan(ثابت Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	ܦܢܐ Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
template <typename Scalar, size_t N>
Dual<Scalar, N> atan2(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	ܦܢܐ Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
template <typename Scalar, size_t M, size_t N>
Matrix<Scalar, M, N> collectDerivatives(ثابت Matrix<Dual<Scalar, N>, M, 1> &input)
{
	Matrix<Scalar, M, N> jac;

	لكل (size_t i = 0; i < M; i++) {
		jac.row(i) = input(i, 0).derivative;
	}

	ܦܢܐ jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
template <typename Scalar, size_t M, size_t N, size_t D>
Matrix<Scalar, M, N> collectReals(ثابت Matrix<Dual<Scalar, D>, M, N> &input)
{
	Matrix<Scalar, M, N> r;

	لكل (size_t i = 0; i < M; i++) {
		لكل (size_t j = 0; j < N; j++) {
			r(i, j) = input(i, j).value;
		}
	}

	ܦܢܐ r;
}

template<typename OStream, typename Type, size_t N>
OStream &operator<<(OStream &os, ثابت matrix::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	ܦܢܐ os;
}

}
