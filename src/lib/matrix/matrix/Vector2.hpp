/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#અન્તર્ભાવ "Vector.hpp"

નામાકાશ matrix
{

template<typename Type>
વર્ગ Vector2 : જાહેર Vector<Type, 2>
{
જાહેર:

	using Matrix21 = Matrix<Type, 2, 1>;
	using Vector3 = Vector<Type, 3>;

	Vector2() = મૂળભૂત;

	Vector2(અચલ Matrix21 &other) :
		Vector<Type, 2>(other)
	{
	}

	explicit Vector2(અચલ Type data_[2]) :
		Vector<Type, 2>(data_)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*આ);
		v(0) = x;
		v(1) = y;
	}

	using base = Vector<Type, 2>;
	using base::base;

	explicit Vector2(અચલ Vector3 &other)
	{
		Vector2 &v(*આ);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(અચલ Matrix21 &b) અચલ
	{
		અચલ Vector2 &a(*આ);
		પાછા a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 operator+(Vector2 other) અચલ
	{
		પાછા Matrix21::operator+(other);
	}

	Vector2 operator+(Type scalar) અચલ
	{
		પાછા Matrix21::operator+(scalar);
	}

	Vector2 operator-(Vector2 other) અચલ
	{
		પાછા Matrix21::operator-(other);
	}

	Vector2 operator-(Type scalar) અચલ
	{
		પાછા Matrix21::operator-(scalar);
	}

	Vector2 operator-() અચલ
	{
		પાછા Matrix21::operator-();
	}

	Vector2 operator*(Type scalar) અચલ
	{
		પાછા Matrix21::operator*(scalar);
	}

	Type operator*(Vector2 b) અચલ
	{
		પાછા Vector<Type, 2>::operator*(b);
	}

	Type operator%(અચલ Matrix21 &b) અચલ
	{
		પાછા (*આ).cross(b);
	}

};

using Vector2f = Vector2<દશાંશ>;
using Vector2d = Vector2<દ્વિગુણ>;

} // namespace matrix
