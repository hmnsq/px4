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

	Vector3() = पूर्वनिर्धारित;

	Vector3(स्थिरांक Matrix31 &other) :
		Vector<Type, 3>(other)
	{
	}

	explicit Vector3(स्थिरांक Type data_[3]) :
		Vector<Type, 3>(data_)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*हे);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	using base = Vector<Type, 3>;
	using base::base;

	Vector3 cross(स्थिरांक Matrix31 &b) स्थिरांक
	{
		स्थिरांक Vector3 &a(*हे);
		परत {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	ओळीत Vector3 चालक+(Vector3 other) स्थिरांक
	{
		परत Matrix31::चालक+(other);
	}

	ओळीत Vector3 चालक+(Type scalar) स्थिरांक
	{
		परत Matrix31::चालक+(scalar);
	}

	ओळीत Vector3 चालक-(Vector3 other) स्थिरांक
	{
		परत Matrix31::चालक-(other);
	}

	ओळीत Vector3 चालक-(Type scalar) स्थिरांक
	{
		परत Matrix31::चालक-(scalar);
	}

	ओळीत Vector3 चालक-() स्थिरांक
	{
		परत Matrix31::चालक-();
	}

	ओळीत Vector3 चालक*(Type scalar) स्थिरांक
	{
		परत Matrix31::चालक*(scalar);
	}

	ओळीत Type चालक*(Vector3 b) स्थिरांक
	{
		परत Vector<Type, 3>::चालक*(b);
	}

	ओळीत Vector3 चालक%(स्थिरांक Matrix31 &b) स्थिरांक
	{
		परत (*हे).cross(b);
	}

	ConstSlice<Type, 2, 1, 3, 1> xy() स्थिरांक
	{
		परत {0, 0, हे};
	}

	Slice<Type, 2, 1, 3, 1> xy()
	{
		परत {0, 0, हे};
	}

	Dcm<Type> hat() स्थिरांक      // inverse to Dcm.vee() operation
	{
		स्थिरांक Vector3 &v(*हे);
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
		परत A;
	}

};

using Vector3f = Vector3<दशांश>;
using Vector3d = Vector3<द्विगुण>;

} // namespace matrix
