/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ "Vector.hpp"

ନାମାକାଶ matrix
{

template<typename Type>
ଶ୍ରେଣୀ Vector2 : ସର୍ବସାଧାରଣ Vector<Type, 2>
{
ସର୍ବସାଧାରଣ:

	using Matrix21 = Matrix<Type, 2, 1>;
	using Vector3 = Vector<Type, 3>;

	Vector2() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	Vector2(ଅଚଳ Matrix21 &other) :
		Vector<Type, 2>(other)
	{
	}

	explicit Vector2(ଅଚଳ Type data_[2]) :
		Vector<Type, 2>(data_)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*ଏହି);
		v(0) = x;
		v(1) = y;
	}

	using base = Vector<Type, 2>;
	using base::base;

	explicit Vector2(ଅଚଳ Vector3 &other)
	{
		Vector2 &v(*ଏହି);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(ଅଚଳ Matrix21 &b) ଅଚଳ
	{
		ଅଚଳ Vector2 &a(*ଏହି);
		ଫେରନ୍ତୁ a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 operator+(Vector2 other) ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix21::operator+(other);
	}

	Vector2 operator+(Type scalar) ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix21::operator+(scalar);
	}

	Vector2 operator-(Vector2 other) ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix21::operator-(other);
	}

	Vector2 operator-(Type scalar) ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix21::operator-(scalar);
	}

	Vector2 operator-() ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix21::operator-();
	}

	Vector2 operator*(Type scalar) ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix21::operator*(scalar);
	}

	Type operator*(Vector2 b) ଅଚଳ
	{
		ଫେରନ୍ତୁ Vector<Type, 2>::operator*(b);
	}

	Type operator%(ଅଚଳ Matrix21 &b) ଅଚଳ
	{
		ଫେରନ୍ତୁ (*ଏହି).cross(b);
	}

};

using Vector2f = Vector2<ଦଶମିକ>;
using Vector2d = Vector2<ଦ୍ୱିଗୁଣ>;

} // namespace matrix
