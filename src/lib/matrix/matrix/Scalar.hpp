/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#અન્તર્ભાવ "Matrix.hpp"

નામાકાશ matrix
{

template<typename Type>
વર્ગ Scalar
{
જાહેર:
	Scalar() = delete;

	Scalar(અચલ Matrix<Type, 1, 1> &other) :
		કિંમત_2{other(0, 0)}
	{
	}

	Scalar(Type other) : કિંમત_2(other)
	{
	}

	operator અચલ Type &()
	{
		પાછા કિંમત_2;
	}

	operator Matrix<Type, 1, 1>() અચલ
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = કિંમત_2;
		પાછા m;
	}

	operator Vector<Type, 1>() અચલ
	{
		Vector<Type, 1> m;
		m(0) = કિંમત_2;
		પાછા m;
	}

	તાર્કિક operator==(અચલ દશાંશ other) અચલ
	{
		પાછા isEqualF(કિંમત_2, other);
	}

ખાનગી:
	અચલ Type કિંમત_2;

};

using Scalarf = Scalar<દશાંશ>;
using Scalard = Scalar<દ્વિગુણ>;

} // namespace matrix
