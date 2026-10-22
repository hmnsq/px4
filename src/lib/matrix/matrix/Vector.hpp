/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ضمن "Matrix.hpp"

نطاق matrix
{

template<typename Type, size_t M>
صنف Vector : عام Matrix<Type, M, 1>
{
عام:
	using MatrixM1 = Matrix<Type, M, 1>;

	Vector() = افتراضي;

	Vector(ثابت MatrixM1 &other) :
		MatrixM1(other)
	{
	}

	explicit Vector(ثابت Type data_[M]) :
		MatrixM1(data_)
	{
	}

	template<size_t P, size_t Q>
	Vector(ثابت Slice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ثابت Slice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*هذا);

		لكل (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	template<size_t P, size_t Q>
	Vector(ثابت ConstSlice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ثابت ConstSlice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*هذا);

		لكل (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	مضمن ثابت Type &operator()(size_t i) ثابت
	{
		أكد(i < M);

		ثابت MatrixM1 &v = *هذا;
		أرجع v(i, 0);
	}

	مضمن Type &operator()(size_t i)
	{
		أكد(i < M);

		MatrixM1 &v = *هذا;
		أرجع v(i, 0);
	}

	Type dot(ثابت MatrixM1 &b) ثابت
	{
		ثابت Vector &a(*هذا);
		Type r(0);

		لكل (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		أرجع r;
	}

	مضمن Type operator*(ثابت MatrixM1 &b) ثابت
	{
		ثابت Vector &a(*هذا);
		أرجع a.dot(b);
	}

	مضمن Vector operator*(Type b) ثابت
	{
		أرجع Vector(MatrixM1::operator*(b));
	}

	Type norm() ثابت
	{
		ثابت Vector &a(*هذا);
		أرجع Type(std::sqrt(a.dot(a)));
	}

	Type norm_squared() ثابت
	{
		ثابت Vector &a(*هذا);
		أرجع a.dot(a);
	}

	مضمن Type length() ثابت
	{
		أرجع norm();
	}

	مضمن فراغ normalize()
	{
		(*هذا) /= norm();
	}

	Vector unit() ثابت
	{
		أرجع (*هذا) / norm();
	}

	Vector unit_or_zero(ثابت Type eps = Type(1e-5)) ثابت
	{
		ثابت Type n = norm();

		𐤀𐤌 (n > eps) {
			أرجع (*هذا) / n;
		}

		أرجع Vector();
	}

	مضمن Vector normalized() ثابت
	{
		أرجع unit();
	}

	منطقي longerThan(Type testVal) ثابت
	{
		أرجع norm_squared() > testVal * testVal;
	}

	Vector sqrt() ثابت
	{
		ثابت Vector &a(*هذا);
		Vector r;

		لكل (size_t i = 0; i < M; i++) {
			r(i) = Type(std::sqrt(a(i)));
		}

		أرجع r;
	}

	فراغ print() ثابت
	{
		(*هذا).transpose().print();
	}

	ساكن size_t size()
	{
		أرجع M;
	}
};

template<typename OStream, typename Type, size_t M>
OStream &operator<<(OStream &os, ثابت matrix::Vector<Type, M> &vector)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	حرف string[n];
	vector.transpose().write_string(string, n);
	os << string;
	أرجع os;
}

} // namespace matrix
