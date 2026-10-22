/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#هدايت once

#ضمن "Vector.hpp"

نطاق ميٽرڪس
{

سانچو<قسم_نالو Type>
صنف Vector2 : عام ويڪٽر<Type, 2>
{
عام:

	استعمال Matrix21 = ميٽرڪس_2<Type, 2, 1>;
	استعمال Vector3 = ويڪٽر<Type, 3>;

	Vector2() = افتراضي;

	Vector2(ثابت Matrix21 &other) :
		ويڪٽر<Type, 2>(other)
	{
	}

	واضح Vector2(ثابت Type معلومات_2[2]) :
		ويڪٽر<Type, 2>(معلومات_2)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*هذا);
		v(0) = x;
		v(1) = y;
	}

	استعمال base = ويڪٽر<Type, 2>;
	استعمال base::base;

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
		أرجع ويڪٽر<Type, 2>::عامل*(b);
	}

	Type عامل%(ثابت Matrix21 &b) ثابت
	{
		أرجع (*هذا).cross(b);
	}

};

استعمال Vector2f = Vector2<عائم>;
استعمال Vector2d = Vector2<مزدوج>;

} // namespace matrix
