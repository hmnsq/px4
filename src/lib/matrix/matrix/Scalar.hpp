/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#هدايت once

#ضمن "Matrix.hpp"

نطاق ميٽرڪس
{

سانچو<قسم_نالو Type>
صنف Scalar
{
عام:
	Scalar() = ميٽيو;

	Scalar(ثابت ميٽرڪس_2<Type, 1, 1> &other) :
		قدر_2{other(0, 0)}
	{
	}

	Scalar(Type other) : قدر_2(other)
	{
	}

	عامل ثابت Type &()
	{
		أرجع قدر_2;
	}

	عامل ميٽرڪس_2<Type, 1, 1>() ثابت
	{
		ميٽرڪس_2<Type, 1, 1> m;
		m(0, 0) = قدر_2;
		أرجع m;
	}

	عامل ويڪٽر<Type, 1>() ثابت
	{
		ويڪٽر<Type, 1> m;
		m(0) = قدر_2;
		أرجع m;
	}

	منطقي عامل==(ثابت عائم other) ثابت
	{
		أرجع isEqualF(قدر_2, other);
	}

خاص:
	ثابت Type قدر_2;

};

استعمال Scalarf = Scalar<عائم>;
استعمال Scalard = Scalar<مزدوج>;

} // namespace matrix
