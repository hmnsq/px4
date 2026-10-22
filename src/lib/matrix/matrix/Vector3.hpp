/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#அந்தர்பாவ "Vector.hpp"

நாமாகாஶ matrix
{

template <typename Type>
வகுப்பு Dcm;

template<typename Type>
வகுப்பு Vector3 : பொது Vector<Type, 3>
{
பொது:

	using Matrix31 = Matrix<Type, 3, 1>;

	Vector3() = இயல்பு;

	Vector3(மாறிலி Matrix31 &other) :
		Vector<Type, 3>(other)
	{
	}

	explicit Vector3(மாறிலி Type data_[3]) :
		Vector<Type, 3>(data_)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*இது);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	using base = Vector<Type, 3>;
	using base::base;

	Vector3 cross(மாறிலி Matrix31 &b) மாறிலி
	{
		மாறிலி Vector3 &a(*இது);
		திரும்பு {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	உள்வரி Vector3 operator+(Vector3 other) மாறிலி
	{
		திரும்பு Matrix31::operator+(other);
	}

	உள்வரி Vector3 operator+(Type scalar) மாறிலி
	{
		திரும்பு Matrix31::operator+(scalar);
	}

	உள்வரி Vector3 operator-(Vector3 other) மாறிலி
	{
		திரும்பு Matrix31::operator-(other);
	}

	உள்வரி Vector3 operator-(Type scalar) மாறிலி
	{
		திரும்பு Matrix31::operator-(scalar);
	}

	உள்வரி Vector3 operator-() மாறிலி
	{
		திரும்பு Matrix31::operator-();
	}

	உள்வரி Vector3 operator*(Type scalar) மாறிலி
	{
		திரும்பு Matrix31::operator*(scalar);
	}

	உள்வரி Type operator*(Vector3 b) மாறிலி
	{
		திரும்பு Vector<Type, 3>::operator*(b);
	}

	உள்வரி Vector3 operator%(மாறிலி Matrix31 &b) மாறிலி
	{
		திரும்பு (*இது).cross(b);
	}

	ConstSlice<Type, 2, 1, 3, 1> xy() மாறிலி
	{
		திரும்பு {0, 0, இது};
	}

	Slice<Type, 2, 1, 3, 1> xy()
	{
		திரும்பு {0, 0, இது};
	}

	Dcm<Type> hat() மாறிலி      // inverse to Dcm.vee() operation
	{
		மாறிலி Vector3 &v(*இது);
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
		திரும்பு A;
	}

};

using Vector3f = Vector3<தசம>;
using Vector3d = Vector3<இரட்டை>;

} // namespace matrix
