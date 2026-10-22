/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#include "Matrix.hpp"

namespace matrix
{

template<typename Type, size_t M>
class Vector : public Matrix<Type, M, 1>
{
public:
	using MatrixM1 = Matrix<Type, M, 1>;

	Vector() = default;

	Vector(const MatrixM1 &other) :
		MatrixM1(other)
	{
	}

	explicit Vector(const Type data_[M]) :
		MatrixM1(data_)
	{
	}

	template<size_t P, size_t Q>
	Vector(const Slice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(const Slice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*this);

		за (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	template<size_t P, size_t Q>
	Vector(const ConstSlice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(const ConstSlice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*this);

		за (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	inline const Type &operator()(size_t i) const
	{
		assert(i < M);

		const MatrixM1 &v = *this;
		върни v(i, 0);
	}

	inline Type &operator()(size_t i)
	{
		assert(i < M);

		MatrixM1 &v = *this;
		върни v(i, 0);
	}

	Type dot(const MatrixM1 &b) const
	{
		const Vector &a(*this);
		Type r(0);

		за (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		върни r;
	}

	inline Type operator*(const MatrixM1 &b) const
	{
		const Vector &a(*this);
		върни a.dot(b);
	}

	inline Vector operator*(Type b) const
	{
		върни Vector(MatrixM1::operator*(b));
	}

	Type norm() const
	{
		const Vector &a(*this);
		върни Type(std::sqrt(a.dot(a)));
	}

	Type norm_squared() const
	{
		const Vector &a(*this);
		върни a.dot(a);
	}

	inline Type length() const
	{
		върни norm();
	}

	inline void normalize()
	{
		(*this) /= norm();
	}

	Vector unit() const
	{
		върни (*this) / norm();
	}

	Vector unit_or_zero(const Type eps = Type(1e-5)) const
	{
		const Type n = norm();

		ако (n > eps) {
			върни (*this) / n;
		}

		върни Vector();
	}

	inline Vector normalized() const
	{
		върни unit();
	}

	bool longerThan(Type testVal) const
	{
		върни norm_squared() > testVal * testVal;
	}

	Vector sqrt() const
	{
		const Vector &a(*this);
		Vector r;

		за (size_t i = 0; i < M; i++) {
			r(i) = Type(std::sqrt(a(i)));
		}

		върни r;
	}

	void print() const
	{
		(*this).transpose().print();
	}

	static size_t size()
	{
		върни M;
	}
};

template<typename OStream, typename Type, size_t M>
OStream &operator<<(OStream &os, const matrix::Vector<Type, M> &vector)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	char string[n];
	vector.transpose().write_string(string, n);
	os << string;
	върни os;
}

} // namespace matrix
