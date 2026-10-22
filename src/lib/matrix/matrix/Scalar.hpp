/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#அந்தர்பாவ "Matrix.hpp"

நாமாகாஶ matrix
{

template<typename Type>
வகுப்பு Scalar
{
பொது:
	Scalar() = delete;

	Scalar(மாறிலி Matrix<Type, 1, 1> &other) :
		மதிப்பு_2{other(0, 0)}
	{
	}

	Scalar(Type other) : மதிப்பு_2(other)
	{
	}

	operator மாறிலி Type &()
	{
		திரும்பு மதிப்பு_2;
	}

	operator Matrix<Type, 1, 1>() மாறிலி
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = மதிப்பு_2;
		திரும்பு m;
	}

	operator Vector<Type, 1>() மாறிலி
	{
		Vector<Type, 1> m;
		m(0) = மதிப்பு_2;
		திரும்பு m;
	}

	தருக்க operator==(மாறிலி தசம other) மாறிலி
	{
		திரும்பு isEqualF(மதிப்பு_2, other);
	}

தனிப்பட்ட:
	மாறிலி Type மதிப்பு_2;

};

using Scalarf = Scalar<தசம>;
using Scalard = Scalar<இரட்டை>;

} // namespace matrix
