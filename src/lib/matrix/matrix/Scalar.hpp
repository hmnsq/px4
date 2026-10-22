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

ढाँचा<typename Type>
वर्ग Scalar
{
सार्वजनिक:
	Scalar() = delete;

	Scalar(अचर Matrix<Type, 1, 1> &other) :
		मान_2{other(0, 0)}
	{
	}

	Scalar(Type other) : मान_2(other)
	{
	}

	चालक अचर Type &()
	{
		फर्काऊ मान_2;
	}

	चालक Matrix<Type, 1, 1>() अचर
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = मान_2;
		फर्काऊ m;
	}

	चालक Vector<Type, 1>() अचर
	{
		Vector<Type, 1> m;
		m(0) = मान_2;
		फर्काऊ m;
	}

	तार्किक चालक==(अचर भिन्न other) अचर
	{
		फर्काऊ isEqualF(मान_2, other);
	}

गुप्त:
	अचर Type मान_2;

};

using Scalarf = Scalar<भिन्न>;
using Scalard = Scalar<द्विगुण>;

} // namespace matrix
