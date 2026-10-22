/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ "Matrix.hpp"

ନାମାକାଶ matrix
{

template<typename Type>
ଶ୍ରେଣୀ Scalar
{
ସର୍ବସାଧାରଣ:
	Scalar() = delete;

	Scalar(ଅଚଳ Matrix<Type, 1, 1> &other) :
		ମୂଲ୍ୟ_2{other(0, 0)}
	{
	}

	Scalar(Type other) : ମୂଲ୍ୟ_2(other)
	{
	}

	operator ଅଚଳ Type &()
	{
		ଫେରନ୍ତୁ ମୂଲ୍ୟ_2;
	}

	operator Matrix<Type, 1, 1>() ଅଚଳ
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = ମୂଲ୍ୟ_2;
		ଫେରନ୍ତୁ m;
	}

	operator Vector<Type, 1>() ଅଚଳ
	{
		Vector<Type, 1> m;
		m(0) = ମୂଲ୍ୟ_2;
		ଫେରନ୍ତୁ m;
	}

	ତାର୍କିକ operator==(ଅଚଳ ଦଶମିକ other) ଅଚଳ
	{
		ଫେରନ୍ତୁ isEqualF(ମୂଲ୍ୟ_2, other);
	}

ବ୍ୟକ୍ତିଗତ:
	ଅଚଳ Type ମୂଲ୍ୟ_2;

};

using Scalarf = Scalar<ଦଶମିକ>;
using Scalard = Scalar<ଦ୍ୱିଗୁଣ>;

} // namespace matrix
