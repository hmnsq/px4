/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ہدایت once

#شامل "Matrix.hpp"

نامخلا میٹرکس
{

سانچہ<قسم_نام Type, size_t M>
جماعت سمتیہ : عوامی میٹرکس_2<Type, M, 1>
{
عوامی:
	استعمال میٹرکس_M1 = میٹرکس_2<Type, M, 1>;

	سمتیہ() = طےشدہ;

	سمتیہ(ثابت میٹرکس_M1 &other) :
		میٹرکس_M1(other)
	{
	}

	صریح سمتیہ(ثابت Type ڈیٹا_2[M]) :
		میٹرکس_M1(ڈیٹا_2)
	{
	}

	سانچہ<size_t P, size_t Q>
	سمتیہ(ثابت قاش_2<Type, M, 1, P, Q> &قاش_اندر) :
		میٹرکس_2<Type, M, 1>(قاش_اندر)
	{
	}

	سانچہ<size_t P, size_t Q, size_t DUMMY = 1>
	سمتیہ(ثابت قاش_2<Type, 1, M, P, Q> &قاش_اندر)
	{
		سمتیہ &self(*یہ);

		برائے (size_t i = 0; i < M; i++) {
			self(i) = قاش_اندر(0, i);
		}
	}

	سانچہ<size_t P, size_t Q>
	سمتیہ(ثابت Const_قاش<Type, M, 1, P, Q> &قاش_اندر) :
		میٹرکس_2<Type, M, 1>(قاش_اندر)
	{
	}

	سانچہ<size_t P, size_t Q, size_t DUMMY = 1>
	سمتیہ(ثابت Const_قاش<Type, 1, M, P, Q> &قاش_اندر)
	{
		سمتیہ &self(*یہ);

		برائے (size_t i = 0; i < M; i++) {
			self(i) = قاش_اندر(0, i);
		}
	}

	اندرونی ثابت Type &عامل()(size_t i) ثابت
	{
		أكد(i < M);

		ثابت میٹرکس_M1 &v = *یہ;
		واپس v(i, 0);
	}

	اندرونی Type &عامل()(size_t i)
	{
		أكد(i < M);

		میٹرکس_M1 &v = *یہ;
		واپس v(i, 0);
	}

	Type dot(ثابت میٹرکس_M1 &b) ثابت
	{
		ثابت سمتیہ &a(*یہ);
		Type r(0);

		برائے (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		واپس r;
	}

	اندرونی Type عامل*(ثابت میٹرکس_M1 &b) ثابت
	{
		ثابت سمتیہ &a(*یہ);
		واپس a.dot(b);
	}

	اندرونی سمتیہ عامل*(Type b) ثابت
	{
		واپس سمتیہ(میٹرکس_M1::عامل*(b));
	}

	Type norm() ثابت
	{
		ثابت سمتیہ &a(*یہ);
		واپس Type(std::جذر(a.dot(a)));
	}

	Type norm_squared() ثابت
	{
		ثابت سمتیہ &a(*یہ);
		واپس a.dot(a);
	}

	اندرونی Type لمبائی_2() ثابت
	{
		واپس norm();
	}

	اندرونی خالی normalize()
	{
		(*یہ) /= norm();
	}

	سمتیہ unit() ثابت
	{
		واپس (*یہ) / norm();
	}

	سمتیہ unit_or_zero(ثابت Type eps = Type(1e-5)) ثابت
	{
		ثابت Type n = norm();

		اگر (n > eps) {
			واپس (*یہ) / n;
		}

		واپس سمتیہ();
	}

	اندرونی سمتیہ normalized() ثابت
	{
		واپس unit();
	}

	منطقی longerThan(Type testVal) ثابت
	{
		واپس norm_squared() > testVal * testVal;
	}

	سمتیہ جذر() ثابت
	{
		ثابت سمتیہ &a(*یہ);
		سمتیہ r;

		برائے (size_t i = 0; i < M; i++) {
			r(i) = Type(std::جذر(a(i)));
		}

		واپس r;
	}

	خالی print() ثابت
	{
		(*یہ).transpose().print();
	}

	ساکن size_t سائز()
	{
		واپس M;
	}
};

سانچہ<قسم_نام OStream, قسم_نام Type, size_t M>
OStream &عامل<<(OStream &os, ثابت میٹرکس::سمتیہ<Type, M> &سمتیہ_2)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	حرف string[n];
	سمتیہ_2.transpose().write_string(string, n);
	os << string;
	واپس os;
}

} // namespace matrix
