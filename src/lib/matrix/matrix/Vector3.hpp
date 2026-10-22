/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#അന്തര്ഭാവ "Vector.hpp"

നാമാകാശ matrix
{

template <typename Type>
വർഗ്ഗം Dcm;

template<typename Type>
വർഗ്ഗം Vector3 : പൊതു Vector<Type, 3>
{
പൊതു:

	using Matrix31 = Matrix<Type, 3, 1>;

	Vector3() = സ്ഥിരസ്ഥിതി;

	Vector3(സ്ഥിരം Matrix31 &other) :
		Vector<Type, 3>(other)
	{
	}

	explicit Vector3(സ്ഥിരം Type data_[3]) :
		Vector<Type, 3>(data_)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*ഇത്);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	using base = Vector<Type, 3>;
	using base::base;

	Vector3 cross(സ്ഥിരം Matrix31 &b) സ്ഥിരം
	{
		സ്ഥിരം Vector3 &a(*ഇത്);
		തിരികെ {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	വരിയിൽ Vector3 operator+(Vector3 other) സ്ഥിരം
	{
		തിരികെ Matrix31::operator+(other);
	}

	വരിയിൽ Vector3 operator+(Type scalar) സ്ഥിരം
	{
		തിരികെ Matrix31::operator+(scalar);
	}

	വരിയിൽ Vector3 operator-(Vector3 other) സ്ഥിരം
	{
		തിരികെ Matrix31::operator-(other);
	}

	വരിയിൽ Vector3 operator-(Type scalar) സ്ഥിരം
	{
		തിരികെ Matrix31::operator-(scalar);
	}

	വരിയിൽ Vector3 operator-() സ്ഥിരം
	{
		തിരികെ Matrix31::operator-();
	}

	വരിയിൽ Vector3 operator*(Type scalar) സ്ഥിരം
	{
		തിരികെ Matrix31::operator*(scalar);
	}

	വരിയിൽ Type operator*(Vector3 b) സ്ഥിരം
	{
		തിരികെ Vector<Type, 3>::operator*(b);
	}

	വരിയിൽ Vector3 operator%(സ്ഥിരം Matrix31 &b) സ്ഥിരം
	{
		തിരികെ (*ഇത്).cross(b);
	}

	ConstSlice<Type, 2, 1, 3, 1> xy() സ്ഥിരം
	{
		തിരികെ {0, 0, ഇത്};
	}

	Slice<Type, 2, 1, 3, 1> xy()
	{
		തിരികെ {0, 0, ഇത്};
	}

	Dcm<Type> hat() സ്ഥിരം      // inverse to Dcm.vee() operation
	{
		സ്ഥിരം Vector3 &v(*ഇത്);
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
		തിരികെ A;
	}

};

using Vector3f = Vector3<ദശാംശം>;
using Vector3d = Vector3<ഇരട്ട>;

} // namespace matrix
