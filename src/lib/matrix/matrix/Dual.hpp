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

#సూచన once

#అన్తర్భావ <cmath>

#అన్తర్భావ "Scalar.hpp"
#అన్తర్భావ "Vector.hpp"

నామాకాశ మాతృక
{

మూస <రకం_పేరు Scalar, size_t N>
నిర్మాణం Dual {
	గణన : size_t { వెడల్పు = N };

	Scalar value {};
	సదిశ<Scalar, N> derivative;

	Dual() = అప్రమేయ;

	స్పష్టమైన Dual(Scalar v, size_t ఆదానం_Dimension = 65535)
	{
		value = v;

		ఒకవేళ (ఆదానం_Dimension < N) {
			derivative(ఆదానం_Dimension) = Scalar(1);
		}
	}

	స్పష్టమైన Dual(Scalar v, స్థిరరాశి సదిశ<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	Dual<Scalar, N> &క్రియాకారకం=(స్థిరరాశి Scalar &a)
	{
		derivative.సమితి_Zero();
		value = a;
		తిరిగి *ఇది;
	}

	Dual<Scalar, N> &క్రియాకారకం +=(స్థిరరాశి Dual<Scalar, N> &a)
	{
		తిరిగి (*ఇది = *ఇది + a);
	}

	Dual<Scalar, N> &క్రియాకారకం *=(స్థిరరాశి Dual<Scalar, N> &a)
	{
		తిరిగి (*ఇది = *ఇది * a);
	}

	Dual<Scalar, N> &క్రియాకారకం -=(స్థిరరాశి Dual<Scalar, N> &a)
	{
		తిరిగి (*ఇది = *ఇది - a);
	}

	Dual<Scalar, N> &క్రియాకారకం /=(స్థిరరాశి Dual<Scalar, N> &a)
	{
		తిరిగి (*ఇది = *ఇది / a);
	}

	Dual<Scalar, N> &క్రియాకారకం +=(Scalar a)
	{
		తిరిగి (*ఇది = *ఇది + a);
	}

	Dual<Scalar, N> &క్రియాకారకం -=(Scalar a)
	{
		తిరిగి (*ఇది = *ఇది - a);
	}

	Dual<Scalar, N> &క్రియాకారకం *=(Scalar a)
	{
		తిరిగి (*ఇది = *ఇది * a);
	}

	Dual<Scalar, N> &క్రియాకారకం /=(Scalar a)
	{
		తిరిగి (*ఇది = *ఇది / a);
	}

	తార్కిక క్రియాకారకం==(స్థిరరాశి Dual<Scalar, N> &other) స్థిరరాశి
	{
		తిరిగి isEqualF(value, other.value) && (derivative == other.derivative);
	}

	తార్కిక క్రియాకారకం!=(స్థిరరాశి Dual<Scalar, N> &other) స్థిరరాశి
	{
		స్థిరరాశి Dual<Scalar, N> &self = *ఇది;
		తిరిగి !(self == other);
	}
};

// operators

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం+(స్థిరరాశి Dual<Scalar, N> &a)
{
	తిరిగి a;
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం-(స్థిరరాశి Dual<Scalar, N> &a)
{
	తిరిగి Dual<Scalar, N>(-a.value, -a.derivative);
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం+(స్థిరరాశి Dual<Scalar, N> &a, స్థిరరాశి Dual<Scalar, N> &b)
{
	తిరిగి Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం-(స్థిరరాశి Dual<Scalar, N> &a, స్థిరరాశి Dual<Scalar, N> &b)
{
	తిరిగి a + (-b);
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం+(స్థిరరాశి Dual<Scalar, N> &a, Scalar b)
{
	తిరిగి Dual<Scalar, N>(a.value + b, a.derivative);
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం-(స్థిరరాశి Dual<Scalar, N> &a, Scalar b)
{
	తిరిగి a + (-b);
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం+(Scalar a, స్థిరరాశి Dual<Scalar, N> &b)
{
	తిరిగి Dual<Scalar, N>(a + b.value, b.derivative);
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం-(Scalar a, స్థిరరాశి Dual<Scalar, N> &b)
{
	తిరిగి a + (-b);
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం*(స్థిరరాశి Dual<Scalar, N> &a, స్థిరరాశి Dual<Scalar, N> &b)
{
	తిరిగి Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం*(స్థిరరాశి Dual<Scalar, N> &a, Scalar b)
{
	తిరిగి Dual<Scalar, N>(a.value * b, a.derivative * b);
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం*(Scalar a, స్థిరరాశి Dual<Scalar, N> &b)
{
	తిరిగి b * a;
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం/(స్థిరరాశి Dual<Scalar, N> &a, స్థిరరాశి Dual<Scalar, N> &b)
{
	స్థిరరాశి Scalar inv_b_real = Scalar(1) / b.value;
	తిరిగి Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం/(స్థిరరాశి Dual<Scalar, N> &a, Scalar b)
{
	తిరిగి a * (Scalar(1) / b);
}

మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> క్రియాకారకం/(Scalar a, స్థిరరాశి Dual<Scalar, N> &b)
{
	స్థిరరాశి Scalar inv_b_real = Scalar(1) / b.value;
	తిరిగి Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

// basic math

// sqrt
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> వర్గమూలం(స్థిరరాశి Dual<Scalar, N> &a)
{
	Scalar real = std::వర్గమూలం(a.value);
	తిరిగి Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

// abs
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> నిరపేక్ష(స్థిరరాశి Dual<Scalar, N> &a)
{
	తిరిగి a.value >= Scalar(0) ? a : -a;
}

// ceil
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> ceil(స్థిరరాశి Dual<Scalar, N> &a)
{
	తిరిగి Dual<Scalar, N>(std::ceil(a.value));
}

// floor
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> floor(స్థిరరాశి Dual<Scalar, N> &a)
{
	తిరిగి Dual<Scalar, N>(std::floor(a.value));
}

// fmod
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> fmod(స్థిరరాశి Dual<Scalar, N> &a, Scalar mod)
{
	తిరిగి Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

// max
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> max(స్థిరరాశి Dual<Scalar, N> &a, స్థిరరాశి Dual<Scalar, N> &b)
{
	తిరిగి a.value >= b.value ? a : b;
}

// min
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> min(స్థిరరాశి Dual<Scalar, N> &a, స్థిరరాశి Dual<Scalar, N> &b)
{
	తిరిగి a.value < b.value ? a : b;
}

// isnan
మూస <రకం_పేరు Scalar>
తార్కిక IsNan(Scalar a)
{
	తిరిగి std::isnan(a);
}

మూస <రకం_పేరు Scalar, size_t N>
తార్కిక IsNan(స్థిరరాశి Dual<Scalar, N> &a)
{
	తిరిగి IsNan(a.value);
}

// isfinite
మూస <రకం_పేరు Scalar>
తార్కిక IsFinite(Scalar a)
{
	తిరిగి std::isfinite(a);
}

మూస <రకం_పేరు Scalar, size_t N>
తార్కిక IsFinite(స్థిరరాశి Dual<Scalar, N> &a)
{
	తిరిగి IsFinite(a.value);
}

// isinf
మూస <రకం_పేరు Scalar>
తార్కిక IsInf(Scalar a)
{
	తిరిగి std::isinf(a);
}

మూస <రకం_పేరు Scalar, size_t N>
తార్కిక IsInf(స్థిరరాశి Dual<Scalar, N> &a)
{
	తిరిగి IsInf(a.value);
}

// trig

// sin
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> జ్యా(స్థిరరాశి Dual<Scalar, N> &a)
{
	తిరిగి Dual<Scalar, N>(std::జ్యా(a.value), std::కోజ్యా(a.value) * a.derivative);
}

// cos
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> కోజ్యా(స్థిరరాశి Dual<Scalar, N> &a)
{
	తిరిగి Dual<Scalar, N>(std::కోజ్యా(a.value), -std::జ్యా(a.value) * a.derivative);
}

// tan
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> tan(స్థిరరాశి Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	తిరిగి Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

// asin
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> asin(స్థిరరాశి Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::వర్గమూలం(Scalar(1) - a.value * a.value);
	తిరిగి Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

// acos
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> acos(స్థిరరాశి Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::వర్గమూలం(Scalar(1) - a.value * a.value);
	తిరిగి Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

// atan
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> atan(స్థిరరాశి Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	తిరిగి Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

// atan2
మూస <రకం_పేరు Scalar, size_t N>
Dual<Scalar, N> atan2(స్థిరరాశి Dual<Scalar, N> &a, స్థిరరాశి Dual<Scalar, N> &b)
{
	// derivative is equal to that of atan(a/b), so substitute a/b into atan and simplify
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	తిరిగి Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

// retrieve the derivative elements of a vector of Duals into a matrix
మూస <రకం_పేరు Scalar, size_t M, size_t N>
మాతృక_2<Scalar, M, N> collectDerivatives(స్థిరరాశి మాతృక_2<Dual<Scalar, N>, M, 1> &ఆదానం)
{
	మాతృక_2<Scalar, M, N> jac;

	కొరకు (size_t i = 0; i < M; i++) {
		jac.అడ్డువరుస(i) = ఆదానం(i, 0).derivative;
	}

	తిరిగి jac;
}

// retrieve the real (non-derivative) elements of a matrix of Duals into an equally sized matrix
మూస <రకం_పేరు Scalar, size_t M, size_t N, size_t D>
మాతృక_2<Scalar, M, N> collectReals(స్థిరరాశి మాతృక_2<Dual<Scalar, D>, M, N> &ఆదానం)
{
	మాతృక_2<Scalar, M, N> r;

	కొరకు (size_t i = 0; i < M; i++) {
		కొరకు (size_t j = 0; j < N; j++) {
			r(i, j) = ఆదానం(i, j).value;
		}
	}

	తిరిగి r;
}

మూస<రకం_పేరు OStream, రకం_పేరు Type, size_t N>
OStream &క్రియాకారకం<<(OStream &os, స్థిరరాశి మాతృక::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	తిరిగి os;
}

}
