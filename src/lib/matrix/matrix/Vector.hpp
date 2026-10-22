/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#അന്തര്ഭാവ "Matrix.hpp"

നാമാകാശ matrix
{

template<typename Type, size_t M>
വർഗ്ഗം Vector : പൊതു Matrix<Type, M, 1>
{
പൊതു:
	using MatrixM1 = Matrix<Type, M, 1>;

	Vector() = സ്ഥിരസ്ഥിതി;

	Vector(സ്ഥിരം MatrixM1 &other) :
		MatrixM1(other)
	{
	}

	explicit Vector(സ്ഥിരം Type data_[M]) :
		MatrixM1(data_)
	{
	}

	template<size_t P, size_t Q>
	Vector(സ്ഥിരം Slice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(സ്ഥിരം Slice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*ഇത്);

		വേണ്ടി (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	template<size_t P, size_t Q>
	Vector(സ്ഥിരം ConstSlice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(സ്ഥിരം ConstSlice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*ഇത്);

		വേണ്ടി (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	വരിയിൽ സ്ഥിരം Type &operator()(size_t i) സ്ഥിരം
	{
		പ്രതിജ്ഞാ(i < M);

		സ്ഥിരം MatrixM1 &v = *ഇത്;
		തിരികെ v(i, 0);
	}

	വരിയിൽ Type &operator()(size_t i)
	{
		പ്രതിജ്ഞാ(i < M);

		MatrixM1 &v = *ഇത്;
		തിരികെ v(i, 0);
	}

	Type dot(സ്ഥിരം MatrixM1 &b) സ്ഥിരം
	{
		സ്ഥിരം Vector &a(*ഇത്);
		Type r(0);

		വേണ്ടി (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		തിരികെ r;
	}

	വരിയിൽ Type operator*(സ്ഥിരം MatrixM1 &b) സ്ഥിരം
	{
		സ്ഥിരം Vector &a(*ഇത്);
		തിരികെ a.dot(b);
	}

	വരിയിൽ Vector operator*(Type b) സ്ഥിരം
	{
		തിരികെ Vector(MatrixM1::operator*(b));
	}

	Type norm() സ്ഥിരം
	{
		സ്ഥിരം Vector &a(*ഇത്);
		തിരികെ Type(std::sqrt(a.dot(a)));
	}

	Type norm_squared() സ്ഥിരം
	{
		സ്ഥിരം Vector &a(*ഇത്);
		തിരികെ a.dot(a);
	}

	വരിയിൽ Type length() സ്ഥിരം
	{
		തിരികെ norm();
	}

	വരിയിൽ ശൂന്യം normalize()
	{
		(*ഇത്) /= norm();
	}

	Vector unit() സ്ഥിരം
	{
		തിരികെ (*ഇത്) / norm();
	}

	Vector unit_or_zero(സ്ഥിരം Type eps = Type(1e-5)) സ്ഥിരം
	{
		സ്ഥിരം Type n = norm();

		എങ്കിൽ (n > eps) {
			തിരികെ (*ഇത്) / n;
		}

		തിരികെ Vector();
	}

	വരിയിൽ Vector normalized() സ്ഥിരം
	{
		തിരികെ unit();
	}

	തർക്കം longerThan(Type testVal) സ്ഥിരം
	{
		തിരികെ norm_squared() > testVal * testVal;
	}

	Vector sqrt() സ്ഥിരം
	{
		സ്ഥിരം Vector &a(*ഇത്);
		Vector r;

		വേണ്ടി (size_t i = 0; i < M; i++) {
			r(i) = Type(std::sqrt(a(i)));
		}

		തിരികെ r;
	}

	ശൂന്യം print() സ്ഥിരം
	{
		(*ഇത്).transpose().print();
	}

	സ്ഥിതം size_t size()
	{
		തിരികെ M;
	}
};

template<typename OStream, typename Type, size_t M>
OStream &operator<<(OStream &os, സ്ഥിരം matrix::Vector<Type, M> &vector)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	അക്ഷരം string[n];
	vector.transpose().write_string(string, n);
	os << string;
	തിരികെ os;
}

} // namespace matrix
