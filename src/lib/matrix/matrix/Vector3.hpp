/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#अन्तर्भाव "Vector.hpp"

नामाकाश matrix
{

ढाँचा <typename Type>
वर्ग Dcm;

ढाँचा<typename Type>
वर्ग Vector3 : सार्वजनिक Vector<Type, 3>
{
सार्वजनिक:

	using Matrix31 = Matrix<Type, 3, 1>;

	Vector3() = स्वभाव;

	Vector3(ध्रुव Matrix31 &other) :
		Vector<Type, 3>(other)
	{
	}

	explicit Vector3(ध्रुव Type data_[3]) :
		Vector<Type, 3>(data_)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*एतत्);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	using base = Vector<Type, 3>;
	using base::base;

	Vector3 cross(ध्रुव Matrix31 &b) ध्रुव
	{
		ध्रुव Vector3 &a(*एतत्);
		प्रत्यागम {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	अन्तःस्थ Vector3 चालक+(Vector3 other) ध्रुव
	{
		प्रत्यागम Matrix31::चालक+(other);
	}

	अन्तःस्थ Vector3 चालक+(Type scalar) ध्रुव
	{
		प्रत्यागम Matrix31::चालक+(scalar);
	}

	अन्तःस्थ Vector3 चालक-(Vector3 other) ध्रुव
	{
		प्रत्यागम Matrix31::चालक-(other);
	}

	अन्तःस्थ Vector3 चालक-(Type scalar) ध्रुव
	{
		प्रत्यागम Matrix31::चालक-(scalar);
	}

	अन्तःस्थ Vector3 चालक-() ध्रुव
	{
		प्रत्यागम Matrix31::चालक-();
	}

	अन्तःस्थ Vector3 चालक*(Type scalar) ध्रुव
	{
		प्रत्यागम Matrix31::चालक*(scalar);
	}

	अन्तःस्थ Type चालक*(Vector3 b) ध्रुव
	{
		प्रत्यागम Vector<Type, 3>::चालक*(b);
	}

	अन्तःस्थ Vector3 चालक%(ध्रुव Matrix31 &b) ध्रुव
	{
		प्रत्यागम (*एतत्).cross(b);
	}

	ConstSlice<Type, 2, 1, 3, 1> xy() ध्रुव
	{
		प्रत्यागम {0, 0, एतत्};
	}

	Slice<Type, 2, 1, 3, 1> xy()
	{
		प्रत्यागम {0, 0, एतत्};
	}

	Dcm<Type> hat() ध्रुव      // inverse to Dcm.vee() operation
	{
		ध्रुव Vector3 &v(*एतत्);
		Dcm<Type> A;
		A(0, 0) = 0;
		A(0, 1) = -v(2);
		A(0, 2) = v(1);
		A(1, 0) = v(2);
		A(1, 1) = 0;
		A(1, 2) = -v(0);
		A(2, 0) = -v(1);
		A(2, 1) = v(0);
		A(2, 2) = 0;
		प्रत्यागम A;
	}

};

using Vector3f = Vector3<भिन्न>;
using Vector3d = Vector3<द्विगुण>;

} // namespace matrix
