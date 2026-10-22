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

#ಅನ್ತರ್ಭಾವ <cmath>

#ಅನ್ತರ್ಭಾವ "Scalar.hpp"
#ಅನ್ತರ್ಭಾವ "Vector.hpp"

ನಾಮಾಕಾಶ matrix
{

template <typename Scalar, size_t N>
ಸಂರಚನಾ Dual {
	ಗಣನಾ : size_t { WIDTH = N };

	Scalar value {};
	Vector<Scalar, N> derivative;

	Dual() = ಸ್ವಭಾವ;

	explicit Dual(Scalar v, size_t ಒಳಹರಿವು_Dimension = 65535)
	{
		value = v;

		ಯದಿ (ಒಳಹರಿವು_Dimension < N) {
			derivative(ಒಳಹರಿವು_Dimension) = Scalar(1);
		}
	}

	explicit Dual(Scalar v, ಧ್ರುವ Vector<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &operator=(ಧ್ರುವ Scalar &a)
	{
		derivative.setZero();
		value = a;
		ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
	}

	Dual<Scalar, N> &operator +=(ಧ್ರುವ Dual<Scalar, N> &a)
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್ = *ಏತತ್ + a);
	}

	Dual<Scalar, N> &operator *=(ಧ್ರುವ Dual<Scalar, N> &a)
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್ = *ಏತತ್ * a);
	}

	Dual<Scalar, N> &operator -=(ಧ್ರುವ Dual<Scalar, N> &a)
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್ = *ಏತತ್ - a);
	}

	Dual<Scalar, N> &operator /=(ಧ್ರುವ Dual<Scalar, N> &a)
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್ = *ಏತತ್ / a);
	}

	Dual<Scalar, N> &operator +=(Scalar a)
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್ = *ಏತತ್ + a);
	}

	Dual<Scalar, N> &operator -=(Scalar a)
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್ = *ಏತತ್ - a);
	}

	Dual<Scalar, N> &operator *=(Scalar a)
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್ = *ಏತತ್ * a);
	}

	Dual<Scalar, N> &operator /=(Scalar a)
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್ = *ಏತತ್ / a);
	}

	ತಾರ್ಕಿಕ operator==(ಧ್ರುವ Dual<Scalar, N> &other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ isEqualF(value, other.value) && (derivative == other.derivative);
	}

	ತಾರ್ಕಿಕ operator!=(ಧ್ರುವ Dual<Scalar, N> &other) ಧ್ರುವ
	{
		ಧ್ರುವ Dual<Scalar, N> &self = *ಏತತ್;
		ಪ್ರತ್ಯಾಗಮ !(self == other);
	}
};

