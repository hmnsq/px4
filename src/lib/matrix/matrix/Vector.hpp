/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#ಅನ್ತರ್ಭಾವ "Matrix.hpp"

ನಾಮಾಕಾಶ matrix
{

template<typename Type, size_t M>
ವರ್ಗ Vector : ಸಾರ್ವಜನಿಕ Matrix<Type, M, 1>
{
ಸಾರ್ವಜನಿಕ:
	using MatrixM1 = Matrix<Type, M, 1>;

	Vector() = ಸ್ವಭಾವ;

	Vector(ಧ್ರುವ MatrixM1 &other) :
		MatrixM1(other)
	{
	}

	explicit Vector(ಧ್ರುವ Type data_[M]) :
		MatrixM1(data_)
	{
	}

	template<size_t P, size_t Q>
	Vector(ಧ್ರುವ Slice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ಧ್ರುವ Slice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*ಏತತ್);

		ಪ್ರತಿ (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	template<size_t P, size_t Q>
	Vector(ಧ್ರುವ ConstSlice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ಧ್ರುವ ConstSlice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*ಏತತ್);

		ಪ್ರತಿ (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	ಅನ್ತಃಸ್ಥ ಧ್ರುವ Type &operator()(size_t i) ಧ್ರುವ
	{
		ಪ್ರತಿಜ್ಞಾ(i < M);

		ಧ್ರುವ MatrixM1 &v = *ಏತತ್;
		ಪ್ರತ್ಯಾಗಮ v(i, 0);
	}

	ಅನ್ತಃಸ್ಥ Type &operator()(size_t i)
	{
		ಪ್ರತಿಜ್ಞಾ(i < M);

		MatrixM1 &v = *ಏತತ್;
		ಪ್ರತ್ಯಾಗಮ v(i, 0);
	}

	Type dot(ಧ್ರುವ MatrixM1 &b) ಧ್ರುವ
	{
		ಧ್ರುವ Vector &a(*ಏತತ್);
		Type r(0);

		ಪ್ರತಿ (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		ಪ್ರತ್ಯಾಗಮ r;
	}

	ಅನ್ತಃಸ್ಥ Type operator*(ಧ್ರುವ MatrixM1 &b) ಧ್ರುವ
	{
		ಧ್ರುವ Vector &a(*ಏತತ್);
		ಪ್ರತ್ಯಾಗಮ a.dot(b);
	}

	ಅನ್ತಃಸ್ಥ Vector operator*(Type b) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ Vector(MatrixM1::operator*(b));
	}

	Type norm() ಧ್ರುವ
	{
		ಧ್ರುವ Vector &a(*ಏತತ್);
		ಪ್ರತ್ಯಾಗಮ Type(std::sqrt(a.dot(a)));
	}

	Type norm_squared() ಧ್ರುವ
	{
		ಧ್ರುವ Vector &a(*ಏತತ್);
		ಪ್ರತ್ಯಾಗಮ a.dot(a);
	}

	ಅನ್ತಃಸ್ಥ Type length() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ norm();
	}

	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ normalize()
	{
		(*ಏತತ್) /= norm();
	}

	Vector unit() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್) / norm();
	}

	Vector unit_or_zero(ಧ್ರುವ Type eps = Type(1e-5)) ಧ್ರುವ
	{
		ಧ್ರುವ Type n = norm();

		ಯದಿ (n > eps) {
			ಪ್ರತ್ಯಾಗಮ (*ಏತತ್) / n;
		}

		ಪ್ರತ್ಯಾಗಮ Vector();
	}

	ಅನ್ತಃಸ್ಥ Vector normalized() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ unit();
	}

	ತಾರ್ಕಿಕ longerThan(Type testVal) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ norm_squared() > testVal * testVal;
	}

	Vector sqrt() ಧ್ರುವ
	{
		ಧ್ರುವ Vector &a(*ಏತತ್);
		Vector r;

		ಪ್ರತಿ (size_t i = 0; i < M; i++) {
			r(i) = Type(std::sqrt(a(i)));
		}

		ಪ್ರತ್ಯಾಗಮ r;
	}

	ಶೂನ್ಯ print() ಧ್ರುವ
	{
		(*ಏತತ್).transpose().print();
	}

	ಸ್ಥಿರ size_t size()
	{
		ಪ್ರತ್ಯಾಗಮ M;
	}
};

template<typename OStream, typename Type, size_t M>
OStream &operator<<(OStream &os, ಧ್ರುವ matrix::Vector<Type, M> &vector)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	ವರ್ಣ string[n];
	vector.transpose().write_string(string, n);
	os << string;
	ಪ್ರತ್ಯಾಗಮ os;
}

} // namespace matrix
