/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#অন্তর্ভাৱ "Matrix.hpp"

নামাকাশ matrix
{

template<typename Type, size_t M>
শ্ৰেণী Vector : সার্ৱজনিক Matrix<Type, M, 1>
{
সার্ৱজনিক:
	using MatrixM1 = Matrix<Type, M, 1>;

	Vector() = স্ৱভাৱ;

	Vector(ধ্রুৱ MatrixM1 &other) :
		MatrixM1(other)
	{
	}

	explicit Vector(ধ্রুৱ Type data_[M]) :
		MatrixM1(data_)
	{
	}

	template<size_t P, size_t Q>
	Vector(ধ্রুৱ Slice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ধ্রুৱ Slice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*এতত্);

		বাবে (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	template<size_t P, size_t Q>
	Vector(ধ্রুৱ ConstSlice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ধ্রুৱ ConstSlice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*এতত্);

		বাবে (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	অন্তঃস্থ ধ্রুৱ Type &operator()(size_t i) ধ্রুৱ
	{
		প্রতিজ্ঞা(i < M);

		ধ্রুৱ MatrixM1 &v = *এতত্;
		ঘূৰাই v(i, 0);
	}

	অন্তঃস্থ Type &operator()(size_t i)
	{
		প্রতিজ্ঞা(i < M);

		MatrixM1 &v = *এতত্;
		ঘূৰাই v(i, 0);
	}

	Type dot(ধ্রুৱ MatrixM1 &b) ধ্রুৱ
	{
		ধ্রুৱ Vector &a(*এতত্);
		Type r(0);

		বাবে (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		ঘূৰাই r;
	}

	অন্তঃস্থ Type operator*(ধ্রুৱ MatrixM1 &b) ধ্রুৱ
	{
		ধ্রুৱ Vector &a(*এতত্);
		ঘূৰাই a.dot(b);
	}

	অন্তঃস্থ Vector operator*(Type b) ধ্রুৱ
	{
		ঘূৰাই Vector(MatrixM1::operator*(b));
	}

	Type norm() ধ্রুৱ
	{
		ধ্রুৱ Vector &a(*এতত্);
		ঘূৰাই Type(std::sqrt(a.dot(a)));
	}

	Type norm_squared() ধ্রুৱ
	{
		ধ্রুৱ Vector &a(*এতত্);
		ঘূৰাই a.dot(a);
	}

	অন্তঃস্থ Type length() ধ্রুৱ
	{
		ঘূৰাই norm();
	}

	অন্তঃস্থ শূন্য normalize()
	{
		(*এতত্) /= norm();
	}

	Vector unit() ধ্রুৱ
	{
		ঘূৰাই (*এতত্) / norm();
	}

	Vector unit_or_zero(ধ্রুৱ Type eps = Type(1e-5)) ধ্রুৱ
	{
		ধ্রুৱ Type n = norm();

		যদি (n > eps) {
			ঘূৰাই (*এতত্) / n;
		}

		ঘূৰাই Vector();
	}

	অন্তঃস্থ Vector normalized() ধ্রুৱ
	{
		ঘূৰাই unit();
	}

	তার্কিক longerThan(Type testVal) ধ্রুৱ
	{
		ঘূৰাই norm_squared() > testVal * testVal;
	}

	Vector sqrt() ধ্রুৱ
	{
		ধ্রুৱ Vector &a(*এতত্);
		Vector r;

		বাবে (size_t i = 0; i < M; i++) {
			r(i) = Type(std::sqrt(a(i)));
		}

		ঘূৰাই r;
	}

	শূন্য print() ধ্রুৱ
	{
		(*এতত্).transpose().print();
	}

	স্থির size_t size()
	{
		ঘূৰাই M;
	}
};

template<typename OStream, typename Type, size_t M>
OStream &operator<<(OStream &os, ধ্রুৱ matrix::Vector<Type, M> &vector)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	ৱর্ণ string[n];
	vector.transpose().write_string(string, n);
	os << string;
	ঘূৰাই os;
}

} // namespace matrix
