/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#شامل "Vector.hpp"

فضاینام matrix
{

template <typename Type>
رده Dcm;

template<typename Type>
رده Vector3 : عمومی Vector<Type, 3>
{
عمومی:

	using Matrix31 = Matrix<Type, 3, 1>;

	Vector3() = پیشفرض;

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
		Vector3 &v(*این);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	using base = Vector<Type, 3>;
	using base::base;

	Vector3 cross(ثابت Matrix31 &b) ثابت
	{
		ثابت Vector3 &a(*این);
		بازگشت {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	درونخطی Vector3 operator+(Vector3 other) ثابت
	{
		بازگشت Matrix31::operator+(other);
	}

	درونخطی Vector3 operator+(Type scalar) ثابت
	{
		بازگشت Matrix31::operator+(scalar);
	}

	درونخطی Vector3 operator-(Vector3 other) ثابت
	{
		بازگشت Matrix31::operator-(other);
	}

	درونخطی Vector3 operator-(Type scalar) ثابت
	{
		بازگشت Matrix31::operator-(scalar);
	}

	درونخطی Vector3 operator-() ثابت
	{
		بازگشت Matrix31::operator-();
	}

	درونخطی Vector3 operator*(Type scalar) ثابت
	{
		بازگشت Matrix31::operator*(scalar);
	}

	درونخطی Type operator*(Vector3 b) ثابت
	{
		بازگشت Vector<Type, 3>::operator*(b);
	}

	درونخطی Vector3 operator%(ثابت Matrix31 &b) ثابت
	{
		بازگشت (*این).cross(b);
	}

	ConstSlice<Type, 2, 1, 3, 1> xy() ثابت
	{
		بازگشت {0, 0, این};
	}

	Slice<Type, 2, 1, 3, 1> xy()
	{
		بازگشت {0, 0, این};
	}

	Dcm<Type> hat() ثابت      // inverse to Dcm.vee() operation
	{
		ثابت Vector3 &v(*این);
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
		بازگشت A;
	}

};

using Vector3f = Vector3<اعشاری>;
using Vector3d = Vector3<دوگانه>;

} // namespace matrix
