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

ढाँचा<typename Type>
वर्ग Vector2 : सार्वजनिक Vector<Type, 2>
{
सार्वजनिक:

	using Matrix21 = Matrix<Type, 2, 1>;
	using Vector3 = Vector<Type, 3>;

	Vector2() = स्वभाव;

	Vector2(अचर Matrix21 &other) :
		Vector<Type, 2>(other)
	{
	}

	explicit Vector2(अचर Type data_[2]) :
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

	explicit Vector2(अचर Vector3 &other)
	{
		Vector2 &v(*एतत्);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(अचर Matrix21 &b) अचर
	{
		अचर Vector2 &a(*एतत्);
		फर्काऊ a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 चालक+(Vector2 other) अचर
	{
		फर्काऊ Matrix21::चालक+(other);
	}

	Vector2 चालक+(Type scalar) अचर
	{
		फर्काऊ Matrix21::चालक+(scalar);
	}

	Vector2 चालक-(Vector2 other) अचर
	{
		फर्काऊ Matrix21::चालक-(other);
	}

	Vector2 चालक-(Type scalar) अचर
	{
		फर्काऊ Matrix21::चालक-(scalar);
	}

	Vector2 चालक-() अचर
	{
		फर्काऊ Matrix21::चालक-();
	}

	Vector2 चालक*(Type scalar) अचर
	{
		फर्काऊ Matrix21::चालक*(scalar);
	}

	Type चालक*(Vector2 b) अचर
	{
		फर्काऊ Vector<Type, 2>::चालक*(b);
	}

	Type चालक%(अचर Matrix21 &b) अचर
	{
		फर्काऊ (*एतत्).cross(b);
	}

};

using Vector2f = Vector2<भिन्न>;
using Vector2d = Vector2<द्विगुण>;

} // namespace matrix
