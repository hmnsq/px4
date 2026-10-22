/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ہدایت once

#ضمن "Matrix.hpp"

نطاق میٹرکس
{

سانچا<قسم_ناں Type>
صنف Scalar
{
عام:
	Scalar() = مٹاؤ;

	Scalar(ثابت میٹرکس_2<Type, 1, 1> &other) :
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

	عامل میٹرکس_2<Type, 1, 1>() ثابت
	{
		میٹرکس_2<Type, 1, 1> m;
		m(0, 0) = قدر_2;
		أرجع m;
	}

	عامل سمتیہ<Type, 1>() ثابت
	{
		سمتیہ<Type, 1> m;
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

ورتوں Scalarf = Scalar<عائم>;
ورتوں Scalard = Scalar<مزدوج>;

} // namespace matrix
