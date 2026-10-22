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

#ଅନ୍ତର୍ଭାଵ <cmath>

#ଅନ୍ତର୍ଭାଵ "Scalar.hpp"
#ଅନ୍ତର୍ଭାଵ "Vector.hpp"

ନାମାକାଶ matrix
{

template <typename Scalar, size_t N>
ସଂରଚନା Dual {
	ଗଣନା : size_t { WIDTH = N };

	Scalar value {};
	Vector<Scalar, N> derivative;

	Dual() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	explicit Dual(Scalar v, size_t ନିବେଶ_Dimension = 65535)
	{
		value = v;

		ଯଦି (ନିବେଶ_Dimension < N) {
			derivative(ନିବେଶ_Dimension) = Scalar(1);
		}
	}

	explicit Dual(Scalar v, ଅଚଳ Vector<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &operator=(ଅଚଳ Scalar &a)
	{
		derivative.setZero();
		value = a;
		ଫେରନ୍ତୁ *ଏହି;
	}

	Dual<Scalar, N> &operator +=(ଅଚଳ Dual<Scalar, N> &a)
	{
		ଫେରନ୍ତୁ (*ଏହି = *ଏହି + a);
	}

	Dual<Scalar, N> &operator *=(ଅଚଳ Dual<Scalar, N> &a)
	{
		ଫେରନ୍ତୁ (*ଏହି = *ଏହି * a);
	}

	Dual<Scalar, N> &operator -=(ଅଚଳ Dual<Scalar, N> &a)
	{
		ଫେରନ୍ତୁ (*ଏହି = *ଏହି - a);
	}

	Dual<Scalar, N> &operator /=(ଅଚଳ Dual<Scalar, N> &a)
	{
		ଫେରନ୍ତୁ (*ଏହି = *ଏହି / a);
	}

	Dual<Scalar, N> &operator +=(Scalar a)
	{
		ଫେରନ୍ତୁ (*ଏହି = *ଏହି + a);
	}

	Dual<Scalar, N> &operator -=(Scalar a)
	{
		ଫେରନ୍ତୁ (*ଏହି = *ଏହି - a);
	}

	Dual<Scalar, N> &operator *=(Scalar a)
	{
		ଫେରନ୍ତୁ (*ଏହି = *ଏହି * a);
	}

	Dual<Scalar, N> &operator /=(Scalar a)
	{
		ଫେରନ୍ତୁ (*ଏହି = *ଏହି / a);
	}

	ତାର୍କିକ operator==(ଅଚଳ Dual<Scalar, N> &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ isEqualF(value, other.value) && (derivative == other.derivative);
	}

	ତାର୍କିକ operator!=(ଅଚଳ Dual<Scalar, N> &other) ଅଚଳ
	{
		ଅଚଳ Dual<Scalar, N> &self = *ଏହି;
		ଫେରନ୍ତୁ !(self == other);
	}
};

// operators

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ଅଚଳ Dual<Scalar, N> &a)
{
	ଫେରନ୍ତୁ a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ଅଚଳ Dual<Scalar, N> &a)
{
	ଫେରନ୍ତୁ Dual<Scalar, N>(-a.value, -a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ଅଚଳ Dual<Scalar, N> &a, ଅଚଳ Dual<Scalar, N> &b)
{
	ଫେରନ୍ତୁ Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ଅଚଳ Dual<Scalar, N> &a, ଅଚଳ Dual<Scalar, N> &b)
{
	ଫେରନ୍ତୁ a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ଅଚଳ Dual<Scalar, N> &a, Scalar b)
{
	ଫେରନ୍ତୁ Dual<Scalar, N>(a.value + b, a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ଅଚଳ Dual<Scalar, N> &a, Scalar b)
{
	ଫେରନ୍ତୁ a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(Scalar a, ଅଚଳ Dual<Scalar, N> &b)
{
	ଫେରନ୍ତୁ Dual<Scalar, N>(a + b.value, b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(Scalar a, ଅଚଳ Dual<Scalar, N> &b)
{
	ଫେରନ୍ତୁ a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ଅଚଳ Dual<Scalar, N> &a, ଅଚଳ Dual<Scalar, N> &b)
{
	ଫେରନ୍ତୁ Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ଅଚଳ Dual<Scalar, N> &a, Scalar b)
{
	ଫେରନ୍ତୁ Dual<Scalar, N>(a.value * b, a.derivative * b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(Scalar a, ଅଚଳ Dual<Scalar, N> &b)
{
	ଫେରନ୍ତୁ b * a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ଅଚଳ Dual<Scalar, N> &a, ଅଚଳ Dual<Scalar, N> &b)
{
	ଅଚଳ Scalar inv_b_real = Scalar(1) / b.value;
	ଫେରନ୍ତୁ Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ଅଚଳ Dual<Scalar, N> &a, Scalar b)
{
	ଫେରନ୍ତୁ a * (Scalar(1) / b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(Scalar a, ଅଚଳ Dual<Scalar, N> &b)
{
	ଅଚଳ Scalar inv_b_real = Scalar(1) / b.value;
	ଫେରନ୍ତୁ Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
template <typename Scalar, size_t N>
Dual<Scalar, N> sqrt(ଅଚଳ Dual<Scalar, N> &a)
{
	Scalar real = std::sqrt(a.value);
	ଫେରନ୍ତୁ Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
template <typename Scalar, size_t N>
Dual<Scalar, N> abs(ଅଚଳ Dual<Scalar, N> &a)
{
	ଫେରନ୍ତୁ a.value >= Scalar(0) ? a : -a;
}

// ceil
template <typename Scalar, size_t N>
Dual<Scalar, N> ceil(ଅଚଳ Dual<Scalar, N> &a)
{
	ଫେରନ୍ତୁ Dual<Scalar, N>(std::ceil(a.value));
}

// floor
template <typename Scalar, size_t N>
Dual<Scalar, N> floor(ଅଚଳ Dual<Scalar, N> &a)
{
	ଫେରନ୍ତୁ Dual<Scalar, N>(std::floor(a.value));
}

// fmod
template <typename Scalar, size_t N>
Dual<Scalar, N> fmod(ଅଚଳ Dual<Scalar, N> &a, Scalar mod)
{
	ଫେରନ୍ତୁ Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
template <typename Scalar, size_t N>
Dual<Scalar, N> max(ଅଚଳ Dual<Scalar, N> &a, ଅଚଳ Dual<Scalar, N> &b)
{
	ଫେରନ୍ତୁ a.value >= b.value ? a : b;
}

// min
template <typename Scalar, size_t N>
Dual<Scalar, N> min(ଅଚଳ Dual<Scalar, N> &a, ଅଚଳ Dual<Scalar, N> &b)
{
	ଫେରନ୍ତୁ a.value < b.value ? a : b;
}

// isnan
template <typename Scalar>
ତାର୍କିକ IsNan(Scalar a)
{
	ଫେରନ୍ତୁ std::isnan(a);
}

template <typename Scalar, size_t N>
ତାର୍କିକ IsNan(ଅଚଳ Dual<Scalar, N> &a)
{
	ଫେରନ୍ତୁ IsNan(a.value);
}

// isfinite
template <typename Scalar>
ତାର୍କିକ IsFinite(Scalar a)
{
	ଫେରନ୍ତୁ std::isfinite(a);
}

template <typename Scalar, size_t N>
ତାର୍କିକ IsFinite(ଅଚଳ Dual<Scalar, N> &a)
{
	ଫେରନ୍ତୁ IsFinite(a.value);
}

// isinf
template <typename Scalar>
ତାର୍କିକ IsInf(Scalar a)
{
	ଫେରନ୍ତୁ std::isinf(a);
}

template <typename Scalar, size_t N>
ତାର୍କିକ IsInf(ଅଚଳ Dual<Scalar, N> &a)
{
	ଫେରନ୍ତୁ IsInf(a.value);
}

// trig

// sin
template <typename Scalar, size_t N>
Dual<Scalar, N> sin(ଅଚଳ Dual<Scalar, N> &a)
{
	ଫେରନ୍ତୁ Dual<Scalar, N>(std::sin(a.value), std::cos(a.value) * a.derivative);
}

// cos
template <typename Scalar, size_t N>
Dual<Scalar, N> cos(ଅଚଳ Dual<Scalar, N> &a)
{
	ଫେରନ୍ତୁ Dual<Scalar, N>(std::cos(a.value), -std::sin(a.value) * a.derivative);
}

// tan
template <typename Scalar, size_t N>
Dual<Scalar, N> tan(ଅଚଳ Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	ଫେରନ୍ତୁ Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
template <typename Scalar, size_t N>
Dual<Scalar, N> asin(ଅଚଳ Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	ଫେରନ୍ତୁ Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
template <typename Scalar, size_t N>
Dual<Scalar, N> acos(ଅଚଳ Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	ଫେରନ୍ତୁ Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
template <typename Scalar, size_t N>
Dual<Scalar, N> atan(ଅଚଳ Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	ଫେରନ୍ତୁ Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
template <typename Scalar, size_t N>
Dual<Scalar, N> atan2(ଅଚଳ Dual<Scalar, N> &a, ଅଚଳ Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	ଫେରନ୍ତୁ Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
template <typename Scalar, size_t M, size_t N>
Matrix<Scalar, M, N> collectDerivatives(ଅଚଳ Matrix<Dual<Scalar, N>, M, 1> &ନିବେଶ)
{
	Matrix<Scalar, M, N> jac;

	ପାଇଁ (size_t i = 0; i < M; i++) {
		jac.row(i) = ନିବେଶ(i, 0).derivative;
	}

	ଫେରନ୍ତୁ jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
template <typename Scalar, size_t M, size_t N, size_t D>
Matrix<Scalar, M, N> collectReals(ଅଚଳ Matrix<Dual<Scalar, D>, M, N> &ନିବେଶ)
{
	Matrix<Scalar, M, N> r;

	ପାଇଁ (size_t i = 0; i < M; i++) {
		ପାଇଁ (size_t j = 0; j < N; j++) {
			r(i, j) = ନିବେଶ(i, j).value;
		}
	}

	ଫେରନ୍ତୁ r;
}

template<typename OStream, typename Type, size_t N>
OStream &operator<<(OStream &os, ଅଚଳ matrix::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	ଫେରନ୍ତୁ os;
}

}
