/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#अन्तर्भाव "Matrix.hpp"

नामाकाश matrix
{

template<typename Type, size_t M>
वर्ग Vector : सार्वजनिक Matrix<Type, M, 1>
{
सार्वजनिक:
	using MatrixM1 = Matrix<Type, M, 1>;

	Vector() = स्वभाव;

	Vector(ध्रुव MatrixM1 &other) :
		MatrixM1(other)
	{
	}

	explicit Vector(ध्रुव Type data_[M]) :
		MatrixM1(data_)
	{
	}

	template<size_t P, size_t Q>
	Vector(ध्रुव Slice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ध्रुव Slice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*एतत्);

		प्रति (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	template<size_t P, size_t Q>
	Vector(ध्रुव ConstSlice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(ध्रुव ConstSlice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*एतत्);

		प्रति (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	अन्तःस्थ ध्रुव Type &operator()(size_t i) ध्रुव
	{
		प्रतिज्ञा(i < M);

		ध्रुव MatrixM1 &v = *एतत्;
		प्रत्यागम v(i, 0);
	}

	अन्तःस्थ Type &operator()(size_t i)
	{
		प्रतिज्ञा(i < M);

		MatrixM1 &v = *एतत्;
		प्रत्यागम v(i, 0);
	}

	Type dot(ध्रुव MatrixM1 &b) ध्रुव
	{
		ध्रुव Vector &a(*एतत्);
		Type r(0);

		प्रति (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		प्रत्यागम r;
	}

	अन्तःस्थ Type operator*(ध्रुव MatrixM1 &b) ध्रुव
	{
		ध्रुव Vector &a(*एतत्);
		प्रत्यागम a.dot(b);
	}

	अन्तःस्थ Vector operator*(Type b) ध्रुव
	{
		प्रत्यागम Vector(MatrixM1::operator*(b));
	}

	Type norm() ध्रुव
	{
		ध्रुव Vector &a(*एतत्);
		प्रत्यागम Type(std::sqrt(a.dot(a)));
	}

	Type norm_squared() ध्रुव
	{
		ध्रुव Vector &a(*एतत्);
		प्रत्यागम a.dot(a);
	}

	अन्तःस्थ Type length() ध्रुव
	{
		प्रत्यागम norm();
	}

	अन्तःस्थ शून्य normalize()
	{
		(*एतत्) /= norm();
	}

	Vector unit() ध्रुव
	{
		प्रत्यागम (*एतत्) / norm();
	}

	Vector unit_or_zero(ध्रुव Type eps = Type(1e-5)) ध्रुव
	{
		ध्रुव Type n = norm();

		जइ (n > eps) {
			प्रत्यागम (*एतत्) / n;
		}

		प्रत्यागम Vector();
	}

	अन्तःस्थ Vector normalized() ध्रुव
	{
		प्रत्यागम unit();
	}

	तार्किक longerThan(Type testVal) ध्रुव
	{
		प्रत्यागम norm_squared() > testVal * testVal;
	}

	Vector sqrt() ध्रुव
	{
		ध्रुव Vector &a(*एतत्);
		Vector r;

		प्रति (size_t i = 0; i < M; i++) {
			r(i) = Type(std::sqrt(a(i)));
		}

		प्रत्यागम r;
	}

	शून्य print() ध्रुव
	{
		(*एतत्).transpose().print();
	}

	स्थिर size_t size()
	{
		प्रत्यागम M;
	}
};

template<typename OStream, typename Type, size_t M>
OStream &operator<<(OStream &os, ध्रुव matrix::Vector<Type, M> &vector)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	वर्ण string[n];
	vector.transpose().write_string(string, n);
	os << string;
	प्रत्यागम os;
}

} // namespace matrix
