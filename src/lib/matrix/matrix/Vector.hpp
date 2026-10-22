/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ہدایت once

#ضمن "Matrix.hpp"

نطاق میٹرکس
{

سانچا<قسم_ناں Type, size_t M>
صنف سمتیہ : عام میٹرکس_2<Type, M, 1>
{
عام:
	ورتوں میٹرکس_M1 = میٹرکس_2<Type, M, 1>;

	سمتیہ() = افتراضي;

	سمتیہ(ثابت میٹرکس_M1 &other) :
		میٹرکس_M1(other)
	{
	}

	واضح سمتیہ(ثابت Type انکڑے_2[M]) :
		میٹرکس_M1(انکڑے_2)
	{
	}

	سانچا<size_t P, size_t Q>
	سمتیہ(ثابت ٹکڑا_2<Type, M, 1, P, Q> &ٹکڑا_وچ) :
		میٹرکس_2<Type, M, 1>(ٹکڑا_وچ)
	{
	}

	سانچا<size_t P, size_t Q, size_t DUMMY = 1>
	سمتیہ(ثابت ٹکڑا_2<Type, 1, M, P, Q> &ٹکڑا_وچ)
	{
		سمتیہ &self(*هذا);

		لكل (size_t i = 0; i < M; i++) {
			self(i) = ٹکڑا_وچ(0, i);
		}
	}

	سانچا<size_t P, size_t Q>
	سمتیہ(ثابت Const_ٹکڑا<Type, M, 1, P, Q> &ٹکڑا_وچ) :
		میٹرکس_2<Type, M, 1>(ٹکڑا_وچ)
	{
	}

	سانچا<size_t P, size_t Q, size_t DUMMY = 1>
	سمتیہ(ثابت Const_ٹکڑا<Type, 1, M, P, Q> &ٹکڑا_وچ)
	{
		سمتیہ &self(*هذا);

		لكل (size_t i = 0; i < M; i++) {
			self(i) = ٹکڑا_وچ(0, i);
		}
	}

	مضمن ثابت Type &عامل()(size_t i) ثابت
	{
		أكد(i < M);

		ثابت میٹرکس_M1 &v = *هذا;
		أرجع v(i, 0);
	}

	مضمن Type &عامل()(size_t i)
	{
		أكد(i < M);

		میٹرکس_M1 &v = *هذا;
		أرجع v(i, 0);
	}

	Type dot(ثابت میٹرکس_M1 &b) ثابت
	{
		ثابت سمتیہ &a(*هذا);
		Type r(0);

		لكل (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		أرجع r;
	}

	مضمن Type عامل*(ثابت میٹرکس_M1 &b) ثابت
	{
		ثابت سمتیہ &a(*هذا);
		أرجع a.dot(b);
	}

	مضمن سمتیہ عامل*(Type b) ثابت
	{
		أرجع سمتیہ(میٹرکس_M1::عامل*(b));
	}

	Type norm() ثابت
	{
		ثابت سمتیہ &a(*هذا);
		أرجع Type(std::جذر(a.dot(a)));
	}

	Type norm_squared() ثابت
	{
		ثابت سمتیہ &a(*هذا);
		أرجع a.dot(a);
	}

	مضمن Type لمائی() ثابت
	{
		أرجع norm();
	}

	مضمن فراغ normalize()
	{
		(*هذا) /= norm();
	}

	سمتیہ unit() ثابت
	{
		أرجع (*هذا) / norm();
	}

	سمتیہ unit_or_zero(ثابت Type eps = Type(1e-5)) ثابت
	{
		ثابت Type n = norm();

		إذا (n > eps) {
			أرجع (*هذا) / n;
		}

		أرجع سمتیہ();
	}

	مضمن سمتیہ normalized() ثابت
	{
		أرجع unit();
	}

	منطقي longerThan(Type testVal) ثابت
	{
		أرجع norm_squared() > testVal * testVal;
	}

	سمتیہ جذر() ثابت
	{
		ثابت سمتیہ &a(*هذا);
		سمتیہ r;

		لكل (size_t i = 0; i < M; i++) {
			r(i) = Type(std::جذر(a(i)));
		}

		أرجع r;
	}

	فراغ print() ثابت
	{
		(*هذا).transpose().print();
	}

	ساكن size_t ناپ()
	{
		أرجع M;
	}
};

سانچا<قسم_ناں OStream, قسم_ناں Type, size_t M>
OStream &عامل<<(OStream &os, ثابت میٹرکس::سمتیہ<Type, M> &سمتیہ_2)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	حرف string[n];
	سمتیہ_2.transpose().write_string(string, n);
	os << string;
	أرجع os;
}

} // namespace matrix
