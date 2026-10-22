/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ہدایت once

#شامل "Vector.hpp"

نامخلا میٹرکس
{

سانچہ<قسم_نام Type>
جماعت Vector2 : عوامی سمتیہ<Type, 2>
{
عوامی:

	استعمال Matrix21 = میٹرکس_2<Type, 2, 1>;
	استعمال Vector3 = سمتیہ<Type, 3>;

	Vector2() = طےشدہ;

	Vector2(ثابت Matrix21 &other) :
		سمتیہ<Type, 2>(other)
	{
	}

	صریح Vector2(ثابت Type ڈیٹا_2[2]) :
		سمتیہ<Type, 2>(ڈیٹا_2)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*یہ);
		v(0) = x;
		v(1) = y;
	}

	استعمال base = سمتیہ<Type, 2>;
	استعمال base::base;

	صریح Vector2(ثابت Vector3 &other)
	{
		Vector2 &v(*یہ);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(ثابت Matrix21 &b) ثابت
	{
		ثابت Vector2 &a(*یہ);
		واپس a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 عامل+(Vector2 other) ثابت
	{
		واپس Matrix21::عامل+(other);
	}

	Vector2 عامل+(Type scalar) ثابت
	{
		واپس Matrix21::عامل+(scalar);
	}

	Vector2 عامل-(Vector2 other) ثابت
	{
		واپس Matrix21::عامل-(other);
	}

	Vector2 عامل-(Type scalar) ثابت
	{
		واپس Matrix21::عامل-(scalar);
	}

	Vector2 عامل-() ثابت
	{
		واپس Matrix21::عامل-();
	}

	Vector2 عامل*(Type scalar) ثابت
	{
		واپس Matrix21::عامل*(scalar);
	}

	Type عامل*(Vector2 b) ثابت
	{
		واپس سمتیہ<Type, 2>::عامل*(b);
	}

	Type عامل%(ثابت Matrix21 &b) ثابت
	{
		واپس (*یہ).cross(b);
	}

};

استعمال Vector2f = Vector2<اعشاری>;
استعمال Vector2d = Vector2<دوہرا>;

} // namespace matrix
