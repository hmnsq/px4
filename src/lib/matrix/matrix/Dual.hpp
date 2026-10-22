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

#شامل <cmath>

#شامل "Scalar.hpp"
#شامل "Vector.hpp"

فضاینام matrix
{

template <typename Scalar, size_t N>
ساختار Dual {
	شمارش : size_t { WIDTH = N };

	Scalar value {};
	Vector<Scalar, N> derivative;

	Dual() = پیشفرض;

	explicit Dual(Scalar v, size_t درون_داد_Dimension = 65535)
	{
		value = v;

		اگر (درون_داد_Dimension < N) {
			derivative(درون_داد_Dimension) = Scalar(1);
		}
	}

	explicit Dual(Scalar v, ثابت Vector<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &operator=(ثابت Scalar &a)
	{
		derivative.setZero();
		value = a;
		بازگشت *این;
	}

	Dual<Scalar, N> &operator +=(ثابت Dual<Scalar, N> &a)
	{
		بازگشت (*این = *این + a);
	}

	Dual<Scalar, N> &operator *=(ثابت Dual<Scalar, N> &a)
	{
		بازگشت (*این = *این * a);
	}

	Dual<Scalar, N> &operator -=(ثابت Dual<Scalar, N> &a)
	{
		بازگشت (*این = *این - a);
	}

	Dual<Scalar, N> &operator /=(ثابت Dual<Scalar, N> &a)
	{
		بازگشت (*این = *این / a);
	}

	Dual<Scalar, N> &operator +=(Scalar a)
	{
		بازگشت (*این = *این + a);
	}

	Dual<Scalar, N> &operator -=(Scalar a)
	{
		بازگشت (*این = *این - a);
	}

	Dual<Scalar, N> &operator *=(Scalar a)
	{
		بازگشت (*این = *این * a);
	}

	Dual<Scalar, N> &operator /=(Scalar a)
	{
		بازگشت (*این = *این / a);
	}

	منطقی operator==(ثابت Dual<Scalar, N> &other) ثابت
	{
		بازگشت isEqualF(value, other.value) && (derivative == other.derivative);
	}

	منطقی operator!=(ثابت Dual<Scalar, N> &other) ثابت
	{
		ثابت Dual<Scalar, N> &self = *این;
		بازگشت !(self == other);
	}
};

// operators

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ثابت Dual<Scalar, N> &a)
{
	بازگشت a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ثابت Dual<Scalar, N> &a)
{
	بازگشت Dual<Scalar, N>(-a.value, -a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	بازگشت Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	بازگشت a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ثابت Dual<Scalar, N> &a, Scalar b)
{
	بازگشت Dual<Scalar, N>(a.value + b, a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ثابت Dual<Scalar, N> &a, Scalar b)
{
	بازگشت a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(Scalar a, ثابت Dual<Scalar, N> &b)
{
	بازگشت Dual<Scalar, N>(a + b.value, b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(Scalar a, ثابت Dual<Scalar, N> &b)
{
	بازگشت a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	بازگشت Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ثابت Dual<Scalar, N> &a, Scalar b)
{
	بازگشت Dual<Scalar, N>(a.value * b, a.derivative * b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(Scalar a, ثابت Dual<Scalar, N> &b)
{
	بازگشت b * a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	ثابت Scalar inv_b_real = Scalar(1) / b.value;
	بازگشت Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ثابت Dual<Scalar, N> &a, Scalar b)
{
	بازگشت a * (Scalar(1) / b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(Scalar a, ثابت Dual<Scalar, N> &b)
{
	ثابت Scalar inv_b_real = Scalar(1) / b.value;
	بازگشت Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
template <typename Scalar, size_t N>
Dual<Scalar, N> sqrt(ثابت Dual<Scalar, N> &a)
{
	Scalar real = std::sqrt(a.value);
	بازگشت Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
template <typename Scalar, size_t N>
Dual<Scalar, N> abs(ثابت Dual<Scalar, N> &a)
{
	بازگشت a.value >= Scalar(0) ? a : -a;
}

// ceil
template <typename Scalar, size_t N>
Dual<Scalar, N> ceil(ثابت Dual<Scalar, N> &a)
{
	بازگشت Dual<Scalar, N>(std::ceil(a.value));
}

// floor
template <typename Scalar, size_t N>
Dual<Scalar, N> floor(ثابت Dual<Scalar, N> &a)
{
	بازگشت Dual<Scalar, N>(std::floor(a.value));
}

// fmod
template <typename Scalar, size_t N>
Dual<Scalar, N> fmod(ثابت Dual<Scalar, N> &a, Scalar mod)
{
	بازگشت Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
template <typename Scalar, size_t N>
Dual<Scalar, N> max(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	بازگشت a.value >= b.value ? a : b;
}

// min
template <typename Scalar, size_t N>
Dual<Scalar, N> min(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	بازگشت a.value < b.value ? a : b;
}

// isnan
template <typename Scalar>
منطقی IsNan(Scalar a)
{
	بازگشت std::isnan(a);
}

template <typename Scalar, size_t N>
منطقی IsNan(ثابت Dual<Scalar, N> &a)
{
	بازگشت IsNan(a.value);
}

// isfinite
template <typename Scalar>
منطقی IsFinite(Scalar a)
{
	بازگشت std::isfinite(a);
}

template <typename Scalar, size_t N>
منطقی IsFinite(ثابت Dual<Scalar, N> &a)
{
	بازگشت IsFinite(a.value);
}

// isinf
template <typename Scalar>
منطقی IsInf(Scalar a)
{
	بازگشت std::isinf(a);
}

template <typename Scalar, size_t N>
منطقی IsInf(ثابت Dual<Scalar, N> &a)
{
	بازگشت IsInf(a.value);
}

// trig

// sin
template <typename Scalar, size_t N>
Dual<Scalar, N> sin(ثابت Dual<Scalar, N> &a)
{
	بازگشت Dual<Scalar, N>(std::sin(a.value), std::cos(a.value) * a.derivative);
}

// cos
template <typename Scalar, size_t N>
Dual<Scalar, N> cos(ثابت Dual<Scalar, N> &a)
{
	بازگشت Dual<Scalar, N>(std::cos(a.value), -std::sin(a.value) * a.derivative);
}

// tan
template <typename Scalar, size_t N>
Dual<Scalar, N> tan(ثابت Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	بازگشت Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
template <typename Scalar, size_t N>
Dual<Scalar, N> asin(ثابت Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	بازگشت Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
template <typename Scalar, size_t N>
Dual<Scalar, N> acos(ثابت Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	بازگشت Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
template <typename Scalar, size_t N>
Dual<Scalar, N> atan(ثابت Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	بازگشت Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
template <typename Scalar, size_t N>
Dual<Scalar, N> atan2(ثابت Dual<Scalar, N> &a, ثابت Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	بازگشت Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
template <typename Scalar, size_t M, size_t N>
Matrix<Scalar, M, N> collectDerivatives(ثابت Matrix<Dual<Scalar, N>, M, 1> &درون_داد)
{
	Matrix<Scalar, M, N> jac;

	برای (size_t i = 0; i < M; i++) {
		jac.row(i) = درون_داد(i, 0).derivative;
	}

	بازگشت jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
template <typename Scalar, size_t M, size_t N, size_t D>
Matrix<Scalar, M, N> collectReals(ثابت Matrix<Dual<Scalar, D>, M, N> &درون_داد)
{
	Matrix<Scalar, M, N> r;

	برای (size_t i = 0; i < M; i++) {
		برای (size_t j = 0; j < N; j++) {
			r(i, j) = درون_داد(i, j).value;
		}
	}

	بازگشت r;
}

template<typename OStream, typename Type, size_t N>
OStream &operator<<(OStream &os, ثابت matrix::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	بازگشت os;
}

}
