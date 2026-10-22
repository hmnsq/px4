/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#அந்தர்பாவ "Vector.hpp"

நாமாகாஶ matrix
{

template<typename Type>
வகுப்பு Vector2 : பொது Vector<Type, 2>
{
பொது:

	using Matrix21 = Matrix<Type, 2, 1>;
	using Vector3 = Vector<Type, 3>;

	Vector2() = இயல்பு;

	Vector2(மாறிலி Matrix21 &other) :
		Vector<Type, 2>(other)
	{
	}

	explicit Vector2(மாறிலி Type data_[2]) :
		Vector<Type, 2>(data_)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*இது);
		v(0) = x;
		v(1) = y;
	}

	using base = Vector<Type, 2>;
	using base::base;

	explicit Vector2(மாறிலி Vector3 &other)
	{
		Vector2 &v(*இது);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(மாறிலி Matrix21 &b) மாறிலி
	{
		மாறிலி Vector2 &a(*இது);
		திரும்பு a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 operator+(Vector2 other) மாறிலி
	{
		திரும்பு Matrix21::operator+(other);
	}

	Vector2 operator+(Type scalar) மாறிலி
	{
		திரும்பு Matrix21::operator+(scalar);
	}

	Vector2 operator-(Vector2 other) மாறிலி
	{
		திரும்பு Matrix21::operator-(other);
	}

	Vector2 operator-(Type scalar) மாறிலி
	{
		திரும்பு Matrix21::operator-(scalar);
	}

	Vector2 operator-() மாறிலி
	{
		திரும்பு Matrix21::operator-();
	}

	Vector2 operator*(Type scalar) மாறிலி
	{
		திரும்பு Matrix21::operator*(scalar);
	}

	Type operator*(Vector2 b) மாறிலி
	{
		திரும்பு Vector<Type, 2>::operator*(b);
	}

	Type operator%(மாறிலி Matrix21 &b) மாறிலி
	{
		திரும்பு (*இது).cross(b);
	}

};

using Vector2f = Vector2<தசம>;
using Vector2d = Vector2<இரட்டை>;

} // namespace matrix
