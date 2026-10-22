/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "Vector.hpp"

ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ
{

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type>
ਵਰਗ Vector2 : ਜਨਤਕ ਸਦਿਸ਼<Type, 2>
{
ਜਨਤਕ:

	ਵਰਤੋਂ Matrix21 = ਮੈਟ੍ਰਿਕਸ_2<Type, 2, 1>;
	ਵਰਤੋਂ Vector3 = ਸਦਿਸ਼<Type, 3>;

	Vector2() = ਮੂਲ;

	Vector2(ਅਚਲ Matrix21 &other) :
		ਸਦਿਸ਼<Type, 2>(other)
	{
	}

	ਸਪਸ਼ਟ Vector2(ਅਚਲ Type ਅੰਕੜੇ_2[2]) :
		ਸਦਿਸ਼<Type, 2>(ਅੰਕੜੇ_2)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*ਇਹ);
		v(0) = x;
		v(1) = y;
	}

	ਵਰਤੋਂ base = ਸਦਿਸ਼<Type, 2>;
	ਵਰਤੋਂ base::base;

	ਸਪਸ਼ਟ Vector2(ਅਚਲ Vector3 &other)
	{
		Vector2 &v(*ਇਹ);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(ਅਚਲ Matrix21 &b) ਅਚਲ
	{
		ਅਚਲ Vector2 &a(*ਇਹ);
		ਵਾਪਸ a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 ਸੰਚਾਲਕ+(Vector2 other) ਅਚਲ
	{
		ਵਾਪਸ Matrix21::ਸੰਚਾਲਕ+(other);
	}

	Vector2 ਸੰਚਾਲਕ+(Type scalar) ਅਚਲ
	{
		ਵਾਪਸ Matrix21::ਸੰਚਾਲਕ+(scalar);
	}

	Vector2 ਸੰਚਾਲਕ-(Vector2 other) ਅਚਲ
	{
		ਵਾਪਸ Matrix21::ਸੰਚਾਲਕ-(other);
	}

	Vector2 ਸੰਚਾਲਕ-(Type scalar) ਅਚਲ
	{
		ਵਾਪਸ Matrix21::ਸੰਚਾਲਕ-(scalar);
	}

	Vector2 ਸੰਚਾਲਕ-() ਅਚਲ
	{
		ਵਾਪਸ Matrix21::ਸੰਚਾਲਕ-();
	}

	Vector2 ਸੰਚਾਲਕ*(Type scalar) ਅਚਲ
	{
		ਵਾਪਸ Matrix21::ਸੰਚਾਲਕ*(scalar);
	}

	Type ਸੰਚਾਲਕ*(Vector2 b) ਅਚਲ
	{
		ਵਾਪਸ ਸਦਿਸ਼<Type, 2>::ਸੰਚਾਲਕ*(b);
	}

	Type ਸੰਚਾਲਕ%(ਅਚਲ Matrix21 &b) ਅਚਲ
	{
		ਵਾਪਸ (*ਇਹ).cross(b);
	}

};

ਵਰਤੋਂ Vector2f = Vector2<ਦਸ਼ਮਲਵ>;
ਵਰਤੋਂ Vector2d = Vector2<ਦੁੱਗਣਾ>;

} // namespace matrix
