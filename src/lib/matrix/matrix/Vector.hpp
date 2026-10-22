/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#অন্তর্ভাব "Matrix.hpp"

নামাকাশ matrix
{

template<typename Type, size_t M>
বর্গ Vector : সার্বজনিক Matrix<Type, M, 1>
{
সার্বজনিক:
	using MatrixM1 = Matrix<Type, M, 1>;

	Vector() = স্বভাব;

	Vector(ধ্রুব MatrixM1 &other) :
		MatrixM1(other)
	{
	}

	explicit Vector(ধ্রুব Type data_[M]) :
		MatrixM1(data_)
	{
	}

	template<size_t P, size_t Q>
	Vector(ধ্রুব Slice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ধ্রুব Slice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*এতত্);

		প্রতি (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	template<size_t P, size_t Q>
	Vector(ধ্রুব ConstSlice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ধ্রুব ConstSlice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*এতত্);

		প্রতি (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	অন্তঃস্থ ধ্রুব Type &operator()(size_t i) ধ্রুব
	{
		প্রতিজ্ঞা(i < M);

		ধ্রুব MatrixM1 &v = *এতত্;
		প্রত্যাগম v(i, 0);
	}

	অন্তঃস্থ Type &operator()(size_t i)
	{
		প্রতিজ্ঞা(i < M);

		MatrixM1 &v = *এতত্;
		প্রত্যাগম v(i, 0);
	}

	Type dot(ধ্রুব MatrixM1 &b) ধ্রুব
	{
		ধ্রুব Vector &a(*এতত্);
		Type r(0);

		প্রতি (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		প্রত্যাগম r;
	}

	অন্তঃস্থ Type operator*(ধ্রুব MatrixM1 &b) ধ্রুব
	{
		ধ্রুব Vector &a(*এতত্);
		প্রত্যাগম a.dot(b);
	}

	অন্তঃস্থ Vector operator*(Type b) ধ্রুব
	{
		প্রত্যাগম Vector(MatrixM1::operator*(b));
	}

	Type norm() ধ্রুব
	{
		ধ্রুব Vector &a(*এতত্);
		প্রত্যাগম Type(std::sqrt(a.dot(a)));
	}

	Type norm_squared() ধ্রুব
	{
		ধ্রুব Vector &a(*এতত্);
		প্রত্যাগম a.dot(a);
	}

	অন্তঃস্থ Type length() ধ্রুব
	{
		প্রত্যাগম norm();
	}

	অন্তঃস্থ শূন্য normalize()
	{
		(*এতত্) /= norm();
	}

	Vector unit() ধ্রুব
	{
		প্রত্যাগম (*এতত্) / norm();
	}

	Vector unit_or_zero(ধ্রুব Type eps = Type(1e-5)) ধ্রুব
	{
		ধ্রুব Type n = norm();

		যদি (n > eps) {
			প্রত্যাগম (*এতত্) / n;
		}

		প্রত্যাগম Vector();
	}

	অন্তঃস্থ Vector normalized() ধ্রুব
	{
		প্রত্যাগম unit();
	}

	তার্কিক longerThan(Type testVal) ধ্রুব
	{
		প্রত্যাগম norm_squared() > testVal * testVal;
	}

	Vector sqrt() ধ্রুব
	{
		ধ্রুব Vector &a(*এতত্);
		Vector r;

		প্রতি (size_t i = 0; i < M; i++) {
			r(i) = Type(std::sqrt(a(i)));
		}

		প্রত্যাগম r;
	}

	শূন্য print() ধ্রুব
	{
		(*এতত্).transpose().print();
	}

	স্থির size_t size()
	{
		প্রত্যাগম M;
	}
};

template<typename OStream, typename Type, size_t M>
OStream &operator<<(OStream &os, ধ্রুব matrix::Vector<Type, M> &vector)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	বর্ণ string[n];
	vector.transpose().write_string(string, n);
	os << string;
	প্রত্যাগম os;
}

} // namespace matrix
