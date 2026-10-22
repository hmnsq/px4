/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#لارښوونه once

#ضمن "Matrix.hpp"

نطاق ماتریکس
{

کينډۍ<ډول_نوم Type>
صنف Scalar
{
عام:
	Scalar() = ړنګول;

	Scalar(ثابت ماتریکس_2<Type, 1, 1> &other) :
		ارزښت_2{other(0, 0)}
	{
	}

	Scalar(Type other) : ارزښت_2(other)
	{
	}

	چلوونکی ثابت Type &()
	{
		أرجع ارزښت_2;
	}

	چلوونکی ماتریکس_2<Type, 1, 1>() ثابت
	{
		ماتریکس_2<Type, 1, 1> m;
		m(0, 0) = ارزښت_2;
		أرجع m;
	}

	چلوونکی ویکټور<Type, 1>() ثابت
	{
		ویکټور<Type, 1> m;
		m(0) = ارزښت_2;
		أرجع m;
	}

	منطقي چلوونکی==(ثابت عائم other) ثابت
	{
		أرجع isEqualF(ارزښت_2, other);
	}

خاص:
	ثابت Type ارزښت_2;

};

کارول Scalarf = Scalar<عائم>;
کارول Scalard = Scalar<مزدوج>;

} // namespace matrix
