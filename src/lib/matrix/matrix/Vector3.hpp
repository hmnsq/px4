/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#సూచన once

#అన్తర్భావ "Vector.hpp"

నామాకాశ మాతృక
{

మూస <రకం_పేరు Type>
తరగతి Dcm;

మూస<రకం_పేరు Type>
తరగతి Vector3 : బహిరంగ సదిశ<Type, 3>
{
బహిరంగ:

	ఉపయోగించి Matrix31 = మాతృక_2<Type, 3, 1>;

	Vector3() = అప్రమేయ;

	Vector3(స్థిరరాశి Matrix31 &other) :
		సదిశ<Type, 3>(other)
	{
	}

	స్పష్టమైన Vector3(స్థిరరాశి Type దత్తాంశం_2[3]) :
		సదిశ<Type, 3>(దత్తాంశం_2)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*ఇది);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	ఉపయోగించి base = సదిశ<Type, 3>;
	ఉపయోగించి base::base;

	Vector3 cross(స్థిరరాశి Matrix31 &b) స్థిరరాశి
	{
		స్థిరరాశి Vector3 &a(*ఇది);
		తిరిగి {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	అంతఃవరుస Vector3 క్రియాకారకం+(Vector3 other) స్థిరరాశి
	{
		తిరిగి Matrix31::క్రియాకారకం+(other);
	}

	అంతఃవరుస Vector3 క్రియాకారకం+(Type scalar) స్థిరరాశి
	{
		తిరిగి Matrix31::క్రియాకారకం+(scalar);
	}

	అంతఃవరుస Vector3 క్రియాకారకం-(Vector3 other) స్థిరరాశి
	{
		తిరిగి Matrix31::క్రియాకారకం-(other);
	}

	అంతఃవరుస Vector3 క్రియాకారకం-(Type scalar) స్థిరరాశి
	{
		తిరిగి Matrix31::క్రియాకారకం-(scalar);
	}

	అంతఃవరుస Vector3 క్రియాకారకం-() స్థిరరాశి
	{
		తిరిగి Matrix31::క్రియాకారకం-();
	}

	అంతఃవరుస Vector3 క్రియాకారకం*(Type scalar) స్థిరరాశి
	{
		తిరిగి Matrix31::క్రియాకారకం*(scalar);
	}

	అంతఃవరుస Type క్రియాకారకం*(Vector3 b) స్థిరరాశి
	{
		తిరిగి సదిశ<Type, 3>::క్రియాకారకం*(b);
	}

	అంతఃవరుస Vector3 క్రియాకారకం%(స్థిరరాశి Matrix31 &b) స్థిరరాశి
	{
		తిరిగి (*ఇది).cross(b);
	}

	Const_ముక్క<Type, 2, 1, 3, 1> xy() స్థిరరాశి
	{
		తిరిగి {0, 0, ఇది};
	}

	ముక్క_2<Type, 2, 1, 3, 1> xy()
	{
		తిరిగి {0, 0, ఇది};
	}

	Dcm<Type> hat() స్థిరరాశి      // inverse to Dcm.vee() operation
	{
		స్థిరరాశి Vector3 &v(*ఇది);
		Dcm<Type> A;
		A(0, 0) = 0;
		A(0, 1) = -v(2);
		A(0, 2) = v(1);
		A(1, 0) = v(2);
		A(1, 1) = 0;
		A(1, 2) = -v(0);
		A(2, 0) = -v(1);
		A(2, 1) = v(0);
		A(2, 2) = 0;
		తిరిగి A;
	}

};

ఉపయోగించి Vector3f = Vector3<దశాంశం>;
ఉపయోగించి Vector3d = Vector3<ద్విగుణ>;

} // namespace matrix
