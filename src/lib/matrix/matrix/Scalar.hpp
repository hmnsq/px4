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

	Scalar(स्थिरांक Matrix<Type, 1, 1> &other) :
		मूल्य_2{other(0, 0)}
	{
	}

	Scalar(Type other) : मूल्य_2(other)
	{
	}

	चालक स्थिरांक Type &()
	{
		परत मूल्य_2;
	}

	चालक Matrix<Type, 1, 1>() स्थिरांक
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = मूल्य_2;
		परत m;
	}

	चालक Vector<Type, 1>() स्थिरांक
	{
		Vector<Type, 1> m;
		m(0) = मूल्य_2;
		परत m;
	}

	तार्किक चालक==(स्थिरांक दशांश other) स्थिरांक
	{
		परत isEqualF(मूल्य_2, other);
	}

खाजगी:
	स्थिरांक Type मूल्य_2;

};

using Scalarf = Scalar<दशांश>;
using Scalard = Scalar<द्विगुण>;

} // namespace matrix
