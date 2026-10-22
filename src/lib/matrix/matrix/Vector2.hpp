/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#অন্তর্ভাৱ "Vector.hpp"

নামাকাশ matrix
{

template<typename Type>
শ্ৰেণী Vector2 : সার্ৱজনিক Vector<Type, 2>
{
সার্ৱজনিক:

	using Matrix21 = Matrix<Type, 2, 1>;
	using Vector3 = Vector<Type, 3>;

	Vector2() = স্ৱভাৱ;

	Vector2(ধ্রুৱ Matrix21 &other) :
		Vector<Type, 2>(other)
	{
	}

	explicit Vector2(ধ্রুৱ Type data_[2]) :
		Vector<Type, 2>(data_)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*এতত্);
		v(0) = x;
		v(1) = y;
	}

	using base = Vector<Type, 2>;
	using base::base;

	explicit Vector2(ধ্রুৱ Vector3 &other)
	{
		Vector2 &v(*এতত্);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(ধ্রুৱ Matrix21 &b) ধ্রুৱ
	{
		ধ্রুৱ Vector2 &a(*এতত্);
		ঘূৰাই a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 operator+(Vector2 other) ধ্রুৱ
	{
		ঘূৰাই Matrix21::operator+(other);
	}

	Vector2 operator+(Type scalar) ধ্রুৱ
	{
		ঘূৰাই Matrix21::operator+(scalar);
	}

	Vector2 operator-(Vector2 other) ধ্রুৱ
	{
		ঘূৰাই Matrix21::operator-(other);
	}

	Vector2 operator-(Type scalar) ধ্রুৱ
	{
		ঘূৰাই Matrix21::operator-(scalar);
	}

	Vector2 operator-() ধ্রুৱ
	{
		ঘূৰাই Matrix21::operator-();
	}

	Vector2 operator*(Type scalar) ধ্রুৱ
	{
		ঘূৰাই Matrix21::operator*(scalar);
	}

	Type operator*(Vector2 b) ধ্রুৱ
	{
		ঘূৰাই Vector<Type, 2>::operator*(b);
	}

	Type operator%(ধ্রুৱ Matrix21 &b) ধ্রুৱ
	{
		ঘূৰাই (*এতত্).cross(b);
	}

};

using Vector2f = Vector2<ভিন্ন>;
using Vector2d = Vector2<দ্ৱিগুণ>;

} // namespace matrix
