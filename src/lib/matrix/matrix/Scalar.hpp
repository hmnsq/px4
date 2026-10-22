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
		हालागा_2{other(0, 0)}
	{
	}

	Scalar(Type other) : हालागा_2(other)
	{
	}

	operator const Type &()
	{
		return हालागा_2;
	}

	operator Matrix<Type, 1, 1>() const
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = हालागा_2;
		return m;
	}

	operator Vector<Type, 1>() const
	{
		Vector<Type, 1> m;
		m(0) = हालागा_2;
		return m;
	}

	bool operator==(const float other) const
	{
		return isEqualF(हालागा_2, other);
	}

private:
	const Type हालागा_2;

};

using Scalarf = Scalar<float>;
using Scalard = Scalar<double>;

} // namespace matrix
