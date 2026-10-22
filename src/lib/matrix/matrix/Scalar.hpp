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
		قدر_2{other(0, 0)}
	{
	}

	Scalar(Type other) : قدر_2(other)
	{
	}

	operator ثابت Type &()
	{
		أرجع قدر_2;
	}

	operator Matrix<Type, 1, 1>() ثابت
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = قدر_2;
		أرجع m;
	}

	operator Vector<Type, 1>() ثابت
	{
		Vector<Type, 1> m;
		m(0) = قدر_2;
		أرجع m;
	}

	منطقي operator==(ثابت عائم other) ثابت
	{
		أرجع isEqualF(قدر_2, other);
	}

خاص:
	ثابت Type قدر_2;

};

using Scalarf = Scalar<عائم>;
using Scalard = Scalar<مزدوج>;

} // namespace matrix
