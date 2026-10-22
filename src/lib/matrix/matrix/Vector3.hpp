/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ "Vector.hpp"

ନାମାକାଶ matrix
{

template <typename Type>
ଶ୍ରେଣୀ Dcm;

template<typename Type>
ଶ୍ରେଣୀ Vector3 : ସର୍ବସାଧାରଣ Vector<Type, 3>
{
ସର୍ବସାଧାରଣ:

	using Matrix31 = Matrix<Type, 3, 1>;

	Vector3() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	Vector3(ଅଚଳ Matrix31 &other) :
		Vector<Type, 3>(other)
	{
	}

	explicit Vector3(ଅଚଳ Type data_[3]) :
		Vector<Type, 3>(data_)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*ଏହି);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	using base = Vector<Type, 3>;
	using base::base;

	Vector3 cross(ଅଚଳ Matrix31 &b) ଅଚଳ
	{
		ଅଚଳ Vector3 &a(*ଏହି);
		ଫେରନ୍ତୁ {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	ଧାଡ଼ିଭିତରେ Vector3 operator+(Vector3 other) ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix31::operator+(other);
	}

	ଧାଡ଼ିଭିତରେ Vector3 operator+(Type scalar) ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix31::operator+(scalar);
	}

	ଧାଡ଼ିଭିତରେ Vector3 operator-(Vector3 other) ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix31::operator-(other);
	}

	ଧାଡ଼ିଭିତରେ Vector3 operator-(Type scalar) ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix31::operator-(scalar);
	}

	ଧାଡ଼ିଭିତରେ Vector3 operator-() ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix31::operator-();
	}

	ଧାଡ଼ିଭିତରେ Vector3 operator*(Type scalar) ଅଚଳ
	{
		ଫେରନ୍ତୁ Matrix31::operator*(scalar);
	}

	ଧାଡ଼ିଭିତରେ Type operator*(Vector3 b) ଅଚଳ
	{
		ଫେରନ୍ତୁ Vector<Type, 3>::operator*(b);
	}

	ଧାଡ଼ିଭିତରେ Vector3 operator%(ଅଚଳ Matrix31 &b) ଅଚଳ
	{
		ଫେରନ୍ତୁ (*ଏହି).cross(b);
	}

	ConstSlice<Type, 2, 1, 3, 1> xy() ଅଚଳ
	{
		ଫେରନ୍ତୁ {0, 0, ଏହି};
	}

	Slice<Type, 2, 1, 3, 1> xy()
	{
		ଫେରନ୍ତୁ {0, 0, ଏହି};
	}

	Dcm<Type> hat() ଅଚଳ      // inverse to Dcm.vee() operation
	{
		ଅଚଳ Vector3 &v(*ଏହି);
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
		ଫେରନ୍ତୁ A;
	}

};

using Vector3f = Vector3<ଦଶମିକ>;
using Vector3d = Vector3<ଦ୍ୱିଗୁଣ>;

} // namespace matrix
