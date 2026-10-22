/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ہدایت once

#شامل "Vector.hpp"

نامخلا میٹرکس
{

سانچہ <قسم_نام Type>
جماعت Dcm;

سانچہ<قسم_نام Type>
جماعت Vector3 : عوامی سمتیہ<Type, 3>
{
عوامی:

	استعمال Matrix31 = میٹرکس_2<Type, 3, 1>;

	Vector3() = طےشدہ;

	Vector3(ثابت Matrix31 &other) :
		سمتیہ<Type, 3>(other)
	{
	}

	صریح Vector3(ثابت Type ڈیٹا_2[3]) :
		سمتیہ<Type, 3>(ڈیٹا_2)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*یہ);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	استعمال base = سمتیہ<Type, 3>;
	استعمال base::base;

	Vector3 cross(ثابت Matrix31 &b) ثابت
	{
		ثابت Vector3 &a(*یہ);
		واپس {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	اندرونی Vector3 عامل+(Vector3 other) ثابت
	{
		واپس Matrix31::عامل+(other);
	}

	اندرونی Vector3 عامل+(Type scalar) ثابت
	{
		واپس Matrix31::عامل+(scalar);
	}

	اندرونی Vector3 عامل-(Vector3 other) ثابت
	{
		واپس Matrix31::عامل-(other);
	}

	اندرونی Vector3 عامل-(Type scalar) ثابت
	{
		واپس Matrix31::عامل-(scalar);
	}

	اندرونی Vector3 عامل-() ثابت
	{
		واپس Matrix31::عامل-();
	}

	اندرونی Vector3 عامل*(Type scalar) ثابت
	{
		واپس Matrix31::عامل*(scalar);
	}

	اندرونی Type عامل*(Vector3 b) ثابت
	{
		واپس سمتیہ<Type, 3>::عامل*(b);
	}

	اندرونی Vector3 عامل%(ثابت Matrix31 &b) ثابت
	{
		واپس (*یہ).cross(b);
	}

	Const_قاش<Type, 2, 1, 3, 1> xy() ثابت
	{
		واپس {0, 0, یہ};
	}

	قاش_2<Type, 2, 1, 3, 1> xy()
	{
		واپس {0, 0, یہ};
	}

	Dcm<Type> hat() ثابت      // inverse to Dcm.vee() operation
	{
		ثابت Vector3 &v(*یہ);
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
		واپس A;
	}

};

استعمال Vector3f = Vector3<اعشاری>;
استعمال Vector3d = Vector3<دوہرا>;

} // namespace matrix
