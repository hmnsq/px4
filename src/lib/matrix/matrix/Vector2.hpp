/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#لارښوونه once

#ضمن "Vector.hpp"

نطاق ماتریکس
{

کينډۍ<ډول_نوم Type>
صنف Vector2 : عام ویکټور<Type, 2>
{
عام:

	کارول Matrix21 = ماتریکس_2<Type, 2, 1>;
	کارول Vector3 = ویکټور<Type, 3>;

	Vector2() = افتراضي;

	Vector2(ثابت Matrix21 &other) :
		ویکټور<Type, 2>(other)
	{
	}

	څرګند Vector2(ثابت Type معلومات_2[2]) :
		ویکټور<Type, 2>(معلومات_2)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*هذا);
		v(0) = x;
		v(1) = y;
	}

	کارول base = ویکټور<Type, 2>;
	کارول base::base;

	څرګند Vector2(ثابت Vector3 &other)
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

	Vector2 چلوونکی+(Vector2 other) ثابت
	{
		أرجع Matrix21::چلوونکی+(other);
	}

	Vector2 چلوونکی+(Type scalar) ثابت
	{
		أرجع Matrix21::چلوونکی+(scalar);
	}

	Vector2 چلوونکی-(Vector2 other) ثابت
	{
		أرجع Matrix21::چلوونکی-(other);
	}

	Vector2 چلوونکی-(Type scalar) ثابت
	{
		أرجع Matrix21::چلوونکی-(scalar);
	}

	Vector2 چلوونکی-() ثابت
	{
		أرجع Matrix21::چلوونکی-();
	}

	Vector2 چلوونکی*(Type scalar) ثابت
	{
		أرجع Matrix21::چلوونکی*(scalar);
	}

	Type چلوونکی*(Vector2 b) ثابت
	{
		أرجع ویکټور<Type, 2>::چلوونکی*(b);
	}

	Type چلوونکی%(ثابت Matrix21 &b) ثابت
	{
		أرجع (*هذا).cross(b);
	}

};

کارول Vector2f = Vector2<عائم>;
کارول Vector2d = Vector2<مزدوج>;

} // namespace matrix
