/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#অন্তর্ভাব "Vector.hpp"

নামাকাশ matrix
{

template<typename Type>
শ্রেণী Vector2 : সর্বজনীন Vector<Type, 2>
{
সর্বজনীন:

	using Matrix21 = Matrix<Type, 2, 1>;
	using Vector3 = Vector<Type, 3>;

	Vector2() = পূর্বনির্ধারিত;

	Vector2(ধ্রুবক Matrix21 &other) :
		Vector<Type, 2>(other)
	{
	}

	explicit Vector2(ধ্রুবক Type তথ্য_2[2]) :
		Vector<Type, 2>(তথ্য_2)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*এই);
		v(0) = x;
		v(1) = y;
	}

	using base = Vector<Type, 2>;
	using base::base;

	explicit Vector2(ধ্রুবক Vector3 &other)
	{
		Vector2 &v(*এই);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(ধ্রুবক Matrix21 &b) ধ্রুবক
	{
		ধ্রুবক Vector2 &a(*এই);
		ফেরত a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 operator+(Vector2 other) ধ্রুবক
	{
		ফেরত Matrix21::operator+(other);
	}

	Vector2 operator+(Type scalar) ধ্রুবক
	{
		ফেরত Matrix21::operator+(scalar);
	}

	Vector2 operator-(Vector2 other) ধ্রুবক
	{
		ফেরত Matrix21::operator-(other);
	}

	Vector2 operator-(Type scalar) ধ্রুবক
	{
		ফেরত Matrix21::operator-(scalar);
	}

	Vector2 operator-() ধ্রুবক
	{
		ফেরত Matrix21::operator-();
	}

	Vector2 operator*(Type scalar) ধ্রুবক
	{
		ফেরত Matrix21::operator*(scalar);
	}

	Type operator*(Vector2 b) ধ্রুবক
	{
		ফেরত Vector<Type, 2>::operator*(b);
	}

	Type operator%(ধ্রুবক Matrix21 &b) ধ্রুবক
	{
		ফেরত (*এই).cross(b);
	}

};

using Vector2f = Vector2<দশমিক>;
using Vector2d = Vector2<দ্বিগুণ>;

} // namespace matrix
