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

#অন্তর্ভাৱ <cmath>

#অন্তর্ভাৱ "Scalar.hpp"
#অন্তর্ভাৱ "Vector.hpp"

নামাকাশ matrix
{

template <typename Scalar, size_t N>
সংরচনা Dual {
	গণনা : size_t { WIDTH = N };

	Scalar value {};
	Vector<Scalar, N> derivative;

	Dual() = স্ৱভাৱ;

	explicit Dual(Scalar v, size_t প্ৰবেশ_Dimension = 65535)
	{
		value = v;

		যদি (প্ৰবেশ_Dimension < N) {
			derivative(প্ৰবেশ_Dimension) = Scalar(1);
		}
	}

	explicit Dual(Scalar v, ধ্রুৱ Vector<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &operator=(ধ্রুৱ Scalar &a)
	{
		derivative.setZero();
		value = a;
		ঘূৰাই *এতত্;
	}

	Dual<Scalar, N> &operator +=(ধ্রুৱ Dual<Scalar, N> &a)
	{
		ঘূৰাই (*এতত্ = *এতত্ + a);
	}

	Dual<Scalar, N> &operator *=(ধ্রুৱ Dual<Scalar, N> &a)
	{
		ঘূৰাই (*এতত্ = *এতত্ * a);
	}

	Dual<Scalar, N> &operator -=(ধ্রুৱ Dual<Scalar, N> &a)
	{
		ঘূৰাই (*এতত্ = *এতত্ - a);
	}

	Dual<Scalar, N> &operator /=(ধ্রুৱ Dual<Scalar, N> &a)
	{
		ঘূৰাই (*এতত্ = *এতত্ / a);
	}

	Dual<Scalar, N> &operator +=(Scalar a)
	{
		ঘূৰাই (*এতত্ = *এতত্ + a);
	}

	Dual<Scalar, N> &operator -=(Scalar a)
	{
		ঘূৰাই (*এতত্ = *এতত্ - a);
	}

	Dual<Scalar, N> &operator *=(Scalar a)
	{
		ঘূৰাই (*এতত্ = *এতত্ * a);
	}

	Dual<Scalar, N> &operator /=(Scalar a)
	{
		ঘূৰাই (*এতত্ = *এতত্ / a);
	}

	তার্কিক operator==(ধ্রুৱ Dual<Scalar, N> &other) ধ্রুৱ
	{
		ঘূৰাই isEqualF(value, other.value) && (derivative == other.derivative);
	}

	তার্কিক operator!=(ধ্রুৱ Dual<Scalar, N> &other) ধ্রুৱ
	{
		ধ্রুৱ Dual<Scalar, N> &self = *এতত্;
		ঘূৰাই !(self == other);
	}
};

// operators

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ধ্রুৱ Dual<Scalar, N> &a)
{
	ঘূৰাই a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ধ্রুৱ Dual<Scalar, N> &a)
{
	ঘূৰাই Dual<Scalar, N>(-a.value, -a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ধ্রুৱ Dual<Scalar, N> &a, ধ্রুৱ Dual<Scalar, N> &b)
{
	ঘূৰাই Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ধ্রুৱ Dual<Scalar, N> &a, ধ্রুৱ Dual<Scalar, N> &b)
{
	ঘূৰাই a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ধ্রুৱ Dual<Scalar, N> &a, Scalar b)
{
	ঘূৰাই Dual<Scalar, N>(a.value + b, a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ধ্রুৱ Dual<Scalar, N> &a, Scalar b)
{
	ঘূৰাই a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(Scalar a, ধ্রুৱ Dual<Scalar, N> &b)
{
	ঘূৰাই Dual<Scalar, N>(a + b.value, b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(Scalar a, ধ্রুৱ Dual<Scalar, N> &b)
{
	ঘূৰাই a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ধ্রুৱ Dual<Scalar, N> &a, ধ্রুৱ Dual<Scalar, N> &b)
{
	ঘূৰাই Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ধ্রুৱ Dual<Scalar, N> &a, Scalar b)
{
	ঘূৰাই Dual<Scalar, N>(a.value * b, a.derivative * b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(Scalar a, ধ্রুৱ Dual<Scalar, N> &b)
{
	ঘূৰাই b * a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ধ্রুৱ Dual<Scalar, N> &a, ধ্রুৱ Dual<Scalar, N> &b)
{
	ধ্রুৱ Scalar inv_b_real = Scalar(1) / b.value;
	ঘূৰাই Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ধ্রুৱ Dual<Scalar, N> &a, Scalar b)
{
	ঘূৰাই a * (Scalar(1) / b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(Scalar a, ধ্রুৱ Dual<Scalar, N> &b)
{
	ধ্রুৱ Scalar inv_b_real = Scalar(1) / b.value;
	ঘূৰাই Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
template <typename Scalar, size_t N>
Dual<Scalar, N> sqrt(ধ্রুৱ Dual<Scalar, N> &a)
{
	Scalar real = std::sqrt(a.value);
	ঘূৰাই Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
template <typename Scalar, size_t N>
Dual<Scalar, N> abs(ধ্রুৱ Dual<Scalar, N> &a)
{
	ঘূৰাই a.value >= Scalar(0) ? a : -a;
}

// ceil
template <typename Scalar, size_t N>
Dual<Scalar, N> ceil(ধ্রুৱ Dual<Scalar, N> &a)
{
	ঘূৰাই Dual<Scalar, N>(std::ceil(a.value));
}

// floor
template <typename Scalar, size_t N>
Dual<Scalar, N> floor(ধ্রুৱ Dual<Scalar, N> &a)
{
	ঘূৰাই Dual<Scalar, N>(std::floor(a.value));
}

// fmod
template <typename Scalar, size_t N>
Dual<Scalar, N> fmod(ধ্রুৱ Dual<Scalar, N> &a, Scalar mod)
{
	ঘূৰাই Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
template <typename Scalar, size_t N>
Dual<Scalar, N> max(ধ্রুৱ Dual<Scalar, N> &a, ধ্রুৱ Dual<Scalar, N> &b)
{
	ঘূৰাই a.value >= b.value ? a : b;
}

// min
template <typename Scalar, size_t N>
Dual<Scalar, N> min(ধ্রুৱ Dual<Scalar, N> &a, ধ্রুৱ Dual<Scalar, N> &b)
{
	ঘূৰাই a.value < b.value ? a : b;
}

// isnan
template <typename Scalar>
তার্কিক IsNan(Scalar a)
{
	ঘূৰাই std::isnan(a);
}

template <typename Scalar, size_t N>
তার্কিক IsNan(ধ্রুৱ Dual<Scalar, N> &a)
{
	ঘূৰাই IsNan(a.value);
}

// isfinite
template <typename Scalar>
তার্কিক IsFinite(Scalar a)
{
	ঘূৰাই std::isfinite(a);
}

template <typename Scalar, size_t N>
তার্কিক IsFinite(ধ্রুৱ Dual<Scalar, N> &a)
{
	ঘূৰাই IsFinite(a.value);
}

// isinf
template <typename Scalar>
তার্কিক IsInf(Scalar a)
{
	ঘূৰাই std::isinf(a);
}

template <typename Scalar, size_t N>
তার্কিক IsInf(ধ্রুৱ Dual<Scalar, N> &a)
{
	ঘূৰাই IsInf(a.value);
}

// trig

// sin
template <typename Scalar, size_t N>
Dual<Scalar, N> sin(ধ্রুৱ Dual<Scalar, N> &a)
{
	ঘূৰাই Dual<Scalar, N>(std::sin(a.value), std::cos(a.value) * a.derivative);
}

// cos
template <typename Scalar, size_t N>
Dual<Scalar, N> cos(ধ্রুৱ Dual<Scalar, N> &a)
{
	ঘূৰাই Dual<Scalar, N>(std::cos(a.value), -std::sin(a.value) * a.derivative);
}

// tan
template <typename Scalar, size_t N>
Dual<Scalar, N> tan(ধ্রুৱ Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	ঘূৰাই Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
template <typename Scalar, size_t N>
Dual<Scalar, N> asin(ধ্রুৱ Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	ঘূৰাই Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
template <typename Scalar, size_t N>
Dual<Scalar, N> acos(ধ্রুৱ Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	ঘূৰাই Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
template <typename Scalar, size_t N>
Dual<Scalar, N> atan(ধ্রুৱ Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	ঘূৰাই Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
template <typename Scalar, size_t N>
Dual<Scalar, N> atan2(ধ্রুৱ Dual<Scalar, N> &a, ধ্রুৱ Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	ঘূৰাই Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
template <typename Scalar, size_t M, size_t N>
Matrix<Scalar, M, N> collectDerivatives(ধ্রুৱ Matrix<Dual<Scalar, N>, M, 1> &প্ৰবেশ)
{
	Matrix<Scalar, M, N> jac;

	বাবে (size_t i = 0; i < M; i++) {
		jac.row(i) = প্ৰবেশ(i, 0).derivative;
	}

	ঘূৰাই jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
template <typename Scalar, size_t M, size_t N, size_t D>
Matrix<Scalar, M, N> collectReals(ধ্রুৱ Matrix<Dual<Scalar, D>, M, N> &প্ৰবেশ)
{
	Matrix<Scalar, M, N> r;

	বাবে (size_t i = 0; i < M; i++) {
		বাবে (size_t j = 0; j < N; j++) {
			r(i, j) = প্ৰবেশ(i, j).value;
		}
	}

	ঘূৰাই r;
}

template<typename OStream, typename Type, size_t N>
OStream &operator<<(OStream &os, ধ্রুৱ matrix::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	ঘূৰাই os;
}

}
