/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ಅನ್ತರ್ಭಾವ "Vector.hpp"

ನಾಮಾಕಾಶ matrix
{

template <typename Type>
ವರ್ಗ Dcm;

template<typename Type>
ವರ್ಗ Vector3 : ಸಾರ್ವಜನಿಕ Vector<Type, 3>
{
ಸಾರ್ವಜನಿಕ:

	using Matrix31 = Matrix<Type, 3, 1>;

	Vector3() = ಸ್ವಭಾವ;

	Vector3(ಧ್ರುವ Matrix31 &other) :
		Vector<Type, 3>(other)
	{
	}

	explicit Vector3(ಧ್ರುವ Type data_[3]) :
		Vector<Type, 3>(data_)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*ಏತತ್);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	using base = Vector<Type, 3>;
	using base::base;

	Vector3 cross(ಧ್ರುವ Matrix31 &b) ಧ್ರುವ
	{
		ಧ್ರುವ Vector3 &a(*ಏತತ್);
		ಪ್ರತ್ಯಾಗಮ {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	ಅನ್ತಃಸ್ಥ Vector3 operator+(Vector3 other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix31::operator+(other);
	}

	ಅನ್ತಃಸ್ಥ Vector3 operator+(Type scalar) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix31::operator+(scalar);
	}

	ಅನ್ತಃಸ್ಥ Vector3 operator-(Vector3 other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix31::operator-(other);
	}

	ಅನ್ತಃಸ್ಥ Vector3 operator-(Type scalar) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix31::operator-(scalar);
	}

	ಅನ್ತಃಸ್ಥ Vector3 operator-() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix31::operator-();
	}

	ಅನ್ತಃಸ್ಥ Vector3 operator*(Type scalar) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Matrix31::operator*(scalar);
	}

	ಅನ್ತಃಸ್ಥ Type operator*(Vector3 b) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Vector<Type, 3>::operator*(b);
	}

	ಅನ್ತಃಸ್ಥ Vector3 operator%(ಧ್ರುವ Matrix31 &b) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್).cross(b);
	}

	ConstSlice<Type, 2, 1, 3, 1> xy() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ {0, 0, ಏತತ್};
	}

	Slice<Type, 2, 1, 3, 1> xy()
	{
		ಪ್ರತ್ಯಾಗಮ {0, 0, ಏತತ್};
	}

	Dcm<Type> hat() ಧ್ರುವ      // inverse to Dcm.vee() operation
	{
		ಧ್ರುವ Vector3 &v(*ಏತತ್);
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
		ಪ್ರತ್ಯಾಗಮ A;
	}

};

using Vector3f = Vector3<ಭಿನ್ನ>;
using Vector3d = Vector3<ದ್ವಿಗುಣ>;

} // namespace matrix
