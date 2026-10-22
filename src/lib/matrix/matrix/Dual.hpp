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

#अन्तर्भाव <cmath>

#अन्तर्भाव "Scalar.hpp"
#अन्तर्भाव "Vector.hpp"

नामाकाश matrix
{

template <typename Scalar, size_t N>
संरचना Dual {
	गणना : size_t { WIDTH = N };

	Scalar value {};
	Vector<Scalar, N> derivative;

	Dual() = स्वभाव;

	explicit Dual(Scalar v, size_t आदान_Dimension = 65535)
	{
		value = v;

		यदि (आदान_Dimension < N) {
			derivative(आदान_Dimension) = Scalar(1);
		}
	}

	explicit Dual(Scalar v, ध्रुव Vector<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &operator=(ध्रुव Scalar &a)
	{
		derivative.setZero();
		value = a;
		प्रत्यागम *एतत्;
	}

	Dual<Scalar, N> &operator +=(ध्रुव Dual<Scalar, N> &a)
	{
		प्रत्यागम (*एतत् = *एतत् + a);
	}

	Dual<Scalar, N> &operator *=(ध्रुव Dual<Scalar, N> &a)
	{
		प्रत्यागम (*एतत् = *एतत् * a);
	}

	Dual<Scalar, N> &operator -=(ध्रुव Dual<Scalar, N> &a)
	{
		प्रत्यागम (*एतत् = *एतत् - a);
	}

	Dual<Scalar, N> &operator /=(ध्रुव Dual<Scalar, N> &a)
	{
		प्रत्यागम (*एतत् = *एतत् / a);
	}

	Dual<Scalar, N> &operator +=(Scalar a)
	{
		प्रत्यागम (*एतत् = *एतत् + a);
	}

	Dual<Scalar, N> &operator -=(Scalar a)
	{
		प्रत्यागम (*एतत् = *एतत् - a);
	}

	Dual<Scalar, N> &operator *=(Scalar a)
	{
		प्रत्यागम (*एतत् = *एतत् * a);
	}

	Dual<Scalar, N> &operator /=(Scalar a)
	{
		प्रत्यागम (*एतत् = *एतत् / a);
	}

	तार्किक operator==(ध्रुव Dual<Scalar, N> &other) ध्रुव
	{
		प्रत्यागम isEqualF(value, other.value) && (derivative == other.derivative);
	}

	तार्किक operator!=(ध्रुव Dual<Scalar, N> &other) ध्रुव
	{
		ध्रुव Dual<Scalar, N> &self = *एतत्;
		प्रत्यागम !(self == other);
	}
};

// operators

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम Dual<Scalar, N>(-a.value, -a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(ध्रुव Dual<Scalar, N> &a, Scalar b)
{
	प्रत्यागम Dual<Scalar, N>(a.value + b, a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(ध्रुव Dual<Scalar, N> &a, Scalar b)
{
	प्रत्यागम a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(Scalar a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम Dual<Scalar, N>(a + b.value, b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(Scalar a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(ध्रुव Dual<Scalar, N> &a, Scalar b)
{
	प्रत्यागम Dual<Scalar, N>(a.value * b, a.derivative * b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(Scalar a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम b * a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	ध्रुव Scalar inv_b_real = Scalar(1) / b.value;
	प्रत्यागम Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(ध्रुव Dual<Scalar, N> &a, Scalar b)
{
	प्रत्यागम a * (Scalar(1) / b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(Scalar a, ध्रुव Dual<Scalar, N> &b)
{
	ध्रुव Scalar inv_b_real = Scalar(1) / b.value;
	प्रत्यागम Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
template <typename Scalar, size_t N>
Dual<Scalar, N> sqrt(ध्रुव Dual<Scalar, N> &a)
{
	Scalar real = std::sqrt(a.value);
	प्रत्यागम Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
template <typename Scalar, size_t N>
Dual<Scalar, N> abs(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम a.value >= Scalar(0) ? a : -a;
}

// ceil
template <typename Scalar, size_t N>
Dual<Scalar, N> ceil(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम Dual<Scalar, N>(std::ceil(a.value));
}

// floor
template <typename Scalar, size_t N>
Dual<Scalar, N> floor(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम Dual<Scalar, N>(std::floor(a.value));
}

// fmod
template <typename Scalar, size_t N>
Dual<Scalar, N> fmod(ध्रुव Dual<Scalar, N> &a, Scalar mod)
{
	प्रत्यागम Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
template <typename Scalar, size_t N>
Dual<Scalar, N> max(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम a.value >= b.value ? a : b;
}

// min
template <typename Scalar, size_t N>
Dual<Scalar, N> min(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम a.value < b.value ? a : b;
}

// isnan
template <typename Scalar>
तार्किक IsNan(Scalar a)
{
	प्रत्यागम std::isnan(a);
}

template <typename Scalar, size_t N>
तार्किक IsNan(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम IsNan(a.value);
}

// isfinite
template <typename Scalar>
तार्किक IsFinite(Scalar a)
{
	प्रत्यागम std::isfinite(a);
}

template <typename Scalar, size_t N>
तार्किक IsFinite(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम IsFinite(a.value);
}

// isinf
template <typename Scalar>
तार्किक IsInf(Scalar a)
{
	प्रत्यागम std::isinf(a);
}

template <typename Scalar, size_t N>
तार्किक IsInf(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम IsInf(a.value);
}

// trig

// sin
template <typename Scalar, size_t N>
Dual<Scalar, N> sin(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम Dual<Scalar, N>(std::sin(a.value), std::cos(a.value) * a.derivative);
}

// cos
template <typename Scalar, size_t N>
Dual<Scalar, N> cos(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम Dual<Scalar, N>(std::cos(a.value), -std::sin(a.value) * a.derivative);
}

// tan
template <typename Scalar, size_t N>
Dual<Scalar, N> tan(ध्रुव Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	प्रत्यागम Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
template <typename Scalar, size_t N>
Dual<Scalar, N> asin(ध्रुव Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	प्रत्यागम Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
template <typename Scalar, size_t N>
Dual<Scalar, N> acos(ध्रुव Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	प्रत्यागम Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
template <typename Scalar, size_t N>
Dual<Scalar, N> atan(ध्रुव Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	प्रत्यागम Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
template <typename Scalar, size_t N>
Dual<Scalar, N> atan2(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	प्रत्यागम Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
template <typename Scalar, size_t M, size_t N>
Matrix<Scalar, M, N> collectDerivatives(ध्रुव Matrix<Dual<Scalar, N>, M, 1> &आदान)
{
	Matrix<Scalar, M, N> jac;

	प्रति (size_t i = 0; i < M; i++) {
		jac.row(i) = आदान(i, 0).derivative;
	}

	प्रत्यागम jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
template <typename Scalar, size_t M, size_t N, size_t D>
Matrix<Scalar, M, N> collectReals(ध्रुव Matrix<Dual<Scalar, D>, M, N> &आदान)
{
	Matrix<Scalar, M, N> r;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			r(i, j) = आदान(i, j).value;
		}
	}

	प्रत्यागम r;
}

template<typename OStream, typename Type, size_t N>
OStream &operator<<(OStream &os, ध्रुव matrix::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	प्रत्यागम os;
}

}
