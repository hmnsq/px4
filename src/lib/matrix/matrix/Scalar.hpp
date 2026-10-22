/**
 * @file Scalar.hpp
 *
 * Defines conversion of matrix to scalar.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "Matrix.hpp"

ನಾಮಾಕಾಶ ಮಾತೃಕೆ
{

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು Type>
ವರ್ಗ Scalar
{
ಸಾರ್ವಜನಿಕ:
	Scalar() = ಅಳಿಸು;

	Scalar(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<Type, 1, 1> &other) :
		ಮೌಲ್ಯ_2{other(0, 0)}
	{
	}

	Scalar(Type other) : ಮೌಲ್ಯ_2(other)
	{
	}

	ಕ್ರಿಯಾಕಾರಕ ಸ್ಥಿರಾಂಕ Type &()
	{
		ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ_2;
	}

	ಕ್ರಿಯಾಕಾರಕ ಮಾತೃಕೆ_2<Type, 1, 1>() ಸ್ಥಿರಾಂಕ
	{
		ಮಾತೃಕೆ_2<Type, 1, 1> m;
		m(0, 0) = ಮೌಲ್ಯ_2;
		ಹಿಂತಿರುಗಿ m;
	}

	ಕ್ರಿಯಾಕಾರಕ ಸದಿಶ<Type, 1>() ಸ್ಥಿರಾಂಕ
	{
		ಸದಿಶ<Type, 1> m;
		m(0) = ಮೌಲ್ಯ_2;
		ಹಿಂತಿರುಗಿ m;
	}

	ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ isEqualF(ಮೌಲ್ಯ_2, other);
	}

ಖಾಸಗಿ:
	ಸ್ಥಿರಾಂಕ Type ಮೌಲ್ಯ_2;

};

ಬಳಸಿ Scalarf = Scalar<ದಶಮಾಂಶ>;
ಬಳಸಿ Scalard = Scalar<ದ್ವಿಗುಣ>;

} // namespace matrix
