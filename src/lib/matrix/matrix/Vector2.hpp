/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ہدایت once

#ضمن "Vector.hpp"

نطاق میٹرکس
{

سانچا<قسم_ناں Type>
صنف Vector2 : عام سمتیہ<Type, 2>
{
عام:

	ورتوں Matrix21 = میٹرکس_2<Type, 2, 1>;
	ورتوں Vector3 = سمتیہ<Type, 3>;

	Vector2() = افتراضي;

	Vector2(ثابت Matrix21 &other) :
		سمتیہ<Type, 2>(other)
	{
	}

	واضح Vector2(ثابت Type انکڑے_2[2]) :
		سمتیہ<Type, 2>(انکڑے_2)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*هذا);
		v(0) = x;
		v(1) = y;
	}

	ورتوں base = سمتیہ<Type, 2>;
	ورتوں base::base;

	واضح Vector2(ثابت Vector3 &other)
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

	Vector2 عامل+(Vector2 other) ثابت
	{
		أرجع Matrix21::عامل+(other);
	}

	Vector2 عامل+(Type scalar) ثابت
	{
		أرجع Matrix21::عامل+(scalar);
	}

	Vector2 عامل-(Vector2 other) ثابت
	{
		أرجع Matrix21::عامل-(other);
	}

	Vector2 عامل-(Type scalar) ثابت
	{
		أرجع Matrix21::عامل-(scalar);
	}

	Vector2 عامل-() ثابت
	{
		أرجع Matrix21::عامل-();
	}

	Vector2 عامل*(Type scalar) ثابت
	{
		أرجع Matrix21::عامل*(scalar);
	}

	Type عامل*(Vector2 b) ثابت
	{
		أرجع سمتیہ<Type, 2>::عامل*(b);
	}

	Type عامل%(ثابت Matrix21 &b) ثابت
	{
		أرجع (*هذا).cross(b);
	}

};

ورتوں Vector2f = Vector2<عائم>;
ورتوں Vector2d = Vector2<مزدوج>;

} // namespace matrix
