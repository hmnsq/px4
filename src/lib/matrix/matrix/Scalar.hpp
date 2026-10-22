/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#অন্তর্ভাৱ "Matrix.hpp"

নামাকাশ matrix
{

template<typename Type>
শ্ৰেণী Scalar
{
সার্ৱজনিক:
	Scalar() = delete;

	Scalar(ধ্রুৱ Matrix<Type, 1, 1> &other) :
		মান_2{other(0, 0)}
	{
	}

	Scalar(Type other) : মান_2(other)
	{
	}

	operator ধ্রুৱ Type &()
	{
		ঘূৰাই মান_2;
	}

	operator Matrix<Type, 1, 1>() ধ্রুৱ
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = মান_2;
		ঘূৰাই m;
	}

	operator Vector<Type, 1>() ধ্রুৱ
	{
		Vector<Type, 1> m;
		m(0) = মান_2;
		ঘূৰাই m;
	}

	তার্কিক operator==(ধ্রুৱ ভিন্ন other) ধ্রুৱ
	{
		ঘূৰাই isEqualF(মান_2, other);
	}

গুপ্ত:
	ধ্রুৱ Type মান_2;

};

using Scalarf = Scalar<ভিন্ন>;
using Scalard = Scalar<দ্ৱিগুণ>;

} // namespace matrix
