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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <cmath>

#ಅನ್ತರ್ಭಾವ "Scalar.hpp"
#ಅನ್ತರ್ಭಾವ "Vector.hpp"

ನಾಮಾಕಾಶ ಮಾತೃಕೆ
{

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
ರಚನೆ Dual {
	ಎಣಿಕೆ : size_t { ಅಗಲ = N };

	Scalar value {};
	ಸದಿಶ<Scalar, N> derivative;

	Dual() = ಪೂರ್ವನಿಯೋಜಿತ;

	ಸ್ಪಷ್ಟ Dual(Scalar v, size_t ಒಳಹರಿವು_Dimension = 65535)
	{
		value = v;

		ಆದರೆ (ಒಳಹರಿವು_Dimension < N) {
			derivative(ಒಳಹರಿವು_Dimension) = Scalar(1);
		}
	}

	ಸ್ಪಷ್ಟ Dual(Scalar v, ಸ್ಥಿರಾಂಕ ಸದಿಶ<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Scalar &a)
	{
		derivative.ಗಣ_Zero();
		value = a;
		ಹಿಂತಿರುಗಿ *ಇದು;
	}

	Dual<Scalar, N> &ಕ್ರಿಯಾಕಾರಕ +=(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
	{
		ಹಿಂತಿರುಗಿ (*ಇದು = *ಇದು + a);
	}

	Dual<Scalar, N> &ಕ್ರಿಯಾಕಾರಕ *=(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
	{
		ಹಿಂತಿರುಗಿ (*ಇದು = *ಇದು * a);
	}

	Dual<Scalar, N> &ಕ್ರಿಯಾಕಾರಕ -=(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
	{
		ಹಿಂತಿರುಗಿ (*ಇದು = *ಇದು - a);
	}

	Dual<Scalar, N> &ಕ್ರಿಯಾಕಾರಕ /=(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
	{
		ಹಿಂತಿರುಗಿ (*ಇದು = *ಇದು / a);
	}

	Dual<Scalar, N> &ಕ್ರಿಯಾಕಾರಕ +=(Scalar a)
	{
		ಹಿಂತಿರುಗಿ (*ಇದು = *ಇದು + a);
	}

	Dual<Scalar, N> &ಕ್ರಿಯಾಕಾರಕ -=(Scalar a)
	{
		ಹಿಂತಿರುಗಿ (*ಇದು = *ಇದು - a);
	}

	Dual<Scalar, N> &ಕ್ರಿಯಾಕಾರಕ *=(Scalar a)
	{
		ಹಿಂತಿರುಗಿ (*ಇದು = *ಇದು * a);
	}

	Dual<Scalar, N> &ಕ್ರಿಯಾಕಾರಕ /=(Scalar a)
	{
		ಹಿಂತಿರುಗಿ (*ಇದು = *ಇದು / a);
	}

	ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ isEqualF(value, other.value) && (derivative == other.derivative);
	}

	ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &other) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &self = *ಇದು;
		ಹಿಂತಿರುಗಿ !(self == other);
	}
};

// operators

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ+(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	ಹಿಂತಿರುಗಿ a;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ-(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(-a.value, -a.derivative);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ+(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &b)
{
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ-(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &b)
{
	ಹಿಂತಿರುಗಿ a + (-b);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ+(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, Scalar b)
{
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(a.value + b, a.derivative);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ-(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, Scalar b)
{
	ಹಿಂತಿರುಗಿ a + (-b);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ+(Scalar a, ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &b)
{
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(a + b.value, b.derivative);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ-(Scalar a, ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &b)
{
	ಹಿಂತಿರುಗಿ a + (-b);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ*(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &b)
{
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ*(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, Scalar b)
{
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(a.value * b, a.derivative * b);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ*(Scalar a, ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &b)
{
	ಹಿಂತಿರುಗಿ b * a;
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ/(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &b)
{
	ಸ್ಥಿರಾಂಕ Scalar inv_b_real = Scalar(1) / b.value;
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ/(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, Scalar b)
{
	ಹಿಂತಿರುಗಿ a * (Scalar(1) / b);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕ್ರಿಯಾಕಾರಕ/(Scalar a, ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &b)
{
	ಸ್ಥಿರಾಂಕ Scalar inv_b_real = Scalar(1) / b.value;
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ವರ್ಗಮೂಲ(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	Scalar real = std::ವರ್ಗಮೂಲ(a.value);
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ನಿರಪೇಕ್ಷ(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	ಹಿಂತಿರುಗಿ a.value >= Scalar(0) ? a : -a;
}

// ceil
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ceil(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(std::ceil(a.value));
}

// floor
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> floor(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(std::floor(a.value));
}

// fmod
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> fmod(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, Scalar mod)
{
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> max(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &b)
{
	ಹಿಂತಿರುಗಿ a.value >= b.value ? a : b;
}

// min
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> min(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &b)
{
	ಹಿಂತಿರುಗಿ a.value < b.value ? a : b;
}

// isnan
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar>
ತಾರ್ಕಿಕ IsNan(Scalar a)
{
	ಹಿಂತಿರುಗಿ std::isnan(a);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
ತಾರ್ಕಿಕ IsNan(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	ಹಿಂತಿರುಗಿ IsNan(a.value);
}

// isfinite
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar>
ತಾರ್ಕಿಕ IsFinite(Scalar a)
{
	ಹಿಂತಿರುಗಿ std::isfinite(a);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
ತಾರ್ಕಿಕ IsFinite(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	ಹಿಂತಿರುಗಿ IsFinite(a.value);
}

// isinf
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar>
ತಾರ್ಕಿಕ IsInf(Scalar a)
{
	ಹಿಂತಿರುಗಿ std::isinf(a);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
ತಾರ್ಕಿಕ IsInf(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	ಹಿಂತಿರುಗಿ IsInf(a.value);
}

// trig

// sin
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಜ್ಯಾ(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(std::ಜ್ಯಾ(a.value), std::ಕೋಜ್ಯಾ(a.value) * a.derivative);
}

// cos
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> ಕೋಜ್ಯಾ(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(std::ಕೋಜ್ಯಾ(a.value), -std::ಜ್ಯಾ(a.value) * a.derivative);
}

// tan
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> tan(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> asin(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::ವರ್ಗಮೂಲ(Scalar(1) - a.value * a.value);
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> acos(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::ವರ್ಗಮೂಲ(Scalar(1) - a.value * a.value);
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> atan(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t N>
Dual<Scalar, N> atan2(ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &a, ಸ್ಥಿರಾಂಕ Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	ಹಿಂತಿರುಗಿ Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t M, size_t N>
ಮಾತೃಕೆ_2<Scalar, M, N> collectDerivatives(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<Dual<Scalar, N>, M, 1> &ಒಳಹರಿವು)
{
	ಮಾತೃಕೆ_2<Scalar, M, N> jac;

	ಗಾಗಿ (size_t i = 0; i < M; i++) {
		jac.ಅಡ್ಡಸಾಲು(i) = ಒಳಹರಿವು(i, 0).derivative;
	}

	ಹಿಂತಿರುಗಿ jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar, size_t M, size_t N, size_t D>
ಮಾತೃಕೆ_2<Scalar, M, N> collectReals(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<Dual<Scalar, D>, M, N> &ಒಳಹರಿವು)
{
	ಮಾತೃಕೆ_2<Scalar, M, N> r;

	ಗಾಗಿ (size_t i = 0; i < M; i++) {
		ಗಾಗಿ (size_t j = 0; j < N; j++) {
			r(i, j) = ಒಳಹರಿವು(i, j).value;
		}
	}

	ಹಿಂತಿರುಗಿ r;
}

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು OStream, ಬಗೆ_ಹೆಸರು Type, size_t N>
OStream &ಕ್ರಿಯಾಕಾರಕ<<(OStream &os, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	ಹಿಂತಿರುಗಿ os;
}

}
