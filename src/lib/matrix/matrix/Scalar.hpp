/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#अन्तर्भाव "Matrix.hpp"

नामाकाश matrix
{

template<typename Type>
वर्ग Scalar
{
सार्वजनिक:
	Scalar() = delete;

	Scalar(ध्रुव Matrix<Type, 1, 1> &other) :
		मूल्य_2{other(0, 0)}
	{
	}

	Scalar(Type other) : मूल्य_2(other)
	{
	}

	operator ध्रुव Type &()
	{
		प्रत्यागम मूल्य_2;
	}

	operator Matrix<Type, 1, 1>() ध्रुव
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = मूल्य_2;
		प्रत्यागम m;
	}

	operator Vector<Type, 1>() ध्रुव
	{
		Vector<Type, 1> m;
		m(0) = मूल्य_2;
		प्रत्यागम m;
	}

	तार्किक operator==(ध्रुव भिन्न other) ध्रुव
	{
		प्रत्यागम isEqualF(मूल्य_2, other);
	}

गुप्त:
	ध्रुव Type मूल्य_2;

};

using Scalarf = Scalar<भिन्न>;
using Scalard = Scalar<द्विगुण>;

} // namespace matrix
