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
বর্গ Scalar
{
সার্বজনিক:
	Scalar() = delete;

	Scalar(ধ্রুব Matrix<Type, 1, 1> &other) :
		মান_2{other(0, 0)}
	{
	}

	Scalar(Type other) : মান_2(other)
	{
	}

	operator ধ্রুব Type &()
	{
		প্রত্যাগম মান_2;
	}

	operator Matrix<Type, 1, 1>() ধ্রুব
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = মান_2;
		প্রত্যাগম m;
	}

	operator Vector<Type, 1>() ধ্রুব
	{
		Vector<Type, 1> m;
		m(0) = মান_2;
		প্রত্যাগম m;
	}

	তার্কিক operator==(ধ্রুব ভিন্ন other) ধ্রুব
	{
		প্রত্যাগম isEqualF(মান_2, other);
	}

গুপ্ত:
	ধ্রুব Type মান_2;

};

using Scalarf = Scalar<ভিন্ন>;
using Scalard = Scalar<দ্বিগুণ>;

} // namespace matrix
