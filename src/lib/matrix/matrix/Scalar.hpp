/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#شامل "Matrix.hpp"

فضاینام matrix
{

template<typename Type>
رده Scalar
{
عمومی:
	Scalar() = delete;

	Scalar(ثابت Matrix<Type, 1, 1> &other) :
		مقدار_2{other(0, 0)}
	{
	}

	Scalar(Type other) : مقدار_2(other)
	{
	}

	operator ثابت Type &()
	{
		بازگشت مقدار_2;
	}

	operator Matrix<Type, 1, 1>() ثابت
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = مقدار_2;
		بازگشت m;
	}

	operator Vector<Type, 1>() ثابت
	{
		Vector<Type, 1> m;
		m(0) = مقدار_2;
		بازگشت m;
	}

	منطقی operator==(ثابت اعشاری other) ثابت
	{
		بازگشت isEqualF(مقدار_2, other);
	}

خصوصی:
	ثابت Type مقدار_2;

};

using Scalarf = Scalar<اعشاری>;
using Scalard = Scalar<دوگانه>;

} // namespace matrix
