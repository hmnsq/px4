/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#لارښوونه once

#ضمن "Matrix.hpp"

نطاق ماتریکس
{

کينډۍ<ډول_نوم Type, size_t M>
صنف ویکټور : عام ماتریکس_2<Type, M, 1>
{
عام:
	کارول ماتریکس_M1 = ماتریکس_2<Type, M, 1>;

	ویکټور() = افتراضي;

	ویکټور(ثابت ماتریکس_M1 &other) :
		ماتریکس_M1(other)
	{
	}

	څرګند ویکټور(ثابت Type معلومات_2[M]) :
		ماتریکس_M1(معلومات_2)
	{
	}

	کينډۍ<size_t P, size_t Q>
	ویکټور(ثابت ټوټه_2<Type, M, 1, P, Q> &ټوټه_دننه) :
		ماتریکس_2<Type, M, 1>(ټوټه_دننه)
	{
	}

	کينډۍ<size_t P, size_t Q, size_t DUMMY = 1>
	ویکټور(ثابت ټوټه_2<Type, 1, M, P, Q> &ټوټه_دننه)
	{
		ویکټور &self(*هذا);

		لكل (size_t i = 0; i < M; i++) {
			self(i) = ټوټه_دننه(0, i);
		}
	}

	کينډۍ<size_t P, size_t Q>
	ویکټور(ثابت Const_ټوټه<Type, M, 1, P, Q> &ټوټه_دننه) :
		ماتریکس_2<Type, M, 1>(ټوټه_دننه)
	{
	}

	کينډۍ<size_t P, size_t Q, size_t DUMMY = 1>
	ویکټور(ثابت Const_ټوټه<Type, 1, M, P, Q> &ټوټه_دننه)
	{
		ویکټور &self(*هذا);

		لكل (size_t i = 0; i < M; i++) {
			self(i) = ټوټه_دننه(0, i);
		}
	}

	مضمن ثابت Type &چلوونکی()(size_t i) ثابت
	{
		أكد(i < M);

		ثابت ماتریکس_M1 &v = *هذا;
		أرجع v(i, 0);
	}

	مضمن Type &چلوونکی()(size_t i)
	{
		أكد(i < M);

		ماتریکس_M1 &v = *هذا;
		أرجع v(i, 0);
	}

	Type dot(ثابت ماتریکس_M1 &b) ثابت
	{
		ثابت ویکټور &a(*هذا);
		Type r(0);

		لكل (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		أرجع r;
	}

	مضمن Type چلوونکی*(ثابت ماتریکس_M1 &b) ثابت
	{
		ثابت ویکټور &a(*هذا);
		أرجع a.dot(b);
	}

	مضمن ویکټور چلوونکی*(Type b) ثابت
	{
		أرجع ویکټور(ماتریکس_M1::چلوونکی*(b));
	}

	Type norm() ثابت
	{
		ثابت ویکټور &a(*هذا);
		أرجع Type(std::جذر(a.dot(a)));
	}

	Type norm_squared() ثابت
	{
		ثابت ویکټور &a(*هذا);
		أرجع a.dot(a);
	}

	مضمن Type اوږدتیا() ثابت
	{
		أرجع norm();
	}

	مضمن فراغ normalize()
	{
		(*هذا) /= norm();
	}

	ویکټور unit() ثابت
	{
		أرجع (*هذا) / norm();
	}

	ویکټور unit_or_zero(ثابت Type eps = Type(1e-5)) ثابت
	{
		ثابت Type n = norm();

		إذا (n > eps) {
			أرجع (*هذا) / n;
		}

		أرجع ویکټور();
	}

	مضمن ویکټور normalized() ثابت
	{
		أرجع unit();
	}

	منطقي longerThan(Type testVal) ثابت
	{
		أرجع norm_squared() > testVal * testVal;
	}

	ویکټور جذر() ثابت
	{
		ثابت ویکټور &a(*هذا);
		ویکټور r;

		لكل (size_t i = 0; i < M; i++) {
			r(i) = Type(std::جذر(a(i)));
		}

		أرجع r;
	}

	فراغ print() ثابت
	{
		(*هذا).transpose().print();
	}

	ساكن size_t اندازه()
	{
		أرجع M;
	}
};

کينډۍ<ډول_نوم OStream, ډول_نوم Type, size_t M>
OStream &چلوونکی<<(OStream &os, ثابت ماتریکس::ویکټور<Type, M> &ویکټور_2)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	حرف string[n];
	ویکټور_2.transpose().write_string(string, n);
	os << string;
	أرجع os;
}

} // namespace matrix
