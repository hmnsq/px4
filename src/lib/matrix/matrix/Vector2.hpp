/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ಅನ್ತರ್ಭಾವ "Vector.hpp"

ನಾಮಾಕಾಶ matrix
{

template<typename Type>
ವರ್ಗ Vector2 : ಸಾರ್ವಜನಿಕ Vector<Type, 2>
{
ಸಾರ್ವಜನಿಕ:

	using Matrix21 = Matrix<Type, 2, 1>;
	using Vector3 = Vector<Type, 3>;

	Vector2() = ಸ್ವಭಾವ;

	Vector2(ಧ್ರುವ Matrix21 &other) :
		Vector<Type, 2>(other)
	{
	}

	explicit Vector2(ಧ್ರುವ Type data_[2]) :
		Vector<Type, 2>(data_)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*ಏತತ್);
		v(0) = x;
		v(1) = y;
	}

	using base = Vector<Type, 2>;
	using base::base;

	explicit Vector2(ಧ್ರುವ Vector3 &other)
	{
		Vector2 &v(*ಏತತ್);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(ಧ್ರುವ Matrix21 &b) ಧ್ರುವ
	{
		ಧ್ರುವ Vector2 &a(*ಏತತ್);
		ಪ್ರತ್ಯಾಗಮ a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 operator+(Vector2 other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix21::operator+(other);
	}

	Vector2 operator+(Type scalar) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix21::operator+(scalar);
	}

	Vector2 operator-(Vector2 other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix21::operator-(other);
	}

	Vector2 operator-(Type scalar) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix21::operator-(scalar);
	}

	Vector2 operator-() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix21::operator-();
	}

	Vector2 operator*(Type scalar) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix21::operator*(scalar);
	}

	Type operator*(Vector2 b) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Vector<Type, 2>::operator*(b);
	}

	Type operator%(ಧ್ರುವ Matrix21 &b) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್).cross(b);
	}

};

using Vector2f = Vector2<ಭಿನ್ನ>;
using Vector2d = Vector2<ದ್ವಿಗುಣ>;

} // namespace matrix
