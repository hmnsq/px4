/**
 * @file Vector3.hpp
 *
 * 3D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#لارښوونه once

#ضمن "Vector.hpp"

نطاق ماتریکس
{

کينډۍ <ډول_نوم Type>
صنف Dcm;

کينډۍ<ډول_نوم Type>
صنف Vector3 : عام ویکټور<Type, 3>
{
عام:

	کارول Matrix31 = ماتریکس_2<Type, 3, 1>;

	Vector3() = افتراضي;

	Vector3(ثابت Matrix31 &other) :
		ویکټور<Type, 3>(other)
	{
	}

	څرګند Vector3(ثابت Type معلومات_2[3]) :
		ویکټور<Type, 3>(معلومات_2)
	{
	}

	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*هذا);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	کارول base = ویکټور<Type, 3>;
	کارول base::base;

	Vector3 cross(ثابت Matrix31 &b) ثابت
	{
		ثابت Vector3 &a(*هذا);
		أرجع {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	مضمن Vector3 چلوونکی+(Vector3 other) ثابت
	{
		أرجع Matrix31::چلوونکی+(other);
	}

	مضمن Vector3 چلوونکی+(Type scalar) ثابت
	{
		أرجع Matrix31::چلوونکی+(scalar);
	}

	مضمن Vector3 چلوونکی-(Vector3 other) ثابت
	{
		أرجع Matrix31::چلوونکی-(other);
	}

	مضمن Vector3 چلوونکی-(Type scalar) ثابت
	{
		أرجع Matrix31::چلوونکی-(scalar);
	}

	مضمن Vector3 چلوونکی-() ثابت
	{
		أرجع Matrix31::چلوونکی-();
	}

	مضمن Vector3 چلوونکی*(Type scalar) ثابت
	{
		أرجع Matrix31::چلوونکی*(scalar);
	}

	مضمن Type چلوونکی*(Vector3 b) ثابت
	{
		أرجع ویکټور<Type, 3>::چلوونکی*(b);
	}

	مضمن Vector3 چلوونکی%(ثابت Matrix31 &b) ثابت
	{
		أرجع (*هذا).cross(b);
	}

	Const_ټوټه<Type, 2, 1, 3, 1> xy() ثابت
	{
		أرجع {0, 0, هذا};
	}

	ټوټه_2<Type, 2, 1, 3, 1> xy()
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

کارول Vector3f = Vector3<عائم>;
کارول Vector3d = Vector3<مزدوج>;

} // namespace matrix
