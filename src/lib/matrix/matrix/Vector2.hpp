/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#شامل "Vector.hpp"

فضاینام matrix
{

template<typename Type>
رده Vector2 : عمومی Vector<Type, 2>
{
عمومی:

	using Matrix21 = Matrix<Type, 2, 1>;
	using Vector3 = Vector<Type, 3>;

	Vector2() = پیشفرض;

	Vector2(ثابت Matrix21 &other) :
		Vector<Type, 2>(other)
	{
	}

	explicit Vector2(ثابت Type data_[2]) :
		Vector<Type, 2>(data_)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*این);
		v(0) = x;
		v(1) = y;
	}

	using base = Vector<Type, 2>;
	using base::base;

	explicit Vector2(ثابت Vector3 &other)
	{
		Vector2 &v(*این);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(ثابت Matrix21 &b) ثابت
	{
		ثابت Vector2 &a(*این);
		بازگشت a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 operator+(Vector2 other) ثابت
	{
		بازگشت Matrix21::operator+(other);
	}

	Vector2 operator+(Type scalar) ثابت
	{
		بازگشت Matrix21::operator+(scalar);
	}

	Vector2 operator-(Vector2 other) ثابت
	{
		بازگشت Matrix21::operator-(other);
	}

	Vector2 operator-(Type scalar) ثابت
	{
		بازگشت Matrix21::operator-(scalar);
	}

	Vector2 operator-() ثابت
	{
		بازگشت Matrix21::operator-();
	}

	Vector2 operator*(Type scalar) ثابت
	{
		بازگشت Matrix21::operator*(scalar);
	}

	Type operator*(Vector2 b) ثابت
	{
		بازگشت Vector<Type, 2>::operator*(b);
	}

	Type operator%(ثابت Matrix21 &b) ثابت
	{
		بازگشت (*این).cross(b);
	}

};

using Vector2f = Vector2<اعشاری>;
using Vector2d = Vector2<دوگانه>;

} // namespace matrix