// operators

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ಧ್ರುವ Dual<Scalar, N> &a)
{
	ಪ್ರತ್ಯಾಗಮ a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ಧ್ರುವ Dual<Scalar, N> &a)
{
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(-a.value, -a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ಧ್ರುವ Dual<Scalar, N> &a, ಧ್ರುವ Dual<Scalar, N> &b)
{
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ಧ್ರುವ Dual<Scalar, N> &a, ಧ್ರುವ Dual<Scalar, N> &b)
{
	ಪ್ರತ್ಯಾಗಮ a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ಧ್ರುವ Dual<Scalar, N> &a, Scalar b)
{
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(a.value + b, a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ಧ್ರುವ Dual<Scalar, N> &a, Scalar b)
{
	ಪ್ರತ್ಯಾಗಮ a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(Scalar a, ಧ್ರುವ Dual<Scalar, N> &b)
{
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(a + b.value, b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(Scalar a, ಧ್ರುವ Dual<Scalar, N> &b)
{
	ಪ್ರತ್ಯಾಗಮ a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ಧ್ರುವ Dual<Scalar, N> &a, ಧ್ರುವ Dual<Scalar, N> &b)
{
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ಧ್ರುವ Dual<Scalar, N> &a, Scalar b)
{
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(a.value * b, a.derivative * b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(Scalar a, ಧ್ರುವ Dual<Scalar, N> &b)
{
	ಪ್ರತ್ಯಾಗಮ b * a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ಧ್ರುವ Dual<Scalar, N> &a, ಧ್ರುವ Dual<Scalar, N> &b)
{
	ಧ್ರುವ Scalar inv_b_real = Scalar(1) / b.value;
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ಧ್ರುವ Dual<Scalar, N> &a, Scalar b)
{
	ಪ್ರತ್ಯಾಗಮ a * (Scalar(1) / b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(Scalar a, ಧ್ರುವ Dual<Scalar, N> &b)
{
	ಧ್ರುವ Scalar inv_b_real = Scalar(1) / b.value;
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
template <typename Scalar, size_t N>
Dual<Scalar, N> sqrt(ಧ್ರುವ Dual<Scalar, N> &a)
{
	Scalar real = std::sqrt(a.value);
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
template <typename Scalar, size_t N>
Dual<Scalar, N> abs(ಧ್ರುವ Dual<Scalar, N> &a)
{
	ಪ್ರತ್ಯಾಗಮ a.value >= Scalar(0) ? a : -a;
}

// ceil
template <typename Scalar, size_t N>
Dual<Scalar, N> ceil(ಧ್ರುವ Dual<Scalar, N> &a)
{
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(std::ceil(a.value));
}

// floor
template <typename Scalar, size_t N>
Dual<Scalar, N> floor(ಧ್ರುವ Dual<Scalar, N> &a)
{
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(std::floor(a.value));
}

// fmod
template <typename Scalar, size_t N>
Dual<Scalar, N> fmod(ಧ್ರುವ Dual<Scalar, N> &a, Scalar mod)
{
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
template <typename Scalar, size_t N>
Dual<Scalar, N> max(ಧ್ರುವ Dual<Scalar, N> &a, ಧ್ರುವ Dual<Scalar, N> &b)
{
	ಪ್ರತ್ಯಾಗಮ a.value >= b.value ? a : b;
}

// min
template <typename Scalar, size_t N>
Dual<Scalar, N> min(ಧ್ರುವ Dual<Scalar, N> &a, ಧ್ರುವ Dual<Scalar, N> &b)
{
	ಪ್ರತ್ಯಾಗಮ a.value < b.value ? a : b;
}

// isnan
template <typename Scalar>
ತಾರ್ಕಿಕ IsNan(Scalar a)
{
	ಪ್ರತ್ಯಾಗಮ std::isnan(a);
}

template <typename Scalar, size_t N>
ತಾರ್ಕಿಕ IsNan(ಧ್ರುವ Dual<Scalar, N> &a)
{
	ಪ್ರತ್ಯಾಗಮ IsNan(a.value);
}

// isfinite
template <typename Scalar>
ತಾರ್ಕಿಕ IsFinite(Scalar a)
{
	ಪ್ರತ್ಯಾಗಮ std::isfinite(a);
}

template <typename Scalar, size_t N>
ತಾರ್ಕಿಕ IsFinite(ಧ್ರುವ Dual<Scalar, N> &a)
{
	ಪ್ರತ್ಯಾಗಮ IsFinite(a.value);
}

// isinf
template <typename Scalar>
ತಾರ್ಕಿಕ IsInf(Scalar a)
{
	ಪ್ರತ್ಯಾಗಮ std::isinf(a);
}

template <typename Scalar, size_t N>
ತಾರ್ಕಿಕ IsInf(ಧ್ರುವ Dual<Scalar, N> &a)
{
	ಪ್ರತ್ಯಾಗಮ IsInf(a.value);
}

// trig

// sin
template <typename Scalar, size_t N>
Dual<Scalar, N> sin(ಧ್ರುವ Dual<Scalar, N> &a)
{
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(std::sin(a.value), std::cos(a.value) * a.derivative);
}

// cos
template <typename Scalar, size_t N>
Dual<Scalar, N> cos(ಧ್ರುವ Dual<Scalar, N> &a)
{
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(std::cos(a.value), -std::sin(a.value) * a.derivative);
}

// tan
template <typename Scalar, size_t N>
Dual<Scalar, N> tan(ಧ್ರುವ Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
template <typename Scalar, size_t N>
Dual<Scalar, N> asin(ಧ್ರುವ Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
template <typename Scalar, size_t N>
Dual<Scalar, N> acos(ಧ್ರುವ Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
template <typename Scalar, size_t N>
Dual<Scalar, N> atan(ಧ್ರುವ Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
template <typename Scalar, size_t N>
Dual<Scalar, N> atan2(ಧ್ರುವ Dual<Scalar, N> &a, ಧ್ರುವ Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	ಪ್ರತ್ಯಾಗಮ Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
template <typename Scalar, size_t M, size_t N>
Matrix<Scalar, M, N> collectDerivatives(ಧ್ರುವ Matrix<Dual<Scalar, N>, M, 1> &ಒಳಹರಿವು)
{
	Matrix<Scalar, M, N> jac;

	ಪ್ರತಿ (size_t i = 0; i < M; i++) {
		jac.row(i) = ಒಳಹರಿವು(i, 0).derivative;
	}

	ಪ್ರತ್ಯಾಗಮ jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
template <typename Scalar, size_t M, size_t N, size_t D>
Matrix<Scalar, M, N> collectReals(ಧ್ರುವ Matrix<Dual<Scalar, D>, M, N> &ಒಳಹರಿವು)
{
	Matrix<Scalar, M, N> r;

	ಪ್ರತಿ (size_t i = 0; i < M; i++) {
		ಪ್ರತಿ (size_t j = 0; j < N; j++) {
			r(i, j) = ಒಳಹರಿವು(i, j).value;
		}
	}

	ಪ್ರತ್ಯಾಗಮ r;
}

template<typename OStream, typename Type, size_t N>
OStream &operator<<(OStream &os, ಧ್ರುವ matrix::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	ಪ್ರತ್ಯಾಗಮ os;
}

}
