/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "Vector.hpp"

ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ
{

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Type>
ਵਰਗ Dcm;

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type>
ਵਰਗ Vector3 : ਜਨਤਕ ਸਦਿਸ਼<Type, 3>
{
ਜਨਤਕ:

	ਵਰਤੋਂ Matrix31 = ਮੈਟ੍ਰਿਕਸ_2<Type, 3, 1>;

	Vector3() = ਮੂਲ;

	Vector3(ਅਚਲ Matrix31 &other) :
		ਸਦਿਸ਼<Type, 3>(other)
	{
	}

	ਸਪਸ਼ਟ Vector3(ਅਚਲ Type ਅੰਕੜੇ_2[3]) :
		ਸਦਿਸ਼<Type, 3>(ਅੰਕੜੇ_2)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*ਇਹ);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	ਵਰਤੋਂ base = ਸਦਿਸ਼<Type, 3>;
	ਵਰਤੋਂ base::base;

	Vector3 cross(ਅਚਲ Matrix31 &b) ਅਚਲ
	{
		ਅਚਲ Vector3 &a(*ਇਹ);
		ਵਾਪਸ {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	ਸਤਰਅੰਦਰ Vector3 ਸੰਚਾਲਕ+(Vector3 other) ਅਚਲ
	{
		ਵਾਪਸ Matrix31::ਸੰਚਾਲਕ+(other);
	}

	ਸਤਰਅੰਦਰ Vector3 ਸੰਚਾਲਕ+(Type scalar) ਅਚਲ
	{
		ਵਾਪਸ Matrix31::ਸੰਚਾਲਕ+(scalar);
	}

	ਸਤਰਅੰਦਰ Vector3 ਸੰਚਾਲਕ-(Vector3 other) ਅਚਲ
	{
		ਵਾਪਸ Matrix31::ਸੰਚਾਲਕ-(other);
	}

	ਸਤਰਅੰਦਰ Vector3 ਸੰਚਾਲਕ-(Type scalar) ਅਚਲ
	{
		ਵਾਪਸ Matrix31::ਸੰਚਾਲਕ-(scalar);
	}

	ਸਤਰਅੰਦਰ Vector3 ਸੰਚਾਲਕ-() ਅਚਲ
	{
		ਵਾਪਸ Matrix31::ਸੰਚਾਲਕ-();
	}

	ਸਤਰਅੰਦਰ Vector3 ਸੰਚਾਲਕ*(Type scalar) ਅਚਲ
	{
		ਵਾਪਸ Matrix31::ਸੰਚਾਲਕ*(scalar);
	}

	ਸਤਰਅੰਦਰ Type ਸੰਚਾਲਕ*(Vector3 b) ਅਚਲ
	{
		ਵਾਪਸ ਸਦਿਸ਼<Type, 3>::ਸੰਚਾਲਕ*(b);
	}

	ਸਤਰਅੰਦਰ Vector3 ਸੰਚਾਲਕ%(ਅਚਲ Matrix31 &b) ਅਚਲ
	{
		ਵਾਪਸ (*ਇਹ).cross(b);
	}

	Const_ਟੁਕੜਾ<Type, 2, 1, 3, 1> xy() ਅਚਲ
	{
		ਵਾਪਸ {0, 0, ਇਹ};
	}

	ਟੁਕੜਾ_2<Type, 2, 1, 3, 1> xy()
	{
		ਵਾਪਸ {0, 0, ਇਹ};
	}

	Dcm<Type> hat() ਅਚਲ      // inverse to Dcm.vee() operation
	{
		ਅਚਲ Vector3 &v(*ਇਹ);
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
		ਵਾਪਸ A;
	}

};

ਵਰਤੋਂ Vector3f = Vector3<ਦਸ਼ਮਲਵ>;
ਵਰਤੋਂ Vector3d = Vector3<ਦੁੱਗਣਾ>;

} // namespace matrix
