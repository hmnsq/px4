/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#సూచన once

#అన్తర్భావ "Matrix.hpp"

నామాకాశ మాతృక
{

మూస<రకం_పేరు Type>
తరగతి Scalar
{
బహిరంగ:
	Scalar() = తొలగించు;

	Scalar(స్థిరరాశి మాతృక_2<Type, 1, 1> &other) :
		విలువ_2{other(0, 0)}
	{
	}

	Scalar(Type other) : విలువ_2(other)
	{
	}

	క్రియాకారకం స్థిరరాశి Type &()
	{
		తిరిగి విలువ_2;
	}

	క్రియాకారకం మాతృక_2<Type, 1, 1>() స్థిరరాశి
	{
		మాతృక_2<Type, 1, 1> m;
		m(0, 0) = విలువ_2;
		తిరిగి m;
	}

	క్రియాకారకం సదిశ<Type, 1>() స్థిరరాశి
	{
		సదిశ<Type, 1> m;
		m(0) = విలువ_2;
		తిరిగి m;
	}

	తార్కిక క్రియాకారకం==(స్థిరరాశి దశాంశం other) స్థిరరాశి
	{
		తిరిగి isEqualF(విలువ_2, other);
	}

అంతరంగ:
	స్థిరరాశి Type విలువ_2;

};

ఉపయోగించి Scalarf = Scalar<దశాంశం>;
ఉపయోగించి Scalard = Scalar<ద్విగుణ>;

} // namespace matrix
