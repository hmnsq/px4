/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "Matrix.hpp"

ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ
{

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type>
ਵਰਗ Scalar
{
ਜਨਤਕ:
	Scalar() = ਮਿਟਾਓ;

	Scalar(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<Type, 1, 1> &other) :
		ਮੁੱਲ_2{other(0, 0)}
	{
	}

	Scalar(Type other) : ਮੁੱਲ_2(other)
	{
	}

	ਸੰਚਾਲਕ ਅਚਲ Type &()
	{
		ਵਾਪਸ ਮੁੱਲ_2;
	}

	ਸੰਚਾਲਕ ਮੈਟ੍ਰਿਕਸ_2<Type, 1, 1>() ਅਚਲ
	{
		ਮੈਟ੍ਰਿਕਸ_2<Type, 1, 1> m;
		m(0, 0) = ਮੁੱਲ_2;
		ਵਾਪਸ m;
	}

	ਸੰਚਾਲਕ ਸਦਿਸ਼<Type, 1>() ਅਚਲ
	{
		ਸਦਿਸ਼<Type, 1> m;
		m(0) = ਮੁੱਲ_2;
		ਵਾਪਸ m;
	}

	ਤਾਰਕਿਕ ਸੰਚਾਲਕ==(ਅਚਲ ਦਸ਼ਮਲਵ other) ਅਚਲ
	{
		ਵਾਪਸ isEqualF(ਮੁੱਲ_2, other);
	}

ਨਿਜੀ:
	ਅਚਲ Type ਮੁੱਲ_2;

};

ਵਰਤੋਂ Scalarf = Scalar<ਦਸ਼ਮਲਵ>;
ਵਰਤੋਂ Scalard = Scalar<ਦੁੱਗਣਾ>;

} // namespace matrix
