/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#अन्तर्भाव "Vector.hpp"

नामाकाश matrix
{

template<typename Type>
वग्ग Vector2 : सार्वजनिक Vector<Type, 2>
{
सार्वजनिक:

	using Matrix21 = Matrix<Type, 2, 1>;
	using Vector3 = Vector<Type, 3>;

	Vector2() = स्वभाव;

	Vector2(ध्रुव Matrix21 &other) :
		Vector<Type, 2>(other)
	{
	}

	explicit Vector2(ध्रुव Type data_[2]) :
		Vector<Type, 2>(data_)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*एतत्);
		v(0) = x;
		v(1) = y;
	}

	using base = Vector<Type, 2>;
	using base::base;

	explicit Vector2(ध्रुव Vector3 &other)
	{
		Vector2 &v(*एतत्);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(ध्रुव Matrix21 &b) ध्रुव
	{
		ध्रुव Vector2 &a(*एतत्);
		पटिनिवत्त a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 operator+(Vector2 other) ध्रुव
	{
		पटिनिवत्त Matrix21::operator+(other);
	}

	Vector2 operator+(Type scalar) ध्रुव
	{
		पटिनिवत्त Matrix21::operator+(scalar);
	}

	Vector2 operator-(Vector2 other) ध्रुव
	{
		पटिनिवत्त Matrix21::operator-(other);
	}

	Vector2 operator-(Type scalar) ध्रुव
	{
		पटिनिवत्त Matrix21::operator-(scalar);
	}

	Vector2 operator-() ध्रुव
	{
		पटिनिवत्त Matrix21::operator-();
	}

	Vector2 operator*(Type scalar) ध्रुव
	{
		पटिनिवत्त Matrix21::operator*(scalar);
	}

	Type operator*(Vector2 b) ध्रुव
	{
		पटिनिवत्त Vector<Type, 2>::operator*(b);
	}

	Type operator%(ध्रुव Matrix21 &b) ध्रुव
	{
		पटिनिवत्त (*एतत्).cross(b);
	}

};

using Vector2f = Vector2<भिन्न>;
using Vector2d = Vector2<द्विगुण>;

} // namespace matrix
