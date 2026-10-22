/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#رهنمود once

#ضمن "Matrix.hpp"

نطاق ماتریس
{

قالب<نام_نوع Type, size_t M>
صنف وکتور : عام ماتریس_2<Type, M, 1>
{
عام:
	استفاده ماتریس_M1 = ماتریس_2<Type, M, 1>;

	وکتور() = افتراضي;

	وکتور(ثابت ماتریس_M1 &other) :
		ماتریس_M1(other)
	{
	}

	صریح وکتور(ثابت Type داده_2[M]) :
		ماتریس_M1(داده_2)
	{
	}

	قالب<size_t P, size_t Q>
	وکتور(ثابت برش_2<Type, M, 1, P, Q> &برش_درون) :
		ماتریس_2<Type, M, 1>(برش_درون)
	{
	}

	قالب<size_t P, size_t Q, size_t DUMMY = 1>
	وکتور(ثابت برش_2<Type, 1, M, P, Q> &برش_درون)
	{
		وکتور &self(*هذا);

		لكل (size_t i = 0; i < M; i++) {
			self(i) = برش_درون(0, i);
		}
	}

	قالب<size_t P, size_t Q>
	وکتور(ثابت Const_برش<Type, M, 1, P, Q> &برش_درون) :
		ماتریس_2<Type, M, 1>(برش_درون)
	{
	}

	قالب<size_t P, size_t Q, size_t DUMMY = 1>
	وکتور(ثابت Const_برش<Type, 1, M, P, Q> &برش_درون)
	{
		وکتور &self(*هذا);

		لكل (size_t i = 0; i < M; i++) {
			self(i) = برش_درون(0, i);
		}
	}

	مضمن ثابت Type &عملگر()(size_t i) ثابت
	{
		أكد(i < M);

		ثابت ماتریس_M1 &v = *هذا;
		أرجع v(i, 0);
	}

	مضمن Type &عملگر()(size_t i)
	{
		أكد(i < M);

		ماتریس_M1 &v = *هذا;
		أرجع v(i, 0);
	}

	Type dot(ثابت ماتریس_M1 &b) ثابت
	{
		ثابت وکتور &a(*هذا);
		Type r(0);

		لكل (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		أرجع r;
	}

	مضمن Type عملگر*(ثابت ماتریس_M1 &b) ثابت
	{
		ثابت وکتور &a(*هذا);
		أرجع a.dot(b);
	}

	مضمن وکتور عملگر*(Type b) ثابت
	{
		أرجع وکتور(ماتریس_M1::عملگر*(b));
	}

	Type norm() ثابت
	{
		ثابت وکتور &a(*هذا);
		أرجع Type(std::جذر(a.dot(a)));
	}

	Type norm_squared() ثابت
	{
		ثابت وکتور &a(*هذا);
		أرجع a.dot(a);
	}

	مضمن Type درازی() ثابت
	{
		أرجع norm();
	}

	مضمن فراغ normalize()
	{
		(*هذا) /= norm();
	}

	وکتور unit() ثابت
	{
		أرجع (*هذا) / norm();
	}

	وکتور unit_or_zero(ثابت Type eps = Type(1e-5)) ثابت
	{
		ثابت Type n = norm();

		إذا (n > eps) {
			أرجع (*هذا) / n;
		}

		أرجع وکتور();
	}

	مضمن وکتور normalized() ثابت
	{
		أرجع unit();
	}

	منطقي longerThan(Type testVal) ثابت
	{
		أرجع norm_squared() > testVal * testVal;
	}

	وکتور جذر() ثابت
	{
		ثابت وکتور &a(*هذا);
		وکتور r;

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

قالب<نام_نوع OStream, نام_نوع Type, size_t M>
OStream &عملگر<<(OStream &os, ثابت ماتریس::وکتور<Type, M> &وکتور_2)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	حرف string[n];
	وکتور_2.transpose().write_string(string, n);
	os << string;
	أرجع os;
}

} // namespace matrix
