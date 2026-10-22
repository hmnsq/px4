/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#അന്തര്ഭാവ "Vector.hpp"

നാമാകാശ matrix
{

template<typename Type>
വർഗ്ഗം Vector2 : പൊതു Vector<Type, 2>
{
പൊതു:

	using Matrix21 = Matrix<Type, 2, 1>;
	using Vector3 = Vector<Type, 3>;

	Vector2() = സ്ഥിരസ്ഥിതി;

	Vector2(സ്ഥിരം Matrix21 &other) :
		Vector<Type, 2>(other)
	{
	}

	explicit Vector2(സ്ഥിരം Type data_[2]) :
		Vector<Type, 2>(data_)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*ഇത്);
		v(0) = x;
		v(1) = y;
	}

	using base = Vector<Type, 2>;
	using base::base;

	explicit Vector2(സ്ഥിരം Vector3 &other)
	{
		Vector2 &v(*ഇത്);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(സ്ഥിരം Matrix21 &b) സ്ഥിരം
	{
		സ്ഥിരം Vector2 &a(*ഇത്);
		തിരികെ a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 operator+(Vector2 other) സ്ഥിരം
	{
		തിരികെ Matrix21::operator+(other);
	}

	Vector2 operator+(Type scalar) സ്ഥിരം
	{
		തിരികെ Matrix21::operator+(scalar);
	}

	Vector2 operator-(Vector2 other) സ്ഥിരം
	{
		തിരികെ Matrix21::operator-(other);
	}

	Vector2 operator-(Type scalar) സ്ഥിരം
	{
		തിരികെ Matrix21::operator-(scalar);
	}

	Vector2 operator-() സ്ഥിരം
	{
		തിരികെ Matrix21::operator-();
	}

	Vector2 operator*(Type scalar) സ്ഥിരം
	{
		തിരികെ Matrix21::operator*(scalar);
	}

	Type operator*(Vector2 b) സ്ഥിരം
	{
		തിരികെ Vector<Type, 2>::operator*(b);
	}

	Type operator%(സ്ഥിരം Matrix21 &b) സ്ഥിരം
	{
		തിരികെ (*ഇത്).cross(b);
	}

};

using Vector2f = Vector2<ദശാംശം>;
using Vector2d = Vector2<ഇരട്ട>;

} // namespace matrix
