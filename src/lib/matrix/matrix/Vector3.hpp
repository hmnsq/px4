/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#رهنمود once

#ضمن "Vector.hpp"

نطاق ماتریس
{

قالب <نام_نوع Type>
صنف Dcm;

قالب<نام_نوع Type>
صنف Vector3 : عام وکتور<Type, 3>
{
عام:

	استفاده Matrix31 = ماتریس_2<Type, 3, 1>;

	Vector3() = افتراضي;

	Vector3(ثابت Matrix31 &other) :
		وکتور<Type, 3>(other)
	{
	}

	صریح Vector3(ثابت Type داده_2[3]) :
		وکتور<Type, 3>(داده_2)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*هذا);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	استفاده base = وکتور<Type, 3>;
	استفاده base::base;

	Vector3 cross(ثابت Matrix31 &b) ثابت
	{
		ثابت Vector3 &a(*هذا);
		أرجع {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	مضمن Vector3 عملگر+(Vector3 other) ثابت
	{
		أرجع Matrix31::عملگر+(other);
	}

	مضمن Vector3 عملگر+(Type scalar) ثابت
	{
		أرجع Matrix31::عملگر+(scalar);
	}

	مضمن Vector3 عملگر-(Vector3 other) ثابت
	{
		أرجع Matrix31::عملگر-(other);
	}

	مضمن Vector3 عملگر-(Type scalar) ثابت
	{
		أرجع Matrix31::عملگر-(scalar);
	}

	مضمن Vector3 عملگر-() ثابت
	{
		أرجع Matrix31::عملگر-();
	}

	مضمن Vector3 عملگر*(Type scalar) ثابت
	{
		أرجع Matrix31::عملگر*(scalar);
	}

	مضمن Type عملگر*(Vector3 b) ثابت
	{
		أرجع وکتور<Type, 3>::عملگر*(b);
	}

	مضمن Vector3 عملگر%(ثابت Matrix31 &b) ثابت
	{
		أرجع (*هذا).cross(b);
	}

	Const_برش<Type, 2, 1, 3, 1> xy() ثابت
	{
		أرجع {0, 0, هذا};
	}

	برش_2<Type, 2, 1, 3, 1> xy()
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

استفاده Vector3f = Vector3<عائم>;
استفاده Vector3d = Vector3<مزدوج>;

} // namespace matrix
