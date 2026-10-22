/**
 * @file Vector2.hpp
 *
 * 2D vector class.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "Vector.hpp"

ನಾಮಾಕಾಶ ಮಾತೃಕೆ
{

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು Type>
ವರ್ಗ Vector2 : ಸಾರ್ವಜನಿಕ ಸದಿಶ<Type, 2>
{
ಸಾರ್ವಜನಿಕ:

	ಬಳಸಿ Matrix21 = ಮಾತೃಕೆ_2<Type, 2, 1>;
	ಬಳಸಿ Vector3 = ಸದಿಶ<Type, 3>;

	Vector2() = ಪೂರ್ವನಿಯೋಜಿತ;

	Vector2(ಸ್ಥಿರಾಂಕ Matrix21 &other) :
		ಸದಿಶ<Type, 2>(other)
	{
	}

	ಸ್ಪಷ್ಟ Vector2(ಸ್ಥಿರಾಂಕ Type ದತ್ತಾಂಶ_2[2]) :
		ಸದಿಶ<Type, 2>(ದತ್ತಾಂಶ_2)
	{
	}

	Vector2(Type x, Type y)
	{
		Vector2 &v(*ಇದು);
		v(0) = x;
		v(1) = y;
	}

	ಬಳಸಿ base = ಸದಿಶ<Type, 2>;
	ಬಳಸಿ base::base;

	ಸ್ಪಷ್ಟ Vector2(ಸ್ಥಿರಾಂಕ Vector3 &other)
	{
		Vector2 &v(*ಇದು);
		v(0) = other(0);
		v(1) = other(1);
	}

	Type cross(ಸ್ಥಿರಾಂಕ Matrix21 &b) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ಥಿರಾಂಕ Vector2 &a(*ಇದು);
		ಹಿಂತಿರುಗಿ a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	Vector2 ಕ್ರಿಯಾಕಾರಕ+(Vector2 other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix21::ಕ್ರಿಯಾಕಾರಕ+(other);
	}

	Vector2 ಕ್ರಿಯಾಕಾರಕ+(Type scalar) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix21::ಕ್ರಿಯಾಕಾರಕ+(scalar);
	}

	Vector2 ಕ್ರಿಯಾಕಾರಕ-(Vector2 other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix21::ಕ್ರಿಯಾಕಾರಕ-(other);
	}

	Vector2 ಕ್ರಿಯಾಕಾರಕ-(Type scalar) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix21::ಕ್ರಿಯಾಕಾರಕ-(scalar);
	}

	Vector2 ಕ್ರಿಯಾಕಾರಕ-() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix21::ಕ್ರಿಯಾಕಾರಕ-();
	}

	Vector2 ಕ್ರಿಯಾಕಾರಕ*(Type scalar) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Matrix21::ಕ್ರಿಯಾಕಾರಕ*(scalar);
	}

	Type ಕ್ರಿಯಾಕಾರಕ*(Vector2 b) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ ಸದಿಶ<Type, 2>::ಕ್ರಿಯಾಕಾರಕ*(b);
	}

	Type ಕ್ರಿಯಾಕಾರಕ%(ಸ್ಥಿರಾಂಕ Matrix21 &b) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ (*ಇದು).cross(b);
	}

};

ಬಳಸಿ Vector2f = Vector2<ದಶಮಾಂಶ>;
ಬಳಸಿ Vector2d = Vector2<ದ್ವಿಗುಣ>;

} // namespace matrix
