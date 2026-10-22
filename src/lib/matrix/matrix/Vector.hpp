/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#شامل "Matrix.hpp"

فضاینام matrix
{

template<typename Type, size_t M>
رده Vector : عمومی Matrix<Type, M, 1>
{
عمومی:
	using MatrixM1 = Matrix<Type, M, 1>;

	Vector() = پیشفرض;

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
		Vector &self(*این);

		برای (size_t i = 0; i < M; i++) {
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
		Vector &self(*این);

		برای (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	درونخطی ثابت Type &operator()(size_t i) ثابت
	{
		أكد(i < M);

		ثابت MatrixM1 &v = *این;
		بازگشت v(i, 0);
	}

	درونخطی Type &operator()(size_t i)
	{
		أكد(i < M);

		MatrixM1 &v = *این;
		بازگشت v(i, 0);
	}

	Type dot(ثابت MatrixM1 &b) ثابت
	{
		ثابت Vector &a(*این);
		Type r(0);

		برای (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		بازگشت r;
	}

	درونخطی Type operator*(ثابت MatrixM1 &b) ثابت
	{
		ثابت Vector &a(*این);
		بازگشت a.dot(b);
	}

	درونخطی Vector operator*(Type b) ثابت
	{
		بازگشت Vector(MatrixM1::operator*(b));
	}

	Type norm() ثابت
	{
		ثابت Vector &a(*این);
		بازگشت Type(std::sqrt(a.dot(a)));
	}

	Type norm_squared() ثابت
	{
		ثابت Vector &a(*این);
		بازگشت a.dot(a);
	}

	درونخطی Type length() ثابت
	{
		بازگشت norm();
	}

	درونخطی تهی normalize()
	{
		(*این) /= norm();
	}

	Vector unit() ثابت
	{
		بازگشت (*این) / norm();
	}

	Vector unit_or_zero(ثابت Type eps = Type(1e-5)) ثابت
	{
		ثابت Type n = norm();

		اگر (n > eps) {
			بازگشت (*این) / n;
		}

		بازگشت Vector();
	}

	درونخطی Vector normalized() ثابت
	{
		بازگشت unit();
	}

	منطقی longerThan(Type testVal) ثابت
	{
		بازگشت norm_squared() > testVal * testVal;
	}

	Vector sqrt() ثابت
	{
		ثابت Vector &a(*این);
		Vector r;

		برای (size_t i = 0; i < M; i++) {
			r(i) = Type(std::sqrt(a(i)));
		}

		بازگشت r;
	}

	تهی print() ثابت
	{
		(*این).transpose().print();
	}

	ایستا size_t size()
	{
		بازگشت M;
	}
};

template<typename OStream, typename Type, size_t M>
OStream &operator<<(OStream &os, ثابت matrix::Vector<Type, M> &vector)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	نویسه string[n];
	vector.transpose().write_string(string, n);
	os << string;
	بازگشت os;
}

} // namespace matrix
