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
		_value{other(0, 0)}
	{
	}

	Scalar(Type other) : _value(other)
	{
	}

	operator ثابت Type &()
	{
		أرجع _value;
	}

	operator Matrix<Type, 1, 1>() ثابت
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = _value;
		أرجع m;
	}

	operator Vector<Type, 1>() ثابت
	{
		Vector<Type, 1> m;
		m(0) = _value;
		أرجع m;
	}

	منطقي operator==(ثابت عائم other) ثابت
	{
		أرجع isEqualF(_value, other);
	}

خاص:
	ثابت Type _value;

};

using Scalarf = Scalar<عائم>;
using Scalard = Scalar<مزدوج>;

} // namespace matrix
