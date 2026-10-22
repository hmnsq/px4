/**
 * @file Vector.hpp
 *
 * Vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "Matrix.hpp"

ನಾಮಾಕಾಶ ಮಾತೃಕೆ
{

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು Type, size_t M>
ವರ್ಗ ಸದಿಶ : ಸಾರ್ವಜನಿಕ ಮಾತೃಕೆ_2<Type, M, 1>
{
ಸಾರ್ವಜನಿಕ:
	ಬಳಸಿ ಮಾತೃಕೆ_M1 = ಮಾತೃಕೆ_2<Type, M, 1>;

	ಸದಿಶ() = ಪೂರ್ವನಿಯೋಜಿತ;

	ಸದಿಶ(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_M1 &other) :
		ಮಾತೃಕೆ_M1(other)
	{
	}

	ಸ್ಪಷ್ಟ ಸದಿಶ(ಸ್ಥಿರಾಂಕ Type ದತ್ತಾಂಶ_2[M]) :
		ಮಾತೃಕೆ_M1(ದತ್ತಾಂಶ_2)
	{
	}

	ಮಾದರಿ<size_t P, size_t Q>
	ಸದಿಶ(ಸ್ಥಿರಾಂಕ ತುಂಡು_2<Type, M, 1, P, Q> &ತುಂಡು_ಒಳ) :
		ಮಾತೃಕೆ_2<Type, M, 1>(ತುಂಡು_ಒಳ)
	{
	}

	ಮಾದರಿ<size_t P, size_t Q, size_t DUMMY = 1>
	ಸದಿಶ(ಸ್ಥಿರಾಂಕ ತುಂಡು_2<Type, 1, M, P, Q> &ತುಂಡು_ಒಳ)
	{
		ಸದಿಶ &self(*ಇದು);

		ಗಾಗಿ (size_t i = 0; i < M; i++) {
			self(i) = ತುಂಡು_ಒಳ(0, i);
		}
	}

	ಮಾದರಿ<size_t P, size_t Q>
	ಸದಿಶ(ಸ್ಥಿರಾಂಕ Const_ತುಂಡು<Type, M, 1, P, Q> &ತುಂಡು_ಒಳ) :
		ಮಾತೃಕೆ_2<Type, M, 1>(ತುಂಡು_ಒಳ)
	{
	}

	ಮಾದರಿ<size_t P, size_t Q, size_t DUMMY = 1>
	ಸದಿಶ(ಸ್ಥಿರಾಂಕ Const_ತುಂಡು<Type, 1, M, P, Q> &ತುಂಡು_ಒಳ)
	{
		ಸದಿಶ &self(*ಇದು);

		ಗಾಗಿ (size_t i = 0; i < M; i++) {
			self(i) = ತುಂಡು_ಒಳ(0, i);
		}
	}

	ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ Type &ಕ್ರಿಯಾಕಾರಕ()(size_t i) ಸ್ಥಿರಾಂಕ
	{
		ಪ್ರತಿಜ್ಞಾ(i < M);

		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_M1 &v = *ಇದು;
		ಹಿಂತಿರುಗಿ v(i, 0);
	}

	ಸಾಲೊಳಗೆ Type &ಕ್ರಿಯಾಕಾರಕ()(size_t i)
	{
		ಪ್ರತಿಜ್ಞಾ(i < M);

		ಮಾತೃಕೆ_M1 &v = *ಇದು;
		ಹಿಂತಿರುಗಿ v(i, 0);
	}

	Type dot(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_M1 &b) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ಥಿರಾಂಕ ಸದಿಶ &a(*ಇದು);
		Type r(0);

		ಗಾಗಿ (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		ಹಿಂತಿರುಗಿ r;
	}

	ಸಾಲೊಳಗೆ Type ಕ್ರಿಯಾಕಾರಕ*(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_M1 &b) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ಥಿರಾಂಕ ಸದಿಶ &a(*ಇದು);
		ಹಿಂತಿರುಗಿ a.dot(b);
	}

	ಸಾಲೊಳಗೆ ಸದಿಶ ಕ್ರಿಯಾಕಾರಕ*(Type b) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ ಸದಿಶ(ಮಾತೃಕೆ_M1::ಕ್ರಿಯಾಕಾರಕ*(b));
	}

	Type norm() ಸ್ಥಿರಾಂಕ
	{
		ಸ್ಥಿರಾಂಕ ಸದಿಶ &a(*ಇದು);
		ಹಿಂತಿರುಗಿ Type(std::ವರ್ಗಮೂಲ(a.dot(a)));
	}

	Type norm_squared() ಸ್ಥಿರಾಂಕ
	{
		ಸ್ಥಿರಾಂಕ ಸದಿಶ &a(*ಇದು);
		ಹಿಂತಿರುಗಿ a.dot(a);
	}

	ಸಾಲೊಳಗೆ Type ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ norm();
	}

	ಸಾಲೊಳಗೆ ಶೂನ್ಯ normalize()
	{
		(*ಇದು) /= norm();
	}

	ಸದಿಶ unit() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ (*ಇದು) / norm();
	}

	ಸದಿಶ unit_or_zero(ಸ್ಥಿರಾಂಕ Type eps = Type(1e-5)) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ಥಿರಾಂಕ Type n = norm();

		ಆದರೆ (n > eps) {
			ಹಿಂತಿರುಗಿ (*ಇದು) / n;
		}

		ಹಿಂತಿರುಗಿ ಸದಿಶ();
	}

	ಸಾಲೊಳಗೆ ಸದಿಶ normalized() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ unit();
	}

	ತಾರ್ಕಿಕ longerThan(Type testVal) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ norm_squared() > testVal * testVal;
	}

	ಸದಿಶ ವರ್ಗಮೂಲ() ಸ್ಥಿರಾಂಕ
	{
		ಸ್ಥಿರಾಂಕ ಸದಿಶ &a(*ಇದು);
		ಸದಿಶ r;

		ಗಾಗಿ (size_t i = 0; i < M; i++) {
			r(i) = Type(std::ವರ್ಗಮೂಲ(a(i)));
		}

		ಹಿಂತಿರುಗಿ r;
	}

	ಶೂನ್ಯ print() ಸ್ಥಿರಾಂಕ
	{
		(*ಇದು).transpose().print();
	}

	ಸ್ಥಿರ size_t ಗಾತ್ರ_2()
	{
		ಹಿಂತಿರುಗಿ M;
	}
};

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು OStream, ಬಗೆ_ಹೆಸರು Type, size_t M>
OStream &ಕ್ರಿಯಾಕಾರಕ<<(OStream &os, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ<Type, M> &ಸದಿಶ_2)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	constexpr size_t n = 15 * M * 1 + 1 + 1;
	ಅಕ್ಷರ string[n];
	ಸದಿಶ_2.transpose().write_string(string, n);
	os << string;
	ಹಿಂತಿರುಗಿ os;
}

} // namespace matrix
