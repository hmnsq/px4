/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#సూచన once

#అన్తర్భావ "Vector.hpp"

నామాకాశ మాతృక
{

మూస<రకం_పేరు Type>
తరగతి Vector2 : బహిరంగ సదిశ<Type, 2>
{
బహిరంగ:

	ఉపయోగించి Matrix21 = మాతృక_2<Type, 2, 1>;
	ఉపయోగించి Vector3 = సదిశ<Type, 3>;

	Vector2() = అప్రమేయ;

	Vector2(స్థిరరాశి Matrix21 &other) :
		సదిశ<Type, 2>(other)
	{
	}

	స్పష్టమైన Vector2(స్థిరరాశి Type దత్తాంశం_2[2]) :
		సదిశ<Type, 2>(దత్తాంశం_2)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*ఇది);
		v(0) = x;
		v(1) = y;
	}

	ఉపయోగించి base = సదిశ<Type, 2>;
	ఉపయోగించి base::base;

	స్పష్టమైన Vector2(స్థిరరాశి Vector3 &other)
	{
		Vector2 &v(*ఇది);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(స్థిరరాశి Matrix21 &b) స్థిరరాశి
	{
		స్థిరరాశి Vector2 &a(*ఇది);
		తిరిగి a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 క్రియాకారకం+(Vector2 other) స్థిరరాశి
	{
		తిరిగి Matrix21::క్రియాకారకం+(other);
	}

	Vector2 క్రియాకారకం+(Type scalar) స్థిరరాశి
	{
		తిరిగి Matrix21::క్రియాకారకం+(scalar);
	}

	Vector2 క్రియాకారకం-(Vector2 other) స్థిరరాశి
	{
		తిరిగి Matrix21::క్రియాకారకం-(other);
	}

	Vector2 క్రియాకారకం-(Type scalar) స్థిరరాశి
	{
		తిరిగి Matrix21::క్రియాకారకం-(scalar);
	}

	Vector2 క్రియాకారకం-() స్థిరరాశి
	{
		తిరిగి Matrix21::క్రియాకారకం-();
	}

	Vector2 క్రియాకారకం*(Type scalar) స్థిరరాశి
	{
		తిరిగి Matrix21::క్రియాకారకం*(scalar);
	}

	Type క్రియాకారకం*(Vector2 b) స్థిరరాశి
	{
		తిరిగి సదిశ<Type, 2>::క్రియాకారకం*(b);
	}

	Type క్రియాకారకం%(స్థిరరాశి Matrix21 &b) స్థిరరాశి
	{
		తిరిగి (*ఇది).cross(b);
	}

};

ఉపయోగించి Vector2f = Vector2<దశాంశం>;
ఉపయోగించి Vector2d = Vector2<ద్విగుణ>;

} // namespace matrix
