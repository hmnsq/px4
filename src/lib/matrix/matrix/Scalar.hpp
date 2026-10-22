/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#include "Matrix.hpp"

namespace matrix
{

template<typename Type>
class Scalar
{
public:
	Scalar() = delete;

	Scalar(const Matrix<Type, 1, 1> &other) :
		_value{other(0, 0)}
	{
	}

	Scalar(Type other) : _value(other)
	{
	}

	operator const Type &()
	{
		ສົ່ງຄືນ _value;
	}

	operator Matrix<Type, 1, 1>() const
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = _value;
		ສົ່ງຄືນ m;
	}

	operator Vector<Type, 1>() const
	{
		Vector<Type, 1> m;
		m(0) = _value;
		ສົ່ງຄືນ m;
	}

	bool operator==(const float other) const
	{
		ສົ່ງຄືນ isEqualF(_value, other);
	}

private:
	const Type _value;

};

using Scalarf = Scalar<float>;
using Scalard = Scalar<dviguu_na>;

} // namespace matrix
