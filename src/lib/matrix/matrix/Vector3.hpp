/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ہدایت once

#ضمن "Vector.hpp"

نطاق میٹرکس
{

سانچا <قسم_ناں Type>
صنف Dcm;

سانچا<قسم_ناں Type>
صنف Vector3 : عام سمتیہ<Type, 3>
{
عام:

	ورتوں Matrix31 = میٹرکس_2<Type, 3, 1>;

	Vector3() = افتراضي;

	Vector3(ثابت Matrix31 &other) :
		سمتیہ<Type, 3>(other)
	{
	}

	واضح Vector3(ثابت Type انکڑے_2[3]) :
		سمتیہ<Type, 3>(انکڑے_2)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*هذا);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	ورتوں base = سمتیہ<Type, 3>;
	ورتوں base::base;

	Vector3 cross(ثابت Matrix31 &b) ثابت
	{
		ثابت Vector3 &a(*هذا);
		أرجع {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	مضمن Vector3 عامل+(Vector3 other) ثابت
	{
		أرجع Matrix31::عامل+(other);
	}

	مضمن Vector3 عامل+(Type scalar) ثابت
	{
		أرجع Matrix31::عامل+(scalar);
	}

	مضمن Vector3 عامل-(Vector3 other) ثابت
	{
		أرجع Matrix31::عامل-(other);
	}

	مضمن Vector3 عامل-(Type scalar) ثابت
	{
		أرجع Matrix31::عامل-(scalar);
	}

	مضمن Vector3 عامل-() ثابت
	{
		أرجع Matrix31::عامل-();
	}

	مضمن Vector3 عامل*(Type scalar) ثابت
	{
		أرجع Matrix31::عامل*(scalar);
	}

	مضمن Type عامل*(Vector3 b) ثابت
	{
		أرجع سمتیہ<Type, 3>::عامل*(b);
	}

	مضمن Vector3 عامل%(ثابت Matrix31 &b) ثابت
	{
		أرجع (*هذا).cross(b);
	}

	Const_ٹکڑا<Type, 2, 1, 3, 1> xy() ثابت
	{
		أرجع {0, 0, هذا};
	}

	ٹکڑا_2<Type, 2, 1, 3, 1> xy()
	{
		أرجع {0, 0, هذا};
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
		أرجع A;
	}

};

ورتوں Vector3f = Vector3<عائم>;
ورتوں Vector3d = Vector3<مزدوج>;

} // namespace matrix
