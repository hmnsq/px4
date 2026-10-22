/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#અન્તર્ભાવ "Vector.hpp"

નામાકાશ matrix
{

template <typename Type>
વર્ગ Dcm;

template<typename Type>
વર્ગ Vector3 : જાહેર Vector<Type, 3>
{
જાહેર:

	using Matrix31 = Matrix<Type, 3, 1>;

	Vector3() = મૂળભૂત;

	Vector3(અચલ Matrix31 &other) :
		Vector<Type, 3>(other)
	{
	}

	explicit Vector3(અચલ Type data_[3]) :
		Vector<Type, 3>(data_)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*આ);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	using base = Vector<Type, 3>;
	using base::base;

	Vector3 cross(અચલ Matrix31 &b) અચલ
	{
		અચલ Vector3 &a(*આ);
		પાછા {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	અંતઃરેખ Vector3 operator+(Vector3 other) અચલ
	{
		પાછા Matrix31::operator+(other);
	}

	અંતઃરેખ Vector3 operator+(Type scalar) અચલ
	{
		પાછા Matrix31::operator+(scalar);
	}

	અંતઃરેખ Vector3 operator-(Vector3 other) અચલ
	{
		પાછા Matrix31::operator-(other);
	}

	અંતઃરેખ Vector3 operator-(Type scalar) અચલ
	{
		પાછા Matrix31::operator-(scalar);
	}

	અંતઃરેખ Vector3 operator-() અચલ
	{
		પાછા Matrix31::operator-();
	}

	અંતઃરેખ Vector3 operator*(Type scalar) અચલ
	{
		પાછા Matrix31::operator*(scalar);
	}

	અંતઃરેખ Type operator*(Vector3 b) અચલ
	{
		પાછા Vector<Type, 3>::operator*(b);
	}

	અંતઃરેખ Vector3 operator%(અચલ Matrix31 &b) અચલ
	{
		પાછા (*આ).cross(b);
	}

	ConstSlice<Type, 2, 1, 3, 1> xy() અચલ
	{
		પાછા {0, 0, આ};
	}

	Slice<Type, 2, 1, 3, 1> xy()
	{
		પાછા {0, 0, આ};
	}

	Dcm<Type> hat() અચલ      // inverse to Dcm.vee() operation
	{
		અચલ Vector3 &v(*આ);
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
		પાછા A;
	}

};

using Vector3f = Vector3<દશાંશ>;
using Vector3d = Vector3<દ્વિગુણ>;

} // namespace matrix
