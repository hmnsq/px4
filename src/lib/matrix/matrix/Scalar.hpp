/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ہدایت once

#شامل "Matrix.hpp"

نامخلا میٹرکس
{

سانچہ<قسم_نام Type>
جماعت Scalar
{
عوامی:
	Scalar() = حذف;

	Scalar(ثابت میٹرکس_2<Type, 1, 1> &other) :
		قدر_2{other(0, 0)}
	{
	}

	Scalar(Type other) : قدر_2(other)
	{
	}

	عامل ثابت Type &()
	{
		واپس قدر_2;
	}

	عامل میٹرکس_2<Type, 1, 1>() ثابت
	{
		میٹرکس_2<Type, 1, 1> m;
		m(0, 0) = قدر_2;
		واپس m;
	}

	عامل سمتیہ<Type, 1>() ثابت
	{
		سمتیہ<Type, 1> m;
		m(0) = قدر_2;
		واپس m;
	}

	منطقی عامل==(ثابت اعشاری other) ثابت
	{
		واپس isEqualF(قدر_2, other);
	}

نجی:
	ثابت Type قدر_2;

};

استعمال Scalarf = Scalar<اعشاری>;
استعمال Scalard = Scalar<دوہرا>;

} // namespace matrix
