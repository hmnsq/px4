/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#অন্তর্ভাব "Matrix.hpp"

নামাকাশ matrix
{

template<typename Type>
শ্রেণী Scalar
{
সর্বজনীন:
	Scalar() = delete;

	Scalar(ধ্রুবক Matrix<Type, 1, 1> &other) :
		মান_2{other(0, 0)}
	{
	}

	Scalar(Type other) : মান_2(other)
	{
	}

	operator ধ্রুবক Type &()
	{
		ফেরত মান_2;
	}

	operator Matrix<Type, 1, 1>() ধ্রুবক
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = মান_2;
		ফেরত m;
	}

	operator Vector<Type, 1>() ধ্রুবক
	{
		Vector<Type, 1> m;
		m(0) = মান_2;
		ফেরত m;
	}

	যৌক্তিক operator==(ধ্রুবক দশমিক other) ধ্রুবক
	{
		ফেরত isEqualF(মান_2, other);
	}

ব্যক্তিগত:
	ধ্রুবক Type মান_2;

};

using Scalarf = Scalar<দশমিক>;
using Scalard = Scalar<দ্বিগুণ>;

} // namespace matrix
