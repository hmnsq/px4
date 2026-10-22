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

#निर्देशः once

#अन्तर्भाव <cmath>

#अन्तर्भाव "Scalar.hpp"
#अन्तर्भाव "Vector.hpp"

नामाकाश matrix
{

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
संरचना Dual {
	गणना : size_t { विस्तारः = N };

	Scalar value {};
	सदिशम्<Scalar, N> derivative;

	Dual() = स्वभाव;

	स्पष्ट Dual(Scalar v, size_t निवेशः_Dimension = 65535)
	{
		value = v;

		यदि (निवेशः_Dimension < N) {
			derivative(निवेशः_Dimension) = Scalar(1);
		}
	}

	स्पष्ट Dual(Scalar v, ध्रुव सदिशम्<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &संक्रियकः=(ध्रुव Scalar &a)
	{
		derivative.समुच्चयः_Zero();
		value = a;
		प्रत्यागम *एतत्;
	}

	Dual<Scalar, N> &संक्रियकः +=(ध्रुव Dual<Scalar, N> &a)
	{
		प्रत्यागम (*एतत् = *एतत् + a);
	}

	Dual<Scalar, N> &संक्रियकः *=(ध्रुव Dual<Scalar, N> &a)
	{
		प्रत्यागम (*एतत् = *एतत् * a);
	}

	Dual<Scalar, N> &संक्रियकः -=(ध्रुव Dual<Scalar, N> &a)
	{
		प्रत्यागम (*एतत् = *एतत् - a);
	}

	Dual<Scalar, N> &संक्रियकः /=(ध्रुव Dual<Scalar, N> &a)
	{
		प्रत्यागम (*एतत् = *एतत् / a);
	}

	Dual<Scalar, N> &संक्रियकः +=(Scalar a)
	{
		प्रत्यागम (*एतत् = *एतत् + a);
	}

	Dual<Scalar, N> &संक्रियकः -=(Scalar a)
	{
		प्रत्यागम (*एतत् = *एतत् - a);
	}

	Dual<Scalar, N> &संक्रियकः *=(Scalar a)
	{
		प्रत्यागम (*एतत् = *एतत् * a);
	}

	Dual<Scalar, N> &संक्रियकः /=(Scalar a)
	{
		प्रत्यागम (*एतत् = *एतत् / a);
	}

	तार्किक संक्रियकः==(ध्रुव Dual<Scalar, N> &other) ध्रुव
	{
		प्रत्यागम isEqualF(value, other.value) && (derivative == other.derivative);
	}

	तार्किक संक्रियकः!=(ध्रुव Dual<Scalar, N> &other) ध्रुव
	{
		ध्रुव Dual<Scalar, N> &self = *एतत्;
		प्रत्यागम !(self == other);
	}
};

// operators

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः+(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम a;
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः-(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम Dual<Scalar, N>(-a.value, -a.derivative);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः+(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः-(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम a + (-b);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः+(ध्रुव Dual<Scalar, N> &a, Scalar b)
{
	प्रत्यागम Dual<Scalar, N>(a.value + b, a.derivative);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः-(ध्रुव Dual<Scalar, N> &a, Scalar b)
{
	प्रत्यागम a + (-b);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः+(Scalar a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम Dual<Scalar, N>(a + b.value, b.derivative);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः-(Scalar a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम a + (-b);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः*(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः*(ध्रुव Dual<Scalar, N> &a, Scalar b)
{
	प्रत्यागम Dual<Scalar, N>(a.value * b, a.derivative * b);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः*(Scalar a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम b * a;
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः/(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	ध्रुव Scalar inv_b_real = Scalar(1) / b.value;
	प्रत्यागम Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः/(ध्रुव Dual<Scalar, N> &a, Scalar b)
{
	प्रत्यागम a * (Scalar(1) / b);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> संक्रियकः/(Scalar a, ध्रुव Dual<Scalar, N> &b)
{
	ध्रुव Scalar inv_b_real = Scalar(1) / b.value;
	प्रत्यागम Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> वर्गमूलम्(ध्रुव Dual<Scalar, N> &a)
{
	Scalar real = std::वर्गमूलम्(a.value);
	प्रत्यागम Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> निरपेक्षम्(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम a.value >= Scalar(0) ? a : -a;
}

// ceil
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> ceil(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम Dual<Scalar, N>(std::ceil(a.value));
}

// floor
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> floor(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम Dual<Scalar, N>(std::floor(a.value));
}

// fmod
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> fmod(ध्रुव Dual<Scalar, N> &a, Scalar mod)
{
	प्रत्यागम Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> max(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम a.value >= b.value ? a : b;
}

// min
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> min(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	प्रत्यागम a.value < b.value ? a : b;
}

// isnan
प्रतिरूपम् <प्रकारनाम Scalar>
तार्किक IsNan(Scalar a)
{
	प्रत्यागम std::isnan(a);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
तार्किक IsNan(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम IsNan(a.value);
}

// isfinite
प्रतिरूपम् <प्रकारनाम Scalar>
तार्किक IsFinite(Scalar a)
{
	प्रत्यागम std::isfinite(a);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
तार्किक IsFinite(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम IsFinite(a.value);
}

// isinf
प्रतिरूपम् <प्रकारनाम Scalar>
तार्किक IsInf(Scalar a)
{
	प्रत्यागम std::isinf(a);
}

प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
तार्किक IsInf(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम IsInf(a.value);
}

// trig

// sin
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> ज्या(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम Dual<Scalar, N>(std::ज्या(a.value), std::कोटिज्या(a.value) * a.derivative);
}

// cos
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> कोटिज्या(ध्रुव Dual<Scalar, N> &a)
{
	प्रत्यागम Dual<Scalar, N>(std::कोटिज्या(a.value), -std::ज्या(a.value) * a.derivative);
}

// tan
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> tan(ध्रुव Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	प्रत्यागम Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> asin(ध्रुव Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::वर्गमूलम्(Scalar(1) - a.value * a.value);
	प्रत्यागम Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> acos(ध्रुव Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::वर्गमूलम्(Scalar(1) - a.value * a.value);
	प्रत्यागम Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> atan(ध्रुव Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	प्रत्यागम Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
प्रतिरूपम् <प्रकारनाम Scalar, size_t N>
Dual<Scalar, N> atan2(ध्रुव Dual<Scalar, N> &a, ध्रुव Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	प्रत्यागम Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
प्रतिरूपम् <प्रकारनाम Scalar, size_t M, size_t N>
Matrix<Scalar, M, N> collectDerivatives(ध्रुव Matrix<Dual<Scalar, N>, M, 1> &निवेशः)
{
	Matrix<Scalar, M, N> jac;

	प्रति (size_t i = 0; i < M; i++) {
		jac.row(i) = निवेशः(i, 0).derivative;
	}

	प्रत्यागम jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
प्रतिरूपम् <प्रकारनाम Scalar, size_t M, size_t N, size_t D>
Matrix<Scalar, M, N> collectReals(ध्रुव Matrix<Dual<Scalar, D>, M, N> &निवेशः)
{
	Matrix<Scalar, M, N> r;

	प्रति (size_t i = 0; i < M; i++) {
		प्रति (size_t j = 0; j < N; j++) {
			r(i, j) = निवेशः(i, j).value;
		}
	}

	प्रत्यागम r;
}

प्रतिरूपम्<प्रकारनाम OStream, प्रकारनाम Type, size_t N>
OStream &संक्रियकः<<(OStream &os, ध्रुव matrix::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	प्रत्यागम os;
}

}
