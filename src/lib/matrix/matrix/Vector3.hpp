/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#অন্তর্ভাব "Vector.hpp"

নামাকাশ matrix
{

template <typename Type>
বর্গ Dcm;

template<typename Type>
বর্গ Vector3 : সার্বজনিক Vector<Type, 3>
{
সার্বজনিক:

	using Matrix31 = Matrix<Type, 3, 1>;

	Vector3() = স্বভাব;

	Vector3(ধ্রুব Matrix31 &other) :
		Vector<Type, 3>(other)
	{
	}

	explicit Vector3(ধ্রুব Type data_[3]) :
		Vector<Type, 3>(data_)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*এতত্);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	using base = Vector<Type, 3>;
	using base::base;

	Vector3 cross(ধ্রুব Matrix31 &b) ধ্রুব
	{
		ধ্রুব Vector3 &a(*এতত্);
		প্রত্যাগম {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	অন্তঃস্থ Vector3 operator+(Vector3 other) ধ্রুব
	{
		প্রত্যাগম Matrix31::operator+(other);
	}

	অন্তঃস্থ Vector3 operator+(Type scalar) ধ্রুব
	{
		প্রত্যাগম Matrix31::operator+(scalar);
	}

	অন্তঃস্থ Vector3 operator-(Vector3 other) ধ্রুব
	{
		প্রত্যাগম Matrix31::operator-(other);
	}

	অন্তঃস্থ Vector3 operator-(Type scalar) ধ্রুব
	{
		প্রত্যাগম Matrix31::operator-(scalar);
	}

	অন্তঃস্থ Vector3 operator-() ধ্রুব
	{
		প্রত্যাগম Matrix31::operator-();
	}

	অন্তঃস্থ Vector3 operator*(Type scalar) ধ্রুব
	{
		প্রত্যাগম Matrix31::operator*(scalar);
	}

	অন্তঃস্থ Type operator*(Vector3 b) ধ্রুব
	{
		প্রত্যাগম Vector<Type, 3>::operator*(b);
	}

	অন্তঃস্থ Vector3 operator%(ধ্রুব Matrix31 &b) ধ্রুব
	{
		প্রত্যাগম (*এতত্).cross(b);
	}

	ConstSlice<Type, 2, 1, 3, 1> xy() ধ্রুব
	{
		প্রত্যাগম {0, 0, এতত্};
	}

	Slice<Type, 2, 1, 3, 1> xy()
	{
		প্রত্যাগম {0, 0, এতত্};
	}

	Dcm<Type> hat() ধ্রুব      // inverse to Dcm.vee() operation
	{
		ধ্রুব Vector3 &v(*এতত্);
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
		প্রত্যাগম A;
	}

};

using Vector3f = Vector3<ভিন্ন>;
using Vector3d = Vector3<দ্বিগুণ>;

} // namespace matrix
