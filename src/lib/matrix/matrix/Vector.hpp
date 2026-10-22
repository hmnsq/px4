/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#هدايت once

#ضمن "Matrix.hpp"

نطاق ميٽرڪس
{

سانچو<قسم_نالو Type, size_t M>
صنف ويڪٽر : عام ميٽرڪس_2<Type, M, 1>
{
عام:
	استعمال ميٽرڪس_M1 = ميٽرڪس_2<Type, M, 1>;

	ويڪٽر() = افتراضي;

	ويڪٽر(ثابت ميٽرڪس_M1 &other) :
		ميٽرڪس_M1(other)
	{
	}

	واضح ويڪٽر(ثابت Type معلومات_2[M]) :
		ميٽرڪس_M1(معلومات_2)
	{
	}

	سانچو<size_t P, size_t Q>
	ويڪٽر(ثابت ٽڪرو_2<Type, M, 1, P, Q> &ٽڪرو_اندريون) :
		ميٽرڪس_2<Type, M, 1>(ٽڪرو_اندريون)
	{
	}

	سانچو<size_t P, size_t Q, size_t DUMMY = 1>
	ويڪٽر(ثابت ٽڪرو_2<Type, 1, M, P, Q> &ٽڪرو_اندريون)
	{
		ويڪٽر &self(*هذا);

		لكل (size_t i = 0; i < M; i++) {
			self(i) = ٽڪرو_اندريون(0, i);
		}
	}

	سانچو<size_t P, size_t Q>
	ويڪٽر(ثابت Const_ٽڪرو<Type, M, 1, P, Q> &ٽڪرو_اندريون) :
		ميٽرڪس_2<Type, M, 1>(ٽڪرو_اندريون)
	{
	}

	سانچو<size_t P, size_t Q, size_t DUMMY = 1>
	ويڪٽر(ثابت Const_ٽڪرو<Type, 1, M, P, Q> &ٽڪرو_اندريون)
	{
		ويڪٽر &self(*هذا);

		لكل (size_t i = 0; i < M; i++) {
			self(i) = ٽڪرو_اندريون(0, i);
		}
	}

	مضمن ثابت Type &عامل()(size_t i) ثابت
	{
		أكد(i < M);

		ثابت ميٽرڪس_M1 &v = *هذا;
		أرجع v(i, 0);
	}

	مضمن Type &عامل()(size_t i)
	{
		أكد(i < M);

		ميٽرڪس_M1 &v = *هذا;
		أرجع v(i, 0);
	}

	Type dot(ثابت ميٽرڪس_M1 &b) ثابت
	{
		ثابت ويڪٽر &a(*هذا);
		Type r(0);

		لكل (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		أرجع r;
	}

	مضمن Type عامل*(ثابت ميٽرڪس_M1 &b) ثابت
	{
		ثابت ويڪٽر &a(*هذا);
		أرجع a.dot(b);
	}

	مضمن ويڪٽر عامل*(Type b) ثابت
	{
		أرجع ويڪٽر(ميٽرڪس_M1::عامل*(b));
	}

	Type norm() ثابت
	{
		ثابت ويڪٽر &a(*هذا);
		أرجع Type(std::جذر(a.dot(a)));
	}

	Type norm_squared() ثابت
	{
		ثابت ويڪٽر &a(*هذا);
		أرجع a.dot(a);
	}

	مضمن Type ڊيگهائي() ثابت
	{
		أرجع norm();
	}

	مضمن فراغ normalize()
	{
		(*هذا) /= norm();
	}

	ويڪٽر unit() ثابت
	{
		أرجع (*هذا) / norm();
	}

	ويڪٽر unit_or_zero(ثابت Type eps = Type(1e-5)) ثابت
	{
		ثابت Type n = norm();

		إذا (n > eps) {
			أرجع (*هذا) / n;
		}

		أرجع ويڪٽر();
	}

	مضمن ويڪٽر normalized() ثابت
	{
		أرجع unit();
	}

	منطقي longerThan(Type testVal) ثابت
	{
		أرجع norm_squared() > testVal * testVal;
	}

	ويڪٽر جذر() ثابت
	{
		ثابت ويڪٽر &a(*هذا);
		ويڪٽر r;

		لكل (size_t i = 0; i < M; i++) {
			r(i) = Type(std::جذر(a(i)));
		}

		أرجع r;
	}

	فراغ print() ثابت
	{
		(*هذا).transpose().print();
	}

	ساكن size_t سائيز()
	{
		أرجع M;
	}
};

سانچو<قسم_نالو OStream, قسم_نالو Type, size_t M>
OStream &عامل<<(OStream &os, ثابت ميٽرڪس::ويڪٽر<Type, M> &ويڪٽر_2)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	حرف string[n];
	ويڪٽر_2.transpose().write_string(string, n);
	os << string;
	أرجع os;
}

} // namespace matrix
