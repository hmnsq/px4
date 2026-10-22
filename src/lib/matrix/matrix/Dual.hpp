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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <cmath>

#ਅਨ੍ਤਰ੍ਭਾਵ "Scalar.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "Vector.hpp"

ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ
{

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
ਬਣਤਰ Dual {
	ਗਿਣਤੀ : size_t { ਚੌੜਾਈ = N };

	Scalar value {};
	ਸਦਿਸ਼<Scalar, N> derivative;

	Dual() = ਮੂਲ;

	ਸਪਸ਼ਟ Dual(Scalar v, size_t ਨਿਵੇਸ਼_Dimension = 65535)
	{
		value = v;

		ਜੇ (ਨਿਵੇਸ਼_Dimension < N) {
			derivative(ਨਿਵੇਸ਼_Dimension) = Scalar(1);
		}
	}

	ਸਪਸ਼ਟ Dual(Scalar v, ਅਚਲ ਸਦਿਸ਼<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &ਸੰਚਾਲਕ=(ਅਚਲ Scalar &a)
	{
		derivative.ਸੈੱਟ_Zero();
		value = a;
		ਵਾਪਸ *ਇਹ;
	}

	Dual<Scalar, N> &ਸੰਚਾਲਕ +=(ਅਚਲ Dual<Scalar, N> &a)
	{
		ਵਾਪਸ (*ਇਹ = *ਇਹ + a);
	}

	Dual<Scalar, N> &ਸੰਚਾਲਕ *=(ਅਚਲ Dual<Scalar, N> &a)
	{
		ਵਾਪਸ (*ਇਹ = *ਇਹ * a);
	}

	Dual<Scalar, N> &ਸੰਚਾਲਕ -=(ਅਚਲ Dual<Scalar, N> &a)
	{
		ਵਾਪਸ (*ਇਹ = *ਇਹ - a);
	}

	Dual<Scalar, N> &ਸੰਚਾਲਕ /=(ਅਚਲ Dual<Scalar, N> &a)
	{
		ਵਾਪਸ (*ਇਹ = *ਇਹ / a);
	}

	Dual<Scalar, N> &ਸੰਚਾਲਕ +=(Scalar a)
	{
		ਵਾਪਸ (*ਇਹ = *ਇਹ + a);
	}

	Dual<Scalar, N> &ਸੰਚਾਲਕ -=(Scalar a)
	{
		ਵਾਪਸ (*ਇਹ = *ਇਹ - a);
	}

	Dual<Scalar, N> &ਸੰਚਾਲਕ *=(Scalar a)
	{
		ਵਾਪਸ (*ਇਹ = *ਇਹ * a);
	}

	Dual<Scalar, N> &ਸੰਚਾਲਕ /=(Scalar a)
	{
		ਵਾਪਸ (*ਇਹ = *ਇਹ / a);
	}

	ਤਾਰਕਿਕ ਸੰਚਾਲਕ==(ਅਚਲ Dual<Scalar, N> &other) ਅਚਲ
	{
		ਵਾਪਸ isEqualF(value, other.value) && (derivative == other.derivative);
	}

	ਤਾਰਕਿਕ ਸੰਚਾਲਕ!=(ਅਚਲ Dual<Scalar, N> &other) ਅਚਲ
	{
		ਅਚਲ Dual<Scalar, N> &self = *ਇਹ;
		ਵਾਪਸ !(self == other);
	}
};

// operators

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ+(ਅਚਲ Dual<Scalar, N> &a)
{
	ਵਾਪਸ a;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ-(ਅਚਲ Dual<Scalar, N> &a)
{
	ਵਾਪਸ Dual<Scalar, N>(-a.value, -a.derivative);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ+(ਅਚਲ Dual<Scalar, N> &a, ਅਚਲ Dual<Scalar, N> &b)
{
	ਵਾਪਸ Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ-(ਅਚਲ Dual<Scalar, N> &a, ਅਚਲ Dual<Scalar, N> &b)
{
	ਵਾਪਸ a + (-b);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ+(ਅਚਲ Dual<Scalar, N> &a, Scalar b)
{
	ਵਾਪਸ Dual<Scalar, N>(a.value + b, a.derivative);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ-(ਅਚਲ Dual<Scalar, N> &a, Scalar b)
{
	ਵਾਪਸ a + (-b);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ+(Scalar a, ਅਚਲ Dual<Scalar, N> &b)
{
	ਵਾਪਸ Dual<Scalar, N>(a + b.value, b.derivative);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ-(Scalar a, ਅਚਲ Dual<Scalar, N> &b)
{
	ਵਾਪਸ a + (-b);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ*(ਅਚਲ Dual<Scalar, N> &a, ਅਚਲ Dual<Scalar, N> &b)
{
	ਵਾਪਸ Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ*(ਅਚਲ Dual<Scalar, N> &a, Scalar b)
{
	ਵਾਪਸ Dual<Scalar, N>(a.value * b, a.derivative * b);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ*(Scalar a, ਅਚਲ Dual<Scalar, N> &b)
{
	ਵਾਪਸ b * a;
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ/(ਅਚਲ Dual<Scalar, N> &a, ਅਚਲ Dual<Scalar, N> &b)
{
	ਅਚਲ Scalar inv_b_real = Scalar(1) / b.value;
	ਵਾਪਸ Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ/(ਅਚਲ Dual<Scalar, N> &a, Scalar b)
{
	ਵਾਪਸ a * (Scalar(1) / b);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਸੰਚਾਲਕ/(Scalar a, ਅਚਲ Dual<Scalar, N> &b)
{
	ਅਚਲ Scalar inv_b_real = Scalar(1) / b.value;
	ਵਾਪਸ Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਵਰਗਮੂਲ(ਅਚਲ Dual<Scalar, N> &a)
{
	Scalar real = std::ਵਰਗਮੂਲ(a.value);
	ਵਾਪਸ Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਨਿਰਪੇਖ(ਅਚਲ Dual<Scalar, N> &a)
{
	ਵਾਪਸ a.value >= Scalar(0) ? a : -a;
}

// ceil
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ceil(ਅਚਲ Dual<Scalar, N> &a)
{
	ਵਾਪਸ Dual<Scalar, N>(std::ceil(a.value));
}

// floor
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> floor(ਅਚਲ Dual<Scalar, N> &a)
{
	ਵਾਪਸ Dual<Scalar, N>(std::floor(a.value));
}

// fmod
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> fmod(ਅਚਲ Dual<Scalar, N> &a, Scalar mod)
{
	ਵਾਪਸ Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> max(ਅਚਲ Dual<Scalar, N> &a, ਅਚਲ Dual<Scalar, N> &b)
{
	ਵਾਪਸ a.value >= b.value ? a : b;
}

// min
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> min(ਅਚਲ Dual<Scalar, N> &a, ਅਚਲ Dual<Scalar, N> &b)
{
	ਵਾਪਸ a.value < b.value ? a : b;
}

// isnan
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar>
ਤਾਰਕਿਕ IsNan(Scalar a)
{
	ਵਾਪਸ std::isnan(a);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
ਤਾਰਕਿਕ IsNan(ਅਚਲ Dual<Scalar, N> &a)
{
	ਵਾਪਸ IsNan(a.value);
}

// isfinite
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar>
ਤਾਰਕਿਕ IsFinite(Scalar a)
{
	ਵਾਪਸ std::isfinite(a);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
ਤਾਰਕਿਕ IsFinite(ਅਚਲ Dual<Scalar, N> &a)
{
	ਵਾਪਸ IsFinite(a.value);
}

// isinf
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar>
ਤਾਰਕਿਕ IsInf(Scalar a)
{
	ਵਾਪਸ std::isinf(a);
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
ਤਾਰਕਿਕ IsInf(ਅਚਲ Dual<Scalar, N> &a)
{
	ਵਾਪਸ IsInf(a.value);
}

// trig

// sin
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਜੀਵਾ(ਅਚਲ Dual<Scalar, N> &a)
{
	ਵਾਪਸ Dual<Scalar, N>(std::ਜੀਵਾ(a.value), std::ਕੋਜੀਵਾ(a.value) * a.derivative);
}

// cos
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> ਕੋਜੀਵਾ(ਅਚਲ Dual<Scalar, N> &a)
{
	ਵਾਪਸ Dual<Scalar, N>(std::ਕੋਜੀਵਾ(a.value), -std::ਜੀਵਾ(a.value) * a.derivative);
}

// tan
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> tan(ਅਚਲ Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	ਵਾਪਸ Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> asin(ਅਚਲ Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::ਵਰਗਮੂਲ(Scalar(1) - a.value * a.value);
	ਵਾਪਸ Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> acos(ਅਚਲ Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::ਵਰਗਮੂਲ(Scalar(1) - a.value * a.value);
	ਵਾਪਸ Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> atan(ਅਚਲ Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	ਵਾਪਸ Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t N>
Dual<Scalar, N> atan2(ਅਚਲ Dual<Scalar, N> &a, ਅਚਲ Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	ਵਾਪਸ Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t M, size_t N>
ਮੈਟ੍ਰਿਕਸ_2<Scalar, M, N> collectDerivatives(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Dual<Scalar, N>, M, 1> &ਨਿਵੇਸ਼)
{
	ਮੈਟ੍ਰਿਕਸ_2<Scalar, M, N> jac;

	ਲਈ (size_t i = 0; i < M; i++) {
		jac.ਪੰਕਤੀ(i) = ਨਿਵੇਸ਼(i, 0).derivative;
	}

	ਵਾਪਸ jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Scalar, size_t M, size_t N, size_t D>
ਮੈਟ੍ਰਿਕਸ_2<Scalar, M, N> collectReals(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Dual<Scalar, D>, M, N> &ਨਿਵੇਸ਼)
{
	ਮੈਟ੍ਰਿਕਸ_2<Scalar, M, N> r;

	ਲਈ (size_t i = 0; i < M; i++) {
		ਲਈ (size_t j = 0; j < N; j++) {
			r(i, j) = ਨਿਵੇਸ਼(i, j).value;
		}
	}

	ਵਾਪਸ r;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ OStream, ਕਿਸਮ_ਨਾਂ Type, size_t N>
OStream &ਸੰਚਾਲਕ<<(OStream &os, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	ਵਾਪਸ os;
}

}
