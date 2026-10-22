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
		بەها_2{other(0, 0)}
	{
	}

	Scalar(Type other) : بەها_2(other)
	{
	}

	operator ثابت Type &()
	{
		أرجع بەها_2;
	}

	operator Matrix<Type, 1, 1>() ثابت
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = بەها_2;
		أرجع m;
	}

	operator Vector<Type, 1>() ثابت
	{
		Vector<Type, 1> m;
		m(0) = بەها_2;
		أرجع m;
	}

	منطقي operator==(ثابت عائم other) ثابت
	{
		أرجع isEqualF(بەها_2, other);
	}

خاص:
	ثابت Type بەها_2;

};

using Scalarf = Scalar<عائم>;
using Scalard = Scalar<مزدوج>;

} // namespace matrix
