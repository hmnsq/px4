/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ "Matrix.hpp"

ନାମାକାଶ matrix
{

template<typename Type, size_t M>
ଶ୍ରେଣୀ Vector : ସର୍ବସାଧାରଣ Matrix<Type, M, 1>
{
ସର୍ବସାଧାରଣ:
	using MatrixM1 = Matrix<Type, M, 1>;

	Vector() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	Vector(ଅଚଳ MatrixM1 &other) :
		MatrixM1(other)
	{
	}

	explicit Vector(ଅଚଳ Type data_[M]) :
		MatrixM1(data_)
	{
	}

	template<size_t P, size_t Q>
	Vector(ଅଚଳ Slice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ଅଚଳ Slice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*ଏହି);

		ପାଇଁ (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	template<size_t P, size_t Q>
	Vector(ଅଚଳ ConstSlice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ଅଚଳ ConstSlice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*ଏହି);

		ପାଇଁ (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	ଧାଡ଼ିଭିତରେ ଅଚଳ Type &operator()(size_t i) ଅଚଳ
	{
		ପ୍ରତିଜ୍ଞା(i < M);

		ଅଚଳ MatrixM1 &v = *ଏହି;
		ଫେରନ୍ତୁ v(i, 0);
	}

	ଧାଡ଼ିଭିତରେ Type &operator()(size_t i)
	{
		ପ୍ରତିଜ୍ଞା(i < M);

		MatrixM1 &v = *ଏହି;
		ଫେରନ୍ତୁ v(i, 0);
	}

	Type dot(ଅଚଳ MatrixM1 &b) ଅଚଳ
	{
		ଅଚଳ Vector &a(*ଏହି);
		Type r(0);

		ପାଇଁ (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		ଫେରନ୍ତୁ r;
	}

	ଧାଡ଼ିଭିତରେ Type operator*(ଅଚଳ MatrixM1 &b) ଅଚଳ
	{
		ଅଚଳ Vector &a(*ଏହି);
		ଫେରନ୍ତୁ a.dot(b);
	}

	ଧାଡ଼ିଭିତରେ Vector operator*(Type b) ଅଚଳ
	{
		ଫେରନ୍ତୁ Vector(MatrixM1::operator*(b));
	}

	Type norm() ଅଚଳ
	{
		ଅଚଳ Vector &a(*ଏହି);
		ଫେରନ୍ତୁ Type(std::sqrt(a.dot(a)));
	}

	Type norm_squared() ଅଚଳ
	{
		ଅଚଳ Vector &a(*ଏହି);
		ଫେରନ୍ତୁ a.dot(a);
	}

	ଧାଡ଼ିଭିତରେ Type length() ଅଚଳ
	{
		ଫେରନ୍ତୁ norm();
	}

	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ normalize()
	{
		(*ଏହି) /= norm();
	}

	Vector unit() ଅଚଳ
	{
		ଫେରନ୍ତୁ (*ଏହି) / norm();
	}

	Vector unit_or_zero(ଅଚଳ Type eps = Type(1e-5)) ଅଚଳ
	{
		ଅଚଳ Type n = norm();

		ଯଦି (n > eps) {
			ଫେରନ୍ତୁ (*ଏହି) / n;
		}

		ଫେରନ୍ତୁ Vector();
	}

	ଧାଡ଼ିଭିତରେ Vector normalized() ଅଚଳ
	{
		ଫେରନ୍ତୁ unit();
	}

	ତାର୍କିକ longerThan(Type testVal) ଅଚଳ
	{
		ଫେରନ୍ତୁ norm_squared() > testVal * testVal;
	}

	Vector sqrt() ଅଚଳ
	{
		ଅଚଳ Vector &a(*ଏହି);
		Vector r;

		ପାଇଁ (size_t i = 0; i < M; i++) {
			r(i) = Type(std::sqrt(a(i)));
		}

		ଫେରନ୍ତୁ r;
	}

	ଶୂନ୍ୟ print() ଅଚଳ
	{
		(*ଏହି).transpose().print();
	}

	ସ୍ଥିର size_t size()
	{
		ଫେରନ୍ତୁ M;
	}
};

template<typename OStream, typename Type, size_t M>
OStream &operator<<(OStream &os, ଅଚଳ matrix::Vector<Type, M> &vector)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	ଅକ୍ଷର string[n];
	vector.transpose().write_string(string, n);
	os << string;
	ଫେରନ୍ତୁ os;
}

} // namespace matrix
