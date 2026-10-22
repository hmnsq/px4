/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#رهنمود once

#ضمن "Matrix.hpp"

نطاق ماتریس
{

قالب<نام_نوع Type>
صنف Scalar
{
عام:
	Scalar() = حذف;

	Scalar(ثابت ماتریس_2<Type, 1, 1> &other) :
		مقدار_2{other(0, 0)}
	{
	}

	Scalar(Type other) : مقدار_2(other)
	{
	}

	عملگر ثابت Type &()
	{
		أرجع مقدار_2;
	}

	عملگر ماتریس_2<Type, 1, 1>() ثابت
	{
		ماتریس_2<Type, 1, 1> m;
		m(0, 0) = مقدار_2;
		أرجع m;
	}

	عملگر وکتور<Type, 1>() ثابت
	{
		وکتور<Type, 1> m;
		m(0) = مقدار_2;
		أرجع m;
	}

	منطقي عملگر==(ثابت عائم other) ثابت
	{
		أرجع isEqualF(مقدار_2, other);
	}

خاص:
	ثابت Type مقدار_2;

};

استفاده Scalarf = Scalar<عائم>;
استفاده Scalard = Scalar<مزدوج>;

} // namespace matrix
