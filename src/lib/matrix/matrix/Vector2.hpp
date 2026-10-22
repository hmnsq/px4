/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#رهنمود once

#ضمن "Vector.hpp"

نطاق ماتریس
{

قالب<نام_نوع Type>
صنف Vector2 : عام وکتور<Type, 2>
{
عام:

	استفاده Matrix21 = ماتریس_2<Type, 2, 1>;
	استفاده Vector3 = وکتور<Type, 3>;

	Vector2() = افتراضي;

	Vector2(ثابت Matrix21 &other) :
		وکتور<Type, 2>(other)
	{
	}

	صریح Vector2(ثابت Type داده_2[2]) :
		وکتور<Type, 2>(داده_2)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*هذا);
		v(0) = x;
		v(1) = y;
	}

	استفاده base = وکتور<Type, 2>;
	استفاده base::base;

	صریح Vector2(ثابت Vector3 &other)
	{
		Vector2 &v(*هذا);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(ثابت Matrix21 &b) ثابت
	{
		ثابت Vector2 &a(*هذا);
		أرجع a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 عملگر+(Vector2 other) ثابت
	{
		أرجع Matrix21::عملگر+(other);
	}

	Vector2 عملگر+(Type scalar) ثابت
	{
		أرجع Matrix21::عملگر+(scalar);
	}

	Vector2 عملگر-(Vector2 other) ثابت
	{
		أرجع Matrix21::عملگر-(other);
	}

	Vector2 عملگر-(Type scalar) ثابت
	{
		أرجع Matrix21::عملگر-(scalar);
	}

	Vector2 عملگر-() ثابت
	{
		أرجع Matrix21::عملگر-();
	}

	Vector2 عملگر*(Type scalar) ثابت
	{
		أرجع Matrix21::عملگر*(scalar);
	}

	Type عملگر*(Vector2 b) ثابت
	{
		أرجع وکتور<Type, 2>::عملگر*(b);
	}

	Type عملگر%(ثابت Matrix21 &b) ثابت
	{
		أرجع (*هذا).cross(b);
	}

};

استفاده Vector2f = Vector2<عائم>;
استفاده Vector2d = Vector2<مزدوج>;

} // namespace matrix
