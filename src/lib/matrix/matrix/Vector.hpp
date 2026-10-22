/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "Matrix.hpp"

ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ
{

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ Type, size_t M>
ਵਰਗ ਸਦਿਸ਼ : ਜਨਤਕ ਮੈਟ੍ਰਿਕਸ_2<Type, M, 1>
{
ਜਨਤਕ:
	ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ_M1 = ਮੈਟ੍ਰਿਕਸ_2<Type, M, 1>;

	ਸਦਿਸ਼() = ਮੂਲ;

	ਸਦਿਸ਼(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_M1 &other) :
		ਮੈਟ੍ਰਿਕਸ_M1(other)
	{
	}

	ਸਪਸ਼ਟ ਸਦਿਸ਼(ਅਚਲ Type ਅੰਕੜੇ_2[M]) :
		ਮੈਟ੍ਰਿਕਸ_M1(ਅੰਕੜੇ_2)
	{
	}

	ਸਾਂਚਾ<size_t P, size_t Q>
	ਸਦਿਸ਼(ਅਚਲ ਟੁਕੜਾ_2<Type, M, 1, P, Q> &ਟੁਕੜਾ_ਵਿੱਚ) :
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, 1>(ਟੁਕੜਾ_ਵਿੱਚ)
	{
	}

	ਸਾਂਚਾ<size_t P, size_t Q, size_t DUMMY = 1>
	ਸਦਿਸ਼(ਅਚਲ ਟੁਕੜਾ_2<Type, 1, M, P, Q> &ਟੁਕੜਾ_ਵਿੱਚ)
	{
		ਸਦਿਸ਼ &self(*ਇਹ);

		ਲਈ (size_t i = 0; i < M; i++) {
			self(i) = ਟੁਕੜਾ_ਵਿੱਚ(0, i);
		}
	}

	ਸਾਂਚਾ<size_t P, size_t Q>
	ਸਦਿਸ਼(ਅਚਲ Const_ਟੁਕੜਾ<Type, M, 1, P, Q> &ਟੁਕੜਾ_ਵਿੱਚ) :
		ਮੈਟ੍ਰਿਕਸ_2<Type, M, 1>(ਟੁਕੜਾ_ਵਿੱਚ)
	{
	}

	ਸਾਂਚਾ<size_t P, size_t Q, size_t DUMMY = 1>
	ਸਦਿਸ਼(ਅਚਲ Const_ਟੁਕੜਾ<Type, 1, M, P, Q> &ਟੁਕੜਾ_ਵਿੱਚ)
	{
		ਸਦਿਸ਼ &self(*ਇਹ);

		ਲਈ (size_t i = 0; i < M; i++) {
			self(i) = ਟੁਕੜਾ_ਵਿੱਚ(0, i);
		}
	}

	ਸਤਰਅੰਦਰ ਅਚਲ Type &ਸੰਚਾਲਕ()(size_t i) ਅਚਲ
	{
		ਪ੍ਰਤਿਜ੍ਞਾ(i < M);

		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_M1 &v = *ਇਹ;
		ਵਾਪਸ v(i, 0);
	}

	ਸਤਰਅੰਦਰ Type &ਸੰਚਾਲਕ()(size_t i)
	{
		ਪ੍ਰਤਿਜ੍ਞਾ(i < M);

		ਮੈਟ੍ਰਿਕਸ_M1 &v = *ਇਹ;
		ਵਾਪਸ v(i, 0);
	}

	Type dot(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_M1 &b) ਅਚਲ
	{
		ਅਚਲ ਸਦਿਸ਼ &a(*ਇਹ);
		Type r(0);

		ਲਈ (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		ਵਾਪਸ r;
	}

	ਸਤਰਅੰਦਰ Type ਸੰਚਾਲਕ*(ਅਚਲ ਮੈਟ੍ਰਿਕਸ_M1 &b) ਅਚਲ
	{
		ਅਚਲ ਸਦਿਸ਼ &a(*ਇਹ);
		ਵਾਪਸ a.dot(b);
	}

	ਸਤਰਅੰਦਰ ਸਦਿਸ਼ ਸੰਚਾਲਕ*(Type b) ਅਚਲ
	{
		ਵਾਪਸ ਸਦਿਸ਼(ਮੈਟ੍ਰਿਕਸ_M1::ਸੰਚਾਲਕ*(b));
	}

	Type norm() ਅਚਲ
	{
		ਅਚਲ ਸਦਿਸ਼ &a(*ਇਹ);
		ਵਾਪਸ Type(std::ਵਰਗਮੂਲ(a.dot(a)));
	}

	Type norm_squared() ਅਚਲ
	{
		ਅਚਲ ਸਦਿਸ਼ &a(*ਇਹ);
		ਵਾਪਸ a.dot(a);
	}

	ਸਤਰਅੰਦਰ Type ਲੰਮਾਈ() ਅਚਲ
	{
		ਵਾਪਸ norm();
	}

	ਸਤਰਅੰਦਰ ਖਾਲੀ normalize()
	{
		(*ਇਹ) /= norm();
	}

	ਸਦਿਸ਼ unit() ਅਚਲ
	{
		ਵਾਪਸ (*ਇਹ) / norm();
	}

	ਸਦਿਸ਼ unit_or_zero(ਅਚਲ Type eps = Type(1e-5)) ਅਚਲ
	{
		ਅਚਲ Type n = norm();

		ਜੇ (n > eps) {
			ਵਾਪਸ (*ਇਹ) / n;
		}

		ਵਾਪਸ ਸਦਿਸ਼();
	}

	ਸਤਰਅੰਦਰ ਸਦਿਸ਼ normalized() ਅਚਲ
	{
		ਵਾਪਸ unit();
	}

	ਤਾਰਕਿਕ longerThan(Type testVal) ਅਚਲ
	{
		ਵਾਪਸ norm_squared() > testVal * testVal;
	}

	ਸਦਿਸ਼ ਵਰਗਮੂਲ() ਅਚਲ
	{
		ਅਚਲ ਸਦਿਸ਼ &a(*ਇਹ);
		ਸਦਿਸ਼ r;

		ਲਈ (size_t i = 0; i < M; i++) {
			r(i) = Type(std::ਵਰਗਮੂਲ(a(i)));
		}

		ਵਾਪਸ r;
	}

	ਖਾਲੀ print() ਅਚਲ
	{
		(*ਇਹ).transpose().print();
	}

	ਸਥਿਰ size_t ਮਾਪ()
	{
		ਵਾਪਸ M;
	}
};

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ OStream, ਕਿਸਮ_ਨਾਂ Type, size_t M>
OStream &ਸੰਚਾਲਕ<<(OStream &os, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<Type, M> &ਸਦਿਸ਼_2)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	ਅੱਖਰ string[n];
	ਸਦਿਸ਼_2.transpose().write_string(string, n);
	os << string;
	ਵਾਪਸ os;
}

} // namespace matrix
