/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ضمن "Vector.hpp"

نطاق matrix
{

template <typename Type>
صنف Dcm;

template<typename Type>
صنف Vector3 : عام Vector<Type, 3>
{
عام:

	using Matrix31 = Matrix<Type, 3, 1>;

	Vector3() = افتراضي;

	Vector3(ثابت Matrix31 &other) :
		Vector<Type, 3>(other)
	{
	}

	explicit Vector3(ثابت Type data_[3]) :
		Vector<Type, 3>(data_)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*هذا);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	using base = Vector<Type, 3>;
	using base::base;

	Vector3 cross(ثابت Matrix31 &b) ثابت
	{
		ثابت Vector3 &a(*هذا);
		إرجاع {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	مضمن Vector3 operator+(Vector3 other) ثابت
	{
		إرجاع Matrix31::operator+(other);
	}

	مضمن Vector3 operator+(Type scalar) ثابت
	{
		إرجاع Matrix31::operator+(scalar);
	}

	مضمن Vector3 operator-(Vector3 other) ثابت
	{
		إرجاع Matrix31::operator-(other);
	}

	مضمن Vector3 operator-(Type scalar) ثابت
	{
		إرجاع Matrix31::operator-(scalar);
	}

	مضمن Vector3 operator-() ثابت
	{
		إرجاع Matrix31::operator-();
	}

	مضمن Vector3 operator*(Type scalar) ثابت
	{
		إرجاع Matrix31::operator*(scalar);
	}

	مضمن Type operator*(Vector3 b) ثابت
	{
		إرجاع Vector<Type, 3>::operator*(b);
	}

	مضمن Vector3 operator%(ثابت Matrix31 &b) ثابت
	{
		إرجاع (*هذا).cross(b);
	}

	ConstSlice<Type, 2, 1, 3, 1> xy() ثابت
	{
		إرجاع {0, 0, هذا};
	}

	Slice<Type, 2, 1, 3, 1> xy()
	{
		إرجاع {0, 0, هذا};
	}

	Dcm<Type> hat() ثابت      // inverse to Dcm.vee() operation
	{
		ثابت Vector3 &v(*هذا);
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
		إرجاع A;
	}

};

using Vector3f = Vector3<عائم>;
using Vector3d = Vector3<مزدوج>;

} // namespace matrix
