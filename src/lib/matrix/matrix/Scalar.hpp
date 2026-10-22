/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ضمن "Matrix.hpp"

نطاق matrix
{

template<typename Type>
صنف Scalar
{
عام:
	Scalar() = delete;

	Scalar(ثابت Matrix<Type, 1, 1> &other) :
		قيمة_2{other(0, 0)}
	{
	}

	Scalar(Type other) : قيمة_2(other)
	{
	}

	operator ثابت Type &()
	{
		إرجاع قيمة_2;
	}

	operator Matrix<Type, 1, 1>() ثابت
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = قيمة_2;
		إرجاع m;
	}

	operator Vector<Type, 1>() ثابت
	{
		Vector<Type, 1> m;
		m(0) = قيمة_2;
		إرجاع m;
	}

	منطقي operator==(ثابت عائم other) ثابت
	{
		إرجاع isEqualF(قيمة_2, other);
	}

خاص:
	ثابت Type قيمة_2;

};

using Scalarf = Scalar<عائم>;
using Scalard = Scalar<مزدوج>;

} // namespace matrix
